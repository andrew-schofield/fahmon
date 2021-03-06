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

/**
 * \file client.cpp
 * Creates a class for every client.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "client.h"

#include "tools.h"
#include "pathManager.h"
#include "fahlogAnalyzer.h"
#include "projectsManager.h"
#include "messagesManager.h"
#include "benchmarksManager.h"
#include "preferencesManager.h"
#include "clientsManager.h"
#include "mainDialog.h"
#include "httpDownloader.h"
#include "eta.h"
#include "queue.h"
#include "multiProtocolFile.h"

#include "wx/textfile.h"

// This mutex is used to ensure that two threads won't try to overwite the same file at the same time

wxMutex Client::mMutexReadWrite;

Client::Client(wxString const &name, wxString const &location, bool enabled, bool VM)
{
	mLastModification          = 0;
	mPreviouslyAnalyzedFrameId = MAX_FRAME_ID;

	SetName(name);
	SetLocation(location);
	SetIsFrameCountAccurate(false);
	mEnabled = enabled;
	SetVM(VM);
	mClientType = _("Unknown");
	mCoreVersion = 0;

	Reset();
}


Client::~Client(void)
{
}


void Client::SetLocation(const wxString& location)
{
	mLocation = location.c_str();

	// Ensure that the location ends with a slash
#ifdef _FAHMON_WIN32_
	if(multiProtocolFile::GetFileProtocol(location) != multiProtocolFile::HTTP && multiProtocolFile::GetFileProtocol(location) != multiProtocolFile::FTP)
	{
		mLocation.Replace(_T("/"), _T("\\"));
		if(mLocation.Last() != '\\')
			mLocation += '\\';
	}
	else
#endif
	{
		if(mLocation.Last() != '/')
			mLocation += '/';
	}
}


void Client::Reset(void)
{
	mLog            = _("Log not loaded!");
	SetState(ST_INACCESSIBLE);
	mProgress       = 0;
	mUserName       = _("anonymous");
	mProjectId      = INVALID_PROJECT_ID;
	mTeamNumber     = 0;
	mDownloadDate   = wxInvalidDateTime;
	mProjectString  = _T("");
	mProgressString = _("N/A");
	mPPD            = 0;
	mCore           = _T("");
	mCredit         = 0;
	mDeadlineDate   = wxInvalidDateTime;
	mClientType     = _("Unknown");
	mRun            = 0;
	mClone          = 0;
	mGen            = 0;

	mETA.Invalidate();
}


bool Client::ReloadNeeded(void) const
{
	if (multiProtocolFile::FileExists(mLocation + _T("FAHlog.txt")))
		return multiProtocolFile::LastModification(mLocation + _T("FAHlog.txt")) != mLastModification;
	else
		return false;
}


void Client::Reload(void)
{
	WorkUnitFrame    *lastFrame;
	wxUint32	      nbBenchmarks;
	wxUint32	      i;
	wxUint32          ppdDisplay;
	float             tempFloat;
	wxString	      PPD;
	wxString	      clientLocation;
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
	wxUint32          frameCount;
	wxUint32          frameTime;

	// The directory must exist, otherwise we can't do anything
	// If it doesn't exist, display an explicit message in the log part of the window
	if(!multiProtocolFile::DirExists(mLocation.c_str()))
	{
		mLog = wxString::Format(_("Directory %s does not exist or cannot be read!"), mLocation.c_str());
		SetState(ST_INACCESSIBLE);
		_LogMsgError(mLog);
		return;
	}

	if (multiProtocolFile::FileExists(wxString::Format(wxT("%sFAHlog.txt"),mLocation.c_str())))
	{
		mLastModification = multiProtocolFile::LastModification(wxString::Format(wxT("%sFAHlog.txt"),mLocation.c_str()));
	}
	else
	{
		SetState(ST_INACCESSIBLE);
		_LogMsgError(wxString::Format(_("%s cannot be reloaded! (FAHlog.txt does not exist)"), mName.c_str()));
		return;
	}
	_LogMsgInfo(wxString::Format(_("Reloading %s"), mName.c_str()), false);

	Reset();

	// Try to load the log file of the client
	if(!LoadLogFile(wxString::Format(wxT("%sFAHlog.txt"),mLocation.c_str())))
	{
		mLog = wxString::Format(_("Error while reading %sFAHlog.txt!"), mLocation.c_str());
		_LogMsgError(mLog);
		SetState(ST_INACCESSIBLE);
		return;
	}

	if(!LoadQueueFile(wxString::Format(wxT("%squeue.dat"),mLocation.c_str())))
	{
		_LogMsgError(wxString::Format(_("Error while reading %squeue.dat"), mLocation.c_str()));
	}
	wxString logFilePath;
#ifdef _FAHMON_WIN32_
	if(multiProtocolFile::GetFileProtocol(mLocation) != multiProtocolFile::HTTP && multiProtocolFile::GetFileProtocol(mLocation) != multiProtocolFile::FTP)
		logFilePath = wxString::Format(_T("%swork\\logfile_%02i.txt"), mLocation.c_str(), mUnitIndex);
	else
#endif
		logFilePath = wxString::Format(_T("%swork/logfile_%02i.txt"), mLocation.c_str(), mUnitIndex);
	if(!LoadWULogFile(logFilePath))
	{
		_LogMsgError(wxString::Format(_("Error while reading %s"), logFilePath.c_str()));
	}

	// Try to load the 'unitinfo.txt' file generated by the client
	if(!LoadUnitInfoFile(wxString::Format(wxT("%sunitinfo.txt"),mLocation.c_str())))
	{
		_LogMsgError(wxString::Format(_("Error while reading %sunitinfo.txt!"), mLocation.c_str()));
	}

	// Extract the duration of the last frame
	lastFrame = FahLogAnalyzer::AnalyzeLastFrame(mLog, mVM);

	project = ProjectsManager::GetInstance()->GetProject(mProjectId);

	mCore = _T("");

	if(lastFrame != NULL && project != INVALID_PROJECT_ID && project != 0)
	{
		frameCount = lastFrame->GetFrameCount();
		mIsFrameCountAccurate = true;
		if(project->GetCoreId() == Core::TINKER)
		{
			frameCount = 400;
		}
		else if (frameCount == 0)
		{
			frameCount = project->GetNbFrames();
			mIsFrameCountAccurate = false;
		}
		if(frameCount != project->GetNbFrames())
		{
			//this might be expensive, maybe just writing the single project back would be better
			ProjectsManager::GetInstance()->AddProject(new Project(project->GetProjectId(), project->GetPreferredDeadlineInDays(), project->GetFinalDeadlineInDays(), frameCount, project->GetCredit(), project->GetCoreId()));
			ProjectsManager::GetInstance()->Save();
		}
		//correct for messed up WUs that run from 100%-200% etc.
		if(project->GetCoreId() != Core::TINKER && lastFrame->GetId() > 100)
		{
			lastFrame->SetId(lastFrame->GetId() % 100);
			if(lastFrame->GetId()==0)
			{
				lastFrame->SetId(100);
			}
		}
	}
	else
	{
		frameCount = 100;
	}

	// Add this duration to the benchmarks for valid projects, but don't store the same frame twice
	if(mProjectId != INVALID_PROJECT_ID && project != 0 && lastFrame != NULL && !lastFrame->ClientIsStopped() && !lastFrame->ClientIsPaused())
	{
		// Calculate effective frame time
		timeNow = wxDateTime::Now();
		// There may be a cleaner way of doing this
		timeDiff = timeNow.Subtract(mDownloadDate);
		// sanity checking because WUs with odd frame numbers still write out the % not the frameID
		if(project != INVALID_PROJECT_ID)
		{
			if ((frameCount != 100) && (project->GetCoreId() != Core::TINKER))
			{
				tempFloat = ((double)timeDiff.GetMinutes() * 60 / lastFrame->GetId()) / ((double)frameCount/100);
			}
			else
			{
				tempFloat = timeDiff.GetMinutes() * 60 / lastFrame->GetId();
			}
		}
		else //"normal" WUs
		{
			tempFloat = timeDiff.GetMinutes() * 60 / lastFrame->GetId();
		}
		lastFrame->SetEffectiveDuration(wxUint32(tempFloat));
		if(lastFrame->GetId() != mPreviouslyAnalyzedFrameId)
		{
			BenchmarksManager::GetInstance()->Add(mProjectId, this, lastFrame);
			mPreviouslyAnalyzedFrameId = lastFrame->GetId();
		}
	}


	// The state can be either running, stopped...
	FindCurrentState(lastFrame);

	_PrefsGetUint(PREF_PPD_DISPLAYSTYLE, ppdDisplay);

	// Default PPD is always 0
	mPPD = 0;
	frameTime = 0;
	
	// Compute ETA
	ComputeETA(lastFrame);

	// If current project is valid and is found in the benchmarks database, then grab the PPD
	// Needs to check state too, no point getting PPD for stopped or dead clients.
	if (mProjectId != INVALID_PROJECT_ID && project != 0 && GetState() != ST_STOPPED && GetState() != ST_INACCESSIBLE && GetState() != ST_HUNG && GetState() != ST_PAUSED)
	{
		if (project != INVALID_PROJECT_ID)
		{
			benchmarks = BenchmarksManager::GetInstance()->GetBenchmarksList(mProjectId, nbBenchmarks);
			if (nbBenchmarks != 0)
			{
				for(i=0; i<nbBenchmarks; ++i)
				{
					clientLocation = BenchmarksManager::GetInstance()->GetClientLocationFromClientId(benchmarks[i]->GetClientId()).c_str();
					clientName     = ClientsManager::GetInstance()->GetNameFromLocation(clientLocation.c_str()).c_str();
					if (clientName == mName)
					{
						switch(ppdDisplay)
						{
							// ---
							case PPDDS_ALL_FRAMES:
								frameTime = benchmarks[i]->GetAvgDuration();
								break;

							// ---
							case PPDDS_LAST_FRAME:
								if((double)benchmarks[i]->GetInstantDuration() != 0)
								{
									frameTime = benchmarks[i]->GetInstantDuration();
								}
								else
								{
									//best guess based on avg frame time
									frameTime = benchmarks[i]->GetAvgDuration();;
								}
								break;

							// ---
							case PPDDS_THREE_FRAMES:
								if((double)benchmarks[i]->Get3FrameDuration() != 0)
								{
									frameTime = benchmarks[i]->Get3FrameDuration();
								}
								else
								{
									//best guess based on avg frame time
									frameTime = benchmarks[i]->GetAvgDuration();
								}
								break;

							case PPDDS_EFFECTIVE_FRAMES:
								if((double)benchmarks[i]->GetEffectiveDuration() != 0)
								{
									frameTime = benchmarks[i]->GetEffectiveDuration();
								}
								else
								{
									//best guess based on avg frame time
									frameTime = benchmarks[i]->GetAvgDuration();
								}
								break;

							// We shall never fall here
							default:
								wxASSERT(false);
								break;
						}
						mPPD = ((double)mETA.GetTotalTimeInDays(mDownloadDate) < (double)project->GetPreferredDeadlineInDays()) ? (project->GetCredit() * std::max(1.0,sqrt(((double)project->GetKFactor() / 100) * ((double)project->GetFinalDeadlineInDays() / 100) / (double)mETA.GetTotalTimeInDays(mDownloadDate))) * 86400.0 / ((double)frameTime * (double)frameCount)) : project->GetCredit() * 86400.0 / ((double)frameTime * (double)frameCount);
					}
				}
			}
		}
	}
	if (mProjectId != INVALID_PROJECT_ID && project != 0 && GetState() != ST_INACCESSIBLE && lastFrame != NULL && GetState() != ST_STOPPED && GetState() != ST_PAUSED)
	{
		if (project != INVALID_PROJECT_ID)
		{
			// work out the actual progress
			mProgress = wxUint32((double)frameCount / ((double)frameCount / (double)lastFrame->GetId()));
			if(project->GetCoreId() == Core::TINKER)
			{
				mProgress = mProgress / 4;
			}
		}
	}
	mProgressString  = wxString::Format(_T("%u%%"), mProgress);
	if (project != INVALID_PROJECT_ID && project != 0)
	{
		mCore = Core::IdToLongName(project->GetCoreId()).c_str();
		mCredit = ((double)mETA.GetTotalTimeInDays(mDownloadDate) < (double)project->GetPreferredDeadlineInDays()) ? project->GetCredit() * std::max(1.0,sqrt(((double)project->GetKFactor() / 100) * ((double)project->GetFinalDeadlineInDays() / 100) / (double)mETA.GetTotalTimeInDays(mDownloadDate))) : project->GetCredit();
		mClientType = Core::IdToClientType(project->GetCoreId()).c_str();
		if(mDownloadDate.IsValid() && project->GetPreferredDeadlineInDays() != 0)
		{
			mDeadlineDate = mDownloadDate;
			mDeadlineDate.Add(wxTimeSpan::Seconds(project->GetPreferredDeadlineInDays() * 864));
		}
	}

	// We don't need this anymore
	if(lastFrame != NULL)
	{
		delete lastFrame;
		lastFrame = NULL;
	}

	_LogMsgInfo(wxString::Format(_("Finished Reloading %s"), mName.c_str()), false);
}


bool Client::LoadLogFile(wxString const &filename)
{
	wxString myLog;
	wxString localFile;

	if(multiProtocolFile::FileExists(GetPreviousFAHlog()) && (multiProtocolFile::GetFileProtocol(filename) == multiProtocolFile::FTP || multiProtocolFile::GetFileProtocol(filename) == multiProtocolFile::HTTP))
		wxRemoveFile(GetPreviousFAHlog());

	if(multiProtocolFile::FileExists(filename) == false || Tools::LoadFile(filename, myLog, localFile, FMC_MAX_LOG_LENGTH, false) == false)
	{
		mLog = _T("");
		return false;
	}

	mLog = myLog.c_str();
	SetPreviousFAHlog(localFile);
	return true;
}


bool Client::LoadUnitInfoFile(wxString const &filename)
{
	bool              progressOk;
	wxInt32           endingPos, startingPos;
	unsigned long     tmpLong;
	wxString          currentLine;
	wxString          localFile;

	if(multiProtocolFile::FileExists(GetPreviousUnitinfo()) && (multiProtocolFile::GetFileProtocol(filename) == multiProtocolFile::FTP || multiProtocolFile::GetFileProtocol(filename) == multiProtocolFile::HTTP))
		wxRemoveFile(GetPreviousUnitinfo());

	if(multiProtocolFile::FileExists(filename) == false || Tools::LoadFile(filename, currentLine, localFile, 512) == false)
	{
		return false;
	}
	SetPreviousUnitinfo(localFile);

	progressOk     = false;
	startingPos = currentLine.Find(_T("Progress:")) + 9;
	endingPos = currentLine.Find('%');

	if(endingPos != -1 && mProjectId != INVALID_PROJECT_ID && mProjectId != 0)
	{
		mProgressString = currentLine.Mid(startingPos, endingPos - startingPos);
		if(mProgressString.ToULong(&tmpLong) == true)
		{
			mProgress        = (wxUint32)tmpLong;
			mProgressString  = wxString::Format(_T("%u%%"), mProgress);
			progressOk       = true;
		}
	}

	// We only succeed if we have found the progress value
	if(progressOk)
	{
		return true;
	}

	// Log any error encountered
	if(!progressOk)
	{
		_LogMsgWarning(wxString::Format(_("The progress value in file %s could not be found/parsed"), filename.c_str()), false);
	}

	return false;
}


bool Client::LoadWULogFile(wxString const &filename)
{
	bool              versionOk;
	wxInt32           endingPos, startingPos;
	double            tmpDouble;
	wxString          tmpString;
	wxString          currentLine;
	wxString          localFile;

	if(multiProtocolFile::FileExists(GetPreviousLogfile()) && (multiProtocolFile::GetFileProtocol(filename) == multiProtocolFile::FTP || multiProtocolFile::GetFileProtocol(filename) == multiProtocolFile::HTTP))
		wxRemoveFile(GetPreviousLogfile());


	if(multiProtocolFile::FileExists(filename) == false || Tools::LoadFile(filename, currentLine, localFile, 256) == false)
	{
		return false;
	}
	SetPreviousLogfile(localFile);

	char *locOld = setlocale(LC_NUMERIC, "C");

	versionOk     = false;
	startingPos = currentLine.Find(_T("Version")) + 8;
	endingPos = currentLine.Find('(');

	if(endingPos != -1 && mProjectId != INVALID_PROJECT_ID && mProjectId != 0)
	{
		tmpString = currentLine.Mid(startingPos, endingPos - startingPos -1);
		if(tmpString.ToDouble(&tmpDouble) == true)
		{
			mCoreVersion     = tmpDouble;
			versionOk        = true;
		}
	}

	setlocale(LC_NUMERIC, locOld);

	// We only succeed if we have found the version value
	if(versionOk)
	{
		return true;
	}

	// Log any error encountered
	if(!versionOk)
	{
		_LogMsgWarning(wxString::Format(_("The core version in file %s could not be found/parsed"), filename.c_str()), false);
	}

	return false;
}


bool Client::LoadQueueFile(wxString const &filename)
{
	Queue    *qf;
	qf = new Queue();
	wxString localFile;

	if(multiProtocolFile::FileExists(GetPreviousQueue()) && (multiProtocolFile::GetFileProtocol(filename) == multiProtocolFile::FTP || multiProtocolFile::GetFileProtocol(filename) == multiProtocolFile::HTTP))
		wxRemoveFile(GetPreviousQueue());

	if (qf->LoadQueueFile(filename, mName, localFile))
	{
		mProjectId = qf->GetProjectId();
		mRun = qf->GetProjectRun();
		mGen = qf->GetProjectGen();
		mClone = qf->GetProjectClone();
		mDownloadDate = qf->GetDownloadDate();
		mUserName = qf->GetUserName();
		mTeamNumber = qf->GetTeamNumber();
		mUnitIndex = qf->GetUnitIndex();
		if(qf != NULL)
		{
			delete qf;
			qf = NULL;
		}
		SetPreviousQueue(localFile);
		return true;
	}
	if(qf != NULL)
	{
		delete qf;
		qf = NULL;
	}
	return false;
}


void Client::ComputeETA(WorkUnitFrame* lastFrame)
{
	wxUint32         totalFrames;
	wxUint32         referenceDuration;
	wxUint32         nbLeftSeconds;
	//wxString         logLine;
	wxUint32         ppdDisplay;
	const Project   *projectInfo;
	const Benchmark *benchmark;

	// If we don't have a valid project id or the client is stopped, there is no ETA
	if(mProjectId == INVALID_PROJECT_ID || mProjectId == 0 || (lastFrame && lastFrame->ClientIsStopped()))
	{
		mETA.SetLeftTimeInMinutes(0);
		return;
	}

	//logLine = wxString::Format(_T("%s [ETA]"), mName.c_str());
	if(lastFrame != NULL)
	{
		_LogMsgInfo(wxString::Format(_("%s is on frame %u"), mName.c_str(), lastFrame->GetId()), false);
	}
	else
	{
		_LogMsgInfo(wxString::Format(_("Cannot determine frame number for %s (this isn't a problem)"), mName.c_str()), false);
	}

	// --- 1) Retrieve the total number of frames for the current project, or try to guess it if possible
	projectInfo = ProjectsManager::GetInstance()->GetProject(mProjectId);
	if(projectInfo != NULL)
	{
		totalFrames = projectInfo->GetNbFrames();
	}
	else if(lastFrame != NULL && mProgress != 0)
	{
		// Compute a simple cross product to guess the total number of frames
		totalFrames = lastFrame->GetId() * 100 / mProgress;
		totalFrames = totalFrames - (totalFrames % 20);
	}
	else
	{
		// If we really have no clue about the total number of frames, we cannot tell anything about the ETA
		mETA.SetLeftTimeInMinutes(0);
		return;
	}


	// This can't be equal to 0, as we need this value in further computations
	wxASSERT(totalFrames != 0);


	_PrefsGetUint(PREF_PPD_DISPLAYSTYLE, ppdDisplay);

	// --- 2) Compute a reference duration of a frame
	benchmark = BenchmarksManager::GetInstance()->GetBenchmark(mProjectId, this);
	if(benchmark != NULL && lastFrame != NULL)
	{
		//logLine = logLine + wxString::Format(wxT(" | avg=%us | last=%us"), benchmark->GetAvgDuration(), lastFrame->GetDuration());
		/**
		 * My theory about the computation of the ETA :-)
		 * As the computation of the WU is reaching the end, the duration of the last run will become more important.
		 * However, when the computation has just begun, the average duration of a run is much more important than the 'current'
		 * duration of runs.
		 * So the duration used as a 'metric' is composed by the two durations, in regards of the progress of the computation.

		 *   Tm = Average run duration
		 *   Tl = Duration of the last run
		 *   Y  = Total number of runs for the WU
		 *   X  = Number of the last run

		 *   Duration = (Tl * X / Y) + (Tm * (1 - X / Y))
		 *            = Tl * X / Y + Tm - Tm * X / Y
		 *            = (Tl - Tm) * X / Y + Tm

		 * wxInt32 Tm = benchmark->GetAvgDuration();
		 * wxInt32 Tl = lastFrame->GetDuration();
		 * wxInt32 Y  = totalFrames;
		 * wxInt32 X  = lastFrame->GetId();

		 * referenceDuration = (wxUint32)((Tl - Tm) * X / Y + Tm);

		 * This causes problems with some WUs, for which the duration of a frame can vary a lot
		 * In this case, the ETA varies a lot too, so for now we only use the average duration of a frame
		 **/

		/**
		 * Additional comment about above method: appears to be a "hybrid" system combining both "avg" and "cur" or "r3f" methods now
		 * implemented as ETA systems. Might be worth considering in future if it is possible to overcome the varying frame time
		 * problem
		 **/

		//logLine += wxString::Format(_T(" | avg=%s"), wxTimeSpan::Seconds(benchmark->GetAvgDuration()).Format(_T("%H:%M:%S")).c_str());

		switch(ppdDisplay)
		{
			// ---
			case PPDDS_EFFECTIVE_FRAMES:
				referenceDuration = benchmark->GetEffectiveDuration();
				break;

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
		//logLine += wxString::Format(_T(" | avg=%s | last=N/A"), wxTimeSpan::Seconds(benchmark->GetAvgDuration()).Format(_T("%H:%M:%S")).c_str());

		// No duration for the last frame, use the average duration
		referenceDuration = benchmark->GetAvgDuration();
	}
	else if(lastFrame != NULL)
	{
		//logLine += wxString::Format(_T(" | avg=N/A | last=%s"), wxTimeSpan::Seconds(lastFrame->GetDuration()).Format(_T("%H:%M:%S")).c_str());

		// No average duration of a frame, use the duration of the last frame
		referenceDuration = lastFrame->GetDuration();
	}
	else
	{
		//logLine += wxString::Format(_T(" | avg=N/A | last=N/A"));
		//_LogMsgInfo(logLine);

		// No indication on the average duration of a frame, nor on the duration of the last frame, so we cannot do anything
		mETA.SetLeftTimeInMinutes(0);
		return;
	}

	//logLine += wxString::Format(_T(" | ref=%s"), wxTimeSpan::Seconds(referenceDuration).Format(_T("%H:%M:%S")).c_str());

	// --- 3) Compute the the left time
	if(lastFrame != NULL)
	{
		if((projectInfo != NULL) && (projectInfo->GetCoreId() == Core::TINKER))
		{
			nbLeftSeconds = referenceDuration * (totalFrames - lastFrame->GetId());
		}
		else
		{
			nbLeftSeconds = referenceDuration * (totalFrames - (int)((lastFrame->GetId() * ((float)totalFrames/100)))); //correction for odd gromacs WUs
			//logLine += wxString::Format(wxT(" | fl=%i"), totalFrames - (int)((lastFrame->GetId() * ((float)totalFrames/100)))); //debug line
		}

		// Make a more accurate value by using the elapsed time since the end of the last frame
		if(lastFrame->GetElapsedSeconds() > referenceDuration)
		{
			nbLeftSeconds = nbLeftSeconds - referenceDuration;
			//logLine += wxString::Format(_T(" | adj=%s"), wxTimeSpan::Seconds(referenceDuration).Format(_T("%H:%M:%S")).c_str());
		}
		else
		{
			nbLeftSeconds = nbLeftSeconds - lastFrame->GetElapsedSeconds();
			//logLine += wxString::Format(_T(" | adj=%s"), wxTimeSpan::Seconds(lastFrame->GetElapsedSeconds()).Format(_T("%H:%M:%S")).c_str());
		}

		//logLine += wxString::Format(_T(" | left=%s"), wxTimeSpan::Seconds(nbLeftSeconds).Format(_T("%H:%M:%S")).c_str());
	}
	else
	{
		// We don't have the identifier of the last computed frame, so we have to try to guess it once again
		wxUint32 lastComputedFrame = totalFrames * mProgress / 100;

		//nbLeftSeconds = referenceDuration * (totalFrames - lastComputedFrame);
		if((projectInfo != NULL) && (projectInfo->GetCoreId() == Core::TINKER))
		{
			nbLeftSeconds = referenceDuration * (totalFrames - lastComputedFrame);
		}
		else
		{
			nbLeftSeconds = referenceDuration * (totalFrames - (lastComputedFrame * (totalFrames/100))); //correction for odd gromacs WUs
		}

		//logLine += wxString::Format(_T(" | left=%us (guess)"), nbLeftSeconds);
	}

	// --- 4) That's it
	mETA.SetLeftTimeInMinutes(nbLeftSeconds/60);
	//_LogMsgInfo(logLine);
}


void Client::FindCurrentState(WorkUnitFrame* lastFrame)
{
	wxUint32         trigger;
	const Benchmark* benchmark;

	// If we don't have information about the last frame, we decide that the corresponding client is inactive
	if(lastFrame == NULL)
	{
		SetState(ST_INACTIVE);
		_LogMsgInfo(wxString::Format(_("%s has an unknown state (Unable to find a complete frame)"), mName.c_str()), false);
		return;
	}


	// No ETA for stopped clients
	if(lastFrame->ClientIsStopped())
	{
		SetState(ST_STOPPED);
		_LogMsgInfo(wxString::Format(_("%s is stopped (The line \"Folding@Home Client Shutdown.\" was found)"), mName.c_str()), false);
		return;
	}


	if(lastFrame->ClientIsPaused())
	{
		SetState(ST_PAUSED);
		_LogMsgInfo(wxString::Format(_("%s has been paused"), mName.c_str()), false);
		return;
	}


	// No elapsed time since the last frame means we couldn't get valid information on elapsed time
	if(lastFrame->GetElapsedSeconds() == 0)
	{
		SetState(ST_INACTIVE);
		_LogMsgInfo(wxString::Format(_("%s has an unknown state (Unable to extract a valid elapsed time since the last completed frame)"), mName.c_str()), false);
		return;
	}

	// Asynchronous client
	if(lastFrame->GetElapsedSeconds() == 65535 && !mVM)
	{
		SetState(ST_ASYNCH);
		_LogMsgInfo(wxString::Format(_("%s has been marked as having a asynchronous clock."), mName.c_str()), false);
		return;
	}


	// Use benchmarked duration if available, otherwise use last frame duration
	// The limit is first fixed to three times the reference duration
	benchmark = BenchmarksManager::GetInstance()->GetBenchmark(mProjectId, this);
	if(benchmark != NULL)
	{
		trigger = 3 * benchmark->GetAvgDuration();
	}
	else
	{
		trigger = 3 * lastFrame->GetDuration();
	}

	// Raise the limit if it is too low
	// For example, if frames take 1mn to complete, a 2mn compilation process (for example) would immediately make the client
	// inactive, and we don't want to alert the user too early
	if(trigger < 45*60)
	{
		trigger = 45*60;
	}

	// Last step
	if(lastFrame->GetElapsedSeconds() < trigger || mVM)
	{
		SetState(ST_RUNNING);
	}
	else
	{
		if(lastFrame->GetElapsedSeconds() > 2 * trigger)
		{
			SetState(ST_HUNG);
			_LogMsgWarning(wxString::Format(_("%s seems to have hung : Elapsed time is %um and limit is %um"), mName.c_str(), lastFrame->GetElapsedSeconds()/60, trigger/30), false);
		}
		else
		{
			SetState(ST_INACTIVE);
			_LogMsgInfo(wxString::Format(_("%s seems to be inactive : Elapsed time is %um and limit is %um"), mName.c_str(), lastFrame->GetElapsedSeconds()/60, trigger/60), false);
		}
	}
}


wxString Client::GetDonatorStatsURL(void) const
{
	return wxString::Format(_T("%s&teamnum=%u&username=%s"), _T(FMC_URL_MYSTATS), mTeamNumber, mUserName.c_str());
}


wxString Client::GetTeamStatsURL(void) const
{
	return wxString::Format(_T("%s&teamnum=%u"), _T(FMC_URL_TEAMSTATS), mTeamNumber);
}


wxString Client::GetJmolURL(void) const
{
	return wxString::Format(_T("%s%u"), _T(FMC_URL_JMOL), GetProjectId());
}


wxString Client::GetFahinfoURL(void) const
{
	return wxString::Format(_T("%s%u"), _T(FMC_URL_FAHINFO), GetProjectId());
}


void Client::SetState(State value)
{
	wxMutexLocker lock(Client::mMutexReadWrite);
	mState = value;
}

const Client::State Client::GetState(void) const
{
	wxMutexLocker lock(mMutexReadWrite);
	return mState;
}
