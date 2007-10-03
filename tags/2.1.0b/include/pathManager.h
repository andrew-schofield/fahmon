/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _PATHMANAGER_H
#define _PATHMANAGER_H

#include "wx/string.h"


/**
 * This class provides the path to the main directories used by FahMon
**/
class PathManager
{
protected:
    static PathManager* mInstance;

    wxString mXYZPath;
    wxString mImgPath;
    wxString mCfgPath;

    // Constructor/Destructor
     PathManager(void);
    ~PathManager(void) {};

    // Singleton pattern
    static const PathManager* GetInstance(void);


public:
    static wxString GetXYZPath(void) {return GetInstance()->mXYZPath;}
    static wxString GetImgPath(void) {return GetInstance()->mImgPath;}
    static wxString GetCfgPath(void) {return GetInstance()->mCfgPath;}
};


#endif /* _PATHMANAGER_H */
