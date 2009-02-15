# Copyright (C) 2007-9 Andrew Schofield <andrew_s@fahmon.net>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS = fahmon
fahmon_SOURCES = aboutDialog.cpp base64Codec.cpp benchmark.cpp benchmarksDialog.cpp benchmarksManager.cpp client.cpp clientDialog.cpp clientHelperThread.cpp clientsManager.cpp core.cpp dataInputStream.cpp dataOutputStream.cpp eta.cpp fahlogAnalyzer.cpp firstTimeDialog.cpp ftpDownloader.cpp htmlParser.cpp httpDownloader.cpp listViewClients.cpp main.cpp mainDialog.cpp messagesFrame.cpp messagesManager.cpp multiProtocolFile.cpp pathManager.cpp preference.cpp preferencesDialog.cpp preferencesManager.cpp progressManager.cpp project.cpp  projectHelperThread.cpp projectsManager.cpp queue.cpp staticBoldedText.cpp tools.cpp trayManager.cpp workUnitFrame.cpp webMonitor.cpp
fahmon_LDADD =$(top_builddir)/wxcurl/src/libwxcurl.la
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/wxcurl/include
AM_CXXFLAGS = -Wall -Wno-strict-aliasing
AM_CPPFLAGS = \
	-DDATADIR=\"$(datadir)\"

