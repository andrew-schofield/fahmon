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
#include "mainDialog.h"
#include "pathManager.h"


// The single instance accross the application
TrayManager* TrayManager::mInstance = NULL;


// Events managed by this class
BEGIN_EVENT_TABLE(TrayManager, wxTaskBarIcon)
    EVT_TASKBAR_LEFT_DOWN   (TrayManager::OnClick)
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
        mInstance = new TrayManager();

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
#ifdef _FAHMON_LINUX_
        SetIcon(wxIcon(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_DIALOG)), mTooltip);
#else
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
        RemoveIcon();
}


/**
 * Change the text of the tooltip associated with the icon
 * If the icon is installed in the systray, the tooltip is immediately updated
**/
void TrayManager::SetTooltip(const wxString& tooltip)
{
    mTooltip = wxString::Format(wxT("%s / %s"), wxT(FMC_PRODUCT), tooltip.c_str());
    
    if(IsIconInstalled() == true)
    {
#ifdef _FAHMON_LINUX_
        SetIcon(wxIcon(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_DIALOG)), mTooltip);
#else
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

        // Fortunately, this function seems to keep track of the previous maximize state of the window;
        // otherwise, we would have to do that
        MainDialog::GetInstance()->Maximize(false);
    }
}
