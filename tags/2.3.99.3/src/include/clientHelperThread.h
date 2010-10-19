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
 * \file clientHelperThread.h
 * Creates a thread launcher for the client reloader.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _CLIENTHELPERTHREAD_H
#define _CLIENTHELPERTHREAD_H


#include "wx/thread.h"


/**
* The client launcher thread.
* When instanciated, this class:
*  - Asks the ClientsManager to reload information on a given client
*  - Sends an EVT_CLIENTRELOADED event to the main dialog when done
*
* This class should only be used by the ClientsManager
**/
class ClientHelperThread : public wxThread
{
protected:
	wxUint32 mClientId; /**< The client id to reload */
	wxUint32 mType;

	/**
	 * This is the entry point of the thread, where the client is reloaded.
	 **/
	void* Entry(void);


public:
	/**
	 * Constructor.
	 * @param clientId The ID of the client to reload.
	 * @param type Local/inet or all
	 **/
	ClientHelperThread(wxUint32 clientId, wxUint32 type);
};


/**
 * The serial client launcher thread.
 * When instanciated, this class:
 *  - Asks the ClientsManager to reload information on a given client
 *  - Sends an EVT_CLIENTRELOADED event to the main dialog when done
 *
 * This class should only be used by the ClientsManager
 **/
class SerialClientHelperThread : public wxThread
{
protected:
	wxUint32 mClientCount; /**< Total number of clients */
	bool mForce; /**< Force reloading of client even if not necessary */
	wxUint32 mType;

	/**
	 * This is the entry point of the thread, where the client is reloaded.
	 **/
	void* Entry(void);


public:
	/**
	 * Constructor.
	 * @param clientCount Number of clients in list.
	 * @param force Force reloading of all clients.
	 * @param type Local/inet or all
	 **/
	SerialClientHelperThread(wxUint32 clientCount, bool force, wxUint32 type);
};


#endif /* _CLIENTHELPERTHREAD_H */
