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
 * \file clientsManager.h
 * Manages stored benchmarks.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _CLIENTSMANAGER_H
#define _CLIENTSMANAGER_H

#include "client.h"
#include "wx/dynarray.h"


#define CM_LOADALL FMC_INTMAX
#define CM_LOADALLF FMC_INTMAX2


WX_DEFINE_ARRAY_PTR(Client*, ArrayOfClients); /**< Definition of client array. */


/**
 * The client manager class.
 * This is the component which manages the set of clients to monitor.
 * It can only be instantiated once.
 **/
class ClientsManager
{
protected:
	static ClientsManager *mInstance; /**< The single instance of the Benchmarks Manager */

	ArrayOfClients mClients; /**< The array of managed clients */

	/**
	 * Contructor.
	 * Contructs the ClientDialog class.
	 **/
	ClientsManager(void);

	/**
	 * Destructor.
	 **/
	~ClientsManager(void);

	/**
	 * Load the list of clients from the disk.
	 **/
	void Load(void);


public:
	// Singleton pattern
	/**
	 * Create the instance of ClientsManager.
	 **/
	static void CreateInstance(void);

	/**
	 * Destroy the single instance of the ClientsManager.
	 * Only if it exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Retrieve the instance of ClientssManager.
	 * If an instance doesn't already exist, create it.
	 * @return An instance of class ClientsManager.
	 **/
	static ClientsManager* GetInstance(void);

	/**
	 * Create threads to reload the given client(s) and return immediately.
	 * EVT_CLIENTRELOADED events (one by client) are sent to the MainDialog when the job is done
	 * If clientId is equal to CM_LOADALL, all clients are reloaded.
	 * @param clientId Client(s) to reload.
	 **/
	void ReloadThreaded(wxUint32 clientId);

	/**
	 * Reload the given client and return only when the reloading is done.
	 * @param clientId Id of client to reload.
	 **/
	void Reload(wxUint32 clientId);

	/**
	 * Change the information associated with the given client.
	 * @param clientId The Id of the client to edit.
	 * @param name The edited name for the client.
	 * @param location The edited location for the client.
	 **/
	void Edit(wxUint32 clientId, wxString const &name, wxString const &location, bool VM);

	/**
	 * Deletes the given client.
	 * @param clientId The Id of the client to delete.
	 **/
	void Delete(wxUint32 clientId) {mClients.RemoveAt(clientId);}

	/**
	 * Add a new client to the list, return the identifier of this new client.
	 * @param name The client name.
	 * @param location The client location.
	 * @param enabled Is client enabled
	 * @return The clientId of the added client.
	 **/
	wxUint32 Add(wxString const &name, wxString const &location, bool enabled, bool VM);

	// --- Getters
	/**
	 * Gets number of clients in the array.
	 * @return Number of clients.
	 **/
	wxUint32 GetCount(void)                                const {return mClients.GetCount();}

	/**
	 * Find the name of the client located at the given location.
	 * Return an empty string if there is no known client at this location
	 * @param location Location to check
	 * @return Client name at location
	 **/
	const wxString GetNameFromLocation(wxString const &location) const;

	/**
	 * Get client info from clientId.
	 * @param clientId The id of the client to return.
	 * @return The client object for clientId
	 **/
	const Client* Get(wxUint32 clientId)                   const {return mClients.Item(clientId);}

	/**
	 * Enable or disable a given client
	 * @param clientId The id of the client to enable or disable
	 * @param value true=enable, false=disable
	 **/
	void Enable(wxUint32 clientId, bool value);

	/**
	 * Save the list of clients to the disk.
	 **/
	void Save(void);
};


#endif /* _CLIENTSMANAGER_H */
