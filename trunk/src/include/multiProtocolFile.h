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
 * Creates a class of statics for managing files across different protocols (local and http).
 * \author Andrew Schofield
 **/

#ifndef _MULTIPROTOCOLFILE_H
#define _MULTIPROTOCOLFILE_H

#include "fahmon.h"
#include "fahmonTypes.h"

#include "wx/protocol/http.h"
#include "wx/uri.h"
#include "wx/filename.h"
#include "wx/thread.h"

/**
 * Abstraction class to hide any internal file protocol messiness behind a single interface.
 * Not a <i>real</i> class, but rather a collection of static methods that link to other classes.
 **/
class multiProtocolFile
{
	public:
		/**
		 * The currently supported protocols.
		 * Adding more is simple, just add a new ID and modify the statics in \file multiProtocolFile.cpp
		 * to deal with the new protocol.
		 **/
		enum _PROTOCOL_ID
		{
			FILE, /**< Local files */
			HTTP, /**< Files accessible via http:// (no support for SSL) */
			FTP, /**< Files accessible via ftp:// */
			PROTOCOL_ID_COUNT
		};

		/**
		 * Checks if file exists.
		 * @param fileName File to check
		 * @return existence
		 **/
		static bool FileExists(const wxString& fileName);

		static wxMutex          mMutexFileExists;

		/**
		 * Retrieves time of last modification of a file.
		 * @param fileName File to check
		 * @return time of last modification
		 **/
		static time_t LastModification(const wxString& fileName);

		static wxMutex          mMutexLastModification;

		/**
		 * Checks if directory exists.
		 * @param dirName directory to check
		 * @return existence
		 **/
		static bool DirExists(const wxString& dirName);

		static wxMutex          mMutexDirExists;

		/**
		 * Get sanitized filename.
		 * This allows all files to be accessed as if local by silently downloading
		 * http/ftp files in the background. Currently this will leave messy remains
		 * in your temp folder, as no cleanup is performed. It essentially acts as a
		 * web browser with an unlimited cache size.
		 * @param fileName File to retrieve
		 * @return sanitized local filename
		 **/
		static wxString GetLocalFileName(const wxString& fileName);

		static wxMutex          mMutexGetLocalFileName;

		/**
		 * Copy file.
		 * @param inFile File to copy from, may be FILE, HTTP or FTP
		 * @param outFile File to copy to. Must be FILE.
		 * @return success
		 **/
		static bool CopyFile(const wxString& inFile, const wxString& outFile);

		/**
		 * Get protocol from filename.
		 * @param fileName File to check
		 * @return File Protocol of file.
		 **/
		static FileProtocol GetFileProtocol(const wxString& fileName);
	private:
};

#endif /* _MULTIPROTOCOLFILE_H */
