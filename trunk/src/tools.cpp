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
 * \file tools.cpp
 * A collection of useful tools.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "tools.h"

#include "preferencesManager.h"

#include "wx/textfile.h"

// This mutex is there to ensure that two threads won't try to open the same file at one time
wxMutex Tools::mMutexLoadFile;

void Tools::OpenURLInBrowser(wxString const &url)
{
#ifdef _FAHMON_WIN32_

	SHELLEXECUTEINFO shellInfo;

	ZeroMemory(&shellInfo, sizeof(shellInfo));
	shellInfo.cbSize = sizeof(shellInfo);
	shellInfo.fMask  = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	shellInfo.lpFile = url.c_str();

	// CloseHandle() must be called to avoid handles leaks
	if(ShellExecuteEx(&shellInfo))
	{
		CloseHandle(shellInfo.hProcess);
	}

#elif __WXGTK__

	wxString browser;

	// Use first the preference
	// If the latter is not set, use the environment variable 'BROWSER'
	_PrefsGetString(PREF_TOOLS_BROWSER, browser);
	if(browser.IsEmpty())
	{
		browser = wxGetenv(_T("BROWSER"));
	}

	if(browser.IsEmpty() || wxExecute(browser + _T(" ") + url) == false)
	{
		ErrorMsgBox(_("Unable to launch the default browser.\n\nPlease check that the environment variable BROWSER is defined or set it in Preferences."));
	}

#elif __WXMAC__

	wxLaunchDefaultBrowser(url);

#else

#error "You\'re talking to me?"

#endif
}


bool Tools::LoadFile(wxString const &filename, wxString& fileContent, wxUint32 length, bool fromStart)
{
	wxMutexLocker lock(mMutexLoadFile);

	if(!wxFile::Exists(filename))
		return false;

	wxFile in(filename, wxFile::read);
	length = (length == 0) ? in.Length() : length;
	if(length==0)
		return false;
	char *m_buffer;
	m_buffer = new char[length];
	m_buffer[length-1] = 0;
	int seeked = 0;

	if(fromStart == false)
		seeked = in.SeekEnd(-(std::min<off_t>(length, in.Length())-1));

	int read = in.Read(m_buffer, std::min<off_t>(length, in.Length()));

	if(read == wxInvalidOffset || (fromStart == false && seeked == wxInvalidOffset)) {
		in.Close();
		if(m_buffer != NULL)
		{
			delete [] m_buffer;
			m_buffer = NULL;
		}
		return false;
	}

	fileContent = wxString::FromAscii(m_buffer);

	if(m_buffer != NULL)
	{
		delete [] m_buffer;
		m_buffer = NULL;
	}

	return true;
}


wxString Tools::FormatSeconds(wxUint32 nbSeconds)
{
	wxUint32 nbHours;
	wxUint32 nbMinutes;

	nbHours   = nbSeconds / 3600;        // There are 3600 seconds in an hour
	nbSeconds = nbSeconds % 3600;
	nbMinutes = nbSeconds / 60;          // There are 60 seconds in a minute
	nbSeconds = nbSeconds % 60;

	if(nbHours != 0)
	{
		return wxString::Format(_T("%uh %02umn %02us"), nbHours, nbMinutes, nbSeconds);
	}
	else if(nbMinutes != 0)
	{
		return wxString::Format(_T("%umn %02us"), nbMinutes, nbSeconds);
	}
	else
	{
		return wxString::Format(_T("%us"), nbSeconds);
	}
}

std::vector<wxString> Tools::SplitLineByDelim(wxString const &line, wxString const &delim)
{
	std::vector<wxString>  v;
	wxInt32                endingPos;
	wxString               choppedString;
	wxString               tmpString;

	choppedString = line;
	choppedString += delim;
	endingPos = choppedString.Find(delim);
	while(endingPos != wxNOT_FOUND)
	{
		tmpString = choppedString.Mid(0, endingPos);
		choppedString = choppedString.Mid(endingPos + 1, choppedString.length()- 1);
		endingPos = choppedString.Find(delim);
		v.push_back(tmpString);
	}
	return v;
}
