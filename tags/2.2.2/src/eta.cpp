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
#include "eta.h"

#include "preferencesManager.h"


/**
 * Constructor
**/
ETA::ETA(void)
{
    mIsOk = false;
}


/**
 * Destructor
**/
ETA::~ETA(void)
{
}


/**
 * Change the value, validate this ETA
**/
void ETA::SetLeftTimeInMinutes(wxUint32 leftTime)
{
    mIsOk              = true;
    mLeftTimeInMinutes = leftTime;
    
    // Compute the corresponding date once for all
    mETA.SetToCurrent();
    mETA.Add(wxTimeSpan::Minutes(leftTime));
}


/**
 * Return the ETA with the format day/month/year hour:minutes
**/
inline wxString ETA::GetString_Standard(void) const
{
    return mETA.Format(wxT("%d/%m/%y  %H:%M"));
}


/**
 * Return the ETA with the format month/day/year hour:minutes
**/
inline wxString ETA::GetString_American(void) const
{
    return mETA.Format(wxT("%m/%d/%y  %H:%M"));
}


/**
 * Return the ETA with the format days hour:minutes
**/
inline wxString ETA::GetString_LeftTime(void) const
{
    wxUint32 nbDays;
    wxUint32 nbHours;
    wxUint32 nbMinutes;
    
    // Split the left time into days, hours and minutes
    nbDays    = mLeftTimeInMinutes / (24 * 60);
    nbMinutes = mLeftTimeInMinutes % (24 * 60);
    nbHours   = nbMinutes / 60;
    nbMinutes = nbMinutes % 60;
    
    // Use a friendly format
    if(nbDays != 0)
        return wxString::Format(wxT("%ud %02uh %02umn"), nbDays, nbHours, nbMinutes);
    else if(nbHours != 0)
        return wxString::Format(wxT("%uh %02umn"), nbHours, nbMinutes);
    else    
        return wxString::Format(wxT("%umn"), nbMinutes);
}


/**
 * Return the ETA using the format defined in preferences
**/
wxString ETA::GetString(void) const
{
    wxUint32 displayStyle;
    
    _PrefsGetUint(PREF_ETA_DISPLAYSTYLE, displayStyle);
    
    switch(displayStyle)
    {
        // ---
        case ETADS_DATE_DAY_MONTH:
            return GetString_Standard();
            break;
        
        // ---
        case ETADS_DATE_MONTH_DAY:
            return GetString_American();
            break;

        // ---
        case ETADS_LEFT_TIME:
            return GetString_LeftTime();
            break;
        
        // We shall never fall here
        default:
            wxASSERT(false);
            break;
    };
    
    return GetString_Standard();
}
