//----------------------------------------------------------------------------------------
/*!
    \file        plucker_defines.h    
    This file contains defines, that get included into all files.    
    See setup.h for the defines that turn on/off compilation of certain sections.     
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Plucker Desktop defines   
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: plucker_defines.h,v 1.50 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PLUCKER_DEFINES_H_
#define _PLUCKER_DEFINES_H_

//----------------------------------------------------------------------------------------
// Compilation setup include (so source files only have to include plucker_defines.h)
//----------------------------------------------------------------------------------------

#include "setup.h"

/*!  
    \addtogroup plkr_defines
    \{
 */

//----------------------------------------------------------------------------------------
// General
//----------------------------------------------------------------------------------------

//! Name of the desktop application [default] only used in plkrAPPLICATION_DESKTOP_NAME.
#define plkrAPPLICATION_DESKTOP_NAME_DEFAULT                _( "Plucker Desktop" )

//! Name of the application suite [default] only used in plkrAPPLICATION_SUITE_NAME.
#define plkrAPPLICATION_SUITE_NAME_DEFAULT                  _( "Plucker" )

//! Name of the publisher [default] only used in plkrAPPLICATION_PUBLISHER.
#define plkrAPPLICATION_PUBLISHER_DEFAULT                   _( "The Plucker Team" )

//! Name of the desktop application [reads from config file].
#define plkrAPPLICATION_DESKTOP_NAME       the_configuration->Read(_("/PLUCKER_DESKTOP/branding_application_desktop_name"), plkrAPPLICATION_DESKTOP_NAME_DEFAULT )

//! Name of the application suite [reads from config file].
#define plkrAPPLICATION_SUITE_NAME         the_configuration->Read(_("/PLUCKER_DESKTOP/branding_application_suite_name"), plkrAPPLICATION_SUITE_NAME_DEFAULT )

//! Name of the publisher [reads from config file.
#define plkrAPPLICATION_PUBLISHER          the_configuration->Read(_("/PLUCKER_DESKTOP/branding_application_publisher"), plkrAPPLICATION_PUBLISHER_DEFAULT )

//! Version of the application.
/*! The version string, currently written to the config file, so can tell if upgraded.
    Also showed in the about screen.
    Rebuild configuration class and main_frame class for it to update.
    First three digits are the Plucker version, last one is any Plucker Desktop updates.
 */
#define plkrPLUCKER_DESKTOP_VERSION_STRING          wxT( "1.8.0.0" )

//----------------------------------------------------------------------------------------
// Configuration file (pluckerrc / plucker.ini)
//----------------------------------------------------------------------------------------

//! The separator used for multiple directories in a key
/*!
    The separator is OS specific, since have to choose a character that cannot
    possibly be in a path on that OS. GTK uses a forward slash, Mac uses a colon, and
    MSW uses a backslash.
 */
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
    #define plkrOS_DIR_SEPARATOR                     '/'
#endif
#ifdef __WXMAC__
    #define plkrOS_DIR_SEPARATOR                     ':'
#endif
#ifdef __WXMSW__
    #define plkrOS_DIR_SEPARATOR                    '\\'
#endif

//! The separator used for multiple files in a key
/*!
    The separator is OS specific, since have to choose a character that cannot
    possibly be in a filename on that OS. GTK uses ':'. All others use ';'.
 */
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
    #define plkrOS_FILE_ARRAY_SEPARATOR             ':'
#else
    #define plkrOS_FILE_ARRAY_SEPARATOR             ';'
#endif

//! Length of a section name in characters before appending a unique number.
/*! The number is appended to make it unique if the base name already exists.

    For example if 'UnnamedChannel' already exists then the new one will be
    UnnamedChannel1. This sets the length that the 'UnnamedChannel' part can
    be, before it is truncated, and a number appended.

    The length is such that it can have very large numbers before exceeding 31
    characters (a MacOS concern)
 */
#define plkrMAXIMUM_BASE_SECTION_LENGTH             25

//! Maximum length of a channel name in characters.
/*! PalmOS can only allow a database name of 31 characters, otherwise it
    won't HotSync. The maximum number of characters for a doc_name key
    entry is therefore 31 characters. However, need to leave 5 characters
    to leave room for Plkr- in metarecord, so can be kept unique.
    So max is actually 26.
 */
#define plkrMAXIMUM_CHANNEL_NAME_LENGTH             26

//!Name of section to store the template for new channels
#define plkrNEW_CHANNEL_TEMPLATE_SECTION            wxT( "NEW_CHANNEL_TEMPLATE" )

//! Default name of channels that don't have a name yet.
#define plkrUNNAMED_CHANNEL_NAME                    _( "Unnamed channel" )

//! Sections of the configuration file that aren't channels
/*
	[PLUCKER_DAEMON] is reserved for possible future use.
*/
#define plkrRESERVED_CONFIGURATION_SECTIONS wxT( "DEFAULT" ), wxT( "POSIX" ), wxT( "OS2" ), wxT( "WINDOWS" ), wxT( "MACINTOSH" ), wxT( "PLUCKER_DESKTOP" ), wxT( "PLUCKER_DAEMON" ), wxT( "CONTEXT_DEFAULT" ), plkrNEW_CHANNEL_TEMPLATE_SECTION

//! A placeholder in a user-entered command string that gets replaced with the filename.
/*!
    For example in the open external editor command, if user entered "vim plkrFILE -s"
    into the edit box and his file start page is somefile.html Plucker Desktop will
    execute "vim somefile.html -s".

    If not included in the command string, it will be appended automatically at the 
    end. 
 */
#define plkrSHELL_COMMAND_REPLACEMENT_FILENAME      wxT( "plkrFILE")

//! A placeholder in a string that gets replaced with a file-friendly datetimestap.
/*!
    For example in the process_progress_listbox. 
 */
#define plkrFILE_FRIENDLY_DATETIME_STAMP            wxT( "plkrFILE_FRIENDLY_DATETIME_STAMP")


//----------------------------------------------------------------------------------------
// Strings
//----------------------------------------------------------------------------------------

//! EOL (End Of Line) character used on the OS.
/*!
    Used for example in copying some items to clipboard.
 */
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
    #define plkrEOL_CHARACTER                       wxT( "\n")
#endif  
#ifdef __WXMAC__
    #define plkrEOL_CHARACTER                       wxT( "\r")
#endif  
#ifdef __WXMSW__
    #define plkrEOL_CHARACTER                       wxT( "\r\n")
#endif 

//----------------------------------------------------------------------------------------
// Controls
//----------------------------------------------------------------------------------------

//! Number of pixels extra to leave besides the scrollbar width
/*!
    The extra pixels are used when sizing columns to fill the whole width of the 
    listctrl. 
   
    Note in GTK on the showcase wherer there is a boatload of hidden columns, there is 
    a small horizontal scrollbar.
    \todo Based on number of columns?
 */
#define plkrLISTCTRL_EXTRA_WIDTH_PADDING            9

//! Normal number of pixels padding a control inside a sizer, in the xrc resources.
/*!
    Used in the progress_dialog for example.
 */
#define plkrSTANDARD_SIZER_PADDING                  5

//! A fiexed range value for gauges. Prevents flicker if need to change both range/value.
/*!
    By using a fixed range size, then calculating a percentage of it for the value, can
    stop the flicker of changing both range and value if they keep changing (as is the
    case in the progress_dialog ).
 */
#define plkrFIXED_PERCENTAGE_GAUGE_RANGE            1000

//! Minimum width in pixels of a wizard 'content' area 
/*! 
    This width does not including the side bitmap, or frame padding, etc. *
  
    Used in plucker_wizard_base class.
 */
#define plkrMINIMUM_WIZARD_WIDTH                    320

//! Minimum height in pixels of a wizard 'content' area.
/*!
    This height does not include the bottom Next/Prev buttons, etc).
    If the side bitmap exceeds this value, it will be set to the bitmap height.
  
    Used in plucker_wizard_base class.
 */
#define plkrMINIMUM_WIZARD_HEIGHT                   250

//! Width of the HTML content window component of a html_tip_dialog
#define plkrHTML_TIP_DIALOG_CONTENT_WIDTH           300

//! Width of the HTML content window component of a html_tip_dialog
#define plkrHTML_TIP_DIALOG_CONTENT_HEIGHT          250

//! The default size of a textctrl font if making a custom font.
/*!
 *  The GTK one has to be bigger than MSW, else unreadable.
   
    Used for example in the editor_dialog.
 */
#ifdef __WXMSW__
    #define plkrDEFAULT_TEXTCTRL_FONT_POINTSIZE     10
#else
    #define plkrDEFAULT_TEXTCTRL_FONT_POINTSIZE     14
#endif

//! Usually use the OS's GUI color, elsewheree, this is the lighter color string to use.
#define  plkr_GUI_LIGHT_COLOR_RGB_STRING           wxT( "#3399CC" )

//! Usually use the OS's GUI color, elsewhere, this is the darker color string to use.
#define  plkrGUI_ELEMENT_DARK_COLOR_RGB_STRING     wxT( "#333366" )

//----------------------------------------------------------------------------------------
// Date and time
//----------------------------------------------------------------------------------------

//! Date format to use. Example: 1994-11-05T08:15:30
/*!
    This is the date format that is recommended by the W3C ( www.w3c.org ).
  
    This is the string that is read and written to the configuration file. The format is
    strict. 
    
    Note that is a capital Y for 4 digit date (lowercase is 2 digit).
 */
#define plkrDATE_TIME_FORMAT                        wxT( "%Y-%m-%dT%H:%M:%S" )

//! The format of the component time. Example: 08:15:30
/*!
    Obviously matching the format of the plkrDATE_TIME_FORMAT .
 */
#define plkrDATE_FORMAT                             wxT( "%Y-%m-%d" )

//! The format of the component time. Example: 1994-11-05
/*!
    Obviously matching the format of the plkrDATE_TIME_FORMAT .
 */
#define plkrTIME_FORMAT                             wxT( "%H:%M:%S")

//! A prettier version of DateTime, used only for display only
/*!
    This is display and display alone, not i/o to configuration file.
  
    Example usage: in the main_listctrl "Due" column.
 */
#define plkrPRETTY_COMPACT_DATE_TIME_FORMAT         wxT( "%b %d  %H:%M")

//! handheld_dest_type values for handheld_target_storage_mode
/*! 
    Used when deciding what to do to send to card/RAM when installing.
 */
enum {
    //! Install to RAM.
    plkrHANDHELD_TARGET_STORAGE_MODE_RAM                    = 0,
    //! Install to SD card.
    plkrHANDHELD_TARGET_STORAGE_MODE_SD_CARD,
    //! Install to Memory Stick
    plkrHANDHELD_TARGET_STORAGE_MODE_MEMORY_STICK,
    //! Install to CompactFlash
    plkrHANDHELD_TARGET_STORAGE_MODE_COMPACT_FLASH
};

//----------------------------------------------------------------------------------------
// File handling
//----------------------------------------------------------------------------------------

//! Behaviour to carry out when user tries to open a file that doesn't exist
/*
 *  Used for example when click 'Edit file' button on the starting page tab of the
 *  channel_dialog .
 */
enum {
    //! Show a message dialog asking if they want to create the file.
    plkrPROMPT_IF_FILE_NOT_FOUND,
    //! Automatically create the file.
    plkrCREATE_IF_FILE_NOT_FOUND,
    //! Show an error message dialog.
    plkrERROR_IF_FILE_NOT_FOUND
};

//----------------------------------------------------------------------------------------
// The window popup placement behaviour
//----------------------------------------------------------------------------------------

//! The window popup placement behaviour
/* !
 * Determines where a newly opened dialog is placed on the screen.
 * 
 * This is configurable in the preferences_dialog Interface tab.
 */
enum {
    //! Center new dialog on the screen
    plkrWINDOW_PLACEMENT_STYLE_CENTER_ON_SCREEN = 0,
    //! Center new dialog on the parent dialog
    plkrWINDOW_PLACEMENT_STYLE_CENTER_ON_PARENT,
    //! Center new dialog on the last remembered location.
    plkrWINDOW_PLACEMENT_STYLE_LAST_LOCATION             
};

// ---------------------------------------------------------------------------------------
// Spider handling
// ---------------------------------------------------------------------------------------

//! A high value for the status_line_length key. If equal to this value, unchecks control.
/*!
    The L is required at end to let the wxConfig::Read() and Write() know it is a long
    property.
 */
#define plkrINFINITE_STATUS_LINE_LENGTH_LENGTH   1000000L

// ---------------------------------------------------------------------------------------
// Image handling
// ---------------------------------------------------------------------------------------

//! Image handling alt_maxwidth/alt_maxheight. Tells to only make a thumbnail, never link.
/*!
    The L is required at end to let the wxConfig::Read() and Write() know it is a long
    property.
 */
#define plkrIMAGES_ONLY_THUMBNAIL           -1L

//! Image handling alt_maxwidth/alt_maxheight. Tells to always link to the full-size one.
/*!
    The L is required at end to let the wxConfig::Read() and Write() know it is a long
    property.
 */ 
#define plkrIMAGES_ALWAYS_LINK              0L

//! Image handling: default maxheight before an image is thumbnailed on a normal viewer.
/*!
    The L is required at end to let the wxConfig::Read() and Write() know it is a long
    property.
 */ 
#define plkrIMAGES_DEFAULT_MAXHEIGHT        250L

//! Image handling: default maxwidth before an image is thumbnailed on a normal viewer.
/*!
    The L is required at end to let the wxConfig::Read() and Write() know it is a long
    property.
 */
#define plkrIMAGES_DEFAULT_MAXWIDTH         150L 

//! Image handling: default maxheight before an image is thumbnailed on a hires viewer.
/*!
    The L is required at end to let the wxConfig::Read() and Write() know it is a long
    property.
 */ 
#define plkrIMAGES_DEFAULT_MAXHEIGHT_HIRES_VIEWER 	500L

//! Image handling: default maxwidth before an image is thumbnailed on a hires viewer.
/*!
    The L is required at end to let the wxConfig::Read() and Write() know it is a long
    property.
 */
#define plkrIMAGES_DEFAULT_MAXWIDTH_HIRES_VIEWER	300L 

//----------------------------------------------------------------------------------------
// Defines of help items (since system may change later on) (Names of pages as listed
// in the 'Index' tab of the wxHtmlHelp controller (which were generated using the section
// names of the LaTex file). This would be fine except won't work if not english. 
//----------------------------------------------------------------------------------------

//! Help identifier to look up this topic in the help_controller.
#define plkrHELP_ID_ADD_CHANNEL_WIZARD                      _("Add Channel Wizard")
//! Help identifier to look up this topic in the help_controller.
#define plkrHELP_ID_ADD_CHANNEL_WIZARD_CHANNEL_NAME_PAGE    _("Channel Name Page")
//! Help identifier to look up this topic in the help_controller.
#define plkrHELP_ID_ADD_CHANNEL_WIZARD_STARTING_PAGE_PAGE   _("Starting Page Page")
//! Help identifier to look up this topic in the help_controller.
#define plkrHELP_ID_ADD_CHANNEL_WIZARD_LIMITS_PAGE          _("Limits Page")
//! Help identifier to look up this topic in the help_controller.
// 1 trailing spaces on this one (to differentiate from setup's, and install viewer's.
#define plkrHELP_ID_ADD_CHANNEL_WIZARD_DESTINATION_PAGE     _("Destination Page ")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_BODY_DIALOG                             _("Insert Body Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_BLOCKED_DIALOG                          _("Adding/Editing An Inclusion/Exclusion")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_BOOKMARK_DIALOG                         _("Insert Bookmark Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_CHANNEL_DIALOG_START_TAB                _("Starting Page Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_CHANNEL_DIALOG_SPIDERING_TAB            _("Spidering Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_CHANNEL_DIALOG_LIMITS_TAB               _("Limits Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_CHANNEL_DIALOG_FORMATTING_TAB           _("Formatting Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_CHANNEL_DIALOG_IMAGES_TAB               _("Images Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_CHANNEL_DIALOG_OUTPUT_TAB               _("Output Options Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_CHANNEL_DIALOG_DESTINATION_TAB          _("Destination Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_CHANNEL_DIALOG_SCHEDULING_TAB           _("Scheduling Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_EDITOR_DIALOG                           _("Editor Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_EMAIL_DIALOG                            _("Insert Email Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_EXCLUSION_DIALOG                        _("Inclusion/Exclusion List Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_FONT_DIALOG                             _("Insert Font Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_HANDHELD_DEST_DIALOG                    _("Handheld Destination Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_HR_DIALOG                               _("Insert Horizontal Rule Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_HYPERLINK_DIALOG                        _("Insert Hyperlink Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_IMAGE_DIALOG                            _("Insert Image Dialog")
//! Help identifier to look up this topic in the help_controller.
#define plkrHELP_ID_INSTALL_VIEWER_WIZARD                   _("Install Handheld Software Wizard")
//! Help identifier to look up this topic in the help_controller.
// 2 trailing spaces on this one.3
#define plkrHELP_ID_INSTALL_VIEWER_WIZARD_SOFTWARE_SELECTION_PAGE _("Software Selection Page  ")
//! Help identifier to look up this topic in the help_controller.
// 2 trailing spaces on this one.
#define plkrHELP_ID_INSTALL_VIEWER_WIZARD_DESTINATION_PAGE  _("Destination Page  ")
//! Help identifier to look up this topic in the help_controller.
#define plkrHELP_ID_MAIN_DIALOG                             _("Main Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_OL_DIALOG                               _("Insert Ordered List Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_POPUP_DIALOG                            _("Insert Popup Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_PREFERENCES_DIALOG_AUTOUPDATE_TAB       _("Autoupdate Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_PREFERENCES_DIALOG_EDITOR_TAB           _("HTML Editor Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_PREFERENCES_DIALOG_INTERFACE_TAB        _("Interface Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_PREFERENCES_DIALOG_SHOWCASE_TAB         _("Showcase Tab")
//! Help identifier to look up this topic in the help_controller (Note trailing space).
// Trailing space on this one to differentiate from channel dialog's similar tab
#define plkrHELP_ID_PREFERENCES_DIALOG_SPIDERING_TAB        _("Spidering Tab ")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_PREFERENCES_DIALOG_PROXY_TAB            _("Proxy Tab")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_PROGRESS_DIALOG                         _("Progress Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_SHOWCASE_DIALOG                         _("Showcase Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_SETUP_WIZARD                            _("Setup Wizard")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_SETUP_WIZARD_SOFTWARE_SELECTION_PAGE    _("Software Selection Page")
//! Help identifier to look up this topic in the help_controller .
// No trailing space on this one.
#define plkrHELP_ID_SETUP_WIZARD_DESTINATION_PAGE           _("Destination Page")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_SETUP_WIZARD_PROXY_PAGE                 _("Proxy Page")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_SETUP_WIZARD_CHANNEL_LIST_PAGE          _("Preconfigured Channels Page")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_SPAN_DIALOG                             _("Insert Span Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_TABLE_DIALOG                            _("Insert Table Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_TD_TH_DIALOG                            _("Insert Table Cell Dialog")
//! Help identifier to look up this topic in the help_controller .
#define plkrHELP_ID_UL_DIALOG                               _("Insert Unordered List Dialog")
 
//----------------------------------------------------------------------------------------
// OS Specific defines: MSW
//----------------------------------------------------------------------------------------

//! Number of times program will look for BMP2TMP still running, if feature enabled.
#define plkrMSW_MAXIMUM_NUMBER_OF_PROCESS_STILL_ACTIVE_CHECKS      10

//! Seconds to sleep before checking again if BMP2TMP still running, if feature enabled.
#define plkrMSW_SLEEP_INTERVAL_BETWEEN_PROCESS_STILL_ACTIVE_CHECKS  1

/*! 
    \} 
 */ 

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif      //_PLUCKER_DEFINES_H_
