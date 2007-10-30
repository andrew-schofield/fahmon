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

#ifndef _PROJECT_H
#define _PROJECT_H


#include "core.h"
#include "fahmonTypes.h"
#include "dataInputStream.h"
#include "dataOutputStream.h"


class Project
{
protected:
	CoreId    mCoreId;
	WuCredit  mCredit;
	wxUint16  mPreferredDeadlineInDays;
	wxUint16  mFinalDeadlineInDays;
	wxUint16  mNbFrames;
	ProjectId mProjectId;


public:
	Project(ProjectId projectId = 0, wxUint16 preferredDeadlineInDays = 0, wxUint16 finalDeadlineInDays = 0, wxUint16 nbFrames = 0, WuCredit credit = 0, CoreId coreId = Core::UNKNOWN);

	void Write(DataOutputStream& out) const;
	void Read(DataInputStream& in);

	// --- Setters
	void SetProjectId(ProjectId projectId)             {mProjectId = projectId;}
	void SetPreferredDeadlineInDays(wxUint16 deadline) {mPreferredDeadlineInDays = deadline;}
	void SetFinalDeadlineInDays(wxUint16 deadline)     {mFinalDeadlineInDays = deadline;}
	void SetNbFrames(wxUint16 nbFrames)                {mNbFrames = nbFrames;}
	void SetCredit(WuCredit credit)                    {mCredit = credit;}
	void SetCoreId(CoreId coreId)                      {mCoreId = coreId;}

	// --- Getters
	CoreId    GetCoreId(void)                  const {return mCoreId;}
	WuCredit  GetCredit(void)                  const {return mCredit;}
	wxUint16  GetPreferredDeadlineInDays(void) const {return mPreferredDeadlineInDays;}
	wxUint16  GetFinalDeadlineInDays(void)     const {return mFinalDeadlineInDays;}
	wxUint16  GetNbFrames(void)                const {return mNbFrames;}
	ProjectId GetProjectId(void)               const {return mProjectId;}
};


#endif /* _PROJECT_H */
