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
 
#ifndef _HTMLPARSER_H
#define _HTMLPARSER_H


#include "wx/wx.h"


// Well, this is a really 'tiny' HTML parser, it does only recognize the tags I need
class HTMLParser
{
public:
    typedef enum _HTML_TOKEN
    {
        TOKEN_TD,           // A 'td' tag
        TOKEN_TR,           // A 'tr' tag
        TOKEN_TEXT,         // No tag, just a piece of text
        TOKEN_UNKNOWN       // An unknown tag (there are many ones with this parser :-)
    } HTMLToken;


protected:
    bool      mIsStartingTag;
    wxString  mParsedContent;
    wxString  mCurrentText;
    HTMLToken mCurrentToken;

    HTMLToken TextToToken(const wxString& text);


public:
    HTMLParser(void);

    void ParseString(const wxString& string);
    bool ParseFile(const wxString& fileName);
    bool NextToken(void);
    void NextToken(wxUint32 nbTokensToSkip);

    // Getters
    bool      Eof(void)            const {return mParsedContent.IsEmpty();}
    bool      IsStartingTag(void)  const {return mIsStartingTag;}
    wxString  GetCurrentText(void) const {return mCurrentText;}
    HTMLToken GetToken(void)       const {return mCurrentToken;}
};


#endif /* _HTMLPARSER_H */
