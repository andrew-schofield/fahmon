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
 * \file fahlogAnalyzer.h
 * Creates a class for analyzing FAHlog.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _FAHLOGANALYZER_H
#define _FAHLOGANALYZER_H


#include "wx/datetime.h"
#include "workUnitFrame.h"


/**
 * FAHlog analyzer class.
 * This class will try to extract some information from a FAHLog file.
 **/
class FahLogAnalyzer
{
protected:

	/**
	 * Log Line Type definition.
	 **/
	typedef enum _LOG_LINE_TYPE
	{
		LLT_COMPLETED, /**< [10:36:18] Completed 770000 out of 1000000 steps  (77%) */
		LLT_FINISHED, /**< [13:25:00] Finished a frame (1) */
		LLT_EMPTY, /**< An empty line (excluding the timestamp) */
		LLT_SHUTDOWN, /**< Folding@Home Client Shutdown. */
		LLT_WU_COMPLETE, /**< A WU has been completed */
		LLT_WU_STARTED, /**< A WU has been started */
		LLT_UNKNOWN /**< Any other type of line */
	} LogLineType;

	/**
	 * Build a structure containing info about each log line.
	 **/
	typedef struct _LOG_LINE
	{
		LogLineType type; /**< The type of log line */
		FrameId     frameId; /**< Valid when (type == LLT_FINISHED) or (type == LLT_COMPLETED) */
		wxDateTime  timestamp; /**< The timestamp on the current line [xx:yy:zz] */
		wxUint32    project; /**< The project for the current logline */
		wxUint32    completedSteps; /**< The number of completed steps (if line contains such information) */
		wxUint32    totalSteps; /**< The total number of steps (if line contains such information) */
	} LogLine;

	/**
	 * Take a line of a FAHLog file, and extract information from it.
	 * Beware that the 'lineToParse' parameter is modified by this function !
	 * @param lineToParse The string representation of the current line.
	 * @param logLine The LogLine structure to write to.
	 **/
	static void ParseLogLine(wxString& lineToParse, LogLine& logLine);


public:
	/**
	 * Return a pointer to a valid WorkUnitFrame object if information could be extracted, NULL otherwise.
	 * It's up to the caller to free the WorkUnitFrame object when needed
	 *
	 *
	 *
	 * We are searching for a pattern like this one :
	 *
	 * [16:04:36] Completed 6900000 out of 12500000 steps  (55%)   <-- We know the run below is complete because of this line
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
	 * @param fahlogComplete The FAHlog to analyze
	 * @return A WorkUnitFrame object representing the last frame.
	 **/
	static WorkUnitFrame* AnalyzeLastFrame(const wxString& fahlogComplete);
};


#endif /* _FAHLOGANALYZER_H */
