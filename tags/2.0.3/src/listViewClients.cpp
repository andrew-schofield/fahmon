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
#include "listViewClients.h"

#include "tools.h"
#include "client.h"
#include "wx/image.h"
#include "mainDialog.h"
#include "clientDialog.h"
#include "clientsManager.h"
#include "preferencesManager.h"

// TODO
// FMC_COLOR_LIST_ODD_LINES creates a new wxColour object each time it is used
// Why not create it once for all in a fahmonConsts.cpp, using a 'extern ...' in the header file?


// The columns
enum _LISTVIEW_COLUMN
{
    LVC_PROGRESS,
    LVC_NAME,
    LVC_ETA
};


// The icons used in the list
enum _LISTVIEW_ICON
{
    LVI_CLIENT_ERROR,
    LVI_CLIENT_INACTIVE,
    LVI_CLIENT_OK,
    LVI_UP_ARROW,
    LVI_DOWN_ARROW
};


// Identifiers for the controls
enum _CONTROL_ID
{
    // --- Context menu
    MID_RELOADCLIENT,
    MID_ADDCLIENT,
    MID_EDITCLIENT,
    MID_DELETECLIENT
};


// Events processed by this class
BEGIN_EVENT_TABLE(ListViewClients, wxListView)
    // Menu events
    EVT_MENU    (MID_RELOADCLIENT,   ListViewClients::OnMenuReloadClient)
    EVT_MENU    (MID_ADDCLIENT,      ListViewClients::OnMenuAddClient)
    EVT_MENU    (MID_EDITCLIENT,     ListViewClients::OnMenuEditClient)
    EVT_MENU    (MID_DELETECLIENT,   ListViewClients::OnMenuDeleteClient)

    // List events
    EVT_LIST_COL_CLICK    (wxID_ANY, ListViewClients::OnColumnLeftClick)
    EVT_RIGHT_DOWN        (          ListViewClients::OnRightClick)
END_EVENT_TABLE()


/**
 * This is the global function used to compare two list entries
**/
int wxCALLBACK ListViewClients_CompareFunction(long clientId1, long clientId2, long pointerToListView)
{
    const ListViewClients *theList = (ListViewClients*)pointerToListView;

    wxASSERT(theList != NULL);

    return theList->CompareClients(clientId1, clientId2);
}


/**
 * Constructor
**/
ListViewClients::ListViewClients(wxWindow* parent, wxWindowID id, wxUint32 nbClients) : wxListView(parent, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL)
{
    wxUint32     progressColumnWidth;
    wxUint32     nameColumnWidth;
    wxUint32     etaColumnWidth;
    wxImageList *imageList;

    // --- Create the columns and restore their size
    InsertColumn(LVC_PROGRESS, wxT("Progress"));
    InsertColumn(LVC_NAME,     wxT("Name"));
    InsertColumn(LVC_ETA,      wxT("ETA"));

    _PrefsGetUint(PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNWIDTH, progressColumnWidth);
    _PrefsGetUint(PREF_LISTVIEWCLIENTS_NAMECOLUMNWIDTH,     nameColumnWidth);
    _PrefsGetUint(PREF_LISTVIEWCLIENTS_ETACOLUMNWIDTH,      etaColumnWidth);

    SetColumnWidth(LVC_PROGRESS, progressColumnWidth);
    SetColumnWidth(LVC_NAME,     nameColumnWidth);
    SetColumnWidth(LVC_ETA,      etaColumnWidth);

    // --- Create the ImageList associated with this ListView
    imageList = new wxImageList(16, 16);
    imageList->Add(wxImage(wxT(FMC_PATH_IMG_LIST_ERROR), wxBITMAP_TYPE_PNG));
    imageList->Add(wxImage(wxT(FMC_PATH_IMG_LIST_INACTIVE), wxBITMAP_TYPE_PNG));
    imageList->Add(wxImage(wxT(FMC_PATH_IMG_LIST_OK), wxBITMAP_TYPE_PNG));
    imageList->Add(wxImage(wxT(FMC_PATH_IMG_LIST_ARROW_UP), wxBITMAP_TYPE_PNG));
    imageList->Add(wxImage(wxT(FMC_PATH_IMG_LIST_ARROW_DOWN), wxBITMAP_TYPE_PNG));
    AssignImageList(imageList, wxIMAGE_LIST_SMALL);

    // --- Restore sorting order
    _PrefsGetUint(PREF_LISTVIEWCLIENTS_SORTCOLUMN,    mSortColumn);
    _PrefsGetBool(PREF_LISTVIEWCLIENTS_SORTASCENDING, mSortAscending);
    
    if(mSortAscending == true)
        SetColumnImage(mSortColumn, LVI_UP_ARROW);
    else
        SetColumnImage(mSortColumn, LVI_DOWN_ARROW);
    
    // --- Initialize the list with the given number of clients
    Reset(nbClients);
}


/**
 * Destructor
**/
ListViewClients::~ListViewClients(void)
{
    // Save the width of the columns
    _PrefsSetUint(PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNWIDTH, GetColumnWidth(LVC_PROGRESS));
    _PrefsSetUint(PREF_LISTVIEWCLIENTS_NAMECOLUMNWIDTH,     GetColumnWidth(LVC_NAME));
    _PrefsSetUint(PREF_LISTVIEWCLIENTS_ETACOLUMNWIDTH,      GetColumnWidth(LVC_ETA));
    
    // Save the sorting order
    _PrefsSetUint(PREF_LISTVIEWCLIENTS_SORTCOLUMN,    mSortColumn);
    _PrefsSetBool(PREF_LISTVIEWCLIENTS_SORTASCENDING, mSortAscending);
}


/**
 * Compare two clients according to the current sorting criterion
**/
int ListViewClients::CompareClients(wxUint32 clientId1, wxUint32 clientId2) const
{
    const Client  *client1;
    const Client  *client2;
          wxInt32  comparisonResult;
    
    client1 = ClientsManager::GetInstance()->Get(clientId1);
    client2 = ClientsManager::GetInstance()->Get(clientId2);

    // Invalid clients must always be at the end of the list
    if(client1->IsOk() == false)
        return 1;
    else if(client2->IsOk() == false)
        return -1;

    // If the two clients are valid, then we compare them using the correct sorting criterion
    switch(mSortColumn)
    {
        // ---
        case LVC_PROGRESS:
            comparisonResult = client1->GetProgress() - client2->GetProgress();
            break;

        // ---
        case LVC_NAME:
            comparisonResult = client1->GetName().CmpNoCase(client2->GetName());
            break;

        // ---
        case LVC_ETA:
            if(client1->GetETA()->IsBefore(client2->GetETA()) == true)
                comparisonResult = -1;
            else
                comparisonResult = 1;
            break;

        // We should never fall here
        default:
            wxASSERT(false);
            comparisonResult = 1;
            break;
    }

    // We need to inverse the result of the comparison if we sort in a descending manner
    if(mSortAscending == false)
        return -comparisonResult;
    
    return comparisonResult;
}


/**
 * Return the identifier of the currently selected client in the list
**/
wxUint32 ListViewClients::GetSelectedClientId(void) const
{
    wxInt32 selectedItemIndex = GetFirstSelected();

    // Check that something is really selected!
    if(selectedItemIndex == -1)
        return FMC_INTMAX;

    // Return the associated client identifier
    return GetItemData(selectedItemIndex);
}


/**
 * Initialize the list with the given number of clients
 * The list is cleared if there was already something in it
**/
void ListViewClients::Reset(wxUint32 nbClients)
{
    wxUint32 i;

    Freeze();
    DeleteAllItems();
    mClientIdToIndex.Empty();

    // Insert dummy entries in the list, one for each client
    for(i=0; i<nbClients; ++i)
    {
        InsertItem(i, wxT(""), LVI_CLIENT_ERROR);
        SetItem(i, LVC_NAME, wxT("Loading..."));

        // Give a slightly darker color to odd lines
        if(i&1 != 0)
            SetItemBackgroundColour(i, FMC_COLOR_LIST_ODD_LINES);
        else
            SetItemBackgroundColour(i, *wxWHITE);

        // Due to the possibility of sorting, items won't keep their order, so we associate the id of clients with their
        // index in the list, so that we can address directly the correct item to update a client
        // We also associate clientId with each item using the possibility to store data
        mClientIdToIndex.Add(i);
        SetItemData(i, i);
    }

    Thaw();
}


/**
 * Helper method to update all clients.
 * It is the same as calling UpdateClient for each known client
**/
void ListViewClients::UpdateAllClients(void)
{
    wxUint32 currentClient;

    for(currentClient=0; currentClient<ClientsManager::GetInstance()->GetCount(); ++currentClient)
        UpdateClient(currentClient);
}


/**
 * Update displayed information on the given client
**/
void ListViewClients::UpdateClient(wxUint32 clientId)
{
    wxUint32      clientIndex;
    const Client *client;

    // The identifier MUST be stored in our translation table
    wxASSERT(clientId < mClientIdToIndex.GetCount());
    
    // Retrieve the correct index and the corresponding client
    clientIndex = mClientIdToIndex.Item(clientId);
    client      = ClientsManager::GetInstance()->Get(clientId);
    
    // If the index is greater than the number of items, then our translation table is really broken
    wxASSERT(clientIndex < (wxUint32)GetItemCount());

    // Update the entry
    Freeze();
    SetItem(clientIndex, LVC_PROGRESS, client->GetProgressString());
    SetItem(clientIndex, LVC_NAME, client->GetName());
    
    // ETA
    if(client->GetProgress() == 100)
        SetItem(clientIndex, LVC_ETA, wxT("Finished"));
    else if(client->GetETA()->IsOk() == true)
        SetItem(clientIndex, LVC_ETA, client->GetETA()->GetString());
    else
        SetItem(clientIndex, LVC_ETA, wxT("No Estimation"));

    // We use leading icons to indicate the status of the client
    if(client->IsOk() == true)
    {
        if(client->IsRunning() == true)
            SetItemImage(clientIndex, LVI_CLIENT_OK);
        else
            SetItemImage(clientIndex, LVI_CLIENT_INACTIVE);
    }
    else
        SetItemImage(clientIndex, LVI_CLIENT_ERROR);
    Thaw();

    // Sort the list: perhaps the value of the sorting criterion has changed!
    Sort();
}


/**
 * Sort the list, according to the current sorting criterion
**/
void ListViewClients::Sort(void)
{
    wxUint32 i;
    wxUint32 selectedClientId;
    wxUint32 selectedClientIndex;

    Freeze();

    // Store the selected client, so that we will be able to to select it again after the sorting job
    selectedClientId = GetSelectedClientId();

    // Sort the list using the default method provided by wxWindows
    SortItems(ListViewClients_CompareFunction, (long)this);

    // Retrieve the new position of each client and correctly color each line
    for(i=0; i<(wxUint32)GetItemCount(); ++i)
    {
        mClientIdToIndex[GetItemData(i)] = i;
        
        if(i&1 != 0)
            SetItemBackgroundColour(i, FMC_COLOR_LIST_ODD_LINES);
        else
            SetItemBackgroundColour(i, *wxWHITE);
    }

    // Select() does not seem to generate a LIST_ITEM_SELECTED event
    // This is good because from an external point of view, nothing has changed (the selected client is the same)
    if(selectedClientId != FMC_INTMAX)
    {
        selectedClientIndex = mClientIdToIndex[selectedClientId];
        
        Select(selectedClientIndex);
        EnsureVisible(selectedClientIndex);
    }
    
    Thaw();
}


/************************************  EVENTS  ************************************/


/**
 * Items are sorted when a column is clicked
**/
void ListViewClients::OnColumnLeftClick(wxListEvent& event)
{
    wxUint32 columnId;

    // Ensure that a column has really been clicked
    if(event.GetColumn() == -1)
        return;

    columnId = (wxUint32)event.GetColumn();

    // Items will be sorted according to columnId
    // If this column is different from the current one, we need to remove the arrow from the latter
    // If the column is the same, we simply change the order
    if(columnId != mSortColumn)
    {
        ClearColumnImage(mSortColumn);

        mSortColumn    = columnId;
        mSortAscending = true;
    }
    else
        mSortAscending = !mSortAscending;

    // Assign the correct sorting arrow to the current column
    if(mSortAscending == true)
        SetColumnImage(mSortColumn, LVI_UP_ARROW);
    else
        SetColumnImage(mSortColumn, LVI_DOWN_ARROW);

    // We can finally sort items
    Sort();
}


/**
 * Manage right clicks on the list
 * A context menu is displayed with miscellaneous entries, allowing the user to directly act on clients
**/
void ListViewClients::OnRightClick(wxMouseEvent& event)
{
    wxMenu  clientContextMenu;
    wxInt32 hitItem;
    wxInt32 hitFlags;
    wxPoint clickPosition;

    // Test if an item was hit by the click or not
    clickPosition = event.GetPosition();
    hitItem       = HitTest(clickPosition, hitFlags);
    if(hitItem == wxNOT_FOUND)
    {
        // Nothing was hit, so we should just allow the creation of a new client
        clientContextMenu.Append(MID_ADDCLIENT, wxT("Add a new client"));
    }
    else
    {
        // An item was hit, so we have to select it
        Select(hitItem);

        // And our context menu should allow some actions on this item
        clientContextMenu.Append(MID_ADDCLIENT, wxT("Add a new client"));
        clientContextMenu.AppendSeparator();
        clientContextMenu.Append(MID_RELOADCLIENT, wxT("Reload this client"));
        clientContextMenu.Append(MID_EDITCLIENT, wxT("Edit this client"));
        clientContextMenu.Append(MID_DELETECLIENT, wxT("Delete this client"));
    }

    PopupMenu(&clientContextMenu);
}


/**
 * Reload the selected client
**/
void ListViewClients::OnMenuReloadClient(wxCommandEvent& event)
{
    wxUint32 selectedClientId = GetSelectedClientId();
    
    // Ensure that something is really selected
    if(selectedClientId != FMC_INTMAX)
        ClientsManager::GetInstance()->ReloadThreaded(selectedClientId);
}


/**
 * Add a new client
**/
void ListViewClients::OnMenuAddClient(wxCommandEvent& event)
{
    // FMC_INTMAX means that we want to create a new client, not to edit an existing one
    ClientDialog::GetInstance(this)->ShowModal(FMC_INTMAX);
}


/**
 * Edit the selected client
**/
void ListViewClients::OnMenuEditClient(wxCommandEvent& event)
{
    wxUint32 selectedClientId = GetSelectedClientId();
    
    // Ensure that something is really selected
    if(selectedClientId == FMC_INTMAX)
        return;
    
    // Ask the main dialog to edit this client
    ClientDialog::GetInstance(this)->ShowModal(selectedClientId);
}


/**
 * Delete the selected client
**/
void ListViewClients::OnMenuDeleteClient(wxCommandEvent& event)
{
    wxUint32       selectedClientId;
    wxCommandEvent deleteEvent(EVT_CLIENTDELETED);

    // Ensure that something is really selected
    selectedClientId = GetSelectedClientId();
    if(selectedClientId == FMC_INTMAX)
        return;

    // Ensure that the user did not ask for deletion by error
    if(Tools::QuestionMsgBox(wxT("Do you really want to delete this client?")) == true)
    {
        // Delete the client
        ClientsManager::GetInstance()->Delete(selectedClientId);
        
        // And warn the main dialog
        MainDialog::GetInstance()->AddPendingEvent(deleteEvent);
    }
}
