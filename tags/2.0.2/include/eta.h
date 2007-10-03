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
 
#ifndef _ETA_H
#define _ETA_H


#include "wx/datetime.h"


// All the possible styles that can be used to display the ETA
// The order should not be changed, or the preferenceDialog will have to be modified in a correct way
typedef enum _ETA_DISPLAY_STYLE
{
    ETADS_DATE_DAY_MONTH,
    ETADS_DATE_MONTH_DAY,
    ETADS_LEFT_TIME
} ETA_DisplayStyle;


/**
 * Preferences used by this class
**/
#define PREF_ETA_DISPLAYSTYLE    wxT("ETA.DisplayStyle")
#define PREF_ETA_DISPLAYSTYLE_DV ETADS_LEFT_TIME


// This class handles an estimated time of arrival
class ETA
{
protected:
    bool       mIsOk;
    wxUint32   mLeftTimeInMinutes;
    wxDateTime mETA;

    wxString GetString_Standard(void) const;
    wxString GetString_American(void) const;
    wxString GetString_LeftTime(void) const;


public:
     ETA(void);
    ~ETA(void);

    void Invalidate(void) {mIsOk = false;}
    
    // --- Setters
    void SetLeftTimeInMinutes(wxUint32 leftTime);

    // --- Getters
    bool     IsOk(void)               const {return mIsOk;}
    bool     IsBefore(const ETA* eta) const {return mLeftTimeInMinutes < eta->mLeftTimeInMinutes;}
    wxString GetString(void)          const;
};


#endif /* _ETA_H */
