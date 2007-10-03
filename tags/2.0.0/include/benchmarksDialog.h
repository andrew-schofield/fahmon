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
 
#ifndef _BENCHMARKSDIALOG_H
#define _BENCHMARKSDIALOG_H


#include "wx/dialog.h"
#include "wx/listctrl.h"
#include "wx/textctrl.h"
#include "wx/splitter.h"


/**
 * Preferences used by this class
**/
#define PREF_BENCHMARKSDIALOG_FRAMEWIDTH    wxT("BenchmarksDialog.FrameWidth")
#define PREF_BENCHMARKSDIALOG_FRAMEWIDTH_DV 550

#define PREF_BENCHMARKSDIALOG_FRAMEHEIGHT    wxT("BenchmarksDialog.FrameHeight")
#define PREF_BENCHMARKSDIALOG_FRAMEHEIGHT_DV 350

#define PREF_BENCHMARKSDIALOG_COLUMNWIDTH    wxT("BenchmarksDialog.ColumnWidth")
#define PREF_BENCHMARKSDIALOG_COLUMNWIDTH_DV 75

#define PREF_BENCHMARKSDIALOG_SASHPOSITION    wxT("BenchmarksDialog.SashPosition")
#define PREF_BENCHMARKSDIALOG_SASHPOSITION_DV 80


// Allow the user to see the registered benchmarks
// It's a singleton
class BenchmarksDialog : public wxDialog
{
protected:
    static BenchmarksDialog *mInstance;

    wxColour          mOddBackgroundColour;
    wxListView       *mListOfProjects;
    wxTextCtrl       *mBenchmarksInformation;
    wxSplitterWindow *mSplitter;

     BenchmarksDialog(wxWindow* parent);
    ~BenchmarksDialog(void);

    void PopulateProjectsList(ProjectId projectIdToSelect);
    void ShowBenchmarks(ProjectId projectIdToShow);

    void OnProjectChanged(wxListEvent& event);


public:
	// Singleton pattern
	static BenchmarksDialog* GetInstance(wxWindow* parent);
	static void DestroyInstance(void);

    int ShowModal(ProjectId projectIdToSelect);


private:
	DECLARE_EVENT_TABLE()
};

#endif /* _BENCHMARKSDIALOG_H */
