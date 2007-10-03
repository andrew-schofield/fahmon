# Copyright (C) 2007 Andrew Schofield <the.uncle.fungus@gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
m4_include(m4/wxwin.m4)
AC_INIT([FahMon],
        [2.3.0],
        [Andrew Schofield the.uncle.fungus@gmail.com],
        [FahMon])
AC_PREFIX
# AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])

# LF_CONFIGURE_CC
# LF_CONFIGURE_CXX
# LF_HOST_TYPE
# LF_SET_WARNINGS
AC_PROG_CXX
#AC_PROG_CC
#AC_PROG_RANLIB

AM_OPTIONS_WXCONFIG
reqwx=2.6.3
AM_PATH_WXCONFIG($reqwx, wxWin=1, wxWin=0, --unicode)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.
 
		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi
 
CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

ALL_LINGUAS="en_GB fr_FR pt_PT ru_RU pl sv_SE"

AM_GNU_GETTEXT([external])

AC_CONFIG_FILES([
   Makefile
   README
   doc/Makefile
   m4/Makefile
   po/Makefile.in
   resources/Makefile
   src/Makefile
   src/include/Makefile
   src/images/Makefile
   src/images/icons/Makefile
   src/images/icons/16/Makefile
   src/images/icons/24/Makefile
   src/images/icons/32/Makefile
   src/images/icons/48/Makefile
])

AC_OUTPUT
