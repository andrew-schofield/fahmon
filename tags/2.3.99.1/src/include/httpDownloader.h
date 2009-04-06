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
 * \file httpDownloader.h
 * Create a class to download files over http.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _HTTPDOWNLOADER_H
#define _HTTPDOWNLOADER_H


#include "wx/string.h"


#define PREF_HTTPDOWNLOADER_USEPROXY    wxT("HTTPDownloader.UseProxy") /**< Preference setting for whether a proxy should be used */
#define PREF_HTTPDOWNLOADER_USEPROXY_DV false /**< Default value for use of proxy setting */

#define PREF_HTTPDOWNLOADER_PROXYADDRESS    wxT("HTTPDownloader.ProxyAddress") /**< Preference setting for proxy address */
#define PREF_HTTPDOWNLOADER_PROXYADDRESS_DV wxT("") /**< Default value for proxy address */

#define PREF_HTTPDOWNLOADER_PROXYPORT    wxT("HTTPDownloader.ProxyPort") /**< Preference setting for proxy port */
#define PREF_HTTPDOWNLOADER_PROXYPORT_DV 8080 /**< Default value for proxy port */

#define PREF_HTTPDOWNLOADER_USE_PROXY_AUTHENTICATION    wxT("HTTPDownloader.UseProxyAuthentication") /**< Preference setting for whether proxy authentication should be used */
#define PREF_HTTPDOWNLOADER_USE_PROXY_AUTHENTICATION_DV false /**< Default value for proxy authentication */

#define PREF_HTTPDOWNLOADER_PROXY_USERNAME    wxT("HTTPDownloader.ProxyUsername") /**< Preference setting for proxy username */
#define PREF_HTTPDOWNLOADER_PROXY_USERNAME_DV wxT("") /**< Default value for proxy username */

#define PREF_HTTPDOWNLOADER_PROXY_PASSWORD    wxT("HTTPDownloader.ProxyPassword") /**< Preference setting for proxy password */
#define PREF_HTTPDOWNLOADER_PROXY_PASSWORD_DV wxT("") /**< Default value for proxy password */

#define PREF_HTTPDOWNLOADER_USEALTERNATEUPDATE    wxT("HTTPDownloader.UseAlternateUpdate") /**< Preference setting for use of alternate update source */
#define PREF_HTTPDOWNLOADER_USEALTERNATEUPDATE_DV true /**< Default value for alternate update source */

#define PREF_HTTPDOWNLOADER_USELOCALFILE    wxT("HTTPDownloader.UseLocalFile") /**< Preference setting for use of a local file as update source */
#define PREF_HTTPDOWNLOADER_USELOCALFILE_DV false /**< Default value for local file */

#define PREF_HTTPDOWNLOADER_LOCALFILELOCATION    wxT("HTTPDownloader.LocalFileLocation") /**< Preference setting for local file location */
#define PREF_HTTPDOWNLOADER_LOCALFILELOCATION_DV wxT("") /**< Default value for local file location */

#define PREF_HTTPDOWNLOADER_ALTERNATEUPDATEADDRESS    wxT("HTTPDownloader.AlternateUpdateAddress") /**< Preference setting for alternate update address */
#define PREF_HTTPDOWNLOADER_ALTERNATEUPDATEADDRESS_DV wxT("http://fah-web.stanford.edu/psummary.html") /**< Default value for alternate update address */


/**
 * HTTPDownloader class.
 * This class allows one to easily download files from a web server.
 **/
class HTTPDownloader
{
	public:
		/**
		 * Download a file via HTTP to the specified (temporary) location.
		 * @param url The file to download.
		 * @param tempFile The location to store the downloaded file.
		 * @return Success of operation.
		 **/
		static bool GetHTTPFile(wxString url, wxString tempFile);

		/**
		 * Grab the response code form an url.
		 * @param url The url to query
		 * @return The HTTP response code
		 **/
		static long GetHTTPResponseCode(wxString url);

		/**
		 * Retrieve a specific header from an HTTP HEAD request.
		 * @param url The url to query
		 * @param header The header required
		 * @return Resulting header value if any.
		 **/
		static wxString GetHTTPHeader(wxString url, wxString header);
};


#endif /* _HTTPDOWNLOADER_H */
