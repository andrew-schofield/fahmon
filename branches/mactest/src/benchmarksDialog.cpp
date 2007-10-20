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
#include "benchmarksDialog.h"

#include "tools.h"
#include "wx/sizer.h"
#include "clientsManager.h"
#include "projectsManager.h"
#include "benchmarksManager.h"
#include "preferencesManager.h"


// Identifiers for the controls
enum _CONTROL_ID
{
	// --- ListView
	LST_PROJECTS = wxID_HIGHEST
};


// Identifiers of the columns used in the ListView with the projects
enum _COLUMNS_ID
{
	LC_PROJECT
};


// Events processed by this class
BEGIN_EVENT_TABLE(BenchmarksDialog, wxDialog)
	// List View
	EVT_LIST_ITEM_SELECTED  (LST_PROJECTS, BenchmarksDialog::OnProjectChanged)

	// Buttons
	EVT_BUTTON(wxID_CLOSE,    BenchmarksDialog::OnCloseButton)
END_EVENT_TABLE()

BenchmarksDialog* BenchmarksDialog::mInstance = NULL;


/**
* Constructor
**/
BenchmarksDialog::BenchmarksDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxString::Format(wxT("%s / %s"), _("Benchmarks"), wxT(FMC_PRODUCT)), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	wxUint32    frameWidth;
	wxUint32    frameHeight;
	wxUint32    columnWidth;
	wxUint32    sashPosition;
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *mainSizer;

	// ---
	mSplitter              = new wxSplitterWindow(this, wxID_ANY);
	mListOfProjects        = new wxListView(mSplitter, LST_PROJECTS, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	mBenchmarksInformation = new wxTextCtrl(mSplitter, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);

	_PrefsGetUint(PREF_BENCHMARKSDIALOG_SASHPOSITION, sashPosition);
	mSplitter->SplitVertically(mListOfProjects, mBenchmarksInformation);
	mSplitter->SetSashPosition(sashPosition);
	mSplitter->SetMinimumPaneSize(20);

	#ifndef __WXMAC__
	mBenchmarksInformation->SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	#else
	mBenchmarksInformation->SetFont(wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	#endif
	

	_PrefsGetUint(PREF_BENCHMARKSDIALOG_COLUMNWIDTH, columnWidth);
	mListOfProjects->InsertColumn(LC_PROJECT, _("Projects"));
	mListOfProjects->SetColumnWidth(LC_PROJECT, columnWidth);

	// ---
	mainSizer = new wxBoxSizer(wxVERTICAL);

	mainSizer->Add(mSplitter, 1, wxEXPAND);
	mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
	#ifndef __WXMAC__
	mainSizer->Add(new wxButton(this, wxID_CLOSE), 0, wxALIGN_CENTER_HORIZONTAL);
	#endif

	// ---
	topLevelSizer = new wxBoxSizer(wxVERTICAL);

	topLevelSizer->Add(mainSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	SetSizer(topLevelSizer);
	topLevelSizer->Fit(this);

	// --- Restore the size of the frame
	_PrefsGetUint(PREF_BENCHMARKSDIALOG_FRAMEWIDTH,  frameWidth);
	_PrefsGetUint(PREF_BENCHMARKSDIALOG_FRAMEHEIGHT, frameHeight);
	SetSize(frameWidth, frameHeight);
}


/**
* Destructor
**/
BenchmarksDialog::~BenchmarksDialog(void)
{
}


/**
* Retrieve the instance of BenchmarksDialog, create it if needed
**/
BenchmarksDialog* BenchmarksDialog::GetInstance(wxWindow* parent)
{
	if(mInstance == NULL)
		mInstance = new BenchmarksDialog(parent);

	return mInstance;
}


/**
* Destroy the instance of BenchmarksDialog, if any
**/
void BenchmarksDialog::DestroyInstance(void)
{
	if(mInstance != NULL)
	{
		// Save sizes before cleaning
		_PrefsSetUint(PREF_BENCHMARKSDIALOG_FRAMEWIDTH,   mInstance->GetSize().GetWidth());
		_PrefsSetUint(PREF_BENCHMARKSDIALOG_FRAMEHEIGHT,  mInstance->GetSize().GetHeight());
		_PrefsSetUint(PREF_BENCHMARKSDIALOG_SASHPOSITION, mInstance->mSplitter->GetSashPosition());
		_PrefsSetUint(PREF_BENCHMARKSDIALOG_COLUMNWIDTH,  mInstance->mListOfProjects->GetColumnWidth(LC_PROJECT));

		mInstance->Destroy();
		mInstance = NULL;
	}
}


/**
* Show the window and automatically select the given project
* If projectId is equal to INVALID_PROJECT_ID, the first available project is selected
**/
int BenchmarksDialog::ShowModal(ProjectId projectIdToSelect)
{
	// Fill the ListView with the correct list of known projects
	PopulateProjectsList(projectIdToSelect);

	Center();

	return wxDialog::ShowModal();
}


/**
* Fill the list with all the project identifiers for which a benchmark is registered
* Select by default the given project
* If there is no benchmark for this project, select the first entry
**/
void BenchmarksDialog::PopulateProjectsList(ProjectId projectIdToSelect)
{
	wxUint32   i;
	wxUint32   nbProjects;
	wxUint32   entryToSelect;
	ProjectId *projectsList;

	// We first remove any previous entry
	mListOfProjects->DeleteAllItems();

	// Try to see if there is at least one registered benchmark
	projectsList = BenchmarksManager::GetInstance()->GetBenchmarkedProjects(nbProjects);
	if(nbProjects == 0)
		return;

	entryToSelect = 0;

	// Ok, we can fill the ListView
	for(i=0; i<nbProjects; ++i)
	{
		// Insert a new entry for each project identifier
		// Each entry is associated with the project identifier, so that we can easily retrieve it for a given selection
		mListOfProjects->InsertItem(i, wxString::Format(wxT("%u"), projectsList[i]));
		mListOfProjects->SetItemData(i, projectsList[i]);

		// Is this the project we should select?
		if(projectsList[i] == projectIdToSelect)
			entryToSelect = i;

		// Give a different color to odd lines
		if((i&1) != 0)
			mListOfProjects->SetItemBackgroundColour(i, FMC_COLOR_LIST_ODD_LINES);
		else
			mListOfProjects->SetItemBackgroundColour(i, *wxWHITE);
	}

	// No memory leak
	delete[] projectsList;


	// Select the correct entry
	mListOfProjects->Select(entryToSelect);
	mListOfProjects->EnsureVisible(entryToSelect);
}


/**
* Show the known benchmark for the given project
* If projectId is equal to INVALID_PROJECT_ID, clear information
**/
void BenchmarksDialog::ShowBenchmarks(ProjectId projectIdToShow)
{
	double            minPPD;
	double            avgPPD;
	double            instantPPD;
	double            tFramePPD;
	wxUint32          i;
	wxUint32          nbBenchmarks;
	wxString          infoString;
	wxString          clientLocation;
	wxString          clientName;
	const Project    *project;
	const Benchmark **benchmarks;

	// Clear the text area if needed
	if(projectIdToShow == INVALID_PROJECT_ID)
	{
		mBenchmarksInformation->SetValue(wxT(""));
		return;
	}

	// Retrieve the benchmarks for the given project
	benchmarks = BenchmarksManager::GetInstance()->GetBenchmarksList(projectIdToShow, nbBenchmarks);
	if(nbBenchmarks == 0)
	{
		mBenchmarksInformation->SetValue(_("No benchmarks for this project!"));
		return;
	}

	// Display project information, if it is available
	project = ProjectsManager::GetInstance()->GetProject(projectIdToShow);
	if(project != NULL)
	{
		infoString += wxT(" ") + wxString::Format(_("Project : %u\n"), projectIdToShow);
		infoString += wxT(" ") + wxString::Format(_("Core    : %s\n"), Core::IdToLongName(project->GetCoreId()).c_str());
		infoString += wxT(" ") + wxString::Format(_("Frames  : %u\n"), project->GetNbFrames());
		infoString += wxT(" ") + wxString::Format(_("Credit  : %u\n"), project->GetCredit());
	}
	else
		infoString += wxT(" ") + wxString::Format(_("Project : %u (Unknown)\n"), projectIdToShow);

	for(i=0; i<nbBenchmarks; ++i)
	{
		// Use the name of the client if found, its location otherwise
		clientLocation = BenchmarksManager::GetInstance()->GetClientLocationFromClientId(benchmarks[i]->GetClientId());
		clientName     = ClientsManager::GetInstance()->GetNameFromLocation(clientLocation);

		if(clientName.IsEmpty() == false)
			infoString += wxString::Format(wxT("\n\n -- %s --\n\n"), clientName.c_str());
		else
			infoString += wxString::Format(wxT("\n\n -- %s --\n\n"), clientLocation.c_str());

		// Best time
		if(benchmarks[i]->GetMinDuration() != 0)
		{
			infoString += wxT(" ") + wxString::Format(_("Min. Time / Frame : %s"), Tools::FormatSeconds(benchmarks[i]->GetMinDuration()).c_str());

			// Compute points per day if possible
			if(project != NULL)
			{
				minPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetMinDuration() * (double)project->GetNbFrames());    // There are 86400 seconds in a day
				infoString += wxT(" ") + wxString::Format(_(" - %.2f ppd"), minPPD);
			}
		}
		else
			infoString += wxString::Format(wxT(" %s"), _("No Min. Time / Frame"));

		infoString += wxT("\n");


		// Average time
		if(benchmarks[i]->GetAvgDuration() != 0)
		{
			infoString += wxT(" ") + wxString::Format(_("Avg. Time / Frame : %s"), Tools::FormatSeconds(benchmarks[i]->GetAvgDuration()).c_str());

			// Compute points per day if possible
			if(project != NULL)
			{
				avgPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetAvgDuration() * (double)project->GetNbFrames());    // There are 86400 seconds in a day
				infoString += wxT(" ") + wxString::Format(_(" - %.2f ppd"), avgPPD);
			}
		}
		else
			infoString += wxString::Format(wxT(" %s"), _("No Avg. Time / Frame"));

		infoString += wxT("\n");


		// Instantaneous time
		if(benchmarks[i]->GetInstantDuration() != 0)
		{
			infoString += wxT(" ") + wxString::Format(_("Cur. Time / Frame : %s"), Tools::FormatSeconds(benchmarks[i]->GetInstantDuration()).c_str());

			// Compute points per day if possible
			if(project != NULL)
			{
				instantPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetInstantDuration() * (double)project->GetNbFrames());    // There are 86400 seconds in a day
				infoString += wxT(" ") + wxString::Format(_(" - %.2f ppd"), instantPPD);
			}
		}
		else
			infoString += wxString::Format(wxT(" %s"), _("No Cur. Time / Frame"));

		infoString += wxT("\n");


		// 3 frame rolling time
		if(benchmarks[i]->GetInstantDuration() != 0)
		{
			infoString += wxT(" ") + wxString::Format(_("R3F. Time / Frame : %s"), Tools::FormatSeconds(benchmarks[i]->Get3FrameDuration()).c_str());

			// Compute points per day if possible
			if(project != NULL)
			{
				tFramePPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->Get3FrameDuration() * (double)project->GetNbFrames());    // There are 86400 seconds in a day
				infoString += wxT(" ") + wxString::Format(_(" - %.2f ppd"), tFramePPD);
			}
		}
		else
			infoString += wxString::Format(wxT(" %s"), _("No R3F. Time / Frame"));

		infoString += wxT("\n");


		// effective time
		if(benchmarks[i]->GetInstantDuration() != 0)
		{
			infoString += wxT(" ") + wxString::Format(_("Eff. Time / Frame : %s"), Tools::FormatSeconds(benchmarks[i]->GetEffectiveDuration()).c_str());

			// Compute points per day if possible
			if(project != NULL)
			{
				tFramePPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetEffectiveDuration() * (double)project->GetNbFrames());    // There are 86400 seconds in a day
				infoString += wxT(" ") + wxString::Format(_(" - %.2f ppd"), tFramePPD);
			}
		}
		else
			infoString += wxString::Format(wxT(" %s"), _("No Eff. Time / Frame"));

		infoString += wxT("\n");
	}

	mBenchmarksInformation->SetValue(infoString);
}


/************************************  EVENTS  ************************************/


/**
* The selected project has changed, display the registered benchmarks for the newly selected project
**/
void BenchmarksDialog::OnProjectChanged(wxListEvent& event)
{
	long selection = mListOfProjects->GetFirstSelected();

	if(selection == -1)
		ShowBenchmarks(INVALID_PROJECT_ID);
	else
		ShowBenchmarks(mListOfProjects->GetItemData(selection));
}


/**
* The 'close' button has been pushed
**/
void BenchmarksDialog::OnCloseButton(wxCommandEvent& event)
{
	Close();
}
