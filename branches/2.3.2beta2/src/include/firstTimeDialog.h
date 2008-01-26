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

#ifndef _FIRSTTIMEDIALOG_H
#define _FIRSTTIMEDIALOG_H


#include "wx/dialog.h"



/**
* This is a small help dialog for the first launch
* It's a singleton
**/
class FirstTimeDialog : public wxDialog
{
protected:
	static FirstTimeDialog *mInstance;

	FirstTimeDialog(void);
	~FirstTimeDialog(void);

	// --- Events
	void OnClose(wxCloseEvent& event);
	void OnUpdateButton(wxCommandEvent& event);


public:
	// Singleton pattern
	static FirstTimeDialog* GetInstance(void);
	static void DestroyInstance(void);

	int ShowModal(void);


private:
	DECLARE_EVENT_TABLE()
};

#endif /* _FIRSTTIMEDIALOG_H */
