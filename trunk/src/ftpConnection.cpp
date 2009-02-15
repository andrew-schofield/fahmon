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
 * \file ftpConnection.cpp
 * Create a class to download files over ftp.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "ftpConnection.h"

#include "tools.h"
#include "preferencesManager.h"
#include "messagesManager.h"


bool FTPConnection::GetFTPFile(wxString url, wxString tempFile)
{
	return true;
}


long FTPConnection::GetFTPResponseCode(wxString url)
{
	return 0;
}


wxString FTPConnection::GetFTPHeader(wxString url, wxString header)
{
	return wxT("");
}
