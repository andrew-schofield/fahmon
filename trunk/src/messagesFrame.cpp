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
#include "messagesFrame.h"

#include "pathManager.h"
#include "messagesManager.h"
#include "preferencesManager.h"


// Events processed by this class
BEGIN_EVENT_TABLE(MessagesFrame, wxFrame)
	// --- Frame
	EVT_CLOSE   (MessagesFrame::OnClose)
END_EVENT_TABLE()


// The single instance of MessagesFrame accross the application
MessagesFrame* MessagesFrame::mInstance = NULL;


/**
* Constructor
**/
MessagesFrame::MessagesFrame(wxWindow *parent) : wxFrame(parent, wxID_ANY, wxString::Format(_("Messages / %s"), wxT(FMC_PRODUCT)))
{
	wxUint32    frameWidth;
	wxUint32    frameHeight;
	wxPanel    *panel;
	wxBoxSizer *topLevelSizer;
	wxBoxSizer *mainSizer;

	// This frame does not inherit the icon of the main dialog
#ifdef _FAHMON_LINUX_
	SetIcon(wxIcon(PathManager::GetImgPath() + wxT(FMC_FILE_IMG_DIALOG)));
#else
	SetIcon(wxICON(dialog_icon));
#endif

	// --- We don't really need a status bar, but with it on Windows a 'grip sizer' appears, so that users can see that the frame is resizable
	CreateStatusBar();


	// --- Construct the frame
	// We use a panel so that it look better under Windows, and a fixed pitch font, because that's easier to
	// read and to see if there's something wrong
	panel         = new wxPanel(this);
	mainSizer     = new wxBoxSizer(wxVERTICAL);
	topLevelSizer = new wxBoxSizer(wxVERTICAL);
	mTextCtrl     = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);

	mainSizer->Add(mTextCtrl, 1, wxEXPAND);
	mTextCtrl->SetFont(wxFont(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	topLevelSizer->Add(mainSizer, 1, wxEXPAND | wxALL, FMC_GUI_BORDER);
	panel->SetSizer(topLevelSizer);


	// --- Restore the size of the frame
	_PrefsGetUint(PREF_MESSAGESFRAME_FRAMEWIDTH,  frameWidth);
	_PrefsGetUint(PREF_MESSAGESFRAME_FRAMEHEIGHT, frameHeight);
	SetSize(frameWidth, frameHeight);
}


/**
* Destructor
**/
MessagesFrame::~MessagesFrame(void)
{
}


/**
* Retrieve the instance of MessagesFrame, create it if needed
**/
MessagesFrame* MessagesFrame::GetInstance(wxWindow* parent)
{
	if(mInstance == NULL)
		mInstance = new MessagesFrame(parent);

	return mInstance;
}


/**
* Destroy the instance of MessagesFrame, if any
**/
void MessagesFrame::DestroyInstance(void)
{
	if(mInstance != NULL)
	{
		// Save the size of the frame before cleaning
		_PrefsSetUint(PREF_MESSAGESFRAME_FRAMEWIDTH,  mInstance->GetSize().GetWidth());
		_PrefsSetUint(PREF_MESSAGESFRAME_FRAMEHEIGHT, mInstance->GetSize().GetHeight());

		mInstance->Destroy();
		mInstance = NULL;
	}
}


/**
* Show/Hide the frame
**/
void MessagesFrame::Toggle(void)
{
	if(IsShown() == false)
	{
		// Change the content of the wxTextCtrl with the current messages before displaying the window
		mTextCtrl->SetValue(MessagesManager::GetInstance()->GetMessages());
		mTextCtrl->ShowPosition(mTextCtrl->GetLastPosition());

		Show(true);
	}
	else
	{
		Show(false);
	}
}


/************************************  EVENTS  ************************************/


/**
* We don't allow the user to destroy the window, we simply hide it
**/
void MessagesFrame::OnClose(wxCloseEvent& event)
{
	Toggle();
}


/**
* A new message has been logged, it will be added only if the frame exists AND is visible
**/
void MessagesFrame::OnNewMessage(void)
{
	if(mInstance != NULL && mInstance->IsShown() == true)
	{
		mInstance->mTextCtrl->SetValue(MessagesManager::GetInstance()->GetMessages());
		mInstance->mTextCtrl->ShowPosition(mInstance->mTextCtrl->GetLastPosition());
	}
}
