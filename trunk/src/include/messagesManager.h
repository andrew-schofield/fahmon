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
 * \file messagesManager.h
 * Manages system messages.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _MESSAGESMANAGER_H
#define _MESSAGESMANAGER_H


#include "wx/thread.h"
#include "wx/string.h"


/**
 * Info message macro.
 * Logs an info message.
 * @param msg The message to log
 **/
#define _LogMsgInfo(msg)                                  \
{                                                         \
	MessagesManager::GetInstance()->LogInformation(msg);  \
}

/**
 * Warning message macro.
 * Logs a warning message.
 * @param msg The message to log
 **/
#define _LogMsgWarning(msg)                           \
{                                                     \
	MessagesManager::GetInstance()->LogWarning(msg);  \
}

/**
 * Error message macro.
 * Logs an error message.
 * @param msg The message to log
 **/
#define _LogMsgError(msg)                           \
{                                                   \
	MessagesManager::GetInstance()->LogError(msg);  \
}


/**
 * Message management class.
 * This provides a way to give information to the user about what the app does (and why).
 * This class can only be instantiated once.
 **/
class MessagesManager
{
protected:
	static wxMutex          mMutexLog; /**< Locks access to the log method */
	static MessagesManager *mInstance; /**< The single instance of the MessagesManager object */

	wxString mMessages; /**< Object storing all the logged messages */
	wxString mNewMessages; /**< Object storing all new logged messages */

	/**
	 * Contructor.
	 * Contructs the message manager.
	 **/
	MessagesManager(void);

	/**
	 * Destructor.
	 **/
	~MessagesManager(void);

	/**
	 * Log a new message.
	 * @param msg The message to log
	 **/
	void Log(const wxString& msg);


public:
	// Singleton pattern
	/**
	 * Create the instance of MessagesManager.
	 **/
	static void             CreateInstance(void);

	/**
	 * Destroy the single instance of the MessagesManager.
	 * Only if it exists.
	 **/
	static void             DestroyInstance(void);

	/**
	 * Retrieve the instance of MessagesManager.
	 * If an instance doesn't already exist, create it.
	 * @return An instance of class MessagesManager.
	 **/
	static MessagesManager* GetInstance(void);

	/**
	 * Retrieve messages from the log.
	 * @return The contents of the log.
	 **/
	wxString GetMessages(void) const {return mMessages;}

	/**
	 * Retrieve new messages from the log.
	 * @return The new messages from the log.
	 **/
	wxString GetNewMessages(void);

	/**
	 * Log an information message.
	 * @param msg The message to log.
	 **/
	void LogInformation(const wxString& msg) {Log(wxT("  ") + msg);}

	/**
	 * Log a warning message.
	 * @param msg The message to log.
	 **/
	void LogWarning(const wxString& msg)     {Log(wxT("! ") + msg);}

	/**
	 * Log an error message.
	 * @param msg The message to log.
	 **/
	void LogError(const wxString& msg)       {Log(wxT("X ") + msg);}
};


#endif /* _MESSAGESMANAGER_H */
