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
 * \file preferencesManager.h
 * Manages stored preferences.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _PREFERENCESMANAGER_H
#define _PREFERENCESMANAGER_H


#include "wx/hashmap.h"
#include "preference.h"

/**
 * Macro to get string preference.
 **/
#define _PrefsGetString(PREF_NAME, VAR_NAME)                                        \
{                                                                                   \
	if(PreferencesManager::GetInstance()->GetString(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                              \
}

/**
 * Macro to get hidden string preference.
 **/
#define _PrefsGetHiddenString(PREF_NAME, VAR_NAME)                                        \
{                                                                                         \
	if(PreferencesManager::GetInstance()->GetHiddenString(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                                    \
}

/**
 * Macro to get unsigned integer preference.
 **/
#define _PrefsGetUint(PREF_NAME, VAR_NAME)                                        \
{                                                                                 \
	if(PreferencesManager::GetInstance()->GetUint(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                            \
}

/**
 * Macro to get signed integer preference.
 **/
#define _PrefsGetInt(PREF_NAME, VAR_NAME)                                        \
{                                                                                \
	if(PreferencesManager::GetInstance()->GetInt(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                           \
}

/**
 * Macro to get double preference.
 **/
#define _PrefsGetDouble(PREF_NAME, VAR_NAME)                                        \
{                                                                                   \
	if(PreferencesManager::GetInstance()->GetDouble(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                              \
}

/**
 * Macro to get boolean preference.
 **/
#define _PrefsGetBool(PREF_NAME, VAR_NAME)                                        \
{                                                                                 \
	if(PreferencesManager::GetInstance()->GetBool(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                            \
}

/**
 * Macro to set string preference.
 **/
#define _PrefsSetString(PREF_NAME, VAR_NAME)                             \
{                                                                       \
	PreferencesManager::GetInstance()->SetString(PREF_NAME, VAR_NAME);  \
}

/**
 * Macro to set hidden string preference.
 **/
#define _PrefsSetHiddenString(PREF_NAME, VAR_NAME)                             \
{                                                                              \
	PreferencesManager::GetInstance()->SetHiddenString(PREF_NAME, VAR_NAME);   \
}

/**
 * Macro to set unsigned integer preference.
 **/
#define _PrefsSetUint(PREF_NAME, VAR_NAME)                            \
{                                                                     \
	PreferencesManager::GetInstance()->SetUint(PREF_NAME, VAR_NAME);  \
}

/**
 * Macro to set signed integer preference.
 **/
#define _PrefsSetInt(PREF_NAME, VAR_NAME)                            \
{                                                                    \
	PreferencesManager::GetInstance()->SetInt(PREF_NAME, VAR_NAME);  \
}

/**
 * Macro to set double preference.
 **/
#define _PrefsSetDouble(PREF_NAME, VAR_NAME)                            \
{                                                                       \
	PreferencesManager::GetInstance()->SetDouble(PREF_NAME, VAR_NAME);  \
}

/**
 * Macro to set boolean preference.
 **/
#define _PrefsSetBool(PREF_NAME, VAR_NAME)                            \
{                                                                     \
	PreferencesManager::GetInstance()->SetBool(PREF_NAME, VAR_NAME);  \
}


WX_DECLARE_STRING_HASH_MAP(Preference*, PreferencesHashMap); /**< Hashmap to store preferences */


/**
 * This class manages the configuration settings.
 * This class can only be instantiated once.
 **/
class PreferencesManager
{
protected:
	PreferencesHashMap         mPrefsHashMap; /**< The preference hashmap */
	static PreferencesManager *mInstance; /**< The single instance of the PreferencesManager */

	/**
	 * Contructor.
	 **/
	PreferencesManager(void);

	/**
	 * Destructor.
	 **/
	~PreferencesManager(void);

	/**
	 * Change/set the value of a preference.
	 * @param preference The preference to set.
	 **/
	void        SetPref(Preference* preference);

	/**
	 * Retrieve the value of a preference.
	 **/
	Preference* GetPref(const wxString &name);

	/**
	 * Load the preferences from the disk.
	 **/
	void Load(void);


public:
	// Singleton pattern
	/**
	 * Create the single instance of the PreferencesManager.
	 **/
	static void CreateInstance(void);

	/**
	 * Destroy the single instance of the PreferencesManager
	 **/
	static void DestroyInstance(void);

	/**
	 * Return the single instance of the PreferencesManager
	 **/
	static PreferencesManager* GetInstance(void);

	/**
	 * Save all the preferences to the disk.
	 **/
	void Save(void);

	// Preferences management
	// All the GetXXX() methods return a boolean, indicating if the preference could be retrieved or not
	/**
	 * Set the value of a boolean preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	void SetBool(wxString const &name, bool value) {SetPref(new Preference(name, value));}

	/**
	 * Retrieve the value of a boolean preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	bool GetBool(wxString const &name, bool& value);

	/**
	 * Set the value of a unsigned int preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	void SetUint(wxString const &name, wxUint32 value) {SetPref(new Preference(name, value));}

	/**
	 * Retrieve the value of an unsigned int preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	bool GetUint(wxString const &name, wxUint32& value);

	/**
	 * Set the value of a signed int preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	void SetInt(wxString const &name, wxInt32 value) {SetPref(new Preference(name, value));}

	/**
	 * Retrieve the value of an signed int preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	bool GetInt(wxString const &name, wxInt32& value);

	/**
	 * Set the value of a double preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	void SetDouble(wxString const &name, double value) {SetPref(new Preference(name, value));}

	/**
	 * Retrieve the value of a double preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	bool GetDouble(wxString const &name, double& value);

	/**
	 * Set the value of a string preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	void SetString(wxString const &name, wxString const &value) {SetPref(new Preference(name, value));}

	/**
	 * Retrieve the value of a string preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	bool GetString(wxString const &name, wxString& value);

	/**
	 * Set the value of a hidden string preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	void SetHiddenString(wxString const &name, const wxString& value) {SetPref(new Preference(name, value, true));}

	/**
	 * Retrieve the value of an hidden string preference.
	 * @param name Preference name
	 * @param value Value to retrieve
	 **/
	bool GetHiddenString(wxString const &name, wxString& value);
};


#endif /* _PREFERENCESMANAGER_H */
