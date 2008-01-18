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
#include "mainDialog.h"

#include "tools.h"
#include "client.h"
#include "wx/colour.h"
#include "wx/filefn.h"
#include "wx/textfile.h"
#include "wx/hyperlink.h"
#include "trayManager.h"
#include "aboutDialog.h"
#include "pathManager.h"
#include "clientDialog.h"
#include "messagesFrame.h"
#include "clientsManager.h"
#include "httpDownloader.h"
#include "messagesManager.h"
#include "projectsManager.h"
#include "staticBoldedText.h"
#include "benchmarksDialog.h"
#include "preferencesDialog.h"
#include "preferencesManager.h"
#include "listViewClients.h"


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
	MID_PREFERENCES,
	MID_WWWJMOL,
	MID_WWWMYSTATS,
	MID_WWWFOLDING,
	MID_WWWFCORG,
	MID_WWWPROJECTS,
	MID_WWWSERVERS,
	MID_WWWFAHINFO,
 	MID_UPDATECHECK,

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


// Indexes of the miscellaneous parts of the status bar
enum _STATUSBAR_PART
{
	STATUS_UNUSED,
	STATUS_CLIENTNAME
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
	EVT_MENU    (MID_PREFERENCES,           MainDialog::OnMenuPreferences)
	EVT_MENU    (MID_WWWJMOL,               MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWFAHINFO,            MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWMYSTATS,            MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWFOLDING,            MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWFCORG,              MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWPROJECTS,           MainDialog::OnMenuWeb)
	EVT_MENU    (MID_WWWSERVERS,            MainDialog::OnMenuWeb)
	EVT_MENU    (wxID_HELP_CONTENTS,        MainDialog::OnMenuWeb)
	EVT_MENU    (wxID_ABOUT,                MainDialog::OnMenuAbout)
	EVT_MENU    (MID_UPDATECHECK,           MainDialog::OnUpdateCheck)

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


/**
* Constructor
**/
MainDialog::MainDialog(void) : wxFrame(NULL, wxID_ANY, wxT(FMC_PRODUCT))
{
	bool trayIconEnabled;
	bool updateCheck;

	// Setting the icon for the main dialog will allows child frames and dialog to inherit from it

#ifdef __WXGTK__
	SetIcon(wxIcon(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_DIALOG)));
#elif _FAHMON_WIN32_
	SetIcon(wxICON(dialog_icon));
#endif

	CreateMenuBar();
	CreateStatusBar(2);
	CreateLayout();
	RestoreFrameState();     // MUST be called when all controls have been created !

	_LogMsgInfo(wxString::Format(_("%s is licenced under the GNU GPL v2"), wxT(FMC_PRODUCT)));
	_LogMsgInfo(_("Copyright (C) 2003-2007 Francois Ingelrest"));
	_LogMsgInfo(_("Copyright (C) 2007-2008 Andrew Schofield"));
	_LogMsgInfo(_T(""));
	_LogMsgInfo(_("FahMon contains code from qd, also licenced under the GNU GPL v2"));
	_LogMsgInfo(_("Copyright (C) 2002-2005 Richard P. Howell IV."));
	_LogMsgInfo(_("Copyright (C) 2005-2008 Sebastiaan Couwenberg"));
	_LogMsgInfo(_T(""));
	// this needs to be before the tray icon, otherwise you can still click the tray icon and crash FahMon
	_PrefsGetBool(PREF_MAINDIALOG_UPDATE_CHECK, updateCheck);
	if(updateCheck == true)
		CheckForUpdates();
	_LogMsgInfo(_T(""));

	// The timer used for auto-reloading
	mAutoReloadTimer.SetOwner(this);
	SetAutoReloadTimer();

	// The tray icon
	_PrefsGetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON, trayIconEnabled);
	if(trayIconEnabled == true)
		TrayManager::GetInstance()->InstallIcon();

}


/**
* Destructor
**/
MainDialog::~MainDialog(void)
{
}


/**
* Create the single instance of the MainDialog
**/
void MainDialog::CreateInstance(void)
{
	wxASSERT(mInstance == NULL);

	mInstance = new MainDialog();
}


/**
* Destroy the single instance of the MainDialog
**/
void MainDialog::DestroyInstance(void)
{
	wxASSERT(mInstance != NULL);

	delete mInstance;
	mInstance = NULL;
}


/**
* Check whether this dialog has been already instanciated
**/
bool MainDialog::HasBeenInstanciated(void)
{
	return mInstance != NULL;
}


/**
* Return the single instance of the MainDialog
**/
MainDialog* MainDialog::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}


/**
* Redefine the wxFrame::Show() method in order to perform some initializations before displaying the frame
**/
bool MainDialog::Show(bool show)
{
	// Showing the frame must be done before selecting a client, or the sash position won't be restored
	bool result = wxFrame::Show(show);

	if(show == true)
	{
		// Items surely won't keep their order when they will be loaded because of sorting, so selecting a
		// default one is useless if there are more than one client
		// Of course, in the case of only one client, we can still select it
		if(ClientsManager::GetInstance()->GetCount() == 1)
			mClientsList->Select(0);
		else
			ShowClientInformation(INVALID_CLIENT_ID);

		ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
	}

	return result;
}


/**
* Create/Kill/Modify the timer for auto-reloading, according to the preferences
**/
void MainDialog::SetAutoReloadTimer(void)
{
	bool     isAutoReloadOn;
	bool     isAdvancedReloadOn;
	wxUint32 autoReloadFrequency;

	// First, we have to stop the timer if it is running, because preferences may be different from the last time
	if(mAutoReloadTimer.IsRunning())
		mAutoReloadTimer.Stop();

	// Then, retrieve the (perhaps new) preferences
	_PrefsGetBool(PREF_MAINDIALOG_AUTORELOAD,          isAutoReloadOn);
	_PrefsGetBool(PREF_MAINDIALOG_ADVANCEDRELOAD,      isAdvancedReloadOn); //this is actually the experimental system
	_PrefsGetUint(PREF_MAINDIALOG_AUTORELOADFREQUENCY, autoReloadFrequency);

	// Ok, now we can start the timer if needed but we need to convert minutes to milliseconds
	if(isAutoReloadOn)
	{
		if(isAdvancedReloadOn)
		{
			mAutoReloadTimer.Start(10 * 1000/*autoReloadFrequency * 60 * 1000*/); //experimental updates
		}
		else
		{
			mAutoReloadTimer.Start(autoReloadFrequency * 60 * 1000); //regular update system
		}
	}
}


/**
* Update the fields using the given client and refresh the GUI
**/
void MainDialog::ShowClientInformation(ClientId clientId)
{
	UpdateClientInformation(clientId);
	mTopLevelSizer->Layout();

	// win32: The layout of the sizer which contains the two StaticUrl objects does not seem
	// to be updated by the previous Layout() call, so we need to do it ourselves
	mUsername->GetContainingSizer()->Layout();
}


/**
* Update the fields using the given client
* This method does not affect the ListView
**/
void MainDialog::UpdateClientInformation(ClientId clientId)
{
	bool          overrideTZ;
	bool          autoUpdateProjects;
	wxUint32      deadlineDays;
	wxInt32       TZ;
	wxDateTime    preferredDeadline;
	wxDateTime    finalDeadline;
	wxDateTime    downloadTime;
	wxDateTime    timeNow;
	wxTimeSpan    timeDiff;
	const Client  *client;
	const Project *project;
	wxInt32       timeInMinutes;
	wxInt32       nbDays;
	wxInt32       nbHours;
	wxInt32       nbMinutes;
	wxString      tempString;

	_PrefsGetBool(PREF_OVERRIDE_TIMEZONE, overrideTZ);
	_PrefsGetInt (PREF_TZ,                TZ);
	_PrefsGetUint(PREF_ETA_DISPLAYSTYLE, deadlineDays);

	// Clear information for invalid clients
	if(clientId == INVALID_CLIENT_ID)
	{
		mUsername->Disable();
		mUsername->SetURL(wxT(""));
		mUsername->SetLabel(wxT(""));
		mTeamNumber->Disable();
		mTeamNumber->SetURL(wxT(""));
		mTeamNumber->SetLabel(wxT(""));

		mCoreName->SetLabel(wxT(""));
		mProjectId->SetLabel(wxT(""));
		mCredit->SetLabel(wxT(""));
		mDownloaded->SetLabel(wxT(""));
		mPreferredDeadline->SetLabel(wxT(""));
		mFinalDeadline->SetLabel(wxT(""));
		mWUProgressGauge->SetValue(0);
		mWUProgressText->SetLabel(wxT(""));
		SetStatusText(wxT(""), STATUS_CLIENTNAME);

		if(mLogFile->IsShown() == true)
			mLogFile->SetValue(wxT(""));

		return;
	}

	client = ClientsManager::GetInstance()->Get(clientId);

	// This method cannot be called for 'ghost' clients, if this is the case, then there's a big problem
	wxASSERT(client != NULL);

	SetStatusText(client->GetLocation(), STATUS_CLIENTNAME);

	// Clear information if this client is not a valid one
	if(!client->IsAccessible())
	{
		mUsername->Disable();
		mUsername->SetURL(wxT(""));
		mUsername->SetLabel(wxT(""));
		mTeamNumber->Disable();
		mTeamNumber->SetURL(wxT(""));
		mTeamNumber->SetLabel(wxT(""));

		mCoreName->SetLabel(wxT(""));
		mProjectId->SetLabel(wxT(""));
		mCredit->SetLabel(wxT(""));
		mDownloaded->SetLabel(wxT(""));
		mPreferredDeadline->SetLabel(wxT(""));
		mFinalDeadline->SetLabel(wxT(""));
		mWUProgressGauge->SetValue(0);
		mWUProgressText->SetLabel(wxT(""));
		mLogFile->SetValue(_("Something is wrong with this client.\nPlease check the messages (FahMon->Show/Hide Messages Window)."));

		return;
	}

	// Load the log only if it is not hidden, it's useless otherwise
	if(mLogFile->IsShown())
	{
		// Arghhhhh, it takes *ages* on wxGTK to load the log file within the wxTextCtrl!!
		mLogFile->SetValue(client->GetLog());
		mLogFile->ShowPosition(mLogFile->GetLastPosition());
	}

	mWUProgressText->SetLabel(wxT("  ") + client->GetProgressString());
	mWUProgressGauge->SetValue(client->GetProgress());

	mUsername->Enable();
	mUsername->SetLabel(client->GetDonatorName());
	mUsername->SetURL(client->GetDonatorStatsURL());

	mTeamNumber->Enable();
	mTeamNumber->SetLabel(wxString::Format(wxT("(%u)"), client->GetTeamNumber()));
	mTeamNumber->SetURL(client->GetTeamStatsURL());

	if(client->GetDownloadDate().IsValid())
	{
		if(overrideTZ)
		{
			downloadTime = client->GetDownloadDate().Add(wxTimeSpan::Hours(TZ));
			timeNow = wxDateTime::Now()/*.Add(wxTimeSpan::Hours(TZ))*/;
		}
		else
		{
			downloadTime = client->GetDownloadDate().FromTimezone(wxDateTime::UTC);
			timeNow = wxDateTime::Now()/*.FromTimezone(wxDateTime::UTC)*/;
		}
		if(deadlineDays == ETADS_LEFT_TIME)
		{
			timeDiff = timeNow.Subtract(downloadTime);
			timeInMinutes = timeDiff.GetMinutes();

			// Split the left time into days, hours and minutes
			nbDays    = timeInMinutes / (24 * 60);
			nbMinutes = timeInMinutes % (24 * 60);
			nbHours   = nbMinutes / 60;
			nbMinutes = nbMinutes % 60;
			// Use a friendly format
			if(nbDays != 0)
				tempString = wxString::Format(wxT("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
			else if(nbHours != 0)
				tempString = wxString::Format(wxT("%ih %02imn"), nbHours, nbMinutes);
			else
				tempString = wxString::Format(wxT("%imn"), nbMinutes);

			mDownloaded->SetLabel(wxString::Format(_("%s ago"), tempString.c_str()));
		}
		else if (deadlineDays == ETADS_DATE_DAY_MONTH)
		{
			mDownloaded->SetLabel(wxString::Format(wxT("%s"), downloadTime.Format(wxT("%d %B, %H:%M")).c_str()));
		} else
		{
			mDownloaded->SetLabel(wxString::Format(wxT("%s"), downloadTime.Format(wxT("%B %d, %H:%M")).c_str()));
		}
	}
	else
		mDownloaded->SetLabel(_("N/A"));

	if(client->GetProjectId() == INVALID_PROJECT_ID)
	{
		mProjectId->SetLabel(_("N/A"));
		mCoreName->SetLabel(_("N/A"));
		mCredit->SetLabel(_("N/A"));
		mPreferredDeadline->SetLabel(_("N/A"));
		mFinalDeadline->SetLabel(_("N/A"));

		return;
	}

	mProjectId->SetLabel(wxString::Format(wxT("%u (R%i, C%i, G%i)"), client->GetProjectId(), client->GetProjectRun(), client->GetProjectClone(), client->GetProjectGen()));
	project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());


	_PrefsGetBool(PREF_MAINDIALOG_AUTOUPDATEPROJECTS, autoUpdateProjects);

	// This project can be unknown, if the database is not up to date
	if(project == NULL && autoUpdateProjects){
		  ProjectsManager::GetInstance()->UpdateDatabase(false, false);
		  project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());
		  ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
	}
	if(project == NULL)
	{
		mCoreName->SetLabel(_("Unknown"));
		mCredit->SetLabel(_("Unknown"));
		mPreferredDeadline->SetLabel(_("Unknown"));
		mFinalDeadline->SetLabel(_("Unknown"));
		_LogMsgWarning(wxString::Format(_("Project %u is unknown, you should try to update the projects database"), client->GetProjectId()));
	} else {

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
				if(timeDiff.GetMinutes() < 0) timeInMinutes = 0 - timeInMinutes;

				// Split the left time into days, hours and minutes
				nbDays    = timeInMinutes / (24 * 60);
				nbMinutes = timeInMinutes % (24 * 60);
				nbHours   = nbMinutes / 60;
				nbMinutes = nbMinutes % 60;
				// Use a friendly format
				if(nbDays != 0)
					tempString = wxString::Format(wxT("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
				else if(nbHours != 0)
					tempString = wxString::Format(wxT("%ih %02imn"), nbHours, nbMinutes);
				else
					tempString = wxString::Format(wxT("%imn"), nbMinutes);

				if(timeDiff.GetMinutes() < 0)
					mPreferredDeadline->SetLabel(wxString::Format(_("%s ago"), tempString.c_str()));
				else
					mPreferredDeadline->SetLabel(wxString::Format(_("In %s"), tempString.c_str()));

			}
			else if (deadlineDays == ETADS_DATE_DAY_MONTH)
			{
				mPreferredDeadline->SetLabel(wxString::Format(wxT("%s"), preferredDeadline.Format(wxT("%d %B, %H:%M")).c_str()));
			} else
			{
				mPreferredDeadline->SetLabel(wxString::Format(wxT("%s"), preferredDeadline.Format(wxT("%B %d, %H:%M")).c_str()));
			}
		}
		else
			mPreferredDeadline->SetLabel(_("N/A"));

		// Final deadline: if it is equal to 0 day, there is no final deadline
		if(client->GetDownloadDate().IsValid() && project->GetFinalDeadlineInDays() != 0)
		{
			finalDeadline = downloadTime;
			finalDeadline.Add(wxTimeSpan::Seconds(project->GetFinalDeadlineInDays() * 864));
			if(deadlineDays == ETADS_LEFT_TIME)
			{
				timeDiff = finalDeadline.Subtract(timeNow);
				timeInMinutes = timeDiff.GetMinutes();
				if(timeDiff.GetMinutes() < 0) timeInMinutes = 0 - timeInMinutes;

				// Split the left time into days, hours and minutes
				nbDays    = timeInMinutes / (24 * 60);
				nbMinutes = timeInMinutes % (24 * 60);
				nbHours   = nbMinutes / 60;
				nbMinutes = nbMinutes % 60;
				// Use a friendly format
				if(nbDays != 0)
					tempString = wxString::Format(wxT("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
				else if(nbHours != 0)
					tempString = wxString::Format(wxT("%ih %02imn"), nbHours, nbMinutes);
				else
					tempString = wxString::Format(wxT("%imn"), nbMinutes);

				if(timeDiff.GetMinutes() < 0)
				mFinalDeadline->SetLabel(wxString::Format(_("%s ago"), tempString.c_str()));
				else
				mFinalDeadline->SetLabel(wxString::Format(_("In %s"), tempString.c_str()));
			}
			else if (deadlineDays == ETADS_DATE_DAY_MONTH)
			{
				mFinalDeadline->SetLabel(wxString::Format(wxT("%s"), finalDeadline.Format(wxT("%d %B, %H:%M")).c_str()));
			} else
			{
				mFinalDeadline->SetLabel(wxString::Format(wxT("%s"), finalDeadline.Format(wxT("%B %d, %H:%M")).c_str()));
			}
		}
		else
			mFinalDeadline->SetLabel(_("N/A"));
	}
	// Get the total PPD to display next to progress bar
	mWUTotalPPD->SetLabel(wxString::Format(_(" :: Total PPD: %.2f"), MainDialog::GetTotalPPD()));

}


/************************************  FRAME CREATION  ************************************/


/**
* Create the menubar
**/
inline void MainDialog::CreateMenuBar(void)
{
	wxMenu    *menu;
	wxMenuBar *menuBar;

	// This method must be called once!
	wxASSERT(GetMenuBar() == NULL);

	// Create the menubar
	menuBar = new wxMenuBar();
	SetMenuBar(menuBar);

	// The 'Main' menu
	menu = new wxMenu();
	menu->Append(MID_TOGGLE_MESSAGES_FRAME, _("&Show/Hide Messages Window"), _("Toggle the messages window"));
	menu->Append(MID_UPDATEPROJECTS, _("&Download New Projects"), _("Update the local project database"));
	menu->AppendSeparator();
	menu->Append(MID_UPDATECHECK, _("&Check for update"), _("Check online for the latest version of FahMon"));
	menu->AppendSeparator();
	menu->Append(MID_BENCHMARKS, _("&Benchmarks...\tCTRL+B"), _("Open the benchmarks dialog"));
	menu->Append(MID_PREFERENCES, _("&Preferences...\tCTRL+P"), _("Open the preferences dialog"));
	menu->AppendSeparator();
#ifdef _FAHMON_WIN32_
	// MSVC stupidity
	menu->Append(wxID_EXIT, _("&Quit\tCtrl+Q"), wxString::Format(_T("%s %s"),  _("Quit"), _T(FMC_APPNAME)));
#elif __WXGTK__
	menu->Append(wxID_EXIT, _("&Quit\tCtrl+Q"), wxString::Format(_T("%s "FMC_APPNAME), _("Quit")));
#elif __WXMAC__
	menu->Append(wxID_EXIT, _("&Quit\tCtrl+Q"), wxString::Format(_T("%s "FMC_APPNAME), _("Quit")));
#endif
	menuBar->Append(menu, wxString::Format(wxT("&%s"), wxT(FMC_APPNAME)));

	// The 'Monitoring' menu
	menu = new wxMenu();
	menu->Append(MID_RELOAD, _("Reload &Selection\tF5"), _("Reload the selected client"));
	menu->Append(MID_RELOAD_ALL, _("Reload &All\tF6"), _("Reload all the clients"));
	menu->AppendSeparator();
	menu->Append(MID_TOGGLELOG, _("&Show/Hide FAHLog\tF8"), _("Toggle the log file"));
	menu->AppendSeparator();
	menu->Append(MID_TOGGLE_ETADATE, _("&Cycle ETA Style\tF9"), _("Cycle through the different ETA display styles"));
	menuBar->Append(menu, _("&Monitoring"));

	// The 'Web' menu
	menu = new wxMenu();
	menu->Append(MID_WWWMYSTATS, _("&My Stats\tF2"), _("View the personal statistics for the selected client"));
	menu->Append(MID_WWWJMOL, _("&Jmol\tF3"), _("View the current project on the Jmol website"));
	menu->Append(MID_WWWFAHINFO, _("&fahinfo\tF4"), _("View the current project on fahinfo.org"));
	menu->AppendSeparator();
	menu->Append(MID_WWWFOLDING, _("F@H &Website"), _("Open to the official Stanford website"));
	menu->Append(MID_WWWFCORG, _("Folding-&Community"), _("Open the Folding@Home support forum"));
	menu->Append(MID_WWWPROJECTS, _("&Projects Summary"), _("Open the list of the current projects"));
	menu->Append(MID_WWWSERVERS, _("&Servers Status"), _("Open the list of the servers with their status"));
	menuBar->Append(menu, _("&Web"));

	// The 'Help' menu
	menu = new wxMenu();
	menu->Append(wxID_HELP_CONTENTS, _("&Help Contents\tF1"), _("See help contents"));
#ifdef _FAHMON_WIN32_
	// MSVC stupidity
	menu->Append(wxID_ABOUT, _("&About"), wxString::Format(_T("%s %s"),  _("About"), _T(FMC_APPNAME)));
#elif __WXGTK__
	menu->Append(wxID_ABOUT, _("&About"), wxString::Format(_T("%s "FMC_APPNAME),  _("About")));
#elif __WXMAC__
	menu->Append(wxID_ABOUT, _("&About"), wxString::Format(_T("%s "FMC_APPNAME),  _("About")));
#endif
	menuBar->Append(menu, _("&Help"));
}


/**
* Create the needed widgets and place them in the frame
**/
inline void MainDialog::CreateLayout(void)
{
	wxPanel          *topLevelPanel;
	wxPanel          *topRightPanel;
	wxBoxSizer       *mainSizer;
	wxBoxSizer       *topSizer;
	wxBoxSizer       *midSizer;
	wxBoxSizer       *userinfoSizer;
	wxGridSizer      *infoSizer;
	wxStaticBoxSizer *topRightSizer;

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
	topRightPanel      = new wxPanel(mSplitterWindow, wxID_ANY);
	topRightSizer      = new wxStaticBoxSizer(wxVERTICAL, topRightPanel, _("Work Unit Information"));
	infoSizer          = new wxGridSizer(2, FMC_GUI_SPACING_HIGH, FMC_GUI_SPACING_LOW);
	userinfoSizer      = new wxBoxSizer(wxHORIZONTAL);
	mCoreName          = new StaticBoldedText(topRightPanel, wxID_ANY, wxT(""));
	mProjectId         = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));
	mCredit            = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));
	mDownloaded        = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));
	mPreferredDeadline = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));
	mFinalDeadline     = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));

	// User's information: username and team number
	mUsername    =  new wxHyperlinkCtrl(topRightPanel, wxID_ANY, wxT(""), wxT(""));
	mUsername->SetNormalColour(*wxBLUE);
	mUsername->SetHoverColour(*wxRED);
	mUsername->SetVisitedColour(*wxBLUE);
	mTeamNumber   = new wxHyperlinkCtrl(topRightPanel, wxID_ANY, wxT(""), wxT(""));
	mTeamNumber->SetNormalColour(*wxBLUE);
	mTeamNumber->SetHoverColour(*wxRED);
	mTeamNumber->SetVisitedColour(*wxBLUE);

	userinfoSizer->Add(mUsername);
	userinfoSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	userinfoSizer->Add(mTeamNumber);

	// Emphasize the name of the core
	mCoreName->SetForegroundColour(*wxRED);

	// Information on the current client is stored using a GridSizer
	infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, _("Core:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mCoreName, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, _("Project:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mProjectId, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, _("Credit:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mCredit, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, _("Username:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(userinfoSizer, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, _("Downloaded:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mDownloaded, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, _("Preferred Deadline:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mPreferredDeadline, 0, wxALIGN_LEFT);
	infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, _("Final Deadline:")), 0, wxALIGN_RIGHT);
	infoSizer->Add(mFinalDeadline, 0, wxALIGN_LEFT);

	// We use AddStretchSpacer() to keep the information vertically centered
	topRightSizer->SetMinSize(300, 0);
	topRightSizer->AddStretchSpacer();
	topRightSizer->Add(infoSizer, 0, wxALIGN_CENTER);
	topRightSizer->AddStretchSpacer();

	topRightPanel->SetSizer(topRightSizer);

	// --- The top part
	// It contains:
	//  - The list of clients on the left side, which takes the available left space
	//  - Information on the selected client on the right side, with a fixed size
	topSizer = new wxBoxSizer(wxHORIZONTAL);

	topSizer->Add(mSplitterWindow, 1, wxEXPAND);
	mSplitterWindow->SplitVertically(mClientsList, topRightPanel);

	// --- The middle part
	// It contains the progress bar (gauge) and a simple label
	// The label keeps its minimum size, while the gauge fill the available left space
	midSizer         = new wxBoxSizer(wxHORIZONTAL);
	mWUProgressGauge = new wxGauge(topLevelPanel, wxID_ANY, 100, wxDefaultPosition, wxSize(0, 18));
	mWUProgressText  = new wxStaticText(topLevelPanel, wxID_ANY, wxT(""));
	mWUTotalPPD = new wxStaticText(topLevelPanel, wxID_ANY, _(" :: Total PPD:"));

	midSizer->Add(mWUProgressGauge, 1);
	midSizer->Add(mWUProgressText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	midSizer->Add(mWUTotalPPD, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

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


/**
* Restore the frame as it was the last time, and center it
* Restored items are:
*  - The size of the frame
*  - Its position
*  - The visibility of the log
*  - The size of the columns in the ListCtrl
**/
inline void MainDialog::RestoreFrameState(void)
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

	// --- Size and position of the frame
	// Retrieve saved values
	_PrefsGetInt(PREF_MAINDIALOG_FRAMEWIDTH,  frameWidth);
	_PrefsGetInt(PREF_MAINDIALOG_FRAMEHEIGHT, frameHeight);

	_PrefsGetInt(PREF_MAINDIALOG_FRAME_POS_X, framePosX);
	_PrefsGetInt(PREF_MAINDIALOG_FRAME_POS_Y, framePosY);

	// Check if these values are correct for the current resolution, which can be different from the last time
	// If some values are not correct, they are changed to default ones
	wxDisplaySize(&displayWidth, &displayHeight);

	if(frameWidth >= displayWidth || frameHeight >= displayHeight)
	{
		// -1 indicates defaults values
		frameWidth  = -1;
		frameHeight = -1;
	}

	if(framePosX >= displayWidth || framePosY >= displayHeight)
	{
		// -1 indicates defaults values
		framePosX = -1;
		framePosY = -1;
	}
	max = false;
	if(frameWidth <= 0 || frameHeight <= 0)
	{
		if(frameWidth == -2 && frameHeight == -2)
			max = true;
		else
			max = false;
		// -1 indicates defaults values
		frameWidth  = -1;
		frameHeight = -1;
	}
	// We now have correct values, resize and move the frame
	SetSize(frameWidth, frameHeight);
	Move(framePosX, framePosY);
	if(max)
	{
		Maximize(true);
	}
}


/************************************  EVENTS  ************************************/


/**
* Called when the user wants to quit the application
**/
void MainDialog::OnMenuQuit(wxCommandEvent& event)
{
	Close(true);
}


/**
* Reload the selected client, if any
**/
void MainDialog::OnMenuReload(wxCommandEvent& event)
{
	ClientId selectedClientId = mClientsList->GetSelectedClientId();

	if(selectedClientId != INVALID_CLIENT_ID)
		ClientsManager::GetInstance()->ReloadThreaded(selectedClientId);
}


/**
* Reload all the clients
**/
void MainDialog::OnMenuReloadAll(wxCommandEvent& event)
{
	ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
}


/**
* Refresh the projects database
**/
void MainDialog::OnMenuUpdateProjects(wxCommandEvent& event)
{
	bool updateResult;

	// The user asked to update the database, so we force this update and we are not doing it silently
	updateResult = ProjectsManager::GetInstance()->UpdateDatabase(true, false);

	// If no error occurred while updating the database, we can then update the displayed information about clients
	if(updateResult == true)
		ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
}


/**
* Show/Hide the log
* The size of the frame is increased/decreased according to the new state of the log
**/
void MainDialog::OnMenuToggleLog(wxCommandEvent& event)
{
	bool          isLogShown;
	ClientId      selectedClientId;
	const Client *selectedClient;
	//mb The frame border at the bottom of the log control must also be taken into account when resizing
	const wxUint32 frameResizingOffset = FMC_GUI_LOG_HEIGHT + FMC_GUI_SPACING_LOW;

	// Show/Hide the log area
	isLogShown = !mLogFile->IsShown();
	mLogFile->Show(isLogShown);
	mTopLevelSizer->Show(mLogFile, isLogShown);
	mTopLevelSizer->Layout();

	// Resize the frame
	if(isLogShown)
		SetSize(-1, GetSize().GetHeight() + frameResizingOffset);
	else
		SetSize(-1, GetSize().GetHeight() - frameResizingOffset);

	// Load the correct log file (if any) when we are going to show the wxTextCtrl
	selectedClientId = mClientsList->GetSelectedClientId();
	if(isLogShown && selectedClientId != INVALID_CLIENT_ID)
	{
		selectedClient = ClientsManager::GetInstance()->Get(selectedClientId);

		mLogFile->SetValue(selectedClient->GetLog());
		mLogFile->ShowPosition(mLogFile->GetLastPosition());
	}
}


/**
* Show/Hide the information window
**/
void MainDialog::OnMenuToggleMessagesFrame(wxCommandEvent& event)
{
	MessagesFrame::GetInstance(this)->Toggle();
}


/**
* Open the benchmarks dialog
**/
void MainDialog::OnMenuBenchmarks(wxCommandEvent& event)
{
	ClientId  selectedClientId;
	ProjectId selectedProjectId;

	// Select the project currently processed by the selected client
	selectedClientId = mClientsList->GetSelectedClientId();

	if(selectedClientId == INVALID_CLIENT_ID)
		selectedProjectId = INVALID_PROJECT_ID;
	else
		selectedProjectId = ClientsManager::GetInstance()->Get(selectedClientId)->GetProjectId();

	// Ok, display the dialog
	BenchmarksDialog::GetInstance(this)->ShowModal(selectedProjectId);
}


/**
* Open the preferences dialog
**/
void MainDialog::OnMenuPreferences(wxCommandEvent& event)
{
	PreferencesDialog::GetInstance(this)->ShowModal();
}


/**
* Manage the menus which open Web pages
**/
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
					Tools::OpenURLInBrowser(ClientsManager::GetInstance()->Get(selectedClientId)->GetJmolURL());
				else
				if(event.GetId() == MID_WWWFAHINFO)
					Tools::OpenURLInBrowser(ClientsManager::GetInstance()->Get(selectedClientId)->GetFahinfoURL());
				else
					Tools::OpenURLInBrowser(ClientsManager::GetInstance()->Get(selectedClientId)->GetDonatorStatsURL());
			}
			else
				Tools::ErrorMsgBox(_("You must first select a client!"));
			break;

		//--
		case MID_WWWFOLDING:
			Tools::OpenURLInBrowser(wxT(FMC_URL_FOLDING));
			break;

		//--
		case MID_WWWFCORG:
			Tools::OpenURLInBrowser(wxT(FMC_URL_FCORG));
			break;

		//--
		case MID_WWWPROJECTS:
			Tools::OpenURLInBrowser(wxT(FMC_URL_PROJECTS));
			break;

		//--
		case MID_WWWSERVERS:
			Tools::OpenURLInBrowser(wxT(FMC_URL_SERVERS));
			break;

		//--
		case wxID_HELP_CONTENTS:
			Tools::OpenURLInBrowser(wxT(FMC_URL_HELP));
			break;

		//--
		default:
			// We should never fall here
			wxASSERT(false);
			break;
	}
}


/**
* Display the about box
**/
void MainDialog::OnMenuAbout(wxCommandEvent& event)
{
	AboutDialog::GetInstance(this)->ShowModal();
}


/**
* The application is going to stop, perform some last actions and/or decide if we are really going to quit
**/
void MainDialog::OnClose(wxCloseEvent& event)
{
	// Don't save window size if it is iconized (win32 bug)
#ifdef _WIN32_
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
		} else {
			_PrefsSetInt(PREF_MAINDIALOG_FRAMEWIDTH,  -2);
			_PrefsSetInt(PREF_MAINDIALOG_FRAMEHEIGHT, -2);
		}
	}

	// Save the position of the sash
	_PrefsSetUint(PREF_MAINDIALOG_SASHPOSITION, mSplitterWindow->GetSashPosition());

	// Save the state of the log
	_PrefsSetBool(PREF_MAINDIALOG_SHOWLOG, mLogFile->IsShown());

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

/**
* Main dialog has been minimized or restored
**/
void MainDialog::OnIconize(wxIconizeEvent& event)
{
	if(event.Iconized())
	{
		bool isTrayIconEnabled;

		_PrefsGetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON, isTrayIconEnabled);

		if(isTrayIconEnabled)
			Hide();
	}
}


/**
* Current selection in the list has changed
**/
void MainDialog::OnListSelectionChanged(wxListEvent& event)
{
	ShowClientInformation(mClientsList->GetSelectedClientId());
}


/**
* A client has been reloaded, update its information
**/
void MainDialog::OnClientReloaded(wxCommandEvent& event)
{
	ClientId clientId = event.GetInt();

	// The ListView must be updated, regardless of the current selection
	mClientsList->UpdateClient(clientId);

	// However, other information must be updated only if the client is the currently selected one
	if(clientId == mClientsList->GetSelectedClientId())
		ShowClientInformation(clientId);
}


/**
* A new client has been added to the ClientsManager
* The integer associated with the event gives the identifier of the new client
**/
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
		if(i != clientId)
			mClientsList->UpdateClient(i);

	// Finally load the new one
	// It will be automatically displayed when done
	ClientsManager::GetInstance()->ReloadThreaded(clientId);
}


/**
* A client has been deleted
**/
void MainDialog::OnClientDeleted(wxCommandEvent& event)
{
	wxUint32 i;

	// Reset the list to have the correct number of clients
	mClientsList->Reset(ClientsManager::GetInstance()->GetCount());

	// Clear displayed information, as the list has been reseted, nothing is selected
	ShowClientInformation(INVALID_CLIENT_ID);

	// Re-display all left clients
	for(i=0; i<ClientsManager::GetInstance()->GetCount(); ++i)
		mClientsList->UpdateClient(i);

}


/**
* The database of projects has been updated
**/
void MainDialog::OnProjectsDatabaseUpdated(wxCommandEvent& event)
{
	// New projects are perhaps available, so we reload all clients to update the displayed information
	ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
}


/**
* This is needed, because MessagesManager cannot make calls to MessagesFrame:
*
* "You can't make GUI method calls from more than one thread. You need to
* organize your app such that the GUI runs in the main thread and events/data
* are transfered to/from worker threads in some thread safe manner."
**/
void MainDialog::OnNewMessageLogged(wxCommandEvent& event)
{
	MessagesFrame::OnNewMessage();
}


/**
* Called by the timers used in this class
**/
void MainDialog::OnAutoReloadTimer(wxTimerEvent& event)
{
	bool isAdvancedReloadOn;

	_PrefsGetBool(PREF_MAINDIALOG_ADVANCEDRELOAD, isAdvancedReloadOn);

	if(isAdvancedReloadOn == true)
	{
		// For now, we have only one timer (auto-reload), so we don't have to test which one fired the event
		// Reload using experimental method testing last update time
		ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALL);
	}
	else
	{
		// Reload using old method - forces an upload regardless of changes
		ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
	}
}


/**
* Create or modify the auto-reload timer
**/
void MainDialog::OnAutoReloadPrefChanged(void)
{
	SetAutoReloadTimer();
}


/**
* Update the style used to display the ETA
* We simply ask the list to reload all clients, ETA will thus automatically be updated
*
* In the future, we could perhaps add a method to the ListView to just update the ETA without reloading the clients
* However this method is called very rarely, so why bother?
**/
void MainDialog::OnETAStylePrefChanged(void)
{
	mClientsList->UpdateAllClients();
}


/**
* Enable/Disable tray icon
**/
void MainDialog::OnTrayIconPrefChanged(void)
{
	bool isTrayIconEnabled;

	// Check the new value of the preference
	_PrefsGetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON, isTrayIconEnabled);

	// And install/uninstall the icon as needed
	if(isTrayIconEnabled)
		TrayManager::GetInstance()->InstallIcon();
	else
		TrayManager::GetInstance()->UninstallIcon();
}

/**
* Reload all clients on (but not limited to) PPD style change
**/
void MainDialog::OnPPDStylePrefChanged(void)
{
	ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALLF);
}

/**
* Reload selected client data on deadline style change
**/
void MainDialog::OnDeadlinePrefChanged(void)
{
	ClientId selectedClientId = mClientsList->GetSelectedClientId();

	if(selectedClientId != INVALID_CLIENT_ID)
		ClientsManager::GetInstance()->ReloadThreaded(selectedClientId);
}

/**
* Calculate total PPD from all active/inactive clients
**/
double MainDialog::GetTotalPPD(void)
{
	wxString  test;
	double  tmpdouble;
	double  TotalPPD;
	wxInt32	i;

	TotalPPD = 0;

	for(i=0; i<mClientsList->GetItemCount(); ++i) {
		test = mClientsList->GetCellContentsString(i,4);
		test.ToDouble(&tmpdouble);
		TotalPPD = TotalPPD + tmpdouble;
	}

	return TotalPPD;

}

/**
* Get the number of clients monitored
**/
wxInt32 MainDialog::GetClientCount(void)
{
	return mClientsList->GetItemCount();
}

/**
* Reload selected client data from traymenu
**/
void MainDialog::TrayReloadSelectedClient(void)
{
	ShowClientInformation(mClientsList->GetSelectedClientId());
}


/**
 * Toggle the ETA format
 **/
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
	}
}

/**
 * Check for updates to FahMon
**/
void MainDialog::OnUpdateCheck(wxCommandEvent& event)
{
	CheckForUpdates();
}


void MainDialog::CheckForUpdates(void)
{
	wxMutexLocker   mutexLocker(mMutexUpdateCheck);        // --- Lock the access to this method
	wxString        updateFile;
	wxString        errorMsg;
	wxString        versionInfo;
	wxTextFile      in;
	bool            updateAvailable = false;
	ProgressManager progressManager(false);

	_LogMsgInfo(_("Checking for FahMon updates"));

	// --- We first have to download the new update file
	progressManager.SetTextAndProgress(_("Checking for update"), 0);
	progressManager.CreateTask(100);
	if(DownloadUpdateFile(updateFile, progressManager, errorMsg, _T("http://fahmon.net/downloads/update.chk")) == false)
	{
		Tools::ErrorMsgBox(errorMsg);

		// Don't forget to remove the file before leaving, if it is needed
		if(updateFile.IsEmpty() == false)
			wxRemoveFile(updateFile);

		// Stop there, since we cannot parse a non-existing file
		return;
	}
	progressManager.EndTask();

	if(!wxFileExists(updateFile) || !in.Open(updateFile))
		return;

	versionInfo = in.GetLine(in.GetLineCount()-1);
	_LogMsgInfo(wxString::Format(_("Your version: %s; New version: %s"), wxT(FMC_VERSION), versionInfo.c_str()));

	if(versionInfo.Cmp(wxT(FMC_VERSION)) > 0)
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
		_LogMsgInfo(_("Update available"));
		if(Tools::QuestionMsgBox(_("A newer version of FahMon is available\nDo you want to go to the FahMon website?")) == true)
			Tools::OpenURLInBrowser(wxT("http://fahmon.net/download.html"));
	} else {
		_LogMsgInfo(_("No update found"));
	}
}

/**
 * This method downloads the files with the current projects
 * Return false if something went wrong, true otherwise
 * In the case of an error, an explicit message is placed in errorMsg
 * The name of the file to which the downloaded data was written is in fileName, which will be empty in case of an error
 **/
bool MainDialog::DownloadUpdateFile(wxString& fileName, ProgressManager& progressManager, wxString& errorMsg, wxString resource)
{

	HTTPDownloader::DownloadStatus downloadStatus;

	//Initialise the port number

	// Download the file
	downloadStatus = HTTPDownloader::Url(resource, fileName, progressManager);

	// If nothing went wrong, we can stop here
	if(downloadStatus == HTTPDownloader::STATUS_NO_ERROR)
		return true;

	// Otherwise, we create an explicit error message to specify what went wrong
	switch(downloadStatus)
	{
		case HTTPDownloader::STATUS_TEMP_FILE_CREATION_ERROR:
			errorMsg = _("Unable to create a temporary file!");
			break;

		case HTTPDownloader::STATUS_TEMP_FILE_OPEN_ERROR:
			errorMsg = wxString::Format(_("Unable to open the temporary file <%s>"), fileName.c_str());
			break;

		case HTTPDownloader::STATUS_CONNECT_ERROR:
			errorMsg = _("Unable to connect to the server!");
			break;

		case HTTPDownloader::STATUS_SEND_REQUEST_ERROR:
			errorMsg = _("Unable to send the request to the server!");
			break;

		case HTTPDownloader::STATUS_ABORTED:
			errorMsg = _("Download aborted!");
			break;

		// We should never fall here
		default:
			wxASSERT(false);
			errorMsg = _("An unknown error happened!");
			break;
	}
	return false;
}
