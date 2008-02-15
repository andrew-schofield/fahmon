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
 * \file preferencesDialog.h
 * The preferences dialog.
 * Creates the dialog to reconfigure FahMon.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _PREFERENCESDIALOG_H
#define _PREFERENCESDIALOG_H


#include "wx/panel.h"
#include "wx/dialog.h"
#include "wx/choice.h"
#ifndef __WXMAC__ 
#include "wx/notebook.h" 
#else 
#include "wx/choicebk.h" 
#endif 
#include "wx/textctrl.h"
#include "wx/checkbox.h"
#include "wx/spinctrl.h"
#include "wx/stattext.h"
#include "wx/button.h"


/**
 * This dialog box allows the user to change 'public' preferences.
 * This class can only be instanciated once.
 **/
class PreferencesDialog : public wxDialog
{
protected:
	static PreferencesDialog *mInstance; /**< The single instance of the PreferencesDialog */

	// Widgets for the general tab
	wxCheckBox *mGeneralEnableTrayIcon; /**< Checkbox for tray icon option */
	wxCheckBox *mGeneralAutoUpdateProjectsDatabase; /**< Checkbox for auto updating projects database */
	wxCheckBox *mGeneralCollectXYZFiles; /**< Checkbox for collecting xyz files */
	wxCheckBox *mGeneralKeepInaccessibleClientsLast; /**< Checkbox for listing dead clients last */
	wxCheckBox *mGeneralStartMinimised; /**< Checkbox for starting minimised */
	wxCheckBox *mGeneralUpdateCheck; /**< Checkbox for update checking */

	// Widgets for the monitoring tab
	wxChoice     *mMonitoringETADisplayStyle; /**< Select box for ETA style */
	wxCheckBox   *mMonitoringAdvancedReload; /**< Checkbox for advanced reload system */
	wxCheckBox   *mMonitoringAutoReload; /**< Checkbox for auto reload */
	wxStaticText *mMonitoringAutoReloadInt; /**< Text control for reload interval */
	wxSpinCtrl   *mMonitoringAutoReloadFrequency; /**< Spin control for reload interval */
	wxChoice     *mMonitoringPPDType; /**< Select box for PPD calculation method */
	wxCheckBox   *mMonitoringIgnoreAsynchrony; /**< Checkbox for ignoring asynchronous clocks */

	// Widgets for the networking tab
	wxCheckBox   *mNetworkingUseProxy; /**< Checkbox for use of proxy */
	wxTextCtrl   *mNetworkingProxyAddress; /**< Input box for proxy address */
	wxTextCtrl   *mNetworkingProxyPort; /**< Input box for proxy port */
	wxCheckBox   *mNetworkingUseProxyAuthentication; /**< Checkbox for use of proxy authentication */
	wxTextCtrl   *mNetworkingProxyUsername; /**< Input box for proxy username */
	wxTextCtrl   *mNetworkingProxyPassword; /**< Input box for proxy password */
	wxStaticText *mNetworkingLabelAddress; /**< Text control for proxy address */
	wxStaticText *mNetworkingLabelPort; /**< Text control for proxy port */
	wxStaticText *mNetworkingLabelUsername; /**< Text control for proxy username */
	wxStaticText *mNetworkingLabelPassword; /**< Text control for proxy password */

	// Widgets for the advanced tab
	wxCheckBox   *mAdvancedUseAlternateProjectSource; /**< Checkbox for use of alternative update source */
	wxTextCtrl   *mAdvancedAlternateProjectSourceLocationAddress; /**< Input box for alternative update source location */
	wxStaticText *mAdvancedLabelLocationAddress; /**< Text control for alternative address location */
	wxCheckBox   *mAdvancedUseLocalFile; /**< Checkbox for use of local file as update source */
	wxTextCtrl   *mAdvancedLocalFileLocation; /**< Input box for local file location */
	wxStaticText *mAdvancedLabelLocalFile; /**< Text control for local file location */
	wxButton     *mAdvancedLocationChooser; /**< Button to open file chooser for local file */

	// Widgets for the system tab
	wxTextCtrl   *mSystemBrowser; /**< Input box for web browser */
	wxStaticText *mSystemBrowserLabel; /**< Text control for web browser */
	wxChoice     *mSystemFileManager; /**< Select box for file manager */
	wxStaticText *mSystemFileManagerLabel; /**< Text control for file manager */
	wxTextCtrl   *mSystemOtherFM; /**< Text control for alternative file manager */
	wxStaticText *mSystemOtherFMLabel; /**< Input box for alternative file manager */
	wxCheckBox   *mSystemOverrideTimezone; /**< Checkbox for overriding timezone */
	wxSpinCtrl   *mSystemTZ; /**< Spin control for timezone offset */

	// Widgets for the webapp tab
	wxTextCtrl   *mWebAppWebAppLocation; /**< Input box for fancy web output location */
	wxStaticText *mWebAppWebAppLabel; /**< Text control for fancy web output location */
	wxButton     *mWebAppWebAppLocationChooser; /**< Button to open file chooser for fancy web output location */
	wxCheckBox   *mWebAppUseWebApp; /**< Checkbox for use of fancy web output */
	wxTextCtrl   *mWebAppSimpleWebLocation; /**< Input box for simple web output location */
	wxStaticText *mWebAppSimpleWebLabel; /**< Text control for simple web output location */
	wxButton     *mWebAppSimpleWebLocationChooser; /**< Button to open file chooser for simple web output location */
	wxCheckBox   *mWebAppUseSimpleWeb; /**< Checkbox for use of simple web output */
	wxTextCtrl   *mWebAppSimpleTextLocation; /**< Input box for simple text output location */
	wxStaticText *mWebAppSimpleTextLabel; /**< Text control for simple text output location */
	wxButton     *mWebAppSimpleTextLocationChooser; /**< Button to open file chooser for simple text output location */
	wxCheckBox   *mWebAppUseSimpleText; /**< Checkbox for use of simple text output */

	// Storage for the initial value of some important preferences
	// This is needed to detect when one of them has changed, so that the concerned component can be warned
	bool     mInitAdvancedReload; /**< Initial value for advanced reload option */
	bool     mInitAutoReload; /**< Inititial value for auto reload option */
	bool     mInitEnableTrayIcon; /**< Initial value for tray icon option */
	bool     mInstantFrames; /**< Initial value for instant frames option */
	bool     mInitOverrideTz; /**< Initial value for overriding of timezone */
	bool     mInitIgnoreAsynchronousClocks; /**< Initial value for ignoring asynchronous clocks */
	wxInt32  mInitTimezone; /**< Initial value for timezone */
	wxUint32 mInitAutoReloadFrequency; /**< Initial value for auto reload frequency */
	wxUint32 mInitETADisplayStyle; /**< Initial value for ETA display style */
	wxUint32 mInitPPDDisplayStyle; /**< Initial value for PPD display style */

	/**
	 * Constructor.
	 * Contructs a Preferences Dialog of type wxDialog with a title of "Preferences".
	 * @param parent The parent window of type wxWindow
	 **/
	PreferencesDialog(wxWindow* parent);

	/**
	 * Destructor.
	 **/
	~PreferencesDialog(void);

	/**
	 * Create the tab containing general preferences.
	 * @param parent The notebook object.
	 * @return A wxPanel object.
	 **/
	wxPanel* CreateGeneralTab(wxBookCtrlBase* parent);

	/**
	 * Create the tab containing monitoring preferences.
	 * @param parent The notebook object.
	 * @return A wxPanel object.
	 **/
	wxPanel* CreateMonitoringTab(wxBookCtrlBase* parent);

	/**
	 * Create the tab containing networking preferences.
	 * @param parent The notebook object.
	 * @return A wxPanel object.
	 **/
	wxPanel* CreateNetworkingTab(wxBookCtrlBase* parent);

	/**
	 * Create the tab containing advanced preferences.
	 * @param parent The notebook object.
	 * @return A wxPanel object.
	 **/
	wxPanel* CreateAdvancedTab(wxBookCtrlBase* parent);

	/**
	 * Create the tab containing system preferences.
	 * @param parent The notebook object.
	 * @return A wxPanel object.
	 **/
	wxPanel* CreateSystemTab(wxBookCtrlBase* parent);
	//wxPanel* CreateFahinfoTab(wxBookCtrlBase* parent);

	/**
	 * Create the tab containing web application preferences.
	 * @param parent The notebook object.
	 * @return A wxPanel object.
	 **/
	wxPanel* CreateWebAppTab(wxBookCtrlBase* parent);

	/**
	 * Give the correct value to each controls using the preferences.
	 **/
	void LoadPreferences(void);

	/**
	 * Save the value of each control using the preferences.
	 **/
	void SavePreferences(void);

	/**
	 * Event: Manages the 'ok' button
	 * Save preferences before exiting the dialog box.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnOkButton(wxCommandEvent& event);

	/**
	 * Event: Manages the 'browse' button for local data file.
	 * Show file chooser dialog box.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnBrowseButton(wxCommandEvent& event);

	/**
	 * Event: Manages the 'browse' button for web application output.
	 * Show file chooser dialog box.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnWebAppBrowseButton(wxCommandEvent& event);

	/**
	 * Event: Manages the 'browse' button for simple web output.
	 * Show file chooser dialog box.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnSimpleWebBrowseButton(wxCommandEvent& event);

	/**
	 * Event: Manages the 'browse' button for simple text output.
	 * Show file chooser dialog box.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnSimpleTextBrowseButton(wxCommandEvent& event);

	/**
	 * Event: Manages the checkbox clicks.
	 * Enable/Disable fields associated with some checkboxes.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnCheckboxes(wxCommandEvent& event);

	/**
	 * Event: Manages the choicebox alterations.
	 * Alter fields when choicebox values are changed.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnChoices(wxCommandEvent& event);

	#ifdef __WXMAC__ 
	void OnClose(wxCloseEvent& event); 
	#endif 

public:
	// Singleton pattern
	/**
	 * Retrieve the instance of PreferencessDialog.
	 * If an instance doesn't already exist, create it.
	 * @param parent The parent windows of type wxWindow.
	 * @return An instance of class PreferencesDialog.
	 **/
	static PreferencesDialog* GetInstance(wxWindow* parent);

	/**
	 * Destroy the instance of PreferencesDialog.
	 * Only if an instance exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Show the Preferences Dialog.
	 * Opens a modal window and centres it.
	 **/
	int ShowModal(void);


private:
	DECLARE_EVENT_TABLE()

};


#endif /* _PREFERENCESDIALOG_H */
