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
 * \file workUnitFrame.h
 * Create a class to store data about a frame.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _WORKUNITFRAME_H
#define _WORKUNITFRAME_H


#include "fahmonTypes.h"
#include "wx/datetime.h"


// TODO
// Use FrameDuration instead of wxUint32?


/**
 * A WorkUnitFrame.
 * Stores information about a given frame of a work unit
 * It can be either a 'real' frame (Tinker) or a percentage (Gromacs, DGromacs...)
 **/
class WorkUnitFrame
{
protected:
	bool     mClientIsStopped;        /**< true if the message 'Folding@Home Client Shutdown.' was found. In this case, other information is not valid! */
	FrameId  mId;                     /**< The ID of the current frame */
	wxUint32 mDuration;               /**< Duration of current frame in seconds */
	wxUint32 mElapsedSeconds;         /**< Elapsed time since this frame has been completed in seconds */
	wxUint32 mEffectiveDuration;      /**< Effective frame time value in seconds */
	wxUint32 mFrameCount;             /**< Number of "frames" in the current WU */


public:
	/**
	 * Constructor.
	 * Contructs a WorkUnitFrame.
	 * @param id Current frame ID
	 * @param clientIsStopped Is client running or not?
	 * @param duration Duration of frame in seconds
	 * @param elapsedSeconds Elapsed time since frame was completed in seconds.
	 * @param effectiveDuration The effective frame time in seconds.
	 * @param frames The number of "frames" in the current WU.
	 **/
	WorkUnitFrame(FrameId id = 0, bool clientIsStopped = true, wxUint32 duration = 0, wxUint32 elapsedSeconds = 0, wxUint32 effectiveDuration = 0, wxUint32 frames = 0);

	/**
	 * Destructor.
	 **/
	~WorkUnitFrame(void);

	// --- Getters
	/**
	 * @return Whether the client is stopped or not.
	 **/
	bool     ClientIsStopped(void)        const {return mClientIsStopped;}
	/**
	 * @return The current frame ID.
	 **/
	FrameId  GetId(void)                  const {return mId;}
	/**
	 * @return The duration of the current frame in seconds.
	 **/
	wxUint32 GetDuration(void)            const {return mDuration;}
	/**
	 * @return The time since the current frame was completed in seconds.
	 **/
	wxUint32 GetElapsedSeconds(void)      const {return mElapsedSeconds;}
	/**
	 * @return The effective duration of the current frame.
	 **/
	wxUint32 GetEffectiveDuration(void)   const {return mEffectiveDuration;}
	/**
	 * @return The number of "frames"in the current WU.
	 **/
	wxUint32 GetFrameCount(void)          const {return mFrameCount;}

	// --- Setters
	/**
	 * Set the current frame ID.
	 * @param id The frame ID to be set.
	 **/
	void SetId(FrameId id)                                {mId = id;}

	/**
	 * Set the current client state.
	 * @param isStopped Is client stopped?.
	 **/
	void SetClientIsStopped(bool isStopped)               {mClientIsStopped = isStopped;}

	/**
	 * Set the duration of the current frame.
	 * @param duration The duration to set.
	 **/
	void SetDuration(wxUint32 duration)                   {mDuration = duration;}

	/**
	 * Set the elapsed time since the current frame finished.
	 * @param seconds The duration to be set.
	 **/
	void SetElapsedSeconds(wxUint32 seconds)              {mElapsedSeconds = seconds;}

	/**
	 * Set the effective duration of the current frame.
	 * @param effectiveDuration The effective duration to be set.
	 **/
	void SetEffectiveDuration(wxUint32 effectiveDuration) {mEffectiveDuration = effectiveDuration;}

	/**
	 * Set the frame count of the current WU.
	 * @param frameCount The number of frames to set for the WU.
	 **/
	void SetFrameCount(wxUint32 frameCount)               {mFrameCount = frameCount;}
};


#endif /* _WORKUNITFRAME_H */
