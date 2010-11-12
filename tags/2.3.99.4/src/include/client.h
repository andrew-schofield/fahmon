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
 * \file client.h
 * Creates a class for every client.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _CLIENT_H
#define _CLIENT_H

#include "eta.h"
#include "wx/string.h"
#include "wx/thread.h"
#include "wx/datetime.h"
#include "workUnitFrame.h"


/**
 * Client class.
 * This class is created for every client and stores information about it's state,
 * current project etc.
 **/
class Client
{
protected:

	/**
	 * Client state definition.
	 **/
	typedef enum _STATE
	{
		ST_INACCESSIBLE, /**< Client is inaccessible */
		ST_STOPPED, /**< Client is stopped */
		ST_INACTIVE, /**< Client is inactive */
		ST_RUNNING, /**< Client is running */
		ST_ASYNCH, /** Client is in async state */
		ST_PAUSED, /** Client is paused */
		ST_HUNG, /** Client is hung */
	} State;

	static wxMutex mMutexReadWrite; /**< Make read/write variables thread-safe */

	ETA        mETA; /**< ETA for project client is working on */
	State      mState; /**< State of client */
	time_t     mLastModification; /**< Time FAHlog.txt was last modified */
	FrameId    mPreviouslyAnalyzedFrameId; /**< Data about last frame */
	wxString   mName; /**< Name of client */
	wxString   mLocation; /**< Location of client */
	wxString   mLog; /**< Client logfile */
	wxString   mProjectString; /**< Project name used for xyz filename */
	wxUint32   mProgress; /**< Current client progress */
	wxString   mProgressString; /**< String of progress + % */
	wxString   mUserName; /**< Username for current client */
	wxUint32   mTeamNumber; /**< Team number for current client */
	ProjectId  mProjectId; /**< Project number for client */
	wxInt16    mRun; /**< Run number for client */
	wxInt16    mClone; /**< Clone number for client */
	wxInt16    mGen; /**< Generation number for current client */
	double     mPPD; /**< PPD for client */
	wxDateTime mDownloadDate; /**< Download date for current WU */
	bool       mIsFrameCountAccurate; /**< Is frame count being guessed? */
	wxString   mCore; /**< Core for current client */
	wxUint32   mCredit; /**< Credit for current client */
	wxDateTime mDeadlineDate; /**< Preferred deadline for current client */
	bool       mEnabled; /**< Whether client should ever reload */
	bool       mVM; /**< Whether client is from a virtual machine */
	wxString   mClientType; /**< CPU/GPU/SMP */
	wxUint32   mUnitIndex; /**< Current work unit index */
	double     mCoreVersion; /**< Core version number */
	wxString   mPreviousFAHlog; /**< Previous location of temporary file (http/ftp) */
	wxString   mPreviousQueue; /**< Previous location of temporary file (http/ftp) */
	wxString   mPreviousUnitinfo; /**< Previous location of temporary file (http/ftp) */
	wxString   mPreviousLogfile; /**< Previous location of temporary file (http/ftp) */

	/**
	 * Calculate the client's ETA.
	 * Use registered benchmarks and the duration of the last frame to compute an ETA, if this is possible.
	 * @param lastFrame The frame to base calculations on
	 **/
	void ComputeETA(WorkUnitFrame* lastFrame);

	/**
	 * Determine the client's current state.
	 * @param lastFrame The frame to use.
	 **/
	void FindCurrentState(WorkUnitFrame* lastFrame);

	/**
	 * Load the file FAHlog\.txt.
	 * Line returns are not converted, as it seems they are correctly taken into account by the text component used
	 * @param filename Log file to load
	 **/
	bool LoadLogFile(wxString const &filename);

	/**
	 * Load information from the unitinfo\.txt file.
	 * Loads current progress if not determined from FAHlog.txt
	 * @param filename unitinfo to load
	 **/
	bool LoadUnitInfoFile(wxString const &filename);

	/**
	 * Load information from the logfile_xx.txt file.
	 * @param filename logfile to load
	 **/
	bool LoadWULogFile(wxString const &filename);

	/**
	 * Load lots of useful info from the queue\.dat file.
	 * @param filename queue file to load
	 */
	bool LoadQueueFile(wxString const &filename);

	/**
	 * Provide thread-safe method for setting client state.
	 * @param value The state to set
	 **/
	void SetState(State value);

	/**
	 * Provide thread-safe method for getting client state.
	 * @return The client state
	 **/
	const State GetState(void) const;


public:
	/**
	 * Constructor.
	 * Constructs a new client based on name and location.
	 * @param name Client name
	 * @param location Client location
	 * @param enabled Is client enabled
	 * @param VM Is client in a virtual machine
	 **/
	Client(wxString const &name, wxString const &location, bool enabled, bool VM);

	/**
	 * Destructor.
	 **/
	~Client(void);

	/**
	 * (Re)Load this client.
	 **/
	void Reload(void);

	/**
	 * Restore the initial state of the client.
	 * Resets everything to no information loaded
	 **/
	void Reset(void);

	/**
	 * Return the URL of the user's stats.
	 **/
	wxString GetDonatorStatsURL(void) const;

	/**
	 * Return the URL of the user's team stats.
	 **/
	wxString GetTeamStatsURL(void)    const;

	/**
	 * Returns the URL of the project page on Jmol website.
	 **/
	wxString GetJmolURL(void)         const;

	/**
	 * Returns the URL of the project page on fahinfo\.org.
	 **/
	wxString GetFahinfoURL(void)      const;

	// -- 'Setters' --
	/**
	 * Set client name.
	 * @param name Name to set.
	 **/
	void SetName(wxString const &name)            {mName = name;}

	/**
	 * Set client location.
	 * @param location Location to set.
	 **/
	void SetLocation(wxString const &location);

	/**
	 * Set state of frame accuracy
	 * @param isAccurate Is frame count accurate?
	 **/
	void SetIsFrameCountAccurate(bool isAccurate) {mIsFrameCountAccurate = isAccurate;}

	/**
	 * Enable or disable the client
	 * @param value Is client enabled?
	 **/
	void Enable(bool value) {mEnabled = value;}

	/**
	 * Set virtual machine state
	 * @param value Is client on a virtual machine
	 **/
	void SetVM(bool value) {mVM = value;}

	// -- 'Getters' --
	bool              IsAccessible(void)            const {return GetState() != ST_INACCESSIBLE;} /**< Returns whether client is inaccessible or not */
	bool              IsStopped(void)               const {return GetState() == ST_STOPPED;} /**< Returns whether client is stopped or not */
	bool              IsInactive(void)              const {return GetState() == ST_INACTIVE;} /**< Returns whether client is inactive or not */
	bool              IsRunning(void)               const {return GetState() == ST_RUNNING;} /**< Returns whether client is running or not */
	bool              IsAsynch(void)                const {return GetState() == ST_ASYNCH;} /**< Returns whether client is asynchronous or not */
	bool              IsHung(void)                  const {return GetState() == ST_HUNG;} /**< Returns whether client is hung or not */
	bool              IsPaused(void)                const {return GetState() == ST_PAUSED;} /**< Returns whether client is paused or not */
	wxString          GetName(void)                 const {return mName;} /**< Returns client name */
	wxString          GetLocation(void)             const {return mLocation;} /**< Returns client location */
	wxString          GetLog(void)                  const {return mLog;} /**< Returns client log */
	wxString          GetProjectString(void)        const {return mProjectString;} /**< Returns client project string for xyz files */
	wxUint32          GetProgress(void)             const {return mProgress;} /**< Returns client progress */
	wxString          GetProgressString(void)       const {return mProgressString;} /**< Returns client progress string */
	wxString          GetDonatorName(void)          const {return mUserName;} /**< Returns client username */
	wxUint32          GetTeamNumber(void)           const {return mTeamNumber;} /**< Returns client team number */
	ProjectId         GetProjectId(void)            const {return mProjectId;} /**< Returns client project number */
	wxInt16           GetProjectRun(void)           const {return mRun;} /**< Returns client Run number */
	wxInt16           GetProjectClone(void)         const {return mClone;} /**< Returns client Clone number */
	wxInt16           GetProjectGen(void)           const {return mGen;}  /**< Returns client Generation number */
	double            GetPPD(void)                  const {return mPPD;} /**< Returns client PPD */
	const wxDateTime& GetDownloadDate(void)         const {return mDownloadDate;} /**< Returns client download date */
	bool              GetIsFrameCountAccurate(void) const {return mIsFrameCountAccurate;} /**< Returns whether frame count is accurate or not */
	const ETA*        GetETA(void)                  const {return &mETA;} /**< Returns client ETA */
	wxString          GetCore(void)                 const {return mCore;} /**< Returns client core */
	wxUint32          GetCredit(void)               const {return mCredit;} /**< Returns client credit */
	const wxDateTime& GetDeadlineDate(void)         const {return mDeadlineDate;} /**< Returns client preferred deadline date */
	bool              IsEnabled(void)               const {return mEnabled;} /**< Returns whether client is enabled */
	bool              IsVM(void)                    const {return mVM;} /**< Returns whether client is from a virtual machine or not */
	wxString          GetClientType(void)           const {return mClientType;} /**< Returns the type of client */
	double            GetCoreVersion(void)          const {return mCoreVersion;} /**< Returns the core version */

	/**
	 * Return true if some modification occurred since the last reload
	 **/
	bool ReloadNeeded(void) const;

	wxString GetPreviousFAHlog(void) {return mPreviousFAHlog;}
	wxString GetPreviousQueue(void) {return mPreviousQueue;}
	wxString GetPreviousUnitinfo(void) {return mPreviousUnitinfo;}
	wxString GetPreviousLogfile(void) {return mPreviousLogfile;}

	void SetPreviousFAHlog(const wxString &value) {mPreviousFAHlog = value;}
	void SetPreviousQueue(const wxString &value) {mPreviousQueue = value;}
	void SetPreviousUnitinfo(const wxString &value) {mPreviousUnitinfo = value;}
	void SetPreviousLogfile(const wxString &value) {mPreviousLogfile = value;}
};


#endif /* _CLIENT_H */
