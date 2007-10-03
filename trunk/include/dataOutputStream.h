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
 
#ifndef _DATAOUTPUTSTREAM_H
#define _DATAOUTPUTSTREAM_H


#include "wx/datstrm.h"
#include "wx/wfstream.h"


class DataOutputStream
{
protected:
	wxDataOutputStream     *mDataOS;
    wxFileOutputStream     *mFileOS;
    wxBufferedOutputStream *mBufferedOS;


public:
     DataOutputStream(const wxString& filename);
    ~DataOutputStream(void);

    void Write(const void *buffer, wxUint32 size) {mDataOS->Write8((const wxUint8*)buffer, size);}

    void WriteBool(bool value)              {mDataOS->Write8( (value == true ? 1 : 0) );}
    void WriteUint(wxUint32 value)          {mDataOS->Write32(value);}
    void WriteInt(wxInt32 value)            {mDataOS->Write32(value);}
    void WriteDouble(double value)          {mDataOS->WriteDouble(value);}
    void WriteString(const wxString& value) {mDataOS->WriteString(value);}
    
    bool Ok(void) const {return mDataOS != NULL;}
};


#endif /* _DATAOUTPUTSTREAM_H */
