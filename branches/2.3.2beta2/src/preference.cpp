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

#include "preference.h"

#include "tools.h"


/**
* Constructor
**/
Preference::Preference(void)
{
	mPrefName = wxT("");
	mPrefType = PT_UNKNOWN;
}


/**
* Constructor
**/
Preference::Preference(wxString name, bool value)
{
	mPrefType  = PT_BOOL;
	mPrefName  = name;
	mBoolValue = value;
}


/**
* Constructor
**/
Preference::Preference(wxString name, wxUint32 value)
{
	mPrefType  = PT_UINT;
	mPrefName  = name;
	mUintValue = value;
}


/**
* Constructor
**/
Preference::Preference(wxString name, wxInt32 value)
{
	mPrefType = PT_INT;
	mPrefName = name;
	mIntValue = value;
}


/**
* Constructor
**/
Preference::Preference(wxString name, double value)
{
	mPrefType    = PT_DOUBLE;
	mPrefName    = name;
	mDoubleValue = value;
}


/**
* Constructor
**/
Preference::Preference(wxString name, const wxString& value, bool isHidden)
{
	// Hidden strings are stored in the same way as 'normal' strings
	// Only the method used to read/write them is different
	if(isHidden == true)
	{
		mPrefType = PT_HIDDEN_STRING;
	}
	else
	{
		mPrefType = PT_STRING;
	}

	mPrefName    = name;
	mStringValue = value;
}


/**
* Read the preference from the given input stream
**/
void Preference::Read(DataInputStream& in)
{
	// 1) The type
	in.Read(&mPrefType, sizeof(PrefType));

	// 2) The name
	in.ReadString(mPrefName);

	// 3) The value
	switch(mPrefType)
	{
		case PT_BOOL:
			in.ReadBool(mBoolValue);
			break;

		case PT_UINT:
			in.ReadUint(mUintValue);
			break;

		case PT_INT:
			in.ReadInt(mIntValue);
			break;

		case PT_DOUBLE:
			in.ReadDouble(mDoubleValue);
			break;

		case PT_STRING:
			in.ReadString(mStringValue);
			break;

		case PT_HIDDEN_STRING:
			in.ReadHiddenString(mStringValue);
			break;

		// We should never fall here
		default:
			wxASSERT(false);
			break;
	}
}


/**
* Write this preference to the given output stream
**/
void Preference::Write(DataOutputStream& out) const
{
	// 1) The type
	out.Write(&mPrefType, sizeof(PrefType));

	// 2) The name
	out.WriteString(mPrefName);

	// 3) The value
	switch(mPrefType)
	{
		case PT_BOOL:
			out.WriteBool(mBoolValue);
			break;

		case PT_UINT:
			out.WriteUint(mUintValue);
			break;

		case PT_INT:
			out.WriteInt(mIntValue);
			break;

		case PT_DOUBLE:
			out.WriteDouble(mDoubleValue);
			break;

		case PT_STRING:
			out.WriteString(mStringValue);
			break;

		case PT_HIDDEN_STRING:
			out.WriteHiddenString(mStringValue);
			break;

		// We should never fall here
		default:
			wxASSERT(false);
			break;
	}
}
