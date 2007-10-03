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
#include "aboutDialog.h"

#include "wx/image.h"
#include "wx/sizer.h"
#include "staticUrl.h"
#include "wx/version.h"
#include "wx/statbmp.h"
#include "wx/statline.h"
#include "pathManager.h"
#include "staticBoldedText.h"


// The single instance of AboutDialog accross the application
AboutDialog* AboutDialog::mInstance = NULL;


/**
 * Constructor
**/
AboutDialog::AboutDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxString(wxT("About")))
{
    wxBoxSizer      *topLevelSizer;
    wxBoxSizer      *infoSizer;
    wxFlexGridSizer *mainSizer;
    wxFlexGridSizer *authorHomepageSizer;


    // ---
    authorHomepageSizer = new wxFlexGridSizer(2, FMC_GUI_SPACING_LOW, FMC_GUI_SPACING_LOW);

    authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, wxT("Author:")), 0, wxALIGN_RIGHT);
    authorHomepageSizer->Add(new wxStaticText(this, wxID_ANY, wxT(" Francois Ingelrest")), 0, wxALIGN_LEFT);
    authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, wxT("Maintainer:")), 0, wxALIGN_RIGHT);
    authorHomepageSizer->Add(new wxStaticText(this, wxID_ANY, wxT(" Andrew Schofield")), 0, wxALIGN_LEFT);
    authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, wxT("Homepage:")), 0, wxALIGN_RIGHT);
    authorHomepageSizer->Add(new StaticUrl(this, wxT(FMC_URL_FAHMON), wxT(FMC_URL_FAHMON)), 0, wxALIGN_LEFT);
    authorHomepageSizer->AddSpacer(FMC_GUI_SPACING_LOW);
    authorHomepageSizer->AddSpacer(FMC_GUI_SPACING_LOW);
    authorHomepageSizer->Add(new StaticBoldedText(this, wxID_ANY, wxT("Library:")), 0, wxALIGN_RIGHT);
    authorHomepageSizer->Add(new StaticUrl(this, wxT(FMC_URL_WXWIDGETS), wxVERSION_STRING), 0, wxALIGN_LEFT);


    // ---
    infoSizer = new wxBoxSizer(wxVERTICAL);

    infoSizer->Add(new StaticBoldedText(this, wxID_ANY, wxT(FMC_PRODUCT)), 0, wxALIGN_CENTER);
    infoSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
    infoSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(65, -1)), 0, wxALIGN_CENTER);
    infoSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
    infoSizer->Add(authorHomepageSizer);


    // ---
    mainSizer = new wxFlexGridSizer(2, FMC_GUI_SPACING_LOW, FMC_GUI_SPACING_LOW);

    mainSizer->Add(new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_ABOUT), wxBITMAP_TYPE_PNG))), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FMC_GUI_SPACING_HIGH);
    mainSizer->Add(infoSizer);

    // ---
    topLevelSizer = new wxBoxSizer(wxVERTICAL);

    topLevelSizer->Add(mainSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
    SetSizer(topLevelSizer);
    topLevelSizer->Fit(this);
}


/**
 * Destructor
**/
AboutDialog::~AboutDialog(void)
{
}


/**
 * Retrieve the instance of AboutDialog, create it if needed
**/
AboutDialog* AboutDialog::GetInstance(wxWindow* parent)
{
    if(mInstance == NULL)
        mInstance = new AboutDialog(parent);

    return mInstance;
}


/**
 * Destroy the instance of AboutDialog, if any
**/
void AboutDialog::DestroyInstance(void)
{
    if(mInstance != NULL)
    {
        mInstance->Destroy();
        mInstance = NULL;
    }
}


/**
 * Center the dialog box before displaying it
**/
int AboutDialog::ShowModal(void)
{
    Center();

    return wxDialog::ShowModal();
}
