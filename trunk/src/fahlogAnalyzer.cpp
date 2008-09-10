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
 * \file fahlogAnalyzer.cpp
 * Creates a class for analyzing FAHlog.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "fahlogAnalyzer.h"

#include "messagesManager.h"
#include "preferencesManager.h"
#include "mainDialog.h"


WorkUnitFrame* FahLogAnalyzer::AnalyzeLastFrame(const wxString& fahlogComplete)
{
	bool      completeRunFound, emptyLineFound, endOfLogReached, clientIsStopped, newWUStarted;
	bool      endFrame1Found, endFrame2Found;
	bool      overrideTimezone, ignoreAsynchrony, isAsync;
	wxInt32   endOfLine;
	wxInt32   elapsedSeconds, runDuration;
	wxInt32   timezoneoffset, frameCount, frameCountTemp1, frameCountTemp2;
	LogLine   endFrame1, endFrame2, endFrame3;
	LogLine   *currentLogLine;
	wxString  lineToParse;
	wxString  fahlog;
	wxDateTime tempDateTime;

	fahlog           = fahlogComplete;
	endFrame1Found   = false;
	endFrame2Found   = false;
	emptyLineFound   = false;
	endOfLogReached  = false;
	clientIsStopped  = false;
	completeRunFound = false;
	newWUStarted     = false;
	isAsync          = false;

	// Remove empty lines (if any) located at the end of the log
	while(fahlog.Last() == '\n' || fahlog.Last() == '\r')
	{
		fahlog.RemoveLast();
	}

	// An empty line means that we've reached the time where the client was started, therefore we won't be able to find a complete run
	// actually this is wrong, since -verbosity 9, when periodically trying to return a finished WU inserts blank lines.
	while(!completeRunFound && !endOfLogReached && !clientIsStopped )
	{
		if(endFrame1Found == false)
		{
			currentLogLine = &endFrame1;
		}
		else if(endFrame2Found == false)
		{
			currentLogLine = &endFrame2;
		}
		else
		{
			currentLogLine = &endFrame3;
		}

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
				if(endFrame1Found == false)
				{
					endFrame1Found = true;
				}
				else if(endFrame2Found == false)
				{
					endFrame2Found = true;
				}
				else if((endFrame1.frameId != endFrame2.frameId) && (endFrame1.frameId != endFrame3.frameId) && (endFrame2.frameId != endFrame3.frameId))
				{
					completeRunFound = true;
				}
				else
				{
					completeRunFound = false;
				}
				break;

			case LLT_FINISHED:
				if(endFrame1Found == false)
				{
					endFrame1Found = true;
				}
				else
				{
					completeRunFound = true;
				}
				break;

			default:
				break;
		}
	}

	// A stopped client is not analyzed
	if(clientIsStopped)
	{
		return new WorkUnitFrame();
	}

	// If we found a complete run, then we can extract information about it
	if(completeRunFound)
	{
		// Compute the frame duration
		// If endFrame2.timestamp is before endFrame1.timestamp, we can assume that we passed midnight, because the two timestamps come from
		// the same machine
		runDuration = endFrame1.timestamp.Subtract(endFrame2.timestamp).GetSeconds().GetLo();
		if(runDuration < 0)
		{
			runDuration += 86400;
		}
		// We won't deal with too large durations, so stop now if it is the case with this one
		if(runDuration > MAX_FRAME_DURATION)
		{
			return NULL;
		}

		_PrefsGetBool(PREF_OVERRIDE_TIMEZONE, overrideTimezone);
		_PrefsGetBool(PREF_IGNORE_ASYNCHRONY, ignoreAsynchrony);
		// Since the timestamps in the log only provide the time, the DateTime construct fills in the date component itself
		// Unfortunately, when passing over 0000UTC in local time, these dates no longer match, so the time difference becomes negative
		// and breaks the state detection. Therefore, the date component is set manually from the converted timezone, such that they
		// *always* match
		if(overrideTimezone != true)
		{
			tempDateTime = endFrame1.timestamp;
			tempDateTime.SetDay(wxDateTime::Now().ToUTC().GetDay());
			tempDateTime.SetMonth(wxDateTime::Now().ToUTC().GetMonth());
			tempDateTime.SetYear(wxDateTime::Now().ToUTC().GetYear());
			//potential asynchrony detection
			if (tempDateTime.IsLaterThan(wxDateTime::Now().ToUTC()))
			{
				isAsync = true;
			}
			else
			{
				isAsync = false;
			}
			// except if the client is still a "day" behind, the dates need to match again
			if (tempDateTime.IsLaterThan(wxDateTime::Now().ToUTC()))
			{
				tempDateTime = tempDateTime.Subtract(wxTimeSpan::Days(1));
			}
			elapsedSeconds = wxDateTime::Now().ToUTC().Subtract(tempDateTime).GetSeconds().ToLong();
		}
		else
		{
			_PrefsGetInt(PREF_TZ, timezoneoffset);
			tempDateTime = endFrame1.timestamp;
			tempDateTime.SetDay(wxDateTime::Now().Subtract(wxTimeSpan::Hours(timezoneoffset)).GetDay());
			tempDateTime.SetMonth(wxDateTime::Now().Subtract(wxTimeSpan::Hours(timezoneoffset)).GetMonth());
			tempDateTime.SetYear(wxDateTime::Now().Subtract(wxTimeSpan::Hours(timezoneoffset)).GetYear());
			if (tempDateTime.IsLaterThan(wxDateTime::Now().Subtract(wxTimeSpan::Hours(timezoneoffset))))
			{
				isAsync = true;
			}
			else
			{
				isAsync = false;
			}
			if (tempDateTime.IsLaterThan(wxDateTime::Now().Subtract(wxTimeSpan::Hours(timezoneoffset))))
			{
				tempDateTime = tempDateTime.Subtract(wxTimeSpan::Days(1));
			}
			elapsedSeconds = wxDateTime::Now().Subtract(wxTimeSpan::Hours(timezoneoffset)).Subtract(tempDateTime).GetSeconds().ToLong();
		}
		if(isAsync == true)
		{
			_LogMsgInfo(wxString::Format(_("Possible clock asynchrony detected!")));
			if(ignoreAsynchrony)
			{
				// forces the client to be detected as still active (may break other detection routines though)
				//maybe a bit risky doing this
				elapsedSeconds = 65535;
			}
		}
		// try to work out the number of markers (frames) that are going to be in the log file based on the "Completed xx of yy steps" line.
		// Otherwise assume frames = 100 (we will correct Tinker WUs elsewhere)

		if(endFrame1.totalSteps > 0 && endFrame2.totalSteps > 0 && endFrame3.totalSteps > 0)
		{
			if( (endFrame1.completedSteps - endFrame2.completedSteps) == (endFrame2.completedSteps - endFrame3.completedSteps) )
			{
				frameCountTemp1 = (endFrame1.completedSteps - endFrame3.completedSteps);
				if(frameCountTemp1 == 0) {
					frameCount = 0;
					// wxPuts(_T("frameCountTemp1 is ZERO"));
				} else {
					frameCountTemp2 = (( frameCountTemp1 ) * 100) / endFrame1.totalSteps;
					frameCount = 200 / frameCountTemp2;
				}
			} else {
				frameCount = 0;
			}
		} else {
			frameCount = 100;
		}
		return new WorkUnitFrame(endFrame1.frameId, false, runDuration, elapsedSeconds, 0, frameCount);
	}
	return NULL;
}


void FahLogAnalyzer::ParseLogLine(wxString& lineToParse, LogLine& logLine)
{
	wxInt32       position, compPos, stepsPos, outOfPos;
	wxString      timestamp;
	unsigned long convertedNumber;
	bool          gpustyleWU = false;

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
	if(lineToParse.IsEmpty())
	{
		logLine.type = LLT_EMPTY;
	}
	else if(lineToParse.StartsWith(wxT("Completed ")))
	{
		logLine.type = LLT_COMPLETED;
	}
	else if(lineToParse.Find(wxT(" steps ")) != wxNOT_FOUND)
	{
		logLine.type = LLT_COMPLETED;
	}
	else if(lineToParse.StartsWith(wxT("Finished a frame")))
	{
		logLine.type = LLT_FINISHED;
	}
	else if(lineToParse.StartsWith(wxT("Folding@Home Client Shutdown.")))
	{
		logLine.type = LLT_SHUTDOWN;
	}
	// this one is not used yet:
	else if(lineToParse.StartsWith(wxT("Folding@home Core Shutdown: FINISHED_UNIT")))
	{
		logLine.type = LLT_WU_COMPLETE;
	}
	else if(lineToParse.StartsWith(wxT("Protein")))
	{
		logLine.type = LLT_WU_STARTED;
	}
	else if(lineToParse.StartsWith(wxT("Project")))
	{
		logLine.type = LLT_WU_STARTED;
	}
	else
	{
		logLine.type = LLT_UNKNOWN;
	}

	// When a frame is finished, extract its identifier and the timestamp
	if(logLine.type == LLT_COMPLETED || logLine.type == LLT_FINISHED)
	{
		// Extract the frame identifier
		// We can't pass logLine.frameId to ToULong() because it's too short : the next bytes would be modified

		// For regular log messages of the type "Completed xxx of yyy steps (1)"
		// or "Finished frame (1)"
		position = lineToParse.Find(wxT("("));
		if(position != -1)
		{
			lineToParse.Right(lineToParse.Len()-position-1).ToULong(&convertedNumber);
			logLine.frameId = (FrameId)convertedNumber;
		}
		// Extra bit for GPU1 cores
		// core_10 writes "Completed 1"
		else if(lineToParse.Mid(10, lineToParse.Len()-10).ToULong(&convertedNumber) == true)
		{
			logLine.frameId = (FrameId)convertedNumber;
			gpustyleWU = true;
		}
		// Extra bit for early versions of 7b core and GPU2 cores
		// These cores write "Completed 1%"
		else if(lineToParse.Mid(10, lineToParse.Len()-11).ToULong(&convertedNumber) == true)
		{
			logLine.frameId = (FrameId)convertedNumber;
			gpustyleWU = true;
		}

		compPos = lineToParse.Find(wxT("Completed "));
		outOfPos = lineToParse.Find(wxT(" out of "));
		stepsPos = lineToParse.Find(wxT(" steps"));
		if (!(compPos == wxNOT_FOUND && outOfPos == wxNOT_FOUND && stepsPos == wxNOT_FOUND))
		{
			if(lineToParse.Mid(compPos+10, outOfPos-(compPos+10)).ToULong(&convertedNumber) == true)
			{
				logLine.completedSteps = (wxUint32)convertedNumber;
			}
			if(lineToParse.Mid(outOfPos+8, stepsPos-(outOfPos+8)).ToULong(&convertedNumber) == true)
			{
				logLine.totalSteps = (wxUint32)convertedNumber;
			}
		} else if (gpustyleWU == true) { //this isn't really the best way to do things, but it'll have to do for now.
			logLine.completedSteps = logLine.frameId;
			logLine.totalSteps = 100;
		}else {
			logLine.completedSteps = 0;
			logLine.totalSteps = 0;
		}
		// Extract the timestamp (it is in UTC format)
		logLine.timestamp.SetToCurrent();
		logLine.timestamp.ParseFormat(timestamp.c_str(), wxT("%H:%M:%S]"));
	}
}
