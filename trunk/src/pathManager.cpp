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
		homeDir += wxT("/");
	}

	mImgPath = wxString::Format(wxT("%s/pixmaps/fahmon/"), wxT(DATADIR));
	mGlobalTplPath = wxString::Format(wxT("%s/fahmon/templates/"), wxT(DATADIR));
	mUserTplPath = homeDir + wxT(".fahmon/templates/");
	mCfgPath = homeDir + wxT(".fahmon/");
	mXYZPath = homeDir + wxT(".fahmon/xyz/");
	mMsgPath = homeDir + wxT(".fahmon/");

#elif _FAHMON_WIN32_

	// On Win32 systems, we use the user's application data folder

	wxString homeDir;

	wxGetEnv(_T("APPDATA"), &homeDir);

	homeDir += wxT("\\FahMon");

	mXYZPath = homeDir + (wxT("\\xyz\\"));
	mImgPath = wxString::Format(wxT("%s\\images\\"), wxFileName(wxTheApp->argv[0]).GetPath(wxPATH_GET_VOLUME));
	mCfgPath = homeDir + (wxT("\\config\\"));
	mGlobalTplPath = wxString::Format(wxT("%s\\templates\\"), wxFileName(wxTheApp->argv[0]).GetPath(wxPATH_GET_VOLUME));
	mUserTplPath = homeDir + (wxT("\\templates\\"));
	mMsgPath = homeDir + wxT("\\");

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
		homeDir += wxT("/");

	mImgPath = resourcesPath;
	mCfgPath = homeDir + wxT("Library/Application Support/FahMon/config/");
	mXYZPath = homeDir + wxT("Library/Application Support/FahMon/xyz/");
	mMsgPath = homeDir + wxT("Library/Application Support/FahMon/");
	mGlobalTplPath = resourcesPath;
	mUserTplPath = homeDir + wxT("Library/Application Support/FahMon/templates/");

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
