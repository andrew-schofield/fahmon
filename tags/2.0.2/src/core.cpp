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
 
#include "fahmon.h"
#include "core.h"


wxString Core::mCoreLongName[CORE_ID_COUNT] = {wxT("Tinker"), wxT("Gromacs"), wxT("Double Gromacs"), wxT("GBGromacs"), wxT("Amber"), wxT("QMD"), wxT("Unknown")};
wxString Core::mCoreShortName[CORE_ID_COUNT] = {wxT("tinker"), wxT("gromacs"), wxT("dgromacs"), wxT("gbgromacs"), wxT("amber"), wxT("qmd"), wxT("Unknown")};


/**
 * Translate a short core name into a core identifier
**/
CoreId Core::ShortNameToId(const wxString& name)
{
    CoreId identifier;
    
    for(identifier=TINKER; identifier<CORE_ID_COUNT; ++identifier)
        if(name.CmpNoCase(mCoreShortName[identifier]) == 0)
            return identifier;
        
    return UNKNOWN;
}


/**
 * Translate a core identifier into a (short) core name
**/
wxString Core::IdToShortName(CoreId identifier)
{
    if(identifier >= CORE_ID_COUNT)
        return mCoreShortName[UNKNOWN];
    
    return mCoreShortName[identifier];
}


/**
 * Translate a core identifier into a (long) core name
**/
wxString Core::IdToLongName(CoreId identifier)
{
    if(identifier >= CORE_ID_COUNT)
        return mCoreLongName[UNKNOWN];
    
    return mCoreLongName[identifier];
}
