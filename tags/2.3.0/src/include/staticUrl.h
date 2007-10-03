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
 
#ifndef _STATICURL_H
#define _STATICURL_H


#include "wx/window.h"
#include "wx/stattext.h"
#include "wx/cursor.h"


/**
 * This class is like wxStaticText, but represents a clickable URL
 * It would have been easier to inherit from wxStaticText, but it doesn't seem to generate MouseEnter and
 * MouseLeave events (at least with wxGTK)
**/
class StaticUrl : public wxWindow
{
protected:
    bool          mEnabled;
    wxString      mURL;
    wxCursor     *mHandCursor;
    wxStaticText *mLabel;

    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnLeftClick(wxMouseEvent& event);


public:
     StaticUrl(wxWindow* parent, const wxString& url = wxT(""), const wxString& label = wxT(""));
    ~StaticUrl(void);

    void SetURL(const wxString& url);
    void SetLabel(const wxString& label);

    void Disable(void) {mEnabled = false;}
    void Enable(void)  {mEnabled = true;}

    void ClearAndDisable(void) {SetLabel(wxT("")); Disable();}


private:
    DECLARE_EVENT_TABLE()
};


#endif /* _STATICURL_H */
