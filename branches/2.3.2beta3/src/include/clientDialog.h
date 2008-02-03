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
 * \file clientDialog
 * The client edition dialog.
 * Creates the dialog to add and edit new clients.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _CLIENTDIALOG_H
#define _CLIENTDIALOG_H


#include "wx/dialog.h"
#include "wx/textctrl.h"


/**
 * Client edition class.
 * This class can only be instantiated once.
**/
class ClientDialog : public wxDialog
{
protected:
	wxUint32             mClientId; /**< The new/current client ID */
	wxTextCtrl          *mClientNameCtrl;  /**< Name of client */
	wxTextCtrl          *mClientLocationCtrl; /**< Location of client */
	static ClientDialog *mInstance; /**< The single instance of the Client Dialog */

	/**
	 * Contructor.
	 * Constructs a Client edition Dialog of type wxDialog with a title of "Client Edition"
	 * @param parent The parent window of type wxWindow
	 **/
	ClientDialog(wxWindow* parent);

	/**
	 * Destructor.
	 **/
	~ClientDialog(void);

	/**
	 * Event: The 'browse' button has been pushed.
	 * Manages the browse button: It displays the standard 'browse for directory...' dialog and fills the corresponding text field.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnBrowseButton(wxCommandEvent& event);

	/**
	 * Event: The 'Ok' button has been pushed.
	 * Manages the Ok button: The client can be validated only if the two textctrl have been filled.
	 * @param event Thr event itself. This is sent automatically.
	 **/
	void OnOkButton(wxCommandEvent& event);


public:
	// Singleton pattern
	/**
	 * Retrieve the instance of ClientDialog.
	 * If an instance doesn't already exist, create it.
	 * @param parent The parent windows of type wxWindow.
	 * @return An instance of class ClientDialog.
	 **/
	static ClientDialog* GetInstance(wxWindow* parent);

	/**
	 * Destroy the instance of ClientDialog.
	 * Only if an instance exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Show the Client Dialog.
	 * Opens a modal window and centres it.
	 * If clientId is equal to INVALID_CLIENT_ID, then a new client will be created.
	 * @param clientId The clientId to select.
	 * @param filename The location of the client to add.
	 **/
	int ShowModal(wxUint32 clientId, wxString filename);


private:
	DECLARE_EVENT_TABLE()
};


#endif /* _CLIENTDIALOG_H */
