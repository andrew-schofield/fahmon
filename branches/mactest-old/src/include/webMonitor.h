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

#ifndef _WEBMONITOR_H
#define _WEBMONITOR_H

#include "wx/thread.h"
#include "listViewClients.h"

/**
* Preferences used by this class
**/
#define PREF_WEBAPP_WEBAPP   wxT("WebMonitor.UseWebApp")
#define PREF_WEBAPP_WEBAPP_DV false

#define PREF_WEBAPP_SIMPLEWEB   wxT("WebMonitor.UseSimpleWeb")
#define PREF_WEBAPP_SIMPLEWEB_DV false

#define PREF_WEBAPP_SIMPLETEXT   wxT("WebMonitor.UseSimpleText")
#define PREF_WEBAPP_SIMPLETEXT_DV false

#define PREF_WEBAPP_WEBAPPLOCATION   wxT("WebMonitor.WebAppLocation")
#define PREF_WEBAPP_WEBAPPLOCATION_DV wxT("")

#define PREF_WEBAPP_SIMPLEWEBLOCATION   wxT("WebMonitor.SimpleWebLocation")
#define PREF_WEBAPP_SIMPLEWEBLOCATION_DV wxT("")

#define PREF_WEBAPP_SIMPLETEXTLOCATION   wxT("WebMonitor.SimpleTextLocation")
#define PREF_WEBAPP_SIMPLETEXTLOCATION_DV wxT("")

class WebMonitor
{
protected:
	static wxMutex     mMutexWebMonitorAccess;
	static WebMonitor* mInstance;
	ListViewClients  *mClientsList;

	WebMonitor(void);
	~WebMonitor(void);

public:
	static void CreateInstance(void);
	static void DestroyInstance(void);
	static WebMonitor* GetInstance(void);

	static wxString PadToLength(wxString, wxUint32);

	void WriteApp(void);
};

#endif /* _WEBMONITOR_H */
