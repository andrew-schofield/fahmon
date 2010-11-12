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

#ifndef _FAHMONTYPES_H
#define _FAHMONTYPES_H


// --- Identifier of a frame
// Can be either a percentage (Gromacs, Amber...) of a 'real' frame number (Tinker)
// For Tinkers, the highest value is currently 400
typedef wxUint16 FrameId;
#define MAX_FRAME_ID 65535


// --- Duration of a frame, in seconds
// This is not enough to store a frame that would take a whole day (65535 < 86400), but these ones won't be taken into account in this case
typedef wxUint16 FrameDuration;
#define MAX_FRAME_DURATION 65535


// --- Identifier (Integer) of a client, especially used to store benchmarks
typedef wxUint16 ClientId;
#define MAX_CLIENT_ID     65535
#define INVALID_CLIENT_ID 65535


// --- Identifier of a Folding@Home core
// One byte should be enough, there are currently 18 known cores
typedef wxByte CoreId;
#define MAX_CORE_ID 255


// --- Identifier of a Folding@Home project
// There are currently around 6000 projects, so 65535 should be enough for some times
typedef wxUint16 ProjectId;
#define MAX_PROJECT_ID     65535
#define INVALID_PROJECT_ID 0


// --- Credit (stats) given for a work unit
// The biggest WU is currently worth 3840 points, so 65535 is a good limit :)
typedef wxUint16 WuCredit;
#define MAX_WU_CREDIT 65535


// --- Type of a preference (bool, int...)
// We surely won't have more than 256 types of preferences
typedef wxByte PrefType;


// --- Type of file protocol (file, http)
typedef wxByte FileProtocol;
#define MAX_PROTOCOL_ID


#endif /* _FAHMONTYPES_H */
