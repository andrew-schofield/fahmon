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
 
#ifndef _PREFERENCE_H
#define _PREFERENCE_H

#include "wx/wx.h"
#include "fahmonTypes.h"
#include "dataInputStream.h"
#include "dataOutputStream.h"


// Storage class for the value of a preference
class Preference
{
protected:

    // Order *MUST* not be changed
    // This is for compatibility with older versions
    enum _PREF_TYPE
    {
        PT_BOOL,
        PT_UINT,
        PT_INT,
        PT_DOUBLE,
        PT_STRING,
        PT_UNKNOWN,
        PT_HIDDEN_STRING        // Used for string that should not be directly "human-readable" from the disk
    };


    PrefType mPrefType;
    wxString mPrefName;
    wxString mStringValue;      // Used for both PT_STRING and PT_HIDDEN_STRING
    wxUint32 mUintValue;
    wxInt32  mIntValue;
    double   mDoubleValue;
    bool     mBoolValue;


public:
    Preference(void);
    Preference(wxString name, bool value);
    Preference(wxString name, double value);
    Preference(wxString name, wxInt32 value);
    Preference(wxString name, wxUint32 value);
    Preference(wxString name, const wxString& value, bool isHidden = false);

    void Read(DataInputStream& in);
    void Write(DataOutputStream& out) const;

    bool IsABoolPref(void)          const {return mPrefType == PT_BOOL;}
    bool IsAnIntPref(void)          const {return mPrefType == PT_INT;}
    bool IsAnUintPref(void)         const {return mPrefType == PT_UINT;}
    bool IsADoublePref(void)        const {return mPrefType == PT_DOUBLE;}
    bool IsAStringPref(void)        const {return mPrefType == PT_STRING;}
    bool IsAnHiddenStringPref(void) const {return mPrefType == PT_HIDDEN_STRING;}

    // --- Getters
    bool     GetBoolValue(void)         const {return mBoolValue;}
    double   GetDoubleValue(void)       const {return mDoubleValue;}
    wxInt32  GetIntValue(void)          const {return mIntValue;}
    wxString GetName(void)              const {return mPrefName;}
    wxString GetStringValue(void)       const {return mStringValue;}
    wxString GetHiddenStringValue(void) const {return mStringValue;}
    wxUint32 GetUintValue(void)         const {return mUintValue;}
};


#endif /* _PREFERENCE_H */
