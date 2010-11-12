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
 * \file project.cpp
 * Project object.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "project.h"
#include "core.h"


Project::Project(ProjectId projectId, wxUint16 preferredDeadlineInDays, wxUint16 finalDeadlineInDays, wxUint16 nbFrames, WuCredit credit, CoreId coreId, wxUint16 kFactor)
{
	SetProjectId(projectId);
	SetPreferredDeadlineInDays(preferredDeadlineInDays);
	SetFinalDeadlineInDays(finalDeadlineInDays);
	SetNbFrames(nbFrames);
	SetCredit(credit);
	SetCoreId(coreId);
	SetKFactor(kFactor);
}


void Project::Write(TiXmlElement* out) const
{
	out->SetAttribute( "Id", wxString::Format(wxT("%i"), mProjectId).mb_str( wxConvUTF8 ) );
	out->SetAttribute( "PreferredDeadline", wxString::Format(wxT("%i"), mPreferredDeadlineInDays).mb_str( wxConvUTF8 ) );
	out->SetAttribute( "FinalDeadline", wxString::Format(wxT("%i"), mFinalDeadlineInDays).mb_str( wxConvUTF8 ) );
	out->SetAttribute( "FrameCount", wxString::Format(wxT("%i"), mNbFrames).mb_str( wxConvUTF8 ) );
	out->SetAttribute( "Credit", wxString::Format(wxT("%i"), mCredit).mb_str( wxConvUTF8 ) );
	out->SetAttribute( "CoreId", Core::IdToShortName(mCoreId).mb_str( wxConvUTF8 ) );
	out->SetAttribute( "KFactor", wxString::Format(wxT("%i"), mKFactor).mb_str( wxConvUTF8 ) );
}


void Project::Read(TiXmlElement* in)
{
	int val;
	in->QueryIntAttribute("Id", &val);
	SetProjectId(val);
	in->QueryIntAttribute("PreferredDeadline", &val);
	SetPreferredDeadlineInDays(val);
	in->QueryIntAttribute("FinalDeadline", &val);
	SetFinalDeadlineInDays(val);
	in->QueryIntAttribute("FrameCount", &val);
	SetNbFrames(val);
	in->QueryIntAttribute("Credit", &val);
	SetCredit(val);
	SetCoreId(Core::ShortNameToId(wxString::FromUTF8(in->Attribute("CoreId"))));
	in->QueryIntAttribute("KFactor", &val);
	SetKFactor(val);
}


void Project::ReadOld(DataInputStream& in)
{
	in.Read(&mProjectId, sizeof(mProjectId));
	in.Read(&mPreferredDeadlineInDays, sizeof(mPreferredDeadlineInDays));
	in.Read(&mFinalDeadlineInDays, sizeof(mFinalDeadlineInDays));
	in.Read(&mNbFrames, sizeof(mNbFrames));
	in.Read(&mCredit, sizeof(mCredit));
	in.Read(&mCoreId, sizeof(mCoreId));
	mKFactor = 0;
}
