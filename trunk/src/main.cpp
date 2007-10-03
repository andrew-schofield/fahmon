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

#include "fahmon.h"
#include "main.h"

#include "tools.h"
#include "wx/image.h"
#include "mainDialog.h"
#include "clientsManager.h"
#include "firstTimeDialog.h"
#include "projectsManager.h"
#include "messagesManager.h"
#include "benchmarksManager.h"
#include "preferencesManager.h"

#ifdef _FAHMON_LINUX_
#include "locale.h"
#endif

// wxWidgets' way of creating a new application
IMPLEMENT_APP(FahMonApp)


/**
 * This is the entry point of the application
**/
bool FahMonApp::OnInit(void)
{
    // Check for another instance
    mInstanceChecker = new wxSingleInstanceChecker(wxT(FMC_UID));
    if(mInstanceChecker->IsAnotherRunning() == true)
    {
        Tools::ErrorMsgBox(wxString::Format(wxT("Another instance of %s is already running!"), wxT(FMC_APPNAME)));
        return false;
    }

    // We must explicitly tell to wxGTK which locale we want to use
    // Otherwise, floating point number conversion doesn't work on non-english Linux systems that do not use point as the decimal character
    // This is because the point is used as the decimal character on Stanford webpages and we can't change that
#ifdef _FAHMON_LINUX_
    setlocale(LC_NUMERIC, "C");
#endif

    // Miscellaneous initializations
    wxImage::AddHandler(new wxPNGHandler);      // We use only PNG images

    // Create mandatory singletons
    MessagesManager::CreateInstance();          // MUST be created first, so that other manager can log messages immediately
    PreferencesManager::CreateInstance();       // MUST be created second, so that other managers can retrieve some preferences when created
    ProjectsManager::CreateInstance();
    ClientsManager::CreateInstance();
    BenchmarksManager::CreateInstance();
    MainDialog::CreateInstance();               // MUST be created last, when all other managers have been created

    // Should we run the first-time dialog?
    if(wxDirExists(wxT(FMC_DIR_CONFIG)) == false)
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
    MainDialog::GetInstance()->Show(true);

    return true;
}


/**
 * This is called just before the application exits
**/
int FahMonApp::OnExit(void)
{
    // This frame is automatically destroyed by wxWidget, so we don't have to do it
    // MainDialog::DestroyInstance();

    BenchmarksManager::DestroyInstance();
    ClientsManager::DestroyInstance();
    ProjectsManager::DestroyInstance();
    PreferencesManager::DestroyInstance();        // MUST be destroyed last, so that other managers can save their preferences when they are destroyed
    MessagesManager::DestroyInstance();
    
    delete mInstanceChecker;
	
    return 0;
}
