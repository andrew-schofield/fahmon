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
 * \file preferencesDialog.cpp
 * The preferences dialog.
 * Creates the dialog to reconfigure FahMon.
 * \author Fran�ois Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "preferencesDialog.h"

#include "client.h"
#include "wx/valtext.h"
#include "wx/filedlg.h"
#include "wx/string.h"
#include "mainDialog.h"
#include "httpDownloader.h"
#include "preferencesManager.h"
#include "tools.h"
#include "webMonitor.h"


// Identifiers for the controls
enum _CONTROL_ID
{
	// Checkboxes
	CHK_AUTORELOAD,
	CHK_USEPROXY,
	CHK_PROXYAUTHENTICATION,
	CHK_USEALTERNATEPROJECTSOURCE,
	CHK_USELOCALFILE,
	CHC_FILEMANAGER,
	CHK_PPDTYPE,
	CHK_TZOVERRIDE,
	CHK_ADVANCEDRELOAD,
	CHK_USEWEBAPP,
	CHK_USESIMPLEWEB,
	CHK_USESIMPLETEXT,
	BTN_BROWSE_WEBAPP,
	BTN_BROWSE_SIMPLEWEB,
	BTN_BROWSE_SIMPLETEXT,
	BTN_BROWSE_WEBAPP_TEMPLATE,
	BTN_BROWSE_SIMPLEWEB_TEMPLATE,
	BTN_BROWSE_SIMPLETEXT_TEMPLATE,
	BTN_BROWSE = wxID_HIGHEST
};


// Events processed by this class
BEGIN_EVENT_TABLE(PreferencesDialog, wxDialog)

	// --- Buttons
	EVT_BUTTON(BTN_BROWSE,            PreferencesDialog::OnBrowseButton)
	EVT_BUTTON(BTN_BROWSE_WEBAPP,     PreferencesDialog::OnWebAppBrowseButton)
	EVT_BUTTON(BTN_BROWSE_SIMPLEWEB,  PreferencesDialog::OnSimpleWebBrowseButton)
	EVT_BUTTON(BTN_BROWSE_SIMPLETEXT, PreferencesDialog::OnSimpleTextBrowseButton)
	EVT_BUTTON(BTN_BROWSE_WEBAPP_TEMPLATE,     PreferencesDialog::OnWebAppTemplateBrowseButton)
	EVT_BUTTON(BTN_BROWSE_SIMPLEWEB_TEMPLATE,  PreferencesDialog::OnSimpleWebTemplateBrowseButton)
	EVT_BUTTON(BTN_BROWSE_SIMPLETEXT_TEMPLATE, PreferencesDialog::OnSimpleTextTemplateBrowseButton)
	EVT_BUTTON(wxID_OK,               PreferencesDialog::OnOkButton)

	// --- Checkboxes
	EVT_CHECKBOX(CHK_USEPROXY,                  PreferencesDialog::OnCheckboxes)
	EVT_CHECKBOX(CHK_AUTORELOAD,                PreferencesDialog::OnCheckboxes)
	EVT_CHECKBOX(CHK_ADVANCEDRELOAD,            PreferencesDialog::OnCheckboxes)
	EVT_CHECKBOX(CHK_PROXYAUTHENTICATION,       PreferencesDialog::OnCheckboxes)
	EVT_CHECKBOX(CHK_USEALTERNATEPROJECTSOURCE, PreferencesDialog::OnCheckboxes)
	EVT_CHECKBOX(CHK_USELOCALFILE,              PreferencesDialog::OnCheckboxes)
	EVT_CHECKBOX(CHK_TZOVERRIDE,                PreferencesDialog::OnCheckboxes)
	EVT_CHECKBOX(CHK_USEWEBAPP,                 PreferencesDialog::OnCheckboxes)
	EVT_CHECKBOX(CHK_USESIMPLEWEB,              PreferencesDialog::OnCheckboxes)
	EVT_CHECKBOX(CHK_USESIMPLETEXT,             PreferencesDialog::OnCheckboxes)
	EVT_CHOICE(CHC_FILEMANAGER,                 PreferencesDialog::OnChoices)
	// EVT_CHOICE(CHK_PPDTYPE, PreferencesDialog::OnChoices)
	#ifdef __WXMAC__
 	EVT_CLOSE(PreferencesDialog::OnClose)
 	#endif

END_EVENT_TABLE()


// The single instance of PreferencesDialog accross the application
PreferencesDialog* PreferencesDialog::mInstance = NULL;


PreferencesDialog::PreferencesDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxString::Format(_("Preferences / %s"), wxT(FMC_PRODUCT)))
{
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *mainSizer;
#ifndef __WXMAC__
	wxBoxSizer *buttonsSizer;
	wxNotebook *noteBook;
#else
	wxChoicebook *noteBook;
#endif

	// Preferences are divided into groups, thanks to a wxNoteBook (tabbed control)
#ifndef __WXMAC__
	noteBook = new wxNotebook(this, wxID_ANY);
#else
	noteBook = new wxChoicebook(this, wxID_ANY);
#endif

	noteBook->AddPage(CreateGeneralTab(noteBook),    _("General"));
	noteBook->AddPage(CreateMonitoringTab(noteBook), _("Monitoring"));
	noteBook->AddPage(CreateNetworkingTab(noteBook), _("Networking"));
	noteBook->AddPage(CreateAdvancedTab(noteBook),   _("Advanced"));
	noteBook->AddPage(CreateSystemTab(noteBook),     _("System"));
	//noteBook->AddPage(CreateFahinfoTab(noteBook),    _T("fahinfo.org"));
	noteBook->AddPage(CreateWebApp1Tab(noteBook),     _("WebApp 1"));
	noteBook->AddPage(CreateWebApp2Tab(noteBook),     _("WebApp 2"));

	// Buttons 'Ok' and 'Cancel' are right-aligned
#ifndef __WXMAC__
	buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

	buttonsSizer->Add(new wxButton(this, wxID_CANCEL), 0, wxALIGN_RIGHT);
	buttonsSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	buttonsSizer->Add(new wxButton(this, wxID_OK), 0, wxALIGN_RIGHT);
#endif

	// Construct the dialog
	mainSizer = new wxBoxSizer(wxVERTICAL);

	mainSizer->Add(noteBook, 1, wxEXPAND);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
#ifndef __WXMAC__
	mainSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT);
#endif

	// The final sizer
	topLevelSizer = new wxBoxSizer(wxVERTICAL);

	topLevelSizer->Add(mainSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	SetSizer(topLevelSizer);
	topLevelSizer->Fit(this);
}


PreferencesDialog::~PreferencesDialog(void)
{
}


PreferencesDialog* PreferencesDialog::GetInstance(wxWindow* parent)
{
	if(mInstance == NULL)
	{
		mInstance = new PreferencesDialog(parent);
	}

	return mInstance;
}


void PreferencesDialog::DestroyInstance(void)
{
	if(mInstance != NULL)
	{
		mInstance->Destroy();
		mInstance = NULL;
	}
}


inline wxPanel* PreferencesDialog::CreateGeneralTab(wxBookCtrlBase* parent)
{
	wxPanel    *panel;
	wxBoxSizer *sizer;
	wxBoxSizer *topLevelSizer;

	panel                               = new wxPanel(parent);
	sizer                               = new wxBoxSizer(wxVERTICAL);
	topLevelSizer                       = new wxBoxSizer(wxVERTICAL);
	#ifndef __WXMAC__
	mGeneralEnableTrayIcon              = new wxCheckBox(panel, wxID_ANY, _("Enable system tray icon"));
	#endif
	mGeneralStartMinimised              = new wxCheckBox(panel, wxID_ANY, _("Start minimized"));
	mGeneralCollectXYZFiles             = new wxCheckBox(panel, wxID_ANY, _("Collect .xyz files"));
	mGeneralAutoUpdateProjectsDatabase  = new wxCheckBox(panel, wxID_ANY, _("Auto update projects database when needed"));
	mGeneralKeepInaccessibleClientsLast = new wxCheckBox(panel, wxID_ANY, _("Always list inaccessible clients last"));
	mGeneralUpdateCheck                 = new wxCheckBox(panel, wxID_ANY, _("Check for FahMon updates on startup"));

	#ifndef __WXMAC__
	sizer->AddStretchSpacer();
	sizer->Add(mGeneralEnableTrayIcon, 0, wxALIGN_LEFT | wxALIGN_TOP);
	#endif
	sizer->AddStretchSpacer();
	sizer->Add(mGeneralCollectXYZFiles, 0, wxALIGN_LEFT | wxALIGN_TOP );
	sizer->AddStretchSpacer();
	sizer->Add(mGeneralAutoUpdateProjectsDatabase, 0, wxALIGN_LEFT | wxALIGN_TOP);
	sizer->AddStretchSpacer();
	sizer->Add(mGeneralKeepInaccessibleClientsLast, 0, wxALIGN_LEFT | wxALIGN_TOP);
	sizer->AddStretchSpacer();
	sizer->Add(mGeneralStartMinimised, 0, wxALIGN_LEFT | wxALIGN_TOP);
	sizer->AddStretchSpacer();
	sizer->Add(mGeneralUpdateCheck, 0, wxALIGN_LEFT | wxALIGN_TOP);
	sizer->AddStretchSpacer();

	topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	panel->SetSizer(topLevelSizer);

	return panel;
}


inline wxPanel* PreferencesDialog::CreateMonitoringTab(wxBookCtrlBase* parent)
{
	wxPanel    *panel;
	wxBoxSizer *sizer;
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *sizerETA;
	wxBoxSizer *sizerAutoReload;
	wxBoxSizer *sizerPPDType;
	wxBoxSizer *sizerAsynchrony;
	const wxString    etaFormats[3] = {_("A date (dd/mm)"), _("A date (mm/dd)"), _("Time left")};    // The order *MUST* correspond to the one used for the definition of ETA_DisplayStyle
	const wxString    ppdFormats[4] = {_("All frames"), _("Last frame only"), _("Last 3 frames"), _("Effective rate")};    // The order *MUST* correspond to the one use for the definition of PPD_DisplayStyle

	panel                          = new wxPanel(parent);
	sizer                          = new wxBoxSizer(wxVERTICAL);
	sizerETA                       = new wxBoxSizer(wxHORIZONTAL);
	topLevelSizer                  = new wxBoxSizer(wxVERTICAL);
	sizerAutoReload                = new wxBoxSizer(wxHORIZONTAL);
	sizerPPDType                   = new wxBoxSizer(wxHORIZONTAL);
	sizerAsynchrony                = new wxBoxSizer(wxHORIZONTAL);

	mMonitoringAutoReload          = new wxCheckBox(panel, CHK_AUTORELOAD, _("Auto reload clients"));
	mMonitoringAutoReloadInt       = new wxStaticText(panel, wxID_ANY, wxString::Format(_T("%s "),  _("Reload interval (mn)")));
	mMonitoringAdvancedReload      = new wxCheckBox(panel, CHK_ADVANCEDRELOAD, _("Use experimental reload system"));
	mMonitoringETADisplayStyle     = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, etaFormats);
	mMonitoringAutoReloadFrequency = new wxSpinCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 5);
	mMonitoringPPDType             = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, ppdFormats);
	mMonitoringIgnoreAsynchrony    = new wxCheckBox(panel, wxID_ANY, _("Ignore asynchronous clocks"));

	sizerETA->Add(new wxStaticText(panel, wxID_ANY, wxString::Format(_T("%s "),  _("Display dates as"))), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizerETA->Add(mMonitoringETADisplayStyle, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	sizerAutoReload->Add(mMonitoringAutoReloadInt, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizerAutoReload->Add(mMonitoringAutoReloadFrequency, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	sizerPPDType->Add(new wxStaticText(panel, wxID_ANY, wxString::Format(_T("%s  "),  _("Calculate PPD based on"))), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizerPPDType->Add(mMonitoringPPDType, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	sizerAsynchrony->Add(mMonitoringIgnoreAsynchrony, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	sizer->AddStretchSpacer();
	sizer->Add(mMonitoringAutoReload, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizer->AddStretchSpacer();
	sizer->Add(sizerAutoReload, 0, wxALIGN_LEFT);
	sizer->AddStretchSpacer();
	sizer->Add(mMonitoringAdvancedReload, 0, wxALIGN_LEFT);
	sizer->AddStretchSpacer();
	sizer->Add(sizerETA, 0, wxALIGN_LEFT);
	sizer->AddStretchSpacer();
	sizer->Add(sizerPPDType, 0, wxALIGN_LEFT);
	sizer->AddStretchSpacer();
	sizer->Add(sizerAsynchrony, 0, wxALIGN_LEFT);
	sizer->AddStretchSpacer();

	topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	panel->SetSizer(topLevelSizer);

	return panel;
}


inline wxPanel* PreferencesDialog::CreateNetworkingTab(wxBookCtrlBase* parent)
{
	wxPanel    *panel;
	wxBoxSizer *sizer;
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *proxyAddressSizer;
	wxBoxSizer *proxyAuthenticationSizer;

	panel                             = new wxPanel(parent);
	sizer                             = new wxBoxSizer(wxVERTICAL);
	topLevelSizer                     = new wxBoxSizer(wxVERTICAL);
	proxyAddressSizer                 = new wxBoxSizer(wxHORIZONTAL);
	proxyAuthenticationSizer          = new wxBoxSizer(wxHORIZONTAL);

	mNetworkingUseProxy               = new wxCheckBox(panel, CHK_USEPROXY, _("Use a proxy for HTTP connections"));
	mNetworkingProxyAddress           = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mNetworkingProxyPort              = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC));
	mNetworkingUseProxyAuthentication = new wxCheckBox(panel, CHK_PROXYAUTHENTICATION, _("Proxy requires authentication"));
	mNetworkingProxyUsername          = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mNetworkingProxyPassword          = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	mNetworkingLabelAddress           = new wxStaticText(panel, wxID_ANY, _("Address:"));
	mNetworkingLabelPort              = new wxStaticText(panel, wxID_ANY, _("Port:"));
	mNetworkingLabelUsername          = new wxStaticText(panel, wxID_ANY, _("Proxy\nUsername:"));
	mNetworkingLabelPassword          = new wxStaticText(panel, wxID_ANY, _("Proxy\nPassword:"));

	proxyAddressSizer->Add(mNetworkingLabelAddress, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	proxyAddressSizer->Add(mNetworkingProxyAddress, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	proxyAddressSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	proxyAddressSizer->Add(mNetworkingLabelPort, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	proxyAddressSizer->Add(mNetworkingProxyPort, 1, wxALIGN_CENTER_VERTICAL);

	proxyAuthenticationSizer->Add(mNetworkingLabelUsername, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	proxyAuthenticationSizer->Add(mNetworkingProxyUsername, 1, wxALIGN_CENTER_VERTICAL);
	proxyAuthenticationSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	proxyAuthenticationSizer->Add(mNetworkingLabelPassword, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	proxyAuthenticationSizer->Add(mNetworkingProxyPassword, 1, wxALIGN_CENTER_VERTICAL);

	sizer->AddStretchSpacer();
	sizer->Add(mNetworkingUseProxy, 0, wxALIGN_LEFT);
	sizer->AddSpacer(FMC_GUI_SPACING_LOW);
	sizer->Add(proxyAddressSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddSpacer(FMC_GUI_SPACING_LOW);
	sizer->Add(mNetworkingUseProxyAuthentication, 0, wxALIGN_LEFT);
	sizer->AddSpacer(FMC_GUI_SPACING_LOW);
	sizer->Add(proxyAuthenticationSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();

	topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	panel->SetSizer(topLevelSizer);

	return panel;
}


inline wxPanel* PreferencesDialog::CreateAdvancedTab(wxBookCtrlBase* parent)
{
	wxPanel    *panel;
	wxBoxSizer *sizer;
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *AddressSizer;
	wxBoxSizer *LocalFileSizer;
	wxBoxSizer *LocationSizer;

	panel                                           = new wxPanel(parent);
	sizer                                           = new wxBoxSizer(wxVERTICAL);
	topLevelSizer                                   = new wxBoxSizer(wxVERTICAL);
	AddressSizer                                    = new wxBoxSizer(wxHORIZONTAL);
	LocalFileSizer                                  = new wxBoxSizer(wxHORIZONTAL);
	LocationSizer                                   = new wxBoxSizer(wxHORIZONTAL);

	mAdvancedUseAlternateProjectSource              = new wxCheckBox(panel, CHK_USEALTERNATEPROJECTSOURCE, _("Use the following settings for new project downloads"));
	mAdvancedAlternateProjectSourceLocationAddress  = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mAdvancedLabelLocationAddress                   = new wxStaticText(panel, wxID_ANY, _("Address:"));
	mAdvancedUseLocalFile                           = new wxCheckBox(panel, CHK_USELOCALFILE, _("Use a local file for project data"));
	mAdvancedLocalFileLocation                      = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mAdvancedLabelLocalFile                         = new wxStaticText(panel, wxID_ANY, _("Filename:"));
#ifndef __WXMAC__
	mAdvancedLocationChooser                        = new wxButton(panel, BTN_BROWSE, wxT("..."), wxDefaultPosition, wxSize(26, 26));
#else
	mAdvancedLocationChooser                        = new wxButton(panel, BTN_BROWSE, _("Choose"), wxDefaultPosition);
#endif

	LocalFileSizer->Add(mAdvancedLabelLocalFile, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	LocalFileSizer->Add(mAdvancedLocalFileLocation, 1, wxALIGN_CENTER_VERTICAL);
	LocalFileSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	LocalFileSizer->Add(mAdvancedLocationChooser, 0, wxALIGN_CENTER_VERTICAL);

	AddressSizer->Add(mAdvancedLabelLocationAddress, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	AddressSizer->Add(mAdvancedAlternateProjectSourceLocationAddress, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);

	sizer->AddStretchSpacer();
	sizer->Add(mAdvancedUseAlternateProjectSource, 0, wxALIGN_LEFT);
	sizer->AddSpacer(FMC_GUI_SPACING_LOW);
	sizer->Add(AddressSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();
	sizer->Add(mAdvancedUseLocalFile, 0, wxALIGN_LEFT);
	sizer->Add(LocalFileSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();

	topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	panel->SetSizer(topLevelSizer);

	return panel;
}


inline wxPanel* PreferencesDialog::CreateSystemTab(wxBookCtrlBase* parent)
{
	wxPanel    *panel;
	wxBoxSizer *sizer;
	wxBoxSizer *browserSizer;
	wxBoxSizer *filemanagerSizer;
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *otherSizer;
	wxBoxSizer *sizerTZOverride;

	panel                   = new wxPanel(parent);
	sizer                   = new wxBoxSizer(wxVERTICAL);
	browserSizer            = new wxBoxSizer(wxHORIZONTAL);
	filemanagerSizer        = new wxBoxSizer(wxHORIZONTAL);
	otherSizer              = new wxBoxSizer(wxHORIZONTAL);
	sizerTZOverride         = new wxBoxSizer(wxHORIZONTAL);
	topLevelSizer           = new wxBoxSizer(wxVERTICAL);
	mSystemBrowser          = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mSystemBrowserLabel     = new wxStaticText(panel, wxID_ANY, _("Web Browser:"));
	mSystemFileManagerLabel = new wxStaticText(panel, wxID_ANY, _("File Manager:"));
	mSystemOtherFM          = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mSystemOtherFMLabel     = new wxStaticText(panel, wxID_ANY, _("File Manager Command:"));
	mSystemOverrideTimezone = new wxCheckBox(panel, CHK_TZOVERRIDE, wxString::Format(_T("%s  "),  _("Manually set timezone to UTC +")));
	mSystemTZ               = new wxSpinCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -24, 24, 0);

	#ifdef _FAHMON_WIN32_
	const wxString    fileManagers[2] = {_("Windows Explorer"), _("Other")};
	mSystemFileManager     = new wxChoice(panel, CHC_FILEMANAGER, wxDefaultPosition, wxDefaultSize, 2, fileManagers);
	#elif __WXGTK__
	const wxString    fileManagers[5] = {wxT("Konqueror (KDE 3)"), wxT("Dolphin (KDE 4)"), wxT("Nautilus (Gnome)"), wxT("Thunar (Xfce 4.4)"), _("Other")};
	mSystemFileManager     = new wxChoice(panel, CHC_FILEMANAGER, wxDefaultPosition, wxDefaultSize, 5, fileManagers);
	#elif __WXMAC__
	const wxString fileManagers[2] = {_("Finder"), _("Other")};
	mSystemFileManager     = new wxChoice(panel, CHC_FILEMANAGER, wxDefaultPosition, wxDefaultSize, 2, fileManagers);
	#endif

	sizer->AddStretchSpacer();

	#ifdef __WXGTK__
	browserSizer->Add(mSystemBrowserLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	browserSizer->Add(mSystemBrowser, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);

	sizer->Add(browserSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();
	#else
	mSystemBrowser->Hide();
	mSystemBrowserLabel->Hide();
	#endif

	filemanagerSizer->Add(mSystemFileManagerLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	filemanagerSizer->Add(mSystemFileManager, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);

	sizer->Add(filemanagerSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();

	otherSizer->Add(mSystemOtherFMLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	otherSizer->Add(mSystemOtherFM, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);

	sizer->Add(otherSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();

	sizerTZOverride->Add(mSystemOverrideTimezone, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizerTZOverride->Add(mSystemTZ, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	sizer->Add(sizerTZOverride, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();

	topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	panel->SetSizer(topLevelSizer);

	return panel;
}


/**
* Create the tab containing fahinfo.org integration preferences
**/
/*inline wxPanel* PreferencesDialog::CreateFahinfoTab(wxBookCtrlBase* parent)
{
	wxPanel    *panel;

	panel                   = new wxPanel(parent);

	return panel;
}*/


inline wxPanel* PreferencesDialog::CreateWebApp1Tab(wxBookCtrlBase* parent)
{
	wxPanel    *panel;
	wxBoxSizer *sizer;
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *webAppSizer;
	wxBoxSizer *webAppLocationSizer;
	wxBoxSizer *simpleWebSizer;
	wxBoxSizer *simpleWebLocationSizer;
	wxBoxSizer *simpleTextSizer;
	wxBoxSizer *simpleTextLocationSizer;

	panel                           = new wxPanel(parent);
	sizer                           = new wxBoxSizer(wxVERTICAL);
	topLevelSizer                   = new wxBoxSizer(wxVERTICAL);
	webAppSizer                     = new wxBoxSizer(wxHORIZONTAL);
	webAppLocationSizer             = new wxBoxSizer(wxHORIZONTAL);
	simpleWebSizer                  = new wxBoxSizer(wxHORIZONTAL);
	simpleWebLocationSizer          = new wxBoxSizer(wxHORIZONTAL);
	simpleTextSizer                 = new wxBoxSizer(wxHORIZONTAL);
	simpleTextLocationSizer         = new wxBoxSizer(wxHORIZONTAL);

	mWebAppUseWebApp                 = new wxCheckBox(panel, CHK_USEWEBAPP, _("Export Web Application"));
	mWebAppWebAppLocation            = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mWebAppWebAppLabel               = new wxStaticText(panel, wxID_ANY, _("Filename:"));
#ifndef __WXMAC__
	mWebAppWebAppLocationChooser             = new wxButton(panel, BTN_BROWSE_WEBAPP, wxT("..."), wxDefaultPosition, wxSize(26, 26));
#else
	mWebAppWebAppLocationChooser             = new wxButton(panel, BTN_BROWSE_WEBAPP, _("Choose"), wxDefaultPosition);
#endif

	mWebAppUseSimpleWeb               = new wxCheckBox(panel, CHK_USESIMPLEWEB, _("Export Simple Web page"));
	mWebAppSimpleWebLocation          = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mWebAppSimpleWebLabel             = new wxStaticText(panel, wxID_ANY, _("Filename:"));
#ifndef __WXMAC__
	mWebAppSimpleWebLocationChooser          = new wxButton(panel, BTN_BROWSE_SIMPLEWEB, wxT("..."), wxDefaultPosition, wxSize(26, 26));
#else
	mWebAppSimpleWebLocationChooser          = new wxButton(panel, BTN_BROWSE_SIMPLEWEB, _("Choose"), wxDefaultPosition);
#endif

	mWebAppUseSimpleText               = new wxCheckBox(panel, CHK_USESIMPLETEXT, _("Export Simple Text file"));
	mWebAppSimpleTextLocation          = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mWebAppSimpleTextLabel             = new wxStaticText(panel, wxID_ANY, _("Filename:"));
#ifndef __WXMAC__
	mWebAppSimpleTextLocationChooser         = new wxButton(panel, BTN_BROWSE_SIMPLETEXT, wxT("..."), wxDefaultPosition, wxSize(26, 26));
#else
	mWebAppSimpleTextLocationChooser         = new wxButton(panel, BTN_BROWSE_SIMPLETEXT, _("Choose"), wxDefaultPosition);
#endif

	webAppSizer->Add(mWebAppUseWebApp, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	webAppLocationSizer->Add(mWebAppWebAppLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	webAppLocationSizer->Add(mWebAppWebAppLocation, 1, wxALIGN_CENTER_VERTICAL);
	webAppLocationSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	webAppLocationSizer->Add(mWebAppWebAppLocationChooser, 0, wxALIGN_CENTER_VERTICAL);

	simpleWebSizer->Add(mWebAppUseSimpleWeb, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	simpleWebLocationSizer->Add(mWebAppSimpleWebLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	simpleWebLocationSizer->Add(mWebAppSimpleWebLocation, 1, wxALIGN_CENTER_VERTICAL);
	simpleWebLocationSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	simpleWebLocationSizer->Add(mWebAppSimpleWebLocationChooser, 0, wxALIGN_CENTER_VERTICAL);

	simpleTextSizer->Add(mWebAppUseSimpleText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	simpleTextLocationSizer->Add(mWebAppSimpleTextLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	simpleTextLocationSizer->Add(mWebAppSimpleTextLocation, 1, wxALIGN_CENTER_VERTICAL);
	simpleTextLocationSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	simpleTextLocationSizer->Add(mWebAppSimpleTextLocationChooser, 0, wxALIGN_CENTER_VERTICAL);

	sizer->AddStretchSpacer();
	sizer->Add(webAppSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->Add(webAppLocationSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();
	sizer->Add(simpleWebSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->Add(simpleWebLocationSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();
	sizer->Add(simpleTextSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->Add(simpleTextLocationSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();

	topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	panel->SetSizer(topLevelSizer);

	return panel;
}


inline wxPanel* PreferencesDialog::CreateWebApp2Tab(wxBookCtrlBase* parent)
{
	wxPanel    *panel;
	wxBoxSizer *sizer;
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *webAppSizer;
	wxBoxSizer *webAppLocationTemplateSizer;
	wxBoxSizer *simpleWebSizer;
	wxBoxSizer *simpleWebLocationTemplateSizer;
	wxBoxSizer *simpleTextSizer;
	wxBoxSizer *simpleTextLocationTemplateSizer;

	panel                           = new wxPanel(parent);
	sizer                           = new wxBoxSizer(wxVERTICAL);
	topLevelSizer                   = new wxBoxSizer(wxVERTICAL);
	webAppSizer                     = new wxBoxSizer(wxHORIZONTAL);
	webAppLocationTemplateSizer     = new wxBoxSizer(wxHORIZONTAL);
	simpleWebSizer                  = new wxBoxSizer(wxHORIZONTAL);
	simpleWebLocationTemplateSizer  = new wxBoxSizer(wxHORIZONTAL);
	simpleTextSizer                 = new wxBoxSizer(wxHORIZONTAL);
	simpleTextLocationTemplateSizer = new wxBoxSizer(wxHORIZONTAL);

	mWebAppWebAppTemplateLabel       = new wxStaticText(panel, wxID_ANY, _("Custom Web Application Template"));
	mWebAppWebAppTemplateLocation    = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mWebAppWebAppFilenameLabel       = new wxStaticText(panel, wxID_ANY, _("Filename:"));
#ifndef __WXMAC__
	mWebAppWebAppTemplateLocationChooser     = new wxButton(panel, BTN_BROWSE_WEBAPP_TEMPLATE, wxT("..."), wxDefaultPosition, wxSize(26, 26));
#else
	mWebAppWebAppTemplateLocationChooser     = new wxButton(panel, BTN_BROWSE_WEBAPP_TEMPLATE, _("Choose"), wxDefaultPosition);
#endif

	mWebAppSimpleWebTemplateLabel     = new wxStaticText(panel, wxID_ANY, _("Custom Simple Web Template"));
	mWebAppSimpleWebTemplateLocation  = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mWebAppSimpleWebFilenameLabel     = new wxStaticText(panel, wxID_ANY, _("Filename:"));
#ifndef __WXMAC__
	mWebAppSimpleWebTemplateLocationChooser  = new wxButton(panel, BTN_BROWSE_SIMPLEWEB_TEMPLATE, wxT("..."), wxDefaultPosition, wxSize(26, 26));
#else
	mWebAppSimpleWebLocationTemplateChooser  = new wxButton(panel, BTN_BROWSE_SIMPLEWEB_TEMPLATE, _("Choose"), wxDefaultPosition);
#endif

	mWebAppSimpleTextTemplateLabel     = new wxStaticText(panel, wxID_ANY, _("Custom Simple Text Template"));
	mWebAppSimpleTextTemplateLocation  = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition);
	mWebAppSimpleTextFilenameLabel     = new wxStaticText(panel, wxID_ANY, _("Filename:"));
#ifndef __WXMAC__
	mWebAppSimpleTextTemplateLocationChooser = new wxButton(panel, BTN_BROWSE_SIMPLETEXT_TEMPLATE, wxT("..."), wxDefaultPosition, wxSize(26, 26));
#else
	mWebAppSimpleTextTemplateLocationChooser = new wxButton(panel, BTN_BROWSE_SIMPLETEXT_TEMPLATE, _("Choose"), wxDefaultPosition);
#endif

	webAppSizer->Add(mWebAppWebAppTemplateLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	webAppLocationTemplateSizer->Add(mWebAppWebAppFilenameLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	webAppLocationTemplateSizer->Add(mWebAppWebAppTemplateLocation, 1, wxALIGN_CENTER_VERTICAL);
	webAppLocationTemplateSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	webAppLocationTemplateSizer->Add(mWebAppWebAppTemplateLocationChooser, 0, wxALIGN_CENTER_VERTICAL);

	simpleWebSizer->Add(mWebAppSimpleWebTemplateLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	simpleWebLocationTemplateSizer->Add(mWebAppSimpleWebFilenameLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	simpleWebLocationTemplateSizer->Add(mWebAppSimpleWebTemplateLocation, 1, wxALIGN_CENTER_VERTICAL);
	simpleWebLocationTemplateSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	simpleWebLocationTemplateSizer->Add(mWebAppSimpleWebTemplateLocationChooser, 0, wxALIGN_CENTER_VERTICAL);

	simpleTextSizer->Add(mWebAppSimpleTextTemplateLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	simpleTextLocationTemplateSizer->Add(mWebAppSimpleTextFilenameLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	simpleTextLocationTemplateSizer->Add(mWebAppSimpleTextTemplateLocation, 1, wxALIGN_CENTER_VERTICAL);
	simpleTextLocationTemplateSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	simpleTextLocationTemplateSizer->Add(mWebAppSimpleTextTemplateLocationChooser, 0, wxALIGN_CENTER_VERTICAL);

	sizer->AddStretchSpacer();
	sizer->Add(webAppSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->Add(webAppLocationTemplateSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();
	sizer->Add(simpleWebSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->Add(simpleWebLocationTemplateSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();
	sizer->Add(simpleTextSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->Add(simpleTextLocationTemplateSizer, 0, wxALIGN_LEFT | wxEXPAND);
	sizer->AddStretchSpacer();

	topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	panel->SetSizer(topLevelSizer);

	return panel;
}


int PreferencesDialog::ShowModal(void)
{
	LoadPreferences();
	Center();

	return wxDialog::ShowModal();
}


inline void PreferencesDialog::LoadPreferences(void)
{
	bool     useProxy;
	bool     isCollectingXYZFiles;
	bool     autoUpdateProjects;
	bool     keepInaccessibleLast;
	bool     useProxyAuthentication;
	bool     useAlternate;
	bool     useLocalFile;
	bool     startMinimised;
	bool     useWebApp;
	bool     useSimpleWeb;
	bool     useSimpleText;
	bool     updateCheck;
	wxUint32 proxyPort;
	wxString proxyAddress;
	wxString proxyUsername;
	wxString proxyPassword;
	wxString browser;
	wxString projectLocationAddress;
	wxString projectLocalFile;
	wxString filemanager;
	wxString webAppLocation;
	wxString simpleWebLocation;
	wxString simpleTextLocation;
	wxString webAppTemplateLocation;
	wxString simpleWebTemplateLocation;
	wxString simpleTextTemplateLocation;

	// -----===== General preferences =====-----
	_PrefsGetBool(PREF_FAHCLIENT_COLLECTXYZFILES,     isCollectingXYZFiles);
	_PrefsGetBool(PREF_MAINDIALOG_AUTOUPDATEPROJECTS, autoUpdateProjects);
	#ifndef __WXMAC__
	_PrefsGetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON,   mInitEnableTrayIcon);
	#endif
	_PrefsGetBool(PREF_LISTCLIENTS_KEEP_DEAD_LAST,    keepInaccessibleLast);
	_PrefsGetBool(PREF_MAINDIALOG_START_MINIMISED,    startMinimised);
	_PrefsGetBool(PREF_MAINDIALOG_UPDATE_CHECK,       updateCheck);


	mGeneralCollectXYZFiles->SetValue(isCollectingXYZFiles);
	#ifndef __WXMAC__
	mGeneralEnableTrayIcon->SetValue(mInitEnableTrayIcon);
	#endif
	mGeneralAutoUpdateProjectsDatabase->SetValue(autoUpdateProjects);
	mGeneralKeepInaccessibleClientsLast->SetValue(keepInaccessibleLast);
	mGeneralStartMinimised->SetValue(startMinimised);
	mGeneralUpdateCheck->SetValue(updateCheck);

	// -----===== Monitoring preferences =====-----
	_PrefsGetBool(PREF_MAINDIALOG_ADVANCEDRELOAD,      mInitAdvancedReload);
	_PrefsGetBool(PREF_MAINDIALOG_AUTORELOAD,          mInitAutoReload);
	_PrefsGetUint(PREF_MAINDIALOG_AUTORELOADFREQUENCY, mInitAutoReloadFrequency);
	_PrefsGetUint(PREF_ETA_DISPLAYSTYLE,               mInitETADisplayStyle);
	_PrefsGetUint(PREF_PPD_DISPLAYSTYLE,               mInitPPDDisplayStyle);
	_PrefsGetBool(PREF_IGNORE_ASYNCHRONY,              mInitIgnoreAsynchronousClocks);

	mMonitoringAdvancedReload->SetValue(mInitAdvancedReload);
	mMonitoringAutoReload->SetValue(mInitAutoReload);
	mMonitoringAutoReloadFrequency->SetValue(wxString::Format(wxT("%u"), mInitAutoReloadFrequency));
	mMonitoringAutoReloadFrequency->Enable(!mInitAdvancedReload);
	mMonitoringETADisplayStyle->Select(mInitETADisplayStyle);
	mMonitoringPPDType->Select(mInitPPDDisplayStyle);
	mMonitoringIgnoreAsynchrony->SetValue(mInitIgnoreAsynchronousClocks);

	mMonitoringAdvancedReload->Enable(mMonitoringAutoReload->GetValue());
	if(mMonitoringAutoReload->GetValue() == true)
	{
		mMonitoringAutoReloadFrequency->Enable(!mMonitoringAdvancedReload->GetValue());
	}
	else
	{
		mMonitoringAutoReloadFrequency->Enable(false);
	}

	// -----===== Networking preferences =====-----

	// --- Proxy
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USEPROXY,                 useProxy);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXYADDRESS,             proxyAddress);
	_PrefsGetUint        (PREF_HTTPDOWNLOADER_PROXYPORT,                proxyPort);
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USE_PROXY_AUTHENTICATION, useProxyAuthentication);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXY_USERNAME,           proxyUsername);
	_PrefsGetHiddenString(PREF_HTTPDOWNLOADER_PROXY_PASSWORD,           proxyPassword);

	mNetworkingUseProxy->SetValue(useProxy);
	mNetworkingProxyAddress->Enable(useProxy);
	mNetworkingLabelAddress->Enable(useProxy);
	mNetworkingProxyPort->Enable(useProxy);
	mNetworkingLabelPort->Enable(useProxy);
	mNetworkingUseProxyAuthentication->Enable(useProxy);
	mNetworkingProxyUsername->Enable(useProxy && useProxyAuthentication);
	mNetworkingLabelUsername->Enable(useProxy && useProxyAuthentication);
	mNetworkingProxyPassword->Enable(useProxy && useProxyAuthentication);
	mNetworkingLabelPassword->Enable(useProxy && useProxyAuthentication);

	if(useProxy == true)
	{
		mNetworkingProxyAddress->SetValue(proxyAddress);
		mNetworkingProxyPort->SetValue(wxString::Format(wxT("%u"), proxyPort));
		mNetworkingUseProxyAuthentication->SetValue(useProxyAuthentication);
	}
	else
	{
		mNetworkingProxyAddress->SetValue(wxT(""));
		mNetworkingProxyPort->SetValue(wxT(""));
		mNetworkingUseProxyAuthentication->SetValue(false);
	}

	if(useProxy == true && useProxyAuthentication == true)
	{
		mNetworkingProxyUsername->SetValue(proxyUsername);
		mNetworkingProxyPassword->SetValue(proxyPassword);
	}
	else
	{
		mNetworkingProxyUsername->SetValue(wxT(""));
		mNetworkingProxyPassword->SetValue(wxT(""));
	}

	// -----===== Advanced preferences =====-----
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USEALTERNATEUPDATE,              useAlternate);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_ALTERNATEUPDATEADDRESS,          projectLocationAddress);
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USELOCALFILE,                    useLocalFile);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_LOCALFILELOCATION,               projectLocalFile);

	mAdvancedUseAlternateProjectSource->SetValue(useAlternate);
	mAdvancedUseLocalFile->SetValue(useLocalFile);

	if(useAlternate == false)
	{
		mAdvancedAlternateProjectSourceLocationAddress->SetValue(wxT("http://fah-web.stanford.edu/psummary.html"));
		mAdvancedLocalFileLocation->SetValue(wxT(""));

		mAdvancedAlternateProjectSourceLocationAddress->Enable(false);
		mAdvancedLabelLocationAddress->Enable(false);
		mAdvancedUseLocalFile->Enable(false);
		mAdvancedLocalFileLocation->Enable(false);
		mAdvancedLabelLocalFile->Enable(false);
		mAdvancedLocationChooser->Enable(false);
	}

	if(useAlternate == true && useLocalFile == false)
	{
		mAdvancedAlternateProjectSourceLocationAddress->SetValue(projectLocationAddress);

		mAdvancedAlternateProjectSourceLocationAddress->Enable(true);
		mAdvancedLabelLocationAddress->Enable(true);
		mAdvancedUseLocalFile->Enable(true);
		mAdvancedLocalFileLocation->Enable(false);
		mAdvancedLabelLocalFile->Enable(false);
		mAdvancedLocationChooser->Enable(false);
	}

	if(useAlternate == true && useLocalFile == true)
	{
		mAdvancedLocalFileLocation->SetValue(projectLocalFile);

		mAdvancedAlternateProjectSourceLocationAddress->Enable(false);
		mAdvancedLabelLocationAddress->Enable(false);
		mAdvancedUseLocalFile->Enable(true);
		mAdvancedLocalFileLocation->Enable(true);
		mAdvancedLabelLocalFile->Enable(true);
		mAdvancedLocationChooser->Enable(true);
	}

	// -----===== System preferences =====-----
	_PrefsGetString(PREF_TOOLS_BROWSER,     browser);
	_PrefsGetString(PREF_TOOLS_FILEMANAGER, filemanager);
	_PrefsGetBool(PREF_OVERRIDE_TIMEZONE,   mInitOverrideTz);
	_PrefsGetInt (PREF_TZ,                  mInitTimezone);
	mSystemBrowser->SetValue(browser);
	mSystemOtherFM->SetValue(filemanager);
	mSystemOverrideTimezone->SetValue(mInitOverrideTz);
	mSystemTZ->SetValue(mInitTimezone);

	// -----===== WebApp preferences =====-----
	_PrefsGetBool(PREF_WEBAPP_WEBAPP,               useWebApp);
	_PrefsGetBool(PREF_WEBAPP_SIMPLEWEB,            useSimpleWeb);
	_PrefsGetBool(PREF_WEBAPP_SIMPLETEXT,           useSimpleText);
	_PrefsGetString(PREF_WEBAPP_WEBAPPLOCATION,     webAppLocation);
	_PrefsGetString(PREF_WEBAPP_SIMPLEWEBLOCATION,  simpleWebLocation);
	_PrefsGetString(PREF_WEBAPP_SIMPLETEXTLOCATION, simpleTextLocation);
	_PrefsGetString(PREF_WEBAPP_WEBAPPTEMPLATELOCATION,     webAppTemplateLocation);
	_PrefsGetString(PREF_WEBAPP_SIMPLEWEBTEMPLATELOCATION,  simpleWebTemplateLocation);
	_PrefsGetString(PREF_WEBAPP_SIMPLETEXTTEMPLATELOCATION, simpleTextTemplateLocation);
	mWebAppUseWebApp->SetValue(useWebApp);
	mWebAppUseSimpleWeb->SetValue(useSimpleWeb);
	mWebAppUseSimpleText->SetValue(useSimpleText);
	mWebAppWebAppLocation->SetValue(webAppLocation);
	mWebAppSimpleWebLocation->SetValue(simpleWebLocation);
	mWebAppSimpleTextLocation->SetValue(simpleTextLocation);
	mWebAppWebAppLocation->Enable(useWebApp);
	mWebAppSimpleWebLocation->Enable(useSimpleWeb);
	mWebAppSimpleTextLocation->Enable(useSimpleText);
	mWebAppWebAppLabel->Enable(useWebApp);
	mWebAppSimpleWebLabel->Enable(useSimpleWeb);
	mWebAppSimpleTextLabel->Enable(useSimpleText);
	mWebAppWebAppLocationChooser->Enable(useWebApp);
	mWebAppSimpleWebLocationChooser->Enable(useSimpleWeb);
	mWebAppSimpleTextLocationChooser->Enable(useSimpleText);
	mWebAppWebAppTemplateLocation->SetValue(webAppTemplateLocation);
	mWebAppSimpleWebTemplateLocation->SetValue(simpleWebTemplateLocation);
	mWebAppSimpleTextTemplateLocation->SetValue(simpleTextTemplateLocation);

	#ifdef _FAHMON_WIN32_
	if(filemanager == wxT("explorer.exe"))
	{
		mSystemFileManager->Select(0);
	}
	else //otherfilemanager
	{
		mSystemFileManager->Select(1);
	}
	#elif __WXGTK__
	if(filemanager == wxT("konqueror --profile filemanagement"))
	{
		mSystemFileManager->Select(0);
	}
	else if(filemanager == wxT("dolphin"))
	{
		mSystemFileManager->Select(1);
	}
	else if(filemanager == wxT("nautilus"))
	{
		mSystemFileManager->Select(2);
	}
	else if(filemanager == wxT("thunar"))
	{
		mSystemFileManager->Select(3);
	}
	else // other filemanager
	{
		mSystemFileManager->Select(4);
	}
	#elif __WXMAC__
 	if(filemanager == wxT("open"))
 	{
		mSystemFileManager->Select(0);
	}
 	else // other filemanager
 	{
		mSystemFileManager->Select(1);
 	}
	#endif
	if (mInitOverrideTz == true)
	{
		mSystemTZ->Enable(true);
	}
	else
	{
		mSystemTZ->Enable(false);
	}


}


inline void PreferencesDialog::SavePreferences(void)
{
	wxUint32 proxyPort;

	// -----===== General preferences =====-----
	_PrefsSetBool(PREF_FAHCLIENT_COLLECTXYZFILES,     mGeneralCollectXYZFiles->GetValue());
	_PrefsSetBool(PREF_MAINDIALOG_START_MINIMISED,    mGeneralStartMinimised->GetValue());
	#ifndef __WXMAC__
	_PrefsSetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON,   mGeneralEnableTrayIcon->GetValue());
	#endif
	_PrefsSetBool(PREF_MAINDIALOG_AUTOUPDATEPROJECTS, mGeneralAutoUpdateProjectsDatabase->GetValue());
	_PrefsSetBool(PREF_LISTCLIENTS_KEEP_DEAD_LAST,    mGeneralKeepInaccessibleClientsLast->GetValue());
	_PrefsSetBool(PREF_MAINDIALOG_UPDATE_CHECK,       mGeneralUpdateCheck->GetValue());

	// -----===== Monitoring preferences =====-----
	_PrefsSetBool(PREF_MAINDIALOG_ADVANCEDRELOAD,      mMonitoringAdvancedReload->GetValue());
	_PrefsSetBool(PREF_MAINDIALOG_AUTORELOAD,          mMonitoringAutoReload->GetValue());
	_PrefsSetUint(PREF_MAINDIALOG_AUTORELOADFREQUENCY, mMonitoringAutoReloadFrequency->GetValue());
	_PrefsSetUint(PREF_ETA_DISPLAYSTYLE,               mMonitoringETADisplayStyle->GetSelection());
	_PrefsSetUint(PREF_PPD_DISPLAYSTYLE,               mMonitoringPPDType->GetSelection());
	_PrefsSetBool(PREF_IGNORE_ASYNCHRONY,              mMonitoringIgnoreAsynchrony->GetValue());

	// -----===== Networking preferences =====-----
	proxyPort = wxAtoi(mNetworkingProxyPort->GetValue());

	_PrefsSetBool  (PREF_HTTPDOWNLOADER_USEPROXY,     mNetworkingUseProxy->GetValue());
	_PrefsSetString(PREF_HTTPDOWNLOADER_PROXYADDRESS, mNetworkingProxyAddress->GetValue());
	_PrefsSetUint  (PREF_HTTPDOWNLOADER_PROXYPORT,    proxyPort);

	_PrefsSetBool        (PREF_HTTPDOWNLOADER_USE_PROXY_AUTHENTICATION, mNetworkingUseProxyAuthentication->GetValue());
	_PrefsSetString      (PREF_HTTPDOWNLOADER_PROXY_USERNAME,           mNetworkingProxyUsername->GetValue());
	_PrefsSetHiddenString(PREF_HTTPDOWNLOADER_PROXY_PASSWORD,           mNetworkingProxyPassword->GetValue());

	// -----===== Advanced preferences =====-----
	_PrefsSetBool  (PREF_HTTPDOWNLOADER_USEALTERNATEUPDATE,              mAdvancedUseAlternateProjectSource->GetValue());
	_PrefsSetString(PREF_HTTPDOWNLOADER_ALTERNATEUPDATEADDRESS,          mAdvancedAlternateProjectSourceLocationAddress->GetValue());
	_PrefsSetBool  (PREF_HTTPDOWNLOADER_USELOCALFILE,                    mAdvancedUseLocalFile->GetValue());
	_PrefsSetString(PREF_HTTPDOWNLOADER_LOCALFILELOCATION,               mAdvancedLocalFileLocation->GetValue());

	// -----===== System preferences =====-----
	_PrefsSetString(PREF_TOOLS_BROWSER,     mSystemBrowser->GetValue());
	_PrefsSetString(PREF_TOOLS_FILEMANAGER, mSystemOtherFM->GetValue());
	_PrefsSetBool(PREF_OVERRIDE_TIMEZONE,   mSystemOverrideTimezone->GetValue());
	_PrefsSetInt (PREF_TZ,                  mSystemTZ->GetValue());

	// -----===== WebApp 1 preferences =====-----
	_PrefsSetBool(PREF_WEBAPP_WEBAPP,               mWebAppUseWebApp->GetValue());
	_PrefsSetString(PREF_WEBAPP_WEBAPPLOCATION,     mWebAppWebAppLocation->GetValue());
	_PrefsSetBool(PREF_WEBAPP_SIMPLEWEB,            mWebAppUseSimpleWeb->GetValue());
	_PrefsSetString(PREF_WEBAPP_SIMPLEWEBLOCATION,  mWebAppSimpleWebLocation->GetValue());
	_PrefsSetBool(PREF_WEBAPP_SIMPLETEXT,           mWebAppUseSimpleText->GetValue());
	_PrefsSetString(PREF_WEBAPP_SIMPLETEXTLOCATION, mWebAppSimpleTextLocation->GetValue());

	// -----===== WebApp 2 preferences =====-----
	_PrefsSetString(PREF_WEBAPP_WEBAPPTEMPLATELOCATION,     mWebAppWebAppTemplateLocation->GetValue());
	_PrefsSetString(PREF_WEBAPP_SIMPLEWEBTEMPLATELOCATION,  mWebAppSimpleWebTemplateLocation->GetValue());
	_PrefsSetString(PREF_WEBAPP_SIMPLETEXTTEMPLATELOCATION, mWebAppSimpleTextTemplateLocation->GetValue());

	// -----===== Alert components when important prefs have changed =====-----
	#ifndef __WXMAC__
	if(mGeneralEnableTrayIcon->GetValue() != mInitEnableTrayIcon)
	{
		MainDialog::GetInstance()->OnTrayIconPrefChanged();
	}
	#endif

	if(mMonitoringAdvancedReload->GetValue() != mInitAdvancedReload || (wxUint32)mMonitoringAutoReloadFrequency->GetValue() != mInitAutoReloadFrequency)
	{
		MainDialog::GetInstance()->OnAutoReloadPrefChanged();
	}

	if((wxUint32)mMonitoringETADisplayStyle->GetSelection() != mInitETADisplayStyle)
	{
		MainDialog::GetInstance()->OnETAStylePrefChanged();
	}

	if((wxUint32)mMonitoringPPDType->GetSelection() != mInitPPDDisplayStyle)
	{
		MainDialog::GetInstance()->OnPPDStylePrefChanged();
	}

	// these conditionals reuse functions in maindialog
	if(mSystemOverrideTimezone->GetValue() != mInitOverrideTz)
	{
		MainDialog::GetInstance()->OnPPDStylePrefChanged();
	}

	if(mSystemTZ->GetValue() != mInitTimezone)
	{
		MainDialog::GetInstance()->OnPPDStylePrefChanged();
	}

	if(mMonitoringIgnoreAsynchrony->GetValue() != mInitIgnoreAsynchronousClocks)
	{
		MainDialog::GetInstance()->OnPPDStylePrefChanged();
	}

}


/************************************  EVENTS  ************************************/


void PreferencesDialog::OnOkButton(wxCommandEvent& event)
{
	SavePreferences();
	PreferencesManager::GetInstance()->Save();
	event.Skip();
}


void PreferencesDialog::OnBrowseButton(wxCommandEvent& event)
{
	wxString selectedFile;

	wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose a local project data file"), wxT(""), mAdvancedLocalFileLocation->GetValue(),wxT("HTML Files (*.html)|*.html"), wxOPEN, wxDefaultPosition);
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
	{
		selectedFile = OpenDialog->GetPath();
		// Sets our current document to the file the user selected
		mAdvancedLocalFileLocation->SetValue(selectedFile); //Opens that file
	}
}


void PreferencesDialog::OnWebAppBrowseButton(wxCommandEvent& event)
{
	wxString selectedFile;

	wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose where to save the Web Application"), wxT(""), mWebAppWebAppLocation->GetValue(),wxT("HTML Files (*.html)|*.html"), wxSAVE, wxDefaultPosition);
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
	{
		selectedFile = OpenDialog->GetPath();
		// Sets our current document to the file the user selected
		mWebAppWebAppLocation->SetValue(selectedFile); //Opens that file
	}
}


void PreferencesDialog::OnSimpleWebBrowseButton(wxCommandEvent& event)
{
	wxString selectedFile;

	wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose where to save the simple web page"), wxT(""), mWebAppSimpleWebLocation->GetValue(),wxT("HTML Files (*.html)|*.html"), wxSAVE, wxDefaultPosition);
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
	{
		selectedFile = OpenDialog->GetPath();
		// Sets our current document to the file the user selected
		mWebAppSimpleWebLocation->SetValue(selectedFile); //Opens that file
	}
}


void PreferencesDialog::OnSimpleTextBrowseButton(wxCommandEvent& event)
{
	wxString selectedFile;

	wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose where to save the simple text file"), wxT(""), mWebAppSimpleTextLocation->GetValue(),wxT("Any File (*.*)|*.*"), wxSAVE, wxDefaultPosition);
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
	{
		selectedFile = OpenDialog->GetPath();
		// Sets our current document to the file the user selected
		mWebAppSimpleTextLocation->SetValue(selectedFile); //Opens that file
	}
}


void PreferencesDialog::OnWebAppTemplateBrowseButton(wxCommandEvent& event)
{
	wxString selectedFile;

	wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose the Web Application Template"), wxT(""), mWebAppWebAppTemplateLocation->GetValue(),wxT("HTML Files (*.html;*.htm)|*.html;*.htm"), wxOPEN, wxDefaultPosition);
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
	{
		selectedFile = OpenDialog->GetPath();
		// Sets our current document to the file the user selected
		mWebAppWebAppTemplateLocation->SetValue(selectedFile); //Opens that file
	}
}


void PreferencesDialog::OnSimpleWebTemplateBrowseButton(wxCommandEvent& event)
{
	wxString selectedFile;

	wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose the Simple Web Page Template"), wxT(""), mWebAppSimpleWebTemplateLocation->GetValue(),wxT("HTML Files (*.html;*.htm)|*.html;*.htm"), wxOPEN, wxDefaultPosition);
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
	{
		selectedFile = OpenDialog->GetPath();
		// Sets our current document to the file the user selected
		mWebAppSimpleWebTemplateLocation->SetValue(selectedFile); //Opens that file
	}
}


void PreferencesDialog::OnSimpleTextTemplateBrowseButton(wxCommandEvent& event)
{
	wxString selectedFile;

	wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose the Simple Text File Template"), wxT(""), mWebAppSimpleTextTemplateLocation->GetValue(),wxT("Any File (*.*)|*.*"), wxOPEN, wxDefaultPosition);
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
	{
		selectedFile = OpenDialog->GetPath();
		// Sets our current document to the file the user selected
		mWebAppSimpleTextTemplateLocation->SetValue(selectedFile); //Opens that file
	}
}


void PreferencesDialog::OnCheckboxes(wxCommandEvent& event)
{
	switch(event.GetId())
	{
		// ---
		case CHK_USEPROXY:
			mNetworkingProxyAddress->Enable(mNetworkingUseProxy->GetValue());
			mNetworkingLabelAddress->Enable(mNetworkingUseProxy->GetValue());
			mNetworkingProxyPort->Enable(mNetworkingUseProxy->GetValue());
			mNetworkingLabelPort->Enable(mNetworkingUseProxy->GetValue());
			mNetworkingUseProxyAuthentication->Enable(mNetworkingUseProxy->GetValue());

			if(mNetworkingUseProxy->GetValue() == false)
			{
				mNetworkingProxyUsername->Enable(false);
				mNetworkingLabelUsername->Enable(false);
				mNetworkingProxyPassword->Enable(false);
				mNetworkingLabelPassword->Enable(false);
			}
			else if(mNetworkingUseProxyAuthentication->GetValue() == true)
			{
				mNetworkingProxyUsername->Enable(true);
				mNetworkingLabelUsername->Enable(true);
				mNetworkingProxyPassword->Enable(true);
				mNetworkingLabelPassword->Enable(true);
			}
			break;

		// ---
		case CHK_ADVANCEDRELOAD:
			mMonitoringAutoReloadFrequency->Enable(!mMonitoringAdvancedReload->GetValue());
			break;

		case CHK_AUTORELOAD:
			mMonitoringAdvancedReload->Enable(mMonitoringAutoReload->GetValue());
			if(mMonitoringAutoReload->GetValue() == true)
			{
				mMonitoringAutoReloadFrequency->Enable(!mMonitoringAdvancedReload->GetValue());
			}
			else
			{
				mMonitoringAutoReloadFrequency->Enable(false);
			}
			break;

		// ---
		case CHK_PROXYAUTHENTICATION:
			mNetworkingProxyUsername->Enable(mNetworkingUseProxyAuthentication->GetValue());
			mNetworkingLabelUsername->Enable(mNetworkingUseProxyAuthentication->GetValue());
			mNetworkingProxyPassword->Enable(mNetworkingUseProxyAuthentication->GetValue());
			mNetworkingLabelPassword->Enable(mNetworkingUseProxyAuthentication->GetValue());
			break;

		// ---
		case CHK_USEALTERNATEPROJECTSOURCE:
			mAdvancedAlternateProjectSourceLocationAddress->Enable(mAdvancedUseAlternateProjectSource->GetValue());
			mAdvancedLabelLocationAddress->Enable(mAdvancedUseAlternateProjectSource->GetValue());
			mAdvancedUseLocalFile->Enable(mAdvancedUseAlternateProjectSource->GetValue());
			mAdvancedUseLocalFile->SetValue(false);
			mAdvancedLocalFileLocation->Enable(false);
			mAdvancedLabelLocalFile->Enable(false);
			mAdvancedLocationChooser->Enable(false);
			break;

		// ---
		case CHK_USELOCALFILE:
			mAdvancedLocalFileLocation->Enable(mAdvancedUseLocalFile->GetValue());
			mAdvancedLabelLocalFile->Enable(mAdvancedUseLocalFile->GetValue());
			mAdvancedLocationChooser->Enable(mAdvancedUseLocalFile->GetValue());

			if(mAdvancedUseLocalFile->GetValue() == true)
			{
				mAdvancedAlternateProjectSourceLocationAddress->Enable(false);
				mAdvancedLabelLocationAddress->Enable(false);
			}
			if(mAdvancedUseLocalFile->GetValue() == false)
			{
				mAdvancedAlternateProjectSourceLocationAddress->Enable(true);
				mAdvancedLabelLocationAddress->Enable(true);
			}
			break;
		// ---
		case CHK_TZOVERRIDE:
			mSystemTZ->Enable(mSystemOverrideTimezone->GetValue());
			break;

		case CHK_USEWEBAPP:
			mWebAppWebAppLabel->Enable(mWebAppUseWebApp->GetValue());
			mWebAppWebAppLocation->Enable(mWebAppUseWebApp->GetValue());
			mWebAppWebAppLocationChooser->Enable(mWebAppUseWebApp->GetValue());
			break;

		case CHK_USESIMPLEWEB:
			mWebAppSimpleWebLabel->Enable(mWebAppUseSimpleWeb->GetValue());
			mWebAppSimpleWebLocation->Enable(mWebAppUseSimpleWeb->GetValue());
			mWebAppSimpleWebLocationChooser->Enable(mWebAppUseSimpleWeb->GetValue());
			break;

		case CHK_USESIMPLETEXT:
			mWebAppSimpleTextLabel->Enable(mWebAppUseSimpleText->GetValue());
			mWebAppSimpleTextLocation->Enable(mWebAppUseSimpleText->GetValue());
			mWebAppSimpleTextLocationChooser->Enable(mWebAppUseSimpleText->GetValue());
			break;

		// We should never fall here
		default:
			wxASSERT(false);
			break;
	}
}


void PreferencesDialog::OnChoices(wxCommandEvent& event)
{
	switch(event.GetId())
	{
	// ---
		case CHC_FILEMANAGER:
			#ifdef _FAHMON_WIN32_
			switch(mSystemFileManager->GetSelection())
			{
			// ---
				case 0: //Windows Explorer
					mSystemOtherFM->SetValue(wxT("explorer.exe"));
					break;

				case 1: //Other
					mSystemOtherFM->SetValue(wxT(""));
					break;

				default:
					break;
			}
			#elif __WXGTK__
			switch(mSystemFileManager->GetSelection())
			{
			// ---
				case 0: //Konqueror
					mSystemOtherFM->SetValue(wxT("konqueror --profile filemanagement"));
					break;

				case 1: //Dolphin
					mSystemOtherFM->SetValue(wxT("dolphin"));
					break;

				case 2: //Nautilus
					mSystemOtherFM->SetValue(wxT("nautilus"));
					break;

				case 3: //Thunar
					mSystemOtherFM->SetValue(wxT("thunar"));
					break;

				case 4: //Other
					mSystemOtherFM->SetValue(wxT(""));
					break;

				default:
					break;
			}
			#elif __WXMAC__
			switch(mSystemFileManager->GetSelection())
			{
				// ---
				case 0: //Finder
					mSystemOtherFM->SetValue(wxT("open"));
					break;

				case 1: //Other
					mSystemOtherFM->SetValue(wxT(""));
					break;

				default:
					break;
			}
			#endif
			break;

		default:
		wxASSERT(false);
		break;
	}
}

#ifdef __WXMAC__
/**
 * Mac Specific close routine to save the preferences: no close button
 **/
void PreferencesDialog::OnClose(wxCloseEvent& event)
{
	SavePreferences();
	PreferencesManager::GetInstance()->Save();
 	event.Skip();
}
#endif
