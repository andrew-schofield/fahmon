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
 * \file clientDialog.cpp
 * The client edition dialog.
 * Creates the dialog to add and edit new clients.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "clientDialog.h"

#include "tools.h"
#include "mainDialog.h"
#include "clientsManager.h"
#include "staticBoldedText.h"

#include "wx/dirdlg.h"
#include "wx/button.h"
#include "wx/checkbox.h"
//#include "wx/intl.h"

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


ClientDialog::ClientDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxString::Format(_("Client Edition / %s"), _T(FMC_PRODUCT)))
{
	wxBoxSizer             *topLevelSizer;
	wxBoxSizer             *mainSizer;
	wxBoxSizer             *buttonsSizer;
	wxBoxSizer             *locationSizer;
	wxBoxSizer             *nameSizer;
	wxBoxSizer             *nameLSizer;
	wxBoxSizer             *locationLSizer;
	wxBoxSizer             *vmwareSizer;
	wxFlexGridSizer        *clientInfoSizer;
	wxBoxSizer             *groupSizer;
	wxButton                chooseButton;

	// The wxTextCtrl used for the location has an associated button to show the directory chooser
	// Beware of the creation order, it implicitly defines the tab order
	topLevelSizer       = new wxBoxSizer(wxVERTICAL);
	mainSizer           = new wxBoxSizer(wxVERTICAL);
	locationSizer       = new wxBoxSizer(wxHORIZONTAL);
	buttonsSizer        = new wxBoxSizer(wxHORIZONTAL);
	nameSizer           = new wxBoxSizer(wxHORIZONTAL);
	locationLSizer      = new wxBoxSizer(wxHORIZONTAL);
	nameLSizer          = new wxBoxSizer(wxHORIZONTAL);
	vmwareSizer         = new wxBoxSizer(wxHORIZONTAL);
	groupSizer          = new wxStaticBoxSizer(wxVERTICAL, this, _("Client information"));
	clientInfoSizer     = new wxFlexGridSizer(2, FMC_GUI_SPACING_LOW, FMC_GUI_SPACING_LOW);

	mClientNameCtrl     = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxSize(FMC_GUI_TEXTCTRL_MIN_LENGTH, -1));
	mClientNameCtrl->SetToolTip(_("Enter a name to describe this client."));
	mClientLocationCtrl = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxSize(FMC_GUI_TEXTCTRL_MIN_LENGTH, -1));
	mClientLocationCtrl->SetToolTip(_("Select the directory containing FAHlog.txt for the client you wish to monitor."));
	mClientVMCtrl       = new wxCheckBox(this, wxID_ANY, _("Client is on a Virtual Machine"));

	locationLSizer->Add(new StaticBoldedText(this, wxID_ANY, _("Location:")),0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	locationLSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	locationSizer->Add(mClientLocationCtrl, 1, wxALIGN_CENTER_VERTICAL);
	locationSizer->AddSpacer(FMC_GUI_SPACING_LOW);
#ifndef __WXMAC__
    locationSizer->Add(new wxButton(this, BTN_BROWSE, _T("..."), wxDefaultPosition, wxSize(26, 26)));
#else
	locationSizer->Add(new wxButton(this, BTN_BROWSE, _("Choose"), wxDefaultPosition));
#endif
	locationSizer->AddSpacer(FMC_GUI_SPACING_LOW);

	nameLSizer->Add(new StaticBoldedText(this, wxID_ANY, _("Name:")), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	nameLSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	nameSizer->Add(mClientNameCtrl, 1, wxALIGN_CENTER_VERTICAL);
	nameSizer->AddSpacer(FMC_GUI_SPACING_LOW);

	vmwareSizer->Add(mClientVMCtrl, 1, wxALIGN_CENTER_VERTICAL);
	vmwareSizer->AddSpacer(FMC_GUI_SPACING_LOW);

	// The top part: it contains the two wxTextCtrl and their labels

	clientInfoSizer->Add(nameLSizer, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	clientInfoSizer->Add(nameSizer, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	clientInfoSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	clientInfoSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	clientInfoSizer->Add(locationLSizer, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	clientInfoSizer->Add(locationSizer, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	clientInfoSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	clientInfoSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	clientInfoSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	clientInfoSizer->Add(vmwareSizer, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	groupSizer->Add(clientInfoSizer);

	// The bottom part contains the Ok and Cancel buttons
	//buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

	buttonsSizer->Add(new wxButton(this, wxID_CANCEL), 0, wxALIGN_RIGHT);
	buttonsSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	buttonsSizer->Add(new wxButton(this, wxID_OK), 0, wxALIGN_RIGHT);

	// Construct the main sizer
	//mainSizer = new wxBoxSizer(wxVERTICAL);

	mainSizer->Add(groupSizer, 1, wxEXPAND);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT);

	// And the top level sizer
	//topLevelSizer = new wxBoxSizer(wxVERTICAL);

	topLevelSizer->Add(mainSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	SetSizer(topLevelSizer);
	topLevelSizer->Fit(this);
}


ClientDialog::~ClientDialog(void)
{
}


ClientDialog* ClientDialog::GetInstance(wxWindow* parent)
{
	if(mInstance == NULL)
	{
		mInstance = new ClientDialog(parent);
	}

	return mInstance;
}


void ClientDialog::DestroyInstance(void)
{
	if(mInstance != NULL)
	{
		mInstance->Destroy();
		mInstance = NULL;
	}
}


int ClientDialog::ShowModal(wxUint32 clientId, wxString filename)
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
		mClientVMCtrl->SetValue(clientToEdit->IsVM());
	}
	else
	{
		// Otherwise, we simply clear the fields
		mClientNameCtrl->SetValue(_T(""));
		mClientLocationCtrl->SetValue(filename);
		mClientVMCtrl->SetValue(false);
	}

	Center();

	return wxDialog::ShowModal();
}


/************************************  EVENTS  ************************************/


void ClientDialog::OnBrowseButton(wxCommandEvent& event)
{
	wxString selectedPath;

	// Pop up the directory selector, starting from the current path
	selectedPath = wxDirSelector(_("Choose a folder"), mClientLocationCtrl->GetValue());

	// Check if 'Ok' button was hit
	if(selectedPath.empty() == false)
	{
		mClientLocationCtrl->SetValue(selectedPath);
	}
}


void ClientDialog::OnOkButton(wxCommandEvent& event)
{
	wxUint32       newClientId;
	wxString       clientName;
	wxString       clientLocation;
	bool           clientVM;
	wxCommandEvent newClientEvent(EVT_NEWCLIENTADDED);

	// Retrieve the fields value
	clientName     = mClientNameCtrl->GetValue();
	clientLocation = mClientLocationCtrl->GetValue();
	clientVM       = mClientVMCtrl->GetValue();

	// The two fields must have been filled
	if(clientName.empty() == true || clientLocation.empty() == true)
	{
		Tools::ErrorMsgBox(_("You must fill the name AND the locations fields."));

		// Don't call event.Skip(), since we are not validating this client
		return;
	}

	// So far, so good
	// If the client already exists, simply reload it, this will automatically update any changed information
	if(mClientId != INVALID_CLIENT_ID)
	{
		ClientsManager::GetInstance()->Edit(mClientId, clientName, clientLocation, clientVM);
		ClientsManager::GetInstance()->ReloadThreaded(mClientId);
	}
	else
	{
		// Add the new client to the list
		newClientId = ClientsManager::GetInstance()->Add(clientName, clientLocation, true, clientVM); // alter last argument when dialog is modified

		// And warn the main dialog
		newClientEvent.SetInt(newClientId);
		MainDialog::GetInstance()->AddPendingEvent(newClientEvent);
	}

	// Let the default handler do its job
	event.Skip();
}
