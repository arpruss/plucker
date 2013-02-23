//----------------------------------------------------------------------------------------
/*!
    \file       setup.h
    This file contains defines, that get included into all files. It is used to turn off
    certain features mostly.
    \modified
    \copyright  (c) Robert O'Connor ( www.oconnor.md )
    \licence    GPL
    \brief      Plucker Desktop compilation settings
    \author     Robert O'Connor
    \date       2001/10/20
 */
// RCS-ID:          $Id: setup.h,v 1.59 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _SETUP_H_
#define _SETUP_H_

//----------------------------------------------------------------------------------------
// Restore the regular wxWindows defines for compilers, if not already there
//----------------------------------------------------------------------------------------

// Check this that it still works on Borland BCC, since __VISUALC__ is a test that we
// are using Visual C++

#if defined(_MSC_VER) && !defined(__VISUALC__)
    #define __VISUALC__ _MSC_VER
#endif

//----------------------------------------------------------------------------------------

/*!
    \addtogroup setup_defines
    \{
 */

//----------------------------------------------------------------------------------------
// CPP feature inclusion/removal defines.
// These remove all calls to a certain plucker-desktop feature. To complete the removal,
// open up the makefile and remove its corresponding group of objects from the
// $(OBJECTS) list for example to get fid of internationalization, set
// setupUSE_INTERATIONALIZATION=0 in this setup.h file, then comment out
// $(INTERNATIONALIZATION_OBJECTS) from the makefile's list of OBJECTS=
//----------------------------------------------------------------------------------------

//! Whether to include the code for internationalization (using .po files)
#if ! defined(__GNUWIN32__)
    #define setupUSE_INTERNATIONALIZATION                1
#else
    #define setupUSE_INTERNATIONALIZATION                0
#endif

//! Whether to use a splashscreen.
/*!
    Program also has a configuration setting in preferences_dialog allowing end user
    to turn off displaying them, even if the compilation setting is 1 .
 */
#define setupUSE_SPLASHSCREEN                            1

//! Whether to use startup tips ("Did you know"...).
/*!
    Program also has a configuration setting in preferences_dialog allowing end user
    to turn off displaying them, even if the compilation setting is 1 .
 */
#define setupUSE_STARTUP_TIPS                            1

//! Whether to use a html enabled tip of the day dialog, instead of the default kind.
/*!
    If don't use it, tips will have visible HTML code though, and unreplaced defines
    like plkrAPPLICATION_SUITE_NAME.
 */
#define setupUSE_HTML_TIP_DIALOG                         1

//! Whether to use the online help (an html help controller, currently )
#define setupUSE_ONLINE_HELP                             1

//! Whether or not to use wizards
#define setupUSE_WIZARDS                                 1

//! Whether to use the progress dialog classes (otherwise console windows only)
#define setupUSE_ENHANCED_PROGRESS_DIALOG                1

//! Whether or not to use the integrated HTML editor
#define setupUSE_INTEGRATED_HTML_EDITOR                  1

//! Whether or not to use the integrated exclusion list editor
#define setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR        1

//! Whether or not to use the Plucker showcase
#define setupUSE_SHOWCASE                                1

//! Whether to use message dialogs with a checkbox that say "Don't remind me again".
/*!
    If set to 0, then some message dialogs will just not be shown, such as
    the message about if couldn't find the Palm Desktop software on MSW.
    This allows annoying reminder dialogs can stay hidden.
 */
#define setupUSE_CHECKBOX_MESSAGE_DIALOG                 1

//! Whether to use the enhanced about dialog
#define setupUSE_ENHANCED_ABOUT_DIALOG                   1

//! Whether to use the single instance checker code
#ifdef __WXMSW__
    #define setupUSE_SINGLE_INSTANCE_CHECKER             0
#else
    #define setupUSE_SINGLE_INSTANCE_CHECKER             0
#endif    

//! Whether to use the import controller (1 is recommended)
#define setupUSE_IMPORT_CONTROLLER                       1

//----------------------------------------------------------------------------------------
// Feature defines/mods, but not removing a cpp file object from the output
//----------------------------------------------------------------------------------------

//! Whether to use drag and drop and clipboard or not.
/*!
    Cygwin b20 doesn't have it. Later Cygwin versions probably do.
 */
#if ! defined(__GNUWIN32__)
    #define setupUSE_DRAG_AND_DROP                       1
#else
    #define setupUSE_DRAG_AND_DROP                       0
#endif

//! Whether to set CWD to pluckerhome before executing plucker-build.
/*! Don't know how it can pick up a pluckerdir of the Plucker Desktop cwd even if I
    specify a pluckerdir key in the channel section (env variable don't work either).
    This is required for MSW version to work successfully.
 */
#define setupSET_CWD_TO_PLUCKERHOME_BEFORE_BUILD         1

//! Sends channel's output file to destination immediately vs. a group at the end.
/*!
    Often, user wants a set of channels to be updated. If set this switch to 1, then
    the output .pdb from each channel will be sent to its destinations as soon as it is
    created. On Linux, they will all be sent in a batch at the end. Linux needs to have
    the batch at the end to be user friendly, otherwise would have to sit and click the
    handheld's sync button multiple times (because of how pilot-link works). For others,
    they can be sent to destinations immediately, so set to 0.
 */
#if defined(__WXMSW__) || defined(__WXMAC__)
    #define setupSEND_CHANNEL_PDBS_TO_DESTINATIONS_IN_BATCH_AT_END_NOT_IMMEDIATELY  0
#else
    #define setupSEND_CHANNEL_PDBS_TO_DESTINATIONS_IN_BATCH_AT_END_NOT_IMMEDIATELY  1
#endif

//----------------------------------------------------------------------------------------
// OS-specific defines: MSW
//----------------------------------------------------------------------------------------

//! Looks up value of plucker bundle (installation) path from the registry and saves it to the .ini during startup.
/*!
    Else it tries to use the a GetCwdDir() on startup, then going up a directory.
 */
#if defined(__WXMSW__)
    #define setupUSE_MSW_PLUCKER_BUNDLE_PATH_REGISTRY_CHECK      1
#else
    #define setupUSE_MSW_PLUCKER_BUNDLE_PATH_REGISTRY_CHECK      0
#endif

//! Improves process killing on MSW with some extra platform-specific code for killing.
/*!
    Will enable a function that can kill children of that process, and also kill better,
    by killing from within the process itself. MSW has an inadequate system of process
    killing, so this is pretty much required.
    Otherwise will just default to the wxKill() function used on all other systems.
 */
#if defined(__WXMSW__)
    #define setupUSE_MSW_ENHANCED_PROCESS_KILLING        1
#else
    #define setupUSE_MSW_ENHANCED_PROCESS_KILLING        0
#endif

//----------------------------------------------------------------------------------------
// Hacks
//----------------------------------------------------------------------------------------

//! Whether to use an idle timer for generating idle events on the process progress dialog.
/*!
    Reason for this is that MSW anyways won't generate an idle event after the first one
    unless the mouse is wriggled. This only happens on the release build, not the debug
    build (might be Borland only too). 
*/
#if defined(__WXMSW__)
    #define setupUSE_PROCESS_PROGRESS_DIALOG_IDLE_TIMER  1
#else
    #define setupUSE_PROCESS_PROGRESS_DIALOG_IDLE_TIMER  0
#endif

//! A hack around the fact that the wxDateTime::ParseFormat() as of wx2.3.2
/*!
    It won't handle a parse of a W3C formatting datetime string, and thus asserts on
    wxGTK. If put this to 0, it will return to using the wxDateTime::ParseFormat()
    function.
 */
#define setupUSE_DATETIME_MANUAL_PARSE_HACK              1

//! Similar to setupUSE_DATETIME_MANUAL_PARSE_HACK, but for time.
#define setupUSE_TIME_MANUAL_PARSE_HACK                  1

//! A hack to stop wxFileDialog from saving to plucker.ini while wxWindows awaits feature.
#define setupUSE_NO_CONTROL_PERSISTANCE_HACK             0

//! A hack to force update UI events from derived listctrls to always be caught by parent
/*!
    It seems that GTK/MSW/OSX doesn't catch the event either. Does there need to be an event.skip in the listctrl?
*/
#define setupUSE_PARENT_WINDOW_CATCH_UPDATE_UI_EVENT_FROM_DERIVED_LISTCTRL_HACK 1

//! A Hack to deal with the fact that for some reason the satellite gets inserted (cached BCC headers?)
#define setupUSE_CHANNEL_LISTCTRL_BASE_FORCE_NEW_ITEM_IMAGE_HACK       1

//! Whether to force shutdown of commandline mode, MSW, progress dialog, autoclose dialog.
/*!
    Without this, on MSW, commandline mode, MSW, progress dialog, autoclose dialog, will
    never terminate, which will either cause many copies to float around, or will cause
    HotSync to hang if waiting for it to finish.
 */
#ifdef __WXMSW__
    #define setupUSE_MSW_FORCE_COMMANDLINE_AUTOCLOSE_PROGRESS_CLOSURE  1
#else
    #define setupUSE_MSW_FORCE_COMMANDLINE_AUTOCLOSE_PROGRESS_CLOSURE  0
#endif

//! On MSW, as of around wx 2.3.0, can't set en_US anymore. (May be fixed in wx2.5.0+)
#ifdef __WXMSW__
    #define setupUSE_MSW_EN_US_LOCALE_FAILURE_HACK                     1
#else
    #define setupUSE_MSW_EN_US_LOCALE_FAILURE_HACK                     0
#endif

//! The MAC native listctrl (but not generic) inserts new items in a different spot vs. normal, and has column header arrows.
/*!
    On WXMAC, as of wx2.8.4, the new native wxListCtrl is looking good, but seems to be some
	incompatibility with the index of newly added items, etc, so that get items inserted in wrong index,
	and there are sort arrows on the column headers that one can't get rid of (and they works to sort 
	the items in the list--which gets it out of sync with the config file).
	This will set a system settings to use the generic wxListCtrls instead
*/
#ifdef __WXMAC__
    #define setupUSE_MAC_NATIVE_LISTCTRL_INSERTS_IN_WRONG_INDEX_AND_CANT_REMOVE_ARROWS_FROM_HEADERS_HACK  1
#else
    #define setupUSE_MAC_NATIVE_LISTCTRL_INSERTS_IN_WRONG_INDEX_AND_CANT_REMOVE_ARROWS_FROM_HEADERS_HACK  0
#endif

//! Whether to force a relayout of XRC dialogs that have an attached unknown control.
/*!
    On WXMAC 2.8.4, when load an XRC resource with an unknown control, the dialog won't adjust 
	to make that unknown control grow to fit the sizer, it will stay its default size.
	However, if that dialog then resizes to a different value, either via user dragging the corner
	of dialog bigger, or automatically gets bigger when dialog loads via the function
	utils_controls::read_dialog_position_and_size, then all works correctly. 
	Note that this doesn't happen with non-XML resources, having the exact same custom control 
	(for example the palm destinations class in the add wizard behaves normally, but the 
	palm destinations class in the channels dialog misbehaves.
	What this does is for the affected controls, it resizes them one pixel wider and taller, before
	calling the utils_controls::read_dialog_position_and_size, thus forcing a correction.
	The utils_controls::read_dialog_position_and_size can then set to whatever it wants, including
	getting rid of that extra pix of wide or tall.
	Note that could probably do the same with some combination of wxWindow::Fit(), wxWindow::Layout(),
	wxWindow::SetSizer(), but I can't figure it out and I am not going to start naming the sizers in
	the XRC resources, since I am not going to pollute it with that: the XML resources should handle
	that internally.
	It is an issue with WXMAC and WXMSW. WXGTK works as expected.
*/
#if defined(__WXMSW__) || defined(__WXMAC__)
    #define setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS  1
#else
    #define setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS  0
#endif

//----------------------------------------------------------------------------------------
// Deprecated
//----------------------------------------------------------------------------------------

//! Deprecated using of 1000000 for always link, as 0 works also.
#define setupUSE_DEPRECATED_ALWAYS_LINK_MAXWIDTH_IS_ALSO_1000000_AS_WELL_AS_0 	1

//----------------------------------------------------------------------------------------

// Close out the mast list of grouping for Doxygen documentation generation:
/*!
    \}
 */

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif      //_SETUP_H_
