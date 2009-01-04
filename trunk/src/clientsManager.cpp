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
 * \file clientsManager.cpp
 * Manages stored benchmarks.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "clientsManager.h"

#include "tools.h"
#include "client.h"
#include "mainDialog.h"
#include "pathManager.h"
#include "messagesManager.h"
#include "clientHelperThread.h"
#include "webMonitor.h"
#include "trayManager.h"
#include "preferencesManager.h"

#include "wx/txtstrm.h"
#include "wx/intl.h"
#include "wx/textfile.h"
#include "wx/wfstream.h"

// The single instance of ClientsManager accross the application
ClientsManager* ClientsManager::mInstance = NULL;


ClientsManager::ClientsManager(void)
{
}


ClientsManager::~ClientsManager(void)
{
}


void ClientsManager::CreateInstance(void)
{
	wxASSERT(mInstance == NULL);

	mInstance = new ClientsManager();
	mInstance->Load();
}


void ClientsManager::DestroyInstance(void)
{
	wxASSERT(mInstance != NULL);

	mInstance->Save();

	delete mInstance;
	mInstance = NULL;
}


ClientsManager* ClientsManager::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}


wxUint32 ClientsManager::Add(wxString const &name, wxString const &location, bool enabled, bool VM)
{
	mClients.Add(new Client(name, location, enabled, VM));

	Save();

	return mClients.GetCount()-1;
}


void ClientsManager::Edit(wxUint32 clientId, wxString const &name, wxString const &location, bool VM)
{
	mClients.Item(clientId)->SetName(name);
	mClients.Item(clientId)->SetLocation(location);
	mClients.Item(clientId)->SetVM(VM);

	Save();
}


void ClientsManager::Load(void)
{
	bool       isNameOk;
	bool       isLocationOk;
	bool       enabled;
	bool       VM;
	wxUint32   i, j;
	wxString   currentLine;
	wxString   clientName;
	wxString   clientLocation;
	wxString   clientEnabled;
	wxString   clientVM;
	wxString   inputFilename;
	wxTextFile in;
	std::vector<wxString> clientstabFields;


	// Try to open the file, check if it exists
	inputFilename = PathManager::GetCfgPath() + _T(FMC_FILE_CLIENTS);
	if(wxFileExists(inputFilename) == false || in.Open(inputFilename) == false)
	{
		return;
	}

	// Retrieve each client, one by line
	for(i=0; i<in.GetLineCount(); ++i)
	{
		currentLine = in.GetLine(i);

		// Do not take into account empty lines or comments
		if(currentLine.Len() != 0 && currentLine.GetChar(0) != '#')
		{
			clientstabFields.empty();
			isNameOk     = false;
			isLocationOk = false;
			enabled      = true;
			VM           = false;

			clientstabFields = Tools::SplitLineByDelim(currentLine, _T("\t"));
			// if size is 1 assume space delimited
			if(clientstabFields.size() == 1)
				clientstabFields = Tools::SplitLineByDelim(currentLine, _T("    "));
			for(j=clientstabFields.size(); j<=FMC_CLIENTSTAB_FIELDS; ++j)
			{
				clientstabFields.push_back(_T(""));
			}
			clientName = clientstabFields.at(0);
			clientLocation = clientstabFields.at(1);
			clientEnabled = clientstabFields.at(2);
			clientVM = clientstabFields.at(3);

			clientName.Replace(_T("\""), _T(""));
			clientLocation.Replace(_T("\""), _T(""));
			clientName.Trim();
			clientName.Trim(false);
			clientLocation.Trim();
			clientLocation.Trim(false);
			clientEnabled.Trim();
			clientVM.Trim();
			if(!clientName.IsEmpty())
				isNameOk = true;
			if(!clientLocation.IsEmpty())
				isLocationOk = true;
			if(clientEnabled.IsSameAs(_T("*")))
				enabled = false;
			if(clientVM.IsSameAs(_T("*")))
				VM = true;

			// Add the client to the list, or warn the user if something went wrong
			if(isNameOk == true && isLocationOk == true)
				Add(clientName, clientLocation, enabled, VM);
			else
				_LogMsgError(wxString::Format(_("Error while parsing %s on line %u"), inputFilename.c_str(), i+1));
		}
	}

	in.Close();
}


void ClientsManager::Save(void)
{
	wxUint32             i;
	wxString             outString;
	wxString             enabled, VM;
	const Client        *client;
	wxFileOutputStream   fileOS(PathManager::GetCfgPath() + _T(FMC_FILE_CLIENTS));
	wxTextOutputStream   textOS(fileOS);

	// Could the file be opened?
	if(fileOS.Ok() == false)
	{
		Tools::ErrorMsgBox(wxString::Format(_("Could not open file <%s> for writing!\nThe list of clients will not be saved!"), (PathManager::GetCfgPath() + _T(FMC_FILE_CLIENTS)).c_str()));
		return;
	}

	// Write a small header
	textOS.WriteString(wxString::Format(_("# %s : contains the list of clients\n#\n# \"Name\"\t\"Location\"\tDisabled(*)\n\n"), (PathManager::GetCfgPath() + _T(FMC_FILE_CLIENTS)).c_str()));

	// And then each client
	for(i=0; i<GetCount(); ++i)
	{
		client    = Get(i);
		if (!client->IsEnabled())
			enabled = _T("*");
		else
			enabled = _T("");
		if (client->IsVM())
			VM = _T("*");
		else
			VM = _T("");
		textOS.WriteString(wxString::Format(_T("\"%s\"\t\"%s\"\t%s\t%s\n"), client->GetName().c_str(), client->GetLocation().c_str(), enabled.c_str(), VM.c_str()));
	}

	fileOS.Close();
}


void ClientsManager::ReloadThreaded(wxUint32 clientId)
{
	wxUint32 i;
	bool nonThreaded;

	_PrefsGetBool(PREF_NON_THREADED_RELOAD, nonThreaded);

	if(clientId != CM_LOADALL && clientId != CM_LOADALLF)
	{
		new ClientHelperThread(clientId);
	}
	else
	{
		if(nonThreaded)
		{
			if(clientId == CM_LOADALL)
				new SerialClientHelperThread(GetCount(), false);
			if(clientId == CM_LOADALLF)
				new SerialClientHelperThread(GetCount(), true);
		}
		else
		{
			for(i=0; i<GetCount(); ++i)
			{
				if(clientId == CM_LOADALL)
				{
					if(mClients.Item(i)->ReloadNeeded() == true)
						new ClientHelperThread(i);
				}
				if(clientId == CM_LOADALLF)
				{
					new ClientHelperThread(i);
				}
			}
		}
	}
	WebMonitor::GetInstance()->WriteApp();
	TrayManager::GetInstance()->SetTooltip(_T(""));
}


void ClientsManager::Reload(wxUint32 clientId)
{
	if (mClients.Item(clientId)->IsEnabled())
		mClients.Item(clientId)->Reload();
}


const wxString ClientsManager::GetNameFromLocation(wxString const &location) const
{
	wxUint32 i;

	for(i=0; i<GetCount(); ++i)
	{
		if(location.Cmp(mClients[i]->GetLocation()) == 0)
		{
			return mClients[i]->GetName();
		}
	}

	return _T("");
}


void ClientsManager::Enable(wxUint32 clientId, bool value)
{
	mClients.Item(clientId)->Enable(value);
}
