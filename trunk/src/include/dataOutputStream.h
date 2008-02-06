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
 * \file dataOutputStream.h
 * Writes out data files
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _DATAOUTPUTSTREAM_H
#define _DATAOUTPUTSTREAM_H


#include "wx/datstrm.h"
#include "wx/wfstream.h"

/**
 * Stream Writer Class.
 * This class creates an object for writing data from file streams.
 **/
class DataOutputStream
{
protected:
	wxDataOutputStream     *mDataOS; /**< A generic data output stream */
	wxFileOutputStream     *mFileOS; /**< A file output stream */
	wxBufferedOutputStream *mBufferedOS; /**< A buffered output stream */


public:
	/**
	 * Contructor.
	 * Contructs the dataOutputStream class.
	 **/
	DataOutputStream(const wxString& filename);

	/**
	 * Destructor.
	 **/
	~DataOutputStream(void);

	/**
	 * Write bytes to the output stream.
	 * @param *buffer Pointer to the input buffer.
	 * @param size Length of the data to write.
	 **/
	void Write(const void *buffer, wxUint32 size) {mDataOS->Write8((const wxUint8*)buffer, size);}

	/**
	 * Write a boolean value to output stream
	 * @param value Object containing boolean value to write.
	 **/
	void WriteBool(bool value)                      {mDataOS->Write8( (value == true ? 1 : 0) );}

	/**
	 * Write an unsigned int32 value to output stream
	 * @param value Object containing Uint32 value to write.
	 **/
	void WriteUint(wxUint32 value)                  {mDataOS->Write32(value);}

	/**
	 * Write a signed int32 value to output stream
	 * @param value Object containing Int32 value to write.
	 **/
	void WriteInt(wxInt32 value)                    {mDataOS->Write32(value);}

	/**
	 * Write a double value to output stream
	 * @param value Object containing double value to write.
	 **/
	void WriteDouble(double value)                  {mDataOS->WriteDouble(value);}

	/**
	 * Write a string value to output stream
	 * @param value Object containing string value to write.
	 **/
	void WriteString(const wxString& value)         {mDataOS->WriteString(value);}

	/**
	 * Write an "encrypted" string value to output stream
	 * @param value Object containing "encrypted" string value to write.
	 **/
	void WriteHiddenString(const wxString& value);

	/**
	 * Checks if string can be used.
	 * @return Can stream be used?
	 **/
	bool Ok(void) const {return mDataOS != NULL;}
};


#endif /* _DATAOUTPUTSTREAM_H */
