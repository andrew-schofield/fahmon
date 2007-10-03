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

#ifndef _CLIENTSMANAGER_H
#define _CLIENTSMANAGER_H

#include "client.h"
#include "wx/dynarray.h"


#define CM_LOADALL FMC_INTMAX


WX_DEFINE_ARRAY_PTR(Client*, ArrayOfClients);


/**
 * This is the component which manages the set of clients to monitor
 * It's a singleton
**/
class ClientsManager
{
protected:
    static ClientsManager *mInstance;

    ArrayOfClients mClients;

     ClientsManager(void);
    ~ClientsManager(void);

    void Load(void);
    void Save(void);


public:
    // Singleton pattern
    static void CreateInstance(void);
    static void DestroyInstance(void);
    static ClientsManager* GetInstance(void);

    void ReloadThreaded(wxUint32 clientId);
    void Reload(wxUint32 clientId);

    void     Edit(wxUint32 clientId, const wxString& name, const wxString& location);
    void     Delete(wxUint32 clientId) {mClients.RemoveAt(clientId);}
    wxUint32 Add(const wxString& name, const wxString& location);
    
    // --- Getters
    wxUint32      GetCount(void)                                const {return mClients.GetCount();}
    wxString      GetNameFromLocation(const wxString& location) const;
    const Client* Get(wxUint32 clientId)                        const {return mClients.Item(clientId);}
};


#endif /* _CLIENTSMANAGER_H */
