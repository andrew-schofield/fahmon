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
 * \file pathManager.h
 * Manages important path locations.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _PATHMANAGER_H
#define _PATHMANAGER_H

#include "wx/string.h"


/**
 * PathManager class.
 * This class provides the paths to the main directories used by FahMon.
 **/
class PathManager
{
protected:
	static PathManager* mInstance; /**< The single instance of the Path Manager */

	wxString mXYZPath; /**< The path to store xyz coordinate files */
	wxString mImgPath; /**< The path to load images from */
	wxString mCfgPath; /**< The path where the config file is located */
	wxString mMsgPath; /**< The path where the message log is located */
	wxString mGlobalTplPath; /**< The global web template path */
	wxString mUserTplPath; /**< The local web template path */

	/**
	 * Contructor.
	 * Contructs the path manager.
	 **/
	PathManager(void);

	/**
	 * Destructor.
	 **/
	~PathManager(void) {};

	// Singleton pattern
	/**
	 * Retrieve the instance of PathManager.
	 * If an instance doesn't already exist, create it.
	 * @return An instance of class PathManager.
	 **/
	static const PathManager* GetInstance(void);


public:
	/**
	 * Get the xyz path.
	 * @return The xyz path.
	 **/
	static wxString GetXYZPath(void)       {return GetInstance()->mXYZPath;}

	/**
	 * Get the img path.
	 * @return The img path.
	 **/
	static wxString GetImgPath(void)       {return GetInstance()->mImgPath;}

	/**
	 * Get the cfg path.
	 * @return The cfg path.
	 **/
	static wxString GetCfgPath(void)       {return GetInstance()->mCfgPath;}

	/**
	 * Get the msg path.
	 * @return The msg path.
	 **/
	static wxString GetMsgPath(void)       {return GetInstance()->mMsgPath;}

	/**
	 * Get the global template path.
	 * @return The global template path.
	 **/
	static wxString GetGlobalTplPath(void) {return GetInstance()->mGlobalTplPath;}

	/**
	 * Get the local template path.
	 * @return The local template path.
	 **/
	static wxString GetUserTplPath(void)   {return GetInstance()->mUserTplPath;}
};


#endif /* _PATHMANAGER_H */
