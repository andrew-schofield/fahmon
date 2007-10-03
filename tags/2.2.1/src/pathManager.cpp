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
 
#include "fahmon.h"
#include "pathManager.h"

#include "wx/utils.h"


// The single instance accross the application
PathManager* PathManager::mInstance = NULL;


/**
 * Constructor
**/
PathManager::PathManager(void)
{
#ifdef _FAHMON_LINUX_

    // On Linux systems, we use the standard way to store data : in a hidden directory
    // in the user's home direcrtory
    wxString homeDir;

    homeDir = wxGetHomeDir();
    if(homeDir.Last() != '/')
        homeDir += wxT("/");

    mImgPath = wxT("./images/");
    mCfgPath = homeDir + wxT(".fahmon/");
    mXYZPath = homeDir + wxT(".fahmon/xyz/");
    mMsgPath = homeDir + wxT(".fahmon/");

#else

    // On Win32 systems, we simply use the current directory
    mXYZPath = wxT("./xyz/");
    mImgPath = wxT("./images/");
    mCfgPath = wxT("./config/");
    mMsgPath = wxT("./");

#endif
}


/**
 * Retrieve the instance of PathManager, create it if needed
**/
const PathManager* PathManager::GetInstance(void)
{
    if(mInstance == NULL)
        mInstance = new PathManager();

    return mInstance;
}
