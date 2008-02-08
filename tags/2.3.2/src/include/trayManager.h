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
 * \file trayManager.h
 * Manages system tray icon.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _TRAYMANAGER_H
#define _TRAYMANAGER_H

#include "wx/taskbar.h"
class wxMenu;

/**
 * Tray manager class.
 * This class is used to manage events from/to the system tray.
**/
class TrayManager : public wxTaskBarIcon
{
protected:
	wxString            mTooltip; /**< Tooltip string */
	static TrayManager* mInstance; /**< The single instance of the TrayManager */

	/**
	 * Contructor.
	 * Contructs a tray manager object.
	 **/
	TrayManager(void);

	/**
	 * Desctructor.
	 **/
	~TrayManager(void);

	/**
	 * Event: Manages clicking on the tray icon.
	 * Shows/Hides the main FahMon window.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnClick(wxTaskBarIconEvent& event);

	/**
	 * Event: Manages the 'quit' menu item.
	 * Closes FahMon.
	 **/
	void Close(wxCommandEvent&);

	/**
	 * Event: Manages the 'show' menu item.
	 * Opens the FahMon window.
	 **/
	void Show(wxCommandEvent&);

	/**
	 * Event: Manages the 'hide' menu item.
	 * Hides the  FahMon window.
	 **/
	void Hide(wxCommandEvent&);

	/**
	 * Event: Manages the 'benchmarks' menu item.
	 * Opens the benchmark window.
	 **/
	void Benchmarks(wxCommandEvent&);

	/**
	 * Event: Manages the 'preferences' menu item.
	 * Opens the preferences window.
	 **/
	void Preferences(wxCommandEvent&);


public:
	// Singleton pattern
	/**
	 * Destroy the single instance of the TrayManager.
	 * Only if it exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Retrieve the instance of TrayManager.
	 * If an instance doesn't already exist, create it.
	 * @return An instance of class TrayManager.
	 **/
	static TrayManager* GetInstance(void);

	// Setters
	/**
	 * Change the text of the tooltip associated with the icon.
	 * If the icon is installed in the systray, the tooltip is immediately updated.
	 **/
	void SetTooltip(const wxString& tooltip);

	/**
	 * Install the icon in the systray if it is not already there
	 **/
	void InstallIcon(void);

	/**
	 * Uninstall the icon if it is present.
	 **/
	void UninstallIcon(void);


private:

	virtual wxMenu* CreatePopupMenu(); /**< The right-click menu for the tray icon */
	DECLARE_EVENT_TABLE()
};


#endif /* _TRAYMANAGER_H */
