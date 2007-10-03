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
#include "staticUrl.h"

#include "tools.h"
#include "wx/sizer.h"


BEGIN_EVENT_TABLE(StaticUrl, wxWindow)
    EVT_ENTER_WINDOW(StaticUrl::OnMouseEnter)
    EVT_LEAVE_WINDOW(StaticUrl::OnMouseLeave)
    EVT_LEFT_UP     (StaticUrl::OnLeftClick)
END_EVENT_TABLE()


/**
 * Constructor
 * If text is an empty string, the url is used
**/
StaticUrl::StaticUrl(wxWindow* parent, const wxString& url, const wxString& text) : wxWindow(parent, wxID_ANY)
{
    wxBoxSizer *topLevelSizer;

    mUrl          = url;
    topLevelSizer = new wxBoxSizer(wxHORIZONTAL);
    mHandCursor   = new wxCursor(wxCURSOR_HAND);
    
    if(text.IsEmpty() == true)
        mLabel = new wxStaticText(this, wxID_ANY, wxT(" ") + mUrl);
    else
        mLabel = new wxStaticText(this, wxID_ANY, wxT(" ") + text);

    topLevelSizer->Add(mLabel);
    SetSizer(topLevelSizer);

    mLabel->SetForegroundColour(*wxBLUE);
    mLabel->SetEventHandler(this);          // We can't use PushEventHandler() because an EventHandler can't belong to two linked lists
}


/**
 * Destructor
**/
StaticUrl::~StaticUrl(void)
{
    delete mHandCursor;
}


/**
 * Called when the mouse enters the window
**/
void StaticUrl::OnMouseEnter(wxMouseEvent& event)
{
    mLabel->SetForegroundColour(*wxRED);
    mLabel->Refresh();
    
    SetCursor(*mHandCursor);
}


/**
 * Called when the mouse leaves the window
**/
void StaticUrl::OnMouseLeave(wxMouseEvent& event)
{
    mLabel->SetForegroundColour(*wxBLUE);
    mLabel->Refresh();
    
    SetCursor(wxNullCursor);
}


/**
 * Called on left clicks
**/
void StaticUrl::OnLeftClick(wxMouseEvent& event)
{
    Tools::OpenURLInBrowser(mUrl);
}
