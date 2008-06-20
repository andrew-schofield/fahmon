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
 * \file tools.h
 * A collection of useful tools.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _TOOLS_H
#define _TOOLS_H


#include "client.h"
#include "wx/msgdlg.h"
#include "fahmonConsts.h"

#define PREF_TOOLS_BROWSER    wxT("Tools.Browser") /**< Preference setting for web browser */
#define PREF_TOOLS_BROWSER_DV wxT("") /**< Default value for web browser */


/**
 * A class of useful tools.
 * A set of static methods.
 **/
class Tools
{
protected:
	//static const wxChar* mBase64Table;

	/**
	 * Wrapper around wxMessageBox.
	 * This function eliminates the need to specify the dialog title.
	 * @param msg Text to display in the messagebox.
	 * @param style Dialog style.
	 * @return A wxMessageDialog.
	 **/
	static int MsgBox(const wxString& msg, long style) {return wxMessageBox(msg, wxT(FMC_PRODUCT), style);}


public:
	/**
	 * Try to open the default browser at the given URL.
	 * Display an error message within a dialog box if something goes wrong
	 *
	 * The function provided by wxWidgets seems buggy under Windows:
	 * When IE is the default browser, it opens two windows, one blank and the other on the given url
	 * This is why this function is not used here, instead the method used in previous versions of FahMon is used (ShellExecuteEx())
	 *
	 * As it is very easy on Linux, it is not used for this platform.
	 * @param url The url to open
	 **/
	static void     OpenURLInBrowser(const wxString& url);

	/**
	 * Load the entire contents of a file into a wxString.
	 * Return false if something went wrong.
	 * A NULL character is appended to terminate the wxString
	 * @param filename The file to open.
	 * @param fileContent Reference to an objec to store the file contents.
	 **/
	static bool     LoadFile(const wxString& filename, wxString& fileContent);

	/**
	 * Format a given time.
	 * @param nbSeconds A time span in number of seconds.
	 * @return The formatted string.
	 **/
	static wxString FormatSeconds(wxUint32 nbSeconds);

	// Miscellaneous message boxes
	/**
	 * Create an error dialog box.
	 * This is a wrapper around MsgBox.
	 * @param error The error message to display.
	 **/
	static void ErrorMsgBox(const wxString& error) {MsgBox(error, wxICON_ERROR | wxOK);}

	/**
	 * Create an question dialog box.
	 * This is a wrapper around MsgBox.
	 * @param question The question to ask.
	 **/
	static bool QuestionMsgBox(const wxString& question) {return MsgBox(question, wxICON_QUESTION | wxYES_NO) == wxYES;}

	/**
	 * Create an information dialog box.
	 * This is a wrapper around MsgBox.
	 * @param information The information to display.
	 **/
	static void InfoMsgBox(const wxString& information) {MsgBox(information, wxICON_INFORMATION | wxOK);}
};


#endif /* _TOOLS_H */
