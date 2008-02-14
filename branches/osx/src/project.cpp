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


Project::Project(ProjectId projectId, wxUint16 preferredDeadlineInDays, wxUint16 finalDeadlineInDays, wxUint16 nbFrames, WuCredit credit, CoreId coreId)
{
	SetProjectId(projectId);
	SetPreferredDeadlineInDays(preferredDeadlineInDays);
	SetFinalDeadlineInDays(finalDeadlineInDays);
	SetNbFrames(nbFrames);
	SetCredit(credit);
	SetCoreId(coreId);
}


void Project::Write(DataOutputStream& out) const
{
	out.Write(&mProjectId, sizeof(mProjectId));
	out.Write(&mPreferredDeadlineInDays, sizeof(mPreferredDeadlineInDays));
	out.Write(&mFinalDeadlineInDays, sizeof(mFinalDeadlineInDays));
	out.Write(&mNbFrames, sizeof(mNbFrames));
	out.Write(&mCredit, sizeof(mCredit));
	out.Write(&mCoreId, sizeof(mCoreId));
}


void Project::Read(DataInputStream& in)
{
	in.Read(&mProjectId, sizeof(mProjectId));
	in.Read(&mPreferredDeadlineInDays, sizeof(mPreferredDeadlineInDays));
	in.Read(&mFinalDeadlineInDays, sizeof(mFinalDeadlineInDays));
	in.Read(&mNbFrames, sizeof(mNbFrames));
	in.Read(&mCredit, sizeof(mCredit));
	in.Read(&mCoreId, sizeof(mCoreId));
}
