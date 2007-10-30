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

#ifndef _DATAINPUTSTREAM_H
#define _DATAINPUTSTREAM_H


#include "wx/datstrm.h"
#include "wx/wfstream.h"


class DataInputStream
{
protected:
	wxDataInputStream     *mDataIS;
	wxFileInputStream     *mFileIS;
	wxBufferedInputStream *mBufferedIS;


public:
	DataInputStream(const wxString& filename);
	~DataInputStream(void);

	bool Ok(void) const {return mDataIS != NULL;}

	void Read(void *buffer, wxUint32 size)  {mDataIS->Read8((wxUint8*)buffer, size);}
	void ReadBool(bool& value)              {value = (mDataIS->Read8() == 1 ? true : false);}
	void ReadUint(wxUint32& value)          {value = mDataIS->Read32();}
	void ReadInt(wxInt32& value)            {value = mDataIS->Read32();}
	void ReadDouble(double& value)          {value = mDataIS->ReadDouble();}
	void ReadString(wxString& value)        {value = mDataIS->ReadString();}
	void ReadHiddenString(wxString& value);
};


#endif /* _DATAINPUTSTREAM_H */
