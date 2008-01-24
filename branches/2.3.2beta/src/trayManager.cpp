/*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "fahmon.h"
#include "trayManager.h"

#include "wx/icon.h"
#include "wx/intl.h"
#include "mainDialog.h"
#include "pathManager.h"
#include "wx/menu.h"
#include "listViewClients.h"
#include "benchmarksDialog.h"
#include "preferencesDialog.h"


// The single instance accross the application
TrayManager* TrayManager::mInstance = NULL;

enum {
	TRAY_MENU_EXIT,
	TRAY_MENU_SHOW,
	TRAY_MENU_HIDE,
	TRAY_MENU_BENCHMARKS,
	TRAY_MENU_PREFERENCES,
	TRAY_MENU_NULL
};


// Events managed by this class
BEGIN_EVENT_TABLE(TrayManager, wxTaskBarIcon)
	EVT_TASKBAR_LEFT_DOWN   (TrayManager::OnClick)
	EVT_MENU( TRAY_MENU_EXIT, TrayManager::Close)
	EVT_MENU( TRAY_MENU_SHOW, TrayManager::Show)
	EVT_MENU( TRAY_MENU_HIDE, TrayManager::Hide)
	EVT_MENU( TRAY_MENU_BENCHMARKS, TrayManager::Benchmarks)
	EVT_MENU( TRAY_MENU_PREFERENCES, TrayManager::Preferences)
END_EVENT_TABLE()


/**
* Constructor
**/
TrayManager::TrayManager(void)
{
	mTooltip = wxT(FMC_PRODUCT);
}


/**
* Destructor
**/
TrayManager::~TrayManager(void)
{
	UninstallIcon();
}


/**
* Retrieve the instance of TrayManager, create it if needed
**/
TrayManager* TrayManager::GetInstance(void)
{
	if(mInstance == NULL)
	{
		mInstance = new TrayManager();
	}

	return mInstance;
}


/**
* Destroy the instance of TrayManager, if any
**/
void TrayManager::DestroyInstance(void)
{
	if(mInstance != NULL)
	{
		delete mInstance;
		mInstance = NULL;
	}
}


/**
* Install the icon in the systray if it is not already there
**/
void TrayManager::InstallIcon(void)
{
	if(IsIconInstalled() == false)
	{
	#ifdef __WXGTK__
		SetIcon(wxIcon(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_DIALOG)), mTooltip);
	#elif _FAHMON_WIN32_
		SetIcon(wxICON(dialog_icon), mTooltip);
	#endif
	}
}


/**
* Uninstall the icon if it is present
**/
void TrayManager::UninstallIcon(void)
{
	if(IsIconInstalled() == true)
	{
		RemoveIcon();
	}
}


/**
* Change the text of the tooltip associated with the icon
* If the icon is installed in the systray, the tooltip is immediately updated
**/
void TrayManager::SetTooltip(const wxString& tooltip)
{
	double  TotalPPD;
	wxInt32  ClientCount;

	ClientCount = MainDialog::GetInstance()->GetClientCount();

	TotalPPD = MainDialog::GetInstance()->GetTotalPPD();

	mTooltip = wxString::Format(_T("%s / %s\n%i clients for %.2fPPD"), wxT(FMC_PRODUCT), tooltip.c_str(), ClientCount, TotalPPD);

	if(IsIconInstalled() == true)
	{
	#ifdef __WXGTK__
		SetIcon(wxIcon(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_DIALOG)), mTooltip);
	#elif _FAHMON_WIN32_
		SetIcon(wxICON(dialog_icon), mTooltip);
	#endif
	}
}


/**
* The user has clicked on the systray icon
**/
void TrayManager::OnClick(wxTaskBarIconEvent& event)
{
	if(MainDialog::GetInstance()->IsShown())
	{
		// Must minimize to the task bar first so that we get the minimize animation :)
		MainDialog::GetInstance()->Iconize();
		MainDialog::GetInstance()->Hide();
	}
	else
	{
		MainDialog::GetInstance()->Show();
		MainDialog::GetInstance()->Raise();

		MainDialog::GetInstance()->TrayReloadSelectedClient();

		// Fortunately, this function seems to keep track of the previous maximize state of the window;
		// otherwise, we would have to do that
		#ifdef _FAHMON_WIN32_
		MainDialog::GetInstance()->Maximize(false);
		#endif
	}
}

/**
* Creates a right-click popup menu from the tray icon
**/
wxMenu* TrayManager::CreatePopupMenu()
{
	double  TotalPPD;
	wxInt32  ClientCount;

	wxMenu *traymenu = new wxMenu();

	// fahmon Version
	traymenu->Append(TRAY_MENU_NULL, wxString::Format(wxT("%s"), wxT(FMC_PRODUCT)));

	// Separator
	traymenu->AppendSeparator();

	ClientCount = MainDialog::GetInstance()->GetClientCount();

	traymenu->Append(TRAY_MENU_NULL, wxString::Format(_("Clients: %i"), ClientCount));

	TotalPPD = MainDialog::GetInstance()->GetTotalPPD();

	traymenu->Append(TRAY_MENU_NULL, wxString::Format(_("Total PPD: %.2f"), TotalPPD));

	// Separator
	traymenu->AppendSeparator();

	traymenu->Append(TRAY_MENU_BENCHMARKS, _("Benchmarks"));

	traymenu->Append(TRAY_MENU_PREFERENCES, _("Preferences"));

	// Separator
	traymenu->AppendSeparator();

	if (MainDialog::GetInstance()->IsShown()) {
		//hide item
		traymenu->Append(TRAY_MENU_HIDE, _("Hide FahMon"));
	}
	else
	{
		//show item
		traymenu->Append(TRAY_MENU_SHOW, _("Show FahMon"));
	}

	// Separator
	traymenu->AppendSeparator();

	// Exit item
	traymenu->Append(TRAY_MENU_EXIT, _("Quit"));

	return traymenu;

}

/**
* The user has clicked close on the popup menu
**/
void TrayManager::Close(wxCommandEvent&)
{
	MainDialog::GetInstance()->Close();
}

/**
* The user has clicked Show on the popup menu
**/
void TrayManager::Show(wxCommandEvent&)
{
	MainDialog::GetInstance()->Show();
	MainDialog::GetInstance()->Raise();

	MainDialog::GetInstance()->TrayReloadSelectedClient();

	// Fortunately, this function seems to keep track of the previous maximize state of the window;
	// otherwise, we would have to do that
	#ifdef _FAHMON_WIN32_
	MainDialog::GetInstance()->Maximize(false);
	#endif
}

/**
* The user has clicked Hide on the popup menu
**/
void TrayManager::Hide(wxCommandEvent&)
{
	// Must minimize to the task bar first so that we get the minimize animation :)
	MainDialog::GetInstance()->Iconize();
	MainDialog::GetInstance()->Hide();
}

/**
* The user has clicked Benchmarks on the popup menu
**/
void TrayManager::Benchmarks(wxCommandEvent&)
{
	BenchmarksDialog::GetInstance(MainDialog::GetInstance())->ShowModal(0);
}


/**
* The user has clicked Preferences on the popup menu
**/
void TrayManager::Preferences(wxCommandEvent&)
{
	PreferencesDialog::GetInstance(MainDialog::GetInstance())->ShowModal();
}
