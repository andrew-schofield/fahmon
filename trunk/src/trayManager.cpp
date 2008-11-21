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

/**
 * \file trayManager.cpp
 * Manages system tray icon.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "trayManager.h"

#include "mainDialog.h"
#include "pathManager.h"
#include "listViewClients.h"
#include "benchmarksDialog.h"
#include "preferencesDialog.h"

#include "wx/icon.h"
#include "wx/intl.h"
#include "wx/menu.h"

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


TrayManager::TrayManager(void)
{
	mTooltip = wxT(FMC_PRODUCT);
}


TrayManager::~TrayManager(void)
{
	UninstallIcon();
}


TrayManager* TrayManager::GetInstance(void)
{
	if(mInstance == NULL)
	{
		mInstance = new TrayManager();
	}

	return mInstance;
}


void TrayManager::DestroyInstance(void)
{
	TrayManager::GetInstance()->UninstallIcon();
	if(mInstance != NULL)
	{
		delete mInstance;
		mInstance = NULL;
	}
}


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


void TrayManager::UninstallIcon(void)
{
	if(IsIconInstalled() == true)
	{
		RemoveIcon();
	}
}


void TrayManager::SetTooltip(const wxString& tooltip)
{
	double  TotalPPD;
	wxInt32  ClientCount;

	ClientCount = MainDialog::GetInstance()->GetClientCount();

	TotalPPD = MainDialog::GetInstance()->GetTotalPPD();

	mTooltip = wxString::Format(_("%s\nClients: %i\nTotal PPD: %.2f"), wxT(FMC_PRODUCT), ClientCount, TotalPPD);

	if(IsIconInstalled() == true)
	{
	#ifdef __WXGTK__
		SetIcon(wxIcon(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_DIALOG)), mTooltip);
	#elif _FAHMON_WIN32_
		SetIcon(wxICON(dialog_icon), mTooltip);
	#endif
	}
}


void TrayManager::OnClick(wxTaskBarIconEvent& event)
{
	if(MainDialog::GetInstance()->IsShown())
	{
		if(MainDialog::GetInstance()->IsMaximized())
			SetMaximised(true);
		else
			SetMaximised(false);
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
		if(GetMaximised())
			MainDialog::GetInstance()->Maximize(true);
#endif
	}
}


wxMenu* TrayManager::CreatePopupMenu()
{
	wxMenu *traymenu = new wxMenu();

	traymenu->Append(TRAY_MENU_BENCHMARKS, _("Benchmarks"));

	traymenu->Append(TRAY_MENU_PREFERENCES, _("Preferences"));

	// Separator
	traymenu->AppendSeparator();

	if (MainDialog::GetInstance()->IsShown())
	{
		//hide item
		wxMenuItem *hideMenu = new wxMenuItem(traymenu,TRAY_MENU_HIDE, _("Hide FahMon"));
		traymenu->Append(hideMenu);
	}
	else
	{
		//show item
		wxMenuItem *showMenu = new wxMenuItem(traymenu,TRAY_MENU_SHOW, _("Show FahMon"));
		traymenu->Append(showMenu);
	}

	// Separator
	traymenu->AppendSeparator();

	// Exit item
	traymenu->Append(TRAY_MENU_EXIT, _("Quit"));

	return traymenu;

}


void TrayManager::Close(wxCommandEvent&)
{
	MainDialog::GetInstance()->Close();
}


void TrayManager::Show(wxCommandEvent&)
{
	MainDialog::GetInstance()->Show();
	MainDialog::GetInstance()->Raise();

	MainDialog::GetInstance()->TrayReloadSelectedClient();

	// Fortunately, this function seems to keep track of the previous maximize state of the window;
	// otherwise, we would have to do that
	#ifdef _FAHMON_WIN32_
	if (GetMaximised())
		MainDialog::GetInstance()->Maximize(true);
	else
		MainDialog::GetInstance()->Maximize(false);
	#endif
}


void TrayManager::Hide(wxCommandEvent&)
{
	if(MainDialog::GetInstance()->IsMaximized())
		SetMaximised(true);
	else
		SetMaximised(false);
	// Must minimize to the task bar first so that we get the minimize animation :)
	MainDialog::GetInstance()->Iconize();
	MainDialog::GetInstance()->Hide();
}


void TrayManager::Benchmarks(wxCommandEvent&)
{
	BenchmarksDialog::GetInstance(MainDialog::GetInstance())->ShowModal(0);
}


void TrayManager::Preferences(wxCommandEvent&)
{
	PreferencesDialog::GetInstance(MainDialog::GetInstance())->ShowModal();
}
