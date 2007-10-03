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
END_EVENT_TABLE()

BenchmarksDialog* BenchmarksDialog::mInstance = NULL;


/**
 * Constructor
**/
BenchmarksDialog::BenchmarksDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, wxString::Format(wxT("Benchmarks / %s"), wxT(FMC_PRODUCT)), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxUint32    frameWidth;
    wxUint32    frameHeight;
    wxUint32    columnWidth;
    wxUint32    sashPosition;
    wxBoxSizer *topLevelSizer;
    wxBoxSizer *mainSizer;

    // TODO
    // Put this in constants
    // This color is used every odd lines, this is easier to read the list this way
    mOddBackgroundColour.Set(228, 228, 228);

    // ---
    mSplitter              = new wxSplitterWindow(this, wxID_ANY);
    mListOfProjects        = new wxListView(mSplitter, LST_PROJECTS, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    mBenchmarksInformation = new wxTextCtrl(mSplitter, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
    
    _PrefsGetUint(PREF_BENCHMARKSDIALOG_SASHPOSITION, sashPosition);
    mSplitter->SplitVertically(mListOfProjects, mBenchmarksInformation);
    mSplitter->SetSashPosition(sashPosition);
    mSplitter->SetMinimumPaneSize(20);
    
    mBenchmarksInformation->SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    _PrefsGetUint(PREF_BENCHMARKSDIALOG_COLUMNWIDTH, columnWidth);
    mListOfProjects->InsertColumn(LC_PROJECT, wxT("Projects"));
    mListOfProjects->SetColumnWidth(LC_PROJECT, columnWidth);
    

    // ---
    mainSizer = new wxBoxSizer(wxVERTICAL);
    
    mainSizer->Add(mSplitter, 1, wxEXPAND);
    mainSizer->AddSpacer(FMC_GUI_SPACING_HIGH);
    mainSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Close")), 0, wxALIGN_CENTER_HORIZONTAL);

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
 * If projectId is equal to MAX_PROJECT_ID, the first project is selected
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

        // Is this the projecy we should select?
        if(projectsList[i] == projectIdToSelect)
            entryToSelect = i;
        
        // Give a slightly darker color to odd lines
        if(i&1 != 0)
            mListOfProjects->SetItemBackgroundColour(i, mOddBackgroundColour);
        else
            mListOfProjects->SetItemBackgroundColour(i, *wxWHITE);
    }
    
    // No memory leak
    delete[] projectsList;

    
    // Select the correct entry
    mListOfProjects->Select(entryToSelect);
}


/**
 * Show the known benchmark for the given project
 * If projectId is equal to MAX_PROJECT_ID, clear information
**/
void BenchmarksDialog::ShowBenchmarks(ProjectId projectIdToShow)
{
    double            minPPD;
    double            avgPPD;
    wxUint32          i;
    wxUint32          nbBenchmarks;
    wxString          infoString;
    const Project    *project;
    const Benchmark **benchmarks;

    // Clear the text area if needed
    if(projectIdToShow == MAX_PROJECT_ID)
    {
        mBenchmarksInformation->SetValue(wxT(""));
        return;
    }

    // Retrieve the benchmarks for the given project
    benchmarks = BenchmarksManager::GetInstance()->GetBenchmarksList(projectIdToShow, nbBenchmarks);
    if(nbBenchmarks == 0)
    {
        mBenchmarksInformation->SetValue(wxT("No benchmarks for this project!"));
        return;
    }

    // Display project information, if it is available
    project = ProjectsManager::GetInstance()->GetProject(projectIdToShow);
    if(project != NULL)
    {
        infoString += wxString::Format(wxT("Project : %u\n"), projectIdToShow);
        infoString += wxString::Format(wxT("Core    : %s\n"), Core::IdToLongName(project->GetCoreId()).c_str());
        infoString += wxString::Format(wxT("Frames  : %u\n"), project->GetNbFrames());
        infoString += wxString::Format(wxT("Credit  : %u\n"), project->GetCredit());
    }
    else
        infoString += wxString::Format(wxT("Project : %u (Unknown)\n"), projectIdToShow);

    for(i=0; i<nbBenchmarks; ++i)
    {
        infoString += wxString::Format(wxT("\n\n -- %s --\n\n"), BenchmarksManager::GetInstance()->GetClientLocationFromClientId(benchmarks[i]->GetClientId()).c_str());

        // Best time
        // Display points per day if possible
        infoString += wxString::Format(wxT("Min. Time / Frame : %s"), Tools::FormatSeconds(benchmarks[i]->GetMinDuration()).c_str());
        if(project != NULL && benchmarks[i]->GetMinDuration() != 0)
        {
            minPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetMinDuration() * (double)project->GetNbFrames());    // There are 86400 seconds in a day
            infoString += wxString::Format(wxT(" - %.2f ppd\n"), minPPD);
        }
        else
            infoString += wxT("\n");

        
        // Average time
        // Display points per day if possible
        infoString += wxString::Format(wxT("Avg. Time / Frame : %s"), Tools::FormatSeconds(benchmarks[i]->GetAvgDuration()).c_str());
        if(project != NULL && benchmarks[i]->GetAvgDuration() != 0)
        {
            avgPPD = (project->GetCredit() * 86400.0) / ((double)benchmarks[i]->GetAvgDuration() * (double)project->GetNbFrames());    // There are 86400 seconds in a day
            infoString += wxString::Format(wxT(" - %.2f ppd\n"), avgPPD);
        }
        else
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
        ShowBenchmarks(MAX_PROJECT_ID);
    else
        ShowBenchmarks(mListOfProjects->GetItemData(selection));
}
