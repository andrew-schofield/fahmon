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
 * \file mainDialog.h
 * The main dialog.
 * Creates the main window for FahMon.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

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
#include "wx/hyperlink.h"
#include "wx/dcbuffer.h"
#include "wx/dcscreen.h"
#include "wx/window.h"
#include "wx/panel.h"
#include "listViewClients.h"
#include "progressManager.h"


#define PREF_MAINDIALOG_FRAMEWIDTH    wxT("MainDialog.FrameWidth") /**< Preference setting for dialog width */
#define PREF_MAINDIALOG_FRAMEWIDTH_DV 862 /**< Default value for dialog width */

#define PREF_MAINDIALOG_FRAMEHEIGHT    wxT("MainDialog.FrameHeight") /**< Preference setting for dialog height */
#define PREF_MAINDIALOG_FRAMEHEIGHT_DV 280 /**< Default value for dialog height */

#define PREF_MAINDIALOG_FRAME_POS_X    wxT("MainDialog.FramePosX") /**< Preference setting for dialog x coordinate */
#define PREF_MAINDIALOG_FRAME_POS_X_DV -1 /**< Default value for dialog x coordinate */

#define PREF_MAINDIALOG_FRAME_POS_Y    wxT("MainDialog.FramePosY") /**< Preference setting for dialog y coordinate */
#define PREF_MAINDIALOG_FRAME_POS_Y_DV -1 /**< Default value for dialog y coordinate */

#define PREF_MAINDIALOG_SASHPOSITION    wxT("MainDialog.SashPosition") /**< Preference setting for divider between client list and info panel */
#define PREF_MAINDIALOG_SASHPOSITION_DV 350 /**< Default value for divider between client list and info panel */

#define PREF_MAINDIALOG_SHOWLOG    wxT("MainDialog.ShowLog") /**< Preference setting for showing log */
#define PREF_MAINDIALOG_SHOWLOG_DV false /**< Default value for showing log */

#define PREF_MAINDIALOG_SHOWINFOPANEL    wxT("MainDialog.ShowInfoPanel")
#define PREF_MAINDIALOG_SHOWINFOPANEL_DV false

#define PREF_MAINDIALOG_AUTOUPDATEPROJECTS    wxT("MainDialog.AutoUpdateProjects") /**< Preference setting for auto updating projects */
#define PREF_MAINDIALOG_AUTOUPDATEPROJECTS_DV true /**< Default value for auto updating projects */

#define PREF_MAINDIALOG_AUTORELOAD    wxT("MainDialog.AutoReload") /**< Preference setting for auto reloading clients */
#define PREF_MAINDIALOG_AUTORELOAD_DV true /**< Default value for auto reloading clients */

#define PREF_MAINDIALOG_ADVANCEDRELOAD    wxT("MainDialog.AdvancedReload") /**< Preference setting for use of advanced reload technique */
#define PREF_MAINDIALOG_ADVANCEDRELOAD_DV false /**< Default value for use of advanced reload technique */

#define PREF_MAINDIALOG_AUTORELOADFREQUENCY    wxT("MainDialog.AutoReloadFrequency") /**< Preference setting for auto reload frequency */
#define PREF_MAINDIALOG_AUTORELOADFREQUENCY_DV 5 /**< Default value for auto reload frequency */

#define PREF_MAINDIALOG_ENABLE_TRAY_ICON    wxT("MainDialog.EnableTrayIcon") /**< Preference setting for use of tray icon */
#define PREF_MAINDIALOG_ENABLE_TRAY_ICON_DV false /**< Default value for use of tray icon */

#define PREF_MAINDIALOG_START_MINIMISED    wxT("MainDialog.StartMinimised") /**< Preference setting for starting FahMon minimised */
#define PREF_MAINDIALOG_START_MINIMISED_DV false /**< Default value for starting FahMon minimised */

#define PREF_LISTCLIENTS_KEEP_DEAD_LAST  wxT("MainDialog.KeepDeadClientsLast") /**< Preference setting for listing dead clients last */
#define PREF_LISTCLIENTS_KEEP_DEAD_LAST_DV true /**< Default value for listing dead clients last */

#define PREF_MAINDIALOG_DEADLINE_DAYS  wxT("MainDialog.ShowDeadlinesDays") /**< Preference setting for showing deadlines in days */
#define PREF_MAINDIALOG_DEADLINE_DAYS_DV false /**< Default value for showing deadlines in days */

#define PREF_OVERRIDE_TIMEZONE wxT("MainDialog.OverrideTimezone") /**< Preference setting for overriding timezones */
#define PREF_OVERRIDE_TIMEZONE_DV false /**< Default value for overriding timezones */

#define PREF_TZ wxT("MainDialog.TZ") /**< Preference setting for timezone offset */
#define PREF_TZ_DV 0 /**< Default value for timezone offset */

#define PREF_IGNORE_ASYNCHRONY wxT("MainDialog.IgnoreClockAsynchrony") /**<  Preference setting for ignoring clock asychrony */
#define PREF_IGNORE_ASYNCHRONY_DV false /**< Default value for ignoring clock asynchrony */

#define PREF_MAINDIALOG_MAXIMISE    wxT("MainDialog.Maxmise") /**< Preference setting for maximised window */
#define PREF_MAINDIALOG_MAXIMISE_DV false /**< Default value for maximised window state */

#define PREF_NON_THREADED_RELOAD    wxT("Clients.NonThreadedReload") /**< Preference setting for serial reloads */
#define PREF_NON_THREADED_RELOAD_DV false /**< Default value for serial reloads */

#define PREF_TOOLS_FILEMANAGER   wxT("MainDialog.FileManager") /**< Preference setting for filemanager */

#ifdef _FAHMON_WIN32_
#define PREF_TOOLS_FILEMANAGER_DV wxT("explorer.exe"); /**< Default value for Windows filemanager */
#elif __WXGTK__
#define PREF_TOOLS_FILEMANAGER_DV wxT("konqueror --profile filemanagement"); /**< Default value for Linux filemanager */
#elif __WXMAC__
#define PREF_TOOLS_FILEMANAGER_DV wxT("open");
#endif

#define PREF_MAINDIALOG_UPDATE_CHECK wxT("MainDialog.UpdateCheck") /**< Preference setting for checking for new FahMon updates */
#define PREF_MAINDIALOG_UPDATE_CHECK_DV false /**< Default value for checking for new FahMon updates */

#define PREF_MAINDIALOG_LOG_ERRORS_ONLY wxT("MainDialog.LogErrorsOnly") /**< Preference setting for checking for new FahMon updates */
#define PREF_MAINDIALOG_LOG_ERRORS_ONLY_DV true /**< Default value for checking for new FahMon updates */


#ifdef __WXMAC__
/**
* Custom progress bar for OSX.
* This class provides the simple non-animated progress bar.
**/
class OSXProgressBar: public wxWindow
{
protected:
	wxUint32    mTotal;
	wxUint32    mCurrentProgress;

	void OnPaint(wxPaintEvent& event);
	void OnResize(wxSizeEvent& event);
	void PaintBackground(wxDC& dc);
	void FillProgress(wxDC& dc);
public:
	OSXProgressBar(wxWindow* parent, wxWindowID, wxUint32 total, const wxPoint& pos, const wxSize& size);
	virtual ~OSXProgressBar();

	void SetValue(wxUint32 value);
private:
	DECLARE_EVENT_TABLE()

};

#endif

/**
* This is the main dialog box.
* This class can only be instantiated once.
**/
class MainDialog : public wxFrame
{
protected:
	static MainDialog *mInstance; /**< The single instance of the Main Dialog */
	static wxMutex     mMutexUpdateCheck; /**< Locks access to the update method */
	static wxMutex     mMutexArrayBlocker; /**< Locks access to the s_clientThreadsArray */

	// Widgets used in the frame
#ifndef __WXMAC__
	wxGauge          *mWUProgressGauge; /**< Progress bar control */
#else
	OSXProgressBar   *mWUProgressGauge;
#endif
	wxHyperlinkCtrl  *mUsername; /**< Username hyperlink control */
	wxHyperlinkCtrl  *mTeamNumber; /**< Team Number hyperlink control */
	wxBoxSizer       *mTopLevelSizer; /**< Sizer control for the whole window */
	wxTextCtrl       *mLogFile; /**< Text control for the log file */
	wxStaticText     *mWUProgressText; /**< Text control for the WU progress */
	wxStaticText     *mCoreName; /**< Text control for the core name */
	wxStaticText     *mProjectId; /**< Text control for the PRCG */
	wxStaticText     *mCredit; /**< Text control for the credit value */
	wxStaticText     *mDownloaded; /**< Text control for the download date */
	wxStaticText     *mPreferredDeadline; /**< Text control for the preferred deadline date */
	wxStaticText     *mFinalDeadline; /**< Text control for the final deadline date */
	ListViewClients  *mClientsList; /**< Client list view control */
	wxSplitterWindow *mSplitterWindow; /**< Pane splitting control */
	wxPanel          *mTopRightPanel;
	wxArrayInt       s_clientThreadsArray; /**< Array holding client whose processes are still active */

	// Misc
	wxTimer       mAutoReloadTimer; /**< Timer object for reloading clients */
	wxTaskBarIcon mSystemTray; /**< System tray icon object */

	/**
	 * Constructor.
	 * Constructs the main dialog window.
	 **/
	MainDialog(void);

	/**
	 * Destructor.
	 **/
	~MainDialog(void);

	/**
	 * Create the menubar.
	 **/
	void CreateMenuBar(void);

	/**
	 * Create the needed widgets and place them in the frame.
	 **/
	void CreateLayout(void);

	/**
	 * Restore the frame as it was the last time, and center it.
	 * Restored items are:
	 *  - The size of the frame
	 *  - Its position
	 *  - The visibility of the log
	 *  - The size of the columns in the ListCtrl
	 **/
	void RestoreFrameState(void);

	/**
	 * Create/Kill/Modify the timer for auto-reloading, according to the preferences.
	 **/
	void SetAutoReloadTimer(void);

	/**
	 * Update the fields using the given client and refresh the GUI.
	 * @param clientId The id of the client to show information for.
	 **/
	void ShowClientInformation(ClientId clientId);

	/**
	 * Update the fields using the given client.
	 * This method does not affect the ListView.
	 * @param clientId The id of the client to update.
	 **/
	void UpdateClientInformation(ClientId clientId);

	// Events
	/**
	 * Event: Manage the 'Reload' menu item.
	 * Reload the selected client, if any.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnMenuReload(wxCommandEvent& event);

	/**
	 * Event: Manage the 'Reload all' menu item.
	 * Reload all the clients.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnMenuReloadAll(wxCommandEvent& event);

	/**
	 * Event: Manage the 'Update projects' menu item.
	 * Refresh the projects database.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnMenuUpdateProjects(wxCommandEvent& event);

	/**
	 * Event: Manage the 'Show/Hide FAHlog' menu item.
	 * Show/Hide the log.
	 * The size of the frame is increased/decreased according to the new state of the log.
	 **/
	void OnMenuToggleLog(wxCommandEvent& event);

	/**
	 * Event: Manage the 'Show/Hide Messages Window' menu item.
	 * Show/Hide the information window
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnMenuToggleMessagesFrame(wxCommandEvent& event);

	/**
	 * Event: Manage the 'Benchmarks' menu item.
	 * Open the benchmarks dialog.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnMenuBenchmarks(wxCommandEvent& event);

	/**
	 * Event: Manage the 'Preferences' menu item.
	 * Open the preferences dialog.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnMenuPreferences(wxCommandEvent& event);

	/**
	 * Event: Manage the menus which open Web pages.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnMenuWeb(wxCommandEvent& event);

	/**
	 * Event: Manage the 'About' menu item.
	 * Display the about box.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnMenuAbout(wxCommandEvent& event);

	/**
	 * Event: Triggered when the application is closed.
	 * The application is going to stop, perform some last actions and/or decide if we are really going to quit.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnClose(wxCloseEvent& event);

	/**
	 * Event: Triggered when main dialog has been minimized or restored.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnIconize(wxIconizeEvent& event);

	/**
	 * Event: Manage the 'Quit' menu item.
	 * Called when the user wants to quit the application.
	 **/
	void OnMenuQuit(wxCommandEvent& event);

	/**
	 * Event: Triggered when current selection in the list has changed.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnListSelectionChanged(wxListEvent& event);

	/**
	 * Event: Triggered when a client has been reloaded.
	 * Update its information.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnClientReloaded(wxCommandEvent& event);

	/**
	 * Event: Triggered when a new client has been added to the ClientsManager.
	 * The integer associated with the event gives the identifier of the new client
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnNewClientAdded(wxCommandEvent& event);

	/**
	 * Event: Triggered when a client has been deleted.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnClientDeleted(wxCommandEvent& event);

	/**
	 * Event: Triggered when the database of projects has been updated.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnProjectsDatabaseUpdated(wxCommandEvent& event);

	/**
	 * Event: Manages new messages.
	 * This is needed, because MessagesManager cannot make calls to MessagesFrame:
	 *
	 * "You can't make GUI method calls from more than one thread. You need to
	 * organize your app such that the GUI runs in the main thread and events/data
	 * are transfered to/from worker threads in some thread safe manner."
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnNewMessageLogged(wxCommandEvent& event);

	/**
	 * Event: Manages the auto reload timer.
	 * Called by the timers used in this class
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnAutoReloadTimer(wxTimerEvent& event);

	/**
	 * Event: Manages the 'Cycle ETA style' menu item.
	 * Toggle the ETA format.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnMenuToggleETADate(wxCommandEvent& event);

	/**
	 * Event: Manages the 'check for update' menu item.
	 * Check for updates to FahMon
	 * @param event The event itself. This is sent automatically
	 **/
	void OnUpdateCheck(wxCommandEvent& event);

	/**
	 * This method downloads the files with the current projects.
	 * Return false if something went wrong, true otherwise
	 * In the case of an error, an explicit message is placed in errorMsg
	 * The name of the file to which the downloaded data was written is in fileName, which will be empty in case of an error
	 * @param fileName The file to save the download to
	 * @param progressManager The progress manager to control the download
	 * @param errorMsg The error message to display
	 * @param resource The location of the file to download
	 **/
	bool DownloadUpdateFile(wxString& fileName, ProgressManager& progressManager, wxString& errorMsg, wxString resource);

	/**
	 * Check online for FahMon updates.
	 * @param silent Whether a message is displayed when no update is found.
	 **/
	void CheckForUpdates(bool silent = true);

	void OnMenuEditClient(wxCommandEvent& event);

	void OnMenuDeleteClient(wxCommandEvent& event);

	void OnMenuViewFiles(wxCommandEvent& event);

	void OnMenuToggleInfoPanel(wxCommandEvent& event);

	void OnMenuCreateDebugReport(wxCommandEvent& event);


public:
	// Singleton pattern
	/**
	 * Create the instance of mainDialog.
	 **/
	static void CreateInstance(void);

	/**
	 * Destroy the single instance of the mainDialog.
	 * Only if it exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Check whether this dialog has been already instanciated.
	 * @return State of instanciation.
	 **/
	static bool HasBeenInstanciated(void);

	/**
	 * Retrieve the instance of mainDialog.
	 * If an instance doesn't already exist, create it.
	 * @return An instance of class mainDialog.
	 **/
	static MainDialog* GetInstance(void);


	/**
	 * Redefine the wxFrame::Show() method in order to perform some initializations before displaying the frame.
	 * @param show Should the frame actually be shown.
	 **/
	bool Show(bool show = true);

	/**
	 * Calculate total PPD from all active/inactive clients.
	 * @return Total PPD
	 **/
	double GetTotalPPD(void) const;

	/**
	 * Get the number of clients monitored.
	 * @return The number of clients.
	 **/
	wxInt32 GetClientCount(void) const;

	// Methods used when some prefs have changed

	/**
	 * Create or modify the auto-reload timer.
	 **/
	void OnAutoReloadPrefChanged(void);

	/**
	 * Update the style used to display the ETA.
	 * We simply ask the list to reload all clients, ETA will thus automatically be updated
	 *
	 * In the future, we could perhaps add a method to the ListView to just update the ETA without reloading the clients
	 * However this method is called very rarely, so why bother?
	 **/
	void OnETAStylePrefChanged(void);

	/**
	 * Enable/Disable tray icon.
	 **/
	void OnTrayIconPrefChanged(void);

	/**
	 * Reload all clients on (but not limited to) PPD style change.
	 **/
	void OnPPDStylePrefChanged(void);

	/**
	 * Reload selected client data on deadline style change.
	 **/
	void OnDeadlinePrefChanged(void);

	/**
	 * Reload selected client data from traymenu.
	 **/
	void TrayReloadSelectedClient(void);

	/**
	 * Return 'true' if the client does NOT already have a client reloading, false otherwise.
	 * @param clientId The id of the client to determine reloadability.
	 **/
	bool ClientReloadAllowed(ClientId clientId);

private:
	DECLARE_EVENT_TABLE()
};


// Custom events
BEGIN_DECLARE_EVENT_TYPES()
		DECLARE_EVENT_TYPE (EVT_CLIENTRELOADED, -1) /**< Sent when a client has been reloaded */
		DECLARE_EVENT_TYPE (EVT_NEWCLIENTADDED, -1) /**< Sent when a new client has been added to the ClientsManager */
		DECLARE_EVENT_TYPE (EVT_CLIENTDELETED, -1) /**< Sent when a client has been deleted */
		DECLARE_EVENT_TYPE (EVT_PROJECTS_DATABASE_UPDATED, -1) /**< Sent when the projects database has been updated */
		DECLARE_EVENT_TYPE (EVT_NEW_MESSAGE_LOGGED, -1) /**< Sent when a new message has been added to the MessagesManager */
END_DECLARE_EVENT_TYPES()

#endif /* _MAINDIALOG_H */
