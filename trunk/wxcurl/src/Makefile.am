# Copyright (C) 2007-9 Andrew Schofield <andrew_s@fahmon.net>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

lib_LTLIBRARIES = libwxcurl.la

libwxcurl_la_SOURCES = ftpparse.h base.cpp dav.cpp davtool.cpp dialog.cpp ftp.cpp ftpparse.cpp ftptool.cpp http.cpp panel.cpp telnet.cpp thread.cpp utils.cpp
libwxcurl_la_LDFLAGS = -version-info $(WXCURL_LT_VERSION_INFO)
INCLUDES = -I$(top_srcdir)/wxcurl/include
AM_CXXFLAGS = -Wall -Wno-strict-aliasing
