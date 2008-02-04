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
 * \file firstTimeDialog.h
 * The First time dialog.
 * Creates the dialog on first run to initially download projects etc.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _FIRSTTIMEDIALOG_H
#define _FIRSTTIMEDIALOG_H


#include "wx/dialog.h"



/**
 * First Time Dialog Class.
 * This is a small help dialog for the first launch.
 * This class can only be instanciated once.
 **/
class FirstTimeDialog : public wxDialog
{
protected:
	static FirstTimeDialog *mInstance; /**< The single instance of the FirstTime Dialog */

	/**
	 * Contructor.
	 * Contructs a FrstTime Dialog of type wxDialog with a title of "Welcome to FahMon"
	 **/
	FirstTimeDialog(void);

	/**
	 * Destructor.
	 **/
	~FirstTimeDialog(void);

	// --- Events
	/**
	 * Event: Window was closed.
	 * Change the behaviour of the default close handler, so that it simulates a push on the OK button.
	 * Indeed, we don't want to return a wxID_CANCEL at this point.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnClose(wxCloseEvent& event);

	/**
	 * Event: The 'update projects' button was pushed.
	 * Instructs FahMon to update the project database.
	 * @param event The event itself. This is sent automatically.
	 **/
	void OnUpdateButton(wxCommandEvent& event);


public:
	// Singleton pattern
	/**
	 * Retrieve the instance of FirstTimeDialog.
	 * If an instance doesn't already exist, create it.
	 * @return An instance of class FirstTimeDialog.
	 **/
	static FirstTimeDialog* GetInstance(void);

	/**
	 * Destroy the instance of FirstTimeDialog.
	 * Only if an instance exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Show the First Time Dialog.
	 * Opens a modal window and centres it.
	 * Also creates the configuration directory if it does not exist.
	 * Return wxID_CANCEL if the directory could not be created
	 **/
	int ShowModal(void);


private:
	DECLARE_EVENT_TABLE()
};

#endif /* _FIRSTTIMEDIALOG_H */
