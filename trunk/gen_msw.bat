@echo off
if not exist .\Release (
mkdir .\Release
)
call po_to_lang.bat
copy /y .\wxcurl\lib\libcurl.dll .\Release
if not exist .\Release\images (
mkdir .\Release\images
)
copy /y .\src\images\list_client_asynch.png .\Release\images
copy /y .\src\images\list_client_inaccessible.png .\Release\images
copy /y .\src\images\list_client_inactive.png .\Release\images
copy /y .\src\images\list_client_ok.png .\Release\images
copy /y .\src\images\list_client_stopped.png .\Release\images
copy /y .\src\images\list_client_paused.png .\Release\images
copy /y .\src\images\list_down_arrow.png .\Release\images
copy /y .\src\images\list_up_arrow.png .\Release\images
copy /y .\src\images\main_icon.png .\Release\images
copy /y .\src\images\dialog_icon.png .\Release\images

if not exist .\Release\docs (
mkdir .\Release\docs
)
copy /y .\AUTHORS .\Release\docs\AUTHORS.txt
copy /y .\ChangeLog .\Release\docs\ChangeLog.txt
copy /y .\COPYING .\Release\docs\COPYING.txt
copy /y .\doc\help.pdf .\Release\docs\help.pdf
copy /y .\NEWS .\Release\docs\NEWS.txt
copy /y .\README .\Release\docs\README.txt
copy /y .\TEMPLATE_SYNTAX .\Release\docs\TEMPLATE_SYNTAX.txt

if not exist .\Release\templates (
mkdir .\Release\templates
)
copy /y .\templates\fancy_template.htm .\Release\templates
copy /y .\templates\simple_template.htm .\Release\templates
copy /y .\templates\simple_template.txt .\Release\templates

copy /y .\FahMon.nsi .\Release\
if exist .\Release\*.obj (
del /F /Q .\Release\*.obj
)
if exist .\Release\*.htm (
del /F /Q .\Release\*.htm
)
if exist .\Release\*.manifest (
del /F /Q .\Release\*.manifest
)
if exist .\Release\*.res (
del /F /Q .\Release\*.res
)
if exist .\Release\*.dep (
del /F /Q .\Release\*.dep
)
if exist .\Release\*.log (
del /F /Q .\Release\*.log
)
if exist .\Release\*.idb (
del /F /Q .\Release\*.idb
)
