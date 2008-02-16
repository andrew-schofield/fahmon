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
 * \file preference.h
 * Preference definition class.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _PREFERENCE_H
#define _PREFERENCE_H

#include "wx/wx.h"
#include "fahmonTypes.h"
#include "dataInputStream.h"
#include "dataOutputStream.h"


/**
 * Class for storing preference values.
 **/
class Preference
{
protected:
	/**
	 * Preference definition.
	 * The order must not be changed to maintain compatibility with older versions.
	 **/
	enum _PREF_TYPE
	{
		PT_BOOL, /**< Boolean preference */
		PT_UINT, /**< Unsigned integer preference */
		PT_INT, /**< Signed integer preference */
		PT_DOUBLE, /**< Double preference */
		PT_STRING, /**< String preference */
		PT_UNKNOWN, /**< Unknown preference */
		PT_HIDDEN_STRING /**< Hidden string. Used for string that should not be directly "human-readable" from the disk */
	};


	PrefType mPrefType; /**< The preference type */
	wxString mPrefName; /**< Preference name */
	wxString mStringValue; /**< String value. Used for both PT_STRING and PT_HIDDEN_STRING */
	wxUint32 mUintValue; /**< Unsigned integer value */
	wxInt32  mIntValue; /**< Signed integer value */
	double   mDoubleValue; /**< Double value */
	bool     mBoolValue; /**< Boolean value */


public:
	/**
	 * Null contructor.
	 **/
	Preference(void);

	/**
	 * Constructor for boolean prefs.
	 * @param name Preference name.
	 * @param value Boolean value
	 **/
	Preference(wxString name, bool value);

	/**
	 * Contructor for double prefs.
	 * @param name Preference name.
	 * @param value Double value
	 **/
	Preference(wxString name, double value);

	/**
	 * Contructor for signed integer prefs.
	 * @param name Preference name.
	 * @param value Int32 value
	 **/
	Preference(wxString name, wxInt32 value);

	/**
	 * Contructor for unsigned integer prefs.
	 * @param name Preference name.
	 * @param value Uint32 value
	 **/
	Preference(wxString name, wxUint32 value);

	/**
	 * Contructor for string and hidden string prefs.
	 * @param name Preference name.
	 * @param value string value
	 * @param isHidden Hidden state
	 **/
	Preference(wxString name, const wxString& value, bool isHidden = false);

	/**
	 * Read the preference from the given input stream.
	 * @param in The input data stream.
	 **/
	void Read(DataInputStream& in);

	/**
	 * Write this preference to the given output stream.
	 * @param out The output data stream.
	 **/
	void Write(DataOutputStream& out) const;

	/**
	 * Checks if preference is boolean.
	 **/
	bool IsABoolPref(void)          const {return mPrefType == PT_BOOL;}

	/**
	 * Checks if preference is a signed integer.
	 **/
	bool IsAnIntPref(void)          const {return mPrefType == PT_INT;}

	/**
	 * Checks if preference is an unsigned integer.
	 **/
	bool IsAnUintPref(void)         const {return mPrefType == PT_UINT;}

	/**
	 * Checks if preference is a double.
	 **/
	bool IsADoublePref(void)        const {return mPrefType == PT_DOUBLE;}

	/**
	 * Checks if preference is a string.
	 **/
	bool IsAStringPref(void)        const {return mPrefType == PT_STRING;}

	/**
	 * Checks if preference is an hidden string.
	 **/
	bool IsAnHiddenStringPref(void) const {return mPrefType == PT_HIDDEN_STRING;}

	// --- Getters
	/**
	 * Get boolean value of preference.
	 **/
	bool     GetBoolValue(void)         const {return mBoolValue;}

	/**
	 * Get double value of preference.
	 **/
	double   GetDoubleValue(void)       const {return mDoubleValue;}

	/**
	 * Get int32 value of preference.
	 **/
	wxInt32  GetIntValue(void)          const {return mIntValue;}

	/**
	 * Get name of preference.
	 **/
	wxString GetName(void)              const {return mPrefName;}

	/**
	 * Get string value of preference.
	 **/
	wxString GetStringValue(void)       const {return mStringValue;}

	/**
	 * Get hidden string value of preference.
	 **/
	wxString GetHiddenStringValue(void) const {return mStringValue;}

	/**
	 * Get uint32 value of preference.
	 **/
	wxUint32 GetUintValue(void)         const {return mUintValue;}
};


#endif /* _PREFERENCE_H */
