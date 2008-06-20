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
 * \file listViewClients.h
 * Creates a list view class for displaying clients.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _LISTVIEWCLIENTS_H
#define _LISTVIEWCLIENTS_H


#include "wx/listctrl.h"
#include "wx/imaglist.h"
#include "wx/dnd.h"


#define PREF_LISTVIEWCLIENTS_STATECOLUMNENABLED    wxT("ListViewClients.StateColumnEnabled")
#define PREF_LISTVIEWCLIENTS_STATECOLUMNENABLED_DV true

#define PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNWIDTH    wxT("ListViewClients.ProgressColumnWidth") /**< Preference setting for the progress column width */
#define PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNWIDTH_DV 82 /**< Default value for progress column width */

#define PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNENABLED    wxT("ListViewClients.ProgressColumnEnabled")
#define PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNENABLED_DV true

#define PREF_LISTVIEWCLIENTS_NAMECOLUMNWIDTH        wxT("ListViewClients.NameColumnWidth") /**< Preference setting for the name column width */
#define PREF_LISTVIEWCLIENTS_NAMECOLUMNWIDTH_DV     80 /**< Default value for name column width */

#define PREF_LISTVIEWCLIENTS_NAMECOLUMNENABLED    wxT("ListViewClients.NameColumnEnabled")
#define PREF_LISTVIEWCLIENTS_NAMECOLUMNENABLED_DV true

#define PREF_LISTVIEWCLIENTS_ETACOLUMNWIDTH         wxT("ListViewClients.ETAColumnWidth") /**< Preference setting for the ETA column width */
#define PREF_LISTVIEWCLIENTS_ETACOLUMNWIDTH_DV      100 /**< Default value for ETA column width */

#define PREF_LISTVIEWCLIENTS_ETACOLUMNENABLED    wxT("ListViewClients.ETAColumnEnabled")
#define PREF_LISTVIEWCLIENTS_ETACOLUMNENABLED_DV true

#define PREF_LISTVIEWCLIENTS_PPDCOLUMNWIDTH         wxT("ListViewClients.PPDColumnWidth") /**< Preference setting for the PPD column width */
#define PREF_LISTVIEWCLIENTS_PPDCOLUMNWIDTH_DV      50 /**< Default value for PPD column width */

#define PREF_LISTVIEWCLIENTS_PPDCOLUMNENABLED    wxT("ListViewClients.PPDColumnEnabled")
#define PREF_LISTVIEWCLIENTS_PPDCOLUMNENABLED_DV true

#define PREF_LISTVIEWCLIENTS_CORECOLUMNWIDTH         wxT("ListViewClients.CoreColumnWidth")
#define PREF_LISTVIEWCLIENTS_CORECOLUMNWIDTH_DV      110

#define PREF_LISTVIEWCLIENTS_CORECOLUMNENABLED    wxT("ListViewClients.CoreColumnEnabled")
#define PREF_LISTVIEWCLIENTS_CORECOLUMNENABLED_DV false

#define PREF_LISTVIEWCLIENTS_PRCGCOLUMNWIDTH         wxT("ListViewClients.PRCGColumnWidth")
#define PREF_LISTVIEWCLIENTS_PRCGCOLUMNWIDTH_DV      150

#define PREF_LISTVIEWCLIENTS_PRCGCOLUMNENABLED    wxT("ListViewClients.PrcgColumnEnabled")
#define PREF_LISTVIEWCLIENTS_PRCGCOLUMNENABLED_DV false

#define PREF_LISTVIEWCLIENTS_CREDITCOLUMNWIDTH         wxT("ListViewClients.CREDITColumnWidth")
#define PREF_LISTVIEWCLIENTS_CREDITCOLUMNWIDTH_DV      60

#define PREF_LISTVIEWCLIENTS_CREDITCOLUMNENABLED    wxT("ListViewClients.CreditColumnEnabled")
#define PREF_LISTVIEWCLIENTS_CREDITCOLUMNENABLED_DV false

#define PREF_LISTVIEWCLIENTS_DOWNLOADEDCOLUMNWIDTH         wxT("ListViewClients.DOWNLOADEDColumnWidth")
#define PREF_LISTVIEWCLIENTS_DOWNLOADEDCOLUMNWIDTH_DV      100

#define PREF_LISTVIEWCLIENTS_DOWNLOADEDCOLUMNENABLED    wxT("ListViewClients.DownloadedColumnEnabled")
#define PREF_LISTVIEWCLIENTS_DOWNLOADEDCOLUMNENABLED_DV false

#define PREF_LISTVIEWCLIENTS_DEADLINECOLUMNWIDTH         wxT("ListViewClients.DEADLINEColumnWidth")
#define PREF_LISTVIEWCLIENTS_DEADLINECOLUMNWIDTH_DV      100

#define PREF_LISTVIEWCLIENTS_DEADLINECOLUMNENABLED    wxT("ListViewClients.DeadlineColumnEnabled")
#define PREF_LISTVIEWCLIENTS_DEADLINECOLUMNENABLED_DV false


#define PREF_LISTVIEWCLIENTS_SORTCOLUMN             wxT("ListViewClients.SortColumn") /**< Preference setting for column to sort by */
#define PREF_LISTVIEWCLIENTS_SORTCOLUMN_DV          LVC_PROGRESS /**< Default value for sorting column */

#define PREF_LISTVIEWCLIENTS_SORTASCENDING          wxT("ListViewClients.SortAscending") /**< Preference setting for ascending sort */
#define PREF_LISTVIEWCLIENTS_SORTASCENDING_DV       false /**< Default value for acscending sort */


WX_DEFINE_ARRAY_INT(int, ArrayOfIndexes); /**< Definition for index array */

/**
 * A subclass of wxListView.
 **/
class ListViewClients : public wxListView
{
friend class Dropfiles; /**< Make friends with the Dropfiles class */
protected:
	bool           mSortAscending; /**< Store the sort order */
	wxUint32       mSortColumn; /**< Store the sort column */
	ArrayOfIndexes mClientIdToIndex; /**< An array of client indices */

	bool           mStateEnabled;
	bool           mProgressEnabled;
	bool           mNameEnabled;
	bool           mETAEnabled;
	bool           mPPDEnabled;
	bool           mCoreEnabled;
	bool           mPRCGEnabled;
	bool           mCreditEnabled;
	bool           mDownloadedEnabled;
	bool           mDeadlineEnabled;

	/**
	 * Sort the list, according to the current sorting criterion.
	 **/
	void Sort(void);

	/**
	 * Event: Left click on column title.
	 * Items are sorted when a column is clicked,
	 * @param event The actual event. This is sent automatically.
	 **/
	void OnColumnLeftClick(wxListEvent& event);

	void OnColumnRightClick(wxListEvent& event);

	/**
	 * Event: Manage right clicks on the list.
	 * A context menu is displayed with miscellaneous entries, allowing the user to directly act on clients
	 * @param event The actual event. This is sent automatically.
	 **/
	void OnRightClick(wxMouseEvent& event);

	/**
	 * Event: Manage double clicks on the list.
	 * Opens the default file manager at the location of the selected client.
	 * @param event The actual event. This is sent automatically.
	 **/
	void OnDoubleClick(wxMouseEvent& event);

	/**
	 * Event: Manage the reload menu item.
	 * Reloads the selected client.
	 * @param event The actual event. This is sent automatically.
	 **/
	void OnMenuReloadClient(wxCommandEvent& event);

	void OnMenuState(wxCommandEvent& event);
	void OnMenuProgress(wxCommandEvent& event);
	void OnMenuName(wxCommandEvent& event);
	void OnMenuETA(wxCommandEvent& event);
	void OnMenuPPD(wxCommandEvent& event);
	void OnMenuCore(wxCommandEvent& event);
	void OnMenuPRCG(wxCommandEvent& event);
	void OnMenuCredit(wxCommandEvent& event);
	void OnMenuDownloaded(wxCommandEvent& event);
	void OnMenuDeadline(wxCommandEvent& event);

	void OnColBeginDrag(wxListEvent& event);

	/**
	 * Event: Manage the edit client menu item.
	 * Edit the selected client.
	 * @param event The actual event. This is sent automatically.
	 **/
	void OnMenuEditClient(wxCommandEvent& event);

	/**
	 * Event: Manage the delete client menu item.
	 * Delete the selected client.
	 * @param event The actual event. This is sent automatically.
	 **/
	void OnMenuDeleteClient(wxCommandEvent& event);

	/**
	 * Event: Manage the view client files menu item.
	 * View client files in filemanager.
	 * @param event The actual event. This is sent automatically.
	 **/
	void OnMenuViewFiles(wxCommandEvent& event);


public:
	/**
	 * Constructor.
	 * Contructs the ListViewClients object.
	 * @param parent The parent window of type wxWindow.
	 * @param id The windows id.
	 * @param nbClients The number of clients to add to the list.
	 **/
	ListViewClients(wxWindow* parent, wxWindowID id, wxUint32 nbClients);

	/**
	 * Destructor.
	 **/
	~ListViewClients(void);

	/**
	 * Initialize the list with the given number of clients.
	 * The list is cleared if there was already something in it.
	 * @param nbClients The number of clients in the list.
	 **/
	void Reset(wxUint32 nbClients);

	/**
	 * Update displayed information on the given client.
	 * @param clientId The client to update.
	 **/
	void UpdateClient(wxUint32 clientId);

	/**
	 * Helper method to update all clients.
	 * It is the same as calling UpdateClient for each known client
	 **/
	void UpdateAllClients(void);

	/**
	 * Compare two clients according to the current sorting criterion.
	 * @param clientId1 First client
	 * @param clientId2 Second client
	 * @return 1 or -1 depending on order
	 **/
	int CompareClients(wxUint32 clientId1, wxUint32 clientId2) const;

	/**
	 * Return the identifier of the currently selected client in the list.
	 **/
	wxUint32 GetSelectedClientId(void) const;

	/**
	 * Extract "cell" text.
	 * This function allows access to single cells in the listView control, much like a grid object.
	 * @param row_number The row of the cell
	 * @param column The column of the cell
	 * @return A string containing the contents of the cell.
	 **/
	wxString GetCellContentsString( long row_number, int column );

	/**
	 * Convert clientId to array index.
	 * @param clientId The client to select.
	 * @return The index of the given client.
	 **/
	wxUint32 ClientIdToIndex(wxUint32 clientId) { return mClientIdToIndex.Item(clientId); };

	/**
	 * Event: Manage the add new client menu item.
	 * Add a new client.
	 *@param event The actual event. This is sent automatically.
	 **/
	void OnMenuAddClient(wxCommandEvent& event);

	/**
	 * Launch filemanager to show client files.
	 **/
	void ShowClientFiles(void);


private:
	DECLARE_EVENT_TABLE()
};

/**
 * A class for managing drag and drop operations.
 **/
class DropFiles: public wxFileDropTarget
{

public:
	/**
	 * Contruct a DropFiles target on a given ListViewClients object.
	 * @param ListView The object to create the DropFiles target on.
	 **/
	DropFiles (ListViewClients *ListView) {m_listview = ListView;}

	/**
	 * Handle dropping files onto the ListViewClients object.
	 * Opens the client edition dialog for every given filename.
	 * @param x X coordinate of dropped file. Ignored.
	 * @param y Y coordinate of dropped file. Ignored.
	 * @param filenames An array of filenames to add to the client list.
	 **/
	virtual bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames);

private:
	ListViewClients *m_listview; /**< The ListViewClients object used as a drop target. */
};


#endif /* _LISTVIEWCLIENTS_H */
