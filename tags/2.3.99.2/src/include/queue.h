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

/*
* qd
* Copyright (C) 2002-2005 Richard P. Howell IV.
* Copyright (C) 2005-2010 Sebastiaan Couwenberg
* This is free software; you can distribute it and/or modify it under
* the terms of the GNU General Public License.  There is no warranty
* whatsoever.
*/

/**
 * \file queue.h
 * Structural definition of queue.dat.
 * This file contains the structures and conversion routines to allow access to
 * all elements of a Folding\@home or Genome\@home queue.dat
 * In FahMon, only the PRCG, issue date, username and teamname are used for anything
 * useful.
 * \author Andrew Schofield
 **/

#ifndef _QUEUE_H
#define _QUEUE_H


#include "fahmonTypes.h"
#include "wx/datetime.h"

class Queue
{
protected:

/**
 * 4 byte Big Endian convertor.
 * Macro that converts the byte order of a given integer to Big Endian if not already.
 * @param x 4 byte (32bit) integer
 **/
	#define be4(x)	(((x)[0]<<24)|(((x)[1]&0xFF)<<16)|(((x)[2]&0xFF)<<8)|((x)[3]&0xFF))

/**
 * 4 byte Little Endian convertor.
 * Macro that converts the byte order of a given integer to Little Endian if not already.
 * @param x 4 byte (32bit) integer
 **/
	#define le4(x)	(((x)[3]<<24)|(((x)[2]&0xFF)<<16)|(((x)[1]&0xFF)<<8)|((x)[0]&0xFF))


/**
 * 2 byte Little Endian convertor.
 * Macro that converts the byte order of a given integer to Little Endian if not already.
 * @param x 2 byte (16bit) integer
 **/
	#define le2(x)	((((x)[1]&0xFF)<<8)|((x)[0]&0xFF))

/**
 * Cycles three values from right to left.
 * Macro that moves a set of three values from right to left
 * Before: {1, 2, 3}
 * After:  {2, 3, 1}
 **/
	#define exch(x, y, t)	{t=x;x=y;y=t;}

/**
 * Timezone offset for Linux vs Cosm epoch.
 * Linux epoch is 1970, FAH (Cosm) is 2000, therefore this offset must be applied to
 * keep times correct.
 **/
	#define COSM_EPOCH_OFFSET 946684800U

/**
 * Maximum supported queue version.
 * This is the maximum known value the version field of queue.dat can take. Higher
 * values will cause FahMon to give a warning message, but it will still try and get
 * any info it can out of the queue.
 **/
	#define MAXQVER 600

/**
 * Type definition for a 32bit unsigned int.
 * This needs to be used instead of wxUint32 to allow correct endian swapping.
 **/
	typedef unsigned int   u32;

/**
 * Type definition for a 16bit unsigned int.
 * This needs to be used instead of wxUint16 to allow correct endian swapping.
 **/
	typedef unsigned short u16;

	ProjectId  mProjectId; /**< Object variable storing the WU project number */
	wxInt16    mRun; /**< Object variable storing the WU run number */
	wxInt16    mGen; /**< Object variable storing the WU generation number */
	wxInt16    mClone; /**< Object variable storing the WU clone number */
	wxDateTime mDownloadDate; /**< Object variable storing the WU download date */
	wxString   mUserName; /**< Object variable storing the username associated with the WU assignment */
	wxUint32   mTeamNumber; /**< Object variable storing the team number associated with the WU assignment */
	wxUint32   mUnitIndex; /**< Object variable storing the current work unit index in the queue */

/**
 * Format of queue.dat.
 * The queue is accesible as a struct of type queue
 * each queue entry is accessible as a struct of queue::qs->entry[x]
 **/
	struct queueformat
	{
		u32 version; /**< 0000 Queue (client) version (v2.17 and above) */
		u32 current; /**< 0004 Current index number */
		struct qs
		{
			u32  stat;         /**< 000 Status */
			char use_cores[4]; /**< 004 Pad for Windows, others as of v4.01, as of v6.01 number of SMP Cores to use (LE) */
			u32  tdata[8];     /**< 008 Time data (epoch 0000 1jan00 UTC) */
			u32  svr1;         /**< 040 Server IP address (until v3.0) */
			u32  ustat;        /**< 044 Upload status */
			char url[128];     /**< 048 Web address for core downloads */
			u32  m176;         /**< 176 Misc1a */
			u32  core;         /**< 180 Core_xx number (hex) */
			u32  m184;         /**< 184 Misc1b */
			u32  dsiz;         /**< 188 wudata_xx.dat file size */
			char z192[16];
			union
			{
				struct
				{
					char proj[2];     /**< 208 Project number (LE) */
					char run[2];      /**< 210 Run (LE) */
					char clone[2];    /**< 212 Clone (LE) */
					char gen[2];      /**< 214 Generation (LE) */
					char issue[2][4]; /**< 216 WU issue time (LE) */
				} f; /**< Folding\@home data */
				struct
				{
					char proj[2];     /**< 208 Project number (LE) */
					u16  miscg1;      /**< 210 Miscg1 */
					char issue[2][4]; /**< 212 WU issue time (LE) */
					u16  miscg2;      /**< 220 Miscg2 */
					u16  miscg3;      /**< 222 Miscg3 */
				} g; /**< Genome\@home data */
			} wuid; /**< 208 Work unit ID information */
			char z224[36];
			char mid[4];      /**< 260 Machine ID (LE) */
			u32  svr2;        /**< 264 Server IP address */
			u32  port;        /**< 268 Server port number */
			char type[64];    /**< 272 Work unit type */
			char uname[64];   /**< 336 User Name */
			char teamn[64];   /**< 400 Team Number */
			char uid[8];      /**< 464 Stored ID for unit (UserID + MachineID) (LE or BE, usually BE) */
			char bench[4];    /**< 472 Benchmark (as of v3.24) (LE) */
			char m476[4];     /**< 476 Misc3b (unused as of v3.24) (LE) */
			u32  cpu_type;    /**< 480 CPU type (LE or BE, sometimes 0) */
			u32  os_type;     /**< 484 OS type (LE or BE, sometimes 0) */
			u32  cpu_spec;    /**< 488 CPU species (LE or BE, sometimes 0) */
			u32  os_spec;     /**< 492 OS species (LE or BE, sometimes 0) */
			u32  expire;      /**< 496 Allowed time to return (seconds) */
			char z500[8];
			char aiflag[4];   /**< 508 Assignment info present flag (LE or BE) */
			char aitime[4];   /**< 512 Assignment timestamp (LE or BE) */
			char aidata[4];   /**< 516 Assignment info (LE or BE) */
			char csip[4];     /**< 520 Collection server IP address (as of v5.00) (LE) */
			char dstart[4];   /**< 524 Download started time (as of v5.00) (BE) */
			char z528[16];
			char cores[4];    /**< 544 Number of SMP cores (as of v5.91) (BE) */
			char tag[16];     /**< 548 Tag of Work Unit (as of v5.00) */
			char z564[16];
			char passkey[32]; /**< 580 Passkey (as of v6.00) */
			char flops[4];    /**< 612 Flops per CPU (core) (as of v6.00) (BE) */
			char memory[4];   /**< 616 Available memory (as of v6.00) (BE) */
			char z620[68];
			u32  due[4];      /**< 688 WU expiration time */
			u32  plimit;      /**< 704 Packet size limit (as of v5.00) */
			u32  uploads;     /**< 708 Number of upload failures (as of v5.00) */
		} entry[10]; /**< 0008 Array of ten queue entries */
		u32  pfract;          /**< 7128 Performance fraction (as of v3.24) */
		u32  punits;          /**< 7132 Performance fraction unit weight (as of v3.24) */
		u32  drate;           /**< 7136 Download rate sliding average (as of v4.00) */
		u32  dunits;          /**< 7140 Download rate unit weight (as of v4.00) */
		u32  urate;           /**< 7144 Upload rate sliding average (as of v4.00) */
		u32  uunits;          /**< 7148 Upload rate unit weight (as of v4.00) */
		char results_sent[4]; /**< 7152 Results successfully sent (after upload failures) (as of v5.00) (LE) */
		char z7156[12];       /**< 7152 (as of v5.00) ...all zeros after queue conversion... */
	};

/**
 * 4-byte endian swap.
 * Swaps the byte order of a given unsigned int32.
 * \note The internal wxWidgets macro probably works too.
 * @param i u32 (Unsigned 32bit Integer).
 * @return A u32 (Unsigned 32bit Integer).
 **/
	u32 es32(u32 i);


/**
 * Swap endianness of entire queue file.
 * In the case of PPC Mac queues on x86, or x86 queues on PPC Mac, the entire queue needs to have
 * a swapped byte order.
 * @param *bp Pointer to the queue buffer.
 * @param qver The version of the current queue (read previously).
 * @param systype The current system type (Windows, Linux, PPC Mac, x86 Mac).
 * @return none.
**/
	void eswp(struct queueformat *bp, u32 qver, u32 systype);

public:
	/**
	 * Contructor.
	 * Constructs a new queue object.
	 *
	**/
	Queue(void);

	/**
	 * Destructor.
	**/
	~Queue(void);

	/**
	 * Loads a given queue.dat file.
	 * Processes queue.dat to extract useful information and stores it in protected object variables.
	 * @param filename The input queue.dat file.
	 * @param clientName The associated client name for the queue.
	 * @returns true/false depending on whether the queue could be read successfully.
	**/
	bool LoadQueueFile(wxString const &filename, wxString const &clientName, wxString& localname);

	ProjectId         GetProjectId(void)    const {return mProjectId;} /**< Returns the WU project number */
	wxInt16           GetProjectRun(void)   const {return mRun;} /**< Returns the WU run number */
	wxInt16           GetProjectGen(void)   const {return mGen;} /**< Returns the WU generation number */
	wxInt16           GetProjectClone(void) const {return mClone;} /**< Returns the WU clone number */
	wxString          GetUserName(void)     const {return mUserName;} /**< Returns the user name associated with the assignment */
	wxUint32          GetTeamNumber(void)   const {return mTeamNumber;} /**< Returns the team number associated with the assignment */
	const wxDateTime& GetDownloadDate(void) const {return mDownloadDate;} /**< Returns the WU download date */
	wxUint32          GetUnitIndex(void)    const {return mUnitIndex;} /**< Returns the Work Unit index */

/**
 * End qd
**/
};

#endif /* _QUEUE_H */
