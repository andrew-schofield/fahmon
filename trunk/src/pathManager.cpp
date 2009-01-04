/*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/**
 * \file pathManager.cpp
 * Manages important path locations.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "pathManager.h"

#include "wx/utils.h"
#include "wx/filename.h"
#include "wx/app.h"

// The single instance accross the application
PathManager* PathManager::mInstance = NULL;


PathManager::PathManager(void)
{
#ifdef __WXGTK__

	/* On Linux systems, we use the standard way to store data : in a hidden directory
	   in the user's home directory the image path is now obtained from the DATADIR
	   which is usually /usr/local/share introduced along with the proper GNU
	   installation system */
	wxString homeDir;

	homeDir = wxGetHomeDir();
	if(homeDir.Last() != '/')
	{
		homeDir += _T("/");
	}

	mImgPath = wxString::Format(_T("%s/pixmaps/fahmon/"), _T(DATADIR));
	mGlobalTplPath = wxString::Format(_T("%s/fahmon/templates/"), _T(DATADIR));
	mUserTplPath = homeDir + _T(".fahmon/templates/");
	mCfgPath = homeDir + _T(".fahmon/");
	mXYZPath = homeDir + _T(".fahmon/xyz/");
	mMsgPath = homeDir + _T(".fahmon/");

#elif _FAHMON_WIN32_

	// On Win32 systems, we use the user's application data folder

	wxString homeDir;

	if(!wxTheApp.GetLocal())
	{
		wxGetEnv(_T("APPDATA"), &homeDir);

		homeDir += _T("\\FahMon");
	}
	else
	{
		homeDir = wxFileName(wxTheApp->argv[0]).GetPath(wxPATH_GET_VOLUME);
	}

	mXYZPath = homeDir + (_T("\\xyz\\"));
	mImgPath = wxString::Format(_T("%s\\images\\"), wxFileName(wxTheApp->argv[0]).GetPath(wxPATH_GET_VOLUME));
	mCfgPath = homeDir + (_T("\\config\\"));
	mGlobalTplPath = wxString::Format(_T("%s\\templates\\"), wxFileName(wxTheApp->argv[0]).GetPath(wxPATH_GET_VOLUME));
	mUserTplPath = homeDir + (_T("\\templates\\"));
	mMsgPath = homeDir + _T("\\");

#elif __WXMAC__

	// On OSX systems, the preferences are stored in ~/Library/Application Support
	// by convention

	wxFileName appPath = wxFileName(wxTheApp->argv[0]).GetPath (wxPATH_GET_VOLUME);
	appPath.RemoveLastDir();

	wxString resourcesPath = appPath.GetPath();

	resourcesPath += _T("/Contents/Resources/");

	wxString homeDir;

	homeDir = wxGetHomeDir();
	if(homeDir.Last() != '/')
		homeDir += _T("/");

	mImgPath = resourcesPath;
	mCfgPath = homeDir + _T("Library/Application Support/FahMon/config/");
	mXYZPath = homeDir + _T("Library/Application Support/FahMon/xyz/");
	mMsgPath = homeDir + _T("Library/Application Support/FahMon/");
	mGlobalTplPath = resourcesPath;
	mUserTplPath = homeDir + _T("Library/Application Support/FahMon/templates/");

#endif
}


const PathManager* PathManager::GetInstance(void)
{
	if(mInstance == NULL)
	{
		mInstance = new PathManager();
	}

	return mInstance;
}
