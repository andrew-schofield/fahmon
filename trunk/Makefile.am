# Copyright (C) 2007-8 Andrew Schofield <andrew_s@fahmon.net>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = config.rpath  \
	reconf \
	configure \
	fahmon.desktop \
	m4/Makefile.in \
	src/gettext.h \
	REQUIRES \
	fahmon.sln \
	fahmon.vcproj \
	FahMon.nsi \
	README.win32.txt \
	po_to_lang.bat \
	doxconf \
	TEMPLATE_SYNTAX \
	gen_msw.bat \
	fahmon.spec \
	FahMon-Info.plist \
	mac-lang
SUBDIRS = m4 src doc po src/include src/images resources templates FahMon.xcodeproj mac

appsdir = $(datadir)/applications
apps_in_files = fahmon.desktop
apps_DATA = $(apps_in_files)

ACLOCAL_AMFLAGS = -I m4

check-gettext:
	@if test x$(USE_NLS) != "xyes" ; then echo "Missing gettext. Rerun configure and check for" \
	"'checking whether to use NLS... yes'!" ; exit 1 ; fi

update-po: check-gettext
	@find $(srcdir)/src/ -name "*.cpp" -print | sort > $(srcdir)/po/POTFILES.in.2 ; \
	if diff $(srcdir)/po/POTFILES.in $(srcdir)/po/POTFILES.in.2 >/dev/null 2>&1 ; then \
		rm -f $(srcdir)/po/POTFILES.in.2 ; \
	else \
		mv $(srcdir)/po/POTFILES.in.2 $(srcdir)/po/POTFILES.in ; \
	fi
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po

update-gmo: check-gettext
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo: check-gettext
	touch po/*.po
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo-%: check-gettext
	@language=`echo $@ | sed s/force-update-gmo-//` ; \
	if test ! -f po/$$language.po ; then echo "file po/$$language.po does not exist" ; exit 1 ; fi ; \
	touch po/$$language.po ; \
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

.PHONY: check-gettext update-po update-gmo force-update-gmo
