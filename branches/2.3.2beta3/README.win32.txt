INSTRUCTIONS FOR COMPILING ON MS WINDOWS
========================================

This guide details how to setup a build environment using the Microsoft Free
Tools and wxWidgets for Windows. Other build environment will require slightly
different setups, but most of the details should be fairly easy to adjust to
your own environment.

Setting up the build environment
================================

Download the following software (free)
* Microsoft Visual C++ 2005 Express Edition (MSVC)
* Visual Studio 8 Service Pack 1
* Microsoft Platform SDK for Windows Server 2003 R2
* wxWidgets >= 2.8.6

The total size of the required installations sources is >500MB

* Install MSVC and it's service pack.
* Install the Platform SDK.
* Install wxWidgets.

Installation of the required software will require >1GB of disk space.

The rest of the documentation assumes that you installed everything into the
default locations. So you may need to alter paths etc.

Configuring MSVC
================

* Open "c:\program files\microsoft visual studio
  8\vc\vcprojectdefaults\corewin_express.vsprops" in a text editor and modify
  the string that reads AdditionalDependencies="kernel32.lib" to
  AdditionalDependencies="kernel32.lib user32.lib gdi32.lib winspool.lib
  comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib"
* Open MSVC and click Tools->Options.
* Goto "Projects and Solutions->VCC++ Directories"
* Make sure "Show directories for:" says "Executable files"
* Add the following directory to the list:
  "c:\program files\microsoft platform sdk\bin"
* Change "Show directories for:" to "Include files"
* Add the following directories to the list:
  "c:\program files\microsoft platform sdk\include"
  "c:\wxwidgets-2.8.7\include"
* Change "Show directories for:" to "Library files"
* Add the following directories to the list:
  "c:\program files\microsoft platform sdk\lib"
  "c:\wxwidgets-2.8.7\lib\vc_lib"
* Close the options dialog (with OK).


Compiling wxWidgets
===================

* Navigate to c:\wxWidgets-2.8.7\build\msw and open wx.dsw.
* When prompted to open and convert the files, click "yes to all".
* Change the build type to "Unicode Release" by default this is set to "Debug".
  This can be altered by selecting the option from the drop-down list between
  the compile button (green arrow) and the platform type (which should say
  "Win32").
 * In the "Solution explorer" select all the wx subprojects. There are 18 of
   them.
 * Right click on the selected projects and select properties.
 * On the properties dialog and go to "Configuration Properties->C/C++->Code
   Generation."
 * Change the value of "Runtime Library" to "Multi-threaded (/MT)"
 * Close the properties dialog (with OK)
 * Go to the Build menu and hit "Build solution"
 * Make a snack, this will take a while
 * When the compiling has finished, you should see a line that reads
   "Build: 18 succeeded, 0 failed, 0 up-to-date, 0 skipped"
 * You have now sucessfully set up your FahMon build environment! Note: if you
   need to make any changes to wxWidgets, just open wx.sln (not wx.dsw) and
   recompile.

* Open fahmon.sln
* Change the build type to "Release"
* Go to the Build menu and hit "Build Solution"
* When the compiling has finished, you should see a line that reads:
  "Build: 1 succeeded, 0 failed, 0 up-to-date, 0 skipped"
* If this is the case, FahMon has compiled successfully and you can find your
  new fahmon.exe inside the "Release" folder
* To move and rename the translation files into the correct folders, run po_to_lang.bat