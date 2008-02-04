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
 * \file eta.h
 * Creates a class for managing client ETAs.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _ETA_H
#define _ETA_H


#include "wx/datetime.h"


/**
 * ETA Display style definition.
 * Contains all the possible styles that can be used to display the ETA.
 * The order should not be changed, or the preferenceDialog will have to be modified to match.
 **/
typedef enum _ETA_DISPLAY_STYLE
{
	ETADS_DATE_DAY_MONTH, /**< dd/mm hh:mm */
	ETADS_DATE_MONTH_DAY, /**< mm/dd hh:mm */
	ETADS_LEFT_TIME /**< x days y minutes etc. */
} ETA_DisplayStyle;

/**
 * PPD Display style definition.
 * Contains all the possible ways of displaying PPD.
 * The order should not be changed.
 **/
typedef enum _PPD_DISPLAY_STYLE
{
	PPDDS_ALL_FRAMES, /**< Base PPD on all frames */
	PPDDS_LAST_FRAME, /**< Base PPD on last frame only */
	PPDDS_THREE_FRAMES, /**< Base PPD on last 3 frames */
	PPDDS_EFFECTIVE_FRAMES /**< Base PPD on effective frame rate */
} ETA_PPDStyle;


#define PREF_ETA_DISPLAYSTYLE    wxT("ETA.DisplayStyle") /**< Preference setting for ETA display style */
#define PREF_ETA_DISPLAYSTYLE_DV ETADS_LEFT_TIME /**< Default value for ETA display style */

#define PREF_PPD_DISPLAYSTYLE  wxT("ETA.PPDStyle") /**< Preference setting for PPD display style */
#define PREF_PPD_DISPLAYSTYLE_DV PPDDS_ALL_FRAMES /**< Default value for PPD display style */


/**
 * ETA class.
 * This class manages the way ETA is calculated and displayed.
 **/
class ETA
{
protected:
	bool       mIsOk; /**< Is the ETA valid */
	wxUint32   mLeftTimeInMinutes; /**< Time left until WU finishes */
	wxDateTime mETA; /**< The actual ETA */

	/**
	 * Format a standard ETA.
	 * @return the ETA with the format day/month/year hour:minutes
	 **/
	wxString GetString_Standard(void) const;

	/**
	 * Format an American ETA.
	 * @return the ETA with the format month/day/year hour:minutes
	 **/
	wxString GetString_American(void) const;

	/**
	 * Format a "time left" ETA.
	 * @return the ETA with the format days hour:minutes
	 **/
	wxString GetString_LeftTime(void) const;


public:
	/**
	 * Contructor.
	 * Contructs the ETA object.
	 **/
	ETA(void);

	/**
	 * Destructor.
	 **/
	~ETA(void);

	/**
	 * Invalidate the ETA.
	 * Makes IsOk = false
	 **/
	void Invalidate(void) {mIsOk = false;}

	// --- Setters
	/**
	 * Change the ETA.
	 * Sets the time left and validates the ETA.
	 * @param leftTime Time remaining.
	 **/
	void SetLeftTimeInMinutes(wxUint32 leftTime);

	// --- Getters
	/**
	 * Check if ETA is valid.
	 * @return State of ETA/
	 **/
	bool     IsOk(void)               const {return mIsOk;}

	/**
	 * Check if ETA is before given ETA.
	 * @param eta ETA to test against.
	 * @return Is eta object before eta parameter.
	 **/
	bool     IsBefore(const ETA* eta) const {return mLeftTimeInMinutes < eta->mLeftTimeInMinutes;}

	/**
	 * Format the ETA as a string.
	 * @return the ETA using the format defined in preferences
	 **/
	wxString GetString(void)          const;
};


#endif /* _ETA_H */
