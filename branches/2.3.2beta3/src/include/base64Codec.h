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
 * \file base64Codec
 * Creates a class for encoding/decoding to base64.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _BASE64CODEC_H
#define _BASE64CODEC_H


#include "wx/string.h"


/**
 * A base 64 codec class.
 * Implements a base 64 string codec used for proxy authentication.
 **/
class Base64Codec
{
protected:
	/**
	 * Base64 mapping table.
	 * Maps each character to its base64 encoded index.
	 **/
	static const wxChar* mBase64Table;

	/**
	 * Transform a base64 index into the corresponding character.
	 * @param index The index in mBase64Table.
	 * @return The character (wxChar) retrieved from mBase64Table
	 **/
	static wxChar   Base64IndexToChar(wxUint32 index);

	/**
	 * Transform a base64 character into the corresponding index.
	 * @param c The character in mBase64Table.
	 * @return The index of that character in mBase64Table.
	 **/
	static wxUint32 CharToBase64Index(wxChar c);

public:
	/**
	 * Transform a 'normal' string into a base64 encoded string.
	 * @param string The string to be encoded.
	 * @return The base64 encoded string.
	 **/
	static wxString Encode(const wxString& string);

	/**
	 * Decode a base64 encoded string.
	 * @param string The base64 string to be decoded.
	 * @return The decoded string.
	 **/
	static wxString Decode(const wxString& string);
};


#endif /* _BASE64CODEC_H */
