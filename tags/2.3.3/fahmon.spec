# Older RPM doesn't define these by default
%{!?perl_vendorlib: %define perl_vendorlib %(eval "`%{__perl} -V:installvendorlib`"; echo $installvendorlib)}
%{!?perl_vendorarch: %define perl_vendorarch %(eval "`%{__perl} -V:installvendorarch`"; echo $installvendorarch)}
%{!?perl_archlib: %define perl_archlib %(eval "`%{__perl} -V:installarchlib`"; echo $installarchlib)}

%define fahmonver 2.3.3

Summary:    A wxGTK based monitor for Folding@home clients
Name:       FahMon
Version:    %fahmonver
Release:    1
License:    GPL
Group:      System/Monitoring
URL:        http://fahmon.net/
Source:     %{name}-%{version}.tar.bz2
Packager:   Andrew Schofield <andrew_s@fahmon.net>
BuildRoot:  %{_tmppath}/%{name}-%{version}-root

# Generic build requirements
BuildRequires: wxGTK2.8-devel

Requires: wxGTK2.8

%description
FahMon is an open-source tool (GPL license) that allows you to quickly
check the progress of your Folding@Home client (or clients if you have
multiple ones), avoiding you having to open different files and/or to
go to the Internet (for example to know how much your current work
unit is worth). Other monitoring tools exist (such as Electron
Microscope or FahLogStats), so if you don't like FahMon, have a look
at them!

FahMon is entirely coded in C++ and uses the wxWidgets library, which
allows FahMon to exist both for Linux and Windows. It is designed to
be really easy to use, and you should thus not encounter any major
problem.

%prep
%setup -q -n %{name}-%{version}

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{_prefix} \
                                    --bindir=%{_bindir} \
                                    --datadir=%{_datadir} \
                                    --includedir=%{_includedir} \
                                    --libdir=%{_libdir} \
                                    --mandir=%{_mandir} \
                                    --sysconfdir=%{_sysconfdir}

make

%install
rm -rf %{buildroot}
make DESTDIR=$RPM_BUILD_ROOT install

# Delete files that we don't want to put in any of the RPMs
# Don't need to do this as we don't leave any cruft hanging around anyway

%clean
rm -rf %{buildroot}

%pre

%post

%files
%defattr(-, root, root)

%doc AUTHORS
%doc COPYING
%doc ChangeLog
%doc NEWS
%doc README
%doc REQUIRES
%doc THANKS
%doc doc/help.pdf

%{_bindir}/fahmon
%{_datadir}/pixmaps/fahmon
%{_datadir}/icons/hicolor/*/apps/fahmon*
%{_datadir}/applications/*
%{_datadir}/locale/*/LC_MESSAGES/fahmon.mo
%{_datadir}/fahmon/templates/*

%changelog
* Thu Jan 17 2008 Andrew Schofield <andrew_s@fahmon.net>
- Added GNU build macros to auto insert version number.

* Tue Oct 30 2007 Andrew Schofield <andrew_s@fahmon.net>
- Updated to 2.3.1

* Mon Sep 10 2007 Andrew Schofield <andrew_s@fahmon.net>
- Updated to 2.3.0

* Wed Aug 01 2007 Andrew Schofield <andrew_s@fahmon.net>
- Updated description

* Thu Jul 12 2007 Andrew Schofield <andrew_s@fahmon.net>
- New spec file for PCLinuxOS 2007, ripped apart from the pidgin 2.0.0beta7 spec file (http://pidgin.im)