  !include "MUI.nsh"

  SetCompressor /SOLID lzma

  Name "FahMon"
  OutFile "FahMon-2.3.99.4-Installer.exe"

  InstallDir "$PROGRAMFILES\FahMon"

  InstallDirRegKey HKCU "Software\FahMon" ""

  RequestExecutionLevel admin

  Var MUI_TEMP
  Var STARTMENU_FOLDER

  !define MUI_ABORTWARNING

  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\FahMon"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

  !insertmacro MUI_LANGUAGE "English"

Section "!FahMon" SecFahMon

  SectionIn 1 RO

  SetOutPath "$INSTDIR"

  File "fahmon.exe"
  File "libcurl.dll"
  SetOutPath "$INSTDIR\images"
  File "images\list_client_asynch.png"
  File "images\list_client_inaccessible.png"
  File "images\list_client_inactive.png"
  File "images\list_client_ok.png"
  File "images\list_client_stopped.png"
  File "images\list_client_paused.png"
  File "images\list_down_arrow.png"
  File "images\list_up_arrow.png"
  File "images\main_icon.png"
  File "images\dialog_icon.png"
  SetOutPath "$INSTDIR\lang"
  File "lang\fahmon.pot"
  SetOutPath "$INSTDIR\docs"
  File "docs\AUTHORS.txt"
  File "docs\ChangeLog.txt"
  File "docs\COPYING.txt"
  File "docs\help.pdf"
  File "docs\NEWS.txt"
  File "docs\README.txt"

  SetOutPath "$INSTDIR"

  WriteRegStr HKCU "Software\FahMon" "" $INSTDIR

  WriteUninstaller "$INSTDIR\Uninstall.exe"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FahMon" \
                 "DisplayName" "FahMon - Folding@home client monitoring software"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FahMon" \
                 "UninstallString" "$INSTDIR\uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER\FahMon.lnk" "$INSTDIR\fahmon.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Help.lnk" "$INSTDIR\docs\help.pdf"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\README.lnk" "$INSTDIR\docs\README.txt"

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "Web Templates" secWebTemplates

  SetOutPath "$INSTDIR\templates"
  File "templates\fancy_template.htm"
  File "templates\simple_template.htm"
  File "templates\simple_template.txt"

SectionEnd

SectionGroup "Languages" secLang
Section /o "British English" secLangenGB

  SetOutPath "$INSTDIR\lang\en_GB"
  File "lang\en_GB\en_GB.po"
  File "lang\en_GB\fahmon.mo"

SectionEnd
Section /o "Czech" secLangcsCZ

  SetOutPath "$INSTDIR\lang\cs_CZ"
  File "lang\cs_CZ\cs_CZ.po"
  File "lang\cs_CZ\fahmon.mo"

SectionEnd
Section /o "Dutch" secLangnlNL

  SetOutPath "$INSTDIR\lang\nl_NL"
  File "lang\nl_NL\nl_NL.po"
  File "lang\nl_NL\fahmon.mo"

SectionEnd
Section /o "French" secLangfrFR

  SetOutPath "$INSTDIR\lang\fr_FR"
  File "lang\fr_FR\fr_FR.po"
  File "lang\fr_FR\fahmon.mo"

SectionEnd
Section /o "German" secLangdeDE

  SetOutPath "$INSTDIR\lang\de_DE"
  File "lang\de_DE\de_DE.po"
  File "lang\de_DE\fahmon.mo"

SectionEnd
Section /o "Italian" secLangitIT

  SetOutPath "$INSTDIR\lang\it_IT"
  File "lang\it_IT\it_IT.po"
  File "lang\it_IT\fahmon.mo"

SectionEnd
Section /o "Polish" secLangplPL

  SetOutPath "$INSTDIR\lang\pl_PL"
  File "lang\pl_PL\pl_PL.po"
  File "lang\pl_PL\fahmon.mo"

SectionEnd
Section /o "Portuguese (Brazil)" secLangptBR

  SetOutPath "$INSTDIR\lang\pt_BR"
  File "lang\pt_BR\pt_BR.po"
  File "lang\pt_BR\fahmon.mo"

SectionEnd
Section /o "Portuguese (Portugal)" secLangptPT

  SetOutPath "$INSTDIR\lang\pt_PT"
  File "lang\pt_PT\pt_PT.po"
  File "lang\pt_PT\fahmon.mo"

SectionEnd
Section /o "Russian" secLangruRU

  SetOutPath "$INSTDIR\lang\ru_RU"
  File "lang\ru_RU\ru_RU.po"
  File "lang\ru_RU\fahmon.mo"

SectionEnd
Section /o "Simplified Chinese" secLangzhCN

  SetOutPath "$INSTDIR\lang\zh_CN"
  File "lang\zh_CN\zh_CN.po"
  File "lang\zh_CN\fahmon.mo"

SectionEnd
Section /o "Spanish" secLangesES

  SetOutPath "$INSTDIR\lang\es_ES"
  File "lang\es_ES\es_ES.po"
  File "lang\es_ES\fahmon.mo"

SectionEnd
Section /o "Swedish" secLangsvSE

  SetOutPath "$INSTDIR\lang\sv_SE"
  File "lang\sv_SE\sv_SE.po"
  File "lang\sv_SE\fahmon.mo"

SectionEnd
Section /o "Slovak" secLangskSK

  SetOutPath "$INSTDIR\lang\sk_SK"
  File "lang\sk_SK\sk_SK.po"
  File "lang\sk_SK\fahmon.mo"

SectionEnd
SectionGroupEnd

  LangString DESC_SecFahMon ${LANG_ENGLISH} "Install FahMon"
  LangString DESC_SecWebTemplates ${LANG_ENGLISH} "Install web output templates"
  LangString DESC_SecLang ${LANG_ENGLISH} "Install optional language components"
  LangString DESC_SecLangenGB ${LANG_ENGLISH} "Install British English translation"
  LangString DESC_SecLangcsCZ ${LANG_ENGLISH} "Install Czech translation"
  LangString DESC_SecLangnlNL ${LANG_ENGLISH} "Install Dutch translation"
  LangString DESC_SecLangfrFR ${LANG_ENGLISH} "Install French translation"
  LangString DESC_SecLangdeDE ${LANG_ENGLISH} "Install German translation"
  LangString DESC_SecLangitIT ${LANG_ENGLISH} "Install Italian translation"
  LangString DESC_SecLangplPL ${LANG_ENGLISH} "Install Polish translation"
  LangString DESC_SecLangptBR ${LANG_ENGLISH} "Install Portuguese (Brazil) translation"
  LangString DESC_SecLangptPT ${LANG_ENGLISH} "Install Portuguese (Portugal) translation"
  LangString DESC_SecLangruRU ${LANG_ENGLISH} "Install Russian translation"
  LangString DESC_SecLangzhCN ${LANG_ENGLISH} "Install Simplified Chinese translation"
  LangString DESC_SecLangesES ${LANG_ENGLISH} "Install Spanish translation"
  LangString DESC_SecLangsvSE ${LANG_ENGLISH} "Install Swedish translation"
  LangString DESC_SecLangskSK ${LANG_ENGLISH} "Install Slovak translation"

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecFahMon} $(DESC_SecFahMon)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecWebTemplates} $(DESC_SecWebTemplates)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLang} $(DESC_SecLang)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangenGB} $(DESC_SecLangenGB)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangcsCZ} $(DESC_SecLangcsCZ)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangnlNL} $(DESC_SecLangnlNL)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangfrFR} $(DESC_SecLangfrFR)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangdeDE} $(DESC_SecLangdeDE)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangitIT} $(DESC_SecLangitIT)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangplPL} $(DESC_SecLangplPL)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangptBR} $(DESC_SecLangptBR)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangptPT} $(DESC_SecLangptPT)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangzhCN} $(DESC_SecLangzhCN)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangesES} $(DESC_SecLangesES)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangsvSE} $(DESC_SecLangsvSE)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLangskSK} $(DESC_SecLangskSK)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

Section "Uninstall"

  Delete "$INSTDIR\fahmon.exe"
  Delete "$INSTDIR\messages.log"
  Delete "$INSTDIR\Uninstall.exe"

  MessageBox MB_YESNO "Keep preferences, client-list, project database and benchmarks database?" IDYES true IDNO false
  true:
    DetailPrint "Keeping config folder"
    Goto next
  false:
    DetailPrint "Removing config folder"
    RMDir /r "$APPDATA\FahMon\config"
    RMDir /r "$APPDATA\FahMon\templates"
    RMDir /r "$APPDATA\FahMon"
  next:

   RMDir /r "$INSTDIR\images"
   RMDir /r "$INSTDIR\lang"
   RMDir /r "$INSTDIR\docs"
   RMDir /r "$INSTDIR\templates"
   RMDir /r "$INSTDIR\config"
   RMDir "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP

  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\FahMon.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\Help.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\README.lnk"

  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"

  startMenuDeleteLoop:
	ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."

    IfErrors startMenuDeleteLoopDone

    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:

  DeleteRegKey /ifempty HKCU "Software\FahMon"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FahMon"

SectionEnd
