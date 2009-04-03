# Copyright (C) 2007-9 Andrew Schofield <andrew_s@fahmon.net>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
m4_include(m4/wxwin.m4)
m4_include(m4/libcurl.m4)
m4_define([wxcurl_lt_current], [1])

AC_INIT([FahMon],
        [2.3.99],
        [Andrew Schofield andrew_s@fahmon.net],
        [FahMon])
AC_PREFIX
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])

WXCURL_LT_VERSION_INFO="wxcurl_lt_current"
AC_SUBST(WXCURL_LT_VERSION_INFO)

AC_PROG_CXX
AC_PROG_LIBTOOL

LIBCURL_CHECK_CONFIG([yes], [7.15.0], [curlOk=1], [curlOk=0])
if [[ $curlOk = 0 ]]; then
    AC_MSG_ERROR([
    Could not find a valid libCURL installation on your system.

    If you do not have libCURL installed or you have an old version
    then please install the latest version from
      http://curl.haxx.se/libcurl/

    If you have a valid libCURL installed, then please check the config.log
    file for more details about the failure of this check.
    ])
fi

AM_OPTIONS_WXCONFIG
reqwx=2.8.0
AC_MSG_NOTICE([ANSI wxWidgets])
AM_PATH_WXCONFIG($reqwx, wxWin=1, wxWin=0, adv core qa net)
AC_MSG_NOTICE([Unicode wxWidgets])
AM_PATH_WXCONFIG($reqwx, wxWinU=1, wxWinU=0, --unicode adv core qa net)
if test "$wxWin" != 1; then
	if test "$wxWinU" != 1; then
		AC_MSG_ERROR([
			wxWidgets must be installed on your system.

			Please check that wx-config is in path, the directory
			where wxWidgets libraries are installed (returned by
			'wx-config --libs' or 'wx-config --static --libs' command)
			is in LD_LIBRARY_PATH or equivalent variable and
			wxWidgets version is $reqwx or above.
			])
	fi
fi
if test "$wxWinU" != 1; then
	AC_MSG_WARN([
		Unicode build of wxWidgets not found.

		While FahMon will still compile with the ansi version
		of wxWidgets, you will lose the ability to use any unicode
		character sets for translations. If this is not what you
		want, install the unicode build of wxWidgets and run
		configure again.
		])
fi

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS $LIBCURL_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY $LIBCURL_CPPFLAGS"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY $LIBCURL_CPPFLAGS"
LIBS="$LIBS $WX_LIBS $LIBCURL"

ALL_LINGUAS="en_GB fr_FR pt_PT ru_RU pl_PL sv_SE cs_CZ nl_NL pt_BR es_ES de_DE it_IT nb_NB"

AM_GNU_GETTEXT([external])

AC_CONFIG_FILES([
   Makefile
   README
   FahMon.nsi
   fahmon.spec
   doxconf
   FahMon-Info.plist
   doc/Makefile
   m4/Makefile
   po/Makefile.in
   resources/Makefile
   wxcurl/Makefile
   wxcurl/include/Makefile
   wxcurl/include/wx/Makefile
   wxcurl/include/wx/curl/Makefile
   wxcurl/src/Makefile
   wxcurl/win32_libcurl/Makefile
   wxcurl/win32_libcurl/include/Makefile
   wxcurl/win32_libcurl/include/curl/Makefile
   wxcurl/win32_libcurl/lib/Makefile
   src/Makefile
   src/include/Makefile
   src/include/fahmonConsts.h
   src/images/Makefile
   src/images/icons/Makefile
   src/images/icons/16/Makefile
   src/images/icons/24/Makefile
   src/images/icons/32/Makefile
   src/images/icons/48/Makefile
   src/images/icons/64/Makefile
   src/images/icons/128/Makefile
   src/images/icons/256/Makefile
   templates/Makefile
   FahMon.xcodeproj/Makefile
   mac/FahMon-Info.plist
   mac/Makefile
])

AC_OUTPUT
