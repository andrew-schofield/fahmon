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

#ifndef _FAHMON_H
#define _FAHMON_H

#include <wx/defs.h>
#include "fahmonTypes.h"
#include "fahmonConsts.h"

#include "wx/debugrpt.h"

#ifdef _FAHMON_WIN32_
#include "resource.h"
#endif

class MyDebugReport : public wxDebugReportUpload
{
	public:
		MyDebugReport() : wxDebugReportUpload(_T("http://fahmon.net"),_T("report:file"),_T("action"))
		{
		}

	protected:
		virtual bool OnServerReply(const wxArrayString& reply)
		{
			if ( reply.IsEmpty() )
			{
				//wxPuts(_T("Didn't receive the expected server reply."));
				return false;
			}

			wxString s(_T("Server replied:\n"));

			const size_t count = reply.GetCount();
			for ( size_t n = 0; n < count; n++ )
			{
				s << _T('\t') << reply[n] << _T('\n');
			}

			//wxPuts(wxString::Format(_T("%s"), s.c_str()));

			return true;
		}
};


#endif /* _FAHMON_H */
