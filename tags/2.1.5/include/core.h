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
 
#ifndef _CORE_H
#define _CORE_H


#include "wx/wx.h"
#include "fahmonTypes.h"

// TODO
// Change this class so that methods won't be static anymore?


class Core
{
public:
    // All the known cores, up to now
    // Their order !MUST! not be changed, for compatibility reasons
    enum _CORE_ID
    {
        TINKER,
        GROMACS,
        DGROMACS,
        GBGROMACS,
        AMBER,
        QMD,
        UNKNOWN,
        GROMACS33,
        GROMACSSMP,
        GROGPU,
        CORE_ID_COUNT
    };

    static CoreId   ShortNameToId(const wxString& name);
    static wxString IdToShortName(CoreId identifier);
    static wxString IdToLongName(CoreId identifier);


protected:
    static wxString mCoreShortName[CORE_ID_COUNT];  // Short names are the ones used on the official projects page
    static wxString mCoreLongName[CORE_ID_COUNT];
};


#endif /* _CORE_H */
