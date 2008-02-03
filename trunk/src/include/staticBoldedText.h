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
 * \file staticBoldedText
 * Creates a bold text object.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _STATICBOLDEDTEXT_H
#define _STATICBOLDEDTEXT_H


#include "wx/window.h"
#include "wx/stattext.h"

/**
 * Creates a Bold Text object.
 **/
class StaticBoldedText : public wxStaticText
{
	public:
	/**
	 * Constructor.
	 * Contructs the object.
	 * @param parent The parent Window
	 * @param id if of the Window.
	 * @param label Text to be displayed.
	 * @param pos Position of object
	 * @param size Size of object
	 * @param style Display style
	 * @param name Objectname
	 **/
	StaticBoldedText(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("staticText"));
};


#endif /* _STATICBOLDEDTEXT_H */
