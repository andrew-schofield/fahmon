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
#include <vector>

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
	wxUint32       deadlineDays;
	wxString       webAppLocation;
	wxString       simpleWebLocation;
	wxString       simpleTextLocation;
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

	mDataArray = new wxString *[ClientsManager::GetInstance()->GetCount()];
	for (i=0; i<ClientsManager::GetInstance()->GetCount(); i++)
	{
		mDataArray[i] = new wxString[13];
	}

	_PrefsGetBool(PREF_OVERRIDE_TIMEZONE,           overrideTZ);
	_PrefsGetInt (PREF_TZ,                          TZ);
	_PrefsGetUint(PREF_ETA_DISPLAYSTYLE,            deadlineDays);

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
			if(!client->IsAccessible())
			{
				mDataArray[currentClient][4] = _("N/A");
				mDataArray[currentClient][7] = _("N/A");
				mDataArray[currentClient][5] = _("N/A");
				mDataArray[currentClient][6] = _("N/A");
				mDataArray[currentClient][8] = _("N/A");
				mDataArray[currentClient][9] = _("N/A");
				mDataArray[currentClient][10] = _("N/A");
			}
			else
			{
				mDataArray[currentClient][7] = wxString::Format(_T("%s (%u)"), client->GetDonatorName().c_str(), client->GetTeamNumber());
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
					if(deadlineDays == ETADS_LEFT_TIME)
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
						{
							tempString = wxString::Format(_T("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
						}
						else if(nbHours != 0)
						{
							tempString = wxString::Format(_T("%ih %02imn"), nbHours, nbMinutes);
						}
						else
						{
							tempString = wxString::Format(_T("%imn"), nbMinutes);
						}

						mDataArray[currentClient][8] = wxString::Format(_("%s ago"), tempString.c_str());
						mUpdateDate = wxString::Format(wxT("%s"), timeNow.Format(wxT("%d %B, %H:%M")).c_str());
					}
					else if (deadlineDays == ETADS_DATE_DAY_MONTH)
					{
						mDataArray[currentClient][8] = wxString::Format(wxT("%s"), downloadTime.Format(wxT("%d %B, %H:%M")).c_str());
						mUpdateDate = wxString::Format(wxT("%s"), timeNow.Format(wxT("%d %B, %H:%M")).c_str());
					}
					else
					{
						mDataArray[currentClient][8] = wxString::Format(wxT("%s"), downloadTime.Format(wxT("%B %d, %H:%M")).c_str());
						mUpdateDate = wxString::Format(wxT("%s"), timeNow.Format(wxT("%B %d, %H:%M")).c_str());
					}
				}
				if(client->GetProjectId() == INVALID_PROJECT_ID)
				{
					mDataArray[currentClient][5] = _("N/A");
					mDataArray[currentClient][4] = _("N/A");
					mDataArray[currentClient][6] = _("N/A");
					mDataArray[currentClient][9] = _("N/A");
					mDataArray[currentClient][10] = _("N/A");
				}
				else
				{
					mDataArray[currentClient][5] = wxString::Format(_T("%u (R%i, C%i, G%i)"), client->GetProjectId(), client->GetProjectRun(), client->GetProjectClone(), client->GetProjectGen());
					project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());
					if(project == NULL) // this may need to be INVALID_PROJECT
					{
						mDataArray[currentClient][5] = _("N/A");
						mDataArray[currentClient][4] = _("N/A");
						mDataArray[currentClient][6] = _("N/A");
						mDataArray[currentClient][9] = _("N/A");
						mDataArray[currentClient][10] = _("N/A");
					}
					else
					{
						mDataArray[currentClient][6] = wxString::Format(_("%u points"), project->GetCredit());
						mDataArray[currentClient][4] = Core::IdToLongName(project->GetCoreId());
						if(client->GetDownloadDate().IsValid() && project->GetPreferredDeadlineInDays() != 0)
						{
							preferredDeadline = downloadTime;
							preferredDeadline.Add(wxTimeSpan::Seconds(project->GetPreferredDeadlineInDays() * 864));
							if(deadlineDays == ETADS_LEFT_TIME)
							{
								timeDiff = preferredDeadline.Subtract(timeNow);
								timeInMinutes = timeDiff.GetMinutes();
								if(timeDiff.GetMinutes() < 0)
								{
									timeInMinutes = 0 - timeInMinutes;
								}
							// Split the left time into days, hours and minutes
								nbDays    = timeInMinutes / (24 * 60);
								nbMinutes = timeInMinutes % (24 * 60);
								nbHours   = nbMinutes / 60;
								nbMinutes = nbMinutes % 60;
							// Use a friendly format
								if(nbDays != 0)
								{
									tempString = wxString::Format(_T("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
								}
								else if(nbHours != 0)
								{
									tempString = wxString::Format(_T("%ih %02imn"), nbHours, nbMinutes);
								}
								else
								{
									tempString = wxString::Format(_T("%imn"), nbMinutes);
								}

								if(timeDiff.GetMinutes() < 0)
								{
									mDataArray[currentClient][9] = wxString::Format(_("%s ago"), tempString.c_str());
								}
								else
								{
									mDataArray[currentClient][9] = wxString::Format(_("In %s"), tempString.c_str());
								}
							}
							else if (deadlineDays == ETADS_DATE_DAY_MONTH)
							{
								mDataArray[currentClient][9] = wxString::Format(wxT("%s"), preferredDeadline.Format(wxT("%d %B, %H:%M")).c_str());
							}
							else
							{
								mDataArray[currentClient][9] = wxString::Format(wxT("%s"), preferredDeadline.Format(wxT("%B %d, %H:%M")).c_str());
							}
						}
						else
						{
							mDataArray[currentClient][9] = _("N/A");
						}

					// Final deadline: if it is equal to 0 day, there is no final deadline
						if(client->GetDownloadDate().IsValid() && project->GetFinalDeadlineInDays() != 0)
						{
							finalDeadline = downloadTime;
							finalDeadline.Add(wxTimeSpan::Seconds(project->GetFinalDeadlineInDays() * 864));
							if(deadlineDays == ETADS_LEFT_TIME)
							{
								timeDiff = finalDeadline.Subtract(timeNow);
								timeInMinutes = timeDiff.GetMinutes();
								if(timeDiff.GetMinutes() < 0)
								{
									timeInMinutes = 0 - timeInMinutes;
								}
							// Split the left time into days, hours and minutes
								nbDays    = timeInMinutes / (24 * 60);
								nbMinutes = timeInMinutes % (24 * 60);
								nbHours   = nbMinutes / 60;
								nbMinutes = nbMinutes % 60;
							// Use a friendly format
								if(nbDays != 0)
								{
									tempString = wxString::Format(_T("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
								}
								else if(nbHours != 0)
								{
									tempString = wxString::Format(_T("%ih %02imn"), nbHours, nbMinutes);
								}
								else
								{
									tempString = wxString::Format(_T("%imn"), nbMinutes);
								}

								if(timeDiff.GetMinutes() < 0)
								{
									mDataArray[currentClient][10] = wxString::Format(_("%s ago"), tempString.c_str());
								}
								else
								{
									mDataArray[currentClient][10] = wxString::Format(_("In %s"), tempString.c_str());
								}
							}
							else if (deadlineDays == ETADS_DATE_DAY_MONTH)
							{
								mDataArray[currentClient][10] = wxString::Format(wxT("%s"), finalDeadline.Format(wxT("%d %B, %H:%M")).c_str());
							}
							else
							{
								mDataArray[currentClient][10] = wxString::Format(wxT("%s"), finalDeadline.Format(wxT("%B %d, %H:%M")).c_str());
							}
						}
						else
						{
							mDataArray[currentClient][10] = _("N/A");
						}
					}
				}
			}

			if(!client->IsAccessible())
			{
				mDataArray[currentClient][12] = _("Inaccessible");
				mDataArray[currentClient][11] = _T("#666666");
			}
			else if(client->IsStopped())
			{
				mDataArray[currentClient][12] = _("Stopped");
				mDataArray[currentClient][11] = _T("#FFAAAA");
			}
			else if(client->IsInactive())
			{
				mDataArray[currentClient][12] = _("Inactive");
				mDataArray[currentClient][11] = _T("#FFFFAA");
			}
			else if(client->IsHung())
			{
				mDataArray[currentClient][12] = _("Hung");
				mDataArray[currentClient][11] = _T("#FFAAAA");
			}
			else if(client->IsAsynch())
			{
				mDataArray[currentClient][12] = _("Async");
				mDataArray[currentClient][11] = _T("#AAAAFF");
			}
			else
			{
				mDataArray[currentClient][12] = _("Ok");
				mDataArray[currentClient][11] = _T("#AAFFAA");
			}

			mDataArray[currentClient][0] = client->GetProgressString();
			mDataArray[currentClient][1] = client->GetName();

			if(client->GetProgress() == 100)
			{
				mDataArray[currentClient][2] = _("Finished");
			}
			else if(!client->IsAccessible() || client->IsStopped())
			{
				mDataArray[currentClient][2] = _("N/A");
			}
			else if(!client->GetETA()->IsOk())
			{
				mDataArray[currentClient][2] = _("N/A");
			}
			else if(client->IsHung())
			{
				mDataArray[currentClient][2] = _("*Hung*");
			}
			else
			{
				mDataArray[currentClient][2] = client->GetETA()->GetString().c_str();
			}

			mDataArray[currentClient][3] = _T("--");

			project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());

		// If it's possible to get the PPD, do so now
			if(client->IsAccessible() && !client->IsStopped() && !client->IsHung() && project != INVALID_PROJECT_ID)
			{
				mDataArray[currentClient][3] = wxString::Format(_T("%.2f"), client->GetPPD());
			}
		}
	}

	if(useWebApp)
	{
		if(wxFileExists(PathManager::GetUserTplPath() + wxT("/fancy_template.htm")))
		{
			ProcessTemplate(PathManager::GetUserTplPath() + wxT("/fancy_template.htm"), webAppLocation);
		}
		else
		{
			ProcessTemplate(PathManager::GetGlobalTplPath() + wxT("/fancy_template.htm"), webAppLocation);
		}
	}
	if(useSimpleWeb)
	{
		if(wxFileExists(PathManager::GetUserTplPath() + wxT("/simple_template.htm")))
		{
			ProcessTemplate(PathManager::GetUserTplPath() + wxT("/simple_template.htm"), simpleWebLocation);
		}
		else
		{
			ProcessTemplate(PathManager::GetGlobalTplPath() + wxT("/simple_template.htm"), simpleWebLocation);
		}
	}
	if(useSimpleText)
	{
		if(wxFileExists(PathManager::GetUserTplPath() + wxT("/simple_template.txt")))
		{
			ProcessTemplate(PathManager::GetUserTplPath() + wxT("/simple_template.txt"), simpleTextLocation);
		}
		else
		{
			ProcessTemplate(PathManager::GetGlobalTplPath() + wxT("/simple_template.txt"), simpleTextLocation);
		}
	}
}

void WebMonitor::ProcessTemplate(wxString templateFile, wxString outputFile)
{
	wxFileOutputStream    fileOS(outputFile);
	wxTextOutputStream    textOS(fileOS);
	wxFileInputStream     fileIS(templateFile);
	wxTextInputStream     textIS(fileIS);
	wxString              currentLine;
	bool                  continueReading;
	wxString              processLine, outputLine, searchString;
	wxUint32              startPos, endPos, currentClient = 0, i;

		// Could the file be opened?
	if(fileIS.Ok() == false)
	{
		_LogMsgError(wxString::Format(_("Could not open template file <%s> for reading!\nThe web application will not be created!"), outputFile.c_str()));
		return;
	}
	if(fileOS.Ok() == false)
	{
		_LogMsgError(wxString::Format(_("Could not open file <%s> for writing!\nThe web application will not be created!"), outputFile.c_str()));
		return;
	}
		// check that images exist in output folder (only check first one, if !exist, recopy all)
	if (!wxFileExists(wxPathOnly(outputFile) + wxT("/dialog_icon.png")))
	{
		wxCopyFile(PathManager::GetImgPath() + wxT("/dialog_icon.png"), wxPathOnly(outputFile) + wxT("/dialog_icon.png"));
		wxCopyFile(PathManager::GetImgPath() + wxT("/list_client_asynch.png"), wxPathOnly(outputFile) + wxT("/list_client_asynch.png"));
		wxCopyFile(PathManager::GetImgPath() + wxT("/list_client_inaccessible.png"), wxPathOnly(outputFile) + wxT("/list_client_inaccessible.png"));
		wxCopyFile(PathManager::GetImgPath() + wxT("/list_client_inactive.png"), wxPathOnly(outputFile) + wxT("/list_client_inactive.png"));
		wxCopyFile(PathManager::GetImgPath() + wxT("/list_client_ok.png"), wxPathOnly(outputFile) + wxT("/list_client_ok.png"));
		wxCopyFile(PathManager::GetImgPath() + wxT("/list_client_stopped.png"), wxPathOnly(outputFile) + wxT("/list_client_stopped.png"));
	}
	continueReading = false;
	processLine = wxT("");
	while(!fileIS.Eof()){
		currentLine = textIS.ReadLine();
		outputLine.clear();
		if (currentLine.Find(wxT("<!--tpl")) == wxNOT_FOUND && continueReading == false)
		{
			textOS.WriteString(currentLine + wxT("\n"));
		}
		else //lets process our template
		{
			processLine = processLine + wxT("") + currentLine;
			if (currentLine.Find(wxT("tpl//-->")) == wxNOT_FOUND)
			{
				continueReading = true;
			}
			else
			{
				continueReading = false;
				if (processLine.Find(wxT("tpl_loop")) != wxNOT_FOUND)
				{
					startPos = processLine.Find(wxT("tpl_loop "))+8;
					endPos = processLine.Find(wxT(" tpl//-->"));
					textOS.WriteString(processLine.Mid(0,startPos-12));
					searchString = processLine.Mid(startPos+1,endPos-startPos);
					for(currentClient=0; currentClient<ClientsManager::GetInstance()->GetCount(); ++currentClient)
					{
						outputLine = searchString;
						std::vector<wxString> v = TemplateToVector(searchString);

						for(i=0;i<v.size();i++)
						{
							outputLine.Replace(v.at(i), DecodeTemplate(v.at(i), currentClient));
						}
						textOS.WriteString(outputLine + wxT("\n"));
					}
					textOS.WriteString(processLine.Mid(endPos+9, processLine.length()-endPos-9) + wxT("\n"));
				}
				else if (processLine.Find(wxT("tpl_static")) != wxNOT_FOUND)
				{
					outputLine = processLine;
						//create a vector of wxStrings
					std::vector<wxString> v = TemplateToVector(outputLine);
					for(i=0;i<v.size();i++)
					{
						outputLine.Replace(v.at(i), DecodeTemplate(v.at(i), currentClient));
					}
					outputLine.Replace(wxT("<!--tpl_static "), wxT(""));
					outputLine.Replace(wxT(" tpl//-->"), wxT(""));
					textOS.WriteString(outputLine + wxT("\n"));
				}
					//must be last
				processLine.clear();
			}
		}
	}

	fileOS.Close();
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

wxString WebMonitor::DecodeTemplate(wxString templateCode, wxUint32 clientId)
{
	//for reference: 0progress, 1client name, 2ETA, 3PPD, 4corename, 5projectID, 6credit, 7username/team, 8downloaded, 9preferred, 10final, 11bgcolor, 12state
	//sigh, why can't we switch on wxStrings
	wxUint32    startCount, endCount;
	wxString    count, tCode;
	double      tmpDouble;
	wxUint32    padding;

	startCount = templateCode.find(wxT("{"));
	endCount = templateCode.find(wxT("}"));

	count = templateCode.Mid(startCount, endCount-startCount+1);
	templateCode.Replace(count, wxT(""));
	count = count.Mid(1, count.Length()-2);

	if(count.ToDouble(&tmpDouble) == false)
		padding = 0;
	padding = (wxUint32)tmpDouble;

	if (templateCode == wxT("@PERCENTAGE@")) tCode = mDataArray[clientId][0];
	else if (templateCode == wxT("@NAME@")) tCode = mDataArray[clientId][1];
	else if (templateCode == wxT("@ETA@")) tCode = mDataArray[clientId][2];
	else if (templateCode == wxT("@PPD@")) tCode = mDataArray[clientId][3];
	else if (templateCode == wxT("@CORE@")) tCode = mDataArray[clientId][4];
	else if (templateCode == wxT("@PRCG@")) tCode = mDataArray[clientId][5];
	else if (templateCode == wxT("@POINTS@")) tCode = mDataArray[clientId][6];
	else if (templateCode == wxT("@USER_TEAM@")) tCode = mDataArray[clientId][7];
	else if (templateCode == wxT("@DOWNLOAD_DATE@")) tCode = mDataArray[clientId][8];
	else if (templateCode == wxT("@PREFERRED_DATE@")) tCode = mDataArray[clientId][9];
	else if (templateCode == wxT("@FINAL_DATE@")) tCode = mDataArray[clientId][10];
	else if (templateCode == wxT("@STATE_COLOR@")) tCode = mDataArray[clientId][11];
	else if (templateCode == wxT("@STATE@")) tCode = mDataArray[clientId][12];
	else if (templateCode == wxT("@FAHMON_VERSION@")) tCode = wxT(FMC_PRODUCT);
	else if (templateCode == wxT("@UPDATE_TIME@")) tCode = mUpdateDate;
	else if (templateCode == wxT("@TOTAL_PPD@")) tCode = wxString::Format(wxT("%.2f"), MainDialog::GetInstance()->GetTotalPPD());
	else if (templateCode == wxT("@STATE_IMAGE@"))
	{
		if(mDataArray[clientId][12] == _("Inaccessible"))
		{
			tCode = _T("list_client_inaccessible.png");
		}
		else if(mDataArray[clientId][12] == _("Stopped"))
		{
			tCode = _T("list_client_stopped.png");
		}
		else if(mDataArray[clientId][12] == _("Inactive"))
		{
			tCode = _T("list_client_inactive.png");
		}
		else if(mDataArray[clientId][12] == _("Hung"))
		{
			tCode = _T("list_client_stopped.png");
		}
		else if(mDataArray[clientId][12] == _("Async"))
		{
			tCode = _T("list_client_asynch.png");
		}
		else
		{
			tCode = _T("list_client_ok.png");
		}
	}
	else if (templateCode == wxT("@CLIENT_ID@"))
	{
		tCode = wxString::Format(wxT("%i"), clientId+1);
	}
	else if (templateCode == wxT("@_State@")) tCode = _("State");
	else if (templateCode == wxT("@_PRCG@")) tCode = _("PRCG");
	else if (templateCode == wxT("@_Credit@")) tCode = _("Credit");
	else if (templateCode == wxT("@_Downloaded@")) tCode = _("Downloaded");
	else if (templateCode == wxT("@_Progress@")) tCode = _("Progress");
	else if (templateCode == wxT("@_Name@")) tCode = _("Name");
	else if (templateCode == wxT("@_ETA@")) tCode = _("ETA");
	else if (templateCode == wxT("@_PPD@")) tCode = _("PPD");
	else if (templateCode == wxT("@_Work_Unit_Information@")) tCode = _("Work Unit Information");
	else if (templateCode == wxT("@_Core:@")) tCode = _("Core:");
	else if (templateCode == wxT("@_Project:@")) tCode = _("Project:");
	else if (templateCode == wxT("@_Credit:@")) tCode = _("Credit:");
	else if (templateCode == wxT("@_Username:@")) tCode = _("Username:");
	else if (templateCode == wxT("@_Preferred_Deadline:@")) tCode = _("Preferred Deadline:");
	else if (templateCode == wxT("@_Final_Deadline:@")) tCode = _("Final Deadline:");
	else if (templateCode == wxT("@_Total_PPD:@")) tCode = _("Total PPD:");
	else if (templateCode == wxT("@_Web_View@")) tCode = _("Web View");
	else if (templateCode == wxT("@_FahMon_Client_Monitoring@")) tCode = _("FahMon Client Monitoring");
	else if (templateCode == wxT("@_Simple_Text_Output@")) tCode = _("Simple Text Output");
	else if (templateCode == wxT("@_Last_updated:@")) tCode = _("Last updated:");
	else if (templateCode == wxT("@_Downloaded:@")) tCode = _("Downloaded:");

	if(padding>0)
	{
		return PadToLength(tCode, padding);
	}
	else
	{
		return tCode;
	}

}

std::vector<wxString> WebMonitor::TemplateToVector(wxString inputTemplate)
{
	//create a vector of wxStrings
	std::vector<wxString> v;
	wxUint32 firstAt = 0,secondAt = 0, i = 0;
	bool isOpenTag = false;

	for(i=0;i<inputTemplate.Length();i++)
	{
		switch(inputTemplate.GetChar(i))
		{
			case '@':
				if(!isOpenTag)
				{
					isOpenTag = true;
					firstAt = i;
				}
				else
				{
					isOpenTag = false;
					secondAt = i;
					v.push_back(inputTemplate.Mid(firstAt, secondAt-firstAt+1));
				}
				break;
			default:
				break;
		}
	}
	return v;
}
