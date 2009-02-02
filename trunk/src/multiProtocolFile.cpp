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
 * \file multiProtocolFile.cpp
 * A class for managing files across different protocols (local, http).
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "multiProtocolFile.h"
#include "tools.h"
#include "preferencesManager.h"
#include "httpDownloader.h"
#include "base64Codec.h"
#include "messagesManager.h"

#include "wx/file.h"
#include "wx/protocol/http.h"
#include "wx/uri.h"
#include "wx/socket.h"
#include "wx/sstream.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"


bool multiProtocolFile::FileExists(const wxString& fileName)
{
	bool returnValue;
	returnValue = false;
	switch(GetFileProtocol(fileName))
	{
		case FILE:
			return wxFile::Exists(fileName);
			break;
		case HTTP:
		{
			wxURI uri(fileName);
			wxSocketClient socket;
			socket.SetFlags(wxSOCKET_NOWAIT);
			SetHTTPConnection(socket, uri);
			switch(GetHTTPResponseCode(socket))
			{
				case 200:
				case 201:
				case 202:
				case 203:
				case 204:
				case 205:
				case 206:
				case 300:
				case 301:
				case 302:
				case 303:
				case 304:
				case 305:
				case 306:
				case 307:
					returnValue = true;
					break;
				case 400:
				case 401:
				case 403:
				case 402:
				case 404:
				case 405:
				case 406:
				case 407:
				case 408:
				case 409:
				case 410:
				case 411:
				case 412:
				case 413:
				case 414:
				case 415:
				case 416:
				case 417:
				case 500:
				case 501:
				case 502:
				case 503:
				case 504:
				case 505:
					returnValue = false;
					break;
			}
			socket.Close();
			break;
		}
		default:
			returnValue = false;
	}
	return returnValue;
}


time_t multiProtocolFile::LastModification(const wxString& fileName)
{
	switch(GetFileProtocol(fileName))
	{
		case HTTP:
		{
			wxURI uri(fileName);
			wxSocketClient socket;
			socket.SetFlags(wxSOCKET_NOWAIT);
			wxString lastMod;
			wxDateTime lastModTime;
			SetHTTPConnection(socket, uri);
			lastMod = GetHTTPHeader(socket, wxT("Last-Modified"));
			lastModTime.ParseDateTime(lastMod);
			socket.Close();
			return lastModTime.GetTicks();
		}
			break;
		case FTP:
			break;
		case FILE:
			return wxFileModificationTime(fileName);
			break;
		default:
			return 0;
	}
	return 0;
}


bool multiProtocolFile::DirExists(const wxString& dirName)
{
	bool returnValue = false;
	switch(GetFileProtocol(dirName))
	{
		case HTTP:
		{
			wxURI uri(dirName);
			wxSocketClient socket;
			socket.SetFlags(wxSOCKET_NOWAIT);
			SetHTTPConnection(socket, uri);
			switch(GetHTTPResponseCode(socket))
			{
				case 200:
				case 201:
				case 202:
				case 203:
				case 204:
				case 205:
				case 206:
				case 300:
				case 301:
				case 302:
				case 303:
				case 304:
				case 305:
				case 306:
				case 307:
				case 401:
				case 403:
				{
					returnValue = true;
					break;
				}
				case 400:
				case 402:
				case 404:
				case 405:
				case 406:
				case 407:
				case 408:
				case 409:
				case 410:
				case 411:
				case 412:
				case 413:
				case 414:
				case 415:
				case 416:
				case 417:
				case 500:
				case 501:
				case 502:
				case 503:
				case 504:
				case 505:
				{
					returnValue = false;
					break;
				}
			}
			socket.Close();
		}
			break;
		case FTP:
			break;
		case FILE:
			returnValue = wxDirExists(dirName);
			break;
		default:
			returnValue = false;
	}
	return returnValue;
}


wxString multiProtocolFile::GetLocalFileName(const wxString& fileName)
{
	wxString fn;
	switch(GetFileProtocol(fileName))
	{
		case HTTP:
		{
			wxURI uri(fileName);
			wxSocketClient socket;
			socket.SetFlags(wxSOCKET_NOWAIT);
			SetHTTPConnection(socket, uri);
			fn =  GetFile(socket);
			socket.Close();
			break;
		}
		case FTP:
			break;
		case FILE:
			fn = fileName;
			break;
		default:
			fn = wxT("");
	}
	return fn;
}


bool multiProtocolFile::CopyFile(const wxString& inFile, const wxString& outFile)
{
	switch(GetFileProtocol(inFile))
	{
		case HTTP:
			return wxCopyFile(GetLocalFileName(inFile), outFile);
			break;
		case FTP:
			break;
		case FILE:
			return wxCopyFile(inFile, outFile);
			break;
		default:
			return false;
	}
	return false;
}


FileProtocol multiProtocolFile::GetFileProtocol(const wxString& fileName)
{
	if(fileName.Mid(0, 4) == _T("http"))
		return HTTP;
	if(fileName.Mid(0, 3) == _T("ftp"))
		return FTP;
	return FILE;
}


bool multiProtocolFile::SetHTTPConnection(wxSocketClient& socket, wxURI uri, wxInt32 byteRange)
{
	bool isUsingProxy;
	wxString proxyAddress;
	wxUint32 proxyPort;
	bool proxyNeedsAuthentication;
	wxString proxyUsername;
	wxString proxyPassword;
	wxIPV4address addr;
	wxString base64ProxyAuthentication;
	wxString server;
	wxString request;
	wxString range;

	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USEPROXY,                   isUsingProxy);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXYADDRESS,               proxyAddress);
	_PrefsGetUint        (PREF_HTTPDOWNLOADER_PROXYPORT,                  proxyPort);
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USE_PROXY_AUTHENTICATION,   proxyNeedsAuthentication);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXY_USERNAME,             proxyUsername);
	_PrefsGetHiddenString(PREF_HTTPDOWNLOADER_PROXY_PASSWORD,             proxyPassword);

	if(byteRange == 0)
	{
		range = wxT("");
	}
	else if(byteRange > 0)
	{
		range = wxString::Format(wxT("Range: bytes=0-%i"), byteRange-1);
	}
	else
	{
		range = wxString::Format(wxT("Range: bytes=-%i"), byteRange);
	}

	wxInt32 port;
	port = (uri.GetPort() == wxT("")) ? 80 : wxAtoi(uri.GetPort());

		// --- Forge the request, considering the proxy configuration, and fill the address of the 'real' host to contact
	if(isUsingProxy == true)
	{
		// Do we need to use authentication?
		if(proxyNeedsAuthentication == true)
		{
			base64ProxyAuthentication = Base64Codec::Encode(wxString::Format(_T("%s:%s"), proxyUsername.c_str(), proxyPassword.c_str()));
			request                   = wxString::Format(_T("GET http://%s:%u/%s HTTP/1.1\nHost: %s\nProxy-Authorization: Basic %s\nUser-Agent: %s/%s\n%s\n\n"), uri.GetServer().c_str(), port, uri.GetPath().c_str(), uri.GetServer().c_str(), base64ProxyAuthentication.c_str(), _T(FMC_APPNAME), _T(FMC_VERSION), range.c_str());
		}
		else
		{
			request = wxString::Format(_T("GET http://%s:%u/%s HTTP/1.1\nHost: %s\nUser-Agent: %s/%s\n%s\n\n"), uri.GetServer().c_str(), port, uri.GetPath().c_str(), uri.GetServer().c_str(), _T(FMC_APPNAME), _T(FMC_VERSION), range.c_str());
		}

		addr.Hostname(proxyAddress);
		addr.Service(proxyPort);
	}
	else
	{
		request = wxString::Format(_T("GET %s HTTP/1.1\nHost: %s\nUser-Agent: %s/%s\n%s\n\n"), uri.GetPath().c_str(), uri.GetServer().c_str(), _T(FMC_APPNAME), _T(FMC_VERSION), range.c_str());
		addr.Hostname(uri.GetServer());
		addr.Service(port);
	}
	socket.SetTimeout(1);
	if(socket.Connect(addr) == true)
	{
		socket.Write((const char*)request.mb_str(), request.Len());
		if(socket.LastCount() == request.Len())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

}


wxUint32 multiProtocolFile::GetHTTPResponseCode(wxSocketClient& socket)
{
	wxByte buffer[12];
	while(!socket.WaitForRead(0,0)){}
	socket.Read(buffer, 12);

	wxUint32 output = 404;
	wxStringOutputStream *out;
	out = new wxStringOutputStream();
	if(socket.LastCount() != 0)
	{
		out->Write(buffer, socket.LastCount());
		output = wxAtoi(out->GetString().Mid(9,3));
	}
	delete out;
	return output;
}


wxString multiProtocolFile::GetHTTPHeader(wxSocketClient& socket, wxString header)
{
	wxChar buffer[1024];
	while(!socket.WaitForRead(0,0)){}
	socket.Read(buffer, 1024);
	wxString output = wxT("");
	wxStringOutputStream *out;
	out = new wxStringOutputStream();
	if(socket.LastCount() != 0)
	{
		out->Write(buffer, socket.LastCount());
		wxStringTokenizer tkz(out->GetString(), wxT("\n"));
		while ( tkz.HasMoreTokens() )
		{
			wxString token = tkz.GetNextToken();

			if(token.BeforeFirst(':').Strip(wxString::both) == header)
				return token.AfterFirst(':').Strip(wxString::both);
		}
	}
	delete out;
	return output;
}


wxString multiProtocolFile::GetFile(wxSocketClient& socket)
{
	wxByte buffer[1024];
	wxFileOutputStream *out;
	wxString localFileName;
	localFileName = wxFileName::CreateTempFileName(_T(FMC_APPNAME));
	bool moreDataToRead;
	bool isFirstSlice = true;
	if(localFileName.empty() == true)
	{
		return wxT("");
	}

	out = new wxFileOutputStream(localFileName);
	if(out->Ok() == false)
	{
		return wxT("");
	}
	moreDataToRead      = true;
	while(!socket.WaitForRead(0,0)){}


	_LogMsgInfo(wxT("Streaming socket data"), false);
	while(moreDataToRead == true)
	{
		if(isFirstSlice == true)
		{
			socket.Peek(buffer, 1024);
			wxInt32 pos = -1;
			wxUint32 streamPos = 0;
			if(socket.LastCount() != 0)
			{
				while( pos == -1)
				{
					if(buffer[streamPos] == 13 && buffer[streamPos+1] == 10 && buffer[streamPos+2] == 13 && buffer[streamPos+3] == 10)
						pos = streamPos;
					streamPos++;
				}
				if(pos > 0)
					socket.Read(buffer, pos+4);
			}
			isFirstSlice        = false;
		}

		socket.Read(buffer, 1024);

		if(socket.LastCount() != 0)
		{
			out->Write(buffer, socket.LastCount());
		}
		else
		{
			moreDataToRead = false;
		}
	}
	_LogMsgInfo(wxT("Socket emptied"), false);
	delete out;
	return localFileName;
}
