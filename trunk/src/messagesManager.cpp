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
 * \file messagesManager.cpp
 * Manages system messages.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "messagesManager.h"

#include "pathManager.h"
#include "mainDialog.h"
#include "tools.h"

#include "wx/datetime.h"
#include "wx/txtstrm.h"
#include "wx/textfile.h"
#include "wx/wfstream.h"
#include "wx/intl.h"

// This mutex is there to ensure that two threads won't try to log something at the same time
wxMutex MessagesManager::mMutexLog;

// The single instance of MessagesManager accross the application
MessagesManager* MessagesManager::mInstance = NULL;


MessagesManager::MessagesManager(void)
{
	mMessages = wxT("");
}


MessagesManager::~MessagesManager(void)
{
}


void MessagesManager::CreateInstance(void)
{
	wxASSERT(mInstance == NULL);

	mInstance = new MessagesManager();
}


void MessagesManager::DestroyInstance(void)
{
	wxASSERT(mInstance != NULL);

	delete mInstance;
	mInstance = NULL;
}


MessagesManager* MessagesManager::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}


void MessagesManager::Log(const wxString& msg)
{
	wxMutexLocker  mutexLocker(mMutexLog);        // --- Lock the access to this method
	wxString       currentDate;
	wxCommandEvent event(EVT_NEW_MESSAGE_LOGGED);
	wxFileOutputStream   fileOS(PathManager::GetMsgPath() + wxT("messages.log"));
	wxTextOutputStream   textOS(fileOS);

	// Format the current time correctly
	currentDate = wxDateTime::Now().Format(wxT("%d/%m/%y - %H:%M:%S"));

	// Add the new message
	mMessages += wxT("[") + currentDate + wxT("] ") + msg + wxT("\n");

	// Write a small header

	// Could the file be opened?
	if(fileOS.Ok() == false)
	{
		Tools::ErrorMsgBox(wxString::Format(_("Could not open file for writing!\nThe log will not be saved!")));
		return;
	}
	else
	{
		textOS.WriteString(mMessages);
		fileOS.Close();
	}

	// Warn the main dialog about this message
	// We can't directly call methods of MessagesFrame, we must use the main dialog to transfer the warning
	if(MainDialog::HasBeenInstanciated() == true)
	{
		MainDialog::GetInstance()->AddPendingEvent(event);
	}
}
