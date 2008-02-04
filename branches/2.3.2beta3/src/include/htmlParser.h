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
 * \file htmlParser.h
 * Create a class for analyzing HTML.
 * Very primitive parsing.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _HTMLPARSER_H
#define _HTMLPARSER_H


#include "wx/wx.h"


/**
 * HTMLParser class.
 * A really 'tiny' HTML parser, it only recognizes the tags needed.
 **/
class HTMLParser
{
public:
	/**
	 * HTML Token definition.
	 **/
	typedef enum _HTML_TOKEN
	{
		TOKEN_TD, /**< A 'td' tag */
		TOKEN_TR, /**< A 'tr' tag */
		TOKEN_TEXT, /**< No tag, just a piece of text */
		TOKEN_UNKNOWN /**< An unknown tag (there are many ones with this parser :-) */
	} HTMLToken;


protected:
	bool      mIsStartingTag; /**< Is the current tag the first? */
	wxString  mParsedContent; /**< String containing the parsed html */
	wxString  mCurrentText; /**< Current text being parsed */
	HTMLToken mCurrentToken; /**< Current token that has been parsed */

	HTMLToken TextToToken(const wxString& text); /**< Convert text to HTMLToken  */


public:
	/**
	 * Constructor.
	 * Contructs the parser object.
	 * ParseFile() or ParseString() must be called.
	 **/
	HTMLParser(void);

	/**
	 * (re)initialize the parser to parse the given string.
	 * @param string The string to parse.
	 **/
	void ParseString(const wxString& string);

	/**
	 * Load a file and (re)initialize the parser.
	 * @param fileName The file to parse.
	 **/
	bool ParseFile(const wxString& fileName);

	/**
	 * @return true if a new token could be found, or false if the end of the file has been reached
	 **/
	bool NextToken(void);

	/**
	 * Skip the given number of tokens.
	 * @param nbTokensToSkip The number of tokens to skip.
	 **/
	void NextToken(wxUint32 nbTokensToSkip);

	// Getters
	/**
	 * Checks if end of file has been reached.
	 * @return Are we at eof?
	 **/
	bool      Eof(void)            const {return mParsedContent.IsEmpty();}

	/**
	 * Checks if current tag is first.
	 * @return Is tag first?
	 **/
	bool      IsStartingTag(void)  const {return mIsStartingTag;}

	/**
	 * Gets current text being parsed.
	 * @return value of current text
	 **/
	wxString  GetCurrentText(void) const {return mCurrentText;}

	/**
	 * Gets current HTMLToken.
	 * @return Current HTMLtoken object.
	 **/
	HTMLToken GetToken(void)       const {return mCurrentToken;}
};


#endif /* _HTMLPARSER_H */
