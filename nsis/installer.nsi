!include "MUI.nsh"

Name "FastW3Join"
OutFile "FastW3Join 1.1.exe"
SetCompressor lzma
InstallDir "$PROGRAMFILES\FastW3Join"

Var STARTMENU_FOLDER
Var MUI_TEMP

!define MUI_ABORTWARNING
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES


!insertmacro MUI_LANGUAGE "German"

Section "FastW3Join" SecInstall

	SetOverwrite on
  SetOutPath "$INSTDIR"

	File ..\bin\Release\fastw3join.exe
	File ..\bin\Release\fastw3join.manifest
	File ..\*.xpm
	File *.dll
	File ..\regex.txt

	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\FastW3Join.lnk" "$INSTDIR\fastw3join.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
	;CreateDirectory "$SMPROGRAMS\FastW3Join"
	;CreateShortCut "$SMPROGRAMS\FastW3Join\FastW3Join.lnk" "$INSTDIR\fastw3join.exe"

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section "WinPcap 4.1.3" PcapInstall

	SetOverwrite on
  SetOutPath "$INSTDIR"

	File WinPcap_4_1_3.exe
	ExecWait '"$INSTDIR\WinPcap_4_1_3.exe"'
	Delete "$INSTDIR\WinPcap_4_1_3.exe"
SectionEnd

LangString DESC_SecInstall ${LANG_GERMAN} "FastW3Join - Hauptprogramm"
LangString DESC_PcapInstall ${LANG_German} "WinPcap wird von FastW3Join benötigt um zu funktionieren. Sollten Sie WinPcap allerdings schon installiert haben, müssen Sie es nicht ein zweites Mal installieren!"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecInstall} $(DESC_SecInstall)
	!insertmacro MUI_DESCRIPTION_TEXT ${PcapInstall} $(DESC_PcapInstall)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section "Uninstall"

	Delete "$INSTDIR\fastw3join.exe"
	Delete "$INSTDIR\fastw3join.manifest"
	Delete "$INSTDIR\wxmsw28u_gcc_custom.dll"
	Delete "$INSTDIR\libgcc_s_dw2-1.dll"
	Delete "$INSTDIR\*.xpm"
	Delete "$INSTDIR\*.dll"
	Delete "$INSTDIR\regex.txt"
	Delete "$INSTDIR\device.txt"
	Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

	!insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP

  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
	Delete "$SMPROGRAMS\$MUI_TEMP\FastW3Join.lnk"

  ;Delete empty start menu parent diretories
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"

  startMenuDeleteLoop:
	ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."

    IfErrors startMenuDeleteLoopDone

    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:
SectionEnd
