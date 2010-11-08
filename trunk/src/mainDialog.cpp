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
 * \file mainDialog.cpp
 * The main dialog.
 * Creates the main window for FahMon.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "mainDialog.h"

#include "main.h"
#include "tools.h"
#include "client.h"
#include "trayManager.h"
#include "aboutDialog.h"
#include "pathManager.h"
#include "clientDialog.h"
#include "messagesFrame.h"
#include "clientsManager.h"
#include "messagesManager.h"
#include "projectsManager.h"
#include "staticBoldedText.h"
#include "benchmarksDialog.h"
#include "preferencesDialog.h"
#include "preferencesManager.h"
#include "listViewClients.h"
#include "httpDownloader.h"
#include "webMonitor.h"
#include "multiProtocolFile.h"

#include "wx/colour.h"
#include "wx/filefn.h"
#include "wx/textfile.h"
#include "wx/hyperlink.h"
#include "wx/filename.h"
#ifdef __WXMAC__
#include "wx/sysopt.h"
#endif


// Identifiers for the controls
enum _CONTROL_ID
{
	// --- Menus
	MID_RELOAD = wxID_HIGHEST,
	MID_RELOAD_ALL,
	MID_UPDATEPROJECTS,
	MID_TOGGLELOG,
	MID_TOGGLE_MESSAGES_FRAME,
	MID_TOGGLE_ETADATE,
	MID_BENCHMARKS,
	MID_WWWJMOL,
	MID_WWWMYSTATS,
	MID_WWWFOLDING,
	MID_WWWFCORG,
	MID_WWWPROJECTS,
	MID_WWWSERVERS,
	MID_WWWFAHINFO,
	MID_UPDATECHECK,
	MID_ADDCLIENT,
	MID_EDITCLIENT,
	MID_DELETECLIENT,
	MID_VIEWCLIENT,
	MID_TOGGLE_INFOPANEL,
	MID_CREATE_DEBUG_REPORT,
	MID_WWWFAHMONIRC,
	MID_LOCALTIMER,
	MID_INETTIMER,

	// --- ListView
	LST_CLIENTS
};

enum _LISTVIEW_COLUMN
{
	LVC_PROGRESS,
	LVC_NAME,
	LVC_ETA,
	LVC_PPD
};


// Custom events
DEFINE_EVENT_TYPE(EVT_CLIENTRELOADED)
DEFINE_EVENT_TYPE(EVT_NEWCLIENTADDED)
DEFINE_EVENT_TYPE(EVT_CLIENTDELETED)
DEFINE_EVENT_TYPE(EVT_PROJECTS_DATABASE_UPDATED)
DEFINE_EVENT_TYPE(EVT_NEW_MESSAGE_LOGGED)


// Events processed by this class
BEGIN_EVENT_TABLE(MainDialog, wxFrame)
	// --- Menus
	EVT_MENU    (wxID_EXIT,                 MainDialog::OnMenuQuit)
	EVT_MENU    (MID_RELOAD,                MainDialog::OnMenuReload)
	EVT_MENU    (MID_RELOAD_ALL,            MainDialog::OnMenuReloadAll)
	EVT_MENU    (MID_UPDATEPROJECTS,        MainDialog::OnMenuUpdateProjects)
	EVT_MENU    (MID_TOGGLELOG,             MainDialog::OnMenuToggleLog)
	EVT_MENU    (MID_TOGGLE_MESSAGES_FRAME, MainDialog::OnMenuToggleMessagesFrame)
	EVT_MENU    (MID_TOGGLE_ETADATE,        MainDialog::OnMenuToggleETADate)
	EVT_MENU    (MID_BENCHMARKS,            MainDialog::OnMenuBenchmarks)
	EVT_MENU    (wxID_PREFERENCES,          MainDialog::OnMenuPreferences)
	EVT_MENU    (MID_WWWJMOL,               MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWFAHINFO,            MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWMYSTATS,            MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWFOLDING,            MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWFCORG,              MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWPROJECTS,           MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWSERVERS,            MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWFAHMONIRC,          MainDialog::OnMenuWeb)
	EVT_MENU    (wxID_HELP,                 MainDialog::OnMenuWeb)
	EVT_MENU    (wxID_ABOUT,                MainDialog::OnMenuAbout)
	EVT_MENU    (MID_UPDATECHECK,           MainDialog::OnUpdateCheck)
	EVT_MENU    (MID_ADDCLIENT,             ListViewClients::OnMenuAddClient)
	EVT_MENU    (MID_EDITCLIENT,            MainDialog::OnMenuEditClient)
	EVT_MENU    (MID_DELETECLIENT,          MainDialog::OnMenuDeleteClient)
	EVT_MENU    (MID_VIEWCLIENT,            MainDialog::OnMenuViewFiles)
	EVT_MENU    (MID_TOGGLE_INFOPANEL,      MainDialog::OnMenuToggleInfoPanel)
	EVT_MENU    (MID_CREATE_DEBUG_REPORT,   MainDialog::OnMenuCreateDebugReport)

	// --- Frame
	EVT_CLOSE   (MainDialog::OnClose)
	EVT_ICONIZE (MainDialog::OnIconize)

	// --- List
	EVT_LIST_ITEM_SELECTED  (LST_CLIENTS, MainDialog::OnListSelectionChanged)

	// --- Timers
	EVT_TIMER   (wxID_ANY,  MainDialog::OnAutoReloadTimer)

	// --- Custom
	EVT_COMMAND    (wxID_ANY, EVT_CLIENTRELOADED,            MainDialog::OnClientReloaded)
	EVT_COMMAND    (wxID_ANY, EVT_NEWCLIENTADDED,            MainDialog::OnNewClientAdded)
	EVT_COMMAND    (wxID_ANY, EVT_CLIENTDELETED,             MainDialog::OnClientDeleted)
	EVT_COMMAND    (wxID_ANY, EVT_PROJECTS_DATABASE_UPDATED, MainDialog::OnProjectsDatabaseUpdated)
	EVT_COMMAND    (wxID_ANY, EVT_NEW_MESSAGE_LOGGED,        MainDialog::OnNewMessageLogged)
END_EVENT_TABLE()

// The single instance of MainDialog accross the application
MainDialog* MainDialog::mInstance = NULL;

wxMutex MainDialog::mMutexUpdateCheck;
wxMutex MainDialog::mMutexArrayBlocker;
wxMutex MainDialog::mMutexClientInformation;


MainDialog::MainDialog(void) : wxFrame(NULL, wxID_ANY, _T(FMC_PRODUCT))
{
	bool trayIconEnabled;
	bool updateCheck;
	int widths[3] = {-1,-1,100};

	// Setting the icon for the main dialog will allows child frames and dialog to inherit from it

#ifdef __WXGTK__
	SetIcon(wxIcon(PathManager::GetImgPath() + _T(FMC_FILE_IMG_DIALOG)));
#elif _FAHMON_WIN32_
	SetIcon(wxICON(dialog_icon));
#endif

	CreateMenuBar();
	CreateStatusBar(3);
	SetStatusWidths(3, widths);
	CreateLayout();
	RestoreFrameState();     // MUST be called when all controls have been created !

	_LogMsgInfo(wxString::Format(_("%s is licenced under the GNU GPL v2"), _T(FMC_PRODUCT)), true); // add true to force this notice
	_LogMsgInfo(_("Copyright (C) 2003-2007 Fran\u00E7ois Ingelrest"), true);
	_LogMsgInfo(_("Copyright (C) 2007-2010 Andrew Schofield"), true);
	_LogMsgInfo(_T(""), true);
	_LogMsgInfo(_("FahMon contains code from qd, also licenced under the GNU GPL v2"), true);
	_LogMsgInfo(_("Copyright (C) 2002-2005 Richard P. Howell IV."), true);
	_LogMsgInfo(_("Copyright (C) 2005-2010 Sebastiaan Couwenberg"), true);
	_LogMsgInfo(_T(""), true);
	// this needs to be before the tray icon, otherwise you can still click the tray icon and crash FahMon
	_PrefsGetBool(PREF_MAINDIALOG_UPDATE_CHECK, updateCheck);
	if(updateCheck == true)
	{
		CheckForUpdates();
		_LogMsgInfo(_T(""), false);
	}

	// The timer used for auto-reloading
	mAutoReloadTimer.SetOwner(this, MID_LOCALTIMER);
	SetAutoReloadTimer();
	mInetAutoReloadTimer.SetOwner(this, MID_INETTIMER);
	SetInetAutoReloadTimer();

	// The tray icon
	_PrefsGetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON, trayIconEnabled);
	if(trayIconEnabled == true)
	{
		TrayManager::GetInstance()->InstallIcon();
	}
}


MainDialog::~MainDialog(void)
{
}


void MainDialog::CreateInstance(void)
{
	wxASSERT(mInstance == NULL);

	mInstance = new MainDialog();
}


void MainDialog::DestroyInstance(void)
{
	wxASSERT(mInstance != NULL);

	delete mInstance;
	mInstance = NULL;
}


bool MainDialog::HasBeenInstanciated(void)
{
	return mInstance != NULL;
}


MainDialog* MainDialog::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}


bool MainDialog::Show(bool show)
{
	// Showing the frame must be done before selecting a client, or the sash position won't be restored
	bool result = wxFrame::Show(show);

	if(show == true)
	{
		// Items surely won't keep their order when they will be loaded because of sorting, so selecting a
		// default one is useless if there are more than one client
		// Of course, in the case of only one client, we can still select it
		if(ClientsManager::GetInstance()->GetCount() > 0)
		{
			mClientsList->Select(0);
		}
		else
		{
			ShowClientInformation(INVALID_CLIENT_ID);
		}
		ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
	}

	return result;
}


void MainDialog::SetAutoReloadTimer(void)
{
	bool     isAutoReloadOn;
	bool     isAdvancedReloadOn;
	wxUint32 autoReloadFrequency;

	// First, we have to stop the timer if it is running, because preferences may be different from the last time
	if(mAutoReloadTimer.IsRunning())
	{
		mAutoReloadTimer.Stop();
	}

	// Then, retrieve the (perhaps new) preferences
	_PrefsGetBool(PREF_MAINDIALOG_AUTORELOAD,          isAutoReloadOn);
	_PrefsGetBool(PREF_MAINDIALOG_ADVANCEDRELOAD,      isAdvancedReloadOn); // this is actually the experimental system
	_PrefsGetUint(PREF_MAINDIALOG_AUTORELOADFREQUENCY, autoReloadFrequency);

	// Ok, now we can start the timer if needed but we need to convert minutes to milliseconds
	if(isAutoReloadOn && !wxGetApp().GetStress())
	{
		if(isAdvancedReloadOn)
		{
			mAutoReloadTimer.Start(10 * 1000); // experimental updates
		}
		else
		{
			mAutoReloadTimer.Start(autoReloadFrequency * 60 * 1000); // regular update system
		}
	}
	else if(wxGetApp().GetStress())
	{
		if(isAdvancedReloadOn)
		{
			mAutoReloadTimer.Start(500); // experimental updates
		}
		else
		{
			mAutoReloadTimer.Start(500); // regular update system
		}
	}
}


void MainDialog::SetInetAutoReloadTimer(void)
{
	bool     isAutoReloadOn;
	wxUint32 autoReloadFrequency;

	// First, we have to stop the timer if it is running, because preferences may be different from the last time
	if(mInetAutoReloadTimer.IsRunning())
	{
		mInetAutoReloadTimer.Stop();
	}

	// Then, retrieve the (perhaps new) preferences
	_PrefsGetBool(PREF_MAINDIALOG_AUTORELOAD,          isAutoReloadOn);
	_PrefsGetUint(PREF_MAINDIALOG_INETAUTORELOADFREQUENCY, autoReloadFrequency);

	// Ok, now we can start the timer if needed but we need to convert minutes to milliseconds
	if(isAutoReloadOn  && !wxGetApp().GetStress())
	{
		mInetAutoReloadTimer.Start(autoReloadFrequency * 60 * 1000); // regular update system
	}
	else if(wxGetApp().GetStress())
	{
		mInetAutoReloadTimer.Start(500);
	}
}


void MainDialog::ShowClientInformation(ClientId clientId)
{
	UpdateClientInformation(clientId);
	mTopLevelSizer->Layout();

	// win32: The layout of the sizer which contains the two StaticUrl objects does not seem
	// to be updated by the previous Layout() call, so we need to do it ourselves
	mUsername->GetContainingSizer()->Layout();
}


void MainDialog::UpdateClientInformation(ClientId clientId)
{
	bool          autoUpdateProjects;
	wxUint32      deadlineDays;
	wxDateTime    preferredDeadline;
	wxDateTime    finalDeadline;
	wxDateTime    downloadTime;
	wxDateTime    timeNow;
	wxTimeSpan    timeDiff;
	const Client  *client;
	const Project *project;
	wxInt32       timeInMinutes;
	wxString      maskedLocation;

	_PrefsGetUint(PREF_ETA_DISPLAYSTYLE, deadlineDays);

	// Clear information for invalid clients
	if(clientId == INVALID_CLIENT_ID)
	{
		if(mSplitterWindow->IsSplit())
		{
			mUsername->Disable();
			mUsername->SetURL(_T(""));
			mUsername->SetLabel(_T(""));
			mTeamNumber->Disable();
			mTeamNumber->SetURL(_T(""));
			mTeamNumber->SetLabel(_T(""));

			mCoreName->SetLabel(_T(""));
			mProjectId->SetLabel(_T(""));
			mCredit->SetLabel(_T(""));
			mDownloaded->SetLabel(_T(""));
			mPreferredDeadline->SetLabel(_T(""));
			mFinalDeadline->SetLabel(_T(""));
			mWUProgressText->SetLabel(_T(""));
		}

		mWUProgressGauge->SetValue(0);
		SetStatusText(_T(""), STATUS_CLIENTNAME);

		if(mLogFile->IsShown() == true)
		{
			mLogFile->SetValue(_T(""));
		}

		return;
	}

	client = ClientsManager::GetInstance()->Get(clientId);

	// This method cannot be called for 'ghost' clients, if this is the case, then there's a big problem
	wxASSERT(client != NULL);

	maskedLocation = client->GetLocation();
	if(multiProtocolFile::GetFileProtocol(maskedLocation) != multiProtocolFile::FILE)
	{
		int start = maskedLocation.Find(wxT("://")) + 3;
		int end = maskedLocation.Find(wxT("@"));
		if(end != wxNOT_FOUND)
		{
			maskedLocation.Replace(maskedLocation.Mid(start, end-start+1), wxT(""));
		}

	}
	SetStatusText(maskedLocation, STATUS_CLIENTNAME);

	// Clear information if this client is not a valid one
	if(!client->IsAccessible())
	{
		if(mSplitterWindow->IsSplit())
		{
			mUsername->Disable();
			mUsername->SetURL(_T(""));
			mUsername->SetLabel(_T(""));
			mTeamNumber->Disable();
			mTeamNumber->SetURL(_T(""));
			mTeamNumber->SetLabel(_T(""));

			mCoreName->SetLabel(_T(""));
			mProjectId->SetLabel(_T(""));
			mCredit->SetLabel(_T(""));
			mDownloaded->SetLabel(_T(""));
			mPreferredDeadline->SetLabel(_T(""));
			mFinalDeadline->SetLabel(_T(""));
			mWUProgressText->SetLabel(_T(""));
		}
		mWUProgressGauge->SetValue(0);
		if(client->IsEnabled())
			mLogFile->SetValue(_("Something is wrong with this client.\nPlease check the messages (Tools->Show/Hide Messages Window)."));
		else
			mLogFile->SetValue(_("This client is currently disabled."));

		return;
	}

	// Load the log only if it is not hidden, it's useless otherwise
	if(mLogFile->IsShown())
	{
		// Arghhhhh, it takes *ages* on wxGTK to load the log file within the wxTextCtrl!!
		mLogFile->ChangeValue(client->GetLog());
#ifdef __WXMAC__
		::wxYield();
		mLogFile->AppendText(_T("\n"));
#endif
		mLogFile->ShowPosition(mLogFile->GetLastPosition());
	}

	mWUProgressText->SetLabel(_T("  ") + client->GetProgressString());
	mWUProgressGauge->SetValue(client->GetProgress());

	if(mSplitterWindow->IsSplit())
	{
		mUsername->Enable();
		mUsername->SetLabel(client->GetDonatorName());
		mUsername->SetURL(client->GetDonatorStatsURL());
		mUsername->Refresh();

		mTeamNumber->Enable();
		mTeamNumber->SetLabel(wxString::Format(_T("(%u)"), client->GetTeamNumber()));
		mTeamNumber->SetURL(client->GetTeamStatsURL());
		mTeamNumber->Refresh();

		if(client->GetDownloadDate().IsValid())
		{
			timeNow = wxDateTime::Now();
			downloadTime = client->GetDownloadDate();
			if(deadlineDays == ETADS_LEFT_TIME)
			{
				//timeDiff = timeNow.Subtract(downloadTime);
				timeInMinutes = timeNow.Subtract(downloadTime).GetMinutes();

				mDownloaded->SetLabel(wxString::Format(_("%s ago"), ETA::FormatMinutes(timeInMinutes).c_str()));
			}
			else if (deadlineDays == ETADS_DATE_DAY_MONTH)
			{
				mDownloaded->SetLabel(wxString::Format(_T("%s"), downloadTime.Format(_T("%d %B, %H:%M")).c_str()));
			}
			else
			{
				mDownloaded->SetLabel(wxString::Format(_T("%s"), downloadTime.Format(_T("%B %d, %H:%M")).c_str()));
			}
		}
		else
		{
			mDownloaded->SetLabel(_("N/A"));
		}

		if(client->GetProjectId() == INVALID_PROJECT_ID)
		{
			mProjectId->SetLabel(_("N/A"));
			mCoreName->SetLabel(_("N/A"));
			mCredit->SetLabel(_("N/A"));
			mPreferredDeadline->SetLabel(_("N/A"));
			mFinalDeadline->SetLabel(_("N/A"));

			return;
		}

		mProjectId->SetLabel(wxString::Format(_T("%u (R%i, C%i, G%i)"), client->GetProjectId(), client->GetProjectRun(), client->GetProjectClone(), client->GetProjectGen()));
	}

	project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());

	_PrefsGetBool(PREF_MAINDIALOG_AUTOUPDATEPROJECTS, autoUpdateProjects);

	// This project can be unknown, if the database is not up to date
	if(project == NULL && autoUpdateProjects)
	{
		  ProjectsManager::GetInstance()->UpdateDatabase(false, false);
		  project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());
		  //ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
	}
	if(project == NULL)
	{
		if(mSplitterWindow->IsSplit())
		{
			mCoreName->SetLabel(_("Unknown"));
			mCredit->SetLabel(_("Unknown"));
			mPreferredDeadline->SetLabel(_("Unknown"));
			mFinalDeadline->SetLabel(_("Unknown"));
		}
		_LogMsgWarning(wxString::Format(_("Project %u is unknown, you should try to update the projects database"), client->GetProjectId()), false);
	}
	else if(mSplitterWindow->IsSplit())
	{
		// We do have project information
		mCoreName->SetLabel(Core::IdToLongName(project->GetCoreId()));
		mCredit->SetLabel(wxString::Format(_("%u points"), project->GetCredit()));

		// Preferred deadline: if it is equal to 0 day, there is no preferred deadline
		if(client->GetDownloadDate().IsValid() && project->GetPreferredDeadlineInDays() != 0)
		{
			preferredDeadline = downloadTime;
			preferredDeadline.Add(wxTimeSpan::Seconds(project->GetPreferredDeadlineInDays() * 864));
			if(deadlineDays == ETADS_LEFT_TIME)
			{
				timeDiff = preferredDeadline.Subtract(timeNow);
				timeInMinutes = timeDiff.GetMinutes();
				if(timeDiff.GetMinutes() < 0)
				{
					timeInMinutes = 0 - timeInMinutes;
				}

				if(timeDiff.GetMinutes() < 0)
				{
					mPreferredDeadline->SetLabel(wxString::Format(_("%s ago"), ETA::FormatMinutes(timeInMinutes).c_str()));
				}
				else
				{
					mPreferredDeadline->SetLabel(wxString::Format(_("In %s"), ETA::FormatMinutes(timeInMinutes).c_str()));
				}

			}
			else if (deadlineDays == ETADS_DATE_DAY_MONTH)
			{
				mPreferredDeadline->SetLabel(wxString::Format(_T("%s"), preferredDeadline.Format(_T("%d %B, %H:%M")).c_str()));
			}
			else
			{
				mPreferredDeadline->SetLabel(wxString::Format(_T("%s"), preferredDeadline.Format(_T("%B %d, %H:%M")).c_str()));
			}
		}
		else
		{
			mPreferredDeadline->SetLabel(_("N/A"));
		}

		// Final deadline: if it is equal to 0 day, there is no final deadline
		if(client->GetDownloadDate().IsValid() && project->GetFinalDeadlineInDays() != 0)
		{
			finalDeadline = downloadTime;
			finalDeadline.Add(wxTimeSpan::Seconds(project->GetFinalDeadlineInDays() * 864));
			if(deadlineDays == ETADS_LEFT_TIME)
			{
				timeDiff = finalDeadline.Subtract(timeNow);
				timeInMinutes = timeDiff.GetMinutes();
				if(timeDiff.GetMinutes() < 0)
				{
					timeInMinutes = 0 - timeInMinutes;
				}

				if(timeDiff.GetMinutes() < 0)
				{
					mFinalDeadline->SetLabel(wxString::Format(_("%s ago"), ETA::FormatMinutes(timeInMinutes).c_str()));
				}
				else
				{
					mFinalDeadline->SetLabel(wxString::Format(_("In %s"), ETA::FormatMinutes(timeInMinutes).c_str()));
				}
			}
			else if (deadlineDays == ETADS_DATE_DAY_MONTH)
			{
				mFinalDeadline->SetLabel(wxString::Format(_T("%s"), finalDeadline.Format(_T("%d %B, %H:%M")).c_str()));
			}
			else
			{
				mFinalDeadline->SetLabel(wxString::Format(_T("%s"), finalDeadline.Format(_T("%B %d, %H:%M")).c_str()));
			}
		}
		else
		{
			mFinalDeadline->SetLabel(_("N/A"));
		}
	}
	// Get the total PPD to display next to progress bar
	SetStatusText(wxString::Format(_("%.2f PPD"), MainDialog::GetTotalPPD()), STATUS_TOTALPPD);

}


/************************************  FRAME CREATION  ************************************/


void MainDialog::CreateMenuBar(void)
{
	wxMenu    *menuFahMon;
	wxMenu    *menuClient;
	wxMenu    *menuMonitoring;
	wxMenu    *menuTools;
	wxMenu    *menuWeb;
	wxMenu    *menuHelp;
	wxMenuBar *menuBar;

	// This method must be called once!
	wxASSERT(GetMenuBar() == NULL);

	// Create the menubar
	menuBar = new wxMenuBar();

	// The 'Main' menu
	menuFahMon = new wxMenu();
	#ifndef __WXMAC__
	menuFahMon->Append(MID_UPDATECHECK, _("&Check for update"), _("Check online for the latest version of FahMon"));
	menuFahMon->AppendSeparator();
	#endif
	menuFahMon->Append(wxID_PREFERENCES, _("&Preferences...\tCTRL+P"), _("Open the preferences dialog"));
	menuFahMon->AppendSeparator();
	menuFahMon->Append(wxID_EXIT, _("&Quit\tCtrl+Q"), wxString::Format(_("Quit %s"), _T(FMC_APPNAME)));
	#ifndef __WXMAC__
	menuBar->Append(menuFahMon, wxString::Format(_T("&%s"), _T(FMC_APPNAME)));
	#endif

	//The 'Clients' menu
	menuClient = new wxMenu();
	menuClient->Append(MID_ADDCLIENT, _("Add a new client"), _("Add a new client to be monitored"));
	menuClient->AppendSeparator();
	menuClient->Append(MID_EDITCLIENT, _("Edit client"), _("Edit the selected client settings"));
	menuClient->Append(MID_DELETECLIENT, _("Delete client"), _("Delete the selected client"));
	menuClient->Append(MID_VIEWCLIENT, _("View client files"), _("Open file browser in client folder"));
	menuClient->AppendSeparator();
	#ifndef __WXMAC__
	menuClient->Append(MID_RELOAD, _("Reload &Selection\tF5"), _("Reload the selected client"));
	menuClient->Append(MID_RELOAD_ALL, _("Reload &All\tF6"), _("Reload all the clients"));
	#else
	menuClient->Append(MID_RELOAD, _("Reload &Selection\tCtrl+R"), _("Reload the selected client"));
	menuClient->Append(MID_RELOAD_ALL, _("Reload &All\tCtrl+Shift+R"), _("Reload all the clients"));
	#endif
	menuBar->Append(menuClient, _("&Clients"));

	// The 'Monitoring' menu
	menuMonitoring = new wxMenu();
	#ifndef __WXMAC__
	menuMonitoring->Append(MID_TOGGLELOG, _("&Show/Hide FAHLog\tF8"), _("Toggle the log file"));
	menuMonitoring->AppendSeparator();
	menuMonitoring->Append(MID_TOGGLE_ETADATE, _("&Cycle ETA Style\tF9"), _("Cycle through the different ETA display styles"));
	#else
	menuMonitoring->Append(MID_TOGGLELOG, _("&Show/Hide FAHLog\tCtrl+L"), _("Toggle the log file"));
	menuMonitoring->AppendSeparator();
	menuMonitoring->Append(MID_TOGGLE_ETADATE, _("&Cycle ETA Style\tCtrl+E"), _("Cycle through the different ETA display styles"));
	#endif
	menuMonitoring->AppendSeparator();
	menuMonitoring->Append(MID_TOGGLE_INFOPANEL, _("Show/Hide &WU Info panel"), _("Toggle the display of the Work Unit Information panel"));
	menuBar->Append(menuMonitoring, _("&View"));

	// The 'Tools' menu
	menuTools = new wxMenu();
	menuTools->Append(MID_TOGGLE_MESSAGES_FRAME, _("&Show/Hide Messages Window"), _("Toggle the messages window"));
	menuTools->Append(MID_BENCHMARKS, _("&Benchmarks...\tCTRL+B"), _("Open the benchmarks dialog"));
	menuTools->Append(MID_UPDATEPROJECTS, _("&Download New Projects"), _("Update the local project database"));
	//menu->Append(MID_CREATE_DEBUG_REPORT, _("Create debug report"), _("Create a debug state report to send to the developers"));
	#ifdef __WXMAC__
	menuTools->AppendSeparator();
	menuTools->Append(MID_UPDATECHECK, _("&Check for update"), _("Check online for the latest version of FahMon"));
	#endif
	menuBar->Append(menuTools, _("&Tools"));

	// The 'Web' menu
	menuWeb = new wxMenu();
	#ifndef __WXMAC__
	menuWeb->Append(MID_WWWMYSTATS, _("&My Stats\tF2"), _("View the personal statistics for the selected client"));
	menuWeb->Append(MID_WWWJMOL, _("&Jmol\tF3"), _("View the current project on the Jmol website"));
	menuWeb->Append(MID_WWWFAHINFO, _("fah&info\tF4"), _("View the current project on fahinfo.org"));
	#else
	menuWeb->Append(MID_WWWMYSTATS, _("&My Stats\tCtrl+Shift+M"), _("View the personal statistics for the selected client"));
	menuWeb->Append(MID_WWWJMOL, _("&Jmol\tCtrl+J"), _("View the current project on the Jmol website"));
	menuWeb->Append(MID_WWWFAHINFO, _("fah&info\tCtrl+I"), _("View the current project on fahinfo.org"));
	#endif
	menuWeb->AppendSeparator();
	menuWeb->Append(MID_WWWFOLDING, _("F@H &Website"), _("Open to the official Stanford website"));
	menuWeb->Append(MID_WWWFCORG, _("Folding &Forum"), _("Open the Folding@Home support forum"));
	menuWeb->Append(MID_WWWPROJECTS, _("&Projects Summary"), _("Open the list of the current projects"));
	menuWeb->Append(MID_WWWSERVERS, _("&Servers Status"), _("Open the list of the servers with their status"));
	menuBar->Append(menuWeb, _("&Web"));

	// The 'Help' menu
	menuHelp = new wxMenu();
	#ifndef __WXMAC__
	menuHelp->Append(wxID_HELP, _("&Help Contents\tF1"), _("See help contents"));
	#else
	menuHelp->Append(wxID_HELP, _("&Help Contents\tCtrl+?"), _("See help contents"));
	#endif
	menuHelp->Append(MID_WWWFAHMONIRC, _("FahMon &IRC Channel"), _("Join the FahMon IRC channel for online help"));
	menuHelp->Append(wxID_ABOUT, _("&About"), wxString::Format(_("About %s"), _T(FMC_APPNAME)));
#ifdef __WXMAC__
	{
		//wxApp::s_macHelpMenuTitleName = _("&Help");
	    wxApp::s_macAboutMenuItemId = wxID_ABOUT;
	}
#endif
	menuBar->Append(menuHelp, _("&Help"));

	SetMenuBar(menuBar);
}


void MainDialog::CreateLayout(void)
{
	wxPanel          *topLevelPanel;
	wxBoxSizer       *mainSizer;
	wxBoxSizer       *topSizer;
	wxBoxSizer       *midSizer;
	wxBoxSizer       *userinfoSizer;
	wxGridSizer      *infoSizer;
	wxStaticBoxSizer *topRightSizer;
#ifdef __WXMAC__
 	wxSystemOptions::SetOption(_T("mac.listctrl.always_use_generic"), 1);
#endif
	// We need to use a panel as a top level component in our frame
	// Without that, the frame looks ugly under Windows (dark grey background)
	topLevelPanel = new wxPanel(this, wxID_ANY);

	// A splitter is used to separate the ListView and the WU information, so that the user can choose the size
	mSplitterWindow = new wxSplitterWindow(topLevelPanel, wxID_ANY);

	// --- The top left part
	// It is simply composed by the list of clients
	mClientsList = new ListViewClients(mSplitterWindow, LST_CLIENTS, ClientsManager::GetInstance()->GetCount());

	// --- The top right part
	// It contains labels to display information on the currently selected client
	mTopRightPanel     = new wxPanel(mSplitterWindow, wxID_ANY);
	topRightSizer      = new wxStaticBoxSizer(wxVERTICAL, mTopRightPanel, _("Work Unit Information"));
	infoSizer          = new wxGridSizer(2, FMC_GUI_SPACING_HIGH, FMC_GUI_SPACING_LOW);
	userinfoSizer      = new wxBoxSizer(wxHORIZONTAL);
	mCoreName          = new StaticBoldedText(mTopRightPanel, wxID_ANY, _T(""));
	mProjectId         = new wxStaticText(mTopRightPanel, wxID_ANY, _T(""));
	mCredit            = new wxStaticText(mTopRightPanel, wxID_ANY, _T(""));
	mDownloaded        = new wxStaticText(mTopRightPanel, wxID_ANY, _T(""));
	mPreferredDeadline = new wxStaticText(mTopRightPanel, wxID_ANY, _T(""));
	mFinalDeadline     = new wxStaticText(mTopRightPanel, wxID_ANY, _T(""));

	// User's information: username and team number
	mUsername    =  new wxHyperlinkCtrl(mTopRightPanel, wxID_ANY, _T(""), _T(""));
	mUsername->SetNormalColour(*wxBLUE);
	mUsername->SetHoverColour(*wxRED);
	mUsername->SetVisitedColour(*wxBLUE);
	mTeamNumber   = new wxHyperlinkCtrl(mTopRightPanel, wxID_ANY, _T(""), _T(""));
	mTeamNumber->SetNormalColour(*wxBLUE);
	mTeamNumber->SetHoverColour(*wxRED);
	mTeamNumber->SetVisitedColour(*wxBLUE);

	userinfoSizer->Add(mUsername);
	userinfoSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	userinfoSizer->Add(mTeamNumber);

	// Emphasize the name of the core
	mCoreName->SetForegroundColour(*wxRED);

	// Information on the current client is stored using a GridSizer
	infoSizer->Add(new StaticBoldedText(mTopRightPanel, wxID_ANY, _("Core:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mCoreName, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(mTopRightPanel, wxID_ANY, _("Project:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mProjectId, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(mTopRightPanel, wxID_ANY, _("Credit:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mCredit, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(mTopRightPanel, wxID_ANY, _("Username:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(userinfoSizer, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(mTopRightPanel, wxID_ANY, _("Downloaded:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mDownloaded, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(mTopRightPanel, wxID_ANY, _("Preferred Deadline:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mPreferredDeadline, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(mTopRightPanel, wxID_ANY, _("Final Deadline:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mFinalDeadline, 0, wxALIGN_LEFT);

	// We use AddStretchSpacer() to keep the information vertically centered
	topRightSizer->SetMinSize(300, 0);
	topRightSizer->AddStretchSpacer();
	topRightSizer->Add(infoSizer, 0, wxALIGN_CENTER);
	topRightSizer->AddStretchSpacer();

	mTopRightPanel->SetSizer(topRightSizer);

	// --- The top part
	// It contains:
	//  - The list of clients on the left side, which takes the available left space
	//  - Information on the selected client on the right side, with a fixed size
	topSizer = new wxBoxSizer(wxHORIZONTAL);

	topSizer->Add(mSplitterWindow, 1, wxEXPAND);
	mSplitterWindow->SplitVertically(mClientsList, mTopRightPanel);

	// --- The middle part
	// It contains the progress bar (gauge) and a simple label
	// The label keeps its minimum size, while the gauge fill the available left space
	midSizer         = new wxBoxSizer(wxHORIZONTAL);
	#ifndef __WXMAC__
	mWUProgressGauge = new wxGauge(topLevelPanel, wxID_ANY, 100, wxDefaultPosition, wxSize(0, 18));
	#else
	mWUProgressGauge = new OSXProgressBar(topLevelPanel, wxID_ANY, 100, wxDefaultPosition, wxSize(0, 18));
	#endif
	mWUProgressText  = new wxStaticText(topLevelPanel, wxID_ANY, _T(""));

	midSizer->Add(mWUProgressGauge, 1);
	midSizer->Add(mWUProgressText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

	// --- The bottom part : Log file
	mLogFile = new wxTextCtrl(topLevelPanel, wxID_ANY, _("Log file."), wxDefaultPosition, wxSize(100, FMC_GUI_LOG_HEIGHT), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

	// -- Finally construct the frame itself
	// Both the top part and the log area will expand vertically, a little more quickly for the log (4/3 ratio)
	// The middle part keeps a fixed size
	mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(topSizer, 1, wxEXPAND);
	mainSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	mainSizer->Add(midSizer, 0, wxEXPAND);
	mainSizer->Add(mLogFile, 0, wxEXPAND | wxTOP, FMC_GUI_SPACING_LOW);

	// That's done, we can finally add the top level sizer to the panel
	mTopLevelSizer = new wxBoxSizer(wxVERTICAL);

	mTopLevelSizer->Add(mainSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);

	topLevelPanel->SetSizer(mTopLevelSizer);
}


void MainDialog::RestoreFrameState(void)
{
	bool     isLogShown;
	wxInt32  displayWidth;
	wxInt32  displayHeight;
	wxInt32  framePosX;
	wxInt32  framePosY;
	wxInt32  frameWidth;
	wxInt32  frameHeight;
	wxUint32 sashPosition;
	bool     max;
	bool     isInfoPanelShown;

	// --- State of the log
	_PrefsGetBool(PREF_MAINDIALOG_SHOWLOG, isLogShown);

	// By default the log is shown, so we hide it only if we should
	// We must not call ToggleLog(), because it resizes the frame and the current size is the correct (restored) one
	if(isLogShown == false)
	{
		mLogFile->Show(false);
		mTopLevelSizer->Show(mLogFile, false);
	}
	mTopLevelSizer->Layout();

	// --- Splitter's state
	_PrefsGetUint(PREF_MAINDIALOG_SASHPOSITION, sashPosition);

	mSplitterWindow->SetMinimumPaneSize(20);
	mSplitterWindow->SetSashPosition(sashPosition, true);

	_PrefsGetBool(PREF_MAINDIALOG_SHOWINFOPANEL, isInfoPanelShown);
	if(!isInfoPanelShown)
	{
		mSplitterWindow->Unsplit();
		mWUProgressGauge->Show(false);
		mWUProgressText->Show(false);
	}

	// --- Size and position of the frame
	// Retrieve saved values
	_PrefsGetInt(PREF_MAINDIALOG_FRAMEWIDTH,  frameWidth);
	_PrefsGetInt(PREF_MAINDIALOG_FRAMEHEIGHT, frameHeight);

	_PrefsGetInt(PREF_MAINDIALOG_FRAME_POS_X, framePosX);
	_PrefsGetInt(PREF_MAINDIALOG_FRAME_POS_Y, framePosY);

	// Check if these values are correct for the current resolution, which can be different from the last time
	// If some values are not correct, they are changed to default ones
	wxDisplaySize(&displayWidth, &displayHeight);

	if(frameWidth >= displayWidth || frameHeight >= displayHeight || frameHeight <= 0 || frameWidth <= 0 )
	{
		// -1 indicates defaults values
		frameWidth  = -1;
		frameHeight = -1;
	}

	if(framePosX >= displayWidth || framePosY >= displayHeight || framePosX <= 0 || framePosY <= 0 )
	{
		// -1 indicates defaults values
		framePosX = -1;
		framePosY = -1;
	}

	_PrefsGetBool(PREF_MAINDIALOG_MAXIMISE, max);

	// We now have correct values, resize and move the frame
	SetSize(frameWidth, frameHeight);
	Move(framePosX, framePosY);
	if(max)
	{
		Maximize(true);
		TrayManager::GetInstance()->SetMaximised(true);
	}
}


/************************************  EVENTS  ************************************/


void MainDialog::OnMenuQuit(wxCommandEvent& event)
{
	Close(true);
}


void MainDialog::OnMenuReload(wxCommandEvent& event)
{
	ClientId selectedClientId = mClientsList->GetSelectedClientId();

	if(selectedClientId != INVALID_CLIENT_ID)
	{
		ClientsManager::GetInstance()->ReloadThreaded(selectedClientId);
	}
}


void MainDialog::OnMenuReloadAll(wxCommandEvent& event)
{
	ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
	WebMonitor::GetInstance()->WriteApp();
}


void MainDialog::OnMenuUpdateProjects(wxCommandEvent& event)
{
	bool updateResult;

	// The user asked to update the database, so we force this update and we are not doing it silently
	updateResult = ProjectsManager::GetInstance()->UpdateDatabase(true, false);

	// If no error occurred while updating the database, we can then update the displayed information about clients
	if(updateResult == true)
	{
		ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
	}
}


void MainDialog::OnMenuToggleLog(wxCommandEvent& event)
{
	bool          isLogShown;
	ClientId      selectedClientId;
	const Client *selectedClient;
	//mb The frame border at the bottom of the log control must also be taken into account when resizing
	const wxUint32 frameResizingOffset = FMC_GUI_LOG_HEIGHT + FMC_GUI_SPACING_LOW;

	// Show/Hide the log area
	mWasMaximised = IsMaximized();
	isLogShown = !mLogFile->IsShown();
	mLogFile->Show(isLogShown);
	mTopLevelSizer->Show(mLogFile, isLogShown);
	mTopLevelSizer->Layout();

	// Resize the frame
	if(isLogShown)
	{
		SetSize(-1, GetSize().GetHeight() + frameResizingOffset);
	}
	else
	{
		if(!mWasMaximised)
			SetSize(-1, GetSize().GetHeight() - frameResizingOffset);
	}

	// Load the correct log file (if any) when we are going to show the wxTextCtrl
	selectedClientId = mClientsList->GetSelectedClientId();
	if(isLogShown && selectedClientId != INVALID_CLIENT_ID)
	{
		selectedClient = ClientsManager::GetInstance()->Get(selectedClientId);

		mLogFile->SetValue(selectedClient->GetLog());
		mLogFile->ShowPosition(mLogFile->GetLastPosition());
	}
}


void MainDialog::OnMenuToggleMessagesFrame(wxCommandEvent& event)
{
	MessagesFrame::GetInstance(this)->Toggle();
}


void MainDialog::OnMenuBenchmarks(wxCommandEvent& event)
{
	ClientId  selectedClientId;
	ProjectId selectedProjectId;

	// Select the project currently processed by the selected client
	selectedClientId = mClientsList->GetSelectedClientId();

	if(selectedClientId == INVALID_CLIENT_ID)
	{
		selectedProjectId = INVALID_PROJECT_ID;
	}
	else
	{
		selectedProjectId = ClientsManager::GetInstance()->Get(selectedClientId)->GetProjectId();
	}

	// Ok, display the dialog
	BenchmarksDialog::GetInstance(this)->ShowModal(selectedProjectId);
}


void MainDialog::OnMenuPreferences(wxCommandEvent& event)
{
	PreferencesDialog::GetInstance(this)->ShowModal();
}


void MainDialog::OnMenuWeb(wxCommandEvent& event)
{
	ClientId selectedClientId;

	// Determine the url to go to
	switch(event.GetId())
	{
		//--
		case MID_WWWJMOL:
		case MID_WWWMYSTATS:
		case MID_WWWFAHINFO:
			// For these two menus, a client must be selected
			selectedClientId = mClientsList->GetSelectedClientId();
			if(selectedClientId != INVALID_CLIENT_ID)
			{
				if(event.GetId() == MID_WWWJMOL)
				{
					Tools::OpenURLInBrowser(ClientsManager::GetInstance()->Get(selectedClientId)->GetJmolURL());
				}
				else if(event.GetId() == MID_WWWFAHINFO)
				{
					Tools::OpenURLInBrowser(ClientsManager::GetInstance()->Get(selectedClientId)->GetFahinfoURL());
				}
				else
				{
					Tools::OpenURLInBrowser(ClientsManager::GetInstance()->Get(selectedClientId)->GetDonatorStatsURL());
				}
			}
			else
			{
				Tools::ErrorMsgBox(_("You must first select a client!"));
			}
			break;

		//--
		case MID_WWWFOLDING:
			Tools::OpenURLInBrowser(_T(FMC_URL_FOLDING));
			break;

		//--
		case MID_WWWFCORG:
			Tools::OpenURLInBrowser(_T(FMC_URL_FCORG));
			break;

		//--
		case MID_WWWPROJECTS:
			Tools::OpenURLInBrowser(_T(FMC_URL_PROJECTS));
			break;

		//--
		case MID_WWWSERVERS:
			Tools::OpenURLInBrowser(_T(FMC_URL_SERVERS));
			break;

		//--
		case wxID_HELP:
			Tools::OpenURLInBrowser(_T(FMC_URL_HELP));
			break;

		//--
		case MID_WWWFAHMONIRC:
			Tools::OpenURLInBrowser(_T(FMC_URL_FAHMON_IRC));
			break;

		//--
		default:
			// We should never fall here
			wxASSERT(false);
			break;
	}
}


void MainDialog::OnMenuAbout(wxCommandEvent& event)
{
	AboutDialog::GetInstance(this)->ShowModal();
}


void MainDialog::OnClose(wxCloseEvent& event)
{
	// Don't save window size if it is iconized (win32 bug)
#ifdef _FAHMON_WIN32_
	if(!IsIconized())
#endif
	{
		if( !IsMaximized())
		{
			// Save the size of the frame
			_PrefsSetInt(PREF_MAINDIALOG_FRAMEWIDTH,  GetSize().GetWidth());
			_PrefsSetInt(PREF_MAINDIALOG_FRAMEHEIGHT, GetSize().GetHeight());

			// Save the position of the frame
			_PrefsSetInt(PREF_MAINDIALOG_FRAME_POS_X, GetPosition().x);
			_PrefsSetInt(PREF_MAINDIALOG_FRAME_POS_Y, GetPosition().y);
		}

		_PrefsSetBool(PREF_MAINDIALOG_MAXIMISE, IsMaximized());
	}

	// Save the position of the sash
	_PrefsSetUint(PREF_MAINDIALOG_SASHPOSITION, mSplitterWindow->GetSashPosition());

	// Save the state of the log
	_PrefsSetBool(PREF_MAINDIALOG_SHOWLOG, mLogFile->IsShown());

	_PrefsSetBool(PREF_MAINDIALOG_SHOWINFOPANEL, mSplitterWindow->IsSplit());

	// Delete any dialog we could have displayed / Objects we could have used
	TrayManager::DestroyInstance();
	AboutDialog::DestroyInstance();
	ClientDialog::DestroyInstance();
	MessagesFrame::DestroyInstance();
	BenchmarksDialog::DestroyInstance();
	PreferencesDialog::DestroyInstance();

	// Stop the application
	Destroy();
}


void MainDialog::OnIconize(wxIconizeEvent& event)
{
  TrayManager::GetInstance()->SetMaximised(IsMaximized());
	if(event.Iconized())
	{
		bool isTrayIconEnabled;

		_PrefsGetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON, isTrayIconEnabled);

		if(isTrayIconEnabled)
		{
			Hide();
		}
	}
}

bool MainDialog::ClientReloadAllowed(ClientId clientId)
{
	wxMutexLocker lock(mMutexArrayBlocker);
	int currentThread = s_clientThreadsArray.Index(clientId);
	/* We didn't find our clientID, so no thread is currently running for that client */
	if(currentThread == wxNOT_FOUND)
	{
		s_clientThreadsArray.Add(clientId);
		s_clientThreadsArray.Item(s_clientThreadsArray.GetCount() - 1) = clientId;
		return true;
	}
	else
	{
		return false;
	}
}

void MainDialog::OnListSelectionChanged(wxListEvent& event)
{
	ShowClientInformation(mClientsList->GetSelectedClientId());
}

void MainDialog::OnClientReloaded(wxCommandEvent& event)
{
	ClientId clientId = event.GetInt();

	// The ListView must be updated, regardless of the current selection
	mClientsList->UpdateClient(clientId);

	wxMutexLocker lock(mMutexArrayBlocker);
	s_clientThreadsArray.Remove(clientId);

	// However, other information must be updated only if the client is the currently selected one
	if(clientId == mClientsList->GetSelectedClientId())
	{
		ShowClientInformation(clientId);
	}
	SetStatusText(wxString::Format(_("%.2f PPD"), MainDialog::GetTotalPPD()), STATUS_TOTALPPD);
}


void MainDialog::OnNewClientAdded(wxCommandEvent& event)
{
	wxUint32 i;
	ClientId clientId;

	// Reset the list to have the correct number of clients
	mClientsList->Reset(ClientsManager::GetInstance()->GetCount());

	// Clear displayed information, as the list has been reseted, nothing is selected
	ShowClientInformation(INVALID_CLIENT_ID);

	// Re-display all clients, except the new one (not yet loaded)
	clientId = (wxUint32)event.GetInt();
	for(i=0; i<ClientsManager::GetInstance()->GetCount(); ++i)
	{
		if(i != clientId)
		{
			mClientsList->UpdateClient(i);
		}
	}

	// Finally load the new one
	// It will be automatically displayed when done
	ClientsManager::GetInstance()->ReloadThreaded(clientId);
}


void MainDialog::OnClientDeleted(wxCommandEvent& event)
{
	wxUint32 i;

	// Reset the list to have the correct number of clients
	mClientsList->Reset(ClientsManager::GetInstance()->GetCount());

	// Clear displayed information, as the list has been reseted, nothing is selected
	ShowClientInformation(INVALID_CLIENT_ID);

	// Re-display all left clients
	for(i=0; i<ClientsManager::GetInstance()->GetCount(); ++i)
	{
		mClientsList->UpdateClient(i);
	}

}


void MainDialog::OnProjectsDatabaseUpdated(wxCommandEvent& event)
{
	// New projects are perhaps available, so we reload all clients to update the displayed information
	ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
}


void MainDialog::OnNewMessageLogged(wxCommandEvent& event)
{
	MessagesFrame::OnNewMessage();
}


void MainDialog::OnAutoReloadTimer(wxTimerEvent& event)
{
	bool isAdvancedReloadOn;

	_PrefsGetBool(PREF_MAINDIALOG_ADVANCEDRELOAD, isAdvancedReloadOn);

	if(event.GetId() == MID_LOCALTIMER)
	{
		_LogMsgInfo(wxT("Triggered auto-reload of local clients"), false);
		if(isAdvancedReloadOn == true)
		{
			// Reload using experimental method testing last update time
			ClientsManager::GetInstance()->ReloadThreaded(CM_LOADLOCAL);
		}
		else
		{
			// Reload using old method - forces an upload regardless of changes
			ClientsManager::GetInstance()->ReloadThreaded(CM_LOADLOCALF);
		}
	}
	else
	{
		_LogMsgInfo(wxT("Triggered auto-reload of inet clients"), false);
		ClientsManager::GetInstance()->ReloadThreaded(CM_LOADINETF);
		WebMonitor::GetInstance()->WriteApp();
	}
}


void MainDialog::OnAutoReloadPrefChanged(void)
{
	SetAutoReloadTimer();
}


void MainDialog::OnETAStylePrefChanged(void)
{
	mClientsList->UpdateAllClients();
}


void MainDialog::OnTrayIconPrefChanged(void)
{
	bool isTrayIconEnabled;

	// Check the new value of the preference
	_PrefsGetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON, isTrayIconEnabled);

	// And install/uninstall the icon as needed
	if(isTrayIconEnabled)
	{
		TrayManager::GetInstance()->InstallIcon();
	}
	else
	{
		TrayManager::GetInstance()->UninstallIcon();
	}
}


void MainDialog::OnPPDStylePrefChanged(void)
{
	ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
}


void MainDialog::OnDeadlinePrefChanged(void)
{
	ClientId selectedClientId = mClientsList->GetSelectedClientId();

	if(selectedClientId != INVALID_CLIENT_ID)
	{
		ClientsManager::GetInstance()->ReloadThreaded(selectedClientId);
	}
}


double MainDialog::GetTotalPPD(void) const
{
	wxString  test;
	double  tmpdouble;
	double  TotalPPD;
	wxInt32	i;

	TotalPPD = 0;

	for(i=0; i<mClientsList->GetItemCount(); ++i)
	{
		test = mClientsList->GetCellContentsString(i,5);
		test.ToDouble(&tmpdouble);
		TotalPPD = TotalPPD + tmpdouble;
	}

	return TotalPPD;
}


wxInt32 MainDialog::GetClientCount(void) const
{
	return mClientsList->GetItemCount();
}


void MainDialog::TrayReloadSelectedClient(void)
{
	ShowClientInformation(mClientsList->GetSelectedClientId());
}


void MainDialog::OnMenuToggleETADate(wxCommandEvent& event)
{
	wxUint32 etaStyle;

	_PrefsGetUint(PREF_ETA_DISPLAYSTYLE, etaStyle);
	switch(etaStyle)
	{
		case ETADS_DATE_DAY_MONTH:
			_PrefsSetUint(PREF_ETA_DISPLAYSTYLE, ETADS_DATE_MONTH_DAY);
			OnETAStylePrefChanged();
			break;

		case ETADS_DATE_MONTH_DAY:
			_PrefsSetUint(PREF_ETA_DISPLAYSTYLE, ETADS_LEFT_TIME);
			OnETAStylePrefChanged();
			break;

		case ETADS_LEFT_TIME:
			_PrefsSetUint(PREF_ETA_DISPLAYSTYLE, ETADS_DATE_DAY_MONTH);
			OnETAStylePrefChanged();
			break;

		default:
			break;
	}
	ShowClientInformation(mClientsList->GetSelectedClientId());
}


void MainDialog::OnUpdateCheck(wxCommandEvent& event)
{
	CheckForUpdates(false);
}


void MainDialog::CheckForUpdates(bool silent)
{
	wxMutexLocker   lock(mMutexUpdateCheck);        // --- Lock the access to this method
	wxString        updateFile;
	updateFile = wxFileName::CreateTempFileName(_T(FMC_APPNAME));
	wxString        errorMsg;
	wxString        versionInfo;
	wxTextFile      in;
	bool            updateAvailable = false;

	_LogMsgInfo(_("Checking for FahMon updates"), false);

	if(!HTTPDownloader::GetHTTPFile(wxT("http://fahmon.net/downloads/update.chk"), updateFile))
		return;

	if(!wxFileExists(updateFile) || !in.Open(updateFile))
	{
		return;
	}

	versionInfo = in.GetLine(in.GetLineCount()-1);
	_LogMsgInfo(wxString::Format(_("Your version: %s; New version: %s"), _T(FMC_VERSION), versionInfo.c_str()), false);

	if(versionInfo.Cmp(_T(FMC_VERSION)) > 0)
	{
		updateAvailable = true;
	}
	// --- We can finally delete the temporary file
	// This file must exist if we reached this part of the code
	wxASSERT(updateFile.IsEmpty() == false);
	wxRemoveFile(updateFile);
	in.Close();

	if(updateAvailable == true)
	{
		_LogMsgInfo(_("Update available"), false);
		if(Tools::QuestionMsgBox(wxString::Format(_("A newer version of FahMon is available!\nYour version: %s; New version: %s\nDo you want to go to the FahMon website?"), _T(FMC_VERSION), versionInfo.c_str())) == true)
		{
			Tools::OpenURLInBrowser(_T("http://fahmon.net/download.html"));
		}
	}
	else
	{
		_LogMsgInfo(_("No update found"), false);
		if (!silent)
			Tools::InfoMsgBox(_("No update found"));
	}
}


void MainDialog::OnMenuEditClient(wxCommandEvent& event)
{
	wxUint32 selectedClientId = mClientsList->GetSelectedClientId();

	// Ensure that something is really selected
	if(selectedClientId == INVALID_CLIENT_ID)
	{
		return;
	}

	// Ask the main dialog to edit this client
	ClientDialog::GetInstance(this)->ShowModal(selectedClientId, wxEmptyString);
}


void MainDialog::OnMenuDeleteClient(wxCommandEvent& event)
{
	wxUint32       selectedClientId;
	wxCommandEvent deleteEvent(EVT_CLIENTDELETED);

	// Ensure that something is really selected
	selectedClientId = mClientsList->GetSelectedClientId();
	if(selectedClientId == INVALID_CLIENT_ID)
	{
		return;
	}

	// Ensure that the user did not ask for deletion by error
	if(Tools::QuestionMsgBox(_("Do you really want to delete this client?")) == true)
	{
		// Delete the client
		ClientsManager::GetInstance()->Delete(selectedClientId);

		// And warn the main dialog
		AddPendingEvent(deleteEvent);
	}
}


void MainDialog::OnMenuViewFiles(wxCommandEvent& event)
{
	wxUint32       selectedClientId;
	selectedClientId = mClientsList->GetSelectedClientId();
	if(selectedClientId == INVALID_CLIENT_ID)
	{
		return;
	}
	mClientsList->ShowClientFiles();
}


void MainDialog::OnMenuToggleInfoPanel(wxCommandEvent& event)
{
	if(mSplitterWindow->IsSplit())
	{
		mSplitterWindow->Unsplit();
		mWUProgressGauge->Show(false);
		mWUProgressText->Show(false);
	}
	else
	{
		mSplitterWindow->SplitVertically(mClientsList, mTopRightPanel);
		mWUProgressGauge->Show(true);
		mWUProgressText->Show(true);
	}
	mTopLevelSizer->Layout();
}


void MainDialog::OnMenuCreateDebugReport(wxCommandEvent& event)
{
	wxGetApp().GenerateReport(wxDebugReport::Context_Current);
}


#ifdef __WXMAC__

BEGIN_EVENT_TABLE(OSXProgressBar, wxWindow)
	EVT_PAINT(OSXProgressBar::OnPaint)
	EVT_SIZE(OSXProgressBar::OnResize)
END_EVENT_TABLE()

OSXProgressBar::OSXProgressBar(wxWindow* parent, wxWindowID, wxUint32 total, const wxPoint& pos, const wxSize& size) : wxWindow(parent, wxID_ANY, pos, size, wxSUNKEN_BORDER)
{
	SetBackgroundColour(*wxWHITE);
	mTotal = total;
	mCurrentProgress = 0;
}

OSXProgressBar::~OSXProgressBar()
{
}

void OSXProgressBar::SetValue(wxUint32 value)
{
	mCurrentProgress = value;
	Refresh();
}

void OSXProgressBar::OnPaint(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);

	PaintBackground(dc);

	FillProgress(dc);
}

void OSXProgressBar::OnResize(wxSizeEvent& event)
{
	Refresh();
}

void OSXProgressBar::PaintBackground(wxDC& dc)
{
	wxColour bacgroundColour = *wxWHITE;

	dc.SetBrush(wxBrush(bacgroundColour));

	dc.Clear();
}

void OSXProgressBar::FillProgress(wxDC& dc)
{

	wxInt32 progress = ((float)mCurrentProgress / (float)mTotal) * GetClientSize().x;
	dc.SetPen(wxPen(*wxLIGHT_GREY, 1));
	dc.SetBrush(wxBrush(*wxLIGHT_GREY));

	dc.DrawRectangle(0,1,progress,GetClientSize().y);

}

#endif

#if 0
/**
 * Used to inject various required strings into the translation files.
**/
_("ctrl");
_("shift");
_("alt");
#endif
