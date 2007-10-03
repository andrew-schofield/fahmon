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

#ifndef _MAIN_H
#define _MAIN_H


#include <wx/wx.h>
#include <wx/ipc.h>
#include "wx/snglinst.h"
#include "mainDialog.h"
#include "tools.h"

const wxString IPC_START = _T("StartOther");

/**
 * This is the main application class, it contains the entry point
**/
class FahMonApp : public wxApp
{
friend class FahMonAppIPCConnection;
protected:
    wxSingleInstanceChecker *mInstanceChecker;
    wxLocale                 m_locale;
    wxServerBase            *mServerIPC;

public:
    virtual bool OnInit(void);
    virtual int OnExit(void);

private:
DECLARE_EVENT_TABLE()
    void OnEndSession( wxCloseEvent& event );
    void OnQueryEndSession( wxCloseEvent& event );
    //void TestEventHandler( wxFocusEvent& event );

};

class FahMonAppIPCConnection : public wxConnection {

public:
    //! application IPC connection
    FahMonAppIPCConnection(): wxConnection() {
    }

    //! execute handler
    virtual bool OnExecute (const wxString& WXUNUSED(topic), wxChar* WXUNUSED(WXdata), int WXUNUSED(size), wxIPCFormat WXUNUSED(format)) {
        MainDialog::GetInstance()->Show();
        MainDialog::GetInstance()->Raise();
        MainDialog::GetInstance()->TrayReloadSelectedClient();
        MainDialog::GetInstance()->Maximize(false);
        return true;
    }

private:
   // character buffer
   wxChar m_buffer[4096];

};

class FahMonAppIPCServer : public wxServer {

public:
    //! accept conncetion handler
    virtual wxConnectionBase *OnAcceptConnection (const wxString& topic) {
        if (topic != IPC_START) return NULL;
        return new FahMonAppIPCConnection;
    }
};


#endif /* _MAIN_H */
