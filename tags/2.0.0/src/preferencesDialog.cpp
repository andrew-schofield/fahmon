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
#include "preferencesDialog.h"

#include "client.h"
#include "wx/valtext.h"
#include "mainDialog.h"
#include "httpDownloader.h"
#include "preferencesManager.h"


// Identifiers for the controls
enum _CONTROL_ID
{
    // Checkboxes
    CHK_AUTORELOAD = wxID_HIGHEST,
    CHK_USEPROXY
};


// Events processed by this class
BEGIN_EVENT_TABLE(PreferencesDialog, wxDialog)

    // --- Buttons
    EVT_BUTTON(wxID_OK,    PreferencesDialog::OnOkButton)

    // --- Checkboxes
    EVT_CHECKBOX(CHK_USEPROXY,   PreferencesDialog::OnCheckboxes)
    EVT_CHECKBOX(CHK_AUTORELOAD, PreferencesDialog::OnCheckboxes)
END_EVENT_TABLE()


// The single instance of PreferencesDialog accross the application
PreferencesDialog* PreferencesDialog::mInstance = NULL;


/**
 * Constructor
**/
PreferencesDialog::PreferencesDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxString::Format(wxT("Preferences / %s"), wxT(FMC_PRODUCT)))
{
    wxBoxSizer *topLevelSizer;
    wxBoxSizer *mainSizer;
    wxBoxSizer *buttonsSizer;
    wxNotebook *noteBook;

    // Preferences are divided into groups, thanks to a wxNoteBook (tabbed control)
    noteBook = new wxNotebook(this, wxID_ANY);

    noteBook->AddPage(CreateGeneralTab(noteBook),    wxT("General"));
    noteBook->AddPage(CreateMonitoringTab(noteBook), wxT("Monitoring"));
    noteBook->AddPage(CreateNetworkingTab(noteBook), wxT("Networking"));

    // Buttons 'Ok' and 'Cancel' are right-aligned
    buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

    buttonsSizer->Add(new wxButton(this, wxID_CANCEL), 0, wxALIGN_RIGHT);
    buttonsSizer->AddSpacer(FMC_GUI_SPACING_LOW);
    buttonsSizer->Add(new wxButton(this, wxID_OK), 0, wxALIGN_RIGHT);

    // Construct the dialog
    mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(noteBook, 1, wxEXPAND);
    mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
    mainSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT);
    
    // The final sizer
    topLevelSizer = new wxBoxSizer(wxVERTICAL);
    
    topLevelSizer->Add(mainSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
    SetSizer(topLevelSizer);
    topLevelSizer->Fit(this);
}


/**
 * Destructor
**/
PreferencesDialog::~PreferencesDialog(void)
{
}


/**
 * Retrieve the instance of PreferencesDialog, create it if needed
**/
PreferencesDialog* PreferencesDialog::GetInstance(wxWindow* parent)
{
    if(mInstance == NULL)
        mInstance = new PreferencesDialog(parent);
    
    return mInstance;
}


/**
 * Destroy the instance of PreferencesDialog, if any
**/
void PreferencesDialog::DestroyInstance(void)
{
    if(mInstance != NULL)
    {
        mInstance->Destroy();
        mInstance = NULL;
    }
}


/**
 * Create the tab containing general preferences
**/
inline wxPanel* PreferencesDialog::CreateGeneralTab(wxNotebook* parent)
{
    wxPanel    *panel;
    wxBoxSizer *sizer;
    wxBoxSizer *topLevelSizer;
    
    // TODO
    // Minimize to tray

    panel                              = new wxPanel(parent);
    sizer                              = new wxBoxSizer(wxVERTICAL);
    topLevelSizer                      = new wxBoxSizer(wxVERTICAL);
//    mGeneralMinimizeToTray             = new wxCheckBox(panel, wxID_ANY, wxT("Minimize to system tray"));
    mGeneralCollectXYZFiles            = new wxCheckBox(panel, wxID_ANY, wxT("Collect .xyz files"));
    mGeneralAutoUpdateProjectsDatabase = new wxCheckBox(panel, wxID_ANY, wxT("Auto update projects database when needed"));

    /*
    sizer->AddStretchSpacer();
    sizer->Add(mGeneralMinimizeToTray, 0, wxALIGN_LEFT);
    */
    sizer->AddStretchSpacer();
    sizer->Add(mGeneralCollectXYZFiles, 0, wxALIGN_LEFT);
    sizer->AddStretchSpacer();
    sizer->Add(mGeneralAutoUpdateProjectsDatabase, 0, wxALIGN_LEFT);
    sizer->AddStretchSpacer();
    
    topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
    panel->SetSizer(topLevelSizer);
    
    return panel;
}


/**
 * Create the tab containing monitoring preferences
**/
inline wxPanel* PreferencesDialog::CreateMonitoringTab(wxNotebook* parent)
{
          wxPanel    *panel;
          wxBoxSizer *sizer;
          wxBoxSizer *topLevelSizer;
          wxBoxSizer *sizerETA;
          wxBoxSizer *sizerAutoReload;
    const wxString    etaFormats[3] = {wxT("A date (dd/mm)"), wxT("A date (mm/dd)"), wxT("Left time")};    // The order *MUST* correspond to the one used for the definition of ETA_DisplayStyle

    panel                          = new wxPanel(parent);
    sizer                          = new wxBoxSizer(wxVERTICAL);
    sizerETA                       = new wxBoxSizer(wxHORIZONTAL);
    topLevelSizer                  = new wxBoxSizer(wxVERTICAL);
    sizerAutoReload                = new wxBoxSizer(wxHORIZONTAL);
    mMonitoringAutoReload          = new wxCheckBox(panel, CHK_AUTORELOAD, wxT("Auto reload every (Mn) "));
    mMonitoringETADisplayStyle     = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, etaFormats);
    mMonitoringAutoReloadFrequency = new wxSpinCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 5);
    
    sizerETA->Add(new wxStaticText(panel, wxID_ANY, wxT("Display ETA as ")), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    sizerETA->Add(mMonitoringETADisplayStyle, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    sizerAutoReload->Add(mMonitoringAutoReload, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    sizerAutoReload->Add(mMonitoringAutoReloadFrequency, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    
    sizer->AddStretchSpacer();
    sizer->Add(sizerAutoReload, 0, wxALIGN_LEFT);
    sizer->AddStretchSpacer();
    sizer->Add(sizerETA, 0, wxALIGN_LEFT);
    sizer->AddStretchSpacer();
    
    topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
    panel->SetSizer(topLevelSizer);
    
    return panel;
}


/**
 * Create the tab containing networking preferences
**/
inline wxPanel* PreferencesDialog::CreateNetworkingTab(wxNotebook* parent)
{
    wxPanel         *panel;
    wxBoxSizer      *sizer;
    wxBoxSizer      *topLevelSizer;
    wxFlexGridSizer *sizerProxyConfiguration;

    panel                   = new wxPanel(parent);
    sizer                   = new wxBoxSizer(wxVERTICAL);
    topLevelSizer           = new wxBoxSizer(wxVERTICAL);
    sizerProxyConfiguration = new wxFlexGridSizer(2, FMC_GUI_SPACING_LOW, FMC_GUI_SPACING_LOW);
    mNetworkingUseProxy     = new wxCheckBox(panel, CHK_USEPROXY, wxT("Use a proxy for HTTP connections"));
    mNetworkingProxyAddress = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(FMC_GUI_TEXTCTRL_MIN_LENGTH, -1));
    mNetworkingProxyPort    = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC));

    sizerProxyConfiguration->Add(new wxStaticText(panel, wxID_ANY, wxT("Address:")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    sizerProxyConfiguration->Add(mNetworkingProxyAddress, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);
    sizerProxyConfiguration->Add(new wxStaticText(panel, wxID_ANY, wxT("Port:")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    sizerProxyConfiguration->Add(mNetworkingProxyPort, 1, wxALIGN_CENTER_VERTICAL);

    sizer->AddStretchSpacer();
    sizer->Add(mNetworkingUseProxy, 0, wxALIGN_LEFT);
    sizer->AddSpacer(FMC_GUI_SPACING_LOW);
    sizer->Add(sizerProxyConfiguration, 0, wxALIGN_LEFT | wxEXPAND);
    sizer->AddStretchSpacer();

    topLevelSizer->Add(sizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
    panel->SetSizer(topLevelSizer);
    
    return panel;
}


/**
 * Give the correct state to each control before displaying the dialog
**/
int PreferencesDialog::ShowModal(void)
{
    LoadPreferences();
    Center();
    
    return wxDialog::ShowModal();
}


/**
 * Give the correct value to each controls using the preferences
**/
inline void PreferencesDialog::LoadPreferences(void)
{
    bool     isUsingProxy;
    bool     isCollectingXYZFiles;
    bool     autoUpdateProjects;
    wxUint32 proxyPort;
    wxString proxyAddress;
    
    // -----===== General preferences =====-----
    _PrefsGetBool(PREF_FAHCLIENT_COLLECTXYZFILES,     isCollectingXYZFiles);
    _PrefsGetBool(PREF_MAINDIALOG_AUTOUPDATEPROJECTS, autoUpdateProjects);

    mGeneralCollectXYZFiles->SetValue(isCollectingXYZFiles);
    mGeneralAutoUpdateProjectsDatabase->SetValue(autoUpdateProjects);

    // -----===== Monitoring preferences =====-----
    _PrefsGetBool(PREF_MAINDIALOG_AUTORELOAD,          mInitAutoReload);
    _PrefsGetUint(PREF_MAINDIALOG_AUTORELOADFREQUENCY, mInitAutoReloadFrequency);
    _PrefsGetUint(PREF_ETA_DISPLAYSTYLE,               mInitETADisplayStyle);
    
    mMonitoringAutoReload->SetValue(mInitAutoReload);
    mMonitoringAutoReloadFrequency->SetValue(wxString::Format(wxT("%u"), mInitAutoReloadFrequency));
    mMonitoringETADisplayStyle->Select(mInitETADisplayStyle);

    // -----===== Networking preferences =====-----

    // --- Proxy
    _PrefsGetBool  (PREF_HTTPDOWNLOADER_USEPROXY,     isUsingProxy);
    _PrefsGetString(PREF_HTTPDOWNLOADER_PROXYADDRESS, proxyAddress);
    _PrefsGetUint  (PREF_HTTPDOWNLOADER_PROXYPORT,    proxyPort);
    
    mNetworkingUseProxy->SetValue(isUsingProxy);
    mNetworkingProxyAddress->Enable(isUsingProxy);
    mNetworkingProxyPort->Enable(isUsingProxy);
    
    if(isUsingProxy == true)
    {
        mNetworkingProxyAddress->SetValue(proxyAddress);
        mNetworkingProxyPort->SetValue(wxString::Format(wxT("%u"), proxyPort));
    }
    else
    {
        mNetworkingProxyAddress->SetValue(wxT(""));
        mNetworkingProxyPort->SetValue(wxT(""));
    }
}


/**
 * Save the value of each control using the preferences
**/
inline void PreferencesDialog::SavePreferences(void)
{
    bool     autoReloadHasChanged;
    bool     etaDisplayStyleHasChanged;
    wxUint32 proxyPort;
    
    // -----===== General preferences =====-----
    _PrefsSetBool(PREF_FAHCLIENT_COLLECTXYZFILES,     mGeneralCollectXYZFiles->GetValue());
    _PrefsSetBool(PREF_MAINDIALOG_AUTOUPDATEPROJECTS, mGeneralAutoUpdateProjectsDatabase->GetValue());

    
    // -----===== Monitoring preferences =====-----
    _PrefsSetBool(PREF_MAINDIALOG_AUTORELOAD,          mMonitoringAutoReload->GetValue());
    _PrefsSetUint(PREF_MAINDIALOG_AUTORELOADFREQUENCY, mMonitoringAutoReloadFrequency->GetValue());
    _PrefsSetUint(PREF_ETA_DISPLAYSTYLE,               mMonitoringETADisplayStyle->GetSelection());
    
    if(mMonitoringAutoReload->GetValue() != mInitAutoReload || (wxUint32)mMonitoringAutoReloadFrequency->GetValue() != mInitAutoReloadFrequency)
        autoReloadHasChanged = true;
    else
        autoReloadHasChanged = false;
    
    if((wxUint32)mMonitoringETADisplayStyle->GetSelection() != mInitETADisplayStyle)
        etaDisplayStyleHasChanged = true;
    else
        etaDisplayStyleHasChanged = false;
        


    // -----===== Networking preferences =====-----
    mNetworkingProxyPort->GetValue().ToLong((long*)&proxyPort);

    _PrefsSetBool  (PREF_HTTPDOWNLOADER_USEPROXY,     mNetworkingUseProxy->GetValue());
    _PrefsSetString(PREF_HTTPDOWNLOADER_PROXYADDRESS, mNetworkingProxyAddress->GetValue());
    _PrefsSetUint  (PREF_HTTPDOWNLOADER_PROXYPORT,    proxyPort);
    
    
    // -----===== Alert components when important prefs have changed =====-----
    if(autoReloadHasChanged == true)
        MainDialog::GetInstance()->OnAutoReloadPrefChanged();
    
    if(etaDisplayStyleHasChanged == true)
        MainDialog::GetInstance()->OnETAStylePrefChanged();
}


/************************************  EVENTS  ************************************/


/**
 * Save preferences before exiting the dialog box
**/
void PreferencesDialog::OnOkButton(wxCommandEvent& event)
{
    SavePreferences();
    
    event.Skip();
}


/**
 * Enable/Disable fields associated with some checkboxes
**/
void PreferencesDialog::OnCheckboxes(wxCommandEvent& event)
{
    switch(event.GetId())
    {
        // ---
        case CHK_USEPROXY:
            mNetworkingProxyAddress->Enable(mNetworkingUseProxy->GetValue());
            mNetworkingProxyPort->Enable(mNetworkingUseProxy->GetValue());
            break;
        
        // ---
        case CHK_AUTORELOAD:
            mMonitoringAutoReloadFrequency->Enable(mMonitoringAutoReload->GetValue());
            break;
        
        // We should never fall here
        default:
            wxASSERT(false);
            break;
    }
}
