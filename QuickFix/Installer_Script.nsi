!include "MUI2.nsh"

; ====== BRANDING ======
!define MUI_ICON       "fusionpp.ico"
!define MUI_UNICON     "fusionpp-uninstall.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "banner.bmp" ; 500x70px, BMP

; ====== APP INFO ======
Name "Fusion++ Language Suite"
OutFile "FusionPP_Setup.exe"
InstallDir "$PROGRAMFILES\FusionPP"
RequestExecutionLevel admin

; ====== PAGES ======
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\fusionpp_multiplayer_gui.exe"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
BrandingText "Fusion++ Language Suite — S.U.E.T. License"

SilentInstall silent

; ====== VARIABLES ======
Var StartMenuFolder

; ====== SECTIONS ======

Section "Fusion++ Core" SecCore
    SectionIn RO
    SetOutPath "$INSTDIR"
    File "dist\fusionpp_runtime.exe"
    File "dist\fusionpp_multiplayer_gui.exe"
    File "dist\ai_bridge.exe"
    File "dist\README.txt"
    File /oname=$INSTDIR\Qt5Core.dll "dist\Qt5Core.dll"
    File /oname=$INSTDIR\Qt5Gui.dll "dist\Qt5Gui.dll"
    File /oname=$INSTDIR\Qt5Widgets.dll "dist\Qt5Widgets.dll"
    File /oname=$INSTDIR\libgcc_s_seh-1.dll "dist\libgcc_s_seh-1.dll"
    File /oname=$INSTDIR\libstdc++-6.dll "dist\libstdc++-6.dll"
    File /oname=$INSTDIR\libwinpthread-1.dll "dist\libwinpthread-1.dll"
SectionEnd

Section "Assets & Headers" SecAssets
    SetOutPath "$INSTDIR\assets"
    File /r "dist\assets\*.*"
    SetOutPath "$INSTDIR\include"
    File /r "dist\include\*.*"
SectionEnd

Section "Start Menu Shortcut" SecStartMenu
    CreateDirectory "$SMPROGRAMS\Fusion++"
    CreateShortCut "$SMPROGRAMS\Fusion++\Fusion++ IDE.lnk" "$INSTDIR\fusionpp_multiplayer_gui.exe"
    CreateShortCut "$DESKTOP\Fusion++ IDE.lnk" "$INSTDIR\fusionpp_multiplayer_gui.exe"
SectionEnd

Section "File Association for .fpp" SecAssoc
    ; Associate .fpp with IDE
    WriteRegStr HKCR ".fpp" "" "FusionPP.File"
    WriteRegStr HKCR "FusionPP.File" "" "Fusion++ Script"
    WriteRegStr HKCR "FusionPP.File\DefaultIcon" "" "$INSTDIR\fusionpp.ico"
    WriteRegStr HKCR "FusionPP.File\shell\open\command" "" '"$INSTDIR\fusionpp_multiplayer_gui.exe" "%1"'
SectionEnd

; ====== Registry Uninstall Entry ======
Section -PostInstall
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FusionPP" "DisplayName" "Fusion++ Language Suite"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FusionPP" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FusionPP" "InstallLocation" "$INSTDIR"
    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

; ====== UNINSTALL SECTION ======
Section "Uninstall"
    Delete "$INSTDIR\fusionpp_runtime.exe"
    Delete "$INSTDIR\fusionpp_multiplayer_gui.exe"
    Delete "$INSTDIR\ai_bridge.exe"
    Delete "$INSTDIR\README.txt"
    Delete "$INSTDIR\Qt5Core.dll"
    Delete "$INSTDIR\Qt5Gui.dll"
    Delete "$INSTDIR\Qt5Widgets.dll"
    Delete "$INSTDIR\libgcc_s_seh-1.dll"
    Delete "$INSTDIR\libstdc++-6.dll"
    Delete "$INSTDIR\libwinpthread-1.dll"
    Delete "$INSTDIR\fusionpp.ico"
    Delete "$DESKTOP\Fusion++ IDE.lnk"
    Delete "$SMPROGRAMS\Fusion++\Fusion++ IDE.lnk"
    RMDir /r "$INSTDIR\assets"
    RMDir /r "$INSTDIR\include"
    RMDir "$SMPROGRAMS\Fusion++"
    RMDir "$INSTDIR"
    DeleteRegKey HKCR ".fpp"
    DeleteRegKey HKCR "FusionPP.File"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FusionPP"
SectionEnd

; ====== Auto-Launch IDE on Install Finish ======
Function .onInstSuccess
    IfSilent 0 +2
    ExecShell "" "$INSTDIR\fusionpp_multiplayer_gui.exe"
FunctionEnd
