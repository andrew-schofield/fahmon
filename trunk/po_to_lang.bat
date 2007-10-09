@echo off
del langs
echo Checking for lang directory:
if exist lang (
echo Exists
) else (
echo Doesn't exist, creating
mkdir lang
)
echo Checking for translations:
echo.
For %%f in (po\*.po) do (
echo Found translation for %%~nf
echo "%%~nf""" >> langs
echo Checking for existing lang\%%~nf folder:
if exist lang\%%~nf (
echo Exists
) else (
echo Doesn't exist, creating
mkdir lang\%%~nf
)
echo Copying translations...
copy /y po\%%~nf.po .\lang\%%~nf\%%~nf.po
copy /y po\%%~nf.gmo .\lang\%%~nf\fahmon.mo
echo Checking for translation in NSI script
find "%%~nf" < fahmon.nsi > lang_out
for /f %%i in ('find /v /c "" ^< lang_out') do set /a lines=%%i
if %lines% NEQ 3 (
echo Warning: %%~nf doesn't appear to be in the NSI script
echo This language won't be included in the installer until you add it
pause
) else (
echo Entry found.
)
del lang_out
echo ============
)
echo Copying translation template
copy /y .\po\fahmon.pot .\lang\fahmon.pot
move .\lang .\Release\
pause
