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
 * \file projectsManager.h
 * Manages projects.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _PROJECTSMANAGER_H
#define _PROJECTSMANAGER_H


#include "project.h"
#include "wx/thread.h"
#include "wx/hashmap.h"
#include "progressManager.h"


#define PREF_PROJECTSMANAGER_INTERVALBETWEENUPDATES    wxT("ProjectsManager.IntervalBetweenUpdates") /**< Preference setting for minimum time between updates */
#define PREF_PROJECTSMANAGER_INTERVALBETWEENUPDATES_DV 3600 /**< Default value for update interval */


WX_DECLARE_HASH_MAP(ProjectId, Project*, wxIntegerHash, wxIntegerEqual, ProjectsListHashMap); /**< ProjectsListHashMap: ProjectId -> Project */


/**
 * Projects management class.
 * This class can only be instantiated once.
 **/
class ProjectsManager
{
protected:
	long                    mLastUpdateTimestamp; /**< Time database was last updated */
	static wxMutex          mMutexUpdateDatabase; /**< Lock access to the update method */
	ProjectsListHashMap     mProjectsHashMap; /**< Hashmap to map project ids to project info */
	static ProjectsManager *mInstance; /**< The single instance of the projects manager */

	/**
	 * Constructor.
	 * Contructs the project manager.
	 **/
	ProjectsManager(void);

	/**
	 * Destructor.
	 **/
	~ProjectsManager(void);

	/**
	 * Load the list of projects from the disk.
	 **/
	void Load(void);

	/**
	 * Update an unversioned database to version 2.
	 * Updates all deadlines to be deadline*100 to allow storing of non integer deadlines.
	 **/
	void UpdateToV2(void);

	/**
	 * Updates version 2 database to version 3.
	 * Adds kfactor information for fast return bonus.
	 **/
	void UpdateToV3(void);

	/**
	 * Test if an update should be performed.
	 * This depends on the elapsed time since the last update
	 **/
	bool ShouldPerformUpdate(void);

	/**
	 * This method downloads the files with the current projects.
	 * Return false if something went wrong, true otherwise
	 * In the case of an error, an explicit message is placed in errorMsg
	 * The name of the file to which the downloaded data was written is in fileName, which will be empty in case of an error
	 * @param fileName File to download to
	 * @param progressManager The object managing the download
	 * @param errorMsg The return error msg if any.
	 **/
	bool     Update_DownloadProjectsFile(wxString& fileName, ProgressManager& progressManager, wxString& errorMsg);

	/**
	 * Read the current projects from the given file.
	 * Return false is something goes wrong, true otherwise
	 * In case of error, put a message in errorMsg
	 * @param fileName The file to read
	 * @param progressManager The object managing the parser
	 * @param errorMsg The return error msg if any
	 **/
	bool     Update_ParseProjectsFile(wxString const &fileName, ProgressManager& progressManager, wxString& errorMsg);

	/**
	 * Parse a line with project information from the PSummary file.
	 * @param projectInfo The project information string.
	 **/
	Project* Update_ParseProjectInfo(wxString const &projectInfo) const;


public:
	// Singleton pattern
	/**
	 * Create the instance of ProjectsManager.
	 **/
	static void CreateInstance(void);

	/**
	 * Destroy the single instance of the ProjectsManager.
	 * Only if it exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Retrieve the instance of ProjectsManager.
	 * @return An instance of class ProjectsManager.
	 **/
	static ProjectsManager* GetInstance(void);

	/**
	 * Create threads to refresh the projects database.
	 * @param forced Should update run regardless of timeout etc.
	 * @param silentMode Should update show a progress dialog
	 **/
	void UpdateDatabaseThreaded(bool forced, bool silentMode);

	/**
	 * Refresh the projects database by downloading the new projects from the official website.
	 * This method returns only when the refreshing is done
	 *
	 * This method is thread-safe
	 *
	 * If forced is false, the refresh is done only if the elapsed time since last one is great enough
	 * If it is true, the refresh is always done
	 *
	 * Return true if the update has been done, or false if an error occured or if it was aborted
	 * @param forced Should update run regardless of timeout etc.
	 * @param silentMode Should update show a progress dialog
	 **/
	bool UpdateDatabase(bool forced, bool silentMode);

	/**
	 * Add a project to the database.
	 * If the project was already there, then it is replaced by the new one
	 **/
	void AddProject(Project* project);

	/**
	 * Save the list of projects to the disk.
	 **/
	void Save(void);

	/**
	 * Retrieve the project corresponding to the given identifier.
	 * Return NULL if the project is unknown.
	 * @param projectId Project number to retrieve.
	 * @return Project object
	 **/
	const Project* GetProject(ProjectId projectId);
};


#endif /* _PROJECTSMANAGER_H */
