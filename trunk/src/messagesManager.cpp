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
#include "preferencesManager.h"

#include "wx/datetime.h"
#include "wx/txtstrm.h"
#include "wx/textfile.h"
#include "wx/wfstream.h"
#include "wx/intl.h"

// This mutex is there to ensure that two threads won't try to log something at the same time
wxMutex MessagesManager::mMutexLog;
wxMutex MessagesManager::mMutexLogSave;

// The single instance of MessagesManager accross the application
MessagesManager* MessagesManager::mInstance = NULL;


MessagesManager::MessagesManager(void)
{
	mMessages = _T("");
	mNewMessages = _T("");
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
	mInstance->SaveMessages();
	wxASSERT(mInstance != NULL);

	delete mInstance;
	mInstance = NULL;
}


MessagesManager* MessagesManager::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}


void MessagesManager::Log(wxString const &msg)
{
	wxMutexLocker  lock(mMutexLog);        // --- Lock the access to this method
	wxString       currentDate, temp;
	wxCommandEvent event(EVT_NEW_MESSAGE_LOGGED);

	// Format the current time correctly
	currentDate = wxDateTime::UNow().Format(_T("%d/%m/%y - %H:%M:%S.%l"));
	temp = msg;
	temp.Trim();
	// Add the new message fixing any linebreaks that occur
	temp.Replace(wxT("\r\n"), _T("\n"), true);
	temp.Replace(wxT("\r"), _T("\n"), true);
	temp.Replace(wxT("\n"), _T("\n[") + currentDate + _T("]   "), true);
	mMessages += _T("[") + currentDate + _T("] ") + temp + _T("\n");
	mNewMessages += _T("[") + currentDate + _T("] ") + temp + _T("\n");

	// Warn the main dialog about this message
	// We can't directly call methods of MessagesFrame, we must use the main dialog to transfer the warning
	if(MainDialog::HasBeenInstanciated() == true)
	{
		MainDialog::GetInstance()->AddPendingEvent(event);
	}
}

wxString MessagesManager::GetNewMessages(void)
{
	wxString tmpString = mNewMessages;
	mNewMessages = _T("");
	return tmpString;
}


void MessagesManager::SaveMessages(void)
{
	wxMutexLocker  lock(mMutexLogSave);        // --- Lock the access to this method
	wxFileOutputStream   fileOS(PathManager::GetMsgPath() + _T("messages.log"));
	wxTextOutputStream   textOS(fileOS);

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
}


void MessagesManager::LogWarning(const wxString& msg, bool force)
{
	bool              logMessagesError;

	_PrefsGetBool        (PREF_MAINDIALOG_LOG_ERRORS_ONLY,                     logMessagesError);
	if(!logMessagesError || force)
		Log(wxT("! ") + msg);
}


void MessagesManager::LogInformation(const wxString& msg, bool force)
{
	bool              logMessagesError;

	_PrefsGetBool        (PREF_MAINDIALOG_LOG_ERRORS_ONLY,                     logMessagesError);
	if(!logMessagesError || force)
		Log(wxT("  ") + msg);
}
