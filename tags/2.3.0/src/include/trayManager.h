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

#ifndef _TRAYMANAGER_H
#define _TRAYMANAGER_H

#include "wx/taskbar.h"
class wxMenu;

/**
 * This class is used to manage events from/to the system tray
**/
class TrayManager : public wxTaskBarIcon
{
protected:
    wxString            mTooltip;
    static TrayManager* mInstance;

     TrayManager(void);
    ~TrayManager(void);

    void OnClick(wxTaskBarIconEvent& event);
    void Close(wxCommandEvent&);
    void Show(wxCommandEvent&);
    void Hide(wxCommandEvent&);
    void Benchmarks(wxCommandEvent&);
    void Preferences(wxCommandEvent&);


public:
    // Singleton pattern
    static void DestroyInstance(void);
    static TrayManager* GetInstance(void);

    // Setters
    void SetTooltip(const wxString& tooltip);

    void InstallIcon(void);
    void UninstallIcon(void);


private:

    virtual wxMenu* CreatePopupMenu();
    DECLARE_EVENT_TABLE()
};


#endif /* _TRAYMANAGER_H */
