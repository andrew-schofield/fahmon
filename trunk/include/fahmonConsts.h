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
 
#ifndef _FAHMONCONSTS_H
#define _FAHMONCONSTS_H


// --- Limits
#define FMC_INTMAX  2147483647l


// --- Strings
#define FMC_APPNAME 	"FahMon"
#define FMC_VERSION 	"2.0.0"
#define FMC_PRODUCT 	FMC_APPNAME" "FMC_VERSION
#define FMC_UID         FMC_APPNAME"__LOCK__"


// --- URLs
#define FMC_URL_MYSTATS           "http://vspx27.stanford.edu/cgi-bin/main.py?qtype=userpage"
#define FMC_URL_JMOL              "http://www.jmol.org/fah/?project="
#define FMC_URL_FOLDING           "http://folding.stanford.edu"
#define FMC_URL_FCORG             "http://forum.folding-community.org/portal.php"
#define FMC_URL_SERVERS           "http://vspx27.stanford.edu/serverstat.html"
#define FMC_URL_PROJECTS          "http://vspx27.stanford.edu/psummary.html"      // Complete URL
#define FMC_URL_PROJECTS_SERVER   "vspx27.stanford.edu"                           // Used for updating projects
#define FMC_URL_PROJECTS_RESOURCE "psummaryC.html"                                // ---
#define FMC_URL_HELP              "/help/readme.html"


// --- Dates format
#define FMC_DATE_MAIN_FORMAT "%B %d, %H:%M"


// --- Directories
#define FMC_DIR_XYZ         "./xyz/"
#define FMC_DIR_CONFIG      "./config/"
#define FMC_DIR_IMAGES      "./images/"


// --- Files
#define FMC_FILE_PREFS      "prefs.dat"
#define FMC_FILE_CLIENTS    "clientstab.txt"
#define FMC_FILE_PROJECTS   "projects.dat"
#define FMC_FILE_BENCHMARKS "benchmarks.dat"

#define FMC_FILE_IMG_ABOUT              "about_icon.png"
#define FMC_FILE_IMG_DIALOG             "dialog_icon.png"
#define FMC_FILE_IMG_LIST_OK            "list_client_ok.png"
#define FMC_FILE_IMG_LIST_ERROR         "list_client_error.png"
#define FMC_FILE_IMG_LIST_INACTIVE      "list_client_inactive.png"
#define FMC_FILE_IMG_LIST_ARROW_UP      "list_up_arrow.png"
#define FMC_FILE_IMG_LIST_ARROW_DOWN    "list_down_arrow.png"


// --- Paths
#define FMC_PATH_PREFS      FMC_DIR_CONFIG""FMC_FILE_PREFS
#define FMC_PATH_CLIENTS    FMC_DIR_CONFIG""FMC_FILE_CLIENTS
#define FMC_PATH_PROJECTS   FMC_DIR_CONFIG""FMC_FILE_PROJECTS
#define FMC_PATH_BENCHMARKS FMC_DIR_CONFIG""FMC_FILE_BENCHMARKS

#define FMC_PATH_IMG_ABOUT              FMC_DIR_IMAGES""FMC_FILE_IMG_ABOUT
#define FMC_PATH_IMG_DIALOG             FMC_DIR_IMAGES""FMC_FILE_IMG_DIALOG
#define FMC_PATH_IMG_LIST_OK            FMC_DIR_IMAGES""FMC_FILE_IMG_LIST_OK
#define FMC_PATH_IMG_LIST_ERROR         FMC_DIR_IMAGES""FMC_FILE_IMG_LIST_ERROR
#define FMC_PATH_IMG_LIST_INACTIVE      FMC_DIR_IMAGES""FMC_FILE_IMG_LIST_INACTIVE
#define FMC_PATH_IMG_LIST_ARROW_UP      FMC_DIR_IMAGES""FMC_FILE_IMG_LIST_ARROW_UP
#define FMC_PATH_IMG_LIST_ARROW_DOWN    FMC_DIR_IMAGES""FMC_FILE_IMG_LIST_ARROW_DOWN


// --- GUI
#ifdef _FAHMON_LINUX_
#define FMC_GUI_BORDER 2     // GTK already have some borders
#else
#define FMC_GUI_BORDER 5     // Windows does not have default borders, so we use ours
#endif

#define FMC_GUI_SPACING_LOW          5       // (Low) Spacing used bewteen controls
#define FMC_GUI_SPACING_HIGH         10      // (High) Spacing used bewteen controls
#define FMC_GUI_TEXTCTRL_MIN_LENGTH  200     // Min length of wxTextCtrl
#define FMC_GUI_LOG_HEIGHT           350


#endif /* _FAHMONCONSTS_H */
