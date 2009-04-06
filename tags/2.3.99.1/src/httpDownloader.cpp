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
#include "preferencesManager.h"
#include "messagesManager.h"

#include "wx/curl/http.h"
#include "wx/tokenzr.h"


bool HTTPDownloader::GetHTTPFile(wxString url, wxString tempFile)
{
	bool isUsingProxy;
	wxString proxyAddress;
	wxUint32 proxyPort;
	bool proxyNeedsAuthentication;
	wxString proxyUsername;
	wxString proxyPassword;
	wxString agent;

	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USEPROXY,                   isUsingProxy);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXYADDRESS,               proxyAddress);
	_PrefsGetUint        (PREF_HTTPDOWNLOADER_PROXYPORT,                  proxyPort);
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USE_PROXY_AUTHENTICATION,   proxyNeedsAuthentication);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXY_USERNAME,             proxyUsername);
	_PrefsGetHiddenString(PREF_HTTPDOWNLOADER_PROXY_PASSWORD,             proxyPassword);

	agent = wxString::Format(wxT("%s/%s"), _T(FMC_APPNAME), _T(FMC_VERSION));

	wxCurlHTTP http(url);
	http.UseProxy(isUsingProxy);
	http.SetProxyHost(proxyAddress);
	http.SetProxyPort(proxyPort);
	http.SetProxyUsername(proxyUsername);
	http.SetProxyPassword(proxyPassword);
	http.SetOpt(CURLOPT_USERAGENT, (const char*)agent.c_str());

	if(!http.Get(tempFile))
	{
		if(tempFile.IsEmpty() == false)
		{
			wxRemoveFile(tempFile);
		}
		return false;
	}
	else
		return true;
}


long HTTPDownloader::GetHTTPResponseCode(wxString url)
{
	bool isUsingProxy;
	wxString proxyAddress;
	wxUint32 proxyPort;
	bool proxyNeedsAuthentication;
	wxString proxyUsername;
	wxString proxyPassword;
	wxString agent;

	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USEPROXY,                   isUsingProxy);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXYADDRESS,               proxyAddress);
	_PrefsGetUint        (PREF_HTTPDOWNLOADER_PROXYPORT,                  proxyPort);
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USE_PROXY_AUTHENTICATION,   proxyNeedsAuthentication);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXY_USERNAME,             proxyUsername);
	_PrefsGetHiddenString(PREF_HTTPDOWNLOADER_PROXY_PASSWORD,             proxyPassword);

	agent = wxString::Format(wxT("%s/%s"), _T(FMC_APPNAME), _T(FMC_VERSION));

	wxCurlHTTP http(url);
	http.UseProxy(isUsingProxy);
	http.SetProxyHost(proxyAddress);
	http.SetProxyPort(proxyPort);
	http.SetProxyUsername(proxyUsername);
	http.SetProxyPassword(proxyPassword);
	http.SetOpt(CURLOPT_USERAGENT, (const char*)agent.c_str());

	http.Head();
	return http.GetResponseCode();
}


wxString HTTPDownloader::GetHTTPHeader(wxString url, wxString header)
{
	bool isUsingProxy;
	wxString proxyAddress;
	wxUint32 proxyPort;
	bool proxyNeedsAuthentication;
	wxString proxyUsername;
	wxString proxyPassword;
	wxString agent;

	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USEPROXY,                   isUsingProxy);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXYADDRESS,               proxyAddress);
	_PrefsGetUint        (PREF_HTTPDOWNLOADER_PROXYPORT,                  proxyPort);
	_PrefsGetBool        (PREF_HTTPDOWNLOADER_USE_PROXY_AUTHENTICATION,   proxyNeedsAuthentication);
	_PrefsGetString      (PREF_HTTPDOWNLOADER_PROXY_USERNAME,             proxyUsername);
	_PrefsGetHiddenString(PREF_HTTPDOWNLOADER_PROXY_PASSWORD,             proxyPassword);

	agent = wxString::Format(wxT("%s/%s"), _T(FMC_APPNAME), _T(FMC_VERSION));

	wxCurlHTTP http(url);
	http.UseProxy(isUsingProxy);
	http.SetProxyHost(proxyAddress);
	http.SetProxyPort(proxyPort);
	http.SetProxyUsername(proxyUsername);
	http.SetProxyPassword(proxyPassword);
	http.SetOpt(CURLOPT_USERAGENT, (const char*)agent.c_str());

	http.Head();

	wxString output = wxT("");
	wxStringTokenizer tkz(http.GetResponseHeader(), wxT("\n"));
	while ( tkz.HasMoreTokens() )
	{
		wxString token = tkz.GetNextToken();

		if(token.BeforeFirst(':').Strip(wxString::both) == header)
			return token.AfterFirst(':').Strip(wxString::both);
	}
	return output;
}
