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
 
#ifndef _LISTVIEWCLIENTS_H
#define _LISTVIEWCLIENTS_H


#include "wx/listctrl.h"
#include "wx/imaglist.h"


/**
 * Preferences used by this class
**/
#define PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNWIDTH    wxT("ListViewClients.ProgressColumnWidth")
#define PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNWIDTH_DV 100

#define PREF_LISTVIEWCLIENTS_NAMECOLUMNWIDTH        wxT("ListViewClients.NameColumnWidth")
#define PREF_LISTVIEWCLIENTS_NAMECOLUMNWIDTH_DV     140

#define PREF_LISTVIEWCLIENTS_ETACOLUMNWIDTH         wxT("ListViewClients.ETAColumnWidth")
#define PREF_LISTVIEWCLIENTS_ETACOLUMNWIDTH_DV      100

#define PREF_LISTVIEWCLIENTS_SORTCOLUMN             wxT("ListViewClients.SortColumn")
#define PREF_LISTVIEWCLIENTS_SORTCOLUMN_DV          LVC_PROGRESS

#define PREF_LISTVIEWCLIENTS_SORTASCENDING          wxT("ListViewClients.SortAscending")
#define PREF_LISTVIEWCLIENTS_SORTASCENDING_DV       false


WX_DEFINE_ARRAY_INT(int, ArrayOfIndexes);


class ListViewClients : public wxListView
{
protected:
    bool           mSortAscending;          // 'false' means descending order
    wxUint32       mSortColumn;
    ArrayOfIndexes mClientIdToIndex;

    void Sort(void);
    void OnColumnLeftClick(wxListEvent& event);
    void OnRightClick(wxMouseEvent& event);
    void OnMenuReloadClient(wxCommandEvent& event);
    void OnMenuAddClient(wxCommandEvent& event);
    void OnMenuEditClient(wxCommandEvent& event);
    void OnMenuDeleteClient(wxCommandEvent& event);


public:
     ListViewClients(wxWindow* parent, wxWindowID id, wxUint32 nbClients);
    ~ListViewClients(void);

    void Reset(wxUint32 nbClients);
    void UpdateClient(wxUint32 clientId);
    void UpdateAllClients(void);
    int CompareClients(wxUint32 clientId1, wxUint32 clientId2) const;
    wxUint32 GetSelectedClientId(void) const;


private:
	DECLARE_EVENT_TABLE()
};


#endif /* _LISTVIEWCLIENTS_H */
