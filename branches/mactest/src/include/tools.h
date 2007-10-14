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

#ifndef _TOOLS_H
#define _TOOLS_H


#include "client.h"
#include "wx/msgdlg.h"
#include "fahmonConsts.h"

#define PREF_TOOLS_BROWSER    wxT("Tools.Browser")
#define PREF_TOOLS_BROWSER_DV wxT("")

class Tools
{
protected:
	static const wxChar* mBase64Table;

	static int MsgBox(const wxString& msg, long style) {return wxMessageBox(msg, wxT(FMC_PRODUCT), style);}


public:
	static void     OpenURLInBrowser(const wxString& url);
	static bool     LoadFile(const wxString& filename, wxString& fileContent);
	static wxString FormatSeconds(wxUint32 nbSeconds);

	// Miscellaneous message boxes
	static void ErrorMsgBox(const wxString& error) {MsgBox(error, wxICON_ERROR | wxOK);}
	static bool QuestionMsgBox(const wxString& question) {return MsgBox(question, wxICON_QUESTION | wxYES_NO) == wxYES;}
};


#endif /* _TOOLS_H */
