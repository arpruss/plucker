;-----------------------------------------------------------------------------------------
; Name:        plucker-desktop.iss
; Purpose:     Inno Setup (www.innosetup.com) file for Plucker Desktop
;              Required software: Inno Setup 3.0.6.3
;                                 Inno Setup Extensions 3.0.6.2
;              Optional software: ISTool 3.0.6.3
;              Note 1: I had to comment out some things from Builtins.iss to compile
;                      (I don't use any Built-in macros anyways).
;              Note 2: You can't compile with the plucker desktop and the other precompiled
;                      packages parts on the desktop or else the ..\..\..\ stuff doesn't work
;                      (some Windows oddity I guess).
; Author:      Robert O'Connor
; Modified by:
; Created:     2002/05/30
; Copyright:   (c) Robert O'Connor ( rob@medicalmnemonics.com )
; Licence:     GPL
; RCS-ID:      $Id: plucker-desktop.iss,v 1.33 2007/06/30 14:55:44 robertoconnor Exp $
;-----------------------------------------------------------------------------------------

;-----------------------------------------------------------------------------------------
; Begin Editable defines section
;-----------------------------------------------------------------------------------------

#define compilation_path_to_msw_package                      	= "..\..\..\..\plucker_msw_package"
#define compilation_path_to_msw_package_dont_overwrite          = "..\..\..\..\plucker_msw_package_dont_overwrite"

; 1 to include the borland 5.5 runtime dlls (only needed if compiled with borland 5.5):
#define compilation_include_borland_dlls                        = 0
; 1 to include the visual C++ 6.0 runtime debug dlls (only needed if compiled with visual C++ 6.0):
#define compilation_include_visual_c_debug_dlls                 = 1

; Name of the wxwindows dll:
;#define wxwindows_dll_filename                                  = "wx24d_bcc.dll"
#define wxwindows_dll_filename                                  = "wxmsw24d.dll"

#include "customization_defines.h"

;-----------------------------------------------------------------------------------------
; End Editable defines section: you shouldn't want to change the stuff below here.
;-----------------------------------------------------------------------------------------

; Without this, the ISPP will treat the \" in "C:\Windows\" as an escaped " at the end
#pragma parseroption -p+

;-----------------------------------------------------------------------------------------
[Setup]
;-----------------------------------------------------------------------------------------
AppName={#branding_application_suite_name}
; This also appears in the Control Panel's "Remove program" listing
AppVerName={#branding_application_suite_name} {#branding_application_version_number}
AppPublisher={#branding_application_publisher}
AppPublisherURL={#branding_application_publisher_url}
AppSupportURL={#branding_application_support_url}
AppUpdatesURL={#branding_application_updates_url}
; NOTE: I think I am going to leave the registry location alone for now,
; since conduit needs it also.
DefaultDirName={reg:HKCU\Software\The Plucker Team\Plucker,Path|{pf}\{#branding_application_suite_name}}
; Need to disable directory page, since plucker_desktop must be a subdir of the Plucker dir in the registry
DisableDirPage=false
DefaultGroupName={#branding_application_suite_name}
AllowNoIcons=true

DirExistsWarning=no
WizardImageFile=installer_wizard_large.bmp
OutputBaseFilename={#branding_installer_root_filename}_{#branding_application_version_number}
DisableStartupPrompt=true
; The directory to place uninstall files. Never change this value or else
; can't find uninstall files from a previous install to overwrite.
UninstallFilesDir={app}\uninstall

LicenseFile=..\..\docs\GPL_LICENSE.txt
; Using bzip instead of zip will make a smaller package, but its too slow, only saves 200K.
; Compression=bzip
#if branding_installer_show_before_install_message == 1
InfoAfterFile=before_install_message.txt
#endif
#if branding_installer_show_after_install_message == 1
InfoBeforeFile=after_install_message.txt
#endif

; I wonder if I should use this key. It will give an error if user doesn't have
; admin privleges when they install.
; AdminPrivilegesRequired=true

; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51


;-----------------------------------------------------------------------------------------
[Tasks]
;-----------------------------------------------------------------------------------------
;Desktop icon
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:; MinVersion: 4,4; Components: plucker_desktop
;Right click conversion
#if branding_installer_tasks_right_click_convert == 1
Name: convert_html_text_documents; Description: Add 'Convert To {#branding_application_suite_name}' menu item for &HTML and text files (htm, html, txt); GroupDescription: Windows Explorer right-click menu integration:; Flags: unchecked
Name: convert_image_documents; Description: Add 'Convert To {#branding_application_suite_name}' menu item for &image files (png, gif, jpg); GroupDescription: Windows Explorer right-click menu integration:; Flags: unchecked
#endif
;Sample channels
#if branding_installer_allow_not_setup_default_channels == 1
Name: sample_channels; Description: Setup &default initial {#branding_application_suite_name} channels; GroupDescription: Automatic setup of initial channels:
#endif

;-----------------------------------------------------------------------------------------
[Files]
;-----------------------------------------------------------------------------------------
Source: ..\..\plucker-desktop.exe; DestDir: {app}\plucker_desktop; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\plucker-desktop.exe.manifest; DestDir: {app}\plucker_desktop; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\{#wxwindows_dll_filename}; DestDir: {app}\plucker_desktop; Components: plucker_desktop; Flags: ignoreversion
#if compilation_include_borland_dlls == 1
Source: ..\..\cc3250mt.dll; DestDir: {app}\plucker_desktop; Components: plucker_desktop; Flags: ignoreversion
#endif
#if compilation_include_visual_c_debug_dlls == 1
Source: {#compilation_path_to_msw_package_dont_overwrite}\msvcrtd.dll; DestDir: {app}\plucker_desktop; Components: plucker_desktop; Flags: ignoreversion
#endif
Source: ..\..\docs\TODO.txt; DestDir: {app}\plucker_desktop\; Components: plucker_desktop
Source: ..\..\docs\PLUCKER_DESKTOP_CHANGELOG.txt; DestDir: {app}\plucker_desktop; Components: plucker_desktop
Source: ..\..\README.txt; DestDir: {app}\plucker_desktop; Components: plucker_desktop
Source: ..\..\resource\graphics_dialog\*.*; DestDir: {app}\plucker_desktop\resource\graphics_dialog; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\graphics_misc\*.*; DestDir: {app}\plucker_desktop\resource\graphics_misc; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\graphics_wizard\*.*; DestDir: {app}\plucker_desktop\resource\graphics_wizard; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\icons_application\*.*; DestDir: {app}\plucker_desktop\resource\icons_application; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\icons_dialog\*.*; DestDir: {app}\plucker_desktop\resource\icons_dialog; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\icons_image_list\*.*; DestDir: {app}\plucker_desktop\resource\icons_image_list; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\launcher_icons_large\*.*; DestDir: {app}\plucker_desktop\resource\launcher_icons_large; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\launcher_icons_small\*.*; DestDir: {app}\plucker_desktop\resource\launcher_icons_small; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\strings_default_exclusion_list\*.*; DestDir: {app}\plucker_desktop\resource\strings_default_exclusion_list; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\strings_tips\*.*; DestDir: {app}\plucker_desktop\resource\strings_tips; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\xrc\*.*; DestDir: {app}\plucker_desktop\resource\xrc; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\docs\GPL_LICENSE.txt; DestDir: {app}\doc; Flags: promptifolder
Source: ..\..\resource\icons_art_provider\*.*; DestDir: {app}\plucker_desktop\resource\icons_art_provider; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\html_about\*.*; DestDir: {app}\plucker_desktop\resource\html_about; Components: plucker_desktop; Flags: ignoreversion
Source: ..\..\resource\docs\plucker-desktop.htb; DestDir: {app}\doc; Components: plucker_desktop; Flags: ignoreversion
#if branding_installer_include_full_plucker_manual == 1
Source: ..\..\resource\docs\plucker-manual.htb; DestDir: {app}\doc; Flags: ignoreversion
#endif
; Settings: change task based on whether or not allowed not install during setup questions
#if branding_installer_allow_not_setup_default_channels	== 1
Source: ..\..\resource\plkrdata_installation\automatically_installed_channels.plkrdata; DestDir: {app}\plucker_desktop\resource\plkrdata_installation; Tasks: sample_channels; Flags: ignoreversion
#else
Source: ..\..\resource\plkrdata_installation\automatically_installed_channels.plkrdata; DestDir: {app}\plucker_desktop\resource\plkrdata_installation; Flags: ignoreversion
#endif
Source: ..\..\resource\plkrdata_installation\automatically_installed_settings.plkrdata; DestDir: {app}\plucker_desktop\resource\plkrdata_installation; Flags: ignoreversion
; Showcase file. Has to be the specific file though.
Source: ..\..\resource\plkrdata_showcase\*.*; DestDir: {app}\plucker_desktop\resource\plkrdata_showcase; Components: plucker_desktop; Flags: ignoreversion
; Setupwizard file: these channels will be optionally installed. Has to be the specific file though.
Source: ..\..\resource\plkrdata_setup_wizard\*.*; DestDir: {app}\plucker_desktop\resource\plkrdata_setup_wizard; Flags: ignoreversion

;Internationalization
#if branding_installer_components_internationalization == 1
Source: ..\..\langs\ca\*.*; DestDir: {app}\plucker_desktop\langs\ca; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\cs\*.*; DestDir: {app}\plucker_desktop\langs\cs; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\da\*.*; DestDir: {app}\plucker_desktop\langs\da; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\de\*.*; DestDir: {app}\plucker_desktop\langs\de; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\es\*.*; DestDir: {app}\plucker_desktop\langs\es; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\fi\*.*; DestDir: {app}\plucker_desktop\langs\fi; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\fo\*.*; DestDir: {app}\plucker_desktop\langs\fo; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\fr\*.*; DestDir: {app}\plucker_desktop\langs\fr; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\it\*.*; DestDir: {app}\plucker_desktop\langs\it; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\ja\*.*; DestDir: {app}\plucker_desktop\langs\ja; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\nl\*.*; DestDir: {app}\plucker_desktop\langs\nl; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\pl\*.*; DestDir: {app}\plucker_desktop\langs\pl; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\ru\*.*; DestDir: {app}\plucker_desktop\langs\ru; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\sv\*.*; DestDir: {app}\plucker_desktop\langs\sv; Components: internationalization; Flags: ignoreversion
Source: ..\..\langs\*.txt; DestDir: {app}\plucker_desktop\langs; Components: internationalization; Flags: ignoreversion
#endif
; Plucker core engine items
Source: {#compilation_path_to_msw_package}\*.*; DestDir: {app}; Flags: ignoreversion
; Plucker python parser
Source: {#compilation_path_to_msw_package}\parser\python\PyPlucker\*.*; Flags: recursesubdirs ignoreversion; DestDir: {app}\parser\python\PyPlucker; Components: plucker_engine
; Python distribution
Source: {#compilation_path_to_msw_package}\parser\python\vm\*.*; Flags: recursesubdirs ignoreversion; DestDir: {app}\parser\python\vm; Components: plucker_engine
; Plucker conduit files
Source: {#compilation_path_to_msw_package}\plucker_conduit\condinst.exe; DestDir: {app}\plucker_conduit; Components: plucker_conduit; Flags: ignoreversion
Source: {#compilation_path_to_msw_package}\plucker_conduit\CondMgr.dll; DestDir: {app}\plucker_conduit; Components: plucker_conduit; Flags: ignoreversion
Source: {#compilation_path_to_msw_package}\plucker_conduit\HSAPI.dll; DestDir: {app}\plucker_conduit; Components: plucker_conduit; Flags: ignoreversion
Source: {#compilation_path_to_msw_package}\plucker_conduit\UserData.dll; DestDir: {app}\plucker_conduit; Components: plucker_conduit; Flags: ignoreversion
Source: {#compilation_path_to_msw_package}\plucker_conduit\{#branding_conduit_filename}; DestDir: {app}\plucker_conduit; Components: plucker_conduit; Flags: ignoreversion

; Plucker standalone documentation viewer
Source: {#compilation_path_to_msw_package}\doc\*.*; DestDir: {app}\doc; Components: plucker_engine; Flags: ignoreversion
; Palm software
#if branding_installer_components_internationalization == 1
Source: {#compilation_path_to_msw_package}\viewer\palmos\*.*; DestDir: {app}\viewer\palmos; Components: internationalization; Flags: ignoreversion
#else
Source: {#compilation_path_to_msw_package}\viewer\palmos\SysZLib.prc; DestDir: {app}\viewer\palmos; Flags: ignoreversion
Source: {#compilation_path_to_msw_package}\viewer\palmos\SysZLib_hires.prc; DestDir: {app}\viewer\palmos; Flags: ignoreversion
Source: {#compilation_path_to_msw_package}\viewer\palmos\PluckerUserGuide.pdb; DestDir: {app}\viewer\palmos; Flags: ignoreversion
Source: {#compilation_path_to_msw_package}\viewer\palmos\{#branding_viewer_lores_basename}en.prc; DestDir: {app}\viewer\palmos; Flags: ignoreversion
Source: {#compilation_path_to_msw_package}\viewer\palmos\{#branding_viewer_hires_basename}en.prc; DestDir: {app}\viewer\palmos; Flags: ignoreversion
#endif
; I put the files that I don't want to overwrite, into a separate directory
Source: {#compilation_path_to_msw_package_dont_overwrite}\plucker.ini; DestDir: {app}; Flags: uninsneveruninstall onlyifdoesntexist


;-----------------------------------------------------------------------------------------
[Icons]
;-----------------------------------------------------------------------------------------
;This one is the icon on the desktop
Name: {userdesktop}\{#branding_application_desktop_name}; Filename: {app}\plucker_desktop\plucker-desktop.exe; MinVersion: 4,4; Tasks: desktopicon; WorkingDir: {app}\plucker_desktop; IconFilename: {app}\plucker_desktop\resource\icons_application\application.ico; IconIndex: 0; Components: plucker_desktop
;This one is to put a shortcut to Plucker Desktop in the Plucker Home directory.
Name: {app}\{#branding_application_desktop_name}; Filename: {app}\plucker_desktop\plucker-desktop.exe; WorkingDir: {app}; IconIndex: 0; IconFilename: {app}\plucker_desktop\resource\icons_application\application.ico
;This ones are the ones in the Start Menu
Name: {group}\{#branding_application_desktop_name}; Filename: {app}\plucker_desktop\plucker-desktop.exe; IconIndex: 0; WorkingDir: {app}\plucker_desktop; IconFilename: {app}\plucker_desktop\resource\icons_application\application.ico; Components: plucker_desktop
Name: {group}\Documentation\{#branding_application_desktop_name} Help; Filename: {app}\doc\helpview.exe; IconIndex: 0; Parameters: """{app}\doc\plucker-desktop.htb"""
Name: {group}\Uninstall {#branding_application_suite_name}; Filename: {uninstallexe}; IconIndex: 0
#if branding_installer_include_full_plucker_manual == 1
Name: {group}\Documentation\Full {#branding_application_suite_name} Manual; Filename: {app}\doc\helpview.exe; IconIndex: 0; Parameters: """{app}\doc\plucker-manual.htb"""
#endif


;-----------------------------------------------------------------------------------------
[Run]
;-----------------------------------------------------------------------------------------
Filename: {app}\plucker_conduit\condinst.exe; Parameters: -i -r -s -e {#branding_creator_id} -n {#branding_conduit_title} -t {#branding_conduit_title} -c {app}\plucker_conduit\{#branding_conduit_filename}; Components: plucker_conduit; Flags: waituntilidle
Filename: {reg:HKCU\Software\U.S. Robotics\Pilot Desktop\Core,HotSyncPath|C:\Palm\HotSync.exe}; Flags: nowait skipifdoesntexist; Components: plucker_conduit
Filename: {reg:HKCU\Software\Palm Computing\Pilot Desktop\Core,HotSyncPath|C:\Palm\HotSync.exe}; Flags: nowait skipifdoesntexist; Components: plucker_conduit
Filename: {app}\plucker_desktop\plucker-desktop.exe; Description: Launch {#branding_application_desktop_name}; Flags: nowait postinstall skipifsilent; Components: plucker_desktop


;-----------------------------------------------------------------------------------------
[UninstallRun]
;-----------------------------------------------------------------------------------------
Filename: {app}\plucker_conduit\condinst.exe; Parameters: -u -s -r -e {#branding_creator_id}; Components: plucker_conduit
Filename: {reg:HKCU\Software\U.S. Robotics\Pilot Desktop\Core,HotSyncPath|C:\Palm\HotSync.exe}; Flags: nowait skipifdoesntexist; Components: plucker_conduit
Filename: {reg:HKCU\Software\Palm Computing\Pilot Desktop\Core,HotSyncPath|C:\Palm\HotSync.exe}; Flags: nowait skipifdoesntexist; Components: plucker_conduit


;-----------------------------------------------------------------------------------------
[InstallDelete]
;-----------------------------------------------------------------------------------------
; Legacy ones Python
Name: {app}\PyPlucker\helper\*.py?; Type: files
Name: {app}\PyPlucker\helper; Type: dirifempty
Name: {app}\PyPlucker\*.py?; Type: files
Name: {app}\PyPlucker; Type: dirifempty
Name: {app}\python\Lib\*.py?; Type: files
Name: {app}\python\Lib; Type: dirifempty
Name: {app}\python\PIL\*.py?; Type: files
Name: {app}\python\PIL; Type: dirifempty
Name: {app}\python\DLLs\*.pyd; Type: files
Name: {app}\python\DLLs\*.dll; Type: files
Name: {app}\python\DLLs; Type: dirifempty
Name: {app}\python; Type: dirifempty
; Legacy ones: plucker-desktop
Name: {app}\plucker_desktop\resource\grahics_dialog\*.png; Type: files
Name: {app}\plucker_desktop\resource\grahics_misc\*.png; Type: files
Name: {app}\plucker_desktop\resource\grahics_wizard\*.png; Type: files
Name: {app}\plucker_desktop\resource\html_about\*.png; Type: files
Name: {app}\plucker_desktop\resource\icons_dialog\*.png; Type: files
Name: {app}\plucker_desktop\resource\strings_tips\*.txt; Type: files
Name: {app}\plucker_desktop\resource\xrc\*.xrc; Type: files
; Legacy ones: rest
Name: {app}\palm\*.prc; Type: files
Name: {app}\palm\*.pdb; Type: files
Name: {app}\palm; Type: dirifempty
Name: {app}\Bmp2TBmp\Bmp2TBmp.exe; Type: files
Name: {app}\Bmp2TBmp; Type: dirifempty
Name: {app}\Conduit\Conduit.exe; Type: files
Name: {app}\Conduit; Type: dirifempty
Name: {app}\plucker-build.exe; Type: files
Name: {app}\RunSync.exe; Type: files


;-----------------------------------------------------------------------------------------
[_ISTool]
EnableISX=true
;-----------------------------------------------------------------------------------------


;-----------------------------------------------------------------------------------------
[Dirs]
;-----------------------------------------------------------------------------------------
Name: {app}\plucker_desktop
;Internationalization
#if branding_installer_components_internationalization == 1
Name: {app}\plucker_desktop\langs; Components: internationalization
Name: {app}\plucker_desktop\langs\ca; Components: internationalization
Name: {app}\plucker_desktop\langs\cs; Components: internationalization
Name: {app}\plucker_desktop\langs\da; Components: internationalization
Name: {app}\plucker_desktop\langs\de; Components: internationalization
Name: {app}\plucker_desktop\langs\es; Components: internationalization
Name: {app}\plucker_desktop\langs\fi; Components: internationalization
Name: {app}\plucker_desktop\langs\fo; Components: internationalization
Name: {app}\plucker_desktop\langs\fr; Components: internationalization
Name: {app}\plucker_desktop\langs\it; Components: internationalization
Name: {app}\plucker_desktop\langs\ja; Components: internationalization
Name: {app}\plucker_desktop\langs\nl; Components: internationalization
Name: {app}\plucker_desktop\langs\pl; Components: internationalization
Name: {app}\plucker_desktop\langs\ru; Components: internationalization
Name: {app}\plucker_desktop\langs\sv; Components: internationalization
#endif
Name: {app}\plucker_desktop\resource\graphics_dialog; Components: plucker_desktop
Name: {app}\plucker_desktop\resource\graphics_misc; Components: plucker_desktop
Name: {app}\plucker_desktop\resource\graphics_wizard; Components: plucker_desktop
Name: {app}\plucker_desktop\resource\icons_application; Components: plucker_desktop
Name: {app}\plucker_desktop\resource\icons_dialog; Components: plucker_desktop
Name: {app}\plucker_desktop\resource\icons_image_list; Components: plucker_desktop
Name: {app}\plucker_desktop\resource\plkrdata_installation; Components: plucker_desktop
Name: {app}\plucker_desktop\resource\plkrdata_showcase; Components: plucker_desktop
Name: {app}\plucker_desktop\resource\strings_tips; Components: plucker_desktop
Name: {app}\plucker_desktop\resource\xrc; Components: plucker_desktop

;-----------------------------------------------------------------------------------------
[Components]
;-----------------------------------------------------------------------------------------
Name: plucker_engine; Description: {#branding_application_suite_name} core engine; Types: full custom compact; Flags: fixed
Name: plucker_desktop; Description: {#branding_application_desktop_name}; Types: compact custom full
#if branding_installer_components_internationalization == 1
Name: internationalization; Description: {#branding_application_suite_name} internationalization support; Types: custom full
#endif
Name: plucker_conduit; Description: {#branding_application_suite_name} HotSync conduit; Types: custom compact full

;-----------------------------------------------------------------------------------------
[Registry]
;-----------------------------------------------------------------------------------------
; These 4 required keys are registry keys needed for Plucker
Root: HKLM; Subkey: SOFTWARE\The Plucker Team\Plucker; ValueType: string; ValueName: Path; ValueData: {app}\
Root: HKLM; SubKey: SOFTWARE\The Plucker Team\Plucker; ValueType: string; ValueName: RunSyncPath; ValueData: {app}\RunSync.exe
Root: HKLM; SubKey: SOFTWARE\The Plucker Team\Plucker; ValueType: string; ValueName: Version; ValueData: 1.4
Root: HKLM; SubKey: SOFTWARE\The Plucker Team\Plucker; ValueType: binary; ValueName: plucker-build_History; Flags: createvalueifdoesntexist

#if branding_installer_tasks_right_click_convert == 1
; Optional right-click explorer entry for HTML files.
Root: HKLM; SubKey: SOFTWARE\Classes\htmlfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_html_text_documents
Root: HKLM; SubKey: SOFTWARE\Classes\htmlfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Tasks: convert_html_text_documents; Flags: uninsdeletekey
Root: HKCR; SubKey: htmlfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Tasks: convert_html_text_documents; Flags: uninsdeletekey
Root: HKCR; SubKey: htmlfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Tasks: convert_html_text_documents; Flags: uninsdeletekey
; Optional right-click explorer entry for txt files.
Root: HKLM; SubKey: SOFTWARE\Classes\txtfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_html_text_documents
Root: HKLM; SubKey: SOFTWARE\Classes\txtfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Tasks: convert_html_text_documents; Flags: uninsdeletekey
Root: HKCR; SubKey: txtfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_html_text_documents
Root: HKCR; SubKey: txtfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_html_text_documents
; Optional right-click explorer entry for JPEG files.
Root: HKLM; SubKey: SOFTWARE\Classes\jpegfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKLM; SubKey: SOFTWARE\Classes\jpegfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: jpegfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: jpegfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey preservestringtype; Tasks: convert_image_documents
; Optional right-click explorer entry for GIF files.
Root: HKLM; SubKey: SOFTWARE\Classes\giffile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKLM; SubKey: SOFTWARE\Classes\giffile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: giffile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Tasks: convert_image_documents
Root: HKCR; SubKey: giffile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Tasks: convert_image_documents; Flags: uninsdeletekey
; Optional right-click explorer entry for PNG files.
Root: HKLM; SubKey: SOFTWARE\Classes\pngfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKLM; SubKey: SOFTWARE\Classes\pngfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: pngfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: pngfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\context-menu-convert.bat"" ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_image_documents
#endif

;-----------------------------------------------------------------------------------------
[INI]
;-----------------------------------------------------------------------------------------
; Sets the value of PLUCKERHOME for the right click conversion.
Filename: {app}\context-menu-convert.bat; Section: INSTALLER_SETTINGS; Key: SET PLUCKERHOME; String: {app}
; Will initially show the progress with a progress dialog instead of a DOS box
Filename: {app}\plucker.ini; Section: PLUCKER_DESKTOP; Key: spidering_display_mode; String: dialog
; If they installed internationalization, then turn it on by default.
#if branding_installer_components_internationalization == 1
Filename: {app}\plucker.ini; Section: PLUCKER_DESKTOP; Key: internationalization_enabled; String: 1; Components: internationalization
#endif
; If they install the Hotsync conduit, then they probably want autoupdate_mode set to 3
; Filename: {app}\plucker.ini; Section: PLUCKER_DAEMON; Key: autoupdate_mode; String: 3; Components: plucker_conduit

; DELME: Replace with a datestamp
Filename: {app}\plucker.ini; Section: PLUCKER_DESKTOP; Key: is_first_execution; String: 1

; These are for the branding of the app window, etc
; Name of the desktop application
Filename: {app}\plucker.ini; Section: PLUCKER_DESKTOP; Key: branding_application_desktop_name; String: {#branding_application_desktop_name}
; Name of the application suite
Filename: {app}\plucker.ini; Section: PLUCKER_DESKTOP; Key: branding_application_suite_name; String: {#branding_application_suite_name}
; Name of the publisher
Filename: {app}\plucker.ini; Section: PLUCKER_DESKTOP; Key: branding_application_publisher; String: {#branding_application_publisher}

Filename: {app}\plucker.ini; Section: DEFAULT; Key: zlib_compression; String: 1
Filename: {app}\plucker.ini; Section: DEFAULT; Key: compression; String: zlib
Filename: {app}\plucker.ini; Section: DEFAULT; Key: maxwidth; String: 150
Filename: {app}\plucker.ini; Section: DEFAULT; Key: maxheight; String: 250
Filename: {app}\plucker.ini; Section: DEFAULT; Key: alt_maxwidth; String: 1000000
Filename: {app}\plucker.ini; Section: DEFAULT; Key: alt_maxheight; String: 1000000
Filename: {app}\plucker.ini; Section: DEFAULT; Key: image_parser; String: pil2
Filename: {app}\plucker.ini; Section: DEFAULT; Key: verbosity; String: 2
Filename: {app}\plucker.ini; Section: DEFAULT; Key: home_maxdepth; String: 2
Filename: {app}\plucker.ini; Section: DEFAULT; Key: home_stayonhost; String: 0
Filename: {app}\plucker.ini; Section: DEFAULT; Key: exclusion_lists; String: 
Filename: {app}\plucker.ini; Section: DEFAULT; Key: no_url_info; String: 1
Filename: {app}\plucker.ini; Section: DEFAULT; Key: copyprevention_bit; String: 0
Filename: {app}\plucker.ini; Section: DEFAULT; Key: backup_bit; String: 0
Filename: {app}\plucker.ini; Section: DEFAULT; Key: launchable_bit; String: 0
Filename: {app}\plucker.ini; Section: DEFAULT; Key: bpp; String: 1
Filename: {app}\plucker.ini; Section: DEFAULT; Key: image_compression_limit; String: 50
Filename: {app}\plucker.ini; Section: DEFAULT; Key: image_parser; String: pil2
; This is probably legacy as of Plucker 1.4, leaving it in for now.
Filename: {app}\plucker.ini; Section: WINDOWS; Key: group_path; String: {group}
Filename: {app}\plucker.ini; Section: WINDOWS; Key: use_conduit; String: 1
; These are legacy as of Plucker 1.4
;Filename: {app}\plucker.ini; Section: WINDOWS; Key: bmp_to_tbmp; String: {app}\Bmp2TBmp\Bmp2TBmp.exe
;Filename: {app}\plucker.ini; Section: WINDOWS; Key: bmp_to_tbmp_parameter; String: -i=%input% -o=%output% -maxwidth=%maxwidth% -maxheight=%maxheight% -compress=%compress% -bpp=%colors%
;Filename: {app}\plucker.ini; Section: WINDOWS; Key: tbmp_compression_type; String: yes
; Question mark of whether can remove this next python program as of Plucker 1.4. Some probs reported.
Filename: {app}\plucker.ini; Section: WINDOWS; Key: python_program; String: {app}\parser\python\vm\python.exe
Filename: {app}\plucker.ini; Section: WINDOWS; Key: close_on_exit; String: 1
Filename: {app}\plucker.ini; Section: WINDOWS; Key: try_reduce_bpp; String: 1
Filename: {app}\plucker.ini; Section: WINDOWS; Key: try_reduce_dimension; String: 0
Filename: {app}\plucker.ini; Section: CONTEXT_DEFAULT; Key: home_stayonhost; String: true
Filename: {app}\plucker.ini; Section: CONTEXT_DEFAULT; Key: bpp; String: 0

;-----------------------------------------------------------------------------------------
[UninstallDelete]
;-----------------------------------------------------------------------------------------
Name: {app}\Plucker Desktop.url; Type: files
Name: {app}\plucker_desktop\resource\doc\plucker-desktop-help.ini; Type: files
Name: {app}\plucker_desktop\resource\doc; Type: dirifempty
Name: {app}\plucker_desktop\resource; Type: dirifempty
Name: {app}\plucker_desktop; Type: dirifempty
Name: {app}\plucker_conduit\*; Type: files
Name: {app}\plucker_conduit; Type: dirifempty
Name: {app}\parser\python\PyPlucker\helper\*.py?; Type: files
Name: {app}\parser\python\PyPlucker\helper; Type: dirifempty
Name: {app}\parser\python\PyPlucker\*.py?; Type: files
Name: {app}\parser\python\PyPlucker; Type: dirifempty
Name: {app}\parser\python\vm\Lib\*.py?; Type: files
Name: {app}\parser\python\vm\Lib; Type: dirifempty
Name: {app}\parser\python\vm\PIL\*.py?; Type: files
Name: {app}\parser\python\vm\PIL; Type: dirifempty
Name: {app}\parser\python\vm\DLLs\*.pyd; Type: files
Name: {app}\parser\python\vm\DLLs\*.dll; Type: files
Name: {app}\parser\python\vm\DLLs; Type: dirifempty
Name: {app}\parser\python\vm; Type: dirifempty
; Legacy ones Python. Note the "legacy" ones here can be removed, since they were
; uninstalled when this installer was run (see the InstallDelete section)
Name: {app}\PyPlucker\helper\*.py?; Type: files
Name: {app}\PyPlucker\helper; Type: dirifempty
Name: {app}\PyPlucker\*.py?; Type: files
Name: {app}\PyPlucker; Type: dirifempty
Name: {app}\python\Lib\*.py?; Type: files
Name: {app}\python\Lib; Type: dirifempty
Name: {app}\python\PIL\*.py?; Type: files
Name: {app}\python\PIL; Type: dirifempty
Name: {app}\python\DLLs\*.pyd; Type: files
Name: {app}\python\DLLs\*.dll; Type: files
Name: {app}\python\DLLs; Type: dirifempty
Name: {app}\python; Type: dirifempty
; Legacy ones: plucker-desktop
Name: {app}\plucker_desktop\resource\grahics_dialog\*.png; Type: files
Name: {app}\plucker_desktop\resource\grahics_misc\*.png; Type: files
Name: {app}\plucker_desktop\resource\grahics_wizard\*.png; Type: files
Name: {app}\plucker_desktop\resource\html_about\*.png; Type: files
Name: {app}\plucker_desktop\resource\icons_dialog\*.png; Type: files
Name: {app}\plucker_desktop\resource\strings_tips\*.txt; Type: files
Name: {app}\plucker_desktop\resource\xrc\*.xrc; Type: files
; Legacy ones: rest
Name: {app}\Bmp2TBmp\Bmp2TBmp.exe; Type: files
Name: {app}\Bmp2TBmp; Type: dirifempty
Name: {app}\Conduit\Conduit.exe; Type: files
Name: {app}\Conduit; Type: dirifempty
Name: {app}\plucker-build.exe; Type: files
Name: {app}\RunSync.exe; Type: files
Name: {app}\palm\*.prc; Type: files
Name: {app}\palm\*.pdb; Type: files
; Must be the last one listed
Name: {app}; Type: dirifempty

