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
#include "clientsManager.h"

#include "tools.h"
#include "client.h"
#include "wx/txtstrm.h"
#include "mainDialog.h"
#include "wx/textfile.h"
#include "wx/wfstream.h"
#include "pathManager.h"
#include "messagesManager.h"
#include "clientHelperThread.h"


// The single instance of ClientsManager accross the application
ClientsManager* ClientsManager::mInstance = NULL;


/**
 * Constructor
**/
ClientsManager::ClientsManager(void)
{
}


/**
 * Destructor
**/
ClientsManager::~ClientsManager(void)
{
}


/**
 * Create the single instance of the ClientsManager
**/
void ClientsManager::CreateInstance(void)
{
    wxASSERT(mInstance == NULL);
	
    mInstance = new ClientsManager();
    mInstance->Load();
}


/**
 * Destroy the single instance of the ClientsManager
**/
void ClientsManager::DestroyInstance(void)
{
    wxASSERT(mInstance != NULL);

    mInstance->Save();

    delete mInstance;
    mInstance = NULL;
}


/**
 * Return the single instance of the ClientsManager
**/
ClientsManager* ClientsManager::GetInstance(void)
{
    wxASSERT(mInstance != NULL);

    return mInstance;
}


/**
 * Add a new client to the list, return the identifier of this new client
**/
wxUint32 ClientsManager::Add(const wxString& name, const wxString& location)
{
    mClients.Add(new Client(name, location));
    
    return mClients.GetCount()-1;
}


/**
 * Change the information associated with the given client
**/
void ClientsManager::Edit(wxUint32 clientId, const wxString& name, const wxString& location)
{
    mClients.Item(clientId)->SetName(name);
    mClients.Item(clientId)->SetLocation(location);
}


/**
 * Load the list of clients from the disk
**/
inline void ClientsManager::Load(void)
{
    bool       isNameOk;
    bool       isLocationOk;
    wxInt32    startingPos;
    wxInt32    endingPos;
    wxUint32   i;
    wxString   currentLine;
    wxString   clientName;
    wxString   clientLocation;
    wxString   inputFilename;
    wxTextFile in;


    // Try to open the file, check if it exists
    inputFilename = PathManager::GetCfgPath() + wxT(FMC_FILE_CLIENTS);
    if(wxFileExists(inputFilename) == false || in.Open(inputFilename) == false)
        return;

    // Retrieve each client, one by line
    for(i=0; i<in.GetLineCount(); ++i)
    {
        currentLine = in.GetLine(i);

        // Do not take into account empty lines or comments
        if(currentLine.Len() != 0 && currentLine.GetChar(0) != '#')
        {
            isNameOk     = false;
            isLocationOk = false;

            // First, extract the name
            // We can't tell Find() where to start, so the " char is replaced with something else
            // to not find the same position more than once
            startingPos = currentLine.Find('"');
            if(startingPos != -1)
            {
                currentLine.SetChar(startingPos, ' ');
                endingPos = currentLine.Find('"');
                if(endingPos != -1)
                {
                    currentLine.SetChar(endingPos, ' ');
                    clientName = currentLine.Mid(startingPos+1, endingPos-startingPos-1);
                    isNameOk   = true;
                }
            }

            // Then, the location
            startingPos = currentLine.Find('"');
            if(startingPos != -1)
            {
                currentLine.SetChar(startingPos, ' ');
                endingPos = currentLine.Find('"');
                if(endingPos != -1)
                {
                    clientLocation = currentLine.Mid(startingPos+1, endingPos-startingPos-1);
                    isLocationOk   = true;
                }
            }

            // Add the client to the list, or warn the user if something went wrong
            if(isNameOk == true && isLocationOk == true)
                Add(clientName, clientLocation);
            else
                _LogMsgError(wxString::Format(wxT("Error while parsing %s on line %u"), inputFilename.c_str(), i+1));
        }
    }

    in.Close();
}


/**
 * Save the list of clients to the disk
**/
inline void ClientsManager::Save(void)
{
    wxUint32             i;
    wxString             outString;
    const Client        *client;
    wxFileOutputStream   fileOS(PathManager::GetCfgPath() + wxT(FMC_FILE_CLIENTS));
    wxTextOutputStream   textOS(fileOS);

    // Could the file be opened?
    if(fileOS.Ok() == false)
    {
        Tools::ErrorMsgBox(wxString::Format(wxT("Could not open file <%s> for writing!\nThe list of clients will not be saved!"), (PathManager::GetCfgPath() + wxT(FMC_FILE_CLIENTS)).c_str()));
        return;
    }    
    
    // Write a small header
    textOS.WriteString(wxString::Format(wxT("# %s : contains the list of clients\n#\n# \"Name\"          \"Location\"\n\n"), (PathManager::GetCfgPath() + wxT(FMC_FILE_CLIENTS)).c_str()));
    
    // And then each client
    for(i=0; i<GetCount(); ++i)
    {
        client    = Get(i);
        textOS.WriteString(wxString::Format(wxT("\"%s\"    \"%s\"\n"), client->GetName().c_str(), client->GetLocation().c_str()));
    }
    
    fileOS.Close();
}


/**
 * Create threads to reload the given client(s) and return immediately
 * EVT_CLIENTRELOADED events (one by client) are sent to the MainDialog when the job is done
 * If clientId is equal to CM_LOADALL, all clients are reloaded
**/
void ClientsManager::ReloadThreaded(wxUint32 clientId)
{
    wxUint32 i;

    if(clientId != CM_LOADALL)
        new ClientHelperThread(clientId);
    else
    {
        for(i=0; i<GetCount(); ++i)
            new ClientHelperThread(i);
    }
}


/**
 * Reload the given client and return only when the reloading is done
**/
void ClientsManager::Reload(wxUint32 clientId)
{
    mClients.Item(clientId)->Reload();
}


/**
 * Find the name of the client located at the given location
 * Return an empty string if there is no known client at this location
**/
wxString ClientsManager::GetNameFromLocation(const wxString& location) const
{
    wxUint32 i;

    for(i=0; i<GetCount(); ++i)
        if(location.Cmp(mClients[i]->GetLocation()) == 0)
            return mClients[i]->GetName();

    return wxT("");
}
