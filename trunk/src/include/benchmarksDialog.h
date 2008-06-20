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
 * \file benchmarksDialog.h
 * The benchmarks dialog.
 * Creates the dialog to list and display available benchmarks per project.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _BENCHMARKSDIALOG_H
#define _BENCHMARKSDIALOG_H


#include "wx/dialog.h"
#include "wx/listctrl.h"
#include "wx/textctrl.h"
#include "wx/splitter.h"


#define PREF_BENCHMARKSDIALOG_FRAMEWIDTH    wxT("BenchmarksDialog.FrameWidth") /**< Preference setting for benchmark dialog width */
#define PREF_BENCHMARKSDIALOG_FRAMEWIDTH_DV 550 /**< Default value for benchmark dialog width */

#define PREF_BENCHMARKSDIALOG_FRAMEHEIGHT    wxT("BenchmarksDialog.FrameHeight") /**< Preference setting for benchmark dialog height */
#define PREF_BENCHMARKSDIALOG_FRAMEHEIGHT_DV 350 /**< Default value for benchmark dialog height */

#define PREF_BENCHMARKSDIALOG_COLUMNWIDTH    wxT("BenchmarksDialog.ColumnWidth") /**< Preference setting for projects column width */
#define PREF_BENCHMARKSDIALOG_COLUMNWIDTH_DV 75 /**< Default value for projects column width */

#define PREF_BENCHMARKSDIALOG_SASHPOSITION    wxT("BenchmarksDialog.SashPosition") /**< Preference setting for projects column sash position */
#define PREF_BENCHMARKSDIALOG_SASHPOSITION_DV 80 /**< Default value for projects column sash position */


/**
 * Benchmarks Dialog class.
 * This class can only be instanciated once.
 */
class BenchmarksDialog : public wxDialog
{
protected:
	static BenchmarksDialog *mInstance; /**< The single instance of the Benchmarks Dialog */

	wxListView       *mListOfProjects; /**< Object storing the Project list  */
	wxTextCtrl       *mBenchmarksInformation; /**< Object storing the benchmark data */
	wxSplitterWindow *mSplitter; /**< Splitter object */

	/**
	 * Constructor.
	 * Contructs a Benchmark Dialog of type wxDialog with a title of "Benchmarks".
	 * @param parent The parent window of type wxWindow
	 **/
	BenchmarksDialog(wxWindow* parent);

	/**
	 * Destructor.
	 **/
	~BenchmarksDialog(void);

	/**
	 * Populate the project list.
	 * Fill the list with all the project identifiers for which a benchmark is registered.
	 * Select by default the given project.
	 * If there is no benchmark for this project, select the first entry.
	 * @param projectIdToSelect The project number to select.
	 **/
	void PopulateProjectsList(ProjectId projectIdToSelect);

	/**
	 * Show the known benchmark for the given project.
	 * If projectId is equal to INVALID_PROJECT_ID, clear information.
	 * @param projectIdToShow The project number to select.
	 **/
	void ShowBenchmarks(ProjectId projectIdToShow);

	// Events
	/**
	 * Event: Project changed.
	 * The selected project has changed, so display the registered benchmarks for the newly selected project.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnProjectChanged(wxListEvent& event);

	/**
	 * Event: The 'close' button has been pushed.
	 * Closes the benchmarks dialog.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnCloseButton(wxCommandEvent& event);


public:
	/**
 	 * Retrieve the instance of BenchmarksDialog.
	 * If an instance doesn't already exist, create it.
	 * @param parent The parent windows of type wxWindow.
	 * @return An instance of class BenchmarksDialog.
	 **/
	static BenchmarksDialog* GetInstance(wxWindow* parent);

	/**
	 * Destroy the instance of BenchmarksDialog.
	 * Only if an instance exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Show the Benchmarks Dialog.
	 * Opens a modal window and centres it.
	 * The given project is automatically selected. If the project is equal to INVALID_PROJECT_ID, the first available project is selected.
	 * @param projectIdToSelect The project number to select.
	 **/
	int ShowModal(ProjectId projectIdToSelect);


private:
	DECLARE_EVENT_TABLE()
};

#endif /* _BENCHMARKSDIALOG_H */
