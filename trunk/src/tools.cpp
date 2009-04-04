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
#include "multiProtocolFile.h"

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


bool Tools::LoadFile(wxString const &filename, wxString& fileContent, wxString& localname, wxUint32 length, bool fromStart)
{
	wxMutexLocker lock(mMutexLoadFile);

	if(!multiProtocolFile::FileExists(filename))
		return false;

	localname = multiProtocolFile::GetLocalFileName(filename);

	if(!wxFile::Access(localname, wxFile::read))
		return false;

	wxFile in(localname, wxFile::read);
	if(!in.IsOpened())
		return false;
	length = (length == 0) ? in.Length() : std::min<off_t>(length, in.Length());
	if(length==0)
		return false;
	char *m_buffer;
	m_buffer = new char[length+1];
	m_buffer[length] = 0;
	int seeked = 0;

	if(fromStart == false)
		seeked = in.SeekEnd(-(std::min<off_t>(length, in.Length())));

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
