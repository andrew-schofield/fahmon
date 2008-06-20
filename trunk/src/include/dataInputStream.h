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
 * \file dataInputStream.h
 * Reads in data files
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _DATAINPUTSTREAM_H
#define _DATAINPUTSTREAM_H


#include "wx/datstrm.h"
#include "wx/wfstream.h"

/**
 * Stream Reader Class.
 * This class creates an object for reading data from file streams.
 **/
class DataInputStream
{
protected:
	wxDataInputStream     *mDataIS; /**< A generic data input stream */
	wxFileInputStream     *mFileIS; /**< A file input stream */
	wxBufferedInputStream *mBufferedIS; /**< A buffered input stream */


public:
	/**
	 * Contructor.
	 * Contructs the dataInputStream class.
	 **/
	DataInputStream(const wxString& filename);

	/**
	 * Destructor.
	 **/
	~DataInputStream(void);

	/**
	 * Checks if data stream can be used.
	 * @return Can stream be used?
	 **/
	bool Ok(void) const {return mDataIS != NULL;}

	/**
	 * Read bytes from the input stream.
	 * @param *buffer Pointer to the output buffer.
	 * @param size Length of data to read.
	 **/
	void Read(void *buffer, wxUint32 size)  {mDataIS->Read8((wxUint8*)buffer, size);}

	/**
	 * Read a boolean value from input stream
	 * @param value Reference to object to store boolean value.
	 **/
	void ReadBool(bool& value)              {value = (mDataIS->Read8() == 1 ? true : false);}

	/**
	 * Read an unsigned int32 value from input stream
	 * @param value Reference to object to store Uint32 value.
	 **/
	void ReadUint(wxUint32& value)          {value = mDataIS->Read32();}

	/**
	 * Read a signed int32 value from input stream
	 * @param value Reference to object to store Int32 value.
	 **/
	void ReadInt(wxInt32& value)            {value = mDataIS->Read32();}

	/**
	 * Read a double value from input stream
	 * @param value Reference to object to store double value.
	 **/
	void ReadDouble(double& value)          {value = mDataIS->ReadDouble();}

	/**
	 * Read a string value from input stream
	 * @param value Reference to object to store string value.
	 **/
	void ReadString(wxString& value)        {value = mDataIS->ReadString();}

	/**
	 * Read an "encrypted" string value from input stream
	 * @param value Reference to object to store "encrypted" string value.
	 **/
	void ReadHiddenString(wxString& value);
};


#endif /* _DATAINPUTSTREAM_H */
