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


ClientHelperThread::ClientHelperThread(wxUint32 clientId) : wxThread(wxTHREAD_DETACHED)
{
	mClientId = clientId;

	Create();
	Run();
}


void* ClientHelperThread::Entry(void)
{

	if(MainDialog::GetInstance()->ClientReloadAllowed(mClientId)) {
//		wxPuts(_T("Entry: CanReload"));
		wxCommandEvent event(EVT_CLIENTRELOADED);

		// We use the 'inlined' method to reload the client
		// (The job is done in the current execution flow)
		ClientsManager::GetInstance()->Reload(mClientId);

		// Post an event to the queue of the MainDialog to warn it that the job is done
		event.SetInt(mClientId);
		MainDialog::GetInstance()->AddPendingEvent(event);
	} else {
//		wxPuts(_T("Unable to reload the client... thread still running"));
	}

	// Dummy return code
	return NULL;
}
