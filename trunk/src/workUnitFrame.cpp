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
 * \file workUnitFrame.cpp
 * Create a class to store data about a frame.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "workUnitFrame.h"


WorkUnitFrame::WorkUnitFrame(FrameId id, bool clientIsStopped, wxUint32 duration, wxUint32 elapsedSeconds, wxUint32 effectiveDuration, wxUint32 frames, bool clientIsPaused)
{
	mId                = id;
	mDuration          = duration;
	mElapsedSeconds    = elapsedSeconds;
	mClientIsStopped   = clientIsStopped;
	mEffectiveDuration = effectiveDuration;
	mFrameCount        = frames;
	mClientIsPaused    = clientIsPaused;
}


WorkUnitFrame::~WorkUnitFrame(void)
{
}
