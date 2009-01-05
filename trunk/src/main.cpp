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
 * \file main.cpp
 * Creates the main class.
 * This is the application insertion point and also handles the single instance system.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "main.h"

#include "tools.h"
#include "mainDialog.h"
#include "pathManager.h"
#include "clientsManager.h"
#include "firstTimeDialog.h"
#include "projectsManager.h"
#include "messagesManager.h"
#include "benchmarksManager.h"
#include "preferencesManager.h"
#include "webMonitor.h"
#include "trayManager.h"

#include "wx/image.h"
#include "wx/intl.h"
#include "wx/filename.h"
#include "wx/debugrpt.h"
#include "wx/cmdline.h"

#include "locale.h"

#if !wxUSE_DEBUGREPORT
#error "FahMon cannot be built without wxUSE_DEBUGREPORT"
#endif

#if !wxUSE_ON_FATAL_EXCEPTION
#error "FahMon cannot be built without wxUSE_ON_FATAL_EXCEPTION"
#endif

// wxWidgets' way of creating a new application
IMPLEMENT_APP(FahMonApp)

BEGIN_EVENT_TABLE(FahMonApp, wxApp)
	EVT_QUERY_END_SESSION (FahMonApp::OnQueryEndSession)
	EVT_END_SESSION (FahMonApp::OnEndSession)
	EVT_CLOSE (FahMonApp::OnClose)
END_EVENT_TABLE()


bool FahMonApp::OnInit(void)
{
	bool               startMinimised;
	bool               isTrayIconEnabled;
	bool               requiresFirstRunDialog;
	bool               startMaximised;

    // at the moment fahmon.net isn't equipped to auto-process the uploads
	m_uploadReport = false;

	mLocal = false;

    // call this to tell the library to call our OnFatalException()
	wxHandleFatalExceptions();

	wxCmdLineParser cmdline(FahMonApp::argc, FahMonApp::argv);

	cmdline.AddSwitch(wxT("v"), wxT("version"), wxT("Displays the current version number."));
#ifdef _FAHMON_WIN32_
	cmdline.AddSwitch(wxT("l"), wxT("local"), wxT("Use local directory for all settings."));
#endif
	cmdline.AddSwitch(wxT("h"), wxT("help"), wxT("Displays this information."));

	// Show help on --help or invalid commands
	if ( cmdline.Parse() ) {
		return false;
	} else if ( cmdline.Found(wxT("help")) ) {
		cmdline.Usage();
		return false;
	}

#ifdef _FAHMON_WIN32_
	mLocal = cmdline.Found(wxT("local"));
#endif
	if ( cmdline.Found(wxT("version")) )
	{
		printf("%s\n", FMC_PRODUCT);
		return false;
	}

	// Check for another instance
	#ifndef __WXMAC__
	mInstanceChecker = new wxSingleInstanceChecker(_T(FMC_UID));
	if(mInstanceChecker->IsAnotherRunning() == true)
	{
		wxClient client;
		wxConnectionBase *conn = client.MakeConnection (wxEmptyString, _T("/tmp/fahmon-ipc"), IPC_START);
		if (conn)
		{
			if (conn->Execute(_T("Show")))
			{
				delete mInstanceChecker;
				mInstanceChecker = NULL;
				return false;
			}
			return false;
		}
		delete conn;
		// Fallback error message for when auto-raising doesn't work.
		Tools::ErrorMsgBox(wxString::Format(_("Another instance of %s is already running!"), _T(FMC_APPNAME)));
		return false;
	}
	mServerIPC = new FahMonAppIPCServer();
	if (!mServerIPC->Create (_T("/tmp/fahmon-ipc")))
	{
		delete mServerIPC;
		mServerIPC = NULL;
		Tools::ErrorMsgBox(_T("Could create socket, auto-raising will not function!"));
	}
	#endif

	if ( !m_locale.Init(wxLANGUAGE_DEFAULT, wxLOCALE_CONV_ENCODING) )
	{
		Tools::ErrorMsgBox(_T("This language is not supported by the system."));
		return false;
	}

	wxString locale = m_locale.GetLocale();

#ifdef _FAHMON_WIN32_
	{
		wxLocale::AddCatalogLookupPathPrefix(_T("./lang"));
	}
#endif
#ifdef __WXGTK__
	{
		wxLocale::AddCatalogLookupPathPrefix(_T(DATADIR));
	}
#endif
#ifdef __WXMAC__
	{
		wxFileName appPath = wxFileName(wxTheApp->argv[0]).GetPath (wxPATH_GET_VOLUME);
		appPath.RemoveLastDir();

		wxString resourcesPath = appPath.GetPath();

		resourcesPath += _T("/Contents/SharedSupport/lang/");

		wxLocale::AddCatalogLookupPathPrefix(resourcesPath);
	}
#endif
	// Initialize the catalogs we'll be using
	m_locale.AddCatalog(_T("fahmon"));

	// this catalog is installed in standard location on Linux systems and
	// shows that you may make use of the standard message catalogs as well
	//
	// if it's not installed on your system, it is just silently ignored
#ifdef __WXGTK__
	{
		wxLogNull noLog;
		m_locale.AddCatalog(_T("fileutils"));
	}
#endif

	// We must explicitly tell to wxGTK which locale we want to use
	// Otherwise, floating point number conversion doesn't work on non-english Linux systems that do not use point as the decimal character
	// This is because the point is used as the decimal character on Stanford webpages and we can't change that
#ifdef __WXGTK__
	setlocale(LC_NUMERIC, "C");
#endif

	// Miscellaneous initializations
	wxImage::AddHandler(new wxPNGHandler);

	// Create the config directory right at the beginning so we can save messages.log there without it complaining about
	// not being able to create the file. Note: This means no messages can/should be logged prior to this point
	requiresFirstRunDialog = false;
#ifdef _FAHMON_WIN32_
	wxUint8 copied = 0;
#endif

	if(!wxDirExists(PathManager::GetCfgPath())) {
		requiresFirstRunDialog = true;
		// The following is necessary on OSX and Win32 as the wxMkDir function doesn't seem to be able to create folders with more
		// than one level at once. Therefore we need to create the "~/Library/Application Support/FahMon" folder first.
#if defined(__WXMAC__) || defined (_FAHMON_WIN32_)
		wxFileName topPath = PathManager::GetCfgPath();
		topPath.RemoveLastDir();
		if(!wxDirExists(topPath.GetPath()) && !wxMkdir(topPath.GetPath()))
		{
			Tools::ErrorMsgBox(wxString::Format(_("Could not create directory <%s>"), topPath.GetPath().c_str()));
			return false;
		}
#endif
		if(!wxMkdir(PathManager::GetCfgPath()))
		{
			Tools::ErrorMsgBox(wxString::Format(_("Could not create directory <%s>"), PathManager::GetCfgPath().c_str()));
			return false;
		}
#ifdef _FAHMON_WIN32_
		if(mLocal == false && wxDirExists(wxString::Format(_T("%s\\config\\"), wxFileName(wxTheApp->argv[0]).GetPath (wxPATH_GET_VOLUME))))
		{
			if(wxCopyFile(wxString::Format(_T("%s\\config\\benchmarks.dat"), wxFileName(wxTheApp->argv[0]).GetPath(wxPATH_GET_VOLUME)), wxString::Format(_T("%sbenchmarks.dat"), PathManager::GetCfgPath())))
				copied++;
			if(wxCopyFile(wxString::Format(_T("%s\\config\\clientstab.txt"), wxFileName(wxTheApp->argv[0]).GetPath(wxPATH_GET_VOLUME)), wxString::Format(_T("%sclientstab.txt"), PathManager::GetCfgPath())))
				copied++;
			if(wxCopyFile(wxString::Format(_T("%s\\config\\prefs.dat"), wxFileName(wxTheApp->argv[0]).GetPath(wxPATH_GET_VOLUME)), wxString::Format(_T("%sprefs.dat"), PathManager::GetCfgPath())))
				copied++;
			if(wxCopyFile(wxString::Format(_T("%s\\config\\projects.dat"), wxFileName(wxTheApp->argv[0]).GetPath(wxPATH_GET_VOLUME)), wxString::Format(_T("%sprojects.dat"), PathManager::GetCfgPath())))
				copied++;
			if(copied > 0)
				requiresFirstRunDialog = false;
		}
#endif
	}

	// Create mandatory singletons
	MessagesManager::CreateInstance();          // MUST be created first, so that other manager can log messages immediately
#ifdef _FAHMON_WIN32_
	if(copied > 0)
		_LogMsgInfo(wxString::Format(_("Old settings imported into %s"), PathManager::GetCfgPath()));
#endif
	PreferencesManager::CreateInstance();       // MUST be created second, so that other managers can retrieve some preferences when created
	ProjectsManager::CreateInstance();
	ClientsManager::CreateInstance();
	BenchmarksManager::CreateInstance();
	WebMonitor::CreateInstance();
	MainDialog::CreateInstance();               // MUST be created last, when all other managers have been created

	// Should we run the first-time dialog?
	if(requiresFirstRunDialog == true)
	{
		// If something goes wrong with the wizard, we quit immediately
		if(FirstTimeDialog::GetInstance()->ShowModal() == wxID_CANCEL)
		{
			delete mInstanceChecker;
			return false;
		}
		FirstTimeDialog::DestroyInstance();
	}

	// Let's start with the main dialog box
	_PrefsGetBool(PREF_MAINDIALOG_START_MINIMISED,  startMinimised);
	_PrefsGetBool(PREF_MAINDIALOG_ENABLE_TRAY_ICON, isTrayIconEnabled);
	_PrefsGetBool(PREF_MAINDIALOG_MAXIMISE, startMaximised);
	if (startMinimised != true)
	{
		MainDialog::GetInstance()->Show(true);
	}
	if (startMinimised == true && isTrayIconEnabled == false)
	{
		//show main window, then minimize it to taskbar
		MainDialog::GetInstance()->Show(true);
		MainDialog::GetInstance()->Iconize();
	}
	if (isTrayIconEnabled == true)
	{
		TrayManager::GetInstance()->SetMaximised(startMaximised);
	}
	return true;
}


int FahMonApp::OnExit(void)
{
	TrayManager::DestroyInstance();
	BenchmarksManager::DestroyInstance();
	ClientsManager::DestroyInstance();
	ProjectsManager::DestroyInstance();
	WebMonitor::DestroyInstance();
	PreferencesManager::DestroyInstance();
	MessagesManager::DestroyInstance();

#ifndef __WXMAC__
	if (mInstanceChecker)
		delete mInstanceChecker;
#endif
	if (mServerIPC)
		delete mServerIPC;

	return 0;
}


void FahMonApp::OnEndSession(wxCloseEvent& event)
{
	MainDialog::GetInstance()->Close();
}


void FahMonApp::OnQueryEndSession(wxCloseEvent& event)
{
	PreferencesManager::GetInstance()->Save();
	ClientsManager::GetInstance()->Save();
	ProjectsManager::GetInstance()->Save();
	BenchmarksManager::GetInstance()->Save();
	MainDialog::GetInstance()->Close();
}

void FahMonApp::OnClose(wxCloseEvent& event)
{
	MainDialog::GetInstance()->Close();
}



void FahMonApp::OnFatalException()
{
	GenerateReport(wxDebugReport::Context_Exception);
}

void FahMonApp::GenerateReport(wxDebugReport::Context ctx)
{
	wxDebugReportCompress *report = m_uploadReport ? new MyDebugReport
	: new wxDebugReportCompress;

    // add all standard files: currently this means just a minidump and an
    // XML file with system info and stack trace
	report->AddAll(ctx);

	wxFileName fn(report->GetDirectory(), _T("timestamp.my"));
	wxFFile file(fn.GetFullPath(), _T("w"));
	if ( file.IsOpened() )
	{
		wxDateTime dt = wxDateTime::Now();
		file.Write(dt.FormatISODate() + _T(' ') + dt.FormatISOTime());
		file.Close();
	}

	report->AddFile(fn.GetFullName(), _T("timestamp of this report"));

	report->AddFile(wxString::Format(_T("%s/clientstab.txt"),PathManager::GetCfgPath().c_str()), _T("Clients list"));
	report->AddFile(wxString::Format(_T("%s/prefs.dat"),PathManager::GetCfgPath().c_str()), _T("Preferences file"));

	if ( wxDebugReportPreviewStd().Show(*report) )
	{
		if ( report->Process() )
		{
			if ( m_uploadReport )
			{
				wxLogMessage(_T("Report successfully uploaded."));
			}
			else
			{
				Tools::InfoMsgBox(wxString::Format(_T("Report generated in \"%s\". A browser window will now open allowing you to submit this debug report."),
								  report->GetCompressedFileName().c_str()));
				Tools::OpenURLInBrowser(_T("http://fahmon.net/debug_report.php"));
				report->Reset();
			}
		}
	}

	delete report;
}
