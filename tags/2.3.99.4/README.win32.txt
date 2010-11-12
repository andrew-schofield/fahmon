INSTRUCTIONS FOR COMPILING ON MS WINDOWS
========================================

This guide details how to setup a build environment using the Microsoft Free
Tools and wxWidgets for Windows. Other build environment will require slightly
different setups, but most of the details should be fairly easy to adjust to
your own environment.

Setting up the build environment
================================

Download the following software (free)
* Microsoft Visual C++ 2010 Express Edition (MSVC)
* wxWidgets >= 2.8.7

The total size of the required installations sources is >500MB

* Install MSVC.
* Install wxWidgets.

Installation of the required software will require >1GB of disk space.

The rest of the documentation assumes that you installed everything into the
default locations. So you may need to alter paths etc.

Aggressive wxWidgets stripping
==============================

If you wish to build an aggressively stripped minimal version of the wxWidgets
library capable of linking against FahMon and reducing the final executable size
by several hundred kb, then copy the "resources\setup_aggressive.h" file to
C:\wxMSW-2.8.x\include\wx\msw\setup.h before continuing. This is not guaranteed
to work from SVN as this file is not updated regularly against the FahMon
sources.

Compiling wxWidgets
===================

* Navigate to c:\wxwidgets-2.8.x\build\msw and open wx.dsw.
* When prompted to open and convert the files, click "yes to all".
* Change the build type to "Unicode Release" by default this is set to "Debug".
  This can be altered by selecting the option from the drop-down list between
  the compile button (green arrow) and the platform type (which should say
  "Win32").
 * In the "Solution explorer" select all the wx subprojects. There are 20 of
   them.
 * Right click on the selected projects and select properties.
 * On the properties dialog and go to "Configuration Properties->C/C++->Code
   Generation."
 * Change the value of "Runtime Library" to "Multi-threaded (/MT)"
 * Close the properties dialog (with OK)
 * Go to the Build menu and hit "Build solution"
 * Make a snack, this will take a while
 * When the compiling has finished, you should see a line that reads
   "Build: 20 succeeded, 0 failed, 0 up-to-date, 0 skipped"
 * You have now sucessfully set up your FahMon build environment! Note: if you
   need to make any changes to wxWidgets, just open wx.sln (not wx.dsw) and
   recompile.

Compiling libcurl
=================

* Open wxcurl\win32_libcurl\curl.sln
* Make sure the build type is set to "DLL Release"
* Go to the build menu and hit "Build Solution"


Configuring FahMon
================

* If you having installed wxwidgets 2.8.11 to c:\wxwidgets-2.8.11 you do not
  need to configure FahMon further.
* Open fahmon.sln
* Change build type to "Release"
* For fahmon and wxcurl, Right click and go to properties.
* Goto "Configuration properties->VCC++ Directories"
* Select "Include Directories"
* Add the following directories to the list:
  "c:\wxMSW-2.8.x\include"
  "c:\wxMSW-2.8.x\include\msvc"
* Select "Library Directories"
* Add the following directories to the list:
  "c:\wxSW-2.8.x\lib\vc_lib"
* Close the options dialog (with OK).

Compiling FahMon
================

* Open fahmon.sln
* Change the build type to "Release"
* Go to the Build menu and hit "Build Solution"
* When the compiling has finished, you should see a line that reads:
  "Build: 1 succeeded, 0 failed, 0 up-to-date, 0 skipped"
* If this is the case, FahMon has compiled successfully and you can find your
  new fahmon.exe inside the "Release" folder
* To clean up the "Release" folder and organise the translations run
  "gen_msw.bat"
