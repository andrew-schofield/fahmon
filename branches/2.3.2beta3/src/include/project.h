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
 * \file project.h
 * Project object.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _PROJECT_H
#define _PROJECT_H


#include "core.h"
#include "fahmonTypes.h"
#include "dataInputStream.h"
#include "dataOutputStream.h"

/**
 * The project class.
 **/
class Project
{
protected:
	CoreId    mCoreId; /**< The project coreId */
	WuCredit  mCredit; /**< The project credit value */
	wxUint16  mPreferredDeadlineInDays; /**< The project preferred deadline */
	wxUint16  mFinalDeadlineInDays; /**< The project final deadline */
	wxUint16  mNbFrames; /**< The number of frames in the current WU */
	ProjectId mProjectId; /**< The project projectId */


public:
	/**
	 * Contructor.
	 * Contructs the project object.
	 * @param projectId The Project number from the PRCG.
	 * @param preferredDeadlineInDays Project preferred deadline.
	 * @param finalDeadlineInDays Project final deadline.
	 * @param nbFrames Frame count.
	 * @param credit Project credit value.
	 * @param coreId Project core type.
	 **/
	Project(ProjectId projectId = 0, wxUint16 preferredDeadlineInDays = 0, wxUint16 finalDeadlineInDays = 0, wxUint16 nbFrames = 0, WuCredit credit = 0, CoreId coreId = Core::UNKNOWN);

	/**
	 * Write this project to the disk.
	 * @param out The output stream.
	 **/
	void Write(DataOutputStream& out) const;

	/**
	 * Read the project from the disk.
	 * @param in The input stream.
	 **/
	void Read(DataInputStream& in);

	// --- Setters
	/**
	 * Set the project number.
	 **/
	void SetProjectId(ProjectId projectId)             {mProjectId = projectId;}

	/**
	 * Set the preferred deadline.
	 **/
	void SetPreferredDeadlineInDays(wxUint16 deadline) {mPreferredDeadlineInDays = deadline;}

	/**
	 * Set the final deadline.
	 **/
	void SetFinalDeadlineInDays(wxUint16 deadline)     {mFinalDeadlineInDays = deadline;}

	/**
	 * Set the frame count.
	 **/
	void SetNbFrames(wxUint16 nbFrames)                {mNbFrames = nbFrames;}

	/**
	 * Set the credit value.
	 **/
	void SetCredit(WuCredit credit)                    {mCredit = credit;}

	/**
	 * Set the core type.
	 **/
	void SetCoreId(CoreId coreId)                      {mCoreId = coreId;}

	// --- Getters
	/**
	 * Get the core type.
	 **/
	CoreId    GetCoreId(void)                  const {return mCoreId;}

	/**
	 * Get the credit value.
	 **/
	WuCredit  GetCredit(void)                  const {return mCredit;}

	/**
	 * Get the preferred deadline.
	 **/
	wxUint16  GetPreferredDeadlineInDays(void) const {return mPreferredDeadlineInDays;}

	/**
	 * Get the final deadline.
	 **/
	wxUint16  GetFinalDeadlineInDays(void)     const {return mFinalDeadlineInDays;}

	/**
	 * Get the frame number.
	 **/
	wxUint16  GetNbFrames(void)                const {return mNbFrames;}

	/**
	 * Get the project number.
	 **/
	ProjectId GetProjectId(void)               const {return mProjectId;}
};


#endif /* _PROJECT_H */
