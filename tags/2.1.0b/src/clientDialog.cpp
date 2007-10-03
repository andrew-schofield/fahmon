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
#include "clientDialog.h"

#include "tools.h"
#include "wx/dirdlg.h"
#include "mainDialog.h"
#include "clientsManager.h"
#include "staticBoldedText.h"


// Identifiers for the controls
enum _CONTROL_ID
{
    // --- Buttons
    BTN_BROWSE = wxID_HIGHEST
};


// Events processed by this class
BEGIN_EVENT_TABLE(ClientDialog, wxDialog)

    // --- Buttons
    EVT_BUTTON(BTN_BROWSE, ClientDialog::OnBrowseButton)
    EVT_BUTTON(wxID_OK,    ClientDialog::OnOkButton)
END_EVENT_TABLE()


// The single instance of AboutDialog accross the application
ClientDialog* ClientDialog::mInstance = NULL;


/**
 * Constructor
**/
ClientDialog::ClientDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxString::Format(wxT("Client Edition / %s"), wxT(FMC_PRODUCT)))
{
    wxBoxSizer       *topLevelSizer;
    wxBoxSizer       *mainSizer;
    wxBoxSizer       *buttonsSizer;
    wxBoxSizer       *locationSizer;
    wxFlexGridSizer  *clientInfoSizer;
    wxStaticBoxSizer *groupSizer;

    // The wxTextCtrl used for the location has an associated button to show the directory chooser
    // Beware of the cration order, it implicitly defines the tab order
    locationSizer       = new wxBoxSizer(wxHORIZONTAL);
    mClientNameCtrl     = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(FMC_GUI_TEXTCTRL_MIN_LENGTH, -1));
    mClientLocationCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(FMC_GUI_TEXTCTRL_MIN_LENGTH, -1));

    locationSizer->Add(mClientLocationCtrl, 1, wxALIGN_CENTER_VERTICAL);
    locationSizer->AddSpacer(FMC_GUI_SPACING_LOW);
    locationSizer->Add(new wxButton(this, BTN_BROWSE, wxT("..."), wxDefaultPosition, wxSize(26, 26)), 0, wxALIGN_CENTER_VERTICAL);

    // The top part: it contains the two wxTextCtrl and their labels
    groupSizer      = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Client information"));
    clientInfoSizer = new wxFlexGridSizer(2, FMC_GUI_SPACING_LOW, FMC_GUI_SPACING_LOW);

    clientInfoSizer->Add(new StaticBoldedText(this, wxID_ANY, wxT("Name:")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    clientInfoSizer->Add(mClientNameCtrl, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
    clientInfoSizer->Add(new StaticBoldedText(this, wxID_ANY, wxT("Location:")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    clientInfoSizer->Add(locationSizer, 0, wxALIGN_CENTER_VERTICAL);
    groupSizer->Add(clientInfoSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
    
    // The bottom part contains the Ok and Cancel buttons
    buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

    buttonsSizer->Add(new wxButton(this, wxID_CANCEL), 0, wxALIGN_RIGHT);
    buttonsSizer->AddSpacer(FMC_GUI_SPACING_LOW);
    buttonsSizer->Add(new wxButton(this, wxID_OK), 0, wxALIGN_RIGHT);
    
    // Construct the main sizer
    mainSizer = new wxBoxSizer(wxVERTICAL);
    
    mainSizer->Add(groupSizer, 1, wxEXPAND);
    mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
    mainSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT);
    
    // And the top level sizer
    topLevelSizer = new wxBoxSizer(wxVERTICAL);
    
    topLevelSizer->Add(mainSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
    SetSizer(topLevelSizer);
    topLevelSizer->Fit(this);
}


/**
 * Destructor
**/
ClientDialog::~ClientDialog(void)
{
}


/**
 * Retrieve the instance of ClientDialog, create it if needed
**/
ClientDialog* ClientDialog::GetInstance(wxWindow* parent)
{
    if(mInstance == NULL)
        mInstance = new ClientDialog(parent);
    
    return mInstance;
}


/**
 * Destroy the instance of AboutDialog, if any
**/
void ClientDialog::DestroyInstance(void)
{
    if(mInstance != NULL)
    {
        mInstance->Destroy();
        mInstance = NULL;
    }
}


/**
 * Show the dialog in order to edit the given client
 * If clientId is equal to INVALID_CLIENT_ID, then a new client will be created
**/
int ClientDialog::ShowModal(wxUint32 clientId)
{
    const Client* clientToEdit;
    
    // Store the client we are going to edit
    mClientId = clientId;

    // And fill the fields with this client's information, if any
    if(mClientId != INVALID_CLIENT_ID)
    {
        clientToEdit = ClientsManager::GetInstance()->Get(mClientId);
        mClientNameCtrl->SetValue(clientToEdit->GetName());
        mClientLocationCtrl->SetValue(clientToEdit->GetLocation());
    }
    else
    {
        // Otherwise, we simply clear the fields
        mClientNameCtrl->SetValue(wxT(""));
        mClientLocationCtrl->SetValue(wxT(""));
    }

    Center();

    return wxDialog::ShowModal();
}


/************************************  EVENTS  ************************************/


/**
 * Manage the browse button: It displays the standard 'browse for directory...' dialog and fill the corresponding text field
**/
void ClientDialog::OnBrowseButton(wxCommandEvent& event)
{
    wxString selectedPath;
    
    // Pop up the directory selector, starting from the current path
    selectedPath = wxDirSelector(wxT("Choose a folder"), mClientLocationCtrl->GetValue());
    
    // Check if 'Ok' button was hit
    if(selectedPath.empty() == false)
        mClientLocationCtrl->SetValue(selectedPath);
}


/**
 * Manage the Ok button: The client can be validated only if the two textctrl have been filled
**/
void ClientDialog::OnOkButton(wxCommandEvent& event)
{
    wxUint32       newClientId;
    wxString       clientName;
    wxString       clientLocation;
    wxCommandEvent newClientEvent(EVT_NEWCLIENTADDED);
    
    // Retrieve the fields value
    clientName     = mClientNameCtrl->GetValue();
    clientLocation = mClientLocationCtrl->GetValue();

    // The two fields must have been filled
    if(clientName.empty() == true || clientLocation.empty() == true)
    {
        Tools::ErrorMsgBox(wxT("You must fill the name AND the locations fields."));

        // Don't call event.Skip(), since we are not validating this client
        return;
    }

    // So far, so good
    // If the client already exists, simply reload it, this will automatically update any changed information
    if(mClientId != INVALID_CLIENT_ID)
    {
        ClientsManager::GetInstance()->Edit(mClientId, clientName, clientLocation);
        ClientsManager::GetInstance()->ReloadThreaded(mClientId);
    }
    else
    {
        // Add the new client to the list
        newClientId = ClientsManager::GetInstance()->Add(clientName, clientLocation);
        
        // And warn the main dialog
        newClientEvent.SetInt(newClientId);
        MainDialog::GetInstance()->AddPendingEvent(newClientEvent);
    }
    
    // Let the default handler do its job
    event.Skip();
}
