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

#ifndef _ABOUTDIALOG_H
#define _ABOUTDIALOG_H

#include "wx/dialog.h"


/**
* This is the famous about dialog box
* It's a singleton
**/
class AboutDialog : public wxDialog
{
protected:
	static AboutDialog *mInstance;

	AboutDialog(wxWindow* parent);
	~AboutDialog(void);


public:
	// Singleton pattern
	static AboutDialog* GetInstance(wxWindow* parent);
	static void DestroyInstance(void);

	int ShowModal(void);
};


#endif /* _ABOUTDIALOG_H */
