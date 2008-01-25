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
 * \file aboutDialog
 * About Dialog.
 * Creates a recognised about dialog for FahMon.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _ABOUTDIALOG_H
#define _ABOUTDIALOG_H

#include "wx/dialog.h"


/**
 * About Dialog class.
 * This class can only be instanciated once.
 **/
class AboutDialog : public wxDialog
{
protected:
	static AboutDialog *mInstance; /**< The single instance of the About Dialog */

	/**
	 * Constructor.
	 * Contructs an About Dialog of type wxDialog with a title of "About".
	 * @param parent The parent window of type wxWindow
	 **/
	AboutDialog(wxWindow* parent);

	/**
	 * Destructor.
	 **/
	~AboutDialog(void);


public:
	// Singleton pattern
	/**
	 * Retrieve the instance of AboutDialog.
	 * If an instance doesn't already exist, create it.
	 * @param parent The parent window of type wxWindow.
	 * @return An instance of class AboutDialog.
	 **/
	static AboutDialog* GetInstance(wxWindow* parent);

	/**
	 * Destroy the instance of AboutDialog.
	 * Only if an instance exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Show the About Dialog.
	 * Opens a modal window, and centres it.
	 **/
	int ShowModal(void);
};


#endif /* _ABOUTDIALOG_H */
