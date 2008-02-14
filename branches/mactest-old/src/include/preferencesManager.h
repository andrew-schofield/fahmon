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

#ifndef _PREFERENCESMANAGER_H
#define _PREFERENCESMANAGER_H


#include "wx/hashmap.h"
#include "preference.h"


// Macros for retrieving preferences
// They 'return' the default value if the pref is unknown
#define _PrefsGetString(PREF_NAME, VAR_NAME)                                        \
{                                                                                   \
	if(PreferencesManager::GetInstance()->GetString(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                              \
}

#define _PrefsGetHiddenString(PREF_NAME, VAR_NAME)                                        \
{                                                                                         \
	if(PreferencesManager::GetInstance()->GetHiddenString(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                                    \
}

#define _PrefsGetUint(PREF_NAME, VAR_NAME)                                        \
{                                                                                 \
	if(PreferencesManager::GetInstance()->GetUint(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                            \
}

#define _PrefsGetInt(PREF_NAME, VAR_NAME)                                        \
{                                                                                \
	if(PreferencesManager::GetInstance()->GetInt(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                           \
}

#define _PrefsGetDouble(PREF_NAME, VAR_NAME)                                        \
{                                                                                   \
	if(PreferencesManager::GetInstance()->GetDouble(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                              \
}

#define _PrefsGetBool(PREF_NAME, VAR_NAME)                                        \
{                                                                                 \
	if(PreferencesManager::GetInstance()->GetBool(PREF_NAME, VAR_NAME) == false)  \
			VAR_NAME = PREF_NAME##_DV;                                            \
}


// Macros for setting preferences
#define _PrefsSetString(PREF_NAME, VAR_NAME)                             \
{                                                                       \
	PreferencesManager::GetInstance()->SetString(PREF_NAME, VAR_NAME);  \
}

#define _PrefsSetHiddenString(PREF_NAME, VAR_NAME)                             \
{                                                                              \
	PreferencesManager::GetInstance()->SetHiddenString(PREF_NAME, VAR_NAME);   \
}

#define _PrefsSetUint(PREF_NAME, VAR_NAME)                            \
{                                                                     \
	PreferencesManager::GetInstance()->SetUint(PREF_NAME, VAR_NAME);  \
}

#define _PrefsSetInt(PREF_NAME, VAR_NAME)                            \
{                                                                    \
	PreferencesManager::GetInstance()->SetInt(PREF_NAME, VAR_NAME);  \
}

#define _PrefsSetDouble(PREF_NAME, VAR_NAME)                            \
{                                                                       \
	PreferencesManager::GetInstance()->SetDouble(PREF_NAME, VAR_NAME);  \
}

#define _PrefsSetBool(PREF_NAME, VAR_NAME)                            \
{                                                                     \
	PreferencesManager::GetInstance()->SetBool(PREF_NAME, VAR_NAME);  \
}


WX_DECLARE_STRING_HASH_MAP(Preference*, PreferencesHashMap);


/**
* This is the component which manages the configuration
* It's a singleton
**/
class PreferencesManager
{
protected:
	PreferencesHashMap         mPrefsHashMap;
	static PreferencesManager *mInstance;

	PreferencesManager(void);
	~PreferencesManager(void);

	void        SetPref(Preference* preference);
	Preference* GetPref(const wxString& name);

	void Load(void);


public:
	// Singleton pattern
	static void CreateInstance(void);
	static void DestroyInstance(void);
	static PreferencesManager* GetInstance(void);
	void Save(void);

	// Preferences management
	// All the GetXXX() methods return a boolean, indicating if the preference could be retrieve or not
	void SetBool(const wxString& name, bool value) {SetPref(new Preference(name, value));}
	bool GetBool(const wxString& name, bool& value);

	void SetUint(const wxString& name, wxUint32 value) {SetPref(new Preference(name, value));}
	bool GetUint(const wxString& name, wxUint32& value);

	void SetInt(const wxString& name, wxInt32 value) {SetPref(new Preference(name, value));}
	bool GetInt(const wxString& name, wxInt32& value);

	void SetDouble(const wxString& name, double value) {SetPref(new Preference(name, value));}
	bool GetDouble(const wxString& name, double& value);

	void SetString(const wxString& name, const wxString& value) {SetPref(new Preference(name, value));}
	bool GetString(const wxString& name, wxString& value);

	void SetHiddenString(const wxString& name, const wxString& value) {SetPref(new Preference(name, value, true));}
	bool GetHiddenString(const wxString& name, wxString& value);
};


#endif /* _PREFERENCESMANAGER_H */
