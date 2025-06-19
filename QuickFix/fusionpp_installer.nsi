!include "MUI2.nsh"

; === Branding (replace with your own) ===
!define MUI_ICON       "fusionpp.ico"             ; Your app icon
!define MUI_UNICON     "fusionpp-uninstall.ico"   ; Uninstaller icon
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "banner.bmp"       ; Banner image (500x70px)

; === Basic Info ===
Name "Fusion++ Language Suite"
OutFile "FusionPP_Setup.exe"
InstallDir "$PROGRAMFILES\FusionPP"
RequestExecutionLevel admin

; === Pages ===
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

; === Languages ===
!insertmacro MUI_LANGUAGE "English"

; === Silent Install Option (add /S on command line) ===
SilentInstall silent

; === Installer Sections ===

Section "Fusion++ Core" SecCore
    SectionIn RO
    SetOutPath "$INSTDIR"
    File "dist\fusionpp_runtime.exe"
    File "dist\fusionpp_multiplayer_gui.exe"
    File "dist\ai_bridge.exe"
    File "dist\README.txt"
    ; Qt DLLs
    File /oname=$INSTDIR\Qt5Core.dll "dist\Qt5Core.dll"
    File /oname=$INSTDIR\Qt5Gui.dll "dist\Qt5Gui.dll"
    File /oname=$INSTDIR\Qt5Widgets.dll "dist\Qt5Widgets.dll"
    File /oname=$INSTDIR\libgcc_s_seh-1.dll "dist\libgcc_s_seh-1.dll"
    File /oname=$INSTDIR\libstdc++-6.dll "dist\libstdc++-6.dll"
    File /oname=$INSTDIR\libwinpthread-1.dll "dist\libwinpthread-1.dll"
    ; (Add other DLLs as needed)
SectionEnd

Section "Assets & Headers" SecAssets
    SetOutPath "$INSTDIR\assets"
    File /r "dist\assets\*.*"
    SetOutPath "$INSTDIR\include"
    File /r "dist\include\*.*"
SectionEnd

Section "Desktop Shortcut" SecShortcut
    CreateShortCut "$DESKTOP\Fusion++ IDE.lnk" "$INSTDIR\fusionpp_multiplayer_gui.exe"
SectionEnd

; === Uninstall Section ===
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
    RMDir /r "$INSTDIR\assets"
    RMDir /r "$INSTDIR\include"
    Delete "$DESKTOP\Fusion++ IDE.lnk"
    RMDir "$INSTDIR"
SectionEnd

; === Installer Branding and Details ===
BrandingText "Fusion++ Language Suite (c) 2025 Violet Aura Creations"

; === Post-Install Auto-Launch ===
Function .onInstSuccess
    IfSilent 0 +2
    ExecShell "" "$INSTDIR\fusionpp_multiplayer_gui.exe"
FunctionEnd
