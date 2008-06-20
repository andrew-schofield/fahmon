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
 * \file httpDownloader.cpp
 * Create a class to download files over http.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "httpDownloader.h"

#include "tools.h"
#include "wx/timer.h"
#include "wx/socket.h"
#include "wx/filename.h"
#include "base64Codec.h"
#include "preferencesManager.h"
#include "messagesManager.h"


// This size should be high enough to store the whole head of the answer
#define BUFFER_SIZE 1024


HTTPDownloader::DownloadStatus HTTPDownloader::DownloadFile(const wxString& host, wxUint32 port, const wxString& resource, wxString& localFileName, ProgressManager& progressManager)
{
	bool                isUsingProxy;
	bool                proxyNeedsAuthentication;
	bool                moreDataToRead;
	bool                isFirstSlice;
	double              downloadSpeed;              // KB/s
	wxByte              buffer[BUFFER_SIZE];
	wxUint32            proxyPort;
	wxUint32            totalDataDownloaded;
	wxUint32            totalDataToDownload;
	wxString            proxyAddress;
	wxString            proxyUsername;
	wxString            proxyPassword;
	wxString            request;
	wxString            base64ProxyAuthentication;
	wxLongLong          startingTime;
	wxLongLong          elapsedTime;
	wxIPV4address       serverAddress;
	wxSocketClient      socket;
	DownloadStatus      returnValue;
	wxFileOutputStream *out;

	// --- Retrieve proxy configuration
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USEPROXY,                   isUsingProxy);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXYADDRESS,               proxyAddress);
	_PrefsGetUint        (PREF_HTTPDOWNLOADER_PROXYPORT,                  proxyPort);
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USE_PROXY_AUTHENTICATION,   proxyNeedsAuthentication);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXY_USERNAME,             proxyUsername);
	_PrefsGetHiddenString(PREF_HTTPDOWNLOADER_PROXY_PASSWORD,             proxyPassword);

	// --- Create a temporary local file and try to open it
	localFileName = wxFileName::CreateTempFileName(wxT(FMC_APPNAME));
	if(localFileName.empty() == true)
	{
		return STATUS_TEMP_FILE_CREATION_ERROR;
	}

	out = new wxFileOutputStream(localFileName);
	if(out->Ok() == false)
	{
		return STATUS_TEMP_FILE_OPEN_ERROR;
	}


	// --- Forge the request, considering the proxy configuration, and fill the address of the 'real' host to contact
	if(isUsingProxy == true)
	{
		// Do we need to use authentication?
		if(proxyNeedsAuthentication == true)
		{
			base64ProxyAuthentication = Base64Codec::Encode(wxString::Format(wxT("%s:%s"), proxyUsername.c_str(), proxyPassword.c_str()));
			request                   = wxString::Format(wxT("GET http://%s:%u/%s HTTP/1.0\nHost: %s\nProxy-Authorization: Basic %s\nUser-Agent: %s/%s\n\n"), host.c_str(), port, resource.c_str(), host.c_str(), base64ProxyAuthentication.c_str(), wxT(FMC_APPNAME), wxT(FMC_VERSION));
		}
		else
		{
			request = wxString::Format(wxT("GET http://%s:%u/%s HTTP/1.0\nHost: %s\nUser-Agent: %s/%s\n\n"), host.c_str(), port, resource.c_str(), host.c_str(), wxT(FMC_APPNAME), wxT(FMC_VERSION));
		}

		serverAddress.Hostname(proxyAddress);
		serverAddress.Service(proxyPort);
	}
	else
	{
		request = wxString::Format(wxT("GET /%s HTTP/1.0\nHost: %s\nUser-Agent: %s/%s\n\n"), resource.c_str(), host.c_str(), wxT(FMC_APPNAME), wxT(FMC_VERSION));
		serverAddress.Hostname(host);
		serverAddress.Service(port);
	}

	returnValue = STATUS_NO_ERROR;

	// --- Try to contact the server
	socket.SetTimeout(15);
	if(socket.Connect(serverAddress) == true)
	{
		// --- Send the request
		socket.Write((const char*)request.mb_str(), request.Len());
		if(socket.LastCount() == request.Len())
		{
			// --- Read the data and display statistics about the progress
			moreDataToRead      = true;
			isFirstSlice        = true;
			startingTime        = wxGetLocalTimeMillis();
			totalDataDownloaded = 0;
			totalDataToDownload = 0;
			while(moreDataToRead == true)
			{
				// Retrieve and save data to the disk
				socket.Read(buffer, BUFFER_SIZE);
				if(socket.LastCount() != 0)
				{
					out->Write(buffer, socket.LastCount());
					totalDataDownloaded += socket.LastCount();

					// The first slice of data contains the header
					if(isFirstSlice == true)
					{
						totalDataToDownload = ExtractContentSize(buffer, socket.LastCount());
						isFirstSlice        = false;
					}
				}
				else
				{
					moreDataToRead = false;
				}

				// Update the download speed
				// The speed is only the average speed for the whole download (not the 'real' current speed)
				// but this is should be enough, we are not developing a browser :-)
				elapsedTime = wxGetLocalTimeMillis() - startingTime;
				if(elapsedTime != 0)
				{
					downloadSpeed = (totalDataDownloaded * 1000.0) / (double)(elapsedTime.ToLong() * 1024);
				}
				else
				{
					downloadSpeed = 0.0;
				}

				if(progressManager.SetText(wxString::Format(_("%.1f KB/s"), downloadSpeed)) == false)
				{
					returnValue    = STATUS_ABORTED;
					moreDataToRead = false;
				}

				// Update the progress
				if(totalDataToDownload != 0)
				{
					if(progressManager.SetProgress((totalDataDownloaded * 100) / totalDataToDownload) == false)
					{
						returnValue    = STATUS_ABORTED;
						moreDataToRead = false;
					}
				}
			}
		}
		else
		{
			returnValue = STATUS_SEND_REQUEST_ERROR;
		}
	}
	else
	{
		returnValue = STATUS_CONNECT_ERROR;
	}

	// --- Clean everything before leaving
	socket.Close();
	out->Close();
	delete out;

	return returnValue;
}


wxUint32 HTTPDownloader::ExtractContentSize(wxByte* buffer, wxUint32 bufferSize)
{
	long      value;
	wxInt32   fieldPos;
	wxInt32   valueEndPos;
	wxString  header;

	// buffer is ansi, so we perhaps need to convert it to unicode
	buffer[bufferSize-1] = '\0';
	header = wxString::FromAscii((const char*)buffer);

	// Find where the field starts
	fieldPos = header.Find(wxT("Content-Length: "));
	if(fieldPos == -1)
	{
		return 0;
	}

	// Remove leading data, and the field name itself
	// 16 is the size of 'Content-Length: '
	header = header.Mid(fieldPos + 16);

	// Find where the value ends
	valueEndPos = header.Find(wxT("\n"));
	if(valueEndPos == -1)
	{
		return 0;
	}

	// Extract the part containing the size and convert it to a numerical value
	header = header.Mid(0, valueEndPos);
	header.ToLong(&value);

	return (wxInt32)value;
}


HTTPDownloader::DownloadStatus HTTPDownloader::Url(const wxString& url, wxString& localFileName, ProgressManager& progressManager)
{
	wxString      tempString;
	wxInt32       portPosition;
	wxInt32       fslashPosition;
	wxString      clientPortStr;
	unsigned long clientPort;
	wxString      clientServer;
	wxString      clientResource;

	if(url.Mid(0, 4) == wxT("http"))
	{
		tempString = url.Mid(7, url.Length() - 7);
		portPosition = tempString.Find(':');
		fslashPosition = tempString.Find('/');
		if(portPosition == -1)
		{
			clientPortStr = wxT("80");
			clientServer = tempString.Mid(0, fslashPosition);
		}
		else
		{
			clientPortStr = tempString.Mid(portPosition+1, fslashPosition-portPosition-1);
			clientServer = tempString.Mid(0, portPosition);
		}
		clientResource = tempString.Mid(fslashPosition +1 , tempString.Length() - fslashPosition);
		clientPortStr.ToULong(&clientPort);
		return DownloadFile(clientServer, clientPort, clientResource, localFileName, progressManager);
	}
	return STATUS_CONNECT_ERROR;
}
