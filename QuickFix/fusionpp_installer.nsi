!include "MUI2.nsh"

; === Basic Info ===
Name "Fusion++ Language Suite"
OutFile "FusionPP_Setup.exe"
InstallDir "$PROGRAMFILES\FusionPP"
RequestExecutionLevel admin

; === Modern Interface Settings ===
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; === Pages ===
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; === Section: Main Install ===
Section "Fusion++ (Required)" SecFusionPP
    SetOutPath "$INSTDIR"
    ; Binaries
    File "dist\fusionpp_runtime.exe"
    File "dist\fusionpp_multiplayer_gui.exe"
    File "dist\ai_bridge.exe"
    ; Qt DLLs (copies all .dll in dist)
    File /oname=$INSTDIR\Qt5Core.dll "dist\Qt5Core.dll"
    File /oname=$INSTDIR\Qt5Gui.dll "dist\Qt5Gui.dll"
    File /oname=$INSTDIR\Qt5Widgets.dll "dist\Qt5Widgets.dll"
    File /oname=$INSTDIR\libgcc_s_seh-1.dll "dist\libgcc_s_seh-1.dll"
    File /oname=$INSTDIR\libstdc++-6.dll "dist\libstdc++-6.dll"
    File /oname=$INSTDIR\libwinpthread-1.dll "dist\libwinpthread-1.dll"
    ; (add any other DLLs from dist as needed)
    ; Headers & Assets
    SetOutPath "$INSTDIR\assets"
    File /r "dist\assets\*.*"
    SetOutPath "$INSTDIR\include"
    File /r "dist\include\*.*"
    SetOutPath "$INSTDIR"
    File "dist\README.txt"
SectionEnd

; === Section: Desktop Shortcut ===
Section "Create Desktop Shortcut"
    CreateShortCut "$DESKTOP\Fusion++ IDE.lnk" "$INSTDIR\fusionpp_multiplayer_gui.exe"
SectionEnd

; === Section: Uninstall ===
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

; === Installer End ===
