//----------------------------------------------------------------------------------------
// Name:        build_progress_wrappers.cpp
// Purpose:     Describes build_progress_wrappers
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: build_progress_wrappers.cpp,v 1.11 2007/06/29 00:58:50 robertoconnor Exp $

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "build_progress_wrappers.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

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

#include "build_progress_wrappers.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "configuration.h"
#include "plucker_controller.h"
#include "build_progress_dialog.h"
#include "proxy_prompt.h"

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------
int build_progress_wrappers::launch_build_progress_dialog( wxWindow* parent, 
                                                           const wxArrayString& channel_sections
#if ( setupUSE_MSW_FORCE_COMMANDLINE_AUTOCLOSE_PROGRESS_CLOSURE )
                                                          , long force_commandline_autoclose
#endif
                                                         )

{
    int dialog_return_code = wxID_CANCEL;

    // No point continuing, if there is no channel sections
    if ( channel_sections.IsEmpty() ) 
    {
        return dialog_return_code;
    }    

#if ( setupUSE_ENHANCED_PROGRESS_DIALOG )

    // This will either return a " --http-proxy-pass=somepassword" or will just 
    // be an empty string if not using proxy prompts.
    wxString http_proxy_password_argument = proxy_prompt::get( parent )->get_spider_argument();

    // Read close behaviour from config file, whether we want to close on exit.
    bool progress_dialog_close_on_exit = 
        (bool)the_configuration->Read( wxT( "/PLUCKER_DESKTOP/progress_dialog_close_on_exit" ), 0L );
    wxLogDebug( wxT( "progress_dialog_close_on_exit=%d" ), (int)progress_dialog_close_on_exit );

    // Make a array of command lines based on the channel section names:
    wxArrayString commandlines;
    size_t channel_sections_array_index;
    wxString current_commandline;

    for ( channel_sections_array_index = 0; 
          (int)channel_sections_array_index < (int)channel_sections.GetCount();
          channel_sections_array_index++ ) 
    {
        
        // Empty the command line from last iteration.
        current_commandline.Empty();        
        
        // Build the commandline.
        // On MAC and MSW, allow the script to be called directly.
        // This allows easier killing.
#if defined(__WXMSW__)
        if ( (bool)the_configuration->Read( wxT( "/PLUCKER_DESKTOP/process_launch_distiller_directly" ),
                                           1L ) ) 
        {
            // If so, construct a commandline directly.                               
            current_commandline << get_bundle_runtime_files_directory() 
                << wxT( "/parser/python/vm/python.exe" ) << wxT( " \"")
                << get_plucker_directory( PYPLUCKER ) 
                << wxT( "/Spider.py\"" ) 
                << wxT( " -P" ) << wxT( " \"" ) << get_plucker_directory( PLUCKERHOME ) 
                << wxT( "\" -s" ) << wxT( " " )
                << channel_sections.Item( channel_sections_array_index )
                << http_proxy_password_argument; 
                                  
        } 
        else 
        {
            // Otherwise, use plucker-build
            current_commandline << wxT( "plucker-build -s " )
                << channel_sections.Item( channel_sections_array_index )
                << http_proxy_password_argument; 
        }         
#elif defined(__WXMAC__)
            // If so, construct a commandline directly.                               
            current_commandline << wxT( "python" ) << wxT( " \"" )
                << get_plucker_directory( PYPLUCKER ) << wxT( "/Spider.py\"" )
                << wxT( " -P" ) << wxT( " \"" ) << get_plucker_directory( PLUCKERHOME ) 
                << wxT( "\" -s" ) << wxT( " " )
                << channel_sections.Item( channel_sections_array_index )
                << http_proxy_password_argument;                               
#else
        // Other platforms terminate plucker-build fine (plus there is no way
        // to know where the script is, so moot anyways).
        current_commandline << wxT( "plucker-build -s " ) 
            << channel_sections.Item( channel_sections_array_index )
            << http_proxy_password_argument;
#endif
        // Add it to our array of commandlines.
        commandlines.Add( current_commandline );
    }       
    
    // Make a kill signal. Default is wxSIGKILL on MSW, wxSIGTERM on rest, but the
    // defaults can be overridden by user in the config file. 
    // Has to be SIGKILL on MSW for plucker-build, since the wxWin code only does 
    // window closing for wxSIGTERM.
#ifdef __WXMSW__
    bool process_kill_using_sigkill_default = TRUE;
#else   // __WXMSW__
    bool process_kill_using_sigkill_default = FALSE;
#endif  // __WXMSW__    
    wxSignal kill_signal;
    if ( (bool)the_configuration->Read( wxT( "/PLUCKER_DESKTOP/process_kill_using_sigkill" ),
                                        process_kill_using_sigkill_default ) ) 
    {
        kill_signal = wxSIGKILL;
    } 
    else 
    {
        kill_signal = wxSIGTERM;
    }    
    
    // All our parameters are now decided. Time to launch the dialog.    
    // Note that the stream_kind in this constructor is trivial, since they are
    // set to stderr or none on a channel to channel basis anyways.
    // Parent was passed by the constructor.
    build_progress_dialog a_build_progress_dialog( parent,
                                                   channel_sections,
                                                   commandlines,
                                                   optionPROCESS_PROGRESS_DIALOG_STDERR_STREAM_KIND,
                                                   kill_signal,
                                                   progress_dialog_close_on_exit,
                                                   wxT( "/PLUCKER_DESKTOP/progress_dialog_show_details" ),
                                                   wxT( "PlkrLog_plkrFILE_FRIENDLY_DATETIME_STAMP.txt" )
#if ( setupUSE_MSW_FORCE_COMMANDLINE_AUTOCLOSE_PROGRESS_CLOSURE )
                                                   , force_commandline_autoclose
#endif
                                                  );

    dialog_return_code = a_build_progress_dialog.ShowModal();

    commandlines.Clear();
    
    return dialog_return_code;  
    
#else   // setupUSE_ENHANCED_PROGRESS_DIALOG
    
    // Make a copy of the sections, otherwise we would be sending a temp variable over
    // which would be a fair-and-square warning:
    wxArrayString temp_channel_sections = channel_sections;

    // If not using progress dialogs in the compilation, call the console window method,
    // catching the bool of it being successful...
    if ( plucker_controller::get()->update_channels( temp_channel_sections ) ) 
    {
        // ...if it was successful, then return a wxID_OK...
        dialog_return_code = wxID_OK;
    } 
    else 
    {
        // ...otherwise return a cancel
        dialog_return_code = wxID_CANCEL;
    }
    
    // Clear array
    temp_channel_sections.Clear();
    
    // Send code.
    return dialog_return_code;  
    
#endif  // setupUSE_ENHANCED_PROGRESS_DIALOG  

}
