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
 
#ifndef _WORKUNITFRAME_H
#define _WORKUNITFRAME_H


#include "fahmonTypes.h"
#include "wx/datetime.h"


// TODO
// Use FrameDuration instead of wxUint32?


/**
 * Store information about a given frame of a work unit
 * It can be either a 'real' frame (Tinker) or a percentage (Gromacs, DGromacs...)
**/
class WorkUnitFrame
{
protected:
    bool     mClientIsStopped;        // true if the message 'Folding@Home Client Shutdown.' was found. In this case, other information is not valid!
    FrameId  mId;
    wxUint32 mDuration;               // In seconds
    wxUint32 mElapsedSeconds;         // Elapsed time since this frame has been completed


public:
     WorkUnitFrame(FrameId id = 0, bool clientIsStopped = true, wxUint32 duration = 0, wxUint32 elapsedSeconds = 0);
    ~WorkUnitFrame(void);

    // --- Getters
    bool     ClientIsStopped(void)        const {return mClientIsStopped;}
    FrameId  GetId(void)                  const {return mId;}
    wxUint32 GetDuration(void)            const {return mDuration;}
    wxUint32 GetElapsedSeconds(void)      const {return mElapsedSeconds;}

    // --- Setters
    void SetId(FrameId id)                   {mId = id;}
    void SetClientIsStopped(bool isStopped)  {mClientIsStopped = isStopped;}
    void SetDuration(wxUint32 duration)      {mDuration = duration;}
    void SetElapsedSeconds(wxUint32 seconds) {mElapsedSeconds = seconds;}
};


#endif /* _WORKUNITFRAME_H */
