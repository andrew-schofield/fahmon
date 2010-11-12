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
#include "ftpConnection.h"
#include "messagesManager.h"

#include "wx/filename.h"

wxMutex multiProtocolFile::mMutexFileExists;
wxMutex multiProtocolFile::mMutexLastModification;
wxMutex multiProtocolFile::mMutexDirExists;
wxMutex multiProtocolFile::mMutexGetLocalFileName;

bool multiProtocolFile::FileExists(const wxString& fileName)
{
	// ----- Access Lock -----
	wxMutexLocker lock(mMutexFileExists);
	// ----- Access Lock -----
	bool returnValue;
	long temp;
	returnValue = false;
	switch(GetFileProtocol(fileName))
	{
		case FILE:
			return wxFile::Exists(fileName);
			break;
		case HTTP:
		{
			switch(HTTPDownloader::GetHTTPResponseCode(fileName))
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
			break;
		}
		case FTP:
		{
			temp = FTPConnection::GetFTPResponseCode(fileName);
			if( temp >= 200 && temp < 300)
			{
				returnValue = true;
			}
			else
			{
				returnValue = false;
			}
			break;
		}
		default:
			returnValue = false;
	}
	return returnValue;
}


time_t multiProtocolFile::LastModification(const wxString& fileName)
{
	// ----- Access Lock -----
	wxMutexLocker lock(mMutexLastModification);
	// ----- Access Lock -----
	switch(GetFileProtocol(fileName))
	{
		case HTTP:
		{
			wxString lastMod;
			wxDateTime lastModTime;
			lastMod = HTTPDownloader::GetHTTPHeader(fileName, wxT("Last-Modified"));
			lastModTime.ParseDateTime(lastMod);
			return lastModTime.GetTicks();
		}
			break;
		case FTP:
			return FTPConnection::GetFTPFileModificationTime(fileName).GetTicks();
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
	// ----- Access Lock -----
	wxMutexLocker lock(mMutexDirExists);
	// ----- Access Lock -----
	bool returnValue = false;
	long temp;
	switch(GetFileProtocol(dirName))
	{
		case HTTP:
		{
			switch(HTTPDownloader::GetHTTPResponseCode(dirName))
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
		}
			break;
		case FTP:
		{
			temp = FTPConnection::GetFTPResponseCode(dirName);
			if( temp >= 200 && temp < 300)
			{
				returnValue = true;
			}
			else
			{
				returnValue = false;
			}
			break;
		}
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
	// ----- Access Lock -----
	wxMutexLocker lock(mMutexGetLocalFileName);
	// ----- Access Lock -----
	wxString fn;
	switch(GetFileProtocol(fileName))
	{
		case HTTP:
		{
			wxString localFileName;
			localFileName = wxFileName::CreateTempFileName(_T(FMC_APPNAME));
			if(!HTTPDownloader::GetHTTPFile(fileName, localFileName))
			{
				fn = wxT("");
				break;
			}
			fn = localFileName;
			break;
		}
		case FTP:
		{
			wxString localFileName;
			localFileName = wxFileName::CreateTempFileName(_T(FMC_APPNAME));
			if(!FTPConnection::GetFTPFile(fileName, localFileName))
			{
				fn = wxT("");
				break;
			}
			fn = localFileName;
			break;
		}
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
