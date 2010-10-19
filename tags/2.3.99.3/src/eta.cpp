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
 * \file eta.cpp
 * Creates a class for managing client ETAs.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "eta.h"

#include "preferencesManager.h"


ETA::ETA(void)
{
	mIsOk = false;
}


ETA::~ETA(void)
{
}


void ETA::SetLeftTimeInMinutes(wxUint32 leftTime)
{
	mIsOk              = true;
	mLeftTimeInMinutes = leftTime;

	// Compute the corresponding date once for all
	mETA.SetToCurrent();
	mETA.Add(wxTimeSpan::Minutes(leftTime));
}


wxString ETA::GetString_Standard(void) const
{
	return mETA.Format(_T("%d/%m/%y  %H:%M"));
}


wxString ETA::GetString_American(void) const
{
	return mETA.Format(_T("%m/%d/%y  %H:%M"));
}


wxString ETA::GetString_LeftTime(void) const
{
	return FormatMinutes(mLeftTimeInMinutes);
}


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


wxString ETA::FormatMinutes(wxUint32 minutes)
{
	wxUint32 nbDays;
	wxUint32 nbHours;
	wxUint32 nbMinutes;

	// Split the left time into days, hours and minutes
	nbDays    = minutes / (24 * 60);
	nbMinutes = minutes % (24 * 60);
	nbHours   = nbMinutes / 60;
	nbMinutes = nbMinutes % 60;
	// Use a friendly format
	if(nbDays != 0)
	{
		return wxString::Format(_T("%id %02ih %02imn"), nbDays, nbHours, nbMinutes);
	}
	else if(nbHours != 0)
	{
		return wxString::Format(_T("%ih %02imn"), nbHours, nbMinutes);
	}
	else
	{
		return wxString::Format(_T("%imn"), nbMinutes);
	}
}


wxString ETA::FormatSeconds(wxUint32 nbSeconds)
{
	wxUint32 nbHours;
	wxUint32 nbMinutes;

	nbHours   = nbSeconds / 3600;        // There are 3600 seconds in an hour
	nbSeconds = nbSeconds % 3600;
	nbMinutes = nbSeconds / 60;          // There are 60 seconds in a minute
	nbSeconds = nbSeconds % 60;

	if(nbHours != 0)
	{
		return wxString::Format(_T("%uh %02umn %02us"), nbHours, nbMinutes, nbSeconds);
	}
	else if(nbMinutes != 0)
	{
		return wxString::Format(_T("%umn %02us"), nbMinutes, nbSeconds);
	}
	else
	{
		return wxString::Format(_T("%us"), nbSeconds);
	}
}
