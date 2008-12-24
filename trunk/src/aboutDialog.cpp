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
 * \file aboutDialog.cpp
 * About Dialog.
 * Creates a recognised about dialog for FahMon.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "aboutDialog.h"

#include "pathManager.h"
#include "staticBoldedText.h"

#include "wx/image.h"
#include "wx/sizer.h"
//#include "wx/intl.h"
#include "wx/version.h"
#include "wx/statbmp.h"
#include "wx/statline.h"
#include "wx/hyperlink.h"
#include "wx/button.h"

// Events processed by this class
BEGIN_EVENT_TABLE(AboutDialog, wxDialog)
	// Buttons
	EVT_BUTTON(wxID_CLOSE,    AboutDialog::OnCloseButton)
END_EVENT_TABLE()

AboutDialog* AboutDialog::mInstance = NULL;


AboutDialog::AboutDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxString(_("About")))
{
	wxBoxSizer      *topLevelSizer;
	wxBoxSizer      *infoSizer;
	wxBoxSizer      *buttonSizer;
	wxFlexGridSizer *flexSizer;
	wxFlexGridSizer *authorHomepageSizer;


	// ---
	authorHomepageSizer = new wxFlexGridSizer(2, FMC_GUI_SPACING_LOW, FMC_GUI_SPACING_LOW);

	authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, _("Lead Developer:")), 0, wxALIGN_RIGHT);
	/* TRANSLATORS: This is a proper name. See the gettext manual, section Names.
	   Pronunciation is like "An-droo Skow(e)-fee-uld"*/
	authorHomepageSizer->Add(new wxStaticText(this, wxID_ANY, _("Andrew Schofield")), 0, wxALIGN_LEFT);
	authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, _("Retired Developer:")), 0, wxALIGN_RIGHT);
	/* TRANSLATORS: This is a proper name. See the gettext manual, section Names. This is a non-ASCII name,
	   hence the use of unicode escape codes. Pronunciation is like "Fran-swa Angle-ray" */
	authorHomepageSizer->Add(new wxStaticText(this, wxID_ANY, _("Fran\u00E7ois Ingelrest")), 0, wxALIGN_LEFT);
	authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, _("Homepage:")), 0, wxALIGN_RIGHT);
	authorHomepageSizer->Add(new wxHyperlinkCtrl(this, wxID_ANY, wxT(FMC_URL_FAHMON), wxT(FMC_URL_FAHMON)));
	authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, _("Qd homepage:")), 0, wxALIGN_RIGHT);
	authorHomepageSizer->Add(new wxHyperlinkCtrl(this, wxID_ANY, wxT("http://linuxminded.nl"), wxT(FMC_URL_QD)));
	authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, _("Licence:")), 0, wxALIGN_RIGHT);
	authorHomepageSizer->Add(new wxHyperlinkCtrl(this, wxID_ANY, wxT("GNU GPL v2"), wxT(FMC_URL_LICENCE)));
	authorHomepageSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	authorHomepageSizer->AddSpacer(FMC_GUI_SPACING_LOW);
	/* TRANSLATORS: This refers to the wxWidgets software library */
	authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, _("Library:")), 0, wxALIGN_RIGHT);
	authorHomepageSizer->Add(new wxHyperlinkCtrl(this, wxID_ANY, wxVERSION_STRING, wxT(FMC_URL_WXWIDGETS)));
	authorHomepageSizer->AddSpacer(FMC_GUI_SPACING_LOW);


	// ---
	infoSizer = new wxBoxSizer(wxVERTICAL);

	infoSizer->Add(new StaticBoldedText(this, wxID_ANY, wxT(FMC_PRODUCT)), 0, wxALIGN_CENTER);
	infoSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	infoSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(65, -1)), 0, wxALIGN_CENTER);
	infoSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	infoSizer->Add(authorHomepageSizer);


	// ---
	flexSizer = new wxFlexGridSizer(2, FMC_GUI_SPACING_LOW, FMC_GUI_SPACING_LOW);

	flexSizer->Add(new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_ABOUT), wxBITMAP_TYPE_PNG))), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FMC_GUI_SPACING_HIGH);
	flexSizer->Add(infoSizer);

	buttonSizer = new wxBoxSizer(wxVERTICAL);
#ifndef __WXMAC__
	buttonSizer->Add(new wxButton(this, wxID_CLOSE), 0, wxALIGN_CENTER_HORIZONTAL);
#endif

	// ---
	topLevelSizer = new wxBoxSizer(wxVERTICAL);

	topLevelSizer->Add(flexSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	topLevelSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, FMC_GUI_BORDER);

	SetSizer(topLevelSizer);
	topLevelSizer->Fit(this);
	SetAffirmativeId(wxID_CLOSE);
}


AboutDialog::~AboutDialog(void)
{
}


AboutDialog* AboutDialog::GetInstance(wxWindow* parent)
{
	if(mInstance == NULL)
	{
		mInstance = new AboutDialog(parent);
	}

	return mInstance;
}


void AboutDialog::DestroyInstance(void)
{
	if(mInstance != NULL)
	{
		mInstance->Destroy();
		mInstance = NULL;
	}
}


int AboutDialog::ShowModal(void)
{
	Center();

	return wxDialog::ShowModal();
}

void AboutDialog::OnCloseButton(wxCommandEvent& event)
{
	Close();
}
