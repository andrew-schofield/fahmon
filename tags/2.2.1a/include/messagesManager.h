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
 
#ifndef _MESSAGESMANAGER_H
#define _MESSAGESMANAGER_H


#include "wx/thread.h"
#include "wx/string.h"


// Macros for logging messages
#define _LogMsgInfo(msg)                                  \
{                                                         \
    MessagesManager::GetInstance()->LogInformation(msg);  \
}

#define _LogMsgWarning(msg)                           \
{                                                     \
    MessagesManager::GetInstance()->LogWarning(msg);  \
}

#define _LogMsgError(msg)                           \
{                                                   \
    MessagesManager::GetInstance()->LogError(msg);  \
}


// This provides a way to give information to the user about what the app does (and why)
// It's a singleton
class MessagesManager
{
protected:
    static wxMutex          mMutexLog;
    static MessagesManager *mInstance;

    wxString mMessages;

     MessagesManager(void);
    ~MessagesManager(void);

    void Log(const wxString& msg);


public:
    // Singleton pattern
    static void             CreateInstance(void);
	static void             DestroyInstance(void);
    static MessagesManager* GetInstance(void);

    wxString GetMessages(void) const {return mMessages;}

    void LogInformation(const wxString& msg) {Log(wxT("  ") + msg);}
    void LogWarning(const wxString& msg)     {Log(wxT("! ") + msg);}
    void LogError(const wxString& msg)       {Log(wxT("X ") + msg);}
};


#endif /* _MESSAGESMANAGER_H */
