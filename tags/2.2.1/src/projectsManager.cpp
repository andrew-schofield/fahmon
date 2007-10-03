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
#include "projectsManager.h"

#include "tools.h"
#include "htmlParser.h"
#include "mainDialog.h"
#include "pathManager.h"
#include "wx/textfile.h"
#include "wx/filename.h"
#include "httpDownloader.h"
#include "messagesManager.h"
#include "dataInputStream.h"
#include "dataOutputStream.h"
#include "preferencesManager.h"
#include "projectHelperThread.h"


// The single instance of ProjectsManager accross the application
ProjectsManager* ProjectsManager::mInstance = NULL;

// This mutex is there to ensure that two threads won't update the database simultaneously
wxMutex ProjectsManager::mMutexUpdateDatabase;


/**
 * Constructor
**/
ProjectsManager::ProjectsManager(void)
{
    // By setting this to 0, we ensure that the next update of the database will always be performed
    mLastUpdateTimestamp = 0;
}


/**
 * Destructor
**/
ProjectsManager::~ProjectsManager(void)
{
}


/**
 * Create the single instance of the ProjectsManager
**/
void ProjectsManager::CreateInstance(void)
{
    wxASSERT(mInstance == NULL);

    mInstance = new ProjectsManager();
    mInstance->Load();
}


/**
 * Destroy the single instance of the ProjectsManager
**/
void ProjectsManager::DestroyInstance(void)
{
    wxASSERT(mInstance != NULL);

    mInstance->Save();

    delete mInstance;
    mInstance = NULL;
}


/**
 * Return the single instance of the ProjectsManager
**/
ProjectsManager* ProjectsManager::GetInstance(void)
{
    wxASSERT(mInstance != NULL);

    return mInstance;
}


/**
 * Load the list of projects from the disk
**/
inline void ProjectsManager::Load(void)
{
    Project         *currentProject;
    wxUint32         nbProjects, i, version;
    DataInputStream  in(PathManager::GetCfgPath() + wxT(FMC_FILE_PROJECTS));

    // Could the file be opened ?
    if(in.Ok() == false)
    {
        _LogMsgWarning(wxT("There is no projects file, or it is not readable"));
        return;
    }

    // Find how many projects are present, get the version number, and read them one by one
    in.ReadUint(nbProjects);
    for(i=0; i<nbProjects; ++i)
    {
        currentProject = new Project();
        currentProject->Read(in);
        AddProject(currentProject);
    }
    in.ReadUint(version);
    if(version == 135456360) //why is it this number?
        version = 1;

    if(version == 1)
        UpdateToV2();
}


/**
 * Save the list of projects to the disk
**/
inline void ProjectsManager::Save(void)
{
    DataOutputStream               out(PathManager::GetCfgPath() + wxT(FMC_FILE_PROJECTS));
    ProjectsListHashMap::iterator  iterator;

    if(out.Ok() == false)
    {
        Tools::ErrorMsgBox(wxString::Format(wxT("Could not open file <%s> for writing!"), (PathManager::GetCfgPath() + wxT(FMC_FILE_PROJECTS)).c_str()));
        return;
    }

    // A simple format is used
    //  1) An unsigned int is first written, containing the number of projects
    //  2) Then, each project is written
    out.WriteUint(mProjectsHashMap.size());
    for(iterator=mProjectsHashMap.begin(); iterator!=mProjectsHashMap.end(); ++iterator)
        iterator->second->Write(out);
    // write projects database version
    out.WriteUint(FMC_PROJECTS_VERSION);
}


/**
 * Add a project to the database
 * If the project was already there, then it is replaced by the new one
**/
void ProjectsManager::AddProject(Project* project)
{
    wxASSERT(project != NULL);

    // Add or update the project
    mProjectsHashMap[project->GetProjectId()] = project;
}


/**
 * Retrieve the project corresponding to the given identifier
 * Return NULL if the project is unknown
**/
const Project* ProjectsManager::GetProject(ProjectId projectId)
{
    ProjectsListHashMap::iterator iterator = mProjectsHashMap.find(projectId);

    if(iterator == mProjectsHashMap.end())
        return NULL;

    return iterator->second;
}


/************************************  DATABASE UPDATE  ************************************/


/**
 * Test if an update should be performed
 * This depends on the elapsed time since the last update
**/
bool ProjectsManager::ShouldPerformUpdate(void)
{
    wxUint32 elapsedTime;
    wxUint32 intervalBetweenUpdates;

    _PrefsGetUint(PREF_PROJECTSMANAGER_INTERVALBETWEENUPDATES, intervalBetweenUpdates);

    elapsedTime = (wxUint32)(wxGetLocalTime() - mLastUpdateTimestamp);
    if(elapsedTime < intervalBetweenUpdates)
        return false;

    return true;
}


/**
 * Create threads to refresh the projects database
**/
void ProjectsManager::UpdateDatabaseThreaded(bool forced, bool silentMode)
{
    // Create a thread which will call the 'inlined' method
    new ProjectHelperThread(forced, silentMode);
}


/**
 * Refresh the projects database by downloading the new projects from the official website
 * This method returns only when the refreshing is done
 *
 * This method is thread-safe
 *
 * If forced is false, the refresh is done only if the elapsed time since last one is great enough
 * If it is true, the refresh is always done
 *
 * Return true if the update has been done, or false if an error occured or if it was aborted
**/
bool ProjectsManager::UpdateDatabase(bool forced, bool silentMode)
{
    wxMutexLocker   mutexLocker(mMutexUpdateDatabase);        // --- Lock the access to this method
    wxString        projectFile;
    wxString        errorMsg;
    ProgressManager progressManager(silentMode);

    // Don't perform this update if we shouldn't
    if(forced == false && ShouldPerformUpdate() == false)
        return false;

    // This update will be the new reference
    mLastUpdateTimestamp = wxGetLocalTime();

    _LogMsgInfo(wxT("Updating projects database"));


    // --- We first have to download the new projects
    // This part is estimated to be 90% of the total work
    progressManager.SetTextAndProgress(wxT("Downloading new projects"), 0);
    progressManager.CreateTask(90);
    if(Update_DownloadProjectsFile(projectFile, progressManager, errorMsg) == false)
    {
        // Display the message only if we are not in silent mode
        if(silentMode == false)
            Tools::ErrorMsgBox(errorMsg);

        // Don't forget to remove the file before leaving, if it is needed
        if(projectFile.IsEmpty() == false)
            wxRemoveFile(projectFile);

        // Stop there, since we cannot parse a non-existing file
        return false;
    }
    progressManager.EndTask();


    // --- We can then parse the file (10% of the total work)
    progressManager.SetText(wxT("Adding the new projects to the database"));
    progressManager.CreateTask(10);
    if(Update_ParseProjectsFile(projectFile, progressManager, errorMsg) == false)
    {
        if(silentMode == false)
            Tools::ErrorMsgBox(errorMsg);
    }
    progressManager.EndTask();


    // --- We can finally delete the temporary file
    // This file must exist if we reached this part of the code
    wxASSERT(projectFile.IsEmpty() == false);
    wxRemoveFile(projectFile);

    return true;
}


/**
 * This method downloads the files with the current projects
 * Return false if something went wrong, true otherwise
 * In the case of an error, an explicit message is placed in errorMsg
 * The name of the file to which the downloaded data was written is in fileName, which will be empty in case of an error
**/
bool ProjectsManager::Update_DownloadProjectsFile(wxString& fileName, ProgressManager& progressManager, wxString& errorMsg)
{
    bool     useAlternate;
    bool     useLocalFile;
    bool     fileexists;
    bool     copied;
    wxString projectLocationServer;
    wxUint32 projectLocationPort;
    wxString projectLocationResource;
    wxString projectLocalFile;
    wxString serverused;
    wxUint32 portused;
    wxString resourceused;

    HTTPDownloader::DownloadStatus downloadStatus;

    _PrefsGetBool        (PREF_HTTPDOWNLOADER_USEALTERNATEUPDATE,              useAlternate);
    _PrefsGetString      (PREF_HTTPDOWNLOADER_ALTERNATEUPDATELOCATIONSERVER,   projectLocationServer);
    _PrefsGetUint        (PREF_HTTPDOWNLOADER_ALTERNATEUPDATELOCATIONPORT,     projectLocationPort);
    _PrefsGetString      (PREF_HTTPDOWNLOADER_ALTERNATEUPDATELOCATIONRESOURCE, projectLocationResource);
    _PrefsGetBool        (PREF_HTTPDOWNLOADER_USELOCALFILE,                    useLocalFile);
    _PrefsGetString      (PREF_HTTPDOWNLOADER_LOCALFILELOCATION,               projectLocalFile);

    //Initialise the port number
    portused = 80;

    if(useLocalFile ==false)
    {
        if(useAlternate == false)
        {
            serverused = wxT(FMC_URL_PROJECTS_SERVER);
            portused = 80;
            resourceused = wxT(FMC_URL_PROJECTS_RESOURCE);
        }

        if(useAlternate == true && useLocalFile == false)
        {
            serverused = projectLocationServer;
            portused = projectLocationPort;
            resourceused = projectLocationResource;
        }

        // Download the file
        downloadStatus = HTTPDownloader::DownloadFile(serverused, portused, resourceused, fileName, progressManager);

        // If nothing went wrong, we can stop here
        if(downloadStatus == HTTPDownloader::STATUS_NO_ERROR)
            return true;

        // Otherwise, we create an explicit error message to specify what went wrong
        switch(downloadStatus)
        {
            case HTTPDownloader::STATUS_TEMP_FILE_CREATION_ERROR:
                errorMsg = wxT("Unable to create a temporary file!");
                break;

            case HTTPDownloader::STATUS_TEMP_FILE_OPEN_ERROR:
                errorMsg = wxString::Format(wxT("Unable to open the temporary file <%s>"), fileName.c_str());
                break;

            case HTTPDownloader::STATUS_CONNECT_ERROR:
                errorMsg = wxT("Unable to connect to the server!");
                break;

            case HTTPDownloader::STATUS_SEND_REQUEST_ERROR:
                errorMsg = wxT("Unable to send the request to the server!");
                break;

            case HTTPDownloader::STATUS_ABORTED:
                errorMsg = wxT("Download aborted!");
                break;

            // We should never fall here
            default:
                wxASSERT(false);
                errorMsg = wxT("An unknown error happened!");
                break;
        }
    } else {
        fileexists = wxFileExists(projectLocalFile);
        if(fileexists == false)
        {
            errorMsg = wxT("Local project update file doesn't exist!");
            return false;
        } else {
            fileName = wxFileName::CreateTempFileName(wxT(FMC_APPNAME));
            copied = wxCopyFile(projectLocalFile, fileName);
            if(copied == false)
            {
                errorMsg = wxT("Unable to copy local project update file to temporary location");
                return false;
            } else {
                return true;
            }
        }
    }

    return false;
}


/**
 * Read the current projects from the given file
 * Return false is something goes wrong, true otherwise
 * In case of error, put a message in errorMsg
**/
bool ProjectsManager::Update_ParseProjectsFile(const wxString& fileName, ProgressManager& progressManager, wxString& errorMsg)
{
    bool        tableHeaderFound;
    bool        errorOccured;
    wxInt32     pos;
    wxUint32    i;
    wxString    currentLine;
    wxString    projectInfo;
    Project    *project;
    wxTextFile  in;

    in.Open(fileName);
    tableHeaderFound = false;
    errorOccured = false;
    for(i=0; i<in.GetLineCount(); ++i)
    {
        currentLine = in.GetLine(i);
        currentLine.LowerCase();

        // The lines with the projects contains the string <tr bgcolor="#xxxxxx">, so this is what we will be
        // looking for
        // The color is not always the same, so we don't include it in the searched string
        pos = currentLine.Find(wxT("<tr bgcolor="));
        if(pos != -1)
        {
            // The first line containing the searched string is the header of the table, so we simply ignore it
            if(tableHeaderFound == false)
                tableHeaderFound = true;
            else
            {
                // 23 is the length of the string we've been looking for, projectInfo string won't contain it
                projectInfo = currentLine.Mid(pos+23);

                // Try to parse this line
                // If the parser fails, store the error and continue with the next line
                project = Update_ParseProjectInfo(projectInfo);
                if(project != NULL)
                    AddProject(project);
                else
                {
                    errorMsg = wxString::Format(wxT("The line %u is not correctly formatted!\n\n%s"), i+1, currentLine.c_str());
                    errorOccured = true;
                }
            }
        }

        // Update the progress
        // in.GetLineCount() can't be equal to 0, otherwise we wouldn't be in the loop
        wxASSERT(in.GetLineCount() != 0);
        if(progressManager.SetProgress((i * 100) / in.GetLineCount()) == false)
        {
            errorMsg = wxT("Update aborted!");
            return false;
        }
    }

    return !errorOccured;
}


/**
 * Parse a line with project information from the PSummary file
**/
Project* ProjectsManager::Update_ParseProjectInfo(const wxString& projectInfo) const
{
    long       tmpLong;
    double     tmpDouble;
    CoreId     coreId;
    FrameId    nbFrames;
    WuCredit   credit;
    wxUint32   preferredDeadlineInDays;
    wxUint32   finalDeadlineInDays;
    ProjectId  projectId;
    HTMLParser parser;

    // This is ugly, but it works...
    parser.ParseString(projectInfo);

    // The identifier
    parser.NextToken(2);
    if(parser.GetCurrentText().ToLong(&tmpLong) == false)
        return NULL;
    projectId = (ProjectId)tmpLong;

    // Preferred deadline in days
    // It is possible that there is no preferred deadline, in this case there is a '--' string instead
    // of the value
    parser.NextToken(12);
    if(parser.GetCurrentText().Find(wxT("--")) != -1)
        preferredDeadlineInDays = 0;
    else
    {
        if(parser.GetCurrentText().ToDouble(&tmpDouble) == false)
            return NULL;
        preferredDeadlineInDays = (wxUint32)tmpDouble * 100;
    }

    // Final deadline in days
    // This can also be the case for the final deadline
    parser.NextToken(3);
    if(parser.GetCurrentText().Find(wxT("--")) != -1)
        finalDeadlineInDays = 0;
    else
    {
        if(parser.GetCurrentText().ToDouble(&tmpDouble) == false)
            return NULL;
        finalDeadlineInDays = (wxUint32)tmpDouble * 100;
    }

    // Credit
    parser.NextToken(3);
    if(parser.GetCurrentText().ToDouble(&tmpDouble) == false)
        return NULL;
    credit = (WuCredit)tmpDouble;

    // Number of frames
    parser.NextToken(3);
    if(parser.GetCurrentText().ToLong(&tmpLong) == false)
        return NULL;
    nbFrames = (FrameId)tmpLong;

    // The erroneous Gromacs 33 '0 frame' issue is no longer present
    // Thus apply a generic rule for if frames = 0 then frames = 100
    if (nbFrames == 0)
         nbFrames = 100;

    // Core
    parser.NextToken(3);
    coreId = Core::ShortNameToId(parser.GetCurrentText());

    return new Project(projectId, preferredDeadlineInDays, finalDeadlineInDays, nbFrames, credit, coreId);
}

/**
 * Update all deadlines to be deadline*100 to allow storing of non integer deadlines
**/
void ProjectsManager::UpdateToV2(void)
{
    ProjectsListHashMap::iterator  iterator;
    const Project     *editingProject;
    Project    *editedProject;
    wxUint32   preferredDeadlineInDays;
    wxUint32   finalDeadlineInDays;

    for(iterator=mProjectsHashMap.begin(); iterator!=mProjectsHashMap.end(); ++iterator)
    {
        editingProject = iterator->second;
        if(editingProject != NULL)
        {
            preferredDeadlineInDays = editingProject->GetPreferredDeadlineInDays() * 100;
            finalDeadlineInDays = editingProject->GetFinalDeadlineInDays() * 100;
            editedProject = new Project(editingProject->GetProjectId(), preferredDeadlineInDays, finalDeadlineInDays, editingProject->GetNbFrames(), editingProject->GetCredit(), editingProject->GetCoreId());
            AddProject(editedProject);
        }
    }

}