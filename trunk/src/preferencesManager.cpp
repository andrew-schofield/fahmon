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
 * \file preferencesManager.cpp
 * Manages stored preferences.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "preferencesManager.h"

#include "tools.h"
#include "pathManager.h"
#include "messagesManager.h"
#include "dataInputStream.h"
#include "dataOutputStream.h"


// The single instance of PreferencesManager accross the application
PreferencesManager* PreferencesManager::mInstance = NULL;


PreferencesManager::PreferencesManager(void)
{
}


PreferencesManager::~PreferencesManager(void)
{
	PreferencesHashMap::iterator iterator;

	for(iterator = mPrefsHashMap.begin(); iterator != mPrefsHashMap.end(); ++iterator)
	{
		delete iterator->second;
	}
}


void PreferencesManager::CreateInstance(void)
{
	wxASSERT(mInstance == NULL);

	mInstance = new PreferencesManager();
	mInstance->Load();
}


void PreferencesManager::DestroyInstance(void)
{
	wxASSERT(mInstance != NULL);

	mInstance->Save();

	delete mInstance;
	mInstance = NULL;
}


PreferencesManager* PreferencesManager::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}


void PreferencesManager::SetPref(Preference* preference)
{
	PreferencesHashMap::iterator iterator;

	wxASSERT(preference != NULL);

	// Let's first free the old preference if needed
	iterator = mPrefsHashMap.find(preference->GetName());
	if(iterator != mPrefsHashMap.end())
	{
		delete iterator->second;
	}

	// We can now insert the new value into the table
	mPrefsHashMap[preference->GetName()] = preference;
}


Preference* PreferencesManager::GetPref(wxString const &name)
{
	PreferencesHashMap::iterator iterator = mPrefsHashMap.find(name);

	if(iterator == mPrefsHashMap.end())
	{
		return NULL;
	}

	return iterator->second;
}


bool PreferencesManager::GetBool(wxString  const &name, bool& value)
{
	Preference *prefValue = GetPref(name);

	if(prefValue == NULL)
	{
		return false;
	}

	wxASSERT(prefValue->IsABoolPref() == true);

	value = prefValue->GetBoolValue();
	return true;
}


bool PreferencesManager::GetUint(wxString const &name, wxUint32& value)
{
	Preference *prefValue = GetPref(name);

	if(prefValue == NULL)
	{
		return false;
	}

	wxASSERT(prefValue->IsAnUintPref() == true);

	value = prefValue->GetUintValue();
	return true;
}


bool PreferencesManager::GetInt(wxString const &name, wxInt32& value)
{
	Preference *prefValue = GetPref(name);

	if(prefValue == NULL)
	{
		return false;
	}

	wxASSERT(prefValue->IsAnIntPref() == true);

	value = prefValue->GetIntValue();
	return true;
}


bool PreferencesManager::GetDouble(wxString const &name, double& value)
{
	Preference *prefValue = GetPref(name);

	if(prefValue == NULL)
	{
		return false;
	}

	wxASSERT(prefValue->IsADoublePref() == true);

	value = prefValue->GetDoubleValue();
	return true;
}


bool PreferencesManager::GetString(wxString const &name, wxString& value)
{
	Preference *prefValue = GetPref(name);

	if(prefValue == NULL)
	{
		return false;
	}

	wxASSERT(prefValue->IsAStringPref() == true);

	value = prefValue->GetStringValue();
	return true;
}


bool PreferencesManager::GetHiddenString(wxString const &name, wxString& value)
{
	Preference *prefValue = GetPref(name);

	if(prefValue == NULL)
	{
		return false;
	}

	wxASSERT(prefValue->IsAnHiddenStringPref() == true);

	value = prefValue->GetHiddenStringValue();
	return true;
}


void PreferencesManager::Load(void)
{
	Preference      *currentPrefValue;
	wxUint32         nbPrefs, i;
	DataInputStream  in(PathManager::GetCfgPath() + _T(FMC_FILE_PREFS));

	// Could the file be opened ?
	if(in.Ok() == false)
	{
		_LogMsgWarning(_("There is no Preferences file, or it is not readable"));
		return;
	}

	// Find how many prefs are present, and read them one by one
	in.ReadUint(nbPrefs);
	for(i=0; i<nbPrefs; ++i)
	{
		currentPrefValue = new Preference;
		currentPrefValue->Read(in);
		SetPref(currentPrefValue);
	}
}


void PreferencesManager::Save(void)
{
	DataOutputStream             out(PathManager::GetCfgPath() + _T(FMC_FILE_PREFS));
	PreferencesHashMap::iterator iterator;

	if(out.Ok() == false)
	{
		Tools::ErrorMsgBox(wxString::Format(_("Could not open file <%s> for writing!"), (PathManager::GetCfgPath() + _T(FMC_FILE_PREFS)).c_str()));
		return;
	}

	// A simple format is used
	//  1) An unsigned int is first written, containing the number of preferences
	//  2) Then, each preference is written
	out.WriteUint(mPrefsHashMap.size());
	for(iterator = mPrefsHashMap.begin(); iterator != mPrefsHashMap.end(); ++iterator)
	{
		iterator->second->Write(out);
	}
}
