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
 
#ifndef _PROGRESSMANAGER_H
#define _PROGRESSMANAGER_H


#include "wx/progdlg.h"


// This class displays some progress information to the user, thanks to a wxProgressDialog
//
// It is easier to use when different tasks have to be performed, because each of them does not have to
// know the percentage of work they represent compared to the total work to be done
//
// It is also easier to use in the case where a method will sometimes not display its progress (silent mode)
// Thus, the test does not have to be done in the method itself, making it clearer to read (and to write)
class ProgressManager
{
protected:
    bool              mIsInSilentMode;
    bool              mIsATaskActive;
    wxUint32          mCurrentProgress;
    wxUint32          mTaskCurrentProgress;
    wxUint32          mTaskPercentageOfTotal;
    wxString          mCurrentText;
    wxProgressDialog *mProgressDlg;


public:
     ProgressManager(bool isInSilentMode);
    ~ProgressManager(void);

    bool SetText(const wxString& text);
    bool SetProgress(wxUint32 progress);
    bool SetTextAndProgress(const wxString& text, wxUint32 progress);

    void CreateTask(wxUint32 percentageOfTotal);
    void EndTask(void);
};


#endif /* _PROGRESSMANAGER_H */
