//----------------------------------------------------------------------------------------
// Name:        utils_clipboard.cpp
// Purpose:     Utils for Clipboard and Drag and Drop
// Author:      Robert O'Connor
// Modified by:
// Created:     2002/01/01
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: utils_execute.cpp,v 1.6 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "utils_execute.h"
#endif

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

#include "utils_execute.h"

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

bool utils_execute::launch_xterm_shell( const wxString& command, 
                                        const wxString& shell_console_title,
                                        bool close_on_exit,
                                        bool synchronous,
                                        bool show_scrollbar,
                                        long saved_line_history
                                       )
{
    bool error_occurred;

    // Note for sanity, we are going to put a trailing space at the end of each 
    // component of the xterm command string.

    // Strip any "'" from the shell console title as they would interfere with the xterm 
    // commandline...    
    wxString stripped_console_title = shell_console_title;  
    stripped_console_title.Replace( wxT( "'" ), wxT( " " ), TRUE );
    
    // If this channel is specified to NOT close on exit then make hold variable to be 
    // "-hold" so window will stay open. (If close_on_exit == FALSE, then the string 
    // will stay empty and this parameter will not be included in xterm commandline).    
    wxString hold_string;
    if ( close_on_exit == FALSE ) 
    {
        hold_string = wxT( "-hold " );
    }   

    // Similarly, if using a scrollbar then include that as a string
    wxString show_scrollbar_string;
    if ( show_scrollbar )
    {
        show_scrollbar_string = wxT( "-sb " );
    }

    // Convert the saved_line_history long to a string
    wxString saved_line_history_string;
    saved_line_history_string.Printf( wxT( "%ld" ), saved_line_history );

    // Assemble the xterm command string.
    wxString xterm_command_string;
    xterm_command_string << wxT( "xterm " ) << hold_string << show_scrollbar_string 
        << wxT( "-sl " ) << saved_line_history_string << wxT( " " ) 
        << wxT( "-title '" ) << stripped_console_title << wxT( "' " )
        << wxT( "-e " ) << command;
 
    // Print a debug message, so can see if a problem with the commandline
    wxLogDebug( wxT( "About to execute: " ) + xterm_command_string );

    // Do it, at last. The synchronous telss whether to wait for it to complete
    // before continuing.
    error_occurred = wxExecute( xterm_command_string, synchronous );

    return error_occurred;
}
