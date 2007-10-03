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
 
#include "fahmon.h"
#include "tools.h"

#include "wx/wfstream.h"
#include "preferencesManager.h"


/**
 * Try to open the default browser on the given URL
 * Display an error message within a dialog box if something goes wrong
 *
 * The function provided by wxWidgets seems buggy under Windows:
 * When IE is the default browser, it opens two windows, one blank and the other on the given url
 * This is why this function is not used here, instead the method used in previous versions of FahMon is used (ShellExecuteEx())
 *
 * As it is very easy on Linux, it is neither used for this platform
**/
void Tools::OpenURLInBrowser(const wxString& url)
{
#ifdef _FAHMON_WIN32_

    SHELLEXECUTEINFO shellInfo;

    ZeroMemory(&shellInfo, sizeof(shellInfo));
    shellInfo.cbSize = sizeof(shellInfo);
    shellInfo.fMask  = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
    shellInfo.lpFile = url.c_str();

    // CloseHandle() must be called to avoid handles leaks
    if(ShellExecuteEx(&shellInfo))
        CloseHandle(shellInfo.hProcess);

#elif _FAHMON_LINUX_

    wxString browser;

    // Use first the preference
    // If the latter is not set, use the environment variable 'BROWSER'
    _PrefsGetString(PREF_TOOLS_BROWSER, browser);
    if(browser.IsEmpty())
        browser = wxGetenv(wxT("BROWSER"));

    if(browser.IsEmpty() || wxExecute(browser + wxT(" ") + url) == false)
        ErrorMsgBox(_("Unable to launch the default browser.\n\nPlease check that the environment variable BROWSER is defined or set it in Preferences."));

#else

#error "You\'re talking to me?"

#endif
}


/**
 * Load the whole content of a file into a wxString, return false if something went wrong
 * A NULL character is appended to terminate the wxString
**/
bool Tools::LoadFile(const wxString& filename, wxString& fileContent)
{
    wxByte            *stringBuffer;
    wxUint32           fileSize;
    wxFileInputStream  in(filename);

    // Could the file be opened?
    if(in.Ok() == false)
        return false;

    // Load the file into the wxString, and put a NULL character at the end to terminate it
    fileSize     = in.GetSize();
    stringBuffer = new wxByte[fileSize+1];

    in.Read(stringBuffer, fileSize);
    stringBuffer[fileSize] = '\0';
    fileContent = wxString::FromAscii((const char*)stringBuffer);
    delete[] stringBuffer;

    // Could we read the whole content of the file?
    if(in.LastRead() != fileSize)
        return false;

    return true;
}


/**
 * Return a string with a formated time
**/
wxString Tools::FormatSeconds(wxUint32 nbSeconds)
{
    wxUint32 nbHours;
    wxUint32 nbMinutes;

    nbHours   = nbSeconds / 3600;        // There are 3600 seconds in an hour
    nbSeconds = nbSeconds % 3600;
    nbMinutes = nbSeconds / 60;          // There are 60 seconds in a minute
    nbSeconds = nbSeconds % 60;

    if(nbHours != 0)
        return wxString::Format(wxT("%uh %02umn %02us"), nbHours, nbMinutes, nbSeconds);
    else if(nbMinutes != 0)
        return wxString::Format(wxT("%umn %02us"), nbMinutes, nbSeconds);
    else
        return wxString::Format(wxT("%us"), nbSeconds);
}
