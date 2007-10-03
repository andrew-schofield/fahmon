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
 
#ifndef _PROJECTSMANAGER_H
#define _PROJECTSMANAGER_H


#include "project.h"
#include "wx/thread.h"
#include "wx/hashmap.h"
#include "progressManager.h"


/**
 * Preferences used by this class
**/
#define PREF_PROJECTSMANAGER_INTERVALBETWEENUPDATES    wxT("ProjectsManager.IntervalBetweenUpdates")   // In seconds
#define PREF_PROJECTSMANAGER_INTERVALBETWEENUPDATES_DV 3600


WX_DECLARE_HASH_MAP(ProjectId, Project*, wxIntegerHash, wxIntegerEqual, ProjectsListHashMap);


/**
 * This is the component which manages the list of projects
 * It's a singleton
**/
class ProjectsManager
{
protected:
    long                    mLastUpdateTimestamp;
    static wxMutex          mMutexUpdateDatabase;
    ProjectsListHashMap     mProjectsHashMap;
    static ProjectsManager *mInstance;

     ProjectsManager(void);
    ~ProjectsManager(void);

    void Save(void);
    void Load(void);

    void UpdateToV2(void);

    void AddProject(Project* project);
    bool ShouldPerformUpdate(void);

    bool     Update_DownloadProjectsFile(wxString& fileName, ProgressManager& progressManager, wxString& errorMsg);
    bool     Update_ParseProjectsFile(const wxString& fileName, ProgressManager& progressManager, wxString& errorMsg);
    Project* Update_ParseProjectInfo(const wxString& projectInfo) const;


public:
    // Singleton pattern
    static void CreateInstance(void);
    static void DestroyInstance(void);
    static ProjectsManager* GetInstance(void);

    void UpdateDatabaseThreaded(bool forced, bool silentMode);
    bool UpdateDatabase(bool forced, bool silentMode);

    // Return NULL if the project is unknown
    const Project* GetProject(ProjectId projectId);
};


#endif /* _PROJECTSMANAGER_H */
