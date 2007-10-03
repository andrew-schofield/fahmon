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


/**
 * Return a pointer to a valid FrameDuration object if information could be extracted, NULL otherwhise
 * It's up to the caller to free the FrameDuration object when needed
**/
WorkUnitFrame* FahLogAnalyzer::AnalyzeLastFrame(const wxString& fahlog)
{
    bool           shouldStopParsing;
    bool           endOfFrameFound;
    bool           startOfFrameFound;
    bool           clientIsStopped;
    wxInt32        endOfLinePos;
    wxInt32        elapsedSeconds;
    wxInt32        duration;
    wxUint32       stepsPerFrame;
    wxUint32       elapsedSteps;
    wxString       currentLine;
    wxString       partOfLogNotParsed;
    LogLineInfo    endOfFrameInfo;
    LogLineInfo    startOfFrameInfo;
    LogLineInfo   *lineInfo;
    WorkUnitFrame *lastFrame;
    

    // --- We first remove empty lines (if any) located at the end of the log
    endOfLinePos = fahlog.Length();
    while(fahlog.Last() == '\n' || fahlog.Last() == '\r')
        --endOfLinePos;
    partOfLogNotParsed = fahlog.Mid(0, endOfLinePos);


    // --- Check each line, starting from the end of the log
    clientIsStopped   = false;
    endOfFrameFound   = false;
    shouldStopParsing = false;
    startOfFrameFound = false;

    while(shouldStopParsing == false)
    {
        // 1) Extract the last line from the part of the log we are considering
        endOfLinePos = partOfLogNotParsed.Find('\n', true);
        if(endOfLinePos == -1)
        {
            // No more lines after this one, we should stop parsing the file
            currentLine       = partOfLogNotParsed;
            shouldStopParsing = true;
        }
        else
        {
            currentLine        = partOfLogNotParsed.Mid(endOfLinePos + 1);
            partOfLogNotParsed = partOfLogNotParsed.Mid(0, endOfLinePos);
        }

        // 2) Determine what we are searching for
        if(endOfFrameFound == false)
            lineInfo = &endOfFrameInfo;
        else
            lineInfo = &startOfFrameInfo;

        // 3) Extract needed information from this line
        ParseLogLine(currentLine, *lineInfo);

        // 4) Keep only completed frames, stop when an empty line is found
        if(lineInfo->type == LLT_SHUTDOWN)
        {
            clientIsStopped   = true;
            shouldStopParsing = true;
        }
        else if(lineInfo->type == LLT_EMPTY)
            shouldStopParsing = true;
        else if(lineInfo->type == LLT_COMPLETED || lineInfo->type == LLT_FINISHED)
        {
            if(endOfFrameFound == false)
                endOfFrameFound = true;
            else
            {
                startOfFrameFound = true;
                shouldStopParsing = true;
            }
        }
    }
    
    
    if(clientIsStopped == true)
    {
        lastFrame = new WorkUnitFrame();
        
        lastFrame->SetClientIsStopped(true);

        return lastFrame;
    }
    // If we found a complete run, then we can extract information about it
    else if(endOfFrameFound == true && startOfFrameFound == true)
    {
        lastFrame = new WorkUnitFrame(endOfFrameInfo.frameId);
        
        // Compute the elapsed seconds since this frame has been completed
        elapsedSeconds = wxDateTime::Now().Subtract(endOfFrameInfo.timestamp).GetSeconds().ToLong();
        if(elapsedSeconds >= 0)
            lastFrame->SetElapsedSeconds(elapsedSeconds);
        else
            lastFrame->SetElapsedSeconds(elapsedSeconds + 86400);


        // Compute a (perhaps temporary) frame duration: it can be modified for LLT_COMPLETED lines
        duration = endOfFrameInfo.timestamp.Subtract(startOfFrameInfo.timestamp).GetSeconds().GetLo();
        
        if(duration >= 0)
            lastFrame->SetDuration(duration, false);
        else
            lastFrame->SetDuration(duration + 86400, false);
        
        
        // Check if the run is complete, extrapolate the time if needed, using the number of steps
        // For this kind of WUs, the frame id is always a percentage
        // QMD WUs are a real pain...
        if(startOfFrameInfo.type == LLT_COMPLETED)
        {
            elapsedSteps  = endOfFrameInfo.nbSteps - startOfFrameInfo.nbSteps;
            stepsPerFrame = endOfFrameInfo.totalSteps / 100;
            if(elapsedSteps != stepsPerFrame && elapsedSteps != 0)
            {
                // The run is not complete, extrapolate the time
                duration = lastFrame->GetDuration() * stepsPerFrame / elapsedSteps;

                lastFrame->SetDuration(duration, true);
            }
        }

        // The object is ready now
        return lastFrame;
    }
    
    return NULL;
}


/**
 * Take a line of a FAHLog file, and extract information from it
**/
void FahLogAnalyzer::ParseLogLine(const wxString& completeLineToParse, LogLineInfo& logLineInfo)
{
    wxInt32       position;
    wxString      lineToParse;
    unsigned long convertedNumber;

    
    // We will progressively remove the parsed elements from lineToParse
    // For now, we have to consider the whole line
    lineToParse = completeLineToParse;
    

    // --- 1) Check if there is a timestamp
    if(lineToParse.GetChar(0) == '[')
    {
        // Extract the timestamp
        // The rest of the date (day, month and year) is automatically set to today
        // Folding@Home client write dates in GMT format, so tell it the wxDateTime
        logLineInfo.timestamp.SetToCurrent();
        logLineInfo.timestamp.ParseFormat(lineToParse.c_str(), wxT("[%H:%M:%S]"));
        logLineInfo.timestamp.MakeGMT();

        // Remove the timestamp
        lineToParse = lineToParse.Mid(11);
    }


    // --- 2) Find the type of this line
    if(lineToParse.IsEmpty() == true)
        logLineInfo.type = LLT_EMPTY;
    else if(lineToParse.StartsWith(wxT("Completed ")) == true)
    {
        logLineInfo.type = LLT_COMPLETED;
        lineToParse      = lineToParse.Mid(10);         // Remove the string "Completed "

        // We first have the current number of steps
        lineToParse.ToULong(&convertedNumber);
        logLineInfo.nbSteps = convertedNumber;

        // We then have the total number of steps
        position = lineToParse.Find(wxT(" out of "));
        if(position != -1)
        {
            lineToParse = lineToParse.Mid(position +  8);
            lineToParse.ToULong(&convertedNumber);
            logLineInfo.totalSteps = convertedNumber;
        }

        // And finally, the frame number
        position = lineToParse.Find(wxT("("));
        if(position != -1)
        {
            lineToParse = lineToParse.Mid(position + 1);
            lineToParse.ToULong(&convertedNumber);
            logLineInfo.frameId = (FrameId)convertedNumber;
        }
    }
    else if(lineToParse.StartsWith(wxT("Finished a frame")) == true)
    {
        logLineInfo.type = LLT_FINISHED;
        lineToParse      = lineToParse.Mid(17);  // Remove the string "Finished a frame "
        
        // We should find the frame number between parenthesis
        if(lineToParse.GetChar(0) == '(')
        {
            lineToParse = lineToParse.Mid(1);
            lineToParse.ToULong(&convertedNumber);

            logLineInfo.frameId = (FrameId)convertedNumber;
        }
    }
    else if(lineToParse.StartsWith(wxT("Folding@Home Client Shutdown.")) == true)
        logLineInfo.type = LLT_SHUTDOWN;
    else
        logLineInfo.type = LLT_UNKNOWN;
}
