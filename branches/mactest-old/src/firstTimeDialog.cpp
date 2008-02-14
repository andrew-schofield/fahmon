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
#include "firstTimeDialog.h"

#include "tools.h"
#include "staticUrl.h"
#include "pathManager.h"
#include "wx/statline.h"
#include "fahmonConsts.h"
#include "projectsManager.h"
#include "staticBoldedText.h"


// Identifiers for the controls
enum _CONTROL_ID
{
	// --- Buttons
	BTN_UPDATE_PROJECTS = wxID_HIGHEST
};


// Events processed by this class
BEGIN_EVENT_TABLE(FirstTimeDialog, wxDialog)
	// --- Buttons
	EVT_BUTTON(BTN_UPDATE_PROJECTS,    FirstTimeDialog::OnUpdateButton)

	// --- Frame
	EVT_CLOSE(FirstTimeDialog::OnClose)
END_EVENT_TABLE()


// The single instance of FirstTimeDialog accross the application
FirstTimeDialog* FirstTimeDialog::mInstance = NULL;


/**
* Constructor
**/
FirstTimeDialog::FirstTimeDialog(void) : wxDialog(NULL, wxID_ANY, wxString::Format(_("Welcome to %s!"), wxT(FMC_PRODUCT)))
{
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *mainSizer;

	// ---
	mainSizer = new wxBoxSizer(wxVERTICAL);

	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new StaticBoldedText(this, wxID_ANY, wxString::Format(_("Welcome to %s, a Folding@Home monitoring tool"), wxT(FMC_APPNAME))), 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1)), 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Project information (e.g. deadlines) is read from a local file, which must be regularly updated.")), 0, wxALIGN_LEFT);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Thus, you should first download the latest available information on currently running projects:")), 0, wxALIGN_LEFT);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, _("(NOTE: If you use a proxy for HTTP connections, you will need to change the preferences first!)")), 0, wxALIGN_LEFT);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxButton(this, BTN_UPDATE_PROJECTS, _("Download current projects")), 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, _("New updates are needed when no information on projects can be displayed (e.g. deadlines).")), 0, wxALIGN_LEFT);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, _("You will be able to perform them using the \'FahMon\' menu.")), 0, wxALIGN_LEFT);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1)), 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, _("To monitor a client, you need to add it by right-clicking on the list and choosing \'Add a new client\'.")), 0, wxALIGN_LEFT);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, _("The name you choose for a client is not important, you can think of it as a nickname.")), 0, wxALIGN_LEFT);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, _("All further operations on clients (e.g. edition, deletion) are also made by right-clicking on the list.")), 0, wxALIGN_LEFT);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1)), 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, wxString::Format(_("The latest version of %s will always be available on the official website:"), wxT(FMC_APPNAME))), 0, wxALIGN_CENTER);
	mainSizer->Add(new StaticUrl(this, wxT(FMC_URL_FAHMON), wxT(FMC_URL_FAHMON)), 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1)), 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	mainSizer->Add(new wxButton(this, wxID_OK), 0, wxALIGN_RIGHT);

	// ---
	topLevelSizer = new wxBoxSizer(wxVERTICAL);

	topLevelSizer->Add(mainSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	SetSizer(topLevelSizer);
	topLevelSizer->Fit(this);
}


/**
* Destructor
**/
FirstTimeDialog::~FirstTimeDialog(void)
{
}


/**
* Retrieve the instance of FirstTimeDialog, create it if needed
**/
FirstTimeDialog* FirstTimeDialog::GetInstance(void)
{
	if(mInstance == NULL)
		mInstance = new FirstTimeDialog();

	return mInstance;
}


/**
* Destroy the instance of FirstTimeDialog, if any
**/
void FirstTimeDialog::DestroyInstance(void)
{
	if(mInstance != NULL)
	{
		mInstance->Destroy();
		mInstance = NULL;
	}
}


/**
* Center the dialog box before displaying it
* Also create the configuration directory if it does not exist
*
* Return wxID_CANCEL if the directory could not be created
**/
int FirstTimeDialog::ShowModal(void)
{
	// Try to create the directory if it does not already exist
	// This should now only be a double-check, as it *should* have been created by main.cpp
	if(!wxDirExists(PathManager::GetCfgPath()))
	{
		if(!wxMkdir(PathManager::GetCfgPath()))
		{
			Tools::ErrorMsgBox(wxString::Format(_("Could not create directory <%s>"), PathManager::GetCfgPath().c_str()));
			return wxID_CANCEL;
		}
	}

	// The directory could be created, so we can display the dialog
	Center();

	return wxDialog::ShowModal();
}


/**
* Change the behaviour of the default close handler, so that it simulate a push on the OK button
* Indeed, we don't want to return a wxID_CANCEL at this point
**/
void FirstTimeDialog::OnClose(wxCloseEvent& event)
{
	// I found this method in src/gtk/dialog.cpp, OnCloseWindow()
	wxCommandEvent okEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);

	okEvent.SetEventObject(this);
	GetEventHandler()->ProcessEvent(okEvent);
}


/**
* Manage the 'update projects' button
**/
void FirstTimeDialog::OnUpdateButton(wxCommandEvent& event)
{
	ProjectsManager::GetInstance()->UpdateDatabase(true, false);
}
