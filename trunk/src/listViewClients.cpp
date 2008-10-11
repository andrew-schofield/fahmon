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
 * \file listViewClients.cpp
 * Creates a list view class for displaying clients.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "listViewClients.h"

#include "tools.h"
#include "client.h"
#include "mainDialog.h"
#include "pathManager.h"
#include "clientDialog.h"
#include "clientsManager.h"
#include "preferencesManager.h"
#include "projectsManager.h"
#include "benchmarksManager.h"

#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/settings.h"
#include "wx/image.h"

// TODO
// FMC_COLOR_LIST_ODD_LINES creates a new wxColour object each time it is used
// Why not create it once for all in a fahmonConsts.cpp, using a 'extern ...' in the header file?


// The columns
enum _LISTVIEW_COLUMN
{
	LVC_STATUS,
	LVC_PROGRESS,
	LVC_NAME,
	LVC_ETA,
	LVC_PPD,
	LVC_CORE,
	LVC_PRCG,
	LVC_CREDIT,
	LVC_DOWNLOADED,
	LVC_DEADLINE
};


// The icons used in the list
enum _LISTVIEW_ICON
{
	LVI_CLIENT_STOPPED,
	LVI_CLIENT_INACTIVE,
	LVI_CLIENT_INACCESSIBLE,
	LVI_CLIENT_OK,
	LVI_CLIENT_ASYNCH,
	LVI_CLIENT_PAUSED,
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
	MID_DELETECLIENT,
	MID_VIEWFILES,
	MID_STATE,
	MID_PROGRESS,
	MID_NAME,
	MID_ETA,
	MID_PPD,
	MID_CORE,
	MID_PRCG,
	MID_CREDIT,
	MID_DOWNLOADED,
	MID_DEADLINE,
	MID_ENDISABLE
};


// Events processed by this class
BEGIN_EVENT_TABLE(ListViewClients, wxListView)
	// Menu events
	EVT_MENU    (MID_RELOADCLIENT,   ListViewClients::OnMenuReloadClient)
	EVT_MENU    (MID_ADDCLIENT,      ListViewClients::OnMenuAddClient)
	EVT_MENU    (MID_EDITCLIENT,     ListViewClients::OnMenuEditClient)
	EVT_MENU    (MID_DELETECLIENT,   ListViewClients::OnMenuDeleteClient)
	EVT_MENU    (MID_VIEWFILES,      ListViewClients::OnMenuViewFiles)
	EVT_MENU    (MID_ENDISABLE,      ListViewClients::OnMenuEnDisable)

	EVT_MENU    (MID_STATE,          ListViewClients::OnMenuState)
	EVT_MENU    (MID_PROGRESS,       ListViewClients::OnMenuProgress)
	EVT_MENU    (MID_NAME,           ListViewClients::OnMenuName)
	EVT_MENU    (MID_ETA,            ListViewClients::OnMenuETA)
	EVT_MENU    (MID_PPD,            ListViewClients::OnMenuPPD)
	EVT_MENU    (MID_CORE,           ListViewClients::OnMenuCore)
	EVT_MENU    (MID_PRCG,           ListViewClients::OnMenuPRCG)
	EVT_MENU    (MID_CREDIT,         ListViewClients::OnMenuCredit)
	EVT_MENU    (MID_DOWNLOADED,     ListViewClients::OnMenuDownloaded)
	EVT_MENU    (MID_DEADLINE,       ListViewClients::OnMenuDeadline)

	// List events
	EVT_LIST_COL_CLICK       (wxID_ANY, ListViewClients::OnColumnLeftClick)
	EVT_LIST_COL_RIGHT_CLICK (wxID_ANY, ListViewClients::OnColumnRightClick)
	EVT_RIGHT_DOWN           (          ListViewClients::OnRightClick)
	EVT_LEFT_DCLICK          (          ListViewClients::OnDoubleClick)

	EVT_LIST_COL_BEGIN_DRAG (wxID_ANY, ListViewClients::OnColBeginDrag)
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


ListViewClients::ListViewClients(wxWindow* parent, wxWindowID id, wxUint32 nbClients) : wxListView(parent, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES)
{
	wxUint32     statusColumnWidth;
	wxUint32     progressColumnWidth;
	wxUint32     nameColumnWidth;
	wxUint32     etaColumnWidth;
	wxUint32     ppdColumnWidth;
	wxUint32     coreColumnWidth;
	wxUint32     prcgColumnWidth;
	wxUint32     creditColumnWidth;
	wxUint32     downloadedColumnWidth;
	wxUint32     deadlineColumnWidth;
	wxImageList *imageList;

	// --- Create the columns and restore their size
	InsertColumn(LVC_STATUS,     wxT(""));
	InsertColumn(LVC_PROGRESS,   _("Progress"));
	InsertColumn(LVC_NAME,       _("Name"));
	InsertColumn(LVC_ETA,        _("ETA"));
	InsertColumn(LVC_PPD,        _("PPD"));
	InsertColumn(LVC_CORE,       _("Core"));
	InsertColumn(LVC_PRCG,       _T("PRCG"));
	InsertColumn(LVC_CREDIT,     _("Credit"));
	InsertColumn(LVC_DOWNLOADED, _("Downloaded"));
	InsertColumn(LVC_DEADLINE,   _("Deadline"));

	_PrefsGetUint(PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNWIDTH,   progressColumnWidth);
	_PrefsGetUint(PREF_LISTVIEWCLIENTS_NAMECOLUMNWIDTH,       nameColumnWidth);
	_PrefsGetUint(PREF_LISTVIEWCLIENTS_ETACOLUMNWIDTH,        etaColumnWidth);
	_PrefsGetUint(PREF_LISTVIEWCLIENTS_PPDCOLUMNWIDTH,        ppdColumnWidth);
	_PrefsGetUint(PREF_LISTVIEWCLIENTS_CORECOLUMNWIDTH,       coreColumnWidth);
	_PrefsGetUint(PREF_LISTVIEWCLIENTS_PRCGCOLUMNWIDTH,       prcgColumnWidth);
	_PrefsGetUint(PREF_LISTVIEWCLIENTS_CREDITCOLUMNWIDTH,     creditColumnWidth);
	_PrefsGetUint(PREF_LISTVIEWCLIENTS_DOWNLOADEDCOLUMNWIDTH, downloadedColumnWidth);
	_PrefsGetUint(PREF_LISTVIEWCLIENTS_DEADLINECOLUMNWIDTH,   deadlineColumnWidth);


	_PrefsGetBool(PREF_LISTVIEWCLIENTS_STATECOLUMNENABLED,      mStateEnabled);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNENABLED,   mProgressEnabled);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_NAMECOLUMNENABLED,       mNameEnabled);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_ETACOLUMNENABLED,        mETAEnabled);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_PPDCOLUMNENABLED,        mPPDEnabled);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_CORECOLUMNENABLED,       mCoreEnabled);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_PRCGCOLUMNENABLED,       mPRCGEnabled);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_CREDITCOLUMNENABLED,     mCreditEnabled);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_DOWNLOADEDCOLUMNENABLED, mDownloadedEnabled);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_DEADLINECOLUMNENABLED,   mDeadlineEnabled);

	progressColumnWidth =   (mProgressEnabled)   ? progressColumnWidth   : 0;
	nameColumnWidth =       (mNameEnabled)       ? nameColumnWidth       : 0;
	etaColumnWidth =        (mETAEnabled)        ? etaColumnWidth        : 0;
	ppdColumnWidth =        (mPPDEnabled)        ? ppdColumnWidth        : 0;
	statusColumnWidth =     (mStateEnabled)      ? 30                    : 0;
	coreColumnWidth =       (mCoreEnabled)       ? coreColumnWidth       : 0;
	prcgColumnWidth =       (mPRCGEnabled)       ? prcgColumnWidth       : 0;
	creditColumnWidth =     (mCreditEnabled)     ? creditColumnWidth     : 0;
	downloadedColumnWidth = (mDownloadedEnabled) ? downloadedColumnWidth : 0;
	deadlineColumnWidth =   (mDeadlineEnabled)   ? deadlineColumnWidth   : 0;

	SetColumnWidth(LVC_PROGRESS,   progressColumnWidth);
	SetColumnWidth(LVC_NAME,       nameColumnWidth);
	SetColumnWidth(LVC_ETA,        etaColumnWidth);
	SetColumnWidth(LVC_PPD,        ppdColumnWidth);
	SetColumnWidth(LVC_STATUS,     statusColumnWidth);
	SetColumnWidth(LVC_CORE,       coreColumnWidth);
	SetColumnWidth(LVC_PRCG,       prcgColumnWidth);
	SetColumnWidth(LVC_CREDIT,     creditColumnWidth);
	SetColumnWidth(LVC_DOWNLOADED, downloadedColumnWidth);
	SetColumnWidth(LVC_DEADLINE,   deadlineColumnWidth);

	// --- Create the ImageList associated with this ListView
	//     Images must be loaded in the order defined by the enum _LISTVIEW_ICON
	imageList = new wxImageList(16, 16);
	imageList->Add(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_LIST_STOPPED), wxBITMAP_TYPE_PNG));
	imageList->Add(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_LIST_INACTIVE), wxBITMAP_TYPE_PNG));
	imageList->Add(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_LIST_INACCESSIBLE), wxBITMAP_TYPE_PNG));
	imageList->Add(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_LIST_OK), wxBITMAP_TYPE_PNG));
	imageList->Add(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_LIST_ASYNCH), wxBITMAP_TYPE_PNG));
	imageList->Add(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_LIST_PAUSED), wxBITMAP_TYPE_PNG));
	imageList->Add(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_LIST_ARROW_UP), wxBITMAP_TYPE_PNG));
	imageList->Add(wxImage(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_LIST_ARROW_DOWN), wxBITMAP_TYPE_PNG));
	AssignImageList(imageList, wxIMAGE_LIST_SMALL);

	// --- Restore sorting order
	_PrefsGetUint(PREF_LISTVIEWCLIENTS_SORTCOLUMN,    mSortColumn);
	_PrefsGetBool(PREF_LISTVIEWCLIENTS_SORTASCENDING, mSortAscending);

	if(mSortAscending == true)
	{
		SetColumnImage(mSortColumn, LVI_UP_ARROW);
	}
	else
	{
		SetColumnImage(mSortColumn, LVI_DOWN_ARROW);
	}

	// --- Initialize the list with the given number of clients
	Reset(nbClients);
	// Initialise drop target
	SetDropTarget (new DropFiles (this));
}


ListViewClients::~ListViewClients(void)
{
	// Save the width of the columns
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNWIDTH,   GetColumnWidth(LVC_PROGRESS));
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_NAMECOLUMNWIDTH,       GetColumnWidth(LVC_NAME));
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_ETACOLUMNWIDTH,        GetColumnWidth(LVC_ETA));
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_PPDCOLUMNWIDTH,        GetColumnWidth(LVC_PPD));
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_CORECOLUMNWIDTH,       GetColumnWidth(LVC_CORE));
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_PRCGCOLUMNWIDTH,       GetColumnWidth(LVC_PRCG));
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_CREDITCOLUMNWIDTH,     GetColumnWidth(LVC_CREDIT));
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_DOWNLOADEDCOLUMNWIDTH, GetColumnWidth(LVC_DOWNLOADED));
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_DEADLINECOLUMNWIDTH,   GetColumnWidth(LVC_DEADLINE));

	//Save the enabled state of the columns
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNENABLED,   mProgressEnabled);
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_NAMECOLUMNENABLED,       mNameEnabled);
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_ETACOLUMNENABLED,        mETAEnabled);
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_PPDCOLUMNENABLED,        mPPDEnabled);
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_CORECOLUMNENABLED,       mCoreEnabled);
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_PRCGCOLUMNENABLED,       mPRCGEnabled);
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_CREDITCOLUMNENABLED,     mCreditEnabled);
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_DOWNLOADEDCOLUMNENABLED, mDownloadedEnabled);
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_DEADLINECOLUMNENABLED,   mDeadlineEnabled);

	// Save the sorting order
	_PrefsSetUint(PREF_LISTVIEWCLIENTS_SORTCOLUMN,    mSortColumn);
	_PrefsSetBool(PREF_LISTVIEWCLIENTS_SORTASCENDING, mSortAscending);
}


int ListViewClients::CompareClients(wxUint32 clientId1, wxUint32 clientId2) const
{
	const Client  *client1;
	const Client  *client2;
	wxInt32  comparisonResult;
	int Client1State;
	int Client2State;
	bool keepDeadLast;


	_PrefsGetBool(PREF_LISTCLIENTS_KEEP_DEAD_LAST,      keepDeadLast);

	client1 = ClientsManager::GetInstance()->Get(clientId1);
	client2 = ClientsManager::GetInstance()->Get(clientId2);

	Client1State = 0;
	Client2State = 0;

	// Invalid clients must always be at the end of the list
	if(keepDeadLast == true)
	{
		if(!client1->IsAccessible() || !client1->IsEnabled())
		{
			return 1;
		}
		else if(!client2->IsAccessible() || !client1->IsEnabled())
		{
			return -1;
		}
	}

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
			{
				comparisonResult = -1;
			}
			else if(client1->GetETA()->IsEqualTo(client2->GetETA()))
			{
				comparisonResult = 0;
			}
			else
			{
				comparisonResult = 1;
			}
			break;

		// ---
		case LVC_PPD:
			if(client1->GetPPD() > client2->GetPPD())
			{
				comparisonResult = -1;
			}
			else if(client1->GetPPD() == client2->GetPPD())
			{
				comparisonResult = 0;
			}
			else
			{
				comparisonResult = 1;
			}
			break;

		// ---
		case LVC_STATUS:
			// enumerate the client statuses
			if(!client1->IsAccessible())
			{
				Client1State = 0; // dead client
			}
			if(!client2->IsAccessible())
			{
				Client2State = 0;
			}
			if(client1->IsStopped())
			{
				Client1State = 1; // stopped client
			}
			if(client2->IsStopped())
			{
				Client2State = 1;
			}
			if(client1->IsHung())
			{
				Client1State = 2; // hung client
			}
			if(client2->IsHung())
			{
				Client2State = 2;
			}
			if(client1->IsInactive())
			{
				Client1State = 3; // inactive client
			}
			if(client2->IsInactive())
			{
				Client2State = 3;
			}
			if(client1->IsPaused())
			{
				Client1State = 4;
			}
			if(client2->IsPaused())
			{
				Client2State = 4;
			}
			if(client1->IsAccessible() && !client1->IsInactive() && !client1->IsStopped() && !client1->IsPaused())
			{
				Client1State = 5; //active client
			}
			if(client2->IsAccessible() && !client2->IsInactive() && !client2->IsStopped() && !client2->IsPaused())
			{
				Client2State = 5;
			}
			if(Client1State > Client2State)
			{
				comparisonResult = -1;
			}
			else if(Client1State == Client2State)
			{
				comparisonResult = 0;
			}
			else
			{
				comparisonResult = 1;
			}
			break;

		case LVC_DOWNLOADED:
			if(client1->GetDownloadDate().IsEarlierThan(client2->GetDownloadDate()) == true)
			{
				comparisonResult = 1;
			}
			else if(client1->GetDownloadDate().IsEqualTo(client2->GetDownloadDate()))
			{
				comparisonResult = 0;
			}
			else
			{
				comparisonResult = -1;
			}
			break;

		// ---
		case LVC_CORE:
			comparisonResult = client1->GetCore().CmpNoCase(client2->GetCore());
			break;

		// ---
		case LVC_PRCG:
			comparisonResult = wxString::Format(wxT("P%i (R%i, C%i, G%i)"), client1->GetProjectId(), client1->GetProjectRun(), client1->GetProjectClone(), client1->GetProjectGen()).CmpNoCase(wxString::Format(wxT("P%i (R%i, C%i, G%i)"), client2->GetProjectId(), client2->GetProjectRun(), client2->GetProjectClone(), client2->GetProjectGen()));
			break;

		// ---
		case LVC_CREDIT:
			if(client1->GetCredit() > client2->GetCredit())
			{
				comparisonResult = -1;
			}
			else if(client1->GetCredit() == client2->GetCredit())
			{
				comparisonResult = 0;
			}
			else
			{
				comparisonResult = 1;
			}
			break;

		// ---
		case LVC_DEADLINE:
			if(client1->GetDeadlineDate().IsEarlierThan(client2->GetDeadlineDate()) == true)
			{
				comparisonResult = 1;
			}
			else if(client1->GetDeadlineDate().IsEqualTo(client2->GetDeadlineDate()))
			{
				comparisonResult = 0;
			}
			else
			{
				comparisonResult = -1;
			}
			break;

		// We should never fall here
		default:
			wxASSERT(false);
			comparisonResult = 1;
			break;
	}

	// We need to inverse the result of the comparison if we sort in a descending manner
	if(mSortAscending == false)
	{
		return -comparisonResult;
	}

	return comparisonResult;
}


wxUint32 ListViewClients::GetSelectedClientId(void) const
{
	wxInt32 selectedItemIndex = GetFirstSelected();

	// Check that something is really selected!
	if(selectedItemIndex == -1)
	{
		return INVALID_CLIENT_ID;
	}

	// Return the associated client identifier
	return GetItemData(selectedItemIndex);
}


void ListViewClients::Reset(wxUint32 nbClients)
{
	wxUint32 i;

	Freeze();
	DeleteAllItems();
	mClientIdToIndex.Empty();

	SetTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

	// Insert dummy entries in the list, one for each client
	for(i=0; i<nbClients; ++i)
	{
		InsertItem(i, wxT(""), LVI_CLIENT_STOPPED);
		SetItem(i, LVC_NAME, _("Loading..."));

		// Due to the possibility of sorting, items won't keep their order, so we associate the id of clients with their
		// index in the list, so that we can address directly the correct item to update a client
		// We also associate clientId with each item using the possibility to store data
		mClientIdToIndex.Add(i);
		SetItemData(i, i);
	}

	Thaw();
}


void ListViewClients::UpdateAllClients(void)
{
	wxUint32 currentClient;

	for(currentClient=0; currentClient<ClientsManager::GetInstance()->GetCount(); ++currentClient)
	{
		UpdateClient(currentClient);
	}
}


void ListViewClients::UpdateClient(wxUint32 clientId)
{
	wxUint32	clientIndex;
	wxString	PPD;
	wxString	clientLocation;
	wxString	clientName;
	const Client *client;
	wxInt32       timeInMinutes;
	wxInt32       nbDays;
	wxInt32       nbHours;
	wxInt32       nbMinutes;
	wxString      tempString;
	wxUint32      deadlineDays;
	wxDateTime    preferredDeadline;
	wxDateTime    finalDeadline;
	wxDateTime    downloadTime;
	wxDateTime    timeNow;
	wxTimeSpan    timeDiff;


	_PrefsGetUint(PREF_ETA_DISPLAYSTYLE, deadlineDays);

	// The identifier MUST be stored in our translation table
	wxASSERT(clientId < mClientIdToIndex.GetCount());

	// Retrieve the correct index and the corresponding client
	client      = ClientsManager::GetInstance()->Get(clientId);
	clientIndex = mClientIdToIndex.Item(clientId);

	// If the index is greater than the number of items, then our translation table is really broken
	wxASSERT(clientIndex < (wxUint32)GetItemCount());

	// Prevent refresh while updating entry
	Freeze();

	SetItem(clientIndex, LVC_PROGRESS, client->GetProgressString());
	SetItem(clientIndex, LVC_NAME, client->GetName());

	// Blank the PPD column
	PPD = wxT("--");

	// If it's possible to get the PPD, do so now
	if(client->IsAccessible() && !client->IsStopped() && !client->IsHung() && client->IsEnabled() && !client->IsPaused())
	{
		PPD = wxString::Format(wxT("%.2f"), client->GetPPD());
		if(!client->GetIsFrameCountAccurate())
		{
			PPD = PPD + wxT("*");
		}
	}

	// we're less stringent about when we display other data
	if(client->IsAccessible() && !client->IsHung() && client->IsEnabled())
	{
		//PRCG
		SetItem(clientIndex, LVC_PRCG, wxString::Format(wxT("P%i (R%i, C%i, G%i)"), client->GetProjectId(), client->GetProjectRun(), client->GetProjectClone(), client->GetProjectGen()));

		//core name
		wxString corename = (client->GetCore() == wxT("")) ? wxT("Unknown") : client->GetCore();
		SetItem(clientIndex, LVC_CORE, corename);
		//credit
		wxString creditvalue = (client->GetCredit() == 0) ? wxT("Unknown") : wxString::Format(_("%u points"), client->GetCredit());
		SetItem(clientIndex, LVC_CREDIT, creditvalue);
		//downloaded
		if(client->GetDownloadDate().IsValid())
		{

			timeNow = wxDateTime::Now();
			downloadTime = client->GetDownloadDate();
			if(deadlineDays == ETADS_LEFT_TIME)
			{
			//timeDiff = timeNow.Subtract(downloadTime);
				timeInMinutes = timeNow.Subtract(downloadTime).GetMinutes();

			// Split the left time into days, hours and minutes
				nbDays    = timeInMinutes / (24 * 60);
				nbMinutes = timeInMinutes % (24 * 60);
				nbHours   = nbMinutes / 60;
				nbMinutes = nbMinutes % 60;
			// Use a friendly format
				if(nbDays != 0)
				{
					tempString = wxString::Format(wxT("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
				}
				else if(nbHours != 0)
				{
					tempString = wxString::Format(wxT("%ih %02imn"), nbHours, nbMinutes);
				}
				else
				{
					tempString = wxString::Format(wxT("%imn"), nbMinutes);
				}

				SetItem(clientIndex, LVC_DOWNLOADED, wxString::Format(_("%s ago"), tempString.c_str()));
			}
			else if (deadlineDays == ETADS_DATE_DAY_MONTH)
			{
				SetItem(clientIndex, LVC_DOWNLOADED, wxString::Format(wxT("%s"), downloadTime.Format(wxT("%d %B, %H:%M")).c_str()));
			}
			else
			{
				SetItem(clientIndex, LVC_DOWNLOADED, wxString::Format(wxT("%s"), downloadTime.Format(wxT("%B %d, %H:%M")).c_str()));
			}
		}
		else
		{
			SetItem(clientIndex, LVC_DOWNLOADED, _("Unknown"));
		}if(client->GetDownloadDate().IsValid() && client->GetDeadlineDate().IsValid())
		{
			preferredDeadline = client->GetDeadlineDate();
			if(deadlineDays == ETADS_LEFT_TIME)
			{
				timeDiff = preferredDeadline.Subtract(timeNow);
				timeInMinutes = timeDiff.GetMinutes();
				if(timeDiff.GetMinutes() < 0)
				{
					timeInMinutes = 0 - timeInMinutes;
				}

				// Split the left time into days, hours and minutes
				nbDays    = timeInMinutes / (24 * 60);
				nbMinutes = timeInMinutes % (24 * 60);
				nbHours   = nbMinutes / 60;
				nbMinutes = nbMinutes % 60;
				// Use a friendly format
				if(nbDays != 0)
				{
					tempString = wxString::Format(wxT("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
				}
				else if(nbHours != 0)
				{
					tempString = wxString::Format(wxT("%ih %02imn"), nbHours, nbMinutes);
				}
				else
				{
					tempString = wxString::Format(wxT("%imn"), nbMinutes);
				}

				if(timeDiff.GetMinutes() < 0)
				{
					SetItem(clientIndex, LVC_DEADLINE, wxString::Format(_("%s ago"), tempString.c_str()));
				}
				else
				{
					SetItem(clientIndex, LVC_DEADLINE, wxString::Format(_("In %s"), tempString.c_str()));
				}

			}
			else if (deadlineDays == ETADS_DATE_DAY_MONTH)
			{
				SetItem(clientIndex, LVC_DEADLINE, wxString::Format(wxT("%s"), preferredDeadline.Format(wxT("%d %B, %H:%M")).c_str()));
			}
			else
			{
				SetItem(clientIndex, LVC_DEADLINE, wxString::Format(wxT("%s"), preferredDeadline.Format(wxT("%B %d, %H:%M")).c_str()));
			}
		}
		else
		{
			SetItem(clientIndex, LVC_DEADLINE, _("Unknown"));
		}
	} else {
		SetItem(clientIndex, LVC_PRCG, wxT(""));
		SetItem(clientIndex, LVC_CORE, wxT(""));
		SetItem(clientIndex, LVC_CREDIT, wxT(""));
		SetItem(clientIndex, LVC_DOWNLOADED, wxT(""));
		SetItem(clientIndex, LVC_DEADLINE, wxT(""));
	}
	SetItem(clientIndex, LVC_PPD, PPD);

	// ETA
	if(!client->IsEnabled())
	{
		SetItem(clientIndex, LVC_ETA, _("Disabled"));
	}
	else if(client->GetProgress() == 100)
	{
		SetItem(clientIndex, LVC_ETA, _("Finished"));
	}
	else if(!client->IsAccessible() || client->IsStopped())
	{
		SetItem(clientIndex, LVC_ETA, _("N/A"));
	}
	else if(!client->GetETA()->IsOk())
	{
		SetItem(clientIndex, LVC_ETA, _("N/A"));
	}
	else if(client->IsHung())
	{
		SetItem(clientIndex, LVC_ETA, _("*Hung*"));
	}
	else if(client->IsPaused())
	{
		SetItem(clientIndex, LVC_ETA, _("Paused"));
	}
	else
	{
		SetItem(clientIndex, LVC_ETA, client->GetETA()->GetString());
	}

	// We use leading icons to indicate the status of the client
	if(!client->IsAccessible() || !client->IsEnabled())
	{
		SetItemImage(clientIndex, LVI_CLIENT_INACCESSIBLE);
	}
	else if(client->IsStopped())
	{
		SetItemImage(clientIndex, LVI_CLIENT_STOPPED);
	}
	else if(client->IsInactive())
	{
		SetItemImage(clientIndex, LVI_CLIENT_INACTIVE);
	}
	else if(client->IsHung())
	{
		SetItemImage(clientIndex, LVI_CLIENT_STOPPED);
	}
	else if(client->IsAsynch())
	{
		SetItemImage(clientIndex, LVI_CLIENT_ASYNCH);
	}
	else if(client->IsPaused())
	{
		SetItemImage(clientIndex, LVI_CLIENT_PAUSED);
	}
	else
	{
		SetItemImage(clientIndex, LVI_CLIENT_OK);
	}


	// Re-enable refresh
	Thaw();

	// Sort the list: perhaps the value of the sorting criterion has changed!
	Sort();
}


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

	SetTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	// Retrieve the new position of each client and correctly color each line
	for(i=0; i<(wxUint32)GetItemCount(); ++i)
	{
		mClientIdToIndex[GetItemData(i)] = i;
	}

	// Select() does not seem to generate a LIST_ITEM_SELECTED event
	// This is good because from an external point of view, nothing has changed (the selected client is the same)
	if(selectedClientId != INVALID_CLIENT_ID)
	{
		selectedClientIndex = mClientIdToIndex[selectedClientId];

		Select(selectedClientIndex);
		EnsureVisible(selectedClientIndex);
	}

	Thaw();
}


/************************************  EVENTS  ************************************/


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
	{
		SetColumnImage(mSortColumn, LVI_UP_ARROW);
	}
	else
	{
		SetColumnImage(mSortColumn, LVI_DOWN_ARROW);
	}

	// We can finally sort items
	Sort();
}


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
		clientContextMenu.Append(MID_ADDCLIENT, _("Add a new client"));
	}
	else
	{
		// An item was hit, so we have to select it
		Select(hitItem);

		// And our context menu should allow some actions on this item
		clientContextMenu.Append(MID_ADDCLIENT, _("Add a new client"));
		clientContextMenu.AppendSeparator();
		clientContextMenu.Append(MID_RELOADCLIENT, _("Reload this client"));
		clientContextMenu.Append(MID_EDITCLIENT, _("Edit this client"));
		clientContextMenu.Append(MID_DELETECLIENT, _("Delete this client"));
		clientContextMenu.Append(MID_VIEWFILES, _("View Client Files"));
		if (ClientsManager::GetInstance()->Get(GetSelectedClientId())->IsEnabled())
			clientContextMenu.Append(MID_ENDISABLE, _("Disable this client"));
		else
			clientContextMenu.Append(MID_ENDISABLE, _("Enable this client"));
	}

	PopupMenu(&clientContextMenu);
}


void ListViewClients::OnColumnRightClick(wxListEvent& event)
{
	wxMenu*   columnContextMenu = new wxMenu();
	//wxMenuItem* stateMenu;
	wxMenuItem* progressMenu;
	wxMenuItem* nameMenu;
	wxMenuItem* etaMenu;
	wxMenuItem* ppdMenu;
	wxMenuItem* coreMenu;
	wxMenuItem* prcgMenu;
	wxMenuItem* creditMenu;
	wxMenuItem* downloadedMenu;
	wxMenuItem* deadlineMenu;

	//stateMenu = new wxMenuItem(columnContextMenu, MID_STATE, _("State"), wxEmptyString, wxITEM_CHECK);
	progressMenu = new wxMenuItem(columnContextMenu, MID_PROGRESS, _("Progress"), wxEmptyString, wxITEM_CHECK);
	nameMenu = new wxMenuItem(columnContextMenu, MID_NAME, _("Name"), wxEmptyString, wxITEM_CHECK);
	etaMenu = new wxMenuItem(columnContextMenu, MID_ETA, _("ETA"), wxEmptyString, wxITEM_CHECK);
	ppdMenu = new wxMenuItem(columnContextMenu, MID_PPD, _("PPD"), wxEmptyString, wxITEM_CHECK);
	coreMenu = new wxMenuItem(columnContextMenu, MID_CORE, _("Core"), wxEmptyString, wxITEM_CHECK);
	prcgMenu = new wxMenuItem(columnContextMenu, MID_PRCG, _("PRCG"), wxEmptyString, wxITEM_CHECK);
	creditMenu = new wxMenuItem(columnContextMenu, MID_CREDIT, _("Credit"), wxEmptyString, wxITEM_CHECK);
	downloadedMenu = new wxMenuItem(columnContextMenu, MID_DOWNLOADED, _("Downloaded"), wxEmptyString, wxITEM_CHECK);
	deadlineMenu = new wxMenuItem(columnContextMenu, MID_DEADLINE, _("Deadline"), wxEmptyString, wxITEM_CHECK);

	//columnContextMenu->Append(stateMenu);
	//stateMenu->Check(mStateEnabled);
	columnContextMenu->Append(progressMenu);
	progressMenu->Check(mProgressEnabled);
	columnContextMenu->Append(nameMenu);
	nameMenu->Check(mNameEnabled);
	columnContextMenu->Append(etaMenu);
	etaMenu->Check(mETAEnabled);
	columnContextMenu->Append(ppdMenu);
	ppdMenu->Check(mPPDEnabled);
	columnContextMenu->Append(coreMenu);
	coreMenu->Check(mCoreEnabled);
	columnContextMenu->Append(prcgMenu);
	prcgMenu->Check(mPRCGEnabled);
	columnContextMenu->Append(creditMenu);
	creditMenu->Check(mCreditEnabled);
	columnContextMenu->Append(downloadedMenu);
	downloadedMenu->Check(mDownloadedEnabled);
	columnContextMenu->Append(deadlineMenu);
	deadlineMenu->Check(mDeadlineEnabled);

	PopupMenu(columnContextMenu);
}


void ListViewClients::OnDoubleClick(wxMouseEvent& event)
{
	wxInt32 hitItem;
	wxInt32 hitFlags;
	wxPoint clickPosition;

	// Test if an item was hit by the click or not
	clickPosition = event.GetPosition();
	hitItem       = HitTest(clickPosition, hitFlags);
	if(hitItem == wxNOT_FOUND)
	{
		// Nothing was hit, so we should do nothing
	}
	else
	{
		// An item was hit, so we have to select it
		Select(hitItem);

		// And we should show the client files
		ShowClientFiles();
	}
}


void ListViewClients::OnMenuReloadClient(wxCommandEvent& event)
{
	wxUint32 selectedClientId = GetSelectedClientId();

	// Ensure that something is really selected
	if(selectedClientId != INVALID_CLIENT_ID)
	{
		ClientsManager::GetInstance()->ReloadThreaded(selectedClientId);
	}
}


void ListViewClients::OnMenuAddClient(wxCommandEvent& event)
{
	// INVALID_CLIENT_ID means that we want to create a new client, not to edit an existing one
	ClientDialog::GetInstance(this)->ShowModal(INVALID_CLIENT_ID, wxEmptyString);
}


void ListViewClients::OnMenuEditClient(wxCommandEvent& event)
{
	wxUint32 selectedClientId = GetSelectedClientId();

	// Ensure that something is really selected
	if(selectedClientId == INVALID_CLIENT_ID)
	{
		return;
	}

	// Ask the main dialog to edit this client
	ClientDialog::GetInstance(this)->ShowModal(selectedClientId, wxEmptyString);
}


void ListViewClients::OnMenuDeleteClient(wxCommandEvent& event)
{
	wxUint32       selectedClientId;
	wxCommandEvent deleteEvent(EVT_CLIENTDELETED);

	// Ensure that something is really selected
	selectedClientId = GetSelectedClientId();
	if(selectedClientId == INVALID_CLIENT_ID)
	{
		return;
	}

	// Ensure that the user did not ask for deletion by error
	if(Tools::QuestionMsgBox(_("Do you really want to delete this client?")) == true)
	{
		// Delete the client
		ClientsManager::GetInstance()->Delete(selectedClientId);

		// And warn the main dialog
		MainDialog::GetInstance()->AddPendingEvent(deleteEvent);
	}
}


wxString ListViewClients::GetCellContentsString( long row_number, int column )
{
	wxListItem     row_info;
	wxString       cell_contents_string;

	// Set what row it is (m_itemId is a member of the regular wxListCtrl class)
	row_info.m_itemId = row_number;
	// Set what column of that row we want to query for information.
	row_info.m_col = column;
	// Set text mask
	row_info.m_mask = wxLIST_MASK_TEXT;

	// Get the info and store it in row_info variable.
	GetItem( row_info );

	// Extract the text out that cell
	cell_contents_string = row_info.m_text;

	return cell_contents_string;
}


void ListViewClients::OnMenuViewFiles(wxCommandEvent& event)
{
	ShowClientFiles();
}


void ListViewClients::ShowClientFiles()
{

	wxString  ClientLocation;
	wxString  FileManager;
	const Client  *client;

	client = ClientsManager::GetInstance()->Get(GetSelectedClientId());

	ClientLocation = client->GetLocation();

	_PrefsGetString(PREF_TOOLS_FILEMANAGER, FileManager);

	#ifdef _FAHMON_WIN32_
		// Converts / to \ in filepaths so Windows can use them correctly
		ClientLocation.Replace(_T("/"), _T("\\"), true);
	#endif

	// Not sure why, but this *never* fails
	if(wxExecute(FileManager + wxT(" \"") + ClientLocation + wxT("\"")) == false)
	{
		Tools::ErrorMsgBox(_("Unable to launch the default filemanager.\n\nPlease check that the correct filemanager is set in Preferences"));
	}
}


bool DropFiles::OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames)
{
	for (size_t n = 0; n < filenames.Count(); n++) {
		ClientDialog::GetInstance(MainDialog::GetInstance())->ShowModal(INVALID_CLIENT_ID, filenames[n]);
	}
	return TRUE;
}


void ListViewClients::OnColBeginDrag(wxListEvent& event)
{
	if ( event.GetColumn() == LVC_STATUS )
	{
		event.Veto();
	}
}

//when enabling columns, use default widths to prevent a size of 0 being used
void ListViewClients::OnMenuState(wxCommandEvent& event)
{
	wxUint32  width;

	mStateEnabled = !mStateEnabled;

	width = mStateEnabled ? 30 : 0;
	SetColumnWidth(LVC_STATUS, width);
}


void ListViewClients::OnMenuProgress(wxCommandEvent& event)
{
	wxUint32  width;

	mProgressEnabled = !mProgressEnabled;

	width = mProgressEnabled ? (wxUint32)PREF_LISTVIEWCLIENTS_PROGRESSCOLUMNWIDTH_DV : 0;
	SetColumnWidth(LVC_PROGRESS, width);
}


void ListViewClients::OnMenuName(wxCommandEvent& event)
{
	wxUint32  width;

	mNameEnabled = !mNameEnabled;

	width = mNameEnabled ? (wxUint32)PREF_LISTVIEWCLIENTS_NAMECOLUMNWIDTH_DV : 0;
	SetColumnWidth(LVC_NAME, width);
}


void ListViewClients::OnMenuETA(wxCommandEvent& event)
{
	wxUint32  width;

	mETAEnabled = !mETAEnabled;

	width = mETAEnabled ? (wxUint32)PREF_LISTVIEWCLIENTS_ETACOLUMNWIDTH_DV : 0;
	SetColumnWidth(LVC_ETA, width);
}


void ListViewClients::OnMenuPPD(wxCommandEvent& event)
{
	wxUint32  width;

	mPPDEnabled = !mPPDEnabled;

	width = mPPDEnabled ? (wxUint32)PREF_LISTVIEWCLIENTS_PPDCOLUMNWIDTH_DV : 0;
	SetColumnWidth(LVC_PPD, width);
}


void ListViewClients::OnMenuCore(wxCommandEvent& event)
{
	wxUint32  width;

	mCoreEnabled = !mCoreEnabled;

	width = mCoreEnabled ? (wxUint32)PREF_LISTVIEWCLIENTS_CORECOLUMNWIDTH_DV : 0;
	SetColumnWidth(LVC_CORE, width);
}


void ListViewClients::OnMenuPRCG(wxCommandEvent& event)
{
	wxUint32  width;

	mPRCGEnabled = !mPRCGEnabled;

	width = mPRCGEnabled ? (wxUint32)PREF_LISTVIEWCLIENTS_PRCGCOLUMNWIDTH_DV : 0;
	SetColumnWidth(LVC_PRCG, width);
}


void ListViewClients::OnMenuCredit(wxCommandEvent& event)
{
	wxUint32  width;

	mCreditEnabled = !mCreditEnabled;

	width = mCreditEnabled ? (wxUint32)PREF_LISTVIEWCLIENTS_CREDITCOLUMNWIDTH_DV : 0;
	SetColumnWidth(LVC_CREDIT, width);
}


void ListViewClients::OnMenuDownloaded(wxCommandEvent& event)
{
	wxUint32  width;

	mDownloadedEnabled = !mDownloadedEnabled;

	width = mDownloadedEnabled ? (wxUint32)PREF_LISTVIEWCLIENTS_DOWNLOADEDCOLUMNWIDTH_DV : 0;
	SetColumnWidth(LVC_DOWNLOADED, width);
}


void ListViewClients::OnMenuDeadline(wxCommandEvent& event)
{
	wxUint32  width;

	mDeadlineEnabled = !mDeadlineEnabled;

	width = mDeadlineEnabled ? (wxUint32)PREF_LISTVIEWCLIENTS_DEADLINECOLUMNWIDTH_DV : 0;
	SetColumnWidth(LVC_DEADLINE, width);
}


void ListViewClients::OnMenuEnDisable(wxCommandEvent& event)
{
	ClientsManager::GetInstance()->Enable(GetSelectedClientId(), !ClientsManager::GetInstance()->Get(GetSelectedClientId())->IsEnabled());
	UpdateClient(GetSelectedClientId());
}
