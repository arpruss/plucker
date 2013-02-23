;-----------------------------------------------------------------------------------------
; Name:        plucker-desktop.iss
; Purpose:     Inno Setup (www.innosetup.com) file for Plucker Desktop
;              Note: requires InnoSetupExtensions for compilation, including the bundled
;              InnoSetup PreProcesor (ISPP).
; Author:      Robert O'Connor
; Modified by:
; Created:     2002/05/30
; Copyright:   (c) Robert O'Connor ( rob@medicalmnemonics.com )
; Licence:     GPL
; RCS-ID:      $Id: plucker-desktop_BACKUP_PRE_PYTHON_REARRANGE.iss,v 1.1 2003/07/27 22:08:20 robertoconnor Exp $
;-----------------------------------------------------------------------------------------

;-----------------------------------------------------------------------------------------
; Begin Editable defines section
;-----------------------------------------------------------------------------------------

#define compilation_path_to_msw_package                      	= "..\..\..\..\..\plucker_msw_package"
#define compilation_path_to_msw_package_dont_overwrite          = "..\..\..\..\..\plucker_msw_package_dont_overwrite"

; 1 to include the borland dlls.
#define compilation_include_borland_dlls                      = 1

#include "customization_defines.h"

;-----------------------------------------------------------------------------------------
; End Editable defines section: you shouldn't want to change the stuff below here.
;-----------------------------------------------------------------------------------------

[Setup]
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
OutputBaseFilename={#branding_installer_root_filename}
DisableStartupPrompt=true
; The directory to place uninstall files. Never change this value or else
; can't find uninstall files from a previous install to overwrite.
UninstallFilesDir={app}\uninstall

LicenseFile=..\..\docs\GPL_LICENSE.txt
; Using bzip instead of zip will make a smaller package, but its too slow, only saves 200K.
; Compression=bzip
#if ( branding_installer_show_before_install_message )
InfoAfterFile=before_install_message.txt
#endif
#if ( branding_installer_show_after_install_message )
InfoBeforeFile=after_install_message.txt
#endif

; I wonder if I should use this key. It will give an error if user doesn't have
; admin privleges when they install.
; AdminPrivilegesRequired=true

; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51


[Tasks]
;Desktop icon
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:; MinVersion: 4,4; Components: plucker_desktop
;Right click conversion
#if ( branding_installer_tasks_right_click_convert )
Name: convert_html_text_documents; Description: Add 'Convert To {#branding_application_suite_name}' menu item for &HTML and text files (htm, html, txt); GroupDescription: Windows Explorer right-click menu integration:; Flags: unchecked
Name: convert_image_documents; Description: Add 'Convert To {#branding_application_suite_name}' menu item for &image files (png, gif, jpg); GroupDescription: Windows Explorer right-click menu integration:; Flags: unchecked
#endif
;Sample channels
#if ( branding_installer_allow_not_setup_default_channels	)
Name: sample_channels; Description: Setup &default initial {#branding_application_suite_name} channels; GroupDescription: Automatic setup of initial channels:
#endif


[Files]
Source: ..\..\plucker-desktop.exe; DestDir: {app}\plucker_desktop; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\plucker-desktop.exe.manifest; DestDir: {app}\plucker_desktop; CopyMode: alwaysoverwrite; Components: plucker_desktop
#if ( compilation_include_borland_dlls )
Source: ..\..\wx32d.dll; DestDir: {app}\plucker_desktop; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\cc3250mt.dll; DestDir: {app}\plucker_desktop; CopyMode: alwaysoverwrite; Components: plucker_desktop
#endif
Source: ..\..\docs\TODO.txt; DestDir: {app}\plucker_desktop\; Components: plucker_desktop
Source: ..\..\docs\PLUCKER_DESKTOP_CHANGELOG.txt; DestDir: {app}\plucker_desktop; Components: plucker_desktop
Source: ..\..\README.txt; DestDir: {app}\plucker_desktop; Components: plucker_desktop
Source: ..\..\resource\graphics_dialog\*.*; DestDir: {app}\plucker_desktop\resource\graphics_dialog; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\resource\graphics_misc\*.*; DestDir: {app}\plucker_desktop\resource\graphics_misc; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\resource\graphics_wizard\*.*; DestDir: {app}\plucker_desktop\resource\graphics_wizard; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\resource\icons_application\*.*; DestDir: {app}\plucker_desktop\resource\icons_application; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\resource\icons_dialog\*.*; DestDir: {app}\plucker_desktop\resource\icons_dialog; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\resource\icons_image_list\*.*; DestDir: {app}\plucker_desktop\resource\icons_image_list; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\resource\strings_tips\*.*; DestDir: {app}\plucker_desktop\resource\strings_tips; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\resource\xrc\*.*; DestDir: {app}\plucker_desktop\resource\xrc; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\docs\GPL_LICENSE.txt; DestDir: {app}\doc; CopyMode: normal
Source: ..\..\resource\icons_art_provider\*.*; DestDir: {app}\plucker_desktop\resource\icons_art_provider; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\resource\html_about\*.*; DestDir: {app}\plucker_desktop\resource\html_about; CopyMode: alwaysoverwrite; Components: plucker_desktop
Source: ..\..\resource\docs\plucker-desktop.htb; DestDir: {app}\doc; CopyMode: alwaysoverwrite; Components: plucker_desktop
#if ( branding_installer_include_full_plucker_manual )
Source: ..\..\resource\docs\plucker-manual.htb; DestDir: {app}\doc; CopyMode: alwaysoverwrite
#endif
; Settings: change task based on whether or not allowed not install during setup questions
#if ( branding_installer_allow_not_setup_default_channels	)
Source: ..\..\resource\plkrdata_installation\automatically_installed_channels.plkrdata; DestDir: {app}\plucker_desktop\resource\plkrdata_installation; CopyMode: alwaysoverwrite; Tasks: sample_channels
#else
Source: ..\..\resource\plkrdata_installation\automatically_installed_channels.plkrdata; DestDir: {app}\plucker_desktop\resource\plkrdata_installation; CopyMode: alwaysoverwrite;
#endif
Source: ..\..\resource\plkrdata_installation\automatically_installed_settings.plkrdata; DestDir: {app}\plucker_desktop\resource\plkrdata_installation; CopyMode: alwaysoverwrite;
; Showcase file. Has to be the specific file though.
Source: ..\..\resource\plkrdata_showcase\*.*; DestDir: {app}\plucker_desktop\resource\plkrdata_showcase; CopyMode: alwaysoverwrite; Components: plucker_desktop
; Setupwizard file: these channels will be optionally installed. Has to be the specific file though.
Source: ..\..\resource\plkrdata_setup_wizard\*.*; DestDir: {app}\plucker_desktop\resource\plkrdata_setup_wizard; CopyMode: alwaysoverwrite;

;Internationalization
#if ( branding_installer_components_internationalization )
Source: ..\..\langs\cs\*.*; DestDir: {app}\plucker_desktop\langs\cs; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\da\*.*; DestDir: {app}\plucker_desktop\langs\da; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\de\*.*; DestDir: {app}\plucker_desktop\langs\de; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\es\*.*; DestDir: {app}\plucker_desktop\langs\es; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\fi\*.*; DestDir: {app}\plucker_desktop\langs\fi; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\fo\*.*; DestDir: {app}\plucker_desktop\langs\fo; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\fr\*.*; DestDir: {app}\plucker_desktop\langs\fr; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\it\*.*; DestDir: {app}\plucker_desktop\langs\it; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\ja\*.*; DestDir: {app}\plucker_desktop\langs\ja; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\nl\*.*; DestDir: {app}\plucker_desktop\langs\nl; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\pl\*.*; DestDir: {app}\plucker_desktop\langs\pl; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\ru\*.*; DestDir: {app}\plucker_desktop\langs\ru; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\sv\*.*; DestDir: {app}\plucker_desktop\langs\sv; CopyMode: alwaysoverwrite; Components: internationalization
Source: ..\..\langs\*.txt; DestDir: {app}\plucker_desktop\langs; CopyMode: alwaysoverwrite; Components: internationalization
#endif
; Plucker core engine items
Source: {#compilation_path_to_msw_package}\*.*; DestDir: {app}; CopyMode: alwaysoverwrite
Source: {#compilation_path_to_msw_package}\Bmp2TBmp\*.*; DestDir: {app}\Bmp2Tbmp; CopyMode: alwaysoverwrite; Components: plucker_engine
Source: {#compilation_path_to_msw_package}\Conduit\*.*; DestDir: {app}\Conduit; CopyMode: alwaysoverwrite; Components: plucker_engine
; Plucker python parser
Source: {#compilation_path_to_msw_package}\PyPlucker\*.*; Flags: recursesubdirs; DestDir: {app}\PyPlucker; CopyMode: alwaysoverwrite; Components: plucker_engine
; Python distribution
Source: {#compilation_path_to_msw_package}\python\*.*; Flags: recursesubdirs; DestDir: {app}\python; CopyMode: alwaysoverwrite; Components: plucker_engine
; Plucker conduit files
Source: {#compilation_path_to_msw_package}\plucker_conduit\*.*; DestDir: {app}\plucker_conduit; Components: plucker_conduit; CopyMode: alwaysoverwrite
; Plucker standalone documentation viewer
Source: {#compilation_path_to_msw_package}\doc\*.*; DestDir: {app}\doc; CopyMode: alwaysoverwrite
; Palm software
#if ( branding_installer_components_internationalization )
Source: {#compilation_path_to_msw_package}\palm\*.*; DestDir: {app}\palm; CopyMode: alwaysoverwrite; Components: internationalization
#else
Source: {#compilation_path_to_msw_package}\palm\SysZLib.prc; DestDir: {app}\palm; CopyMode: alwaysoverwrite
Source: {#compilation_path_to_msw_package}\palm\PluckerUserGuide.pdb; DestDir: {app}\palm; CopyMode: alwaysoverwrite
Source: {#compilation_path_to_msw_package}\palm\viewer_en.prc; DestDir: {app}\palm; CopyMode: alwaysoverwrite
Source: {#compilation_path_to_msw_package}\palm\viewer_hires_en.prc; DestDir: {app}\palm; CopyMode: alwaysoverwrite
#endif
; I put the files that I don't want to overwrite, into a separate directory
Source: {#compilation_path_to_msw_package_dont_overwrite}\plucker.ini; DestDir: {app}; CopyMode: onlyifdoesntexist; Flags: uninsneveruninstall

[Icons]
;This one is the icon on the desktop
Name: {userdesktop}\{#branding_application_desktop_name}; Filename: {app}\plucker_desktop\plucker-desktop.exe; MinVersion: 4,4; Tasks: desktopicon; WorkingDir: {app}\plucker_desktop; IconFilename: {app}\plucker_desktop\resource\icons_application\application.ico; IconIndex: 0; Components: plucker_desktop
;This one is to put a shortcut to Plucker Desktop in the Plucker Home directory.
Name: {app}\{#branding_application_desktop_name}; Filename: {app}\plucker_desktop\plucker-desktop.exe; WorkingDir: {app}; IconIndex: 0; IconFilename: {app}\plucker_desktop\resource\icons_application\application.ico
;This ones are the ones in the Start Menu
Name: {group}\{#branding_application_desktop_name}; Filename: {app}\plucker_desktop\plucker-desktop.exe; IconIndex: 0; WorkingDir: {app}\plucker_desktop; IconFilename: {app}\plucker_desktop\resource\icons_application\application.ico; Components: plucker_desktop
Name: {group}\Documentation\{#branding_application_desktop_name} Help; Filename: {app}\doc\helpview.exe; IconIndex: 0; Parameters: """{app}\doc\plucker-desktop.htb"""
#if ( branding_installer_startmenu_plucker_build )
Name: {group}\Tools\Command-line {#branding_application_suite_name}; Filename: {app}\plucker-build.exe; WorkingDir: {app}; IconFilename: {app}\Install.exe; IconIndex: 0; Components: plucker_engine
#endif
Name: {group}\Uninstall {#branding_application_suite_name}; Filename: {uninstallexe}; IconIndex: 0
#if ( branding_installer_include_full_plucker_manual )
Name: {group}\Documentation\Full {#branding_application_suite_name} Manual; Filename: {app}\doc\helpview.exe; IconIndex: 0; Parameters: """{app}\doc\plucker-manual.htb"""
#endif

[Run]
Filename: {app}\plucker_conduit\condinst.exe; Parameters: -install -creator Plkr -name {#branding_application_suite_name} Conduit -title {#branding_application_suite_name} -conduit {app}\plucker_conduit\PluckerCond.dll; Components: plucker_conduit; Flags: waituntilidle
Filename: {reg:HKCU\Software\U.S. Robotics\Pilot Desktop\Core,HotSyncPath|C:\Palm\HotSync.exe}; Flags: nowait skipifdoesntexist; Components: plucker_conduit
Filename: {reg:HKCU\Software\Palm Computing\Pilot Desktop\Core,HotSyncPath|C:\Palm\HotSync.exe}; Flags: nowait skipifdoesntexist; Components: plucker_conduit
Filename: {app}\plucker_desktop\plucker-desktop.exe; Description: Launch {#branding_application_desktop_name}; Flags: nowait postinstall skipifsilent; Components: plucker_desktop


[UninstallRun]
Filename: {app}\plucker_conduit\condinst.exe; Parameters: -uninstall -creator Plkr; Components: plucker_conduit
Filename: {reg:HKCU\Software\U.S. Robotics\Pilot Desktop\Core,HotSyncPath|C:\Palm\HotSync.exe}; Flags: nowait skipifdoesntexist; Components: plucker_conduit
Filename: {reg:HKCU\Software\Palm Computing\Pilot Desktop\Core,HotSyncPath|C:\Palm\HotSync.exe}; Flags: nowait skipifdoesntexist; Components: plucker_conduit


[_ISTool]
EnableISX=true


[Dirs]
Name: {app}\plucker_desktop
;Internationalization
#if ( branding_installer_components_internationalization )
Name: {app}\plucker_desktop\langs; Components: internationalization
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


[Components]
Name: plucker_engine; Description: {#branding_application_suite_name} core engine; Types: full custom compact; Flags: fixed
Name: plucker_desktop; Description: {#branding_application_desktop_name}; Types: compact custom full
#if ( branding_installer_components_internationalization )
Name: internationalization; Description: {#branding_application_suite_name} internationalization support; Types: custom full
#endif
Name: plucker_conduit; Description: {#branding_application_suite_name} HotSync conduit; Types: custom compact full


[Registry]
; These 4 required keys are registry keys needed for Plucker
Root: HKCU; Subkey: Software\The Plucker Team\Plucker; ValueType: string; ValueName: Path; ValueData: {app}\
Root: HKCU; SubKey: Software\The Plucker Team\Plucker; ValueType: string; ValueName: RunSyncPath; ValueData: {app}\RunSync.exe
Root: HKCU; SubKey: Software\The Plucker Team\Plucker; ValueType: string; ValueName: Version; ValueData: 1.2
Root: HKCU; SubKey: Software\The Plucker Team\Plucker; ValueType: binary; ValueName: plucker-build_History; Flags: createvalueifdoesntexist

#if ( branding_installer_tasks_right_click_convert )
; Optional right-click explorer entry for HTML files.
Root: HKLM; SubKey: SOFTWARE\Classes\htmlfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_html_text_documents
Root: HKLM; SubKey: SOFTWARE\Classes\htmlfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Tasks: convert_html_text_documents; Flags: uninsdeletekey
Root: HKCR; SubKey: htmlfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Tasks: convert_html_text_documents; Flags: uninsdeletekey
Root: HKCR; SubKey: htmlfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Tasks: convert_html_text_documents; Flags: uninsdeletekey
; Optional right-click explorer entry for txt files.
Root: HKLM; SubKey: SOFTWARE\Classes\txtfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_html_text_documents
Root: HKLM; SubKey: SOFTWARE\Classes\txtfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Tasks: convert_html_text_documents; Flags: uninsdeletekey
Root: HKCR; SubKey: txtfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_html_text_documents
Root: HKCR; SubKey: txtfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_html_text_documents
; Optional right-click explorer entry for JPEG files.
Root: HKLM; SubKey: SOFTWARE\Classes\jpegfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKLM; SubKey: SOFTWARE\Classes\jpegfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: jpegfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: jpegfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe\"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey preservestringtype; Tasks: convert_image_documents
; Optional right-click explorer entry for GIF files.
Root: HKLM; SubKey: SOFTWARE\Classes\giffile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKLM; SubKey: SOFTWARE\Classes\giffile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: giffile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Tasks: convert_image_documents
Root: HKCR; SubKey: giffile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe\"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Tasks: convert_image_documents; Flags: uninsdeletekey
; Optional right-click explorer entry for PNG files.
Root: HKLM; SubKey: SOFTWARE\Classes\pngfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKLM; SubKey: SOFTWARE\Classes\pngfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: pngfile\shell\PlkrConvert; ValueType: string; ValueData: Convert to Plucker; Flags: uninsdeletekey; Tasks: convert_image_documents
Root: HKCR; SubKey: pngfile\shell\PlkrConvert\command; ValueType: string; ValueData: """{app}\plucker-build.exe"" -H file:""%1"" -f ""%1"" -s CONTEXT_DEFAULT"; Flags: uninsdeletekey; Tasks: convert_image_documents
#endif


[INI]
; Will initially show the progress with a progress dialog instead of a DOS box
Filename: {app}\plucker.ini; Section: PLUCKER_DESKTOP; Key: spidering_display_mode; String: dialog
; If they installed internationalization, then turn it on by default.
#if ( branding_installer_components_internationalization )
Filename: {app}\plucker.ini; Section: PLUCKER_DESKTOP; Key: internationalization_enabled; String: 1; Components: internationalization
#endif
; Default channel setup (will be removed from here)
#if ( branding_installer_allow_not_setup_default_channels )
#endif
; If they install the Hotsync conduit, then they probably want autoupdate_mode set to 3
; Filename: {app}\plucker.ini; Section: PLUCKER_DAEMON; Key: autoupdate_mode; String: 3; Components: plucker_conduit

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
Filename: {app}\plucker.ini; Section: WINDOWS; Key: group_path; String: {group}
Filename: {app}\plucker.ini; Section: WINDOWS; Key: bmp_to_tbmp; String: {app}\Bmp2TBmp\Bmp2TBmp.exe
Filename: {app}\plucker.ini; Section: WINDOWS; Key: bmp_to_tbmp_parameter; String: -i=%input% -o=%output% -maxwidth=%maxwidth% -maxheight=%maxheight% -compress=%compress% -bpp=%colors%
Filename: {app}\plucker.ini; Section: WINDOWS; Key: tbmp_compression_type; String: yes
Filename: {app}\plucker.ini; Section: WINDOWS; Key: use_conduit; String: 1
Filename: {app}\plucker.ini; Section: WINDOWS; Key: python_program; String: {app}\Python\python.exe
Filename: {app}\plucker.ini; Section: WINDOWS; Key: close_on_exit; String: 1
Filename: {app}\plucker.ini; Section: WINDOWS; Key: try_reduce_bpp; String: 1
Filename: {app}\plucker.ini; Section: WINDOWS; Key: try_reduce_dimension; String: 0
Filename: {app}\plucker.ini; Section: CONTEXT_DEFAULT; Key: home_stayonhost; String: true
Filename: {app}\plucker.ini; Section: CONTEXT_DEFAULT; Key: bpp; String: 0


[UninstallDelete]
Name: {app}\Plucker Desktop.url; Type: files
Name: {app}\plucker_desktop\resource\doc\plucker-desktop-help.ini; Type: files
Name: {app}\plucker_desktop\resource\doc; Type: dirifempty
Name: {app}\plucker_desktop\resource; Type: dirifempty
Name: {app}\plucker_desktop; Type: dirifempty
Name: {app}\PyPlucker\helper\*.py?; Type: files
Name: {app}\PyPlucker\helper; Type: dirifempty
Name: {app}\PyPlucker\*.py?; Type: files
Name: {app}\PyPlucker; Type: dirifempty
Name: {app}\python\Lib\*.py?; Type: files
Name: {app}\python\Lib; Type: dirifempty
Name: {app}\python\PIL\*.py?; Type: files
Name: {app}\python\PIL; Type: dirifempty
Name: {app}\python; Type: dirifempty
Name: {app}\plucker_conduit\*; Type: files
Name: {app}\plucker_conduit; Type: dirifempty
; Must be the last one listed
Name: {app}; Type: dirifempty

