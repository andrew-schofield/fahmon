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
 * \file dataOutputStream.cpp
 * Writes out data files
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "dataOutputStream.h"

#include "base64Codec.h"


DataOutputStream::DataOutputStream(wxString const &filename)
{
	mDataOS = NULL;

	mFileOS = new wxFileOutputStream(filename);
	if(mFileOS != NULL)
	{
		mBufferedOS = new wxBufferedOutputStream(*mFileOS);
		if(mBufferedOS != NULL)
		{
			mDataOS = new wxDataOutputStream(*mBufferedOS);
		}
	}
}


DataOutputStream::~DataOutputStream(void)
{
	if(mDataOS != NULL)
	{
		delete mDataOS;
		mDataOS = NULL;
	}

	if(mBufferedOS != NULL)
	{
		mBufferedOS->Close();
		delete mBufferedOS;
		mBufferedOS = NULL;
	}

	if(mFileOS != NULL)
	{
		delete mFileOS;
		mFileOS = NULL;
	}
}


void DataOutputStream::WriteHiddenString(wxString const &value)
{
	// The string is written in base64
	WriteString(Base64Codec::Encode(value));
}
