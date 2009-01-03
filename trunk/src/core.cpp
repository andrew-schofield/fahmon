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
 * \file core.cpp
 * Manages core definitions.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "core.h"


wxString Core::mCoreLongName[CORE_ID_COUNT] = {
	_T("Tinker"),
	_T("Gromacs"),
	_T("Double Gromacs"),
	_T("GBGromacs"),
	_T("Amber"),
	_T("QMD"),
	_T("Unknown"),
	_T("Gromacs 33"),
	_T("SMP Gromacs"),
	_T("GPU Gromacs"),
	_T("SREM Gromacs"),
	_T("SimT Gromacs"),
	_T("Double Gromacs B"),
	_T("SMP Gromacs CVS"),
	_T("PS3 Gromacs (SCEARD2)"),
	_T("GPUv2 Gromacs"),
	_T("Double Gromacs C"),
	_T("GPU2 ATI Dev"),
	_T("GPU2 NV Dev"),
};
wxString Core::mCoreShortName[CORE_ID_COUNT] = {
	_T("tinker"),
	_T("gromacs"),
	_T("dgromacs"),
	_T("gbgromacs"),
	_T("amber"),
	_T("qmd"),
	_T("Unknown"),
	_T("gromacs33"),
	_T("gro-smp"),
	_T("grogpu"),
	_T("grost"),
	_T("grosimt"),
	_T("dgromacsb"),
	_T("grocvs"),
	_T("gro-ps3"),
	_T("grogpu2"),
	_T("dgromacsc"),
	_T("ati-dev"),
	_T("nvidia-dev")
};


CoreId Core::ShortNameToId(const wxString& name)
{
	CoreId identifier;

	for(identifier=TINKER; identifier<CORE_ID_COUNT; ++identifier)
	{
		if(name.CmpNoCase(mCoreShortName[identifier]) == 0)
		{
			return identifier;
		}
	}

	return UNKNOWN;
}


wxString Core::IdToShortName(CoreId identifier)
{
	if(identifier >= CORE_ID_COUNT)
	{
		return mCoreShortName[UNKNOWN];
	}

	return mCoreShortName[identifier];
}


wxString Core::IdToLongName(CoreId identifier)
{
	if(identifier >= CORE_ID_COUNT)
	{
		return mCoreLongName[UNKNOWN];
	}

	return mCoreLongName[identifier];
}


wxString Core::IdToClientType(CoreId identifier)
{
	wxString clientType;

	switch(identifier)
	{
		case TINKER:
		case GROMACS:
		case DGROMACS:
		case GBGROMACS:
		case AMBER:
		case QMD:
		case GROMACS33:
		case GROST:
		case GROSIMT:
		case DGROMACSB:
		case DGROMACSC:
			clientType = _("CPU");
			break;
		case GROSMP:
		case GROCVS:
			clientType = _("SMP");
			break;
		case GROGPU:
		case GROGPU2:
		case ATIDEV:
		case NVDEV:
			clientType = _("GPU");
			break;
		case UNKNOWN:
		case GROPS3:
		default:
			clientType = _("Unknown");
			break;
	}

	return clientType;
}
