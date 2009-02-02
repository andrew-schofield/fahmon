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
#include "progressManager.h"


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
		 * Download Status definition.
		 **/
		typedef enum _DOWNLOAD_STATUS
		{
			STATUS_TEMP_FILE_CREATION_ERROR, /**< Temporary file creation error */
			STATUS_TEMP_FILE_OPEN_ERROR, /**< Temporary file opening error */
			STATUS_CONNECT_ERROR, /**< Connection error */
			STATUS_SEND_REQUEST_ERROR, /**< Send request error */
			STATUS_ABORTED, /**< Connection aborted */
			STATUS_NO_ERROR /**< Connection successful */
		} DownloadStatus;


	protected:
		/**
		 * Get the size of the downloaded content.
		 * Uses the content-size field.
		 * If no field is found, value is 0.
		 * ! buffer will be modified by this function!
		 * @param buffer The downloaded content buffer.
		 * @param bufferSize The size of the downloaded content buffer.
		 * @return The content size.
		 **/
		static wxUint32 ExtractContentSize(wxByte* buffer, wxUint32 bufferSize);

	public:
		/**
		 * Create a temporary file and download the remote resource to this file.
		 *
		 * The path to the temporary file is stored in the parameter localFileName, empty string if it could not be created
		 * This file must be deleted by the caller, if this is needed, regardless of the return value
		 *
		 * Use the defined proxy in preferences, if any
		 *
		 * Using wxURL would have been a lot easier, but wxURL::SetProxy() doesn't work, and I can't ask
		 * users to patch their version of wxWindows to compile this app
		 * @param host host part of the url
		 * @param port port part of url (default is 80)
		 * @param resource location of file on host
		 * @param localFileName Location to download file to
		 * @param progressManager The progressManager object
		 * @return A download status object.
		 **/
		static DownloadStatus DownloadFile(wxString const &host, wxUint32 port, wxString const &resource, wxString& localFileName, ProgressManager& progressManager, wxInt32 byteRange = 0);

		/**
		 * Decontructs a "real" url into its Server, Port and Resource fields.
		 * This is a wrapper around the DownloadFile method to allow access by a readable url.
		 * @param url The real url like 'http://fah-web.stanford.edu/psummary.html'
		 * @param localFileName Location to download file to
		 * @param progressManager The progressManager object
		 * @return A download status object.
		 **/
		static DownloadStatus Url(wxString const &url, wxString& localFileName, ProgressManager& progressManager, wxInt32 byteRange = 0);
};


#endif /* _HTTPDOWNLOADER_H */
