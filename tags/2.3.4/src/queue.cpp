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
 * \file queue.cpp
 * Structural definition of queue.dat.
 * This file contains the structures and conversion routines to allow access to
 * all elements of a Folding@home or Genome@home queue.dat
 * In FahMon, only the PRCG, download date, username and teamname are used for anything
 * useful.
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "queue.h"

#include "messagesManager.h"
#include "preferencesManager.h"
#include "mainDialog.h"

#include "wx/file.h"

typedef unsigned int   u32;

Queue::Queue()
{
	mProjectId = INVALID_PROJECT_ID;
}


Queue::~Queue(void)
{
}

bool Queue::LoadQueueFile(const wxString& filename, wxString clientName)
{
	queueformat       queuebuffer, *bp;
	queueformat::qs  *p;
	FILE             *fp;
	wxInt32           i, queueversion, systype, n, estat, na, nc, it;
	wxUint32          *tp;
	char             *q;
	bool              genome, endianswap;
	unsigned long     tmpLong;
	bool              overrideTZ;
	wxInt32           TZ;

	_PrefsGetBool(PREF_OVERRIDE_TIMEZONE, overrideTZ);
	_PrefsGetInt (PREF_TZ,                TZ);

#ifdef __WXGTK__
	systype = 0;
#elif _FAHMON_WIN32_
	systype = 1;
#elif defined(__ppc__) && defined(__WXMAC__)
	systype = 2;
#elif defined(__i386__) && defined(__WXMAC__)
	systype = 3;
#endif


	// Try to open the file, check if it exists
	if(!wxFileExists(filename))
	{
		return false;
	}
	if((fp = fopen(filename.mb_str(), "rb")) == NULL)
	{
		return false;
	}
	else
	{
		i = fread(&queuebuffer, 1, sizeof(struct queueformat), fp);
		fclose(fp);
		if(i < 6844)
		{
			_LogMsgWarning(_T("Invalid queue!"));
			return false;
		}
		q = NULL;
		// we should ALWAYS know the system type here, so no need to check it
		genome = (queuebuffer.version > 0xFFFF) && ((queuebuffer.current > 0xFFFF) || (queuebuffer.current == 0));
		queueversion = 0;
		wxString tmpWxStr(_T(""));
		switch(i)
		{
			case 6884:
				queueversion = 217;
				goto tw;
			case 6888:
				queueversion = 314;
				goto tw;
			case 7048:
			case 7056:
				queueversion = 324;
				tw:				tmpWxStr = _T("1Windows");
				break;
			case 6848:
				queueversion = 314;
				goto tl;
			case 7008:
			case 7016:
			case 7032:
				queueversion = 324;
tl:				if (genome)
#if defined(__ppc__) && defined(__WXMAC__)
					tmpWxStr = _T("0Linux");
#else
					tmpWxStr = _T("2Mac/PPC");
				else
					tmpWxStr = _T("0Linux");
#endif
				break;
			case 7072:
				queueversion = 401;
				goto tt;
			case 7168:
				queueversion = 500;
tt:				if (genome)
#if defined(__ppc__) && defined(__WXMAC__)
					tmpWxStr = _T("1Linux, Windows or Mac/x86");
#else
					tmpWxStr = _T("2Mac/PPC");
				else if (queuebuffer.version == 400)
					tmpWxStr = _T("1Windows");
#endif
				break;
		}
		q = (char *)tmpWxStr.c_str();

		if (q != NULL)
			systype = *q++ - '0';
		//endian swap for PPC machines and queues
		endianswap = FALSE;
#if defined(__ppc__) && defined(__WXMAC__)
		if (systype != 2)
			endianswap = TRUE;
#else
		if (systype == 2)
			endianswap = TRUE;
#endif
		i = queuebuffer.version;
		if (endianswap)
		{
			i = es32(i);
		}
		if (i > 9)
		{
			bp = &queuebuffer;
			if ((queueversion == 314) && ((i >= 324) && (i < 400)))
			{
				tmpWxStr = _T("Google");
				q = (char *)tmpWxStr.c_str();
				i = queueversion;
			}
			if (i > MAXQVER)
			{

				_LogMsgInfo(wxString::Format(_T("%s Queue version %i.%02d"), clientName.c_str(), i / 100, i % 100));
				_LogMsgInfo(_T("WARNING: unknown version number"));
				if (queueversion == 0)
				{
					queueversion = 400;
				}
			}
			else
			{
				queueversion = i;
			}
		}
		else
		{
			bp = (struct queueformat *) ((char *) &queuebuffer - 4);
			queueversion = 0;
		}
		if (endianswap)
		{
			eswp(&queuebuffer, queueversion, systype);
		}
		//_LogMsgInfo(wxString::Format(_T("Current index: %u"), bp->current));
		n = (bp->current & 0xFF) % 10;
		p = &bp->entry[n];
		if (queueversion < 324)
		{
			p = (struct queueformat::qs *) ((char *) p - 16 * n);
		}
		if (queueversion < 500)
		{
			p = (struct queueformat::qs *) ((char *) p - 8 * n);
		}
		if ((queueversion < 401) && (systype != 1))
		{
			p = (struct queueformat::qs *) ((char *) p - 4 * n);
			estat = p->stat;
			p = (struct queueformat::qs *) ((char *) p - 4);
		}
		else
		{
			estat = p->stat;
		}
		tp = p->tdata;
		if ((queueversion <= 9) || (systype == 2))
		{
			++tp;
		}
		// we don't really need estat, as WU state is irrelevant at present
		genome = ((p->core == 0xC9) || (p->core == 0xCA));
		if ((systype == 2) && (!genome))
		{
			na = (p->wuid.f.run[0] & 0xFE) + (p->wuid.f.clone[0] & 0xFE) + (p->wuid.f.gen[0] & 0xFE);
			nc = (p->wuid.f.run[1] & 0xFE) + (p->wuid.f.clone[1] & 0xFE) + (p->wuid.f.gen[1] & 0xFE);
			if (na <= nc)
			{
				exch(p->wuid.f.run[0], p->wuid.f.run[1], na);
				exch(p->wuid.f.clone[0], p->wuid.f.clone[1], na);
				exch(p->wuid.f.gen[0], p->wuid.f.gen[1], na);
			}
		}
		mProjectId = genome ? le2(p->wuid.g.proj) : le2(p->wuid.f.proj);
		mRun = le2(p->wuid.f.run);
		mGen = le2(p->wuid.f.gen);
		mClone = le2(p->wuid.f.clone);
		wxString username(p->uname, wxConvUTF8);
		wxString teamnumber(p->teamn, wxConvUTF8);
		it = tp[0] + COSM_EPOCH_OFFSET;
		mDownloadDate = it;
		mUserName = username;
		if(teamnumber.ToULong(&tmpLong) == true)
		{
			mTeamNumber  = (wxUint32)tmpLong;
		}
		return true;
	}
	return true;
}


u32 Queue::es32(u32 i)
{
	return ((i << 24) | ((i & 0xFF00) << 8) | ((i & 0xFF0000) >> 8) | ((i & 0xFF000000) >> 24));
}


void Queue::eswp(struct queueformat *bp, u32 qver, u32 systype)
{
	int                     i, n;
	struct queueformat::qs *p;

	bp->version = es32(bp->version);
	bp->current = es32(bp->current);
	for (n = 10; --n >= 0; )
	{
		p = &bp->entry[n];
		if (qver < 324)
		{
			p = (struct queueformat::qs *) ((char *) p - 16 * n);
		}
		if (qver < 500)
		{
			p = (struct queueformat::qs *) ((char *) p - 8 * n);
		}
		if ((qver < 401) && (systype != 1))
		{
			p = (struct queueformat::qs *) ((char *) p - 4 * n);
		}
		p->stat = es32(p->stat);
		if ((qver < 401) && (systype != 1))
		{
			p = (struct queueformat::qs *) ((char *) p - 4);
		}
		p->tdata[0] = es32(p->tdata[0]);
		p->tdata[1] = es32(p->tdata[1]);
		p->tdata[2] = es32(p->tdata[2]);
		p->tdata[3] = es32(p->tdata[3]);
		p->tdata[4] = es32(p->tdata[4]);
		p->tdata[5] = es32(p->tdata[5]);
		p->tdata[6] = es32(p->tdata[6]);
		p->tdata[7] = es32(p->tdata[7]);
		p->svr1 = es32(p->svr1);
		p->ustat = es32(p->ustat);
		p->m176 = es32(p->m176);
		p->core = es32(p->core);
		p->m184 = es32(p->m184);
		p->dsiz = es32(p->dsiz);
		if (((i = p->core) & 0xFF) == 0)
		{
			i = es32(i);
		}
		p->svr2 = es32(p->svr2);
		p->port = es32(p->port);
		p->expire = es32(p->expire);
		if (qver >= 324)
		{
			p->due[0] = es32(p->due[0]);
			p->due[1] = es32(p->due[1]);
			p->due[2] = es32(p->due[2]);
			p->due[3] = es32(p->due[3]);
		}
		if (qver >= 500)
		{
			p->uploads = es32(p->uploads);
			p->plimit  = es32(p->plimit);
			/* may also have to swap p->cores and p->tag */
		}
	}
	if (qver < 500)
	{
		bp = (struct queueformat *) ((char *) bp - 80);
	}
	if (qver >= 324)
	{
		bp->pfract = es32(bp->pfract);
		bp->punits = es32(bp->punits);
	}
	if (qver >= 400)
	{
		bp->drate  = es32(bp->drate);
		bp->dunits = es32(bp->dunits);
		bp->urate  = es32(bp->urate);
		bp->uunits = es32(bp->uunits);
	}
}
