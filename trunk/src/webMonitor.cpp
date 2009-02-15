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
 * \file webMonitor.cpp
 * Web Monitoring.
 * Creates the class or managing the web output options.
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "webMonitor.h"

#include "tools.h"
#include "pathManager.h"
#include "clientsManager.h"
#include "projectsManager.h"
#include "listViewClients.h"
#include "preferencesManager.h"
#include "mainDialog.h"
#include "fahmonConsts.h"
#include "messagesManager.h"
#include "multiProtocolFile.h"
#include "ftpConnection.h"

#include "wx/textfile.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "wx/curl/ftp.h"

#include <vector>

// The single instance of WebMonitor across the application
WebMonitor* WebMonitor::mInstance = NULL;

// Protects from multiple access from different threads
// Prevents it from being called multiple times whilst already running
wxMutex WebMonitor::mMutexWebMonitorAccess;


WebMonitor::WebMonitor(void)
{
}


WebMonitor::~WebMonitor(void)
{
}


void WebMonitor::CreateInstance(void)
{
	wxASSERT(mInstance == NULL);

	mInstance = new WebMonitor();

}


void WebMonitor::DestroyInstance(void)
{
	wxASSERT(mInstance != NULL);

	delete mInstance;
	mInstance = NULL;
}


WebMonitor* WebMonitor::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}


void WebMonitor::WriteApp(void)
{
	bool           useWebApp;
	bool           useSimpleWeb;
	bool           useSimpleText;
	wxUint32       deadlineDays;
	wxString       webAppLocation;
	wxString       simpleWebLocation;
	wxString       simpleTextLocation;
	const Client  *client;
	const Project *project;
	wxInt32        timeInMinutes;
	wxUint32       currentClient, i;
	wxDateTime     preferredDeadline;
	wxDateTime     finalDeadline;
	wxDateTime     downloadTime;
	wxDateTime     timeNow;
	wxTimeSpan     timeDiff;

	LoadCustomVariables();

	mDataArray = new wxString *[ClientsManager::GetInstance()->GetCount()];
	for (i=0; i<ClientsManager::GetInstance()->GetCount(); i++)
	{
		mDataArray[i] = new wxString[17];
	}

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
			if(!client->IsAccessible() || !client->IsEnabled())
			{
				mDataArray[currentClient][4]  = _("N/A");
				mDataArray[currentClient][7]  = _("N/A");
				mDataArray[currentClient][5]  = _("N/A");
				mDataArray[currentClient][6]  = _("N/A");
				mDataArray[currentClient][8]  = _("N/A");
				mDataArray[currentClient][9]  = _("N/A");
				mDataArray[currentClient][10] = _("N/A");
				mDataArray[currentClient][13] = _("N/A");
				mDataArray[currentClient][14] = _("N/A");
				mDataArray[currentClient][15] = _("N/A");
				mDataArray[currentClient][16] = _("N/A");
			}
			else
			{
				mDataArray[currentClient][7] = wxString::Format(_T("%s (%u)"), client->GetDonatorName().c_str(), client->GetTeamNumber());
				mDataArray[currentClient][13] = client->GetDonatorName();
				mDataArray[currentClient][14] = wxString::Format(_T("%u"), client->GetTeamNumber());
				mDataArray[currentClient][15] = client->GetClientType();
				mDataArray[currentClient][16] = wxString::Format(_T("%.2f"), client->GetCoreVersion());
				if(client->GetDownloadDate().IsValid())
				{
					timeNow = wxDateTime::Now();
					downloadTime = client->GetDownloadDate();
					if(deadlineDays == ETADS_LEFT_TIME)
					{
						timeDiff = timeNow.Subtract(downloadTime);
						timeInMinutes = timeDiff.GetMinutes();

						mDataArray[currentClient][8] = wxString::Format(_("%s ago"), ETA::FormatMinutes(timeInMinutes).c_str());
						mUpdateDate = wxString::Format(_T("%s"), timeNow.Format(_T("%d %B, %H:%M")).c_str());
					}
					else if (deadlineDays == ETADS_DATE_DAY_MONTH)
					{
						mDataArray[currentClient][8] = wxString::Format(_T("%s"), downloadTime.Format(_T("%d %B, %H:%M")).c_str());
						mUpdateDate = wxString::Format(_T("%s"), timeNow.Format(_T("%d %B, %H:%M")).c_str());
					}
					else
					{
						mDataArray[currentClient][8] = wxString::Format(_T("%s"), downloadTime.Format(_T("%B %d, %H:%M")).c_str());
						mUpdateDate = wxString::Format(_T("%s"), timeNow.Format(_T("%B %d, %H:%M")).c_str());
					}
				}
				if(client->GetProjectId() == INVALID_PROJECT_ID)
				{
					mDataArray[currentClient][5]  = _("N/A");
					mDataArray[currentClient][4]  = _("N/A");
					mDataArray[currentClient][6]  = _("N/A");
					mDataArray[currentClient][9]  = _("N/A");
					mDataArray[currentClient][10] = _("N/A");
					mDataArray[currentClient][13] = _("N/A");
					mDataArray[currentClient][14] = _("N/A");
					mDataArray[currentClient][15] = _("N/A");
					mDataArray[currentClient][16] = _("N/A");
				}
				else
				{
					mDataArray[currentClient][5] = wxString::Format(_T("%u (R%i, C%i, G%i)"), client->GetProjectId(), client->GetProjectRun(), client->GetProjectClone(), client->GetProjectGen());
					project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());
					if(project == NULL) // this may need to be INVALID_PROJECT
					{
						mDataArray[currentClient][5]  = _("N/A");
						mDataArray[currentClient][4]  = _("N/A");
						mDataArray[currentClient][6]  = _("N/A");
						mDataArray[currentClient][9]  = _("N/A");
						mDataArray[currentClient][10] = _("N/A");
						mDataArray[currentClient][15] = _("N/A");
						mDataArray[currentClient][16] = _("N/A");
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
								if(timeDiff.GetMinutes() < 0)
								{
									mDataArray[currentClient][9] = wxString::Format(_("%s ago"), ETA::FormatMinutes(timeInMinutes).c_str());
								}
								else
								{
									mDataArray[currentClient][9] = wxString::Format(_("In %s"), ETA::FormatMinutes(timeInMinutes).c_str());
								}
							}
							else if (deadlineDays == ETADS_DATE_DAY_MONTH)
							{
								mDataArray[currentClient][9] = wxString::Format(_T("%s"), preferredDeadline.Format(_T("%d %B, %H:%M")).c_str());
							}
							else
							{
								mDataArray[currentClient][9] = wxString::Format(_T("%s"), preferredDeadline.Format(_T("%B %d, %H:%M")).c_str());
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
								if(timeDiff.GetMinutes() < 0)
								{
									mDataArray[currentClient][10] = wxString::Format(_("%s ago"), ETA::FormatMinutes(timeInMinutes).c_str());
								}
								else
								{
									mDataArray[currentClient][10] = wxString::Format(_("In %s"), ETA::FormatMinutes(timeInMinutes).c_str());
								}
							}
							else if (deadlineDays == ETADS_DATE_DAY_MONTH)
							{
								mDataArray[currentClient][10] = wxString::Format(_T("%s"), finalDeadline.Format(_T("%d %B, %H:%M")).c_str());
							}
							else
							{
								mDataArray[currentClient][10] = wxString::Format(_T("%s"), finalDeadline.Format(_T("%B %d, %H:%M")).c_str());
							}
						}
						else
						{
							mDataArray[currentClient][10] = _("N/A");
						}
					}
				}
			}

			if(!client->IsEnabled())
			{
				mDataArray[currentClient][12] = _("Disabled");
				mDataArray[currentClient][11] = _T("#FFAAAA");
			}
			else if(!client->IsAccessible())
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
			else if(client->IsPaused())
			{
				mDataArray[currentClient][12] = _("Paused");
				mDataArray[currentClient][11] = _T("#FBA16C");
			}
			else
			{
				mDataArray[currentClient][12] = _("Ok");
				mDataArray[currentClient][11] = _T("#AAFFAA");
			}

			mDataArray[currentClient][0] = client->GetProgressString();
			mDataArray[currentClient][1] = client->GetName();

			if(!client->IsEnabled())
			{
				mDataArray[currentClient][2] = _("Disabled");
			}
			else if(client->GetProgress() == 100)
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
			else if(client->IsPaused())
			{
				mDataArray[currentClient][2] = _("Paused");
			}
			else
			{
				mDataArray[currentClient][2] = client->GetETA()->GetString().c_str();
			}

			mDataArray[currentClient][3] = _T("--");

			project = ProjectsManager::GetInstance()->GetProject(client->GetProjectId());

		// If it's possible to get the PPD, do so now
			if(client->IsAccessible() && !client->IsStopped() && !client->IsHung() && project != INVALID_PROJECT_ID && client->IsEnabled() && !client->IsPaused())
			{
				mDataArray[currentClient][3] = wxString::Format(_T("%.2f"), client->GetPPD());
			}
		}
	}

	if(useWebApp)
	{
		wxString webAppTemplateLocation;
		_PrefsGetString(PREF_WEBAPP_WEBAPPTEMPLATELOCATION,     webAppTemplateLocation);
		ProcessTemplate(webAppTemplateLocation, webAppLocation);
	}
	if(useSimpleWeb)
	{
		wxString simpleWebTemplateLocation;
		_PrefsGetString(PREF_WEBAPP_SIMPLEWEBTEMPLATELOCATION,  simpleWebTemplateLocation);
		ProcessTemplate(simpleWebTemplateLocation, simpleWebLocation);
	}
	if(useSimpleText)
	{
		wxString simpleTextTemplateLocation;
		_PrefsGetString(PREF_WEBAPP_SIMPLETEXTTEMPLATELOCATION, simpleTextTemplateLocation);
		ProcessTemplate(simpleTextTemplateLocation, simpleTextLocation);
	}
	for (i=0; i<ClientsManager::GetInstance()->GetCount(); i++)
	{
		delete [ ] mDataArray[i];
	}
	delete [ ] mDataArray;
}

void WebMonitor::ProcessTemplate(wxString const &templateFile, wxString const &outputFile)
{
	bool ftp = false;
	wxString fn;
	if(multiProtocolFile::GetFileProtocol(outputFile) == multiProtocolFile::FTP)
	{
		fn = wxFileName::CreateTempFileName(_T(FMC_APPNAME));
		ftp = true;
	}
	else
		fn = outputFile;

	wxFileOutputStream    fileOS(fn);
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
	if (!wxFileExists(wxPathOnly(outputFile) + _T("/dialog_icon.png")) && ftp == false)
	{
		wxCopyFile(PathManager::GetImgPath() + _T("/dialog_icon.png"), wxPathOnly(outputFile) + _T("/dialog_icon.png"));
		wxCopyFile(PathManager::GetImgPath() + _T("/list_client_asynch.png"), wxPathOnly(outputFile) + _T("/list_client_asynch.png"));
		wxCopyFile(PathManager::GetImgPath() + _T("/list_client_inaccessible.png"), wxPathOnly(outputFile) + _T("/list_client_inaccessible.png"));
		wxCopyFile(PathManager::GetImgPath() + _T("/list_client_inactive.png"), wxPathOnly(outputFile) + _T("/list_client_inactive.png"));
		wxCopyFile(PathManager::GetImgPath() + _T("/list_client_ok.png"), wxPathOnly(outputFile) + _T("/list_client_ok.png"));
		wxCopyFile(PathManager::GetImgPath() + _T("/list_client_stopped.png"), wxPathOnly(outputFile) + _T("/list_client_stopped.png"));
		wxCopyFile(PathManager::GetImgPath() + _T("/list_client_paused.png"), wxPathOnly(outputFile) + _T("/list_client_paused.png"));
	}
	continueReading = false;
	processLine = _T("");
	while(!fileIS.Eof()){
		currentLine = textIS.ReadLine();
		outputLine.clear();
		if (currentLine.Find(_T("<!--tpl")) == wxNOT_FOUND && continueReading == false)
		{
			textOS.WriteString(currentLine + _T("\n"));
		}
		else //lets process our template
		{
			processLine = processLine + _T("") + currentLine;
			if (currentLine.Find(_T("tpl//-->")) == wxNOT_FOUND)
			{
				continueReading = true;
			}
			else
			{
				continueReading = false;
				if (processLine.Find(_T("tpl_loop")) != wxNOT_FOUND)
				{
					startPos = processLine.Find(_T("tpl_loop "))+8;
					endPos = processLine.Find(_T(" tpl//-->"));
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
						textOS.WriteString(outputLine + _T("\n"));
					}
					textOS.WriteString(processLine.Mid(endPos+9, processLine.length()-endPos-9) + _T("\n"));
				}
				else if (processLine.Find(_T("tpl_static")) != wxNOT_FOUND)
				{
					outputLine = processLine;
					//create a vector of wxStrings
					std::vector<wxString> v = TemplateToVector(outputLine);
					for(i=0;i<v.size();i++)
					{
						outputLine.Replace(v.at(i), DecodeTemplate(v.at(i), currentClient));
					}
					outputLine.Replace(_T("<!--tpl_static "), _T(""));
					outputLine.Replace(_T(" tpl//-->"), _T(""));
					textOS.WriteString(outputLine + _T("\n"));
				}
				//must be last
				processLine.clear();
			}
		}
	}

	fileOS.Close();
	if(ftp)
	{
		FTPConnection::PutFTPFile(outputFile, fn);
		wxRemoveFile(fn);
	}
}

const wxString WebMonitor::PadToLength(wxString text, wxUint32 length)
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

const wxString WebMonitor::DecodeTemplate(wxString templateCode, wxUint32 clientId)
{
	//for reference: 0progress, 1client name, 2ETA, 3PPD, 4corename, 5projectID, 6credit, 7username/team, 8downloaded, 9preferred, 10final, 11bgcolor, 12state, 13username, 14team
	//sigh, why can't we switch on wxStrings
	wxUint32    startCount, endCount;
	wxString    count, tCode;
	double      tmpDouble;
	wxUint32    padding;

	startCount = templateCode.find(_T("{"));
	endCount = templateCode.find(_T("}"));

	count = templateCode.Mid(startCount, endCount-startCount+1);
	templateCode.Replace(count, _T(""));
	count = count.Mid(1, count.Length()-2);

	if(count.ToDouble(&tmpDouble) == false)
		padding = 0;
	padding = (wxUint32)tmpDouble;

	if (templateCode == _T("@PERCENTAGE@"))          tCode = mDataArray[clientId][0];
	else if (templateCode == _T("@NAME@"))           tCode = mDataArray[clientId][1];
	else if (templateCode == _T("@ETA@"))            tCode = mDataArray[clientId][2];
	else if (templateCode == _T("@PPD@"))            tCode = mDataArray[clientId][3];
	else if (templateCode == _T("@CORE@"))           tCode = mDataArray[clientId][4];
	else if (templateCode == _T("@PRCG@"))           tCode = mDataArray[clientId][5];
	else if (templateCode == _T("@POINTS@"))         tCode = mDataArray[clientId][6];
	else if (templateCode == _T("@USER_TEAM@"))      tCode = mDataArray[clientId][7];
	else if (templateCode == _T("@DOWNLOAD_DATE@"))  tCode = mDataArray[clientId][8];
	else if (templateCode == _T("@PREFERRED_DATE@")) tCode = mDataArray[clientId][9];
	else if (templateCode == _T("@FINAL_DATE@"))     tCode = mDataArray[clientId][10];
	else if (templateCode == _T("@STATE_COLOR@"))    tCode = mDataArray[clientId][11];
	else if (templateCode == _T("@STATE@"))          tCode = mDataArray[clientId][12];
	else if (templateCode == _T("@USERNAME@"))       tCode = mDataArray[clientId][13];
	else if (templateCode == _T("@TEAMNUMBER@"))     tCode = mDataArray[clientId][14];
	else if (templateCode == _T("@CLIENT_TYPE@"))    tCode = mDataArray[clientId][15];
	else if (templateCode == _T("@CORE_VERSION@"))   tCode = mDataArray[clientId][16];
	else if (templateCode == _T("@FAHMON_VERSION@")) tCode = _T(FMC_PRODUCT);
	else if (templateCode == _T("@UPDATE_TIME@"))    tCode = mUpdateDate;
	else if (templateCode == _T("@TOTAL_PPD@"))      tCode = wxString::Format(_T("%.2f"), MainDialog::GetInstance()->GetTotalPPD());
	else if (templateCode == _T("@STATE_IMAGE@"))
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
		else if(mDataArray[clientId][12] == _("Paused"))
		{
			tCode = _T("list_client_paused.png");
		}
		else
		{
			tCode = _T("list_client_ok.png");
		}
	}
	else if (templateCode == _T("@CLIENT_ID@"))
	{
		tCode = wxString::Format(_T("%i"), clientId+1);
	}
	else if (templateCode == _T("@_State@"))                    tCode = _("State");
	else if (templateCode == _T("@_PRCG@"))                     tCode = _("PRCG");
	else if (templateCode == _T("@_Credit@"))                   tCode = _("Credit");
	else if (templateCode == _T("@_Downloaded@"))               tCode = _("Downloaded");
	else if (templateCode == _T("@_Progress@"))                 tCode = _("Progress");
	else if (templateCode == _T("@_Name@"))                     tCode = _("Name");
	else if (templateCode == _T("@_ETA@"))                      tCode = _("ETA");
	else if (templateCode == _T("@_PPD@"))                      tCode = _("PPD");
	else if (templateCode == _T("@_Work_Unit_Information@"))    tCode = _("Work Unit Information");
	else if (templateCode == _T("@_Core@"))                     tCode = _("Core:");
	else if (templateCode == _T("@_Project:@"))                 tCode = _("Project:");
	else if (templateCode == _T("@_Credit:@"))                  tCode = _("Credit:");
	else if (templateCode == _T("@_Username:@"))                tCode = _("Username:");
	else if (templateCode == _T("@_Preferred_Deadline:@"))      tCode = _("Preferred Deadline:");
	else if (templateCode == _T("@_Final_Deadline:@"))          tCode = _("Final Deadline:");
	else if (templateCode == _T("@_Total_PPD:@"))               tCode = _("Total PPD:");
	else if (templateCode == _T("@_Web_View@"))                 tCode = _("Web View");
	else if (templateCode == _T("@_FahMon_Client_Monitoring@")) tCode = _("FahMon Client Monitoring");
	else if (templateCode == _T("@_Simple_Text_Output@"))       tCode = _("Simple Text Output");
	else if (templateCode == _T("@_Last_updated:@"))            tCode = _("Last updated:");
	else if (templateCode == _T("@_Downloaded:@"))              tCode = _("Downloaded:");
	else if (templateCode == _T("@_Client_Type@"))              tCode = _("Client Type");
	else if (templateCode == _T("@_Core_Version@"))             tCode = _("Core Version");
	else tCode = GetVariable(templateCode);

	if(padding>0)
	{
		return PadToLength(tCode, padding);
	}
	else
	{
		return tCode;
	}

}

std::vector<wxString> WebMonitor::TemplateToVector(wxString const &inputTemplate) const
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


const wxString WebMonitor::GetVariable(wxString const &variable)
{
	CustomVariableHashMap::iterator iterator = mCustomVariableHashMap.find(variable);

	if(iterator == mCustomVariableHashMap.end())
	{
		return variable;
	}
	else
	{
		return iterator->second;
	}
}


void WebMonitor::LoadCustomVariables(void)
{
	bool       isVariableOk;
	bool       isValueOk;
	wxInt32    startingPos;
	wxInt32    endingPos;
	wxUint32   i;
	wxString   currentLine;
	wxString   variable;
	wxString   value;
	wxString   inputFilename;
	wxTextFile in;


	// Try to open the file, check if it exists
	inputFilename = PathManager::GetCfgPath() + _T("custom.dat");
	if(wxFileExists(inputFilename) == false || in.Open(inputFilename) == false)
	{
		return;
	}

	// Retrieve each client, one by line
	for(i=0; i<in.GetLineCount(); ++i)
	{
		currentLine = in.GetLine(i);

		// Do not take into account empty lines or comments
		if(currentLine.Len() != 0 && currentLine.GetChar(0) != '#')
		{
			isVariableOk     = false;
			isValueOk = false;

			// First, extract the name
			// We can't tell Find() where to start, so the " char is replaced with something else
			// to not find the same position more than once
			startingPos = currentLine.Find('"');
			if(startingPos != -1)
			{
				currentLine.SetChar(startingPos, ' ');
				endingPos = currentLine.Find('"');
				if(endingPos != -1)
				{
					currentLine.SetChar(endingPos, ' ');
					variable = currentLine.Mid(startingPos+1, endingPos-startingPos-1);
					isVariableOk   = true;
				}
			}

			// Then, the location
			startingPos = currentLine.Find('"');
			if(startingPos != -1)
			{
				currentLine.SetChar(startingPos, ' ');
				endingPos = currentLine.Find('"');
				if(endingPos != -1)
				{
					value = currentLine.Mid(startingPos+1, endingPos-startingPos-1);
					isValueOk   = true;
				}
			}

			if(isVariableOk == true && isValueOk == true)
				mCustomVariableHashMap[variable] = value;
			else
				_LogMsgError(wxString::Format(_("Error while parsing %s on line %u"), inputFilename.c_str(), i+1));
		}
	}

	in.Close();
}
