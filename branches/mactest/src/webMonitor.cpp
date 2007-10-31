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
#include "webMonitor.h"

#include "wx/textfile.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "tools.h"
#include "pathManager.h"
#include "clientsManager.h"
#include "projectsManager.h"
#include "listViewClients.h"
#include "preferencesManager.h"
#include "mainDialog.h"
#include "fahmonConsts.h"
#include "messagesManager.h"

// The single instance of WebMonitor across the application
WebMonitor* WebMonitor::mInstance = NULL;

// Protects from multiple access from different threads
// Prevents it from being called multiple times whilst already running
wxMutex WebMonitor::mMutexWebMonitorAccess;

/**
* Constructor
**/
WebMonitor::WebMonitor(void)
{
}

/**
* Destructor
**/
WebMonitor::~WebMonitor(void)
{
}

/**
* Create the single instance of WebMonitor
**/
void WebMonitor::CreateInstance(void)
{
	wxASSERT(nInstance == NULL);

	mInstance = new WebMonitor();
}

/**
* Destroy the single instance of the WebMonitor
*/
void WebMonitor::DestroyInstance(void)
{
	wxASSERT(mInstance != NULL);

	delete mInstance;
	mInstance = NULL;
}

/**
* Return the single instance of WebMonitor
**/
WebMonitor* WebMonitor::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}

/**
* Write the webmonitor applet
**/
void WebMonitor::WriteApp(void)
{
	bool           useWebApp;
	bool           useSimpleWeb;
	bool           useSimpleText;
	bool           overrideTZ;
	bool           deadlineDays;
	wxString       webAppLocation;
	wxString       simpleWebLocation;
	wxString       simpleTextLocation;
	//wxString       dataArray[ClientsManager::GetInstance()->GetCount()][13];
	//for reference: 0progress, 1client name, 2ETA, 3PPD, 4corename, 5projectID, 6credit, 7username/team, 8downloaded, 9preferred, 10final, 11bgcolor, 12state
	wxString      ** dataArray;
	wxString       tempString;
	const Client  *client;
	const Project *project;
	wxInt32        timeInMinutes;
	wxUint32       currentClient, i;
	wxInt32        nbDays;
	wxInt32        nbHours;
	wxInt32        nbMinutes;
	wxInt32        TZ;
	wxDateTime     preferredDeadline;
	wxDateTime     finalDeadline;
	wxDateTime     downloadTime;
	wxDateTime     timeNow;
	wxTimeSpan     timeDiff;

	dataArray = new wxString *[ClientsManager::GetInstance()->GetCount()];
	for (i=0; i<ClientsManager::GetInstance()->GetCount(); i++)
	{
		dataArray[i] = new wxString[13];
	}

	_PrefsGetBool(PREF_OVERRIDE_TIMEZONE,           overrideTZ);
	_PrefsGetInt (PREF_TZ,                          TZ);
	_PrefsGetBool(PREF_MAINDIALOG_DEADLINE_DAYS,    deadlineDays);

	_PrefsGetBool(PREF_WEBAPP_WEBAPP,               useWebApp)
	_PrefsGetBool(PREF_WEBAPP_SIMPLEWEB,            useSimpleWeb)
	_PrefsGetBool(PREF_WEBAPP_SIMPLETEXT,           useSimpleText)
	_PrefsGetString(PREF_WEBAPP_WEBAPPLOCATION,     webAppLocation);
	_PrefsGetString(PREF_WEBAPP_SIMPLEWEBLOCATION,  simpleWebLocation);
	_PrefsGetString(PREF_WEBAPP_SIMPLETEXTLOCATION, simpleTextLocation);

	if(useWebApp == true || useSimpleWeb == true || useSimpleText == true)
	{
		// load up the array with data
		for(currentClient=0; currentClient<ClientsManager::GetInstance()->GetCount(); ++currentClient)
		{
			client      = ClientsManager::GetInstance()->Get(currentClient);
			if(!client->IsAccessible()) {
				dataArray[currentClient][4] = _("N/A");
				dataArray[currentClient][7] = _("N/A");
				dataArray[currentClient][5] = _("N/A");
				dataArray[currentClient][6] = _("N/A");
				dataArray[currentClient][8] = _("N/A");
				dataArray[currentClient][9] = _("N/A");
				dataArray[currentClient][10] = _("N/A");
			}
			else
			{
				dataArray[currentClient][7] = wxString::Format(_T("%s (%u)"), client->GetDonatorName().c_str(), client->GetTeamNumber());
				if(client->GetDownloadDate().IsValid())
				{
					if(overrideTZ)
					{
						downloadTime = client->GetDownloadDate().Add(wxTimeSpan::Hours(TZ));
						timeNow = wxDateTime::Now()/*.Add(wxTimeSpan::Hours(TZ))*/;
					}
					else
					{
						downloadTime = client->GetDownloadDate().FromTimezone(wxDateTime::UTC);
						timeNow = wxDateTime::Now()/*.FromTimezone(wxDateTime::UTC)*/;
					}
					if(deadlineDays == true)
					{
						timeDiff = timeNow.Subtract(downloadTime);
						timeInMinutes = timeDiff.GetMinutes();


					// Split the left time into days, hours and minutes
						nbDays    = timeInMinutes / (24 * 60);
						nbMinutes = timeInMinutes % (24 * 60);
						nbHours   = nbMinutes / 60;
						nbMinutes = nbMinutes % 60;
					// Use a friendly format
						if(nbDays != 0)
							tempString = wxString::Format(_T("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
						else if(nbHours != 0)
							tempString = wxString::Format(_T("%ih %02imn"), nbHours, nbMinutes);
						else
							tempString = wxString::Format(_T("%imn"), nbMinutes);

						dataArray[currentClient][8] = wxString::Format(_("%s ago"), tempString.c_str());
					}
					else
					{
						dataArray[currentClient][8] = wxString::Format(_T("%s"), downloadTime.Format(wxT(FMC_DATE_MAIN_FORMAT)).c_str());
					}
				}
				if(client->GetProjectId() == INVALID_PROJECT_ID)
				{
					dataArray[currentClient][5] = _("N/A");
					dataArray[currentClient][4] = _("N/A");
					dataArray[currentClient][6] = _("N/A");
					dataArray[currentClient][9] = _("N/A");
					dataArray[currentClient][10] = _("N/A");
				}
				else
				{
					dataArray[currentClient][5] = wxString::Format(_T("%u (R%i, C%i, G%i)"), client->GetProjectId(), client->GetProjectRun(), client->GetProjectClone(), client->GetProjectGen());
					project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());
					if(project == NULL) // this may need to be INVALID_PROJECT
					{
						dataArray[currentClient][5] = _("N/A");
						dataArray[currentClient][4] = _("N/A");
						dataArray[currentClient][6] = _("N/A");
						dataArray[currentClient][9] = _("N/A");
						dataArray[currentClient][10] = _("N/A");
					} else {
						dataArray[currentClient][6] = wxString::Format(_("%u points"), project->GetCredit());
						dataArray[currentClient][4] = Core::IdToLongName(project->GetCoreId());
						if(client->GetDownloadDate().IsValid() && project->GetPreferredDeadlineInDays() != 0)
						{
							preferredDeadline = downloadTime;
							preferredDeadline.Add(wxTimeSpan::Seconds(project->GetPreferredDeadlineInDays() * 864));
							if(deadlineDays == true)
							{
								timeDiff = preferredDeadline.Subtract(timeNow);
								timeInMinutes = timeDiff.GetMinutes();
								if(timeDiff.GetMinutes() < 0) timeInMinutes = 0 - timeInMinutes;

							// Split the left time into days, hours and minutes
								nbDays    = timeInMinutes / (24 * 60);
								nbMinutes = timeInMinutes % (24 * 60);
								nbHours   = nbMinutes / 60;
								nbMinutes = nbMinutes % 60;
							// Use a friendly format
								if(nbDays != 0)
									tempString = wxString::Format(_T("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
								else if(nbHours != 0)
									tempString = wxString::Format(_T("%ih %02imn"), nbHours, nbMinutes);
								else
									tempString = wxString::Format(_T("%imn"), nbMinutes);

								if(timeDiff.GetMinutes() < 0)
									dataArray[currentClient][9] = wxString::Format(_("%s ago"), tempString.c_str());
								else
									dataArray[currentClient][9] = wxString::Format(_("In %s"), tempString.c_str());

							}
							else
							{
								dataArray[currentClient][9] = wxString::Format(_T("%s"), preferredDeadline.Format(wxT(FMC_DATE_MAIN_FORMAT)).c_str());
							}
						}
						else
							dataArray[currentClient][9] = _("N/A");

					// Final deadline: if it is equal to 0 day, there is no final deadline
						if(client->GetDownloadDate().IsValid() && project->GetFinalDeadlineInDays() != 0)
						{
							finalDeadline = downloadTime;
							finalDeadline.Add(wxTimeSpan::Seconds(project->GetFinalDeadlineInDays() * 864));
							if(deadlineDays == true)
							{
								timeDiff = finalDeadline.Subtract(timeNow);
								timeInMinutes = timeDiff.GetMinutes();
								if(timeDiff.GetMinutes() < 0) timeInMinutes = 0 - timeInMinutes;


							// Split the left time into days, hours and minutes
								nbDays    = timeInMinutes / (24 * 60);
								nbMinutes = timeInMinutes % (24 * 60);
								nbHours   = nbMinutes / 60;
								nbMinutes = nbMinutes % 60;
							// Use a friendly format
								if(nbDays != 0)
									tempString = wxString::Format(_T("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
								else if(nbHours != 0)
									tempString = wxString::Format(_T("%ih %02imn"), nbHours, nbMinutes);
								else
									tempString = wxString::Format(_T("%imn"), nbMinutes);

								if(timeDiff.GetMinutes() < 0)
									dataArray[currentClient][10] = wxString::Format(_("%s ago"), tempString.c_str());
								else
									dataArray[currentClient][10] = wxString::Format(_("In %s"), tempString.c_str());
							}
							else
							{
								dataArray[currentClient][10] = wxString::Format(_T("%s"), finalDeadline.Format(wxT(FMC_DATE_MAIN_FORMAT)).c_str());
							}
						}
						else
							dataArray[currentClient][10] = _("N/A");
					}
				}
			}

			if(!client->IsAccessible())
			{
				dataArray[currentClient][12] = _("Inaccessible");
				dataArray[currentClient][11] = _T("#666666");
			}
			else if(client->IsStopped())
			{
				dataArray[currentClient][12] = _("Stopped");
				dataArray[currentClient][11] = _T("#FFAAAA");
			}
			else if(client->IsInactive())
			{
				dataArray[currentClient][12] = _("Inactive");
				dataArray[currentClient][11] = _T("#FFFFAA");
			}
			else if(client->IsHung())
			{
				dataArray[currentClient][12] = _("Hung");
				dataArray[currentClient][11] = _T("#FFAAAA");
			}
			else if(client->IsAsynch())
			{
				dataArray[currentClient][12] = _("Async");
				dataArray[currentClient][11] = _T("#AAAAFF");
			}
			else
			{
				dataArray[currentClient][12] = _("Ok");
				dataArray[currentClient][11] = _T("#AAFFAA");
			}

			dataArray[currentClient][0] = client->GetProgressString();
			dataArray[currentClient][1] = client->GetName();


			if(client->GetProgress() == 100)                        dataArray[currentClient][2] = _("Finished");
			else if(!client->IsAccessible() || client->IsStopped()) dataArray[currentClient][2] = _("N/A");
			else if(!client->GetETA()->IsOk())                      dataArray[currentClient][2] = _("N/A");
			else if(client->IsHung())                               dataArray[currentClient][2] = _("*Hung*");
			else                                                    dataArray[currentClient][2] = client->GetETA()->GetString().c_str();

			dataArray[currentClient][3] = _T("--");

			project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());

		// If it's possible to get the PPD, do so now
			if(client->IsAccessible() && !client->IsStopped() && !client->IsHung() && project != INVALID_PROJECT_ID)
			{
				dataArray[currentClient][3] = wxString::Format(_T("%.2f"), client->GetPPD());
			}
		}
	}

	if(useWebApp == true)
	{
		wxFileOutputStream   fileOS(webAppLocation);
		wxTextOutputStream   textOS(fileOS);

		// Could the file be opened?
		if(fileOS.Ok() == false)
		{
			_LogMsgError(wxString::Format(_("Could not open file <%s> for writing!\nThe web application will not be created!"), webAppLocation.c_str()));
			return;
		}

		// Write out the web header
		textOS.WriteString(_T("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n<head>\n\t<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\n\t<meta http-equiv=\"Pragma\" content=\"no-cache\" />\n\t<meta http-equiv=\"refresh\" content=\"100\" />\n"));
		textOS.WriteString(wxString::Format(_T("\t<title>%s</title>\n<script type=\"text/javascript\">\n"), wxT(FMC_PRODUCT)));
		textOS.WriteString(_T("<!--\n"));

		// Write out the wuinfo panel variable data
		textOS.WriteString(_T("pdata = new Array("));
		for(currentClient=0; currentClient<ClientsManager::GetInstance()->GetCount(); ++currentClient)
		{
			textOS.WriteString(wxString::Format(_T("['%s','%s','%s','%s','%s','%s','%s'],"), dataArray[currentClient][4].c_str(), dataArray[currentClient][5].c_str(), dataArray[currentClient][6].c_str(), dataArray[currentClient][7].c_str(), dataArray[currentClient][8].c_str(), dataArray[currentClient][9].c_str(), dataArray[currentClient][10].c_str()));
		}
		textOS.WriteString(_T("[]);\n"));

		// Write out the scripting
		textOS.WriteString(_T("function removeClassName (elem, className) {\n\telem.className = elem.className.replace(className, \"\").trim();\n}\n\nfunction addCSSClass (elem, className) {\n\tremoveClassName (elem, className);\n\telem.className = (elem.className + \" \" + className).trim();\n}\n\nString.prototype.trim = function() {\n\treturn this.replace( /^\\s+|\\s+$/, \"\" );\n}\n\nfunction ChangeColor(tableRow, highLight){\n\tif (highLight){\n\t\tvar objbranch\n\t\tstripedTable();\n\t\tremoveClassName(tableRow, 'alternateRow');\n\t\tremoveClassName(tableRow, 'normalRow');\n\t\taddCSSClass(tableRow, 'clickedRow');\n\t\tobjbranch = document.getElementById('core');\n\t\tobjbranch.innerHTML = pdata[tableRow.id - 1][0];\n\t\tobjbranch = document.getElementById('project');\n\t\tobjbranch.innerHTML = pdata[tableRow.id - 1][1];\n\t\tobjbranch = document.getElementById('credit');\n\t\tobjbranch.innerHTML = pdata[tableRow.id - 1][2];\n\t\tobjbranch = document.getElementById('username');\n\t\tobjbranch.innerHTML = pdata[tableRow.id - 1][3];\n\t\tobjbranch = document.getElementById('downloaded');\n\t\tobjbranch.innerHTML = pdata[tableRow.id - 1][4];\n\t\tobjbranch = document.getElementById('preferred');\n\t\tobjbranch.innerHTML = pdata[tableRow.id - 1][5];\n\t\tobjbranch = document.getElementById('final');\n\t\tobjbranch.innerHTML = pdata[tableRow.id - 1][6];\n\t}\n}\n\nfunction stripedTable() {\n\tif (document.getElementById && document.getElementsByTagName) {\n\t\tvar allTables = document.getElementsByTagName('table');\n\t\tif (!allTables) { return; }\n\n\t\tfor (var i = 0; i < allTables.length; i++) {\n\t\t\tif (allTables[i].className.match(/[\\w\\s ]*scrollTable[\\w\\s ]*/)) {\n\t\t\t\tvar trs = allTables[i].getElementsByTagName(\"tr\");\n\t\t\t\tfor (var j = 0; j < trs.length; j++) {\n\t\t\t\t\tremoveClassName(trs[j], 'alternateRow');\n\t\t\t\t\tremoveClassName(trs[j], 'clickedRow');\n\t\t\t\t\taddCSSClass(trs[j], 'normalRow');\n\t\t\t\t}\n\t\t\t\tfor (var k = 0; k < trs.length; k += 2) {\n\t\t\t\t\tremoveClassName(trs[k], 'normalRow');\n\t\t\t\t\tremoveClassName(trs[k], 'clickedRow');\n\t\t\t\t\taddCSSClass(trs[k], 'alternateRow');\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t}\n}\n\nfunction addIEonScroll() {\n\tvar thisContainer = document.getElementById('tableContainer');\n\tif (!thisContainer) { return; }\n}\n\nwindow.onload = function() { stripedTable(); addIEonScroll(); }\n-->\n</script>\n"));

		// Write out the CSS
		textOS.WriteString(_T("<style type=\"text/css\">\n<!--\ntable, td, a {\n\tcolor: #000;\n\tfont: normal normal 12px Verdana, Geneva, Arial, Helvetica, sans-serif\n}\n\nh1 {\n\tfont: normal normal 18px Verdana, Geneva, Arial, Helvetica, sans-serif;\n\tmargin: 0 0 5px 0\n}\n\nh2 {\n\tfont: normal normal 16px Verdana, Geneva, Arial, Helvetica, sans-serif;\n\tmargin: 0 0 5px 0\n}\n\nh3 {\n\tfont: normal normal 13px Verdana, Geneva, Arial, Helvetica, sans-serif;\n\tcolor: #008000;\n\tmargin: 0 0 15px 0\n}\ndiv.tableContainer {\n\tclear: both;\n\tborder: 1px solid #d1c8bf;\n\theight: 223px;\n\toverflow: auto;\n\twidth: 466px;\n\tfloat:left;\n}\n\\html div.tableContainer/* */ {\n\tpadding: 0 16px 0 0;\n\twidth: 450px;\n}\nhtml>body div.tableContainer {\n\theight: auto;\n\tpadding: 0;\n}\nhead:first-child+body div[class].tableContainer {\n\theight: 223px;\n\toverflow: hidden;\n\twidth: 466px\n}\ndiv.tableContainer table {\n\tfloat: left;\n\twidth: 100%\n}\n\\html div.tableContainer table/* */ {\n\tmargin: 0 -16px 0 0\n}\nhtml>body div.tableContainer table {\n\tfloat: none;\n\tmargin: 0;\n\twidth: 450px\n}\nhead:first-child+body div[class].tableContainer table {\n\twidth: 466px\n}\nthead.fixedHeader tr {\n\tposition: relative;\n\ttop: expression(document.getElementById(\"tableContainer\").scrollTop);\n}\nhead:first-child+body thead[class].fixedHeader tr {\n\tdisplay: block;\n}\nthead.fixedHeader th {\n\tbackground: #eeeae6;\n\tfont-weight: normal;\n\tpadding: 4px 3px;\n\ttext-align: left\n}\nthead.fixedHeader a, thead.fixedHeader a:link, thead.fixedHeader a:visited {\n\tcolor: #FFF;\n\tdisplay: block;\n\ttext-decoration: none;\n\twidth: 100%\n}\nthead.fixedHeader a:hover {\n\tcolor: #FFF;\n\tdisplay: block;\n\ttext-decoration: underline;\n\twidth: 100%\n}\nhead:first-child+body tbody[class].scrollContent {\n\tdisplay: block;\n\theight: 200px;\n\toverflow: auto;\n\twidth: 100%\n}\ntbody.scrollContent td, tbody.scrollContent tr.normalRow td {\n\tbackground: #FFF;\n\tpadding: 2px 3px 3px 4px\n}\ntbody.scrollContent tr.alternateRow td {\n\tbackground: #efebe7;\n\tpadding: 2px 3px 3px 4px\n}\ntbody.scrollContent tr.clickedRow td {\n\tbackground: #3858a5;\n\tpadding: 2px 3px 3px 4px\n}\nhead:first-child+body thead[class].fixedHeader th {\n\twidth: 30px\n}\nhead:first-child+body thead[class].fixedHeader th + th {\n\twidth: 60px\n}\nhead:first-child+body thead[class].fixedHeader th + th +th {\n\twidth: 150px\n}\nhead:first-child+body thead[class].fixedHeader th + th +th +th {\n\twidth: 150px\n}\nhead:first-child+body thead[class].fixedHeader th + th + th + th + th {\n\tborder-right: none;\n\tpadding: 4px 4px 4px 3px;\n\twidth: 76px\n}\nhead:first-child+body tbody[class].scrollContent td {\n\twidth: 30px\n}\nhead:first-child+body tbody[class].scrollContent td + td {\n\twidth: 60px\n}\nhead:first-child+body tbody[class].scrollContent td + td + td {\n\twidth: 150px\n}\nhead:first-child+body tbody[class].scrollContent td + td + td + td {\n\twidth: 150px\n}\nhead:first-child+body tbody[class].scrollContent td + td + td + td + td {\n\tborder-right: none;\n\tpadding: 2px 4px 2px 3px;\n\twidth: 60px\n}\nbody {\n\tfont-family:verdana, arial, sans-serif;\n\tfont-size:8pt;\n\tline-height:12pt;\n\tbackground:#FFFFFF;\n\tcolor:#333333;\n\tmargin-top:10px;\n}\n#wuinfopanel {\n\tfloat:left;\n\tpadding-left:2px;\n\theight: 224px;\n\twidth: 300px;\n}\n#fahmon {\n\twidth: 774px;\n\tmargin: auto;\n\theight: 245px;\n\tbackground: #efebe7;\n\tborder: 2px solid #3858a5;\n}\nTD {\n\tmargin-left:1px;\n\tmargin-right:1px;\n\tpadding:2px;\n}\n//-->\n</style>\n</head>\n"));

		// The actual visible bits
		textOS.WriteString(_T("<body>\n<div id=\"fahmon\">\n"));
		textOS.WriteString(_T("<table style=\"border-collapse:collapse;width:774px;\"><tr bgcolor=\"#3858a5\"><td width=\"18px\"><img src=\"dialog_icon.png\" /></td><td><b><font color=\"#FFFFFF\">"));
		textOS.WriteString(wxString::Format(_("%s Web View - Last updated: %s"), wxT(FMC_PRODUCT), wxDateTime::Now().Format(wxT(FMC_DATE_MAIN_FORMAT)).c_str()));
		textOS.WriteString(_T("</font></b></td><td align=\"right\"><b><font color=\"#FFFFFF\">"));
		textOS.WriteString(wxString::Format(_("Total PPD: %.2f"), MainDialog::GetInstance()->GetTotalPPD()));
		textOS.WriteString(_T("</font></b></td><td align=\"right\" onclick=\"javascript:window.close();\"><b><font color=\"#FFFFFF\">X</font></b></td></tr></table>\n"));
		textOS.WriteString(wxString::Format(_T("<div id=\"tableContainer\" class=\"tableContainer\">\n<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"100%\" class=\"scrollTable\">\n<thead class=\"fixedHeader\" id=\"fixedHeader\">\n\t<tr>\n<th></th>\n<th><u>%s</u></th>\n<th><u>%s</u></th>\n<th><u>%s</u></th>\n<th><u>%s</u></th>\n\t</tr>\n</thead>\n<tbody class=\"scrollContent\">\n"), _("Progress"), _("Name"), _("ETA"), _("PPD") ));

		// The client data
		for(currentClient=0; currentClient<ClientsManager::GetInstance()->GetCount(); ++currentClient)
		{
			textOS.WriteString(wxString::Format(_T("<tr onclick=\"ChangeColor(this, true);\" id=\"%u\">\n"), currentClient + 1));
			if(dataArray[currentClient][12] == _("Inaccessible"))
			{
				textOS.WriteString(_T("<td><img src=\"list_client_inaccessible.png\" /></td>\n"));
			}
			else if(dataArray[currentClient][12] == _("Stopped"))
			{
				textOS.WriteString(_T("<td><img src=\"list_client_stopped.png\" /></td>\n"));
			}
			else if(dataArray[currentClient][12] == _("Inactive"))
			{
				textOS.WriteString(_T("<td><img src=\"list_client_inactive.png\" /></td>\n"));
			}
			else if(dataArray[currentClient][12] == _("Hung"))
			{
				textOS.WriteString(_T("<td><img src=\"list_client_stopped.png\" /></td>\n"));
			}
			else if(dataArray[currentClient][12] == _("Async"))
			{
				textOS.WriteString(_T("<td><img src=\"list_client_asynch.png\" /></td>\n"));
			}
			else
			{
				textOS.WriteString(_T("<td><img src=\"list_client_ok.png\" /></td>\n"));
			}

			textOS.WriteString(wxString::Format(_T("<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n</tr>\n"), dataArray[currentClient][0].c_str(), dataArray[currentClient][1].c_str(), dataArray[currentClient][2].c_str(), dataArray[currentClient][3].c_str()));
		}

		// The Wuinfo panel, static portions
		textOS.WriteString(wxString::Format(_T("</tbody>\n</table>\n</div>\n<div id=\"wuinfopanel\">\n<table style=\"border-collapse: collapse;border:1px solid #d1c8bf;background:#efebe7;margin:1px;height:100%%;width:100%%;\">\n<tr>\n<td colspan=\"2\">%s</td>\n</tr>\n<tr>\n<td align=\"right\" width=\"50%%\"><b>%s</b></td>\n<td align=\"left\"><b><font color=\"#F00\" id=\"core\"></font></b></td>\n</tr>\n<tr>\n<td align=\"right\"><b>%s</b></td>\n<td align=\"left\" id=\"project\"></td>\n</tr>\n<tr>\n<td align=\"right\"><b>%s</b></td>\n<td align=\"left\" id=\"credit\"></td>\n</tr>\n<tr>\n<td align=\"right\"><b>%s</b></td>\n<td align=\"left\"><font color=\"#00F\" id=\"username\"></font></td>\n</tr>\n<tr>\n<td align=\"right\"><b>%s</b></td>\n<td align=\"left\" id=\"downloaded\"></td>\n</tr>\n<tr>\n<td align=\"right\"><b>%s</b></td>\n<td align=\"left\" id=\"preferred\"></td>\n</tr>\n<tr>\n<td align=\"right\"><b>%s</b></td>\n<td align=\"left\" id=\"final\"></td>\n</tr>\n</table>\n</div>\n</div>\n</body>\n</html>\n"), _("Work Unit Information"), _("Core:"), _("Project:"), _("Credit:"), _("Username:"), _("Downloaded:"), _("Preferred Deadline:"), _("Final Deadline:")));


		fileOS.Close();
	}
	if(useSimpleWeb == true)
	{
		//WriteSimpleWeb(simpleWebLocation);
		wxFileOutputStream   fileOS(simpleWebLocation);
		wxTextOutputStream   textOS(fileOS);

		// Could the file be opened?
		if(fileOS.Ok() == false)
		{
			_LogMsgError(wxString::Format(_("Could not open file <%s> for writing!\nThe simple web output will not be created!"), simpleWebLocation.c_str()));
			return;
		}

		// Write out the web header
		textOS.WriteString(_T("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n<head>\n\t<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\n\t<meta http-equiv=\"Pragma\" content=\"no-cache\" />\n\t<meta http-equiv=\"refresh\" content=\"100\" />\n"));
		textOS.WriteString(wxString::Format(_T("\t<title>%s</title>\n"), wxT(FMC_PRODUCT)));
		textOS.WriteString(wxString::Format(_T("<style type=\"text/css\">\n<!--\nTD {\npadding:2px;\nborder:1px solid #000;\n}\n//-->\n</style>\n</head>\n<body>\n<pre>\n<table style=\"border-collapse:collapse;border: 1px solid #000;\">\n<tr bgcolor=\"#eeeeee\">\n<td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n"), _("State"), _("Progress"), _("Name"), _("ETA"), _("PPD"), _("PRCG"), _("Credit"), _("Downloaded")));

		// Iterate through the clients and print data
		for(currentClient=0; currentClient<ClientsManager::GetInstance()->GetCount(); ++currentClient)
		{
			textOS.WriteString(wxString::Format(_T("<tr><td bgcolor=\"%s\">%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n"), dataArray[currentClient][11].c_str(), dataArray[currentClient][12].c_str(), dataArray[currentClient][0].c_str(), dataArray[currentClient][1].c_str(), dataArray[currentClient][2].c_str(), dataArray[currentClient][3].c_str(), dataArray[currentClient][5].c_str(), dataArray[currentClient][6].c_str(), dataArray[currentClient][8].c_str()));
		}

		// Write out the web footer
		textOS.WriteString(_T("</table>\n</pre>\n</body>\n</html>\n"));

		fileOS.Close();
	}
	if(useSimpleText == true)
	{
		//WriteSimpleWeb(simpleWebLocation);
		wxFileOutputStream   fileOS(simpleTextLocation);
		wxTextOutputStream   textOS(fileOS);

		// Could the file be opened?
		if(fileOS.Ok() == false)
		{
			_LogMsgError(wxString::Format(_("Could not open file <%s> for writing!\nThe simple text output will not be created!"), simpleTextLocation.c_str()));
			return;
		}
		textOS.WriteString(wxString::Format(_("FahMon Client Monitoring %s - Simple Text Output\n\n"), wxT(FMC_PRODUCT)));
		textOS.WriteString(wxString::Format(_T("%s|%s|%s|%s|%s|%s|%s\n"), PadToLength(_("State"),7).c_str(), PadToLength(_("Progress"),4).c_str(), PadToLength(_("Name"),20).c_str(), PadToLength(_("ETA"),15).c_str(), PadToLength(_("PPD"),7).c_str(), PadToLength(_("PRCG"),23).c_str(), PadToLength(_("Credit"),11).c_str()));
		textOS.WriteString(_T("=======|====|====================|===============|=======|=======================|===========\n"));
		// Iterate through the clients and print data
		for(currentClient=0; currentClient<ClientsManager::GetInstance()->GetCount(); ++currentClient)
		{
			textOS.WriteString(wxString::Format(_T("%s|%s|%s|%s|%s|%s|%s\n"), PadToLength(dataArray[currentClient][12],7).c_str(), PadToLength(dataArray[currentClient][0],4).c_str(), PadToLength(dataArray[currentClient][1],20).c_str(), PadToLength(dataArray[currentClient][2],15).c_str(), PadToLength(dataArray[currentClient][3],7).c_str(), PadToLength(dataArray[currentClient][5],23).c_str(), PadToLength(dataArray[currentClient][6],11).c_str()));
		}
		textOS.WriteString(_T("\n"));
	}
}

wxString WebMonitor::PadToLength(wxString text, wxUint32 length)
{
	if (text.length() >= length)
	{
		return text.Truncate(length);
	}
	else
	{
		return text.Pad((length - text.length()));
	}
}
