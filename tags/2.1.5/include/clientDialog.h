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
 
#ifndef _CLIENTDIALOG_H
#define _CLIENTDIALOG_H


#include "wx/dialog.h"
#include "wx/textctrl.h"


/**
 * This dialog box allows the user to create/edit clients
 * It's a singleton
**/
class ClientDialog : public wxDialog
{
protected:
           wxUint32      mClientId;
           wxTextCtrl   *mClientNameCtrl;
           wxTextCtrl   *mClientLocationCtrl;
    static ClientDialog *mInstance;

     ClientDialog(wxWindow* parent);
    ~ClientDialog(void);

    void OnBrowseButton(wxCommandEvent& event);
    void OnOkButton(wxCommandEvent& event);


public:
	// Singleton pattern
	static ClientDialog* GetInstance(wxWindow* parent);
	static void DestroyInstance(void);

    int ShowModal(wxUint32 clientId);


private:
	DECLARE_EVENT_TABLE()
};


#endif /* _CLIENTDIALOG_H */
