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
 
#ifndef _HTTPDOWNLOADER_H
#define _HTTPDOWNLOADER_H


#include "wx/string.h"
#include "progressManager.h"


/**
 * Preferences used by this class
**/
#define PREF_HTTPDOWNLOADER_USEPROXY    wxT("HTTPDownloader.UseProxy")
#define PREF_HTTPDOWNLOADER_USEPROXY_DV false

#define PREF_HTTPDOWNLOADER_PROXYADDRESS    wxT("HTTPDownloader.ProxyAddress")
#define PREF_HTTPDOWNLOADER_PROXYADDRESS_DV wxT("")

#define PREF_HTTPDOWNLOADER_PROXYPORT    wxT("HTTPDownloader.ProxyPort")
#define PREF_HTTPDOWNLOADER_PROXYPORT_DV 8080


// This class allows one to easily download files from a web server
class HTTPDownloader
{
public:
    typedef enum _DOWNLOAD_STATUS
    {
        STATUS_TEMP_FILE_CREATION_ERROR,
        STATUS_TEMP_FILE_OPEN_ERROR,
        STATUS_CONNECT_ERROR,
        STATUS_SEND_REQUEST_ERROR,
        STATUS_ABORTED,
        STATUS_NO_ERROR
    } DownloadStatus;


protected:
    static wxUint32 ExtractContentSize(wxByte* buffer, wxUint32 bufferSize);
    
    
public:
    static DownloadStatus DownloadFile(const wxString& host, wxUint32 port, const wxString& resource, wxString& localFileName, ProgressManager& progressManager);
};


#endif /* _HTTPDOWNLOADER_H */
