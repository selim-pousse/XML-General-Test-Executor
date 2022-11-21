;author : ChKu

!define PRODUCT_NAME "Xml General Test Executor"
!define PRODUCT_TECHNICAL_NAME "Xml-GTE"
!define PRODUCT_BINARY_NAME "XmlGTE.exe"
!define PRODUCT_VERSION "01.02.05"
!define PRODUCT_PUBLISHER "Pousse Systems GmbH"
!define PRODUCT_WEB_SITE "http://pousse.systems"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_TECHNICAL_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

!addplugindir  "..\..\CommonLibs\nsisplugins"
!define XmlGTE_SOURCES_PATH ".."
!define PTE_TEMPORARY_FILES_PATH "$APPDATA\${PRODUCT_TECHNICAL_NAME}"

!define COMMON_INSTALL_PATH "$INSTDIR\bin"

; MUI 1.67 compatible ------
!include "MUI.nsh"
;call system function
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "WinMessages.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEPAGE_TITLE "${PRODUCT_NAME}"
!insertmacro MUI_PAGE_WELCOME

; Directory page
; Note: The user can't change the installation directory -> hide the dialog
; !define MUI_PAGE_CUSTOMFUNCTION_SHOW dirDisabled
; !insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TITLE "${PRODUCT_NAME}"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
; Only English
;!insertmacro MUI_LANGUAGE "French"
;!insertmacro MUI_LANGUAGE "German"
;!insertmacro MUI_LANGUAGE "Italian"

; MUI end ------

; Language strings
LangString msgInstalled ${LANG_ENGLISH} "${PRODUCT_NAME} is already installed.$\r$\nUninstall the existing version?"
;LangString msgInstalled ${LANG_FRENCH}  "${PRODUCT_NAME} is already installed.$\r$\nUninstall the existing version?"
;LangString msgInstalled ${LANG_GERMAN}  "${PRODUCT_NAME} ist bereits installiert.$\r$\nMöchten Sie jetzt deinstallieren?"
;LangString msgInstalled ${LANG_ITALIAN} "${PRODUCT_NAME} is already installed.$\r$\nUninstall the existing version?"

LangString msgUninstallCompleted ${LANG_ENGLISH} "${PRODUCT_NAME} has been removed from your computer."
;LangString msgUninstallCompleted ${LANG_FRENCH}  "${PRODUCT_NAME} has been removed from your computer."
;LangString msgUninstallCompleted ${LANG_GERMAN}  "${PRODUCT_NAME} wurde von Ihrem Computer entfernt."
;LangString msgUninstallCompleted ${LANG_ITALIAN} "${PRODUCT_NAME} has been removed from your computer."

LangString msgConfirmUninstall ${LANG_ENGLISH} "Are you sure you want to remove $(^Name) from your computer?"
;LangString msgConfirmUninstall ${LANG_FRENCH}  "Are you sure you want to remove $(^Name) from your computer?"
;LangString msgConfirmUninstall ${LANG_GERMAN}  "Sind Sie sicher, dass sie $(^Name) deinstallieren möchten?"
;LangString msgConfirmUninstall ${LANG_ITALIAN} "Are you sure you want to remove $(^Name) from your computer?"

LangString msgConfirmGTENotRunning ${LANG_ENGLISH} "${PRODUCT_NAME} is running. Please close it first before continuing the installation."
;LangString msgConfirmGTENotRunning ${LANG_FRENCH} "${PRODUCT_NAME} est en cours d'éxecution. Veuillez le fermer avant de continuer l'installation."
;LangString msgConfirmGTENotRunning ${LANG_GERMAN} "${PRODUCT_NAME} läuft. Bitte schließen Sie sie before fortsetzen."
;LangString msgConfirmGTENotRunning ${LANG_ITALIAN} "${PRODUCT_NAME} is running. Please close it first before continuing the installation."

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Xml_GTE_Setup_${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_TECHNICAL_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section
  ${GetParent} "$INSTDIR" $R0 ;useless now
  ; $R0= parent direction of install path
  ${GetParent} "$WINDIR" $R1 ;useless now
  ;OS root drive
SectionEnd

Section "MainSection" SEC01
;********** Check if XmlGTE is Running **********  
CheckRunning:
; to check if a process having the name XmlGTE executable is running.
FindProcDLL::FindProc "XmlGTE.exe" 
IntCmp $R0 1 0 NotRunning
    MessageBox MB_YESNO $(msgConfirmGTENotRunning) /SD IDYES IDNO Quit
    Goto CheckRunning
NotRunning:

;********** Check if already installed **********
; Check if already installed
  ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  IfFileExists $R0 0 NotInstalled
  MessageBox MB_YESNO $(msgInstalled) /SD IDYES IDNO Quit
    Pop $R1
  StrCmp $R1 2 Quit +1
  
  ;The NSIS uninstaller copies itself to the temporary directory, runs the temporary uninstaller created and then exits. This is done so it will be able to delete itself. 
  ;the system can be disasbled by using the following command line parameter:
  ExecWait '"$INSTDIR\uninst.exe" _?=$INSTDIR' 
  
  Goto NotInstalled
Quit:
  Quit

NotInstalled:
  SetShellVarContext current

;************************************************
  SetOutPath "$INSTDIR\configfiles"
;*****************Librairies dependency *****************
  SetOutPath "$INSTDIR\bin\platforms"
  SetOverwrite ifnewer
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\platforms\qminimal.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\platforms\qoffscreen.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\platforms\qwindows.dll"
  
  SetOutPath "$INSTDIR\bin\imageformats"
  SetOverwrite ifnewer
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\imageformats\qgif.dll"
  
  SetOutPath "$INSTDIR\bin"
  SetOverwrite ifnewer
  ;SetOverwrite ifnewer on off try
  ; add the Qt5 Librairies version 5.3
  ;File "..\..\CommonLibsQt5\Qt5RuntimeDlls\icudt54.dll"
  ;File "..\..\CommonLibsQt5\Qt5RuntimeDlls\icuin54.dll"
  ;File "..\..\CommonLibsQt5\Qt5RuntimeDlls\icuuc54.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\libgcc_s_dw2-1.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\libstdc++-6.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\libwinpthread-1.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\Qt5Core.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\Qt5Gui.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\Qt5Network.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\Qt5Widgets.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\Qt5Xml.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\Qt5XmlPatterns.dll"
  File "..\..\CommonLibsQt5\Qt5RuntimeDlls\Qt5Script.dll"
  File "..\..\CommonLibsQt5\qt-ssh-libs\ssh.dll"
  
  ;***************** XmlGTE *****************
  
  SetOutPath "${COMMON_INSTALL_PATH}"
  File "${XmlGTE_SOURCES_PATH}\bin\XmlGTE.exe"
  File "${XmlGTE_SOURCES_PATH}\bin\xmlagte.exe"
  SetOutPath "$INSTDIR\"
  File /r "${XmlGTE_SOURCES_PATH}\configfiles"
  ;SetOutPath "$INSTDIR\language"
  ;File "${XmlGTE_SOURCES_PATH}\language\*.qm"
  SetOutPath "$INSTDIR\qrc"
  File "${XmlGTE_SOURCES_PATH}\qrc\Xml.ico"
  SetOutPath "$INSTDIR\qrc"
  File /r "${XmlGTE_SOURCES_PATH}\qrc\images"
  SetOutPath "$INSTDIR\configfiles"
  File "ReleaseNotes.txt"
   textreplace::_ReplaceInFile "$INSTDIR\configfiles\sysConfig.ini" "$INSTDIR\configfiles\sysConfig.ini" "<<PRODUCT_VERSION_GTE>>" "${PRODUCT_VERSION}" "/S=1 /C=0"


  ;SetAutoClose false
SectionEnd

Section -AdditionalIcons
  SetShellVarContext current
  CreateShortCut "$DESKTOP\${PRODUCT_TECHNICAL_NAME}.lnk" "${COMMON_INSTALL_PATH}\${PRODUCT_BINARY_NAME}"
  CreateDirectory "$SMPROGRAMS\${PRODUCT_TECHNICAL_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_TECHNICAL_NAME}\${PRODUCT_TECHNICAL_NAME}.lnk" "${COMMON_INSTALL_PATH}\${PRODUCT_BINARY_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_TECHNICAL_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"
  ;DO NOT set path environment variable ANYMORE
  ;ReadRegStr $0 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path" ; admin right
  ;WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path" "$0;$INSTDIR\DependentDlls"
  ;ReadRegStr $1 HKCU "Environment" "Path" ; user right
  ;WriteRegStr HKCU "Environment" "Path" "$1;$INSTDIR\DependentDlls"
  SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=250
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\AAL.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PRODUCT_BINARY_NAME}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK $(msgUninstallCompleted)
FunctionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 $(msgConfirmUninstall) IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$DESKTOP\${PRODUCT_TECHNICAL_NAME}.lnk"

# For some reason the start menu entries are stored in the 'all users' section
# even if declared 'SetShellVarContext current' during the installation
# At uninstallation time these links can only be deleted if the user is admin
# -> determine the account type here and delete the appropriate links
  ClearErrors
  UserInfo::GetName
  IfErrors Win9x
  Pop $0
  UserInfo::GetAccountType
  Pop $1
  # GetOriginalAccountType will check the tokens of the original user of the
  # current thread/process. If the user tokens were elevated or limited for
  # this process, GetOriginalAccountType will return the non-restricted
  # account type.
  # On Vista with UAC, for example, this is not the same value when running
  # with `RequestExecutionLevel user`. GetOriginalAccountType will return
  # "admin" while GetAccountType will return "user".

#  UserInfo::GetOriginalAccountType
#  !define REALMSG "$\nOriginal non-restricted account type: $2"
#  Pop $2
  StrCmp $1 "Admin" 0 Next1
#    MessageBox MB_OK 'User "$0" is in the Administrators group${REALMSG}'
# Consider doing _both_ 'user' and 'all'
	SetShellVarContext current
	RMDir /r "$SMPROGRAMS\${PRODUCT_TECHNICAL_NAME}"
	SetShellVarContext all
    Goto done
  Next1:
  StrCmp $1 "Power" 0 Next2
#    MessageBox MB_OK 'User "$0" is in the Power Users group${REALMSG}'
# Consider doing _both_ 'user' and 'all'
	SetShellVarContext current
	RMDir /r "$SMPROGRAMS\${PRODUCT_TECHNICAL_NAME}"
	SetShellVarContext all
    Goto done
  Next2:
  StrCmp $1 "User" 0 Next3
#    MessageBox MB_OK 'User "$0" is just a regular user${REALMSG}'
	SetShellVarContext current
    Goto done
  Next3:
  StrCmp $1 "Guest" 0 Next4
#    MessageBox MB_OK 'User "$0" is a guest${REALMSG}'
	SetShellVarContext current
    Goto done
  Next4:
  MessageBox MB_OK "Unknown error"
  
  Goto done
  Win9x:
    # This one means you don't need to care about admin or
    # not admin because Windows 9x doesn't either
#    MessageBox MB_OK "Error! This DLL can't run under Windows 9x!"

  done:
  RMDir /r "$SMPROGRAMS\${PRODUCT_TECHNICAL_NAME}"
  RMDir /r "${PTE_TEMPORARY_FILES_PATH}"
  RMDir /r "$INSTDIR\"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  
  ;restore path environment variable
  ;ReadRegStr $R3 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path" ; admin right
  ;${WordReplace} $R3 ";$INSTDIR\DependentDlls" "" "+" $R4
  ;WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path" "$R4"
  ;ReadRegStr $R3 HKCU "Environment" "Path" ; user right
  ;${WordReplace} $R3 ";$INSTDIR\DependentDlls" "" "+" $R4
  ;WriteRegStr HKCU "Environment" "Path" "$R4"
  
  SetAutoClose true
SectionEnd