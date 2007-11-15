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

#ifndef _QUEUE_H
#define _QUEUE_H


#include "fahmonTypes.h"
#include "wx/datetime.h"

/**
* qd
* Copyright (C) 2002-2005 Richard P. Howell IV.
* Copyright (C) 2005-2007 Sebastiaan Couwenberg
* This is free software; you can distribute it and/or modify it under
* the terms of the GNU General Public License.  There is no warranty
* whatsoever.
*/

#define be4(x)	(((x)[0]<<24)|(((x)[1]&0xFF)<<16)|(((x)[2]&0xFF)<<8)|((x)[3]&0xFF))
#define le4(x)	(((x)[3]<<24)|(((x)[2]&0xFF)<<16)|(((x)[1]&0xFF)<<8)|((x)[0]&0xFF))
#define le2(x)	((((x)[1]&0xFF)<<8)|((x)[0]&0xFF))
#define exch(x, y, t)	{t=x;x=y;y=t;}

#define TIME_OFS 946684800U	/* Linux epoch is 1970, FAH (Cosm) is 2000 */

typedef unsigned int u32;
typedef unsigned short u16;

struct queue
{
	u32 version; /* 0000 Queue (client) version (v2.17 and above) */
	u32 current; /* 0004 Current index number */
	struct qs
	{
		u32 stat;     /* 000 Status */
		char   z004[4];  /* 004 Pad for Windows, others as of v4.01 */
		u32 tdata[8]; /* 008 Time data (epoch 0000 1jan00 UTC) */
		u32 svr1;     /* 040 Server IP address (until v3.0) */
		u32 ustat;    /* 044 Upload status */
		char   url[128]; /* 048 Web address for core downloads */
		u32 m176;     /* 176 Misc1a */
		u32 core;     /* 180 Core_xx number (hex) */
		u32 m184;     /* 184 Misc1b */
		u32 dsiz;     /* 188 wudata_xx.dat file size */
		char	z192[16];
		union
		{
			struct
			{
				char proj[2];     /* 208 Project number (LE) */
				char run[2];      /* 210 Run (LE) */
				char clone[2];    /* 212 Clone (LE) */
				char gen[2];      /* 214 Generation (LE) */
				char issue[2][4]; /* 216 WU issue time (LE) */
			} f; /* Folding@home data */
			struct
			{
				char   proj[2];     /* 208 Project number (LE) */
				u16 miscg1;      /* 210 Miscg1 */
				char   issue[2][4]; /* 212 WU issue time (LE) */
				u16 miscg2;      /* 220 Miscg2 */
				u16 miscg3;      /* 222 Miscg3 */
			} g; /* Genome@home data */
		} wuid; /* 208 Work unit ID information */
		char   z224[36];
		char   mid[4];    /* 260 Machine ID (LE) */
		u32 svr2;      /* 264 Server IP address */
		u32 port;      /* 268 Server port number */
		char   type[64];  /* 272 Work unit type */
		char   uname[64]; /* 336 User Name */
		char   teamn[64]; /* 400 Team Number */
		char   uid[8];    /* 464 Stored ID for unit (UserID + MachineID) (LE or BE, usually BE) */
		char   bench[4];  /* 472 Benchmark (as of v3.24) (LE) */
		char   m476[4];   /* 476 Misc3b (unused as of v3.24) (LE) */
		u32 cpu_type;  /* 480 CPU type (LE or BE, sometimes 0) */
		u32 os_type;   /* 484 OS type (LE or BE, sometimes 0) */
		u32 cpu_spec;  /* 488 CPU species (LE or BE, sometimes 0) */
		u32 os_spec;   /* 492 OS species (LE or BE, sometimes 0) */
		u32 expire;    /* 496 Allowed time to return (seconds) */
		char   z500[8];
		char   aiflag[4]; /* 508 Assignment info present flag (LE or BE) */
		char   aitime[4]; /* 512 Assignment timestamp (LE or BE) */
		char   aidata[4]; /* 516 Assignment info (LE or BE) */
		char   csip[4];   /* 520 Collection server IP address (as of v5.00) (LE) */
		char   dstart[4]; /* 524 Download started time (as of v5.00) (BE) */
		char   z528[16];
		char   cores[4];  /* 544 Number of SMP cores (as of v5.91) (BE) */
		char   tag[16];   /* 548 Tag of Work Unit (as of v5.00) */
		char   z564[52];
		char   memory[4]; /* 616 Available memory (as of v6.00) (BE) */
		char   z620[68];
		u32 due[4];    /* 688 WU expiration time */
		u32 plimit;    /* 704 Packet size limit (as of v5.00) */
		u32 uploads;   /* 708 Number of upload failures (as of v5.00) */
	} entry[10]; /* 0008 Array of ten queue entries */
	u32 pfract;    /* 7128 Performance fraction (as of v3.24) */
	u32 punits;    /* 7132 Performance fraction unit weight (as of v3.24) */
	u32 drate;     /* 7136 Download rate sliding average (as of v4.00) */
	u32 dunits;    /* 7140 Download rate unit weight (as of v4.00) */
	u32 urate;     /* 7144 Upload rate sliding average (as of v4.00) */
	u32 uunits;    /* 7148 Upload rate unit weight (as of v4.00) */
	char   z7152[16]; /* 7152 (as of v5.00) ...all zeros after queue conversion... */
};

/* Endian-swap functions */

u32 es32(u32 i)				/* 4-byte endian swap */
{
	return ((i << 24) | ((i & 0xFF00) << 8)
			| ((i & 0xFF0000) >> 8) | ((i & 0xFF000000) >> 24));
}

void eswp(struct queue *bp, u32 qver, u32 systype)	/* Swap endianness of entire queue file */
{
	int i, n;
	struct queue::qs *p;

	bp->version = es32(bp->version);
	bp->current = es32(bp->current);
	for (n = 10; --n >= 0; )
	{	p = &bp->entry[n];
	if (qver < 324)
		p = (struct queue::qs *) ((char *) p - 16 * n);
	if (qver < 500)
		p = (struct queue::qs *) ((char *) p - 8 * n);
	if ((qver < 401) && (systype != 1))
		p = (struct queue::qs *) ((char *) p - 4 * n);
	p->stat = es32(p->stat);
	if ((qver < 401) && (systype != 1))
		p = (struct queue::qs *) ((char *) p - 4);
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
		i = es32(i);
	p->svr2 = es32(p->svr2);
	p->port = es32(p->port);
	p->expire = es32(p->expire);
	if (qver >= 324)
	{	p->due[0] = es32(p->due[0]);
	p->due[1] = es32(p->due[1]);
	p->due[2] = es32(p->due[2]);
	p->due[3] = es32(p->due[3]);
	}
	if (qver >= 500)
	{	p->uploads = es32(p->uploads);
	p->plimit  = es32(p->plimit);
	/* may also have to swap p->cores and p->tag */
	}
	}
	if (qver < 500)
		bp = (struct queue *) ((char *) bp - 80);
	if (qver >= 324)
	{	bp->pfract = es32(bp->pfract);
	bp->punits = es32(bp->punits);
	}
	if (qver >= 400)
	{	bp->drate  = es32(bp->drate);
	bp->dunits = es32(bp->dunits);
	bp->urate  = es32(bp->urate);
	bp->uunits = es32(bp->uunits);
	}
}
/**
* End qd
**/

#endif /* _QUEUE_H */
