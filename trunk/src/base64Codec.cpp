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

#include "fahmon.h"
#include "base64Codec.h"

#include "messagesManager.h"

/**
 * Base64 mapping table.
 * Maps each character to its base64 encoded index.
 **/
const wxChar* Base64Codec::mBase64Table = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");


/**
 * Transform a base64 index into the corresponding character.
 * @param index The index in mBase64Table.
 * @return The character (wxChar) retrieved from mBase64Table
 **/
wxChar Base64Codec::Base64IndexToChar(wxUint32 index)
{
	if(index < 64)
		return mBase64Table[index];

	_LogMsgError(_T("Base64Codec: The given base64 index is greater than 63!"));

	// This character does not belong to the table, it is normally used for padding
	return '=';
}


/**
 * Transform a base64 character into the corresponding index.
 * @param c The character in mBase64Table.
 * @return The index of that character in mBase64Table.
 **/
wxUint32 Base64Codec::CharToBase64Index(wxChar c)
{
	if(c >= 'A' && c <= 'Z')
		return c - 'A';

	if(c >= 'a' && c <= 'z')
		return c - 'a' + 26;

	if(c >= '0' && c <= '9')
		return c - '0' + 52;

	if(c == '+')
		return 62;

	if(c == '/')
		return 63;

	_LogMsgError(_T("Base64Codec: The given base64 character is invalid!"));

	return 0;
}


/**
 * Transform a 'normal' string into a base64 encoded string.
 * @param string The string to be encoded.
 * @return The base64 encoded string.
 **/
wxString Base64Codec::Encode(const wxString& string)
{
	wxChar   c1;
	wxChar   c2;
	wxChar   c3;
	wxInt32  i;      // Must NOT be Uint, because 'string.Len()-2' can be smaller than 0
	wxString result;

	result = wxT("");

	// Consider each block of 3 8-bits blocks and transform them in 4 6-bits blocks
	for(i=0; i<(wxInt32)string.Len()-2; i+=3)
	{
		c1 = string[i];
		c2 = string[i+1];
		c3 = string[i+2];

		result += Base64IndexToChar(c1 >> 2);
		result += Base64IndexToChar((c1 << 4 | c2 >> 4) & 63);
		result += Base64IndexToChar((c2 << 2 | c3 >> 6) & 63);
		result += Base64IndexToChar(c3 & 63);
	}

	// If the length of the string was a multiple of 3, everything is Ok
	// However, there can be 1 or 2 characters left, so we have to use a special case for them
	// The length of the base64 encoded string must be a multiple of 4, so '=' characters are used for padding

	// 1 character left
	if(i == (wxInt32)string.Len()-1)
	{
		c1 = string[i];

		result += Base64IndexToChar(c1 >> 2);
		result += Base64IndexToChar((c1 << 4) & 63);
		result += '=';
		result += '=';
	}
	// 2 characters left
	else if(i == (wxInt32)string.Len()-2)
	{
		c1 = string[i];
		c2 = string[i+1];

		result += Base64IndexToChar(c1 >> 2);
		result += Base64IndexToChar((c1 << 4 | c2 >> 4) & 63);
		result += Base64IndexToChar((c2 << 2) & 63);
		result += '=';
	}

	return result;
}


/**
 * Decode a base64 encoded string.
 * @param string The base64 string to be decoded.
 * @return The decoded string.
 **/
wxString Base64Codec::Decode(const wxString& string)
{
	wxUint32 i;
	wxUint32 base64Index1;
	wxUint32 base64Index2;
	wxUint32 base64Index3;
	wxUint32 base64Index4;
	wxString result;

	result = wxT("");

	// Base64 encoded strings must have a length multiple of 4
	if(string.Len()%4 != 0)
	{
		_LogMsgError(_T("Base64Codec: The base64 encoded string has an incorrect length!"));
		return result;
	}

	// Each group of 4 base64 characters provides 3 decoded characters
	for(i=0; i<string.Len(); i+=4)
	{
		// The first two characters of a group always provide a valid decoded character
		base64Index1 = CharToBase64Index(string[i]);
		base64Index2 = CharToBase64Index(string[i+1]);

		result += (wxChar)(base64Index1 << 2 | base64Index2 >> 4);

		// '=' character indicates that the there is no more character to decode
		if(string[i+2] != '=')
		{
			base64Index3 = CharToBase64Index(string[i+2]);
			result += (wxChar)(base64Index2 << 4 | base64Index3 >> 2);

			// Same thing for the last character
			if(string[i+3] != '=')
			{
				base64Index4 = CharToBase64Index(string[i+3]);
				result += (wxChar)(base64Index3 << 6 | base64Index4);
			}
		}
	}

	return result;
}
