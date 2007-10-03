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
#include "wx/textfile.h"
#include "fahlogAnalyzer.h"
#include "messagesManager.h"
#include "projectsManager.h"
#include "messagesManager.h"
#include "benchmarksManager.h"
#include "preferencesManager.h"


// This mutex is there to ensure that two threads won't try to overwite the same file at the same time
wxMutex Client::mMutexXYZFiles;


/**
 * Constructor
**/
Client::Client(const wxString& name, const wxString& location)
{
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
    mIsOk      = false;
    mIsRunning = true;

    mLog = wxT("Log not loaded!");

    mProjectId      = 0;
    mProjectString  = wxT("");
    mProgressString = wxT("");
    mProgress       = 0;

    mDownloadDate.SetToCurrent();

    mUserName   = wxT("anonymous");
    mTeamNumber = 0;
    
    mETA.Invalidate();
}


/**
 * (Re)Load this client
**/
void Client::Reload(void)
{
    bool           collectXYZFiles;
    WorkUnitFrame *lastFrame;
    
    _LogMsgInfo(wxString::Format(wxT("Reloading %s"), mName.c_str()));

    Reset();

    // 1) Test if the directory exists before anything else
    if(wxDirExists(mLocation) == false)
    {
        _LogMsgError(wxString::Format(wxT("Directory %s does not exist!"), mLocation.c_str()));
        mLog = wxT("Invalid directory!");
        return;
    }

    // 2) FAHlog.txt
    if(LoadLogFile(mLocation + wxT("FAHlog.txt")) == false)
    {
        _LogMsgError(wxString::Format(wxT("Error while reading %sFAHlog.txt!"), mLocation.c_str()));
        mLog = wxT("Error while reading <FAHlog.txt>!");
        return;
    }

    // 3) unitinfo.txt
    if(LoadUnitInfoFile(mLocation + wxT("unitinfo.txt")) == false)
    {
        _LogMsgError(wxString::Format(wxT("Error while reading %sunitinfo.txt!"), mLocation.c_str()));
        mLog = wxT("Error while reading <unitinfo.txt>!");
        return;
    }

    // 4) client.cfg
    if(LoadClientCfg(mLocation + wxT("client.cfg")) == false)
    {
        _LogMsgError(wxString::Format(wxT("Error while reading %sclient.cfg!"), mLocation.c_str()));
        mLog = wxT("Error while reading <client.cfg>!");
        return;
    }

    // So far, so good
    mIsOk = true;

    // 5) Should we collect .xyz files?
    _PrefsGetBool(PREF_FAHCLIENT_COLLECTXYZFILES, collectXYZFiles);
    if(collectXYZFiles == true)
        SaveXYZFile();

    // 6) Extract the duration of the last frame
    //     - Don't store it if it has been extrapolated, as the value can be very different from 'real' durations
    //     - Don't store the same frame twice, it would give wrong average durations
    lastFrame = FahLogAnalyzer::AnalyzeLastFrame(mLog);
    if(lastFrame != NULL && lastFrame->DurationIsExtrapolated() == false && lastFrame->GetId() != mPreviouslyAnalyzedFrameId)
    {
        mPreviouslyAnalyzedFrameId = lastFrame->GetId();
        BenchmarksManager::GetInstance()->Add(mProjectId, this, lastFrame);
    }

    // 7) Compute ETA
    // Fields are not valid when the client is stopped, in this case we don't give them to this method
    if(lastFrame != NULL && lastFrame->ClientIsStopped() == false)
        ComputeETA(lastFrame);
    else
        ComputeETA(NULL);

    // 8) Detect inactivity
    DetectInactivity(lastFrame);

    // We don't need this anymore
    if(lastFrame != NULL)
        delete lastFrame;
}


/**
 * Load the file FAHlog.txt, and remove trailing empty lines
 * Line returns are not converted, as it seems they are correctly take into account by the text component used
**/
inline bool Client::LoadLogFile(const wxString& filename)
{
    if(wxFileExists(filename) == false || Tools::LoadFile(filename, mLog) == false)
        return false;

    // Remove trailing empty lines
    while(mLog.Last() == '\n' || mLog.Last() == '\r')
        mLog.RemoveLast();

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
    wxTextFile    in(filename);
    wxString      currentLine;
    wxString      progressString, tmpString;
    wxInt32       startingPos, endingPos;
    wxUint32      i;
    bool          projectOk, downloadDateOk, progressOk;
    unsigned long tmpLong;

    // Try to open the file, check if it exists
    if(wxFileExists(filename) == false || in.Open(filename) == false)
        return false;

    // We need to find all these elements, otherwise the file is not correct
    projectOk      = false;
    downloadDateOk = false;
    progressOk     = false;
    
    // Retrieve each line and try to extract the needed elements
    for(i=0; i<in.GetLineCount(); ++i)
    {
        currentLine = in.GetLine(i);
 
        // Name: p890_p53dimer890
        if(currentLine.StartsWith(wxT("Name: "), &mProjectString) == true)
        {
            startingPos = mProjectString.Find('p');
            endingPos   = mProjectString.Find('_');

            if(startingPos != -1 && endingPos != -1 && endingPos > startingPos)
            {
                tmpString = mProjectString.Mid(startingPos+1, endingPos-startingPos-1);
                if(tmpString.ToULong(&tmpLong) == true)
                {
                    mProjectId = (wxUint32)tmpLong;
                    projectOk  = true;
                }
            }
            else
                _LogMsgWarning(wxString::Format(wxT("The string <%s> in file %s is not a valid project!"), mProjectString.c_str(), filename.c_str()));
        }
        // Download time: July 19 16:46:49
        else if(currentLine.StartsWith(wxT("Download time: "), &tmpString) == true)
        {
            // We cannot rely on ParseFormat() function to parse the month names because of locales
            // Indeed, the month name is written in english and english could be unavailable on some non-english systems
            // So we must parse the month name 'by hand'
            // The rest of the date is only composed of numbers, so ParseFormat() can be used for them
            wxDateTime::Month theMonth;

            // Some f@h cores write "Feburary" instead of "February", so we must be careful
            if(tmpString.StartsWith(wxT("January"), &tmpString) == true)
                theMonth = wxDateTime::Jan;
            else if(tmpString.StartsWith(wxT("February"), &tmpString) == true || tmpString.StartsWith(wxT("Feburary"), &tmpString) == true)
                theMonth = wxDateTime::Feb;
            else if(tmpString.StartsWith(wxT("March"), &tmpString) == true)
                theMonth = wxDateTime::Mar;
            else if(tmpString.StartsWith(wxT("April"), &tmpString) == true)
                theMonth = wxDateTime::Apr;
            else if(tmpString.StartsWith(wxT("May"), &tmpString) == true)
                theMonth = wxDateTime::May;
            else if(tmpString.StartsWith(wxT("June"), &tmpString) == true)
                theMonth = wxDateTime::Jun;
            else if(tmpString.StartsWith(wxT("July"), &tmpString) == true)
                theMonth = wxDateTime::Jul;
            else if(tmpString.StartsWith(wxT("August"), &tmpString) == true)
                theMonth = wxDateTime::Aug;
            else if(tmpString.StartsWith(wxT("September"), &tmpString) == true)
                theMonth = wxDateTime::Sep;
            else if(tmpString.StartsWith(wxT("October"), &tmpString) == true)
                theMonth = wxDateTime::Oct;
            else if(tmpString.StartsWith(wxT("November"), &tmpString) == true)
                theMonth = wxDateTime::Nov;
            else if(tmpString.StartsWith(wxT("December"), &tmpString) == true)
                theMonth = wxDateTime::Dec;
            else
                theMonth = wxDateTime::Inv_Month;

            if(theMonth != wxDateTime::Inv_Month && mDownloadDate.ParseFormat(tmpString.c_str(), wxT(" %d %H:%M:%S")) != NULL)
            {
                mDownloadDate.SetMonth(theMonth);
                downloadDateOk = true;
            }
            else
                _LogMsgWarning(wxString::Format(wxT("The download date in file %s could not be parsed"), filename.c_str()));
        }
        // Progress: 2%  [__________]
        else if(currentLine.StartsWith(wxT("Progress: "), &mProgressString) == true)
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
    return (projectOk == true && downloadDateOk == true && progressOk == true);
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
    wxTextFile    in(filename);
    unsigned long tmpLong;

    // Try to open the file, check if it exists
    if(wxFileExists(filename) == false || in.Open(filename) == false)
        return false;

    // We need to find all these elements, otherwise the file is not correct
    userNameOk   = false;
    teamNumberOk = false;
    
    // Retrieve each line and try to extract the needed elements
    for(i=0; i<in.GetLineCount() && (userNameOk == false || teamNumberOk == false); ++i)
    {
        currentLine = in.GetLine(i);

        // username=athropos_666
        if(currentLine.StartsWith(wxT("username="), &mUserName) == true)
        {
            userNameOk = true;
        }
        // team=51
        else if(currentLine.StartsWith(wxT("team="), &teamNumberString) == true)
        {
            if(teamNumberString.ToULong(&tmpLong) == true)
            {
                mTeamNumber  = (wxUint32)tmpLong;
                teamNumberOk = true;
            }
        }
    }
    
    return (userNameOk == true && teamNumberOk == true);
}


/**
 * Save the file 'current.xyz' to the appropriate directory with the approppriate name
 * This method is thread-safe, two files won't be overwritten at the same time by two different threads
**/
inline void Client::SaveXYZFile(void) const
{
    wxMutexLocker mutexLocker(mMutexXYZFiles);        // --- Lock the access to this method
    wxString      xyzInFile;
    wxString      xyzOutFile;
    
    // Create the complete path of the files to consider
    xyzInFile  = mLocation + wxT("work/current.xyz");
    xyzOutFile = wxT(FMC_DIR_XYZ) + mProjectString + wxT(".xyz");
    
    // Check that the file to save exists
    if(wxFileExists(xyzInFile) == false)
        return;

    // Create the ouput directory if needed
    if(wxDirExists(wxT(FMC_DIR_XYZ)) == false && wxMkdir(wxT(FMC_DIR_XYZ), 0777) == false)
        return;
    
    // Check that the file does not already exist
    if(wxFileExists(xyzOutFile) == true)
        return;

    // Ok, we can finally copy the file
    wxCopyFile(xyzInFile, xyzOutFile);
}


/**
 * Use registered benchmarks and the duration of the last frame to compute an ETA, if this is possible
 * The parameter can be NULL, if no information is available on the last frame duration
**/
void Client::ComputeETA(WorkUnitFrame* lastFrame)
{
    wxUint32         totalFrames;
    wxUint32         referenceDuration;
    wxUint32         nbLeftSeconds;
    wxString         logLine;
    const Project   *projectInfo;
    const Benchmark *benchmark;
    
    logLine = wxString::Format(wxT("%s [ETA]"), mName.c_str());
    
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
        */
        
        // This causes problems with some WUs, for which the duration of a frame can vary a lot
        // In this case, the ETA varies a lot too, so for now we only use the average duration of a frame
        
        logLine = logLine + wxString::Format(wxT(" | avg=%us"), benchmark->GetAvgDuration());
        
        referenceDuration = benchmark->GetAvgDuration();
    }
    else if(benchmark != NULL)
    {
        logLine = logLine + wxString::Format(wxT(" | avg=%us | last=N/A"), benchmark->GetAvgDuration());
        
        // No duration for the last frame, use the average duration
        referenceDuration = benchmark->GetAvgDuration();
    }
    else if(lastFrame != NULL)
    {
        logLine = logLine + wxString::Format(wxT(" | avg=N/A | last=%us"), lastFrame->GetDuration());
        
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
    
    logLine = logLine + wxString::Format(wxT(" | ref=%us"), referenceDuration);


    // --- 3) Compute the the left time
    if(lastFrame != NULL)
    {
        nbLeftSeconds = referenceDuration * (totalFrames - lastFrame->GetId());
        
        // Make a more accurate value by using the elapsed time since the end of the last frame
        if(lastFrame->GetElapsedSeconds() > referenceDuration)
        {
            nbLeftSeconds = nbLeftSeconds - referenceDuration;
            logLine = logLine + wxString::Format(wxT(" | adj=%us"), referenceDuration);
        }
        else
        {
            nbLeftSeconds = nbLeftSeconds - lastFrame->GetElapsedSeconds();
            logLine = logLine + wxString::Format(wxT(" | adj=%us"), lastFrame->GetElapsedSeconds());
        }
        
        logLine = logLine + wxString::Format(wxT(" | left=%us"), nbLeftSeconds);
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
void Client::DetectInactivity(WorkUnitFrame* lastFrame)
{
    wxUint32         trigger;
    wxString         triggerInfo;
    const Benchmark* benchmark;

    if(lastFrame == NULL)
    {
        // TODO
        // What can we do in this case?
    }
    else if(lastFrame->ClientIsStopped() == true)
    {
        mIsRunning = false;
        
        _LogMsgInfo(wxString::Format(wxT("%s is inactive (The line \"Folding@Home Client Shutdown.\" was found)"), mName.c_str()));

        // Don't go further, as we are sure that the client is not running
        return;
    }
    else
    {
        // Use two times the average duration as a trigger, if it is available
        // Otherwise, use two times the duration of the last frame
        
        benchmark = BenchmarksManager::GetInstance()->GetBenchmark(mProjectId, this);
        if(benchmark != NULL)
        {
            trigger = 2 * (wxUint32)benchmark->GetAvgDuration();

            triggerInfo = wxString::Format(wxT("limit is 2 * %us (average duration)"), benchmark->GetAvgDuration());
        }
        else
        {
            trigger = 2 * (wxUint32)lastFrame->GetDuration();
            
            triggerInfo = wxString::Format(wxT("limit is 2 * %us (last frame duration)"), lastFrame->GetDuration());
        }
        
        if(lastFrame->GetElapsedSeconds() > trigger)
        {
            mIsRunning = false;
            
            _LogMsgInfo(wxString::Format(wxT("%s is inactive : Elapsed time is %us and %s"), mName.c_str(), lastFrame->GetElapsedSeconds(), triggerInfo.c_str()));
        }
    }
}
