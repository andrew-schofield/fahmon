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
 * \file staticBoldedText.cpp
 * Creates a bold text object.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "staticBoldedText.h"

#include "wx/font.h"


StaticBoldedText::StaticBoldedText(wxWindow* parent, wxWindowID id, wxString const &label, wxPoint const &pos, wxSize const &size, long style, wxString const &name) : wxStaticText(parent, id, label, pos, size, style, name)
{
	wxFont font;

	font = GetFont();
	font.SetWeight(wxFONTWEIGHT_BOLD);
	SetFont(font);
}
