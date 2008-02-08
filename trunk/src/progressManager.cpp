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
 * \file progressManager.cpp
 * Manages progress bar.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "progressManager.h"

#include "wx/icon.h"
#include "pathManager.h"


ProgressManager::ProgressManager(bool isInSilentMode)
{
	mIsInSilentMode        = isInSilentMode;
	mCurrentText           = wxT("");
	mCurrentProgress       = 0;
	mIsATaskActive         = false;
	mTaskCurrentProgress   = 0;
	mTaskPercentageOfTotal = 0;


	// We only need the progress dialog when silent mode is off
	// We pass a long 'empty' string so that the dialog won't be too small
	if(mIsInSilentMode == false)
	{
		mProgressDlg = new wxProgressDialog(wxT(FMC_PRODUCT), wxT("                                                                    "), 100, NULL, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME);
	}
}


ProgressManager::~ProgressManager(void)
{
	if(mIsInSilentMode == false)
	{
		delete mProgressDlg;
	}
}


bool ProgressManager::SetText(const wxString& text)
{
	wxUint32 progressToDisplay;
	wxString textToDisplay;

	if(mIsInSilentMode == true)
	{
		return true;
	}

	if(mIsATaskActive == true)
	{
		progressToDisplay = mCurrentProgress + ((mTaskCurrentProgress * mTaskPercentageOfTotal) / 100);
		textToDisplay     = mCurrentText + wxT(" (") + text + wxT(")");
	}
	else
	{
		progressToDisplay = mCurrentProgress;
		mCurrentText      = text;
		textToDisplay     = mCurrentText;
	}

	if(progressToDisplay > 100)
	{
		progressToDisplay = 100;
	}

	return mProgressDlg->Update(progressToDisplay, textToDisplay);
}


bool ProgressManager::SetProgress(wxUint32 progress)
{
	wxUint32 progressToDisplay;

	if(mIsInSilentMode == true)
	{
		return true;
	}

	// Decide whether the new progress is about the total progress, or the progress of the current task
	if(mIsATaskActive == true)
	{
		mTaskCurrentProgress = progress;
		if(mTaskCurrentProgress > 100)
		{
			mTaskCurrentProgress = 100;
		}

		progressToDisplay = mCurrentProgress + ((mTaskCurrentProgress * mTaskPercentageOfTotal) / 100);
	}
	else
	{
		mCurrentProgress = progress;
		if(mCurrentProgress > 100)
		{
			mCurrentProgress = 100;
		}

		progressToDisplay = mCurrentProgress;
	}

	return mProgressDlg->Update(progressToDisplay);
}


bool ProgressManager::SetTextAndProgress(const wxString& text, wxUint32 progress)
{
	bool textResult;
	bool progressResult;

	textResult     = SetText(text);
	progressResult = SetProgress(progress);

	return (textResult && progressResult);
}


void ProgressManager::CreateTask(wxUint32 percentageOfTotal)
{
	if(mIsInSilentMode == true)
	{
		return;
	}

	wxASSERT(mIsATaskActive == false);

	mIsATaskActive         = true;
	mTaskPercentageOfTotal = percentageOfTotal;
	mTaskCurrentProgress   = 0;
}


void ProgressManager::EndTask(void)
{
	if(mIsInSilentMode == true)
	{
		return;
	}

	wxASSERT(mIsATaskActive == true);

	mIsATaskActive    = false;
	mCurrentProgress += mTaskPercentageOfTotal;

	SetProgress(mCurrentProgress);
}
