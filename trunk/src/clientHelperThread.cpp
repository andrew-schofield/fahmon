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
 * \file clientHelperThread.cpp
 * Creates a thread launcher for the client reloader.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "clientHelperThread.h"
#include "fahmon.h"

#include "mainDialog.h"
#include "clientsManager.h"
#include "multiProtocolFile.h"
#include "messagesManager.h"


ClientHelperThread::ClientHelperThread(wxUint32 clientId, wxUint32 type) : wxThread(wxTHREAD_DETACHED)
{
	mClientId = clientId;
	mType = type;

	Create();
	Run();
}


void* ClientHelperThread::Entry(void)
{
	const Client* client;
	client = ClientsManager::GetInstance()->Get(mClientId);

	if(MainDialog::GetInstance()->ClientReloadAllowed(mClientId))
	{
		wxCommandEvent event(EVT_CLIENTRELOADED);
		// We use the 'inlined' method to reload the client
		// (The job is done in the current execution flow)
		ClientsManager::GetInstance()->Reload(mClientId);
		// Post an event to the queue of the MainDialog to warn it that the job is done
		event.SetInt(mClientId);
		MainDialog::GetInstance()->AddPendingEvent(event);
	}
	// Dummy return code
	return NULL;
}


SerialClientHelperThread::SerialClientHelperThread(wxUint32 clientCount, bool force, wxUint32 type) : wxThread(wxTHREAD_DETACHED)
{
	mClientCount = clientCount;
	mForce = force;
	mType = type;

	Create();
	Run();
}


void* SerialClientHelperThread::Entry(void)
{
	const Client* client;

	for(wxUint32 i = 0; i<mClientCount; ++i)
	{
		client = ClientsManager::GetInstance()->Get(i);

		if(!(mType == CM_LOADALL || mType == CM_LOADALLF || ((mType == CM_LOADLOCAL || mType == CM_LOADLOCALF) && multiProtocolFile::GetFileProtocol(client->GetLocation()) == multiProtocolFile::FILE) || ((mType == CM_LOADINET || mType == CM_LOADINETF) && (multiProtocolFile::GetFileProtocol(client->GetLocation()) == multiProtocolFile::FTP || multiProtocolFile::GetFileProtocol(client->GetLocation()) == multiProtocolFile::HTTP))))
			continue;
		if(mForce == false && !client->ReloadNeeded())
			continue;

		if(MainDialog::GetInstance()->ClientReloadAllowed(i))
		{
			wxCommandEvent event(EVT_CLIENTRELOADED);
			// We use the 'inlined' method to reload the client
			// (The job is done in the current execution flow)
			ClientsManager::GetInstance()->Reload(i);
			// Post an event to the queue of the MainDialog to warn it that the job is done
			event.SetInt(i);
			MainDialog::GetInstance()->AddPendingEvent(event);
		}

	}

	// Dummy return code
	return NULL;
}
