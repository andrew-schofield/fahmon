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
 * \file ftpDownloader.h
 * Create a class to download files over ftp.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _FTPDOWNLOADER_H
#define _FTPDOWNLOADER_H


#include "wx/string.h"


#define PREF_FTPDOWNLOADER_USEPROXY    wxT("FTPDownloader.UseProxy") /**< Preference setting for whether a proxy should be used */
#define PREF_FTPDOWNLOADER_USEPROXY_DV false /**< Default value for use of proxy setting */

#define PREF_FTPDOWNLOADER_PROXYADDRESS    wxT("FTPDownloader.ProxyAddress") /**< Preference setting for proxy address */
#define PREF_FTPDOWNLOADER_PROXYADDRESS_DV wxT("") /**< Default value for proxy address */

#define PREF_FTPDOWNLOADER_PROXYPORT    wxT("FTPDownloader.ProxyPort") /**< Preference setting for proxy port */
#define PREF_FTPDOWNLOADER_PROXYPORT_DV 8080 /**< Default value for proxy port */

#define PREF_FTPDOWNLOADER_USE_PROXY_AUTHENTICATION    wxT("FTPDownloader.UseProxyAuthentication") /**< Preference setting for whether proxy authentication should be used */
#define PREF_FTPDOWNLOADER_USE_PROXY_AUTHENTICATION_DV false /**< Default value for proxy authentication */

#define PREF_FTPDOWNLOADER_PROXY_USERNAME    wxT("FTPDownloader.ProxyUsername") /**< Preference setting for proxy username */
#define PREF_FTPDOWNLOADER_PROXY_USERNAME_DV wxT("") /**< Default value for proxy username */

#define PREF_FTPDOWNLOADER_PROXY_PASSWORD    wxT("FTPDownloader.ProxyPassword") /**< Preference setting for proxy password */
#define PREF_FTPDOWNLOADER_PROXY_PASSWORD_DV wxT("") /**< Default value for proxy password */


/**
 * FTPDownloader class.
 * This class allows one to easily download files from a web server.
 **/
class FTPDownloader
{
	public:
		/**
		 * Download a file via FTP to the specified (temporary) location.
		 * @param url The file to download.
		 * @param tempFile The location to store the downloaded file.
		 * @return Success of operation.
		 **/
		static bool GetFTPFile(wxString url, wxString tempFile);

		/**
		 * Grab the response headers from an ftp request and get the response code.
		 * @param url The url to query
		 * @return The FTP response code
		 **/
		static long GetFTPResponseCode(wxString url);

		static wxString GetFTPHeader(wxString url, wxString header);
};


#endif /* _FTPDOWNLOADER_H */
