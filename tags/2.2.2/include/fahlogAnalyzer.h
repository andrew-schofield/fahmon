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

#ifndef _FAHLOGANALYZER_H
#define _FAHLOGANALYZER_H


#include "wx/datetime.h"
#include "workUnitFrame.h"


/**
 * This class will try to extract some information from a FAHLog file
**/
class FahLogAnalyzer
{
protected:
    
    // The different types of lines that can be found in a FAHLog file
    typedef enum _LOG_LINE_TYPE
    {
        LLT_COMPLETED,          // [10:36:18] Completed 770000 out of 1000000 steps  (77%)
        LLT_FINISHED,           // [13:25:00] Finished a frame (1)
        LLT_EMPTY,              // An empty line (excluding the timestamp)
        LLT_SHUTDOWN,           // Folding@Home Client Shutdown.
        LLT_WU_COMPLETE,        // A WU has been completed
        LLT_WU_STARTED,         // A WU has been started
        LLT_UNKNOWN             // Any other type of line
    } LogLineType;

    // Information that can be extracted from a line
    typedef struct _LOG_LINE
    {
        LogLineType type;
        FrameId     frameId;       // Valid when (type == LLT_FINISHED) or (type == LLT_COMPLETED)
        wxDateTime  timestamp;
        wxUint32    project;
    } LogLine;


    static void ParseLogLine(wxString& lineToParse, LogLine& logLine);


public:
    static WorkUnitFrame* AnalyzeLastFrame(const wxString& fahlogComplete);
};


#endif /* _FAHLOGANALYZER_H */
