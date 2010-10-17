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
 * \file dataInputStream.cpp
 * Reads in data files
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "dataInputStream.h"

#include "base64Codec.h"


DataInputStream::DataInputStream(const wxString& filename)
{
	mFileIS     = NULL;
	mDataIS     = NULL;
	mBufferedIS = NULL;

	// wxWidgets will pop up error messages when the file does not exist
	// so we avoid this by testing first the existence of the file
	if(wxFileExists(filename) == false)
	{
		return;
	}

	mFileIS = new wxFileInputStream(filename);
	if(mFileIS != NULL)
	{
		mBufferedIS = new wxBufferedInputStream(*mFileIS);
		if(mBufferedIS != NULL)
		{
			mDataIS = new wxDataInputStream(*mBufferedIS);
		}
	}
}


DataInputStream::~DataInputStream(void)
{
	if(mDataIS != NULL)
	{
		delete mDataIS;
		mDataIS = NULL;
	}

	if(mBufferedIS != NULL)
	{
		delete mBufferedIS;
		mBufferedIS = NULL;
	}

	if(mFileIS != NULL)
	{
		delete mFileIS;
		mFileIS = NULL;
	}
}


void DataInputStream::ReadHiddenString(wxString& value)
{
	wxString encodedString;

	ReadString(encodedString);

	// We must decode the read string
	value = Base64Codec::Decode(encodedString);
}
