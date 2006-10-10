!define PRODUCT_NAME "Scooter"
!define PRODUCT_PUBLISHER "kloper@users.sf.net"
!define PRODUCT_WEB_SITE "http://sctr.sf.net"

SetCompressor lzma

!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!define MUI_LICENSEPAGE_CHECKBOX
!insertmacro MUI_PAGE_LICENSE "COPYING"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES


; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${OUTFILE}"
InstallDir "$PROGRAMFILES\Scooter"
ShowInstDetails show
ShowUnInstDetails show

Section "Executable" SEC01
  SetOutPath "$INSTDIR\bin"
  SetOverwrite ifnewer
  File "cr.exe"
  File "cr0antlr.dll"
  File "cr0dgd.dll"
  File "cr0jpeg.dll"
  File "cr0openvrml.dll"
  File "cr0png.dll"
  File "cr0zlib.dll"
  File "boost_regex-mgw-mt-d-1_31.dll"
  File "Qt3Support4.dll"
  File "QtCore4.dll"
  File "QtGui4.dll"
  File "QtNetwork4.dll"
  File "QtOpenGL4.dll"
  File "QtSvg4.dll"
  File "QtXml4.dll"
  File "mingwm10.dll"
SectionEnd

Section "Config" SEC02
  SetOutPath "$INSTDIR\etc"  
  File "COPYING"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section "Desktop Shortcut" SecShortcuts
  CreateDirectory $SMPROGRAMS\Scooter
  CreateShortCut "$SMPROGRAMS\Scooter\Chromium.lnk" "$INSTDIR\bin\cr.exe" ""
  WriteINIStr "$SMPROGRAMS\Scooter\Scooter on sourceforge.url" "InternetShortcut" "URL" "http://sourceforge.net/projects/sctr"
  CreateShortCut "$SMPROGRAMS\Scooter\Uninstall Scooter.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\Chromium.lnk" "$INSTDIR\bin\cr.exe"
SectionEnd

Section Uninstall
  RMDir /r $SMPROGRAMS\Scooter
  Delete "$DESKTOP\Chromium.lnk"

  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\bin\*"
  Delete "$INSTDIR\etc\*"

  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR\etc"
  RMDir "$INSTDIR"

  SetAutoClose true
SectionEnd