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
 * \file projectsManager.cpp
 * Manages projects.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "projectsManager.h"

#include "tools.h"
#include "htmlParser.h"
#include "mainDialog.h"
#include "pathManager.h"
#include "httpDownloader.h"
#include "messagesManager.h"
#include "dataInputStream.h"
#include "preferencesManager.h"
#include "projectHelperThread.h"
#include "progressManager.h"
#include "tinyxml.h"

#include "wx/textfile.h"
#include "wx/filename.h"

#include "locale.h"

// The single instance of ProjectsManager accross the application
ProjectsManager* ProjectsManager::mInstance = NULL;

// This mutex is there to ensure that two threads don't update the database simultaneously
wxMutex ProjectsManager::mMutexUpdateDatabase;


ProjectsManager::ProjectsManager(void)
{
	// By setting this to 0, we ensure that the next update of the database will always be performed
	mLastUpdateTimestamp = 0;
}


ProjectsManager::~ProjectsManager(void)
{
	ProjectsListHashMap::iterator  iterator;
	for(iterator=mProjectsHashMap.begin(); iterator!=mProjectsHashMap.end(); ++iterator)
	{
		if(iterator->second != NULL)
		{
			delete iterator->second;
			iterator->second = NULL;
		}
	}
}


void ProjectsManager::CreateInstance(void)
{
	wxASSERT(mInstance == NULL);

	mInstance = new ProjectsManager();
	mInstance->Load();
}


void ProjectsManager::DestroyInstance(void)
{
	wxASSERT(mInstance != NULL);

	mInstance->Save();

	delete mInstance;
	mInstance = NULL;
}


ProjectsManager* ProjectsManager::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}


void ProjectsManager::LoadOld(void)
{
	Project         *currentProject;
	wxUint32         nbProjects, i, version;
	DataInputStream  in(PathManager::GetCfgPath() + _T("projects.dat"));

	// Could the file be opened ?
	if(in.Ok() == false)
	{
		_LogMsgWarning(_("There is no projects file, or it is not readable"), false);
		return;
	}

	// Find how many projects are present, get the version number, and read them one by one
	in.ReadUint(nbProjects);
	for(i=0; i<nbProjects; ++i)
	{
		currentProject = new Project;
		currentProject->ReadOld(in);
		AddProject(currentProject);
	}
	in.ReadUint(version);
	if(version < 1 || version > FMC_PROJECTS_VERSION)
	{
		version = 1;
	}

	if(version == 1)
	{
		UpdateToV2();
		version = 2;
	}

	if(version == 2)
	{
		UpdateToV3();
		version = 3;
	}
}


void ProjectsManager::Save(void)
{
    TiXmlDocument doc;
	TiXmlComment * comment;
	ProjectsListHashMap::iterator  iterator;


	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "yes");
	doc.LinkEndChild( decl );
	
	comment = new TiXmlComment();
	comment->SetValue(wxString::Format(_T(" %s Projects Database "), _T(FMC_APPNAME)).mb_str( wxConvUTF8 ));
	doc.LinkEndChild( comment );
	
	TiXmlElement * root = new TiXmlElement( "Database" );
	doc.LinkEndChild( root );
	wxString Version = wxString::Format(wxT("%i"), FMC_PROJECTS_VERSION);
	root->SetAttribute( "Version", Version.mb_str( wxConvUTF8 ) );
	
	TiXmlElement * projects = new TiXmlElement( "Projects" );
	root->LinkEndChild( projects );

	for(iterator=mProjectsHashMap.begin(); iterator!=mProjectsHashMap.end(); ++iterator)
	{
    	TiXmlElement * project = new TiXmlElement( "Project" );
		iterator->second->Write( project );
		projects->LinkEndChild( project );
	}
	if( doc.SaveFile( (PathManager::GetCfgPath() + _T(FMC_FILE_PROJECTS)).mb_str( wxConvUTF8 ) ) == false )
	{
		Tools::ErrorMsgBox(wxString::Format(_("Could not open file <%s> for writing!"), (PathManager::GetCfgPath() + _T(FMC_FILE_PROJECTS)).c_str()));
		return;
	}
}


void ProjectsManager::Load(void)
{
    if(wxFileExists(PathManager::GetCfgPath() + _T("projects.dat")))
    {
        LoadOld();
        Save();
	    wxRemoveFile(PathManager::GetCfgPath() + _T("projects.dat"));
        return;
    }
    TiXmlDocument  doc((PathManager::GetCfgPath() + _T(FMC_FILE_PROJECTS)).mb_str( wxConvUTF8 ));
	TiXmlElement*  elem;
	Project       *currentProject;
	int            version;
	
	bool loadOkay = doc.LoadFile();
	if(loadOkay == false)
	{
		_LogMsgWarning(_("There is no projects file, or it is not readable"), false);
		return;
	}

	TiXmlHandle docHandle( &doc );
	TiXmlElement* root = docHandle.FirstChild( "Database" ).ToElement();
	root->QueryIntAttribute("Version", &version); //for future changes
	if( root )
	{
	    elem = docHandle.FirstChild( "Database" ).FirstChild( "Projects" ).FirstChild( "Project" ).ToElement();
		while( elem )
		{
		    currentProject = new Project;
		    currentProject->Read(elem);
		    AddProject(currentProject);
			elem=elem->NextSiblingElement();
		}
	}
}


void ProjectsManager::AddProject(Project* project)
{
	wxASSERT(project != NULL);

	// If the project already exists we must first delete the project pointer in the
	// hashmap, and then remove the entry - prevent memory leaks due to pointers not being
	// deleted on a straight update (a *new* is used futher up the chain).
	ProjectsListHashMap::iterator iterator = mProjectsHashMap.find(project->GetProjectId());

	if(iterator != mProjectsHashMap.end())
	{
		if(iterator->second != NULL)
		{
			delete iterator->second;
			iterator->second = NULL;
		}
	}
	mProjectsHashMap.erase(project->GetProjectId());

	// Add the project
	mProjectsHashMap[project->GetProjectId()] = project;
}


const Project* ProjectsManager::GetProject(ProjectId projectId)
{
	ProjectsListHashMap::iterator iterator = mProjectsHashMap.find(projectId);

	if(iterator == mProjectsHashMap.end())
	{
		return NULL;
	}
	else
	{
		return iterator->second;
	}
}


/************************************  DATABASE UPDATE  ************************************/


bool ProjectsManager::ShouldPerformUpdate(void)
{
	wxUint32 elapsedTime;
	wxUint32 intervalBetweenUpdates;

	_PrefsGetUint(PREF_PROJECTSMANAGER_INTERVALBETWEENUPDATES, intervalBetweenUpdates);

	elapsedTime = (wxUint32)(wxGetLocalTime() - mLastUpdateTimestamp);
	if(elapsedTime < intervalBetweenUpdates)
	{
		return false;
	}

	return true;
}


void ProjectsManager::UpdateDatabaseThreaded(bool forced, bool silentMode)
{
	// Create a thread which will call the 'inlined' method
	new ProjectHelperThread(forced, silentMode);
}


bool ProjectsManager::UpdateDatabase(bool forced, bool silentMode)
{
	wxMutexLocker   lock(mMutexUpdateDatabase);        // --- Lock the access to this method
	wxString        projectFile;
	wxString        errorMsg;

	// Don't perform this update if we shouldn't
	if(forced == false && ShouldPerformUpdate() == false)
	{
		return false;
	}

	ProgressManager progressManager(silentMode);

	// This update will be the new reference
	mLastUpdateTimestamp = wxGetLocalTime();

	_LogMsgInfo(_("Updating projects database"), false);

	// --- We first have to download the new projects
	// This part is estimated to be 90% of the total work
	progressManager.SetTextAndProgress(_("Downloading new projects"), 0);

	progressManager.CreateTask(90);
	projectFile = wxFileName::CreateTempFileName(_T(FMC_APPNAME));
	if(Update_DownloadProjectsFile(projectFile, progressManager, errorMsg) == false)
	{
		// Display the message only if we are not in silent mode
		if(silentMode == false)
		{
			Tools::ErrorMsgBox(errorMsg);
		}

		// Don't forget to remove the file before leaving, if it is needed
		if(projectFile.IsEmpty() == false)
		{
			wxRemoveFile(projectFile);
		}

		// Stop there, since we cannot parse a non-existing file
		return false;
	}
	progressManager.EndTask();


	// --- We can then parse the file (10% of the total work)
	progressManager.SetText(_("Adding the new projects to the database"));
	progressManager.CreateTask(10);
	if(Update_ParseProjectsFile(projectFile, progressManager, errorMsg) == false)
	{
		if(silentMode == false)
		{
			Tools::ErrorMsgBox(errorMsg);
		}
	}
	progressManager.EndTask();


	// --- We can finally delete the temporary file
	// This file must exist if we reached this part of the code
	wxASSERT(projectFile.IsEmpty() == false);
	wxRemoveFile(projectFile);

	return true;
}


bool ProjectsManager::Update_DownloadProjectsFile(wxString& fileName, ProgressManager& progressManager, wxString& errorMsg)
{
	bool     useAlternate;
	bool     useLocalFile;
	bool     fileexists;
	bool     copied;
	wxString projectLocalFile;
	wxString resource, alternateAddress;

	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USEALTERNATEUPDATE,              useAlternate);
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USELOCALFILE,                    useLocalFile);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_LOCALFILELOCATION,               projectLocalFile);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_ALTERNATEUPDATEADDRESS,          alternateAddress);

	if(useLocalFile ==false)
	{
		if(useAlternate == false)
		{
			resource = _T(FMC_URL_PROJECTS);
		}

		else if(useAlternate == true && useLocalFile == false)
		{
			resource = alternateAddress;
		}

		// Download the file
		if(HTTPDownloader::GetHTTPFile(resource, fileName))
			return true;
		errorMsg = wxT("Something went wrong!");

	}
	else
	{
		fileexists = wxFileExists(projectLocalFile);
		if(fileexists == false)
		{
			errorMsg = _("Local project update file doesn't exist!");
			return false;
		}
		else
		{
			fileName = wxFileName::CreateTempFileName(_T(FMC_APPNAME));
			copied = wxCopyFile(projectLocalFile, fileName);
			if(copied == false)
			{
				errorMsg = _("Unable to copy local project update file to temporary location");
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}


bool ProjectsManager::Update_ParseProjectsFile(wxString const &fileName, ProgressManager& progressManager, wxString& errorMsg)
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
	char *locOld = setlocale(LC_NUMERIC, "C");
	for(i=0; i<in.GetLineCount(); ++i)
	{
		currentLine = in.GetLine(i);
		currentLine.LowerCase();

		// The lines with the projects contains the string <tr bgcolor="#xxxxxx">, so this is what we will be
		// looking for
		// The color is not always the same, so we don't include it in the searched string
		pos = currentLine.Find(_T("<tr bgcolor="));
		if(pos != -1)
		{
			// The first line containing the searched string is the header of the table, so we simply ignore it
			if(tableHeaderFound == false)
			{
				tableHeaderFound = true;
			}
			else
			{
				// 23 is the length of the string we've been looking for, projectInfo string won't contain it
				projectInfo = currentLine.Mid(pos+23);
				//_LogMsgInfo(projectInfo, false);

				// Try to parse this line
				// If the parser fails, store the error and continue with the next line
				project = Update_ParseProjectInfo(projectInfo);
				if(project != NULL)
				{
					AddProject(project);
				}
				else
				{
					_LogMsgWarning(wxString::Format(_("Line %u is not correctly formatted! %s"), i+1, currentLine.c_str()), false);
					//errorOccured = true;
				}
			}
		}

		// Update the progress
		// in.GetLineCount() can't be equal to 0, otherwise we wouldn't be in the loop
		wxASSERT(in.GetLineCount() != 0);
		if(progressManager.SetProgress((i * 100) / in.GetLineCount()) == false)
		{
			errorMsg = _("Update aborted!");
			in.Close();
			return false;
		}
	}
	setlocale(LC_NUMERIC, locOld);

	in.Close();
	return !errorOccured;
}


Project* ProjectsManager::Update_ParseProjectInfo(wxString const &projectInfo) const
{
	long       tmpLong;
	double     tmpDouble;
	CoreId     coreId;
	FrameId    nbFrames;
	WuCredit   credit;
	wxUint32   preferredDeadlineInDays;
	wxUint32   finalDeadlineInDays;
	wxUint32   kFactor;
	ProjectId  projectId;
	HTMLParser parser;

	// This is ugly, but it works...
	parser.ParseString(projectInfo);

	// The identifier
	parser.NextToken(2);
	if(parser.GetCurrentText().ToLong(&tmpLong) == false)
	{
		return NULL;
	}
	projectId = (ProjectId)tmpLong;

	// Preferred deadline in days
	// It is possible that there is no preferred deadline, in this case there is a '--' string instead
	// of the value
	parser.NextToken(12);
	if(parser.GetCurrentText().Find(_T("--")) != -1)
	{
		preferredDeadlineInDays = 0;
	}
	else
	{
		if(parser.GetCurrentText().ToDouble(&tmpDouble) == false)
		{
			return NULL;
		}
		preferredDeadlineInDays = (wxUint32)(tmpDouble * 100);
	}

	// Final deadline in days
	// This can also be the case for the final deadline
	parser.NextToken(3);
	if(parser.GetCurrentText().Find(_T("--")) != -1)
	{
		finalDeadlineInDays = 0;
	}
	else
	{
		if(parser.GetCurrentText().ToDouble(&tmpDouble) == false)
		{
			return NULL;
		}
		finalDeadlineInDays = (wxUint32)(tmpDouble * 100);
	}

	// Credit
	parser.NextToken(3);
	if(parser.GetCurrentText().ToDouble(&tmpDouble) == false)
	{
		return NULL;
	}
	credit = (WuCredit)tmpDouble;

	// Number of frames
	parser.NextToken(3);
	if(parser.GetCurrentText().ToLong(&tmpLong) == false)
	{
		return NULL;
	}
	nbFrames = (FrameId)tmpLong;

	// The erroneous Gromacs 33 '0 frame' issue is no longer present
	// Thus apply a generic rule for if frames = 0 then frames = 100
	if (nbFrames == 0)
	{
		nbFrames = 100;
	}

	// Core
	parser.NextToken(3);
	coreId = Core::ShortNameToId(parser.GetCurrentText());

	//KFactor
	parser.NextToken(15);
	if(parser.GetCurrentText().ToDouble(&tmpDouble) == false)
	{
		return NULL;
	}
	kFactor = (wxUint32)(tmpDouble * 100);

	return new Project(projectId, preferredDeadlineInDays, finalDeadlineInDays, nbFrames, credit, coreId, kFactor);
}


void ProjectsManager::UpdateToV2(void)
{
	ProjectsListHashMap::iterator  iterator;
	const Project     *editingProject;
	Project    *editedProject;
	wxUint32   preferredDeadlineInDays;
	wxUint32   finalDeadlineInDays;
	ProjectsListHashMap tempHM = mProjectsHashMap;

	for(iterator=tempHM.begin(); iterator!=tempHM.end(); ++iterator)
	{
		editingProject = iterator->second;
		if(editingProject != NULL)
		{
			preferredDeadlineInDays = editingProject->GetPreferredDeadlineInDays() * 100;
			finalDeadlineInDays = editingProject->GetFinalDeadlineInDays() * 100;
			editedProject = new Project(editingProject->GetProjectId(), preferredDeadlineInDays, finalDeadlineInDays, editingProject->GetNbFrames(), editingProject->GetCredit(), editingProject->GetCoreId(),0);
			AddProject(editedProject);
		}
	}
}


void ProjectsManager::UpdateToV3(void)
{
	ProjectsListHashMap::iterator  iterator;
	const Project     *editingProject;
	Project    *editedProject;
	ProjectsListHashMap tempHM = mProjectsHashMap;

	for(iterator=tempHM.begin(); iterator!=tempHM.end(); ++iterator)
	{
		editingProject = iterator->second;
		if(editingProject != NULL)
		{
			editedProject = new Project(editingProject->GetProjectId(), editingProject->GetPreferredDeadlineInDays(), editingProject->GetFinalDeadlineInDays(), editingProject->GetNbFrames(), editingProject->GetCredit(), editingProject->GetCoreId(), 0);
			AddProject(editedProject);
		}
	}
}
