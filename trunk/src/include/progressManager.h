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
 * \file progressManager.h
 * Manages progress bar.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _PROGRESSMANAGER_H
#define _PROGRESSMANAGER_H


#include "wx/progdlg.h"


/**
 * Progress management class.
 * This class displays some progress information to the user, thanks to a wxProgressDialog
 *
 * It is easier to use when different tasks have to be performed, because each of them does not have to
 * know the percentage of work they represent compared to the total work to be done
 *
 * It is also easier to use in the case where a method will sometimes not display its progress (silent mode)
 * Thus, the test does not have to be done in the method itself, making it clearer to read (and to write).
 **/
class ProgressManager
{
protected:
	bool              mIsInSilentMode; /**< Is manager in silent mode */
	bool              mIsATaskActive; /**< Is a task active */
	wxUint32          mCurrentProgress; /**< Current overall progress */
	wxUint32          mTaskCurrentProgress;  /**< Current task progress */
	wxUint32          mTaskPercentageOfTotal; /**< How much task is worth */
	wxString          mCurrentText; /**< Text value */
	wxProgressDialog *mProgressDlg; /**< Progress Dialog object */


public:
	/**
	 * Constructor.
	 *
	 * If silent mode is on, future calls to methods won't do anything.
	 **/
	ProgressManager(bool isInSilentMode);

	/**
	 * Destructor.
	 **/
	~ProgressManager(void);

	/**
	 * Change the displayed text.
	 *
	 * Return false if the user wants to cancel the process
	 * @param text The text to set.
	 **/
	bool SetText(const wxString& text);

	/**
	 * Change the displayed progress.
	 *
	 * Return false if the user wants to cancel the process
	 * @param progress The progress to set.
	 **/
	bool SetProgress(wxUint32 progress);

	/**
	 * Change both text and progress,
	 * This is the same as calling SetText() and SetProgress()
	 *
	 * Return false if the user wants to cancel the process.
	 * @param text The text to set.
	 * @param progress The progress to set.
	 **/
	bool SetTextAndProgress(const wxString& text, wxUint32 progress);

	/**
	 * A task is represented by the percentage of the total work it represents.
	 * Only one task can be active at a time (EndTask() must be called before calling CreateTask() a second time)
	 * @param percentageOfTotal How much the task is worth.
	 **/
	void CreateTask(wxUint32 percentageOfTotal);

	/**
	 * End a task previously created with CreateTask().
	 **/
	void EndTask(void);
};


#endif /* _PROGRESSMANAGER_H */
