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


// Identifiers for the controls
enum _CONTROL_ID
{
    // --- Menus
    MID_RELOAD = wxID_HIGHEST,
    MID_RELOAD_ALL,
    MID_UPDATEPROJECTS,
    MID_TOGGLELOG,
    MID_TOGGLE_MESSAGES_FRAME,
    MID_BENCHMARKS,
    MID_PREFERENCES,
    MID_WWWJMOL,
    MID_WWWMYSTATS,
    MID_WWWFOLDING,
    MID_WWWFCORG,
    MID_WWWPROJECTS,
    MID_WWWSERVERS,
    
    // --- ListView
    LST_CLIENTS
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
    EVT_MENU    (MID_BENCHMARKS,            MainDialog::OnMenuBenchmarks)
    EVT_MENU    (MID_PREFERENCES,           MainDialog::OnMenuPreferences)
    EVT_MENU    (MID_WWWJMOL,               MainDialog::OnMenuWeb)
    EVT_MENU    (MID_WWWMYSTATS,            MainDialog::OnMenuWeb)
    EVT_MENU    (MID_WWWFOLDING,            MainDialog::OnMenuWeb)
    EVT_MENU    (MID_WWWFCORG,              MainDialog::OnMenuWeb)
    EVT_MENU    (MID_WWWPROJECTS,           MainDialog::OnMenuWeb)
    EVT_MENU    (MID_WWWSERVERS,            MainDialog::OnMenuWeb)
    EVT_MENU    (wxID_HELP_CONTENTS,        MainDialog::OnMenuWeb)
    EVT_MENU    (wxID_ABOUT,                MainDialog::OnMenuAbout)

    // --- Frame
    EVT_CLOSE   (MainDialog::OnClose)

    // --- List
    EVT_LIST_ITEM_SELECTED  (LST_CLIENTS, MainDialog::OnListSelectionChanged)
    
    // --- Timers
    EVT_TIMER   (wxID_ANY,  MainDialog::OnAutoReloadTimer)

    // --- Custom
    EVT_COMMAND    (wxID_ANY, EVT_CLIENTRELOADED,               MainDialog::OnClientReloaded)
    EVT_COMMAND    (wxID_ANY, EVT_NEWCLIENTADDED,               MainDialog::OnNewClientAdded)
    EVT_COMMAND    (wxID_ANY, EVT_CLIENTDELETED,                MainDialog::OnClientDeleted)
    EVT_COMMAND    (wxID_ANY, EVT_PROJECTS_DATABASE_UPDATED,    MainDialog::OnProjectsDatabaseUpdated)
    EVT_COMMAND    (wxID_ANY, EVT_NEW_MESSAGE_LOGGED,           MainDialog::OnNewMessageLogged)
END_EVENT_TABLE()


// The single instance of MainDialog accross the application
MainDialog* MainDialog::mInstance = NULL;


/**
 * Constructor
**/
MainDialog::MainDialog(void) : wxFrame(NULL, wxID_ANY, wxT(FMC_PRODUCT))
{
    bool trayIconEnabled;

    // Setting the icon for the main dialog will allows child frames and dialog to inherit from it

#ifdef _FAHMON_LINUX_
    SetIcon(wxIcon(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_DIALOG)));
#else
    SetIcon(wxICON(dialog_icon));
#endif

    CreateMenuBar();
    CreateStatusBar(2);
    CreateLayout();
    RestoreFrameState();     // MUST be called when all controls have been created !

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
    if(show == true)
    {
        // Items surely won't keep their order when they will be loaded because of sorting, so selecting a
        // default one is useless if there are more than one client
        // Of course, in the case of only one client, we can still select it
        if(ClientsManager::GetInstance()->GetCount() == 1)
            mClientsList->Select(0);

        ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALL);
    }

    return wxFrame::Show(show);
}


/**
 * Create/Kill/Modify the timer for auto-reloading, according to the preferences
**/
void MainDialog::SetAutoReloadTimer(void)
{
    bool     isAutoReloadOn;
    wxUint32 autoReloadFrequency;

    // First, we have to stop the timer if it is running, because preferences can be different from the last time
    if(mAutoReloadTimer.IsRunning() == true)
        mAutoReloadTimer.Stop();

    // Then, retrieve the (perhaps new) preferences
    _PrefsGetBool(PREF_MAINDIALOG_AUTORELOAD,          isAutoReloadOn);
    _PrefsGetUint(PREF_MAINDIALOG_AUTORELOADFREQUENCY, autoReloadFrequency);

    // Ok, now we can start the timer if needed but we need to convert minutes to milliseconds
    if(isAutoReloadOn == true)
        mAutoReloadTimer.Start(autoReloadFrequency * 60 * 1000);
}


/**
 * Display available information on the given clientId
 * This method does not affect the ListView
**/
void MainDialog::ShowClientInformation(ClientId clientId)
{
    bool           autoUpdateProjects;
    wxDateTime     preferredDeadline;
    wxDateTime     finalDeadline;
    const Client  *client;
    const Project *project;

    // Clear information for invalid clients
    if(clientId == INVALID_CLIENT_ID)
    {
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
        mCoreName->SetLabel(wxT(""));
        mProjectId->SetLabel(wxT(""));
        mCredit->SetLabel(wxT(""));
        mDownloaded->SetLabel(wxT(""));
        mPreferredDeadline->SetLabel(wxT(""));
        mFinalDeadline->SetLabel(wxT(""));
        mWUProgressGauge->SetValue(0);
        mWUProgressText->SetLabel(wxT(""));
        mLogFile->SetValue(wxT("Something is wrong with this client.\nPlease check the messages (FahMon->Show/Hide Messages Window)."));

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
    
    if(client->GetDownloadDate() == wxInvalidDateTime)
        mDownloaded->SetLabel(wxT("N/A"));
    else
        mDownloaded->SetLabel(client->GetDownloadDate().Format(wxT(FMC_DATE_MAIN_FORMAT)));

    if(client->GetProjectId() == INVALID_PROJECT_ID)
    {
        mProjectId->SetLabel(wxT("N/A"));
        mCoreName->SetLabel(wxT("N/A"));
        mCredit->SetLabel(wxT("N/A"));
        mPreferredDeadline->SetLabel(wxT("N/A"));
        mFinalDeadline->SetLabel(wxT("N/A"));
    }
    else
    {
        mProjectId->SetLabel(wxString::Format(wxT("%u"), client->GetProjectId()));
        project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());

        // This project can be unknown, if the database is not up to date
        if(project == NULL)
        {
            // Update the database, if the user wants to
            // This update is not forced, it will occur only if the elapsed time since the last one is high enough
            // This way, we ensure that we won't perform many multiple requests for the file, while it does not
            // contain the information we want
            _PrefsGetBool(PREF_MAINDIALOG_AUTOUPDATEPROJECTS, autoUpdateProjects);
            
            if(autoUpdateProjects)
            {
                if(ProjectsManager::GetInstance()->UpdateDatabase(false, false))
                    ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALL);
            }
            else
            {
                mCoreName->SetLabel(wxT("N/A"));
                mCredit->SetLabel(wxT("N/A"));
                mPreferredDeadline->SetLabel(wxT("N/A"));
                mFinalDeadline->SetLabel(wxT("N/A"));
                _LogMsgWarning(wxString::Format(wxT("Project %u is unknown, you should try to update the projects database"), client->GetProjectId()));
            }
        }
        else
        {
            mCoreName->SetLabel(Core::IdToLongName(project->GetCoreId()));
            mCredit->SetLabel(wxString::Format(wxT("%u points"), project->GetCredit()));
            
            // Preferred deadline: if it is equal to 0 day, there is no preferred deadline
            if(client->GetDownloadDate() != wxInvalidDateTime && project->GetPreferredDeadlineInDays() != 0)
            {
                preferredDeadline = client->GetDownloadDate();
                preferredDeadline.Add(wxTimeSpan::Days(project->GetPreferredDeadlineInDays()));
                mPreferredDeadline->SetLabel(preferredDeadline.Format(wxT(FMC_DATE_MAIN_FORMAT)));
            }
            else
                mPreferredDeadline->SetLabel(wxT("N/A"));

            // Final deadline: if it is equal to 0 day, there is no final deadline
            if(client->GetDownloadDate() != wxInvalidDateTime && project->GetFinalDeadlineInDays() != 0)
            {
                finalDeadline = client->GetDownloadDate();
                finalDeadline.Add(wxTimeSpan::Days(project->GetFinalDeadlineInDays()));
                mFinalDeadline->SetLabel(finalDeadline.Format(wxT(FMC_DATE_MAIN_FORMAT)));
            }
            else
                mFinalDeadline->SetLabel(wxT("N/A"));
        }
    }

    mTopLevelSizer->Layout();
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
    menu->Append(MID_TOGGLE_MESSAGES_FRAME, wxT("&Show/Hide Messages Window"), wxT("Toggle the messages window"));
    menu->Append(MID_UPDATEPROJECTS, wxT("&Download New Projects"), wxT("Update the local project database"));
    menu->AppendSeparator();
    menu->Append(MID_BENCHMARKS, wxT("&Benchmarks...\tCTRL+B"), wxT("Open the benchmarks dialog"));
    menu->Append(MID_PREFERENCES, wxT("&Preferences...\tCTRL+P"), wxT("Open the preferences dialog"));
    menu->AppendSeparator();
    menu->Append(wxID_EXIT, wxT("&Quit\tCtrl+Q"), wxT("Quit "FMC_APPNAME));
    menuBar->Append(menu, wxT("&"FMC_APPNAME));

    // The 'Monitoring' menu
    menu = new wxMenu();
    menu->Append(MID_RELOAD, wxT("Reload &Selection\tF5"), wxT("Reload the selected client"));
    menu->Append(MID_RELOAD_ALL, wxT("Reload &All\tF6"), wxT("Reload all the clients"));
    menu->AppendSeparator();
    menu->Append(MID_TOGGLELOG, wxT("&Show/Hide FAHLog\tF8"), wxT("Toggle the log file"));
    menuBar->Append(menu, wxT("&Monitoring"));

    // The 'Web' menu
    menu = new wxMenu();
    menu->Append(MID_WWWMYSTATS, wxT("&My Stats\tF2"), wxT("View the personal statistics for the selected client"));
    menu->Append(MID_WWWJMOL, wxT("&Jmol\tF3"), wxT("View the current project on the Jmol website"));
    menu->AppendSeparator();
    menu->Append(MID_WWWFOLDING, wxT("F@H &Website"), wxT("Open to the official Stanford website"));
    menu->Append(MID_WWWFCORG, wxT("Folding-&Community"), wxT("Open the Folding@Home support forum"));
    menu->Append(MID_WWWPROJECTS, wxT("&Projects Summary"), wxT("Open the list of the current projects"));
    menu->Append(MID_WWWSERVERS, wxT("&Servers Status"), wxT("Open the list of the servers with their status"));
    menuBar->Append(menu, wxT("&Web"));
	
    // The 'Help' menu
    menu = new wxMenu();
    menu->Append(wxID_HELP_CONTENTS, wxT("&Help Contents\tF1"), wxT("See help contents"));
    menu->Append(wxID_ABOUT, wxT("&About"), wxT("About "FMC_APPNAME));
    menuBar->Append(menu, wxT("&Help"));	
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
    topRightSizer      = new wxStaticBoxSizer(wxVERTICAL, topRightPanel, wxT("Work Unit Information"));
    infoSizer          = new wxGridSizer(2, FMC_GUI_SPACING_HIGH, FMC_GUI_SPACING_LOW);
    mCoreName          = new StaticBoldedText(topRightPanel, wxID_ANY, wxT(""));
    mProjectId         = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));
    mCredit            = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));
    mDownloaded        = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));
    mPreferredDeadline = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));
    mFinalDeadline     = new wxStaticText(topRightPanel, wxID_ANY, wxT(""));

    // Emphasize the name of the core
    mCoreName->SetForegroundColour(*wxRED);

    // Information on the current client is stored using a GridSizer
    infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, wxT("Core:")), 0, wxALIGN_RIGHT);
    infoSizer->Add(mCoreName, 0, wxALIGN_LEFT);
    infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, wxT("Project:")), 0, wxALIGN_RIGHT);
    infoSizer->Add(mProjectId, 0, wxALIGN_LEFT);
    infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, wxT("Credit:")), 0, wxALIGN_RIGHT);
    infoSizer->Add(mCredit, 0, wxALIGN_LEFT);
    infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, wxT("Downloaded:")), 0, wxALIGN_RIGHT);
    infoSizer->Add(mDownloaded, 0, wxALIGN_LEFT);    
    infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, wxT("Preferred Deadline:")), 0, wxALIGN_RIGHT);
    infoSizer->Add(mPreferredDeadline, 0, wxALIGN_LEFT);    
    infoSizer->Add(new StaticBoldedText(topRightPanel, wxID_ANY, wxT("Final Deadline:")), 0, wxALIGN_RIGHT);
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
    mSplitterWindow->SplitVertically(mClientsList,topRightPanel);


    // --- The middle part
    // It contains the progress bar (gauge) and a simple label 
    // The label keeps its minimum size, while the gauge fill the available left space
    midSizer         = new wxBoxSizer(wxHORIZONTAL);
    mWUProgressGauge = new wxGauge(topLevelPanel, wxID_ANY, 100, wxDefaultPosition, wxSize(0, 18));
    mWUProgressText  = new wxStaticText(topLevelPanel, wxID_ANY, wxT(""));
    
    midSizer->Add(mWUProgressGauge, 1);
    midSizer->Add(mWUProgressText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    
    
    // --- The bottom part : Log file
    mLogFile = new wxTextCtrl(topLevelPanel, wxID_ANY, wxT("Log file."), wxDefaultPosition, wxSize(100, FMC_GUI_LOG_HEIGHT), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);
    

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


    // --- State of the log
    _PrefsGetBool(PREF_MAINDIALOG_SHOWLOG, isLogShown);

    // By default the log is shown, so we hide it only if we should
    // We must not call ToggleLog(), because it resizes the frame and the current size is the correct (restored) one
    if(isLogShown == false)
    {
        mLogFile->Show(false);
        mTopLevelSizer->Show(mLogFile, false);
    }
    // This must be called even if we don't hide the log, otherwise the wxSplitterWindow does not correctly restore the sash position
    mTopLevelSizer->Layout();
    
    
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

    // We now have correct values, resize and move the frame
    SetSize(frameWidth, frameHeight);
    Move(framePosX, framePosY);

    
    // --- Splitter's state
    _PrefsGetUint(PREF_MAINDIALOG_SASHPOSITION, sashPosition);

    mSplitterWindow->SetMinimumPaneSize(20);
    mSplitterWindow->SetSashPosition(sashPosition);
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
    ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALL);
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
        ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALL);
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
    
    // Show/Hide the log area
    isLogShown = !mLogFile->IsShown();
    mLogFile->Show(isLogShown);
    mTopLevelSizer->Show(mLogFile, isLogShown);
    mTopLevelSizer->Layout();

    // Resize the frame
    if(isLogShown == true)
        SetSize(-1, GetSize().GetHeight() + FMC_GUI_LOG_HEIGHT);
    else
        SetSize(-1, GetSize().GetHeight() - FMC_GUI_LOG_HEIGHT);

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
    wxString      webAddress;
    ClientId      selectedClientId;
    const Client *selectedClient;

    // Determine the url to go to
    switch(event.GetId())
    {
        //--
        case MID_WWWJMOL:
        case MID_WWWMYSTATS:
            // For these two menus, a client MUST be selected
            selectedClientId = mClientsList->GetSelectedClientId();
            if(selectedClientId != INVALID_CLIENT_ID)
            {
                selectedClient = ClientsManager::GetInstance()->Get(selectedClientId);
                
                if(event.GetId() == MID_WWWJMOL)
                    webAddress = wxString::Format(wxT("%s%u"), wxT(FMC_URL_JMOL), selectedClient->GetProjectId());
                else
                    webAddress = wxString::Format(wxT("%s&teamnum=%u&username=%s"), wxT(FMC_URL_MYSTATS), selectedClient->GetTeamNumber(), selectedClient->GetUserName().c_str());
                
                Tools::OpenURLInBrowser(webAddress);
            }
            else
                Tools::ErrorMsgBox(wxT("You must first select a client!"));
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
    // Save the size of the frame
    _PrefsSetInt(PREF_MAINDIALOG_FRAMEWIDTH,  GetSize().GetWidth());
    _PrefsSetInt(PREF_MAINDIALOG_FRAMEHEIGHT, GetSize().GetHeight());
    
    // Save the position of the frame
    _PrefsSetInt(PREF_MAINDIALOG_FRAME_POS_X, GetPosition().x);
    _PrefsSetInt(PREF_MAINDIALOG_FRAME_POS_Y, GetPosition().y);
    
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
    ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALL);
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
    // For now, we have only one timer (auto-reload), so we don't have to test which one fired the event
    ClientsManager::GetInstance()->ReloadThreaded(CM_LOADALL);
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
    bool trayIconEnabled;

    // Check the new value of the preference
    _PrefsGetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON, trayIconEnabled);

    // And install/uninstall the icon as needed
    if(trayIconEnabled == true)
        TrayManager::GetInstance()->InstallIcon();
    else
        TrayManager::GetInstance()->UninstallIcon();
}
