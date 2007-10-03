/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "fahmon.h"
#include "client.h"

#include "tools.h"
#include "pathManager.h"
#include "wx/textfile.h"
#include "fahlogAnalyzer.h"
#include "messagesManager.h"
#include "projectsManager.h"
#include "messagesManager.h"
#include "benchmarksManager.h"
#include "preferencesManager.h"
#include "clientsManager.h"
#include "mainDialog.h"
#include "httpDownloader.h"
#include "eta.h"


// This mutex is used to ensure that two threads won't try to overwite the same file at the same time
wxMutex Client::mMutexXYZFiles;


/**
 * Constructor
**/
Client::Client(const wxString& name, const wxString& location)
{
    mLastModification          = 0;
    mPreviouslyAnalyzedFrameId = MAX_FRAME_ID;

    SetName(name);
    SetLocation(location);

    Reset();
}


/**
 * Destructor
**/
Client::~Client(void)
{
}


/**
 * Change the location of the client
**/
void Client::SetLocation(const wxString& location)
{
    mLocation = location;

    // Avoid the use of backslashes
    mLocation.Replace(wxT("\\"), wxT("/"));

    // Ensure that the location ends with a slash
    if(mLocation.Last() != '/')
        mLocation += '/';
}


/**
 * Restore the initial state of the client: no information loaded
**/
void Client::Reset(void)
{
    mLog            = _("Log not loaded!");
    mState          = ST_INACCESSIBLE;
    mProgress       = 0;
    mUserName       = _("anonymous");
    mProjectId      = INVALID_PROJECT_ID;
    mTeamNumber     = 0;
    mDownloadDate   = wxInvalidDateTime;
    mProjectString  = wxT("");
    mProgressString = _("N/A");
    mPPD = 0;

    mETA.Invalidate();
}


/**
 * Return true if some modification occurred since the last reload
**/
bool Client::ReloadNeeded(void) const
{
    return wxFileModificationTime(mLocation + wxT("FAHlog.txt")) != mLastModification;
}


/**
 * (Re)Load this client
**/
void Client::Reload(void)
{
    bool              collectXYZFiles;
    bool              overrideTZ;
    WorkUnitFrame    *lastFrame;
    wxUint32	      nbBenchmarks;
    wxUint32	      i;
    wxUint32          ppdDisplay;
    //wxInt32           portPosition;
    //wxInt32           fslashPosition;
    wxInt32           TZ;
    float             tempFloat;
    wxString	      PPD;
    wxString	      clientLocation;
    wxString          templocation;
    wxString	      clientName;
    wxString          clientServer;
    wxString          clientPort;
    wxString          clientResource;
    wxString          tempString;
    wxDateTime        timeNow;
    wxDateTime        downloadTime;
    wxTimeSpan        timeDiff;
    const Project    *project;
    const Benchmark **benchmarks;


    _PrefsGetBool(PREF_OVERRIDE_TIMEZONE, overrideTZ);
    _PrefsGetInt (PREF_TZ,                TZ);

    templocation = mLocation;
    /* Preparatory code for monitoring of web clients
    HTTPDownloader::DownloadStatus downloadStatus;

    if(templocation.Mid(0, 4) == "http")
    {
        _LogMsgInfo(wxString::Format(wxT("Web client found at: %s"), templocation.c_str()));
        tempString = templocation.Mid(7, templocation.Length() - 7);
        portPosition = tempString.Find(':');
        fslashPosition = tempString.Find('/');
        if(portPosition == -1)
        {
            clientPort = "80";
            clientServer = tempString.Mid(0, fslashPosition);
        }
        else
        {
            clientPort = tempString.Mid(portPosition+1, fslashPosition-portPosition-1);
            clientServer = tempString.Mid(0, portPosition);
        }
        clientResource = tempString.Mid(fslashPosition +1 , tempString.Length() - fslashPosition);
        // now we have all the settings to download files over http
        templocation = "./web_temp/";
    }*/

    mLastModification = wxFileModificationTime(templocation + wxT("FAHlog.txt"));
    _LogMsgInfo(wxString::Format(_("Reloading %s"), mName.c_str()));

    Reset();

    // The directory must exist, otherwise we can't do anything
    // If it doesn't exist, display an explicit message in the log part of the window
    if(!wxDirExists(templocation))
    {
        mLog = wxString::Format(_("Directory %s does not exist or cannot be read!"), templocation.c_str());
        _LogMsgError(mLog);
        return;
    }
    else
        mState = ST_STOPPED;

    // Try to load the log file of the client
    if(!LoadLogFile(templocation + wxT("FAHlog.txt")))
    {
        mLog = wxString::Format(_("Error while reading %sFAHlog.txt!"), templocation.c_str());
        _LogMsgError(mLog);
        mState = ST_INACCESSIBLE;
        return;
    }

    // Try to load the 'unitinfo.txt' file generated by the client
    if(!LoadUnitInfoFile(templocation + wxT("unitinfo.txt")))
        _LogMsgError(wxString::Format(_("Error while reading %sunitinfo.txt!"), templocation.c_str()));

    // Try to load the configuration file 'client.cfg'
    if(!LoadClientCfg(templocation + wxT("client.cfg")))
        _LogMsgError(wxString::Format(_("Error while reading %sclient.cfg!"), templocation.c_str()));

    // Extract the duration of the last frame
    lastFrame = FahLogAnalyzer::AnalyzeLastFrame(mLog);

    // Should we collect .xyz files?
    _PrefsGetBool(PREF_FAHCLIENT_COLLECTXYZFILES, collectXYZFiles);
    // lastFrame->GetId() is needed to make sure we dont save current.xyz file from previous WU
    if(collectXYZFiles == true && mProjectId != INVALID_PROJECT_ID && lastFrame != NULL && lastFrame->GetId() > 0)
        SaveXYZFile();

    // Add this duration to the benchmarks for valid projects, but don't store the same frame twice
    if(mProjectId != INVALID_PROJECT_ID && lastFrame != NULL && !lastFrame->ClientIsStopped() && lastFrame->GetId() != mPreviouslyAnalyzedFrameId)
    {
        mPreviouslyAnalyzedFrameId = lastFrame->GetId();
        // Calculate effective frame time
        timeNow = wxDateTime::Now();
        if(overrideTZ)
        {
            downloadTime = mDownloadDate.Add(wxTimeSpan::Hours(TZ));
        }
        else
        {
            downloadTime = mDownloadDate.FromTimezone(wxDateTime::UTC);
        }
        // There may be a cleaner way of doing this
        timeDiff = timeNow.Subtract(downloadTime);
        tempFloat = timeDiff.GetMinutes() * 60 / lastFrame->GetId();
        lastFrame->SetEffectiveDuration(wxUint32(tempFloat));
        BenchmarksManager::GetInstance()->Add(mProjectId, this, lastFrame);
    }


    // The state can be either running, stopped...
    FindCurrentState(lastFrame);

    _PrefsGetUint(PREF_PPD_DISPLAYSTYLE, ppdDisplay);

    // Default PPD is always 0
    mPPD = 0;

    // If current project is valid and is found in the benchmarks database, then grab the PPD
    // Needs to check state too, no point getting PPD for stopped or dead clients.
    if (mProjectId != INVALID_PROJECT_ID && mState != ST_STOPPED && mState != ST_INACCESSIBLE && mState != ST_HUNG)
    {
        project = ProjectsManager::GetInstance()->GetProject(mProjectId);
        if (project != INVALID_PROJECT_ID)
        {
            benchmarks = BenchmarksManager::GetInstance()->GetBenchmarksList(mProjectId, nbBenchmarks);
            if (nbBenchmarks != 0)
            {
                for(i=0; i<nbBenchmarks; ++i)
                {
                    clientLocation = BenchmarksManager::GetInstance()->GetClientLocationFromClientId(benchmarks[i]->GetClientId());
                    clientName     = ClientsManager::GetInstance()->GetNameFromLocation(clientLocation);
                    if (clientName == mName)
                    {
                        switch(ppdDisplay)
                        {
                            // ---
                            case PPDDS_ALL_FRAMES:
                                mPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetAvgDuration() * (double)project->GetNbFrames());
                                break;

                            // ---
                            case PPDDS_LAST_FRAME:
                                if((double)benchmarks[i]->GetInstantDuration() != 0)
                                {
                                    mPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetInstantDuration() * (double)project->GetNbFrames());
                                }
                                else
                                {
                                    //best guess based on avg frame time
                                    mPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetAvgDuration() * (double)project->GetNbFrames());
                                }
                                break;

                            // ---
                            case PPDDS_THREE_FRAMES:
                                if((double)benchmarks[i]->Get3FrameDuration() != 0)
                                {
                                    mPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->Get3FrameDuration() * (double)project->GetNbFrames());
                                }
                                else
                                {
                                    //best guess based on avg frame time
                                    mPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetAvgDuration() * (double)project->GetNbFrames());
                                }
                                break;

                            case PPDDS_EFFECTIVE_FRAMES:
                                if((double)benchmarks[i]->GetEffectiveDuration() != 0)
                                {
                                    mPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetEffectiveDuration() * (double)project->GetNbFrames());
                                }
                                else
                                {
                                    //best guess based on avg frame time
                                    mPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetAvgDuration() * (double)project->GetNbFrames());
                                }
                                break;

                            // We shall never fall here
                            default:
                                wxASSERT(false);
                                break;
                        };
                    }
                }
            }
        }
    }

    // Compute ETA
    ComputeETA(lastFrame);

    // We don't need this anymore
    if(lastFrame != NULL)
        delete lastFrame;
}


/**
 * Load the file FAHlog.txt
 * Line returns are not converted, as it seems they are correctly taken into account by the text component used
**/
inline bool Client::LoadLogFile(const wxString& filename)
{
    if(wxFileExists(filename) == false || Tools::LoadFile(filename, mLog) == false)
        return false;

    return true;
}


/**
 * Load information from the unitinfo.txt file:
 *  - Project number
 *  - Download date
 *  - Current progress
**/
inline bool Client::LoadUnitInfoFile(const wxString& filename)
{
    bool              projectOk, downloadDateOk, progressOk;
    wxInt32           startingPos, endingPos;
    wxUint32          i;
    wxString          currentLine;
    wxString          progressString, downloadDateString;
    wxTextFile        in;
    unsigned long     tmpLong;
    wxDateTime::Month theMonth;

    // Try to open the file, check if it exists
    if(!wxFileExists(filename) || !in.Open(filename))
        return false;

    // We need to find all these elements, otherwise the file is not correct
    projectOk      = false;
    progressOk     = false;
    downloadDateOk = false;

    // Retrieve each line and try to extract the needed elements
    for(i=0; i<in.GetLineCount(); ++i)
    {
        currentLine = in.GetLine(i);

        // Name: p890_p53dimer890
       /**if(currentLine.StartsWith(wxT("Name: "), &mProjectString))
      * {
	*     startingPos = mProjectString.Find('p');
	*    endingPos   = mProjectString.Find('_');
	*
        *    if(startingPos != -1 && endingPos != -1 && endingPos > startingPos)
        *    {
        *        if(mProjectString.Mid(startingPos+1, endingPos-startingPos-1).ToULong(&tmpLong) == true)
        *        {
        *            projectOk  = true;
        *            mProjectId = (wxUint32)tmpLong;
        *        }
        *    }
        * }
       *
        else**/
	if(currentLine.StartsWith(wxT("Download time: "), &downloadDateString))
        {
            // We cannot rely on ParseFormat() function to parse the month name because of locales
            // Indeed, the month name is written in english and english language can be unavailable on some non-english systems
            // So we must parse the month name 'by hand'

            // Some f@h cores write "Feburary" instead of "February", so we must be careful
            if(downloadDateString.StartsWith(wxT("January "),   &downloadDateString))    theMonth = wxDateTime::Jan;
            else if(downloadDateString.StartsWith(wxT("February "),  &downloadDateString))    theMonth = wxDateTime::Feb;
            else if(downloadDateString.StartsWith(wxT("Feburary "),  &downloadDateString))    theMonth = wxDateTime::Feb;
            else if(downloadDateString.StartsWith(wxT("March "),     &downloadDateString))    theMonth = wxDateTime::Mar;
            else if(downloadDateString.StartsWith(wxT("April "),     &downloadDateString))    theMonth = wxDateTime::Apr;
            else if(downloadDateString.StartsWith(wxT("May "),       &downloadDateString))    theMonth = wxDateTime::May;
            else if(downloadDateString.StartsWith(wxT("June "),      &downloadDateString))    theMonth = wxDateTime::Jun;
            else if(downloadDateString.StartsWith(wxT("July "),      &downloadDateString))    theMonth = wxDateTime::Jul;
            else if(downloadDateString.StartsWith(wxT("August "),    &downloadDateString))    theMonth = wxDateTime::Aug;
            else if(downloadDateString.StartsWith(wxT("September "), &downloadDateString))    theMonth = wxDateTime::Sep;
            else if(downloadDateString.StartsWith(wxT("October "),   &downloadDateString))    theMonth = wxDateTime::Oct;
            else if(downloadDateString.StartsWith(wxT("November "),  &downloadDateString))    theMonth = wxDateTime::Nov;
            else if(downloadDateString.StartsWith(wxT("December "),  &downloadDateString))    theMonth = wxDateTime::Dec;
            else                                                                              theMonth = wxDateTime::Inv_Month;

            if(theMonth != wxDateTime::Inv_Month && mDownloadDate.ParseFormat(downloadDateString.c_str(), wxT("%d %H:%M")) != NULL)
            {
                // ParseFormat() does not set the day of the month if a month was not given within the string to parse
                // We can't even give it an arbitrary month, because it could fail due to locales (see above comment)
                // So we need to parse the day ourselves (sic)
                endingPos = downloadDateString.Find(' ');
                if(endingPos != -1 && downloadDateString.Left(endingPos).ToULong(&tmpLong) == true)
                {
                    mDownloadDate.SetMonth(theMonth);
                    mDownloadDate.SetDay((wxDateTime::wxDateTime_t)tmpLong);
                    downloadDateOk = true;
                }
            }
        }
        // Progress: 2%  [__________]
        else if(currentLine.StartsWith(wxT("Progress: "), &mProgressString))
        {
            endingPos = mProgressString.Find('%');

            if(endingPos != -1)
            {
                mProgressString = mProgressString.Mid(0, endingPos);
                if(mProgressString.ToULong(&tmpLong) == true)
                {
                    mProgress        = (wxUint32)tmpLong;
                    mProgressString += wxT("%");
                    progressOk       = true;
                }
            }
        }
    }

    in.Close();

    // some hacked together code to try and get project name from fahlog (a very bad way of doing it)
    // we should really be using the queue.dat file for this
    if(!wxFileExists(mLocation + wxT("FAHlog.txt")) || !in.Open(mLocation + wxT("FAHlog.txt")))
    {
        _LogMsgWarning(_("Couldn't Open FAHlog.txt"));
        return false;
    }
    i = in.GetLineCount()-1; // if log is blank, i becomes 2^32
    if (in.GetLineCount() > 0) // sanity checking for empty log files
    {
        while( i > 0)
        {
            currentLine = in.GetLine(i);
            currentLine = currentLine.Mid(11);
            if(currentLine.StartsWith(wxT("Project: "), &mProjectString) || currentLine.StartsWith(wxT("Protein: "), &mProjectString))
            {
                endingPos = mProjectString.Find('(');
                if (endingPos !=-1)
                {
                    mProjectString = mProjectString.Mid(0, endingPos-1);
                    if(mProjectString.ToULong(&tmpLong) == true)
                    {
                        mProjectId = (wxUint32)tmpLong;
                        projectOk = true;
                        // Now to get the rcg values
                        // First we reset them just in case they are not readable (like in some SMP logs)
                        mRun = -1;
                        mClone = -1;
                        mGen = -1;
                        startingPos = currentLine.Find(wxT("Run "));
                        endingPos = currentLine.Find(wxT(", Clone "));
                        if (currentLine.Mid(startingPos+4, (endingPos)-(startingPos+4)).ToULong(&tmpLong) == true)
                            mRun = (wxInt16)tmpLong;
                        startingPos = currentLine.Find(wxT("Clone "));
                        endingPos = currentLine.Find(wxT(", Gen "));
                        if (currentLine.Mid(startingPos+6, (endingPos)-(startingPos+6)).ToULong(&tmpLong) == true)
                            mClone = (wxInt16)tmpLong;
                        startingPos = currentLine.Find(wxT("Gen "));
                        endingPos = currentLine.Find(wxT(")"));
                        if (currentLine.Mid(startingPos+4, (endingPos)-(startingPos+4)).ToULong(&tmpLong) == true)
                            mGen = (wxInt16)tmpLong;
                        _LogMsgInfo(wxString::Format(_("PRCG found for %s from FAHlog.txt: %u/%i/%i/%i"), mName.c_str(), mProjectId, mRun, mClone, mGen));
                        break;
                     }
                }
            }
            i--;
         }
    }
    in.Close();


    // We succeeded only if we found these three elements
    if(projectOk && downloadDateOk && progressOk)
        return true;

    // Log any error encountered
    if(!progressOk)
        _LogMsgWarning(wxString::Format(_("The progress value in file %s could not be found/parsed"), filename.c_str()));

    if(!projectOk)
        _LogMsgWarning(wxString::Format(_("No valid project number was found for %s in FAHlog.txt"), mName.c_str()));

    if(!downloadDateOk)
    {
        mDownloadDate = wxInvalidDateTime;
        _LogMsgWarning(wxString::Format(_("The download date in file %s could not be parsed"), filename.c_str()));
    }

    return false;
}


/**
 * Load information from the client.cfg file:
 *  - username
 *  - team number
**/
inline bool Client::LoadClientCfg(const wxString& filename)
{
    bool          userNameOk;
    bool          teamNumberOk;
    wxUint32      i;
    wxString      currentLine;
    wxString      teamNumberString;
    wxTextFile    in;
    unsigned long tmpLong;

    // Try to open the file, check if it exists
    if(!wxFileExists(filename) || !in.Open(filename))
        return false;

    // We need to find all these elements, otherwise the file is not correct
    userNameOk   = false;
    teamNumberOk = false;

    // Retrieve each line and try to extract the needed elements
    for(i=0; i<in.GetLineCount() && (!userNameOk || !teamNumberOk); ++i)
    {
        currentLine = in.GetLine(i);

        // username=anonymous
        if(currentLine.StartsWith(wxT("username="), &mUserName))
        {
            userNameOk = true;
        }
        // team=0
        else if(currentLine.StartsWith(wxT("team="), &teamNumberString))
        {
            if(teamNumberString.ToULong(&tmpLong) == true)
            {
                mTeamNumber  = (wxUint32)tmpLong;
                teamNumberOk = true;
            }
        }
    }

    in.Close();

    // We succeeded only if we found these two elements
    if(userNameOk && teamNumberOk)
        return true;

    // Log any error encountered
    if(!userNameOk)
        _LogMsgWarning(wxString::Format(_("No valid username were found in file %s"), filename.c_str()));

    if(!teamNumberOk)
        _LogMsgWarning(wxString::Format(_("No valid team number were found in file %s"), filename.c_str()));

    return false;
}


/**
 * Save the file 'current.xyz' to the appropriate directory with the approppriate name
 * This method is thread-safe, two files won't be overwritten at the same time by two different threads
**/
inline void Client::SaveXYZFile(void) const
{
    // ----- Access Lock -----
    wxMutexLocker mutexLocker(mMutexXYZFiles);
    // ----- Access Lock -----


    wxString xyzInFile;
    wxString xyzOutFile;

    // Create the complete path of the files
    xyzInFile  = mLocation + wxT("work/current.xyz");
    xyzOutFile = PathManager::GetXYZPath() + mProjectString + wxT(".xyz");

    // Check that the file to save exists
    if(!wxFileExists(xyzInFile))
    {
        _LogMsgWarning(wxString::Format(_("Unable to save %s because it does not exist"), xyzInFile.c_str()));
        return;
    }

    // Create the ouput directory if needed
    if(!wxDirExists(PathManager::GetXYZPath()) && !wxMkdir(PathManager::GetXYZPath(), 0777))
    {
        _LogMsgWarning(wxString::Format(_("Unable to create directory %s"), PathManager::GetXYZPath().c_str()));
        return;
    }

    // Check that the file does not already exist
    if(wxFileExists(xyzOutFile))
        return;

    // Ok, we can finally copy the file
    if(!wxCopyFile(xyzInFile, xyzOutFile))
         _LogMsgWarning(wxString::Format(_("Unable to copy %s to %s"), xyzInFile.c_str(), xyzOutFile.c_str()));
}


/**
 * Use registered benchmarks and the duration of the last frame to compute an ETA, if this is possible
**/
void Client::ComputeETA(WorkUnitFrame* lastFrame)
{
    wxUint32         totalFrames;
    wxUint32         referenceDuration;
    wxUint32         nbLeftSeconds;
    wxString         logLine;
    wxUint32         ppdDisplay;
    const Project   *projectInfo;
    const Benchmark *benchmark;

    // If we don't have a valid project id, there is no ETA
    if(mProjectId == INVALID_PROJECT_ID)
        return;

    // If the client is stopped, there is no ETA
    if(lastFrame && lastFrame->ClientIsStopped())
        return;

    logLine = wxString::Format(_T("%s [ETA]"), mName.c_str());
    if(lastFrame != NULL)
    {
        _LogMsgInfo(wxString::Format(_("%s is on frame %u"), mName.c_str(), lastFrame->GetId()));
    }
    else
    {
        _LogMsgInfo(wxString::Format(_("Cannot determine frame number for %s (this isn't a problem)"), mName.c_str()));
    }

    // --- 1) Retrieve the total number of frames for the current project, or try to guess it if possible
    projectInfo = ProjectsManager::GetInstance()->GetProject(mProjectId);
    if(projectInfo != NULL)
        totalFrames = projectInfo->GetNbFrames();
    else if(lastFrame != NULL && mProgress != 0)
    {
        // Compute a simple cross product to guess the total number of frames
        totalFrames = lastFrame->GetId() * 100 / mProgress;
        totalFrames = totalFrames - (totalFrames % 20);
    }
    else
    {
        // If we really have no clue about the total number of frames, we cannot tell anything about the ETA
        return;
    }


    // This can't be equal to 0, as we need this value in further computations
    wxASSERT(totalFrames != 0);


    _PrefsGetUint(PREF_PPD_DISPLAYSTYLE, ppdDisplay);

    // --- 2) Compute a reference duration of a frame
    benchmark = BenchmarksManager::GetInstance()->GetBenchmark(mProjectId, this);
    if(benchmark != NULL && lastFrame != NULL)
    {
        /*
        logLine = logLine + wxString::Format(wxT(" | avg=%us | last=%us"), benchmark->GetAvgDuration(), lastFrame->GetDuration());

        // My theory about the computation of the ETA :-)
        //
        // As the computation of the WU is reaching the end, the duration of the last run will become more important.
        // However, when the computation has just begun, the average duration of a run is much more important than the 'current'
        // duration of runs.
        // So the duration used as a 'metric' is composed by the two durations, in regards of the progress of the computation.
        //
        //    Tm = Average run duration
        //    Tl = Duration of the last run
        //    Y  = Total number of runs for the WU
        //    X  = Number of the last run
        //
        //    Duration = (Tl * X / Y) + (Tm * (1 - X / Y))
        //             = Tl * X / Y + Tm - Tm * X / Y
        //             = (Tl - Tm) * X / Y + Tm

        wxInt32 Tm = benchmark->GetAvgDuration();
        wxInt32 Tl = lastFrame->GetDuration();
        wxInt32 Y  = totalFrames;
        wxInt32 X  = lastFrame->GetId();

        referenceDuration = (wxUint32)((Tl - Tm) * X / Y + Tm);

        // This causes problems with some WUs, for which the duration of a frame can vary a lot
        // In this case, the ETA varies a lot too, so for now we only use the average duration of a frame
        */

        /*
        Additional comment about above method: appears to be a "hybrid" system combining both "avg" and "cur" or "r3f" methods now
        implemented as ETA systems. Might be worth considering in future if it is possible to overcome the varying frame time
        problem
        */
        logLine = logLine + wxString::Format(wxT(" | avg=%s"), wxTimeSpan::Seconds(benchmark->GetAvgDuration()).Format(wxT("%H:%M:%S")).c_str());

        switch(ppdDisplay)
        {
            // ---
            case PPDDS_ALL_FRAMES:
                referenceDuration = benchmark->GetAvgDuration();
                break;

            // ---
            case PPDDS_LAST_FRAME:
                referenceDuration = benchmark->GetInstantDuration();
                break;

            // ---
            case PPDDS_THREE_FRAMES:
                referenceDuration = benchmark->Get3FrameDuration();
                break;

            // We shall never fall here
            default:
                wxASSERT(false);
                referenceDuration = benchmark->GetAvgDuration();
                break;
        };
    }
    else if(benchmark != NULL)
    {
        logLine = logLine + wxString::Format(wxT(" | avg=%s | last=N/A"), wxTimeSpan::Seconds(benchmark->GetAvgDuration()).Format(wxT("%H:%M:%S")).c_str());

        // No duration for the last frame, use the average duration
        referenceDuration = benchmark->GetAvgDuration();
    }
    else if(lastFrame != NULL)
    {
        logLine = logLine + wxString::Format(wxT(" | avg=N/A | last=%s"), wxTimeSpan::Seconds(lastFrame->GetDuration()).Format(wxT("%H:%M:%S")).c_str());

        // No average duration of a frame, use the duration of the last frame
        referenceDuration = lastFrame->GetDuration();
    }
    else
    {
        logLine = logLine + wxString::Format(wxT(" | avg=N/A | last=N/A"));
        _LogMsgInfo(logLine);

        // No indication on the average duration of a frame, nor on the duration of the last frame, so we cannot do anything
        return;
    }

    logLine = logLine + wxString::Format(wxT(" | ref=%s"), wxTimeSpan::Seconds(referenceDuration).Format(wxT("%H:%M:%S")).c_str());

    // --- 3) Compute the the left time
    if(lastFrame != NULL)
    {
        nbLeftSeconds = referenceDuration * (totalFrames - lastFrame->GetId());

        // Make a more accurate value by using the elapsed time since the end of the last frame
        if(lastFrame->GetElapsedSeconds() > referenceDuration)
        {
            nbLeftSeconds = nbLeftSeconds - referenceDuration;
            logLine = logLine + wxString::Format(wxT(" | adj=%s"), wxTimeSpan::Seconds(referenceDuration).Format(wxT("%H:%M:%S")).c_str());
        }
        else
        {
            nbLeftSeconds = nbLeftSeconds - lastFrame->GetElapsedSeconds();
            logLine = logLine + wxString::Format(wxT(" | adj=%s"), wxTimeSpan::Seconds(lastFrame->GetElapsedSeconds()).Format(wxT("%H:%M:%S")).c_str());
        }

        logLine = logLine + wxString::Format(wxT(" | left=%s"), wxTimeSpan::Seconds(nbLeftSeconds).Format(wxT("%H:%M:%S")).c_str());
    }
    else
    {
        // We don't have the identifier of the last computed frame, so we have to try to guess it once again
        wxUint32 lastComputedFrame = totalFrames * mProgress / 100;

        nbLeftSeconds = referenceDuration * (totalFrames - lastComputedFrame);

        logLine = logLine + wxString::Format(wxT(" | left=%us (guess)"), nbLeftSeconds);
    }

    // --- 4) That's it
    mETA.SetLeftTimeInMinutes(nbLeftSeconds/60);
    _LogMsgInfo(logLine);
}


/**
 * Try to determine if the client is running or not
**/
void Client::FindCurrentState(WorkUnitFrame* lastFrame)
{
    wxUint32         trigger;
    const Benchmark* benchmark;

    // If we don't have information about the last frame, we decide that the corresponding client is inactive
    if(lastFrame == NULL)
    {
        mState = ST_INACTIVE;
        _LogMsgInfo(wxString::Format(_("%s has an unknown state (Unable to find a complete frame)"), mName.c_str()));
        return;
    }


    // No ETA for stopped clients
    if(lastFrame->ClientIsStopped())
    {
        mState = ST_STOPPED;
        _LogMsgInfo(wxString::Format(_("%s is stopped (The line \"Folding@Home Client Shutdown.\" was found)"), mName.c_str()));
        return;
    }


    // No elapsed time since the last frame means we couldn't get valid information on elapsed time
    if(lastFrame->GetElapsedSeconds() == 0)
    {
        mState = ST_INACTIVE;
        _LogMsgInfo(wxString::Format(_("%s has an unknown state (Unable to extract a valid elapsed time since the last completed frame)"), mName.c_str()));
        return;
    }

    // Asynchronous client
    if(lastFrame->GetElapsedSeconds() == 65535)
    {
        mState = ST_ASYNCH;
        _LogMsgInfo(wxString::Format(_("%s has been marked as having a asynchronous clock."), mName.c_str()));
        return;
    }


    // Use benchmarked duration if available, otherwise use last frame duration
    // The limit is first fixed to three times the reference duration
    benchmark = BenchmarksManager::GetInstance()->GetBenchmark(mProjectId, this);
    if(benchmark != NULL)
        trigger = 3 * benchmark->GetAvgDuration();
    else
        trigger = 3 * lastFrame->GetDuration();

    // Raise the limit if it is too low
    // For example, if frames take 1mn to complete, a 2mn compilation process (for example) would immediately make the client
    // inactive, and we don't want to alert the user too early
    if(trigger < 45*60)
        trigger = 45*60;

    // Last step
    if(lastFrame->GetElapsedSeconds() < trigger)
        mState = ST_RUNNING;
    else
    {
        if(lastFrame->GetElapsedSeconds() > 2 * trigger)
        {
            mState = ST_HUNG;
            _LogMsgWarning(wxString::Format(_("%s seems to have hung : Elapsed time is %um and limit is %um"), mName.c_str(),   lastFrame->GetElapsedSeconds()/60, trigger/30));
        }
        else
        {
            mState = ST_INACTIVE;
            _LogMsgInfo(wxString::Format(_("%s seems to be inactive : Elapsed time is %um and limit is %um"), mName.c_str(),   lastFrame->GetElapsedSeconds()/60, trigger/60));
        }
    }
}


/**
 * Return the URL of the user's stats
**/
wxString Client::GetDonatorStatsURL(void) const
{
    return wxString::Format(wxT("%s&teamnum=%u&username=%s"), wxT(FMC_URL_MYSTATS), mTeamNumber, mUserName.c_str());
}


/**
 * Return the URL of the user's team stats
**/
wxString Client::GetTeamStatsURL(void) const
{
    return wxString::Format(wxT("%s&teamnum=%u"), wxT(FMC_URL_TEAMSTATS), mTeamNumber);
}


/**
 * Returns the URL of the project page on Jmol website
**/
wxString Client::GetJmolURL(void) const
{
    return wxString::Format(wxT("%s%u"), wxT(FMC_URL_JMOL), GetProjectId());
}


/**
 * Returns the URL of the project page on fahinfo.org
**/
wxString Client::GetFahinfoURL(void) const
{
    return wxString::Format(wxT("%s%u"), wxT(FMC_URL_FAHINFO), GetProjectId());
}
