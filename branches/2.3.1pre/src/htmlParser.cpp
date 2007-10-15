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
#include "htmlParser.h"

#include "tools.h"


/**
* Constructor
* ParseFile() or ParseString() must be called
**/
HTMLParser::HTMLParser(void)
{
	mIsStartingTag = false;
	mParsedContent = wxT("");
	mCurrentText   = wxT("");
	mCurrentToken  = TOKEN_UNKNOWN;
}


/**
* Load a file and (re)initialize the parser
**/
bool HTMLParser::ParseFile(const wxString& fileName)
{
	mIsStartingTag = false;
	mParsedContent = wxT("");
	mCurrentText   = wxT("");
	mCurrentToken  = TOKEN_UNKNOWN;

	if(fileName.IsEmpty() == false && Tools::LoadFile(fileName, mParsedContent) == false)
	{
		mParsedContent = wxT("");
		return false;
	}

	return true;
}


/**
* (re)initialize the parser to parse the given string
**/
void HTMLParser::ParseString(const wxString& string)
{
	mIsStartingTag = false;
	mParsedContent = string;
	mCurrentText   = wxT("");
	mCurrentToken  = TOKEN_UNKNOWN;
}


/**
* Return true if a new token could be found, or false if the end of the file has been reached
**/
bool HTMLParser::NextToken(void)
{
	wxInt32  nextTagPos;
	wxInt32  endTagPos;
	wxString currentTag;

	// No more tokens if we reached the end of the file
	if(Eof() == true)
		return false;

	// We can be either on a HTML tag, or on a piece of text
	if(mParsedContent[0] == '<')
	{
		mIsStartingTag = true;
		endTagPos      = mParsedContent.Find('>');

		// We reached the end of the file
		if(endTagPos == -1)
		{
			// Ensure that there is some characters after '<'
			if(mParsedContent.Len() > 1)
				currentTag = mParsedContent.Mid(1);
			else
				currentTag = wxT("");

			mParsedContent = wxT("");
		}
		else
		{
			currentTag     = mParsedContent.Mid(1, endTagPos-1);
			mParsedContent = mParsedContent.Mid(endTagPos+1);
		}

		mCurrentToken = TextToToken(currentTag);
	}
	else
	{
		mCurrentToken = TOKEN_TEXT;
		nextTagPos    = mParsedContent.Find('<');

		// We reached the end of the file
		if(nextTagPos == -1)
		{
			mCurrentText   = mParsedContent;
			mParsedContent = wxT("");
		}
		else
		{
			mCurrentText   = mParsedContent.Mid(0, nextTagPos);
			mParsedContent = mParsedContent.Mid(nextTagPos);
		}
	}

	return true;
}


/**
* Convert a text into its corresponding token
**/
HTMLParser::HTMLToken HTMLParser::TextToToken(const wxString& text)
{
	wxInt32  argumentPos;
	wxString toConvert;

	// Remove any leading slash, which indicates that this is an ending tag and not a starting one
	if(text[0] == '/')
	{
		toConvert      = text.Mid(1);
		mIsStartingTag = false;
	}
	else
		toConvert = text;

	// Perhaps are there some arguments with the tag, we need to ignore them (eg. <tr bgcolor="#ffffff">)
	argumentPos = toConvert.Find(' ');
	if(argumentPos != -1)
		toConvert = toConvert.Mid(0, argumentPos);

	// Simple comparisons...
	if(toConvert.CmpNoCase(wxT("td")) == 0)
		return TOKEN_TD;
	else if(toConvert.CmpNoCase(wxT("tr")) == 0)
		return TOKEN_TR;

	return TOKEN_UNKNOWN;
}


/**
* Skip the given number of tokens
**/
void HTMLParser::NextToken(wxUint32 nbTokensToSkip)
{
	wxUint32 i;

	for(i=0; i<nbTokensToSkip; ++i)
		NextToken();
}
