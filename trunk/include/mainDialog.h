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

#ifndef _MAINDIALOG_H
#define _MAINDIALOG_H


#include "wx/frame.h"
#include "wx/timer.h"
#include "wx/sizer.h"
#include "wx/gauge.h"
#include "wx/taskbar.h"
#include "wx/splitter.h"
#include "wx/textctrl.h"
#include "wx/stattext.h"
#include "listViewClients.h"
#include "staticUrl.h"


/**
 * Preferences used by this class
**/
#define PREF_MAINDIALOG_FRAMEWIDTH    wxT("MainDialog.FrameWidth")
#define PREF_MAINDIALOG_FRAMEWIDTH_DV 650

#define PREF_MAINDIALOG_FRAMEHEIGHT    wxT("MainDialog.FrameHeight")
#define PREF_MAINDIALOG_FRAMEHEIGHT_DV 260

#define PREF_MAINDIALOG_FRAME_POS_X    wxT("MainDialog.FramePosX")
#define PREF_MAINDIALOG_FRAME_POS_X_DV -1

#define PREF_MAINDIALOG_FRAME_POS_Y    wxT("MainDialog.FramePosY")
#define PREF_MAINDIALOG_FRAME_POS_Y_DV -1

#define PREF_MAINDIALOG_SASHPOSITION    wxT("MainDialog.SashPosition")
#define PREF_MAINDIALOG_SASHPOSITION_DV 350

#define PREF_MAINDIALOG_SHOWLOG    wxT("MainDialog.ShowLog")
#define PREF_MAINDIALOG_SHOWLOG_DV false

#define PREF_MAINDIALOG_AUTOUPDATEPROJECTS    wxT("MainDialog.AutoUpdateProjects")
#define PREF_MAINDIALOG_AUTOUPDATEPROJECTS_DV true

#define PREF_MAINDIALOG_AUTORELOAD    wxT("MainDialog.AutoReload")
#define PREF_MAINDIALOG_AUTORELOAD_DV true

#define PREF_MAINDIALOG_ADVANCEDRELOAD    wxT("MainDialog.AdvancedReload")
#define PREF_MAINDIALOG_ADVANCEDRELOAD_DV false

#define PREF_MAINDIALOG_AUTORELOADFREQUENCY    wxT("MainDialog.AutoReloadFrequency")
#define PREF_MAINDIALOG_AUTORELOADFREQUENCY_DV 5

#define PREF_MAINDIALOG_ENABLE_TRAY_ICON    wxT("MainDialog.EnableTrayIcon")
#define PREF_MAINDIALOG_ENABLE_TRAY_ICON_DV false

#define PREF_MAINDIALOG_START_MINIMISED    wxT("MainDialog.StartMinimised")
#define PREF_MAINDIALOG_START_MINIMISED_DV false

#define PREF_LISTCLIENTS_KEEP_DEAD_LAST  wxT("MainDialog.KeepDeadClientsLast")
#define PREF_LISTCLIENTS_KEEP_DEAD_LAST_DV true

#define PREF_MAINDIALOG_DEADLINE_DAYS  wxT("MainDialog.ShowDeadlinesDays")
#define PREF_MAINDIALOG_DEADLINE_DAYS_DV false

#define PREF_OVERRIDE_TIMEZONE wxT("MainDialog.OverrideTimezone")
#define PREF_OVERRIDE_TIMEZONE_DV false

#define PREF_TZ wxT("MainDialog.TZ")
#define PREF_TZ_DV 0

#define PREF_IGNORE_ASYNCHRONY wxT("MainDialog.IgnoreClockAsynchrony")
#define PREF_IGNORE_ASYNCHRONY_DV false

#define PREF_TOOLS_FILEMANAGER   wxT("MainDialog.FileManager")

#ifndef _FAHMON_LINUX_
#define PREF_TOOLS_FILEMANAGER_DV wxT("explorer.exe");
#else
#define PREF_TOOLS_FILEMANAGER_DV wxT("konqueror --profile=filemanagement");
#endif

// Custom events
DECLARE_EVENT_TYPE(EVT_CLIENTRELOADED, -1)              // Sent when a client has been reloaded
DECLARE_EVENT_TYPE(EVT_NEWCLIENTADDED, -1)              // Sent when a new client has been added to the ClientsManager
DECLARE_EVENT_TYPE(EVT_CLIENTDELETED, -1)               // Sent when a client has been deleted
DECLARE_EVENT_TYPE(EVT_PROJECTS_DATABASE_UPDATED, -1)   // Sent when the projects database has been updated
DECLARE_EVENT_TYPE(EVT_NEW_MESSAGE_LOGGED, -1)          // Sent when a new message has been added to the MessagesManager


/**
 * This is the main dialog box
 * It's a singleton
**/
class MainDialog : public wxFrame
{
protected:
	static MainDialog *mInstance;

    // Widgets used in the frame
    wxGauge          *mWUProgressGauge;
    StaticUrl        *mUsername;
    StaticUrl        *mTeamNumber;
    wxBoxSizer       *mTopLevelSizer;
    wxTextCtrl       *mLogFile;
    wxStaticText     *mWUProgressText;
    wxStaticText     *mWUTotalPPD;
    wxStaticText     *mCoreName;
    wxStaticText     *mProjectId;
    wxStaticText     *mCredit;
    wxStaticText     *mDownloaded;
    wxStaticText     *mPreferredDeadline;
    wxStaticText     *mFinalDeadline;
    ListViewClients  *mClientsList;
    wxSplitterWindow *mSplitterWindow;

    // Misc
    wxTimer       mAutoReloadTimer;
    wxTaskBarIcon mSystemTray;

     MainDialog(void);
    ~MainDialog(void);

    void CreateMenuBar(void);
    void CreateLayout(void);
    void RestoreFrameState(void);

    void SetAutoReloadTimer(void);
    void ShowClientInformation(ClientId clientId);
    void UpdateClientInformation(ClientId clientId);

    // Events
    void OnMenuReload(wxCommandEvent& event);
    void OnMenuReloadAll(wxCommandEvent& event);
    void OnMenuUpdateProjects(wxCommandEvent& event);
    void OnMenuToggleLog(wxCommandEvent& event);
    void OnMenuToggleMessagesFrame(wxCommandEvent& event);
    void OnMenuBenchmarks(wxCommandEvent& event);
    void OnMenuPreferences(wxCommandEvent& event);
    void OnMenuWeb(wxCommandEvent& event);
    void OnMenuAbout(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnIconize(wxIconizeEvent& event);
    void OnMenuQuit(wxCommandEvent& event);
    void OnListSelectionChanged(wxListEvent& event);
    void OnClientReloaded(wxCommandEvent& event);
    void OnNewClientAdded(wxCommandEvent& event);
    void OnClientDeleted(wxCommandEvent& event);
    void OnProjectsDatabaseUpdated(wxCommandEvent& event);
    void OnNewMessageLogged(wxCommandEvent& event);
    void OnAutoReloadTimer(wxTimerEvent& event);


public:
	// Singleton pattern
    static void CreateInstance(void);
    static void DestroyInstance(void);
    static bool HasBeenInstanciated(void);
    static MainDialog* GetInstance(void);

    bool Show(bool show = true);
    double GetTotalPPD(void);
    wxInt32 GetClientCount(void);

    // Methods used when some prefs have changed
    void OnAutoReloadPrefChanged(void);
    void OnETAStylePrefChanged(void);
    void OnTrayIconPrefChanged(void);
    void OnPPDStylePrefChanged(void);
    void OnDeadlinePrefChanged(void);


private:
	DECLARE_EVENT_TABLE()
};


#endif /* _MAINDIALOG_H */
