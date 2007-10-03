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

#ifndef _CLIENT_H
#define _CLIENT_H

#include "eta.h"
#include "wx/string.h"
#include "wx/thread.h"
#include "wx/datetime.h"
#include "workUnitFrame.h"


/**
 * Preferences used by this class
**/
#define PREF_FAHCLIENT_COLLECTXYZFILES    wxT("FahClient.CollectXYZFiles")
#define PREF_FAHCLIENT_COLLECTXYZFILES_DV false


class Client
{
protected:
    static wxMutex mMutexXYZFiles;

    ETA        mETA;
    bool       mIsOk;             // Is the client valid?
    bool       mIsRunning;        // Is it running?
    FrameId    mPreviouslyAnalyzedFrameId;
    wxString   mName;
    wxString   mLocation;
    wxString   mLog;
    wxUint32   mProjectId;
    wxString   mProjectString;
    wxUint32   mProgress;
    wxString   mProgressString;
    wxString   mUserName;
    wxUint32   mTeamNumber;
    wxDateTime mDownloadDate;

    void ComputeETA(WorkUnitFrame* lastFrame);
    void DetectInactivity(WorkUnitFrame* lastFrame);
    bool LoadLogFile(const wxString& filename);
    bool LoadUnitInfoFile(const wxString& filename);
    bool LoadClientCfg(const wxString& filename);
    void Reset(void);
    void SaveXYZFile(void) const;


public:
     Client(const wxString& name, const wxString& location);
    ~Client(void);

    void Reload(void);

    // -- 'Setters' --
    void SetName(const wxString& name) {mName = name;}
    void SetLocation(const wxString& location);

    // -- 'Getters' --
    bool       IsOk(void)              const {return mIsOk;}
    bool       IsRunning(void)         const {return mIsRunning;}
    wxString   GetName(void)           const {return mName;}
    wxString   GetLocation(void)       const {return mLocation;}
    wxString   GetLog(void)            const {return mLog;}
    wxUint32   GetProjectId(void)      const {return mProjectId;}
    wxString   GetProjectString(void)  const {return mProjectString;}
    wxUint32   GetProgress(void)       const {return mProgress;}
    wxString   GetProgressString(void) const {return mProgressString;}
    wxString   GetUserName(void)       const {return mUserName;}
    wxUint32   GetTeamNumber(void)     const {return mTeamNumber;}
    wxDateTime GetDownloadDate(void)   const {return mDownloadDate;}
    
    const ETA* GetETA(void) const {return &mETA;}
};


#endif /* _CLIENT_H */
