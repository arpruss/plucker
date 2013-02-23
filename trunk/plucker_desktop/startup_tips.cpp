//----------------------------------------------------------------------------------------
// Name:        startup_tips.cpp
// Purpose:     Provides 'Did you know' tips on application startup
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/12/05
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: startup_tips.cpp,v 1.19 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "startup_tips.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_STARTUP_TIPS )

//----------------------------------------------------------------------------------------
// Standard wxWindows headers
//----------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//----------------------------------------------------------------------------------------
// Header of this .cpp file
//----------------------------------------------------------------------------------------

#include "startup_tips.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/tipdlg.h"                  // wxTipProvider and related
#include "wx/textfile.h"                // wxTextFile

// ---------------------------------------------------------------------------------------

#include "configuration.h"
#include "html_tip_dialog.h"
#include "processed_tip_provider.h"
#include "utils_string.h"

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

bool startup_tips::show_startup_tips( wxWindow* parent,
                                      bool is_menu_command )
{
    // If prefs say don't user doesn't want to display tips, and it is not a menu
    // command telling it to show tips, then just abort here
    bool help_startup_tips_enabled = the_configuration->Read(wxT( "/PLUCKER_DESKTOP/help_startup_tips_enabled"), 1L );

    if ( ! help_startup_tips_enabled && ! is_menu_command ) 
    {
        return FALSE;
    }

    wxString tips_textfile_name;        // Text filename to use as source of tips
    wxString tips_path;                 // Directory + filename of the source tips file
    size_t   last_tip_shown;            // Stores last tip shown, enabling next one to be read

    // Get path to tip file
    tips_textfile_name = wxT( "tips.txt");
    tips_path = get_plucker_directory( RESOURCES ) << wxT( "/strings_tips/") << tips_textfile_name;

    // Read the last tip shown from the configuration file, so know where left off last day.
    last_tip_shown = (size_t)the_configuration->Read(wxT( "/PLUCKER_DESKTOP/help_startup_tips_last_tip_shown"), 0L );

    // Create a tip provider object and initialize it with the correct starting tip
    // Note the casting from processed_tip_provider to wxTipProvider. This is crucial,
    // else will get memory under/overwrite error in DebugView, reported on shutdown,
    // This cast is what wxCreateFileTipProvider() does in the stock wxWindows.
    wxTipProvider *the_tip_provider = (wxTipProvider*)new processed_tip_provider( tips_path, last_tip_shown );
    // Show the tip dialog. We can either our custom HTML one or the stock one.
#if ( setupUSE_HTML_TIP_DIALOG )

    // Will be used for the html later on
    wxString tips_style_name;
    tips_style_name << plkrAPPLICATION_SUITE_NAME << wxT( " " ) << _( "tip" );

    // In a HTML tip window, the tip gets placed between an optional HTML snippet
    // and footer HTML snippet:

    // First create a string for what we want the header of the html window to be...
    wxString html_header;
    // so assemble the string, translating what we need to translate, and generating
    // the urls to the images....

    html_header
    << wxT( "<body bgcolor='#ffffff' text='") << plkrGUI_ELEMENT_DARK_COLOR_RGB_STRING << wxT( "'>")
    << wxT( "<table border='0' cellpadding='0' cellspacing='0' width='100%'>")
    <<   wxT( "<tr>")
    <<      wxT( "<td colspan='2'>")
    <<         wxT( "<img src='") << get_plucker_directory( RESOURCES )
    <<                      wxT( "/graphics_dialog/html_tip_dialog_header_upper.xpm") << wxT( "'>")
    <<         wxT( "</td>")
    <<      wxT( "<td colspan='4' align='right'>")
    <<         wxT( "<b><font size='+2'>") << _( "Tip of the day" ) << wxT( "</font></b>")
    <<         wxT( "<img src='") << get_plucker_directory( RESOURCES )
    <<                     wxT( "/graphics_dialog/html_tip_dialog_spacer.xpm") << wxT( "'>")
    <<      wxT( "</td>")
    <<   wxT( "</tr>")
    <<   wxT( "<tr>")
    <<      wxT( "<td colspan='1' bgcolor='") << plkr_GUI_LIGHT_COLOR_RGB_STRING << wxT( "' valign='top'>")
    <<         wxT( "<img src='") << get_plucker_directory( RESOURCES )
    <<                      wxT( "/graphics_dialog/html_tip_dialog_header_lower.xpm") << wxT( "'>")
    <<      wxT( "</td>")
    <<      wxT( "<td colspan='5' bgcolor='") << plkr_GUI_LIGHT_COLOR_RGB_STRING << wxT( "' valign='middle' align='right'>")
    // ... adding in the type of tip we are showing, into the header (scroll up to the
    // earlier switch statement where tips_style_name was set.
    <<          wxT( "<font color='#FFFFFF'>") << tips_style_name << wxT( "</font>")
    <<          wxT( "<img src='") << get_plucker_directory( RESOURCES )
    <<                     wxT( "/graphics_dialog/html_tip_dialog_spacer.xpm") << wxT( "'>")
    <<      wxT( "</td>")
    <<   wxT( "</tr>")
    << wxT( "</table>")
    << wxT( "<br>")
    << wxT( "<table border='0' cellpadding ='5'>")
    <<    wxT( "<tr>")
    <<       wxT( "<td>")
    ;

    // ..and close what we started, in the footer.
    wxString html_footer;
    // Assemble the footer
    html_footer
    <<       wxT( "</td>")
    <<    wxT( "</tr>")
    << wxT( "</table>")
    << wxT( "</body>")
    ;

    // Optionally generate a nice title for the dialog
    wxString dialog_title;
    dialog_title << plkrAPPLICATION_DESKTOP_NAME << wxT( ": " ) << _( "Tip of the day" );

    // Now display the html tip window. Returns the value of the 'show next time' checkbox,
    // telling whether or not they want these to display on startup in the future.
    // The third argument is the configuration value read way back at the top, since
    // if called by a menu and they specified they don't want these to show at
    // startup, then the "show next time" should initialize as unchecked.
    help_startup_tips_enabled = ::show_html_tip( parent, the_tip_provider,
                                                 help_startup_tips_enabled,
                                                 html_header, html_footer,
                                                 TRUE,
                                                 wxSize( 420, 230 ),
                                                 0,
                                                 dialog_title
                                                );

#else   // setup_USE_HTML_TIP_DIALOG

    // The stock one is the same idea and first 3 parameters as above, but obviously no
    // header and footer.
    help_startup_tips_enabled = wxShowTip( parent, the_tip_provider,
                                           help_startup_tips_enabled );

#endif  // setup_USE_HTML_TIP_DIALOG

    // Write number of last tip shown so know what to show next time.
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/help_startup_tips_last_tip_shown"),
                              (long)the_tip_provider->GetCurrentTip() );

    // Write to config whether want to show startup tips the next time.
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/help_startup_tips_enabled"),
                              (long)help_startup_tips_enabled );

    // Combust the tip provider in unholy flames.
    delete the_tip_provider;

     // Return whether want to show the tips next startup (in case want to set a checkbox
    // on the dialog that is the parent of this one).
    return help_startup_tips_enabled;
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_STARTUP_TIPS
