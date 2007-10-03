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
 
#ifndef _PREFERENCESDIALOG_H
#define _PREFERENCESDIALOG_H


#include "wx/panel.h"
#include "wx/dialog.h"
#include "wx/choice.h"
#include "wx/notebook.h"
#include "wx/textctrl.h"
#include "wx/checkbox.h"
#include "wx/spinctrl.h"
#include "wx/stattext.h"


/**
 * This dialog box allows the user to change 'public' preferences
 * It's a singleton
**/
class PreferencesDialog : public wxDialog
{
protected:
    static PreferencesDialog *mInstance;

    // Widgets for the general tab
    wxCheckBox *mGeneralEnableTrayIcon;
    wxCheckBox *mGeneralAutoUpdateProjectsDatabase;
    wxCheckBox *mGeneralCollectXYZFiles;

    // Widgets for the monitoring tab
    wxChoice   *mMonitoringETADisplayStyle;
    wxCheckBox *mMonitoringAutoReload;
    wxSpinCtrl *mMonitoringAutoReloadFrequency;

    // Widgets for the networking tab
    wxCheckBox   *mNetworkingUseProxy;
    wxTextCtrl   *mNetworkingProxyAddress;
    wxTextCtrl   *mNetworkingProxyPort;
    wxCheckBox   *mNetworkingUseProxyAuthentication;
    wxTextCtrl   *mNetworkingProxyUsername;
    wxTextCtrl   *mNetworkingProxyPassword;
    wxStaticText *mNetworkingLabelAddress;
    wxStaticText *mNetworkingLabelPort;
    wxStaticText *mNetworkingLabelUsername;
    wxStaticText *mNetworkingLabelPassword;

    // Storage for the initial value of some important preferences
    // This is needed to detect when one of them has changed, so that the concerned component can be warned
    bool     mInitAutoReload;
    bool     mInitEnableTrayIcon;
    wxUint32 mInitAutoReloadFrequency;
    wxUint32 mInitETADisplayStyle;
    

     PreferencesDialog(wxWindow* parent);
    ~PreferencesDialog(void);

    wxPanel* CreateGeneralTab(wxNotebook* parent);
    wxPanel* CreateMonitoringTab(wxNotebook* parent);
    wxPanel* CreateNetworkingTab(wxNotebook* parent);
    
    void LoadPreferences(void);
    void SavePreferences(void);
    
    void OnOkButton(wxCommandEvent& event);
    void OnCheckboxes(wxCommandEvent& event);


public:
	// Singleton pattern
	static PreferencesDialog* GetInstance(wxWindow* parent);
	static void DestroyInstance(void);

    int ShowModal(void);


private:
	DECLARE_EVENT_TABLE()

};


#endif /* _PREFERENCESDIALOG_H */
