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
 * \file main.h
 * Creates the main class.
 * This is the application insertion point and also handles the single instance system.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

/**
 * \mainpage FahMon
 * \section Overview
 * FahMon is an open-source tool (GPL license) that allows you to quickly check the
 * progress of your Folding\@Home client (or clients if you have multiple), avoiding
 * you having to open different files and/or to go to the Internet (for example to know
 * how much your current work unit is worth). Other monitoring tools exist (such as
 * Electron Microscope or FahLogStats), so if you don't like FahMon, have a look at them!
 *
 * FahMon is entirely coded in C++ and uses the wxWidgets library, which allows FahMon to
 * exist both for Linux and Windows. It is designed to be really easy to use, and you
 * should thus not encounter any major problem.
 * \section Features
 * - Information on many clients at a glance
 * - A local projects database is maintained to avoid using an Internet connection
 * - Estimated time of arrival for each client
 * - Work units benchmarking
 * - Exists for both Linux & Windows
 * \section Details
 * FahMon is a multi-platform application for monitoring the various Folding\@home clients.
 * It is capable of monitoring all the current Folding\@home clients with the only exception
 * being the PS3 client which can't export the data required by FahMon to monitor it.
 *
 * Monitoring multiple machines is easy with FahMon too. You can monitor clients that are
 * shared over a network, Windows users can just type in network share names by their UNC
 * path. Linux users can monitor any clients that they can mount folder from into their
 * filesystem. This includes NFS/SMB/WevDAV shares. No provision for monitoring over http
 * is possible at the moment.
 *
 * Multi-platform compatibility is acheived by using the wxWidgets library, which provides
 * a common programming API to the native APIs on multiple operating systems. In this way,
 * applications written using wxWidgets look like they were built natively for that
 * operating system.
 * At present FahMon is available as a Windows executable, a Mac OSX Universal binary (as
 * of version 2.3.2a) and also as a source package for compiling on Linux.
 * \section Legal
 * FahMon is Copyright François Ingelrest (Original Author/Retired Developer) 2003-2007
 * and Andrew Schofield (Lead Developer) 2007-2010
 *
 * FahMon is free software and released under the GNU Public Licence (GPL). As such you are
 * free to modify and distribute FahMon with the only condition being that you reference
 * the original source. (See the current docs).
 *
 * FahMon will always be free software, and will always have source that can be compiled on
 * at least Windows and Linux.
 **/

#ifndef _MAIN_H
#define _MAIN_H


#include <wx/wx.h>
#include <wx/ipc.h>
#include "wx/snglinst.h"
#include "mainDialog.h"
#include "tools.h"

const wxString IPC_START = _T("StartOther"); /**< The message to send between FahMon instances */

/**
* This is the main application class, it contains the entry point.
**/
class FahMonApp : public wxApp
{
friend class FahMonAppIPCConnection;
protected:
	wxSingleInstanceChecker *mInstanceChecker; /**< The single instance of the About Dialog */
	wxLocale                 m_locale; /**< The locale of the system */
	wxServerBase            *mServerIPC; /**< The IPC server (unix socket on Linux, DDE on Windows) */
	bool                     mLocal; /**< Should FahMon use local settings */
	bool                     mStress; /**< Should FahMon be run in stress-test mode */

public:
	/**
	 * This is the entry point of the application.
	 **/
	virtual bool OnInit(void);

    /**
	 * Called when a crash occurs in this application.
	 **/
	virtual void OnFatalException();

    /**
	 * This is where we really generate the debug report.
	 **/
	void GenerateReport(wxDebugReport::Context ctx);

    /**
	 * Upload the debug report.
	 * If this function is called, we'll use MyDebugReport which would try to
     * upload the (next) generated debug report to its URL, otherwise we just
     * generate the debug report and leave it in a local file
	 **/
	void UploadReport(bool doIt) { m_uploadReport = doIt; }

	/**
	 * Should FahMon run from a local directory only (no special config folder)
	 **/
	bool GetLocal(void) {return mLocal;}

	/**
	 * Should FahMon run in stress-test mode
	 **/
	bool GetStress(void) {return mStress;}

	/**
	 * This is called just before the application exits.
	 **/
	virtual int OnExit(void);

private:
DECLARE_EVENT_TABLE()

	bool m_uploadReport;

	/**
	 * Event: Catches shutdown/logoff events.
	 * This event allows FahMon to save settings and exit correctly when the system is logged off or shutdown.
	 * For some reason it doesn't work on Linux.
	 * @param event The actual event. This gets sent automatically.
	 **/
	void OnEndSession( wxCloseEvent& event );

	/**
	 * Event: Catches shutdown/logoff events.
	 * This event allows FahMon to save settings and exit correctly when the system is logged off or shutdown.
	 * For some reason it doesn't work on Linux.
	 * @param event The actual event. This gets sent automatically.
	 **/
	void OnQueryEndSession( wxCloseEvent& event );

	/**
	 * Event: Catches close events.
	 * This event allows FahMon to save settings and exit correctly when it is closed.
	 * @param event The actual event. This gets sent automatically.
	 **/
	void OnClose( wxCloseEvent& event);

};

/**
 * The IPC Connection client.
 * This intiates a connection to the IPC server, and allows the current instance to be raised.
 **/
class FahMonAppIPCConnection : public wxConnection {

public:
	/**
	 * The application IPC connection
	 **/
	FahMonAppIPCConnection(): wxConnection() {
	}

	/**
	 * Execute handler.
	 * This allows the current instance to be raised.
	 **/
	virtual bool OnExecute (const wxString& WXUNUSED(topic), wxChar* WXUNUSED(WXdata), int WXUNUSED(size), wxIPCFormat WXUNUSED(format)) {
		MainDialog::GetInstance()->Show();
		MainDialog::GetInstance()->Raise();
		MainDialog::GetInstance()->TrayReloadSelectedClient();
		MainDialog::GetInstance()->Maximize(false);
		return true;
	}

private:

wxChar m_buffer[4096]; /**< Character buffer used for the IPC message */

};

/**
 * The IPC Connection server.
 **/
class FahMonAppIPCServer : public wxServer {

public:
	/**
	 * Accept connection handler.
	 **/
	virtual wxConnectionBase *OnAcceptConnection (const wxString& topic) {
		if (topic != IPC_START) return NULL;
		return new FahMonAppIPCConnection;
	}
};


DECLARE_APP(FahMonApp);


#endif /* _MAIN_H */
