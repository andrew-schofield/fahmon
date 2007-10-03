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

#ifndef _PROJECTHELPERTHREAD_H
#define _PROJECTHELPERTHREAD_H


#include "wx/thread.h"


/**
* When instanciated, this class:
*  - Asks the ProjectsManager to update its database
*  - Sends an EVT_PROJECTS_DATABASE_UPDATED event to the main dialog when done, if no error occurred
*
* This class should only be used by the ProjectsManager
**/
class ProjectHelperThread : public wxThread
{
protected:
	bool mIsForced;
	bool mIsInSilentMode;

	void* Entry(void);


public:
	ProjectHelperThread(bool isForced, bool isInSilentMode);
};


#endif /* _PROJECTHELPERTHREAD_H */
