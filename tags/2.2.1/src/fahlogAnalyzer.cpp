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

#include "fahmon.h"
#include "fahlogAnalyzer.h"
#include "messagesManager.h"
#include "preferencesManager.h"
#include "mainDialog.h"


/**
 * Return a pointer to a valid WorkUnitFrame object if information could be extracted, NULL otherwhise
 * It's up to the caller to free the WorkUnitFrame object when needed
 *
 *
 *
 * We are searching for a pattern like this one :
 *
 * [16:04:36] Completed 6900000 out of 12500000 steps  (55%)   <-- We now the run below is complete because of this line
 * ...
 * [16:25:32] Completed 7000000 out of 12500000 steps  (56%)   <-+
 * ...                                                           |-- Complete run
 * [16:52:12] Completed 7125000 out of 12500000 steps  (57%)   <-+
 *
 *
 *
 * For Tinker units, this pattern is sufficient :
 *
 * [16:26:42] Finished a frame (2) <-+
 * ...                               |-- Complete run
 * [16:38:42] Finished a frame (3) <-+
 *
**/
WorkUnitFrame* FahLogAnalyzer::AnalyzeLastFrame(const wxString& fahlogComplete)
{
    bool      completeRunFound, emptyLineFound, endOfLogReached, clientIsStopped, newWUStarted;
    bool      endFrame1Found, endFrame2Found;
    bool      overrideTimezone, ignoreAsynchrony;
    wxInt32   endOfLine;
    wxInt32   elapsedSeconds, runDuration;
    wxInt32   timezoneoffset;
    LogLine   endFrame1, endFrame2, endFrame3;
    LogLine   *currentLogLine;
    wxString  lineToParse;
    wxString  fahlog;

    fahlog           = fahlogComplete;
    endFrame1Found   = false;
    endFrame2Found   = false;
    emptyLineFound   = false;
    endOfLogReached  = false;
    clientIsStopped  = false;
    completeRunFound = false;
    newWUStarted     = false;

    // Remove empty lines (if any) located at the end of the log
    while(fahlog.Last() == '\n' || fahlog.Last() == '\r')
        fahlog.RemoveLast();

    // An empty line means that we've reached the time where the client was started, therefore we won't be able to find a complete run
    // actually this is wrong, since -verbosity 9 when periodically trying to return a finished WU inserts blank lines.
    while(!completeRunFound && !endOfLogReached && !clientIsStopped )
    {
        if(endFrame1Found == false) currentLogLine = &endFrame1;
        else if(endFrame2Found == false) currentLogLine = &endFrame2;
        else                             currentLogLine = &endFrame3;

        // Extract the next line to parse
        endOfLine = fahlog.Find('\n', true);
        if(endOfLine == -1)
        {
            lineToParse     = fahlog;
            endOfLogReached = true;
        }
        else
        {
            lineToParse = fahlog.Mid(endOfLine + 1);
            fahlog      = fahlog.Mid(0, endOfLine);
        }

        // Parse it
        ParseLogLine(lineToParse, *currentLogLine);

        // What did we find ?
        switch(currentLogLine->type)
        {
            case LLT_SHUTDOWN:
                clientIsStopped = true;
                break;

            case LLT_EMPTY:
                emptyLineFound = true;
                break;

            case LLT_WU_STARTED:
                endOfLogReached = true;
                break;

            case LLT_COMPLETED:
                if(endFrame1Found == false)      endFrame1Found   = true;
                else if(endFrame2Found == false) endFrame2Found   = true;
                else                             completeRunFound = true;
                break;

            case LLT_FINISHED:
                if(endFrame1Found == false) endFrame1Found   = true;
                else                        completeRunFound = true;
                break;

            default:
                break;
        }
    }

    // A stopped client is not analyzed
    if(clientIsStopped)
        return new WorkUnitFrame();

    // If we found a complete run, then we can extract information about it
    if(completeRunFound)
    {
        // Compute the frame duration
        // If endFrame2.timestamp is before endFrame1.timestamp, we can assume that we passed midnight, because the two timestamps come from
        // the same machine
        runDuration = endFrame1.timestamp.Subtract(endFrame2.timestamp).GetSeconds().GetLo();
        if(runDuration < 0)
            runDuration += 86400;

        // We won't deal with too large durations, so stop now if it is the case with this one
        if(runDuration > MAX_FRAME_DURATION)
            return NULL;

        // Compute the elapsed seconds since this frame has been completed
        // If wxDateTime::Now() is before endFrame1.timestamp, we don't use elapsed time because if FahMon is running on a different machine
        // than the client, we can't assume that they are sufficiently synchronized

        _PrefsGetBool(PREF_OVERRIDE_TIMEZONE, overrideTimezone);
        _PrefsGetBool(PREF_IGNORE_ASYNCHRONY, ignoreAsynchrony);
        if(overrideTimezone != true)
        {
            elapsedSeconds = wxDateTime::Now().ToUTC().Subtract(endFrame1.timestamp).GetSeconds().ToLong();
        }
        else
        {
            _PrefsGetInt(PREF_TZ, timezoneoffset);
            elapsedSeconds = wxDateTime::Now().Subtract(endFrame1.timestamp + wxTimeSpan::Hours(timezoneoffset)).GetSeconds().ToLong();
        }
        // asynchronous clocks
        if(elapsedSeconds < 0)
        {
            _LogMsgInfo(wxString::Format(wxT("Possible clock asynchrony detected!")));
            if(ignoreAsynchrony)
            {
            // forces the client to be detected as still active (may break other detection routines though)
                //maybe a bit risky doing this
                elapsedSeconds = 65535;
            }
            else
            {
                elapsedSeconds = 0;
            }
        }

        return new WorkUnitFrame(endFrame1.frameId, false, runDuration, elapsedSeconds);
    }

    return NULL;
}


/**
 * Take a line of a FAHLog file, and extract information from it
 * Beware that the 'lineToParse' parameter is modified by this function !
**/
void FahLogAnalyzer::ParseLogLine(wxString& lineToParse, LogLine& logLine)
{
    wxInt32       position;
    wxString      timestamp;
    unsigned long convertedNumber;

    // If the line is empty, we don't do anything
    if(lineToParse.IsEmpty() == true)
    {
        logLine.type = LLT_EMPTY;
        return;
    }

    // Check if a timestamp is present, extract it if this is the case
    if(lineToParse.GetChar(0) == '[')
    {
        timestamp   = lineToParse.Mid(1, 10);
        lineToParse = lineToParse.Mid(11);
    }

    // Trim heading spaces
    // This will empty the line if it contains only spaces
    lineToParse.Trim(false);

    // Find the type of the line
    if(lineToParse.IsEmpty())                                                         logLine.type = LLT_EMPTY;
    else if(lineToParse.StartsWith(wxT("Completed ")))                                logLine.type = LLT_COMPLETED;
    else if(lineToParse.StartsWith(wxT("Finished a frame")))                          logLine.type = LLT_FINISHED;
    else if(lineToParse.StartsWith(wxT("Folding@Home Client Shutdown.")))             logLine.type = LLT_SHUTDOWN;
    // not used yet:
    else if(lineToParse.StartsWith(wxT("Folding@home Core Shutdown: FINISHED_UNIT"))) logLine.type = LLT_WU_COMPLETE;
    else if(lineToParse.StartsWith(wxT("Protein")))                                   logLine.type = LLT_WU_STARTED;
    else if(lineToParse.StartsWith(wxT("Project")))                                   logLine.type = LLT_WU_STARTED;
    else                                                                              logLine.type = LLT_UNKNOWN;

    // When a frame is finished, extract its identifier and the timestamp
    if(logLine.type == LLT_COMPLETED || logLine.type == LLT_FINISHED)
    {
        // Extract the frame identifier
        // We can't pass logLine.frameId to ToULong() because it's too short : the next bytes would be modified
        position = lineToParse.Find(wxT("("));
        if(position != -1)
        {
            lineToParse.Right(lineToParse.Len()-position-1).ToULong(&convertedNumber);
            logLine.frameId = (FrameId)convertedNumber;
        }
	//extra bit for GPU WUs
	else if(lineToParse.Mid(10, lineToParse.Len()-10).ToULong(&convertedNumber) == true)
	{
	    logLine.frameId = (FrameId)convertedNumber;
	}

        // Extract the timestamp (it is in GMT format)
        logLine.timestamp.SetToCurrent();
        logLine.timestamp.ParseFormat(timestamp.c_str(), wxT("%H:%M:%S]"));
    }
}
