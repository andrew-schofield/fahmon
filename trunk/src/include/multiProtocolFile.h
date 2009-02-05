/*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/**
 * \file multiProtocolFile.h
 * Creates a class for managing files across different protocols (local and http).
 * \author Andrew Schofield
 **/

#ifndef _MULTIPROTOCOLFILE_H
#define _MULTIPROTOCOLFILE_H

#include "fahmon.h"
#include "fahmonTypes.h"

#include "wx/protocol/http.h"
#include "wx/uri.h"
#include "wx/filename.h"

class multiProtocolFile
{
	public:
		enum _PROTOCOL_ID
		{
			FILE,
			HTTP,
			FTP,
			PROTOCOL_ID_COUNT
		};

		static bool FileExists(const wxString& fileName);

		static time_t LastModification(const wxString& fileName);

		static bool DirExists(const wxString& dirName);

		static wxString GetLocalFileName(const wxString& fileName);

		static bool CopyFile(const wxString& inFile, const wxString& outFile);

		static FileProtocol GetFileProtocol(const wxString& fileName);
	private:

		static bool SetHTTPConnection(wxSocketClient& socket, wxURI uri, wxInt32 byteRange = 0);

		static wxUint32 GetHTTPResponseCode(wxSocketClient& socket);

		static wxString GetHTTPHeader(wxSocketClient& socket, wxString header);

		static wxString GetFile(wxSocketClient& socket);
};

#endif /* _MULTIPROTOCOLFILE_H */
