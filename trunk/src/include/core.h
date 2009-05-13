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
 * \file core.h
 * Manages core definitions.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _CORE_H
#define _CORE_H


#include "wx/wx.h"
#include "fahmonTypes.h"

// TODO
// Change this class so that methods won't be static anymore?

/**
 * Core class.
 * This class manages FahCore definitions and stores full names for them.
 **/
class Core
{
public:
	/**
	 * All the known cores, up to now.
	 * Ideally these should be the names as given on psummary.html
	 * Any changes in this list !MUST! be reflected in IdToClientType.
	 * Their order !MUST! not be changed, for compatibility reasons.
	 **/
	enum _CORE_ID
	{
		TINKER, /**< Tinker (65) */
		GROMACS, /**< Gromacs (78) */
		DGROMACS, /**< Double Gromacs (79) */
		GBGROMACS, /**< GB Gromacs (7a) */
		AMBER, /**< Amber (82) */
		QMD, /**< QMD (96) */
		UNKNOWN, /**< Core is unknown */
		GROMACS33, /**< Gromacs 3.3 (a0) */
		GROSMP, /**< SMP Gromacs (a1) */
		GROGPU, /**< GPU Gromacs (10) */
		GROST, /**< SREM Gromacs (80) */
		GROSIMT, /**< SimT Gromacs (81) */
		DGROMACSB, /**< Double Gromacs B (7b) */
		GROCVS, /**< SMP Gromacs CVS (a2) */
		GROPS3, /**< PS3 Gromacs (SCEARD) */
		GROGPU2, /**< GPU2 Gromacs (11) */
		DGROMACSC, /**< Double Gromacs C (7c) */
		ATIDEV, /**< ATI development Core (12) */
		NVDEV, /**< NVIDIA development Core (13) */
		GROA4, /**< Gromacs 4 GB */
		GROGPU2MT, /**< NVIDIA w/Mem-Test */
		CORE_ID_COUNT /**<  */
	};

	/**
	 * Translate a short core name into a core identifier.
	 * @param name Short name for core.
	 * @return Indentifier for core
	 **/
	static const CoreId   ShortNameToId(wxString const &name);

	/**
	 * Translate a core identifier into a (short) core name.
	 * @param identifier The core Id.
	 * @return String containing short core name.
	 **/
	static const wxString IdToShortName(CoreId identifier);

	/**
	 * Translate a core identifier into a (long) core name.
	 * @param identifier The core Id.
	 * @return String containing the full core name.
	 **/
	static const wxString IdToLongName(CoreId identifier);

	/**
	 * Translate a core identifier into a client type (CPU/GPU/SMP).
	 * @param identifier The core Id.
	 * @return String containing the client type.
	 **/
	static const wxString IdToClientType(CoreId identifier);


protected:
	/**
	 * Array of core names as given on psummary.
	 * The elements are case insensitive and need to match the order in _CORE_ID
	 **/
	static const wxString mCoreShortName[CORE_ID_COUNT];

	/**
	 * Array of full core names as shown in the WU information panel.
	 * The elements need to match the order in _CORE_ID but are descriptions not identifiers.
	 **/
	static const wxString mCoreLongName[CORE_ID_COUNT];
};


#endif /* _CORE_H */
