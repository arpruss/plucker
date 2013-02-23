//----------------------------------------------------------------------------------------
// Name:        build_progress_dialog.cpp
// Purpose:     Describes build_progress_dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( rob@medicalmnemonics.com )
// Licence:     GPL
// RCS-ID:      $Id: build_progress_dialog_[with_os_msw_killing].cpp,v 1.1 2002/08/31 23:42:56 robertoconnor Exp $

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "build_progress_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_ENHANCED_PROGRESS_DIALOG )

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

#include "build_progress_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/txtstrm.h"
#include "wx/process.h"

// ---------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

// ---------------------------------------------------------------------------------------

#include "configuration.h"
#include "plucker_controller.h"
#include "os_msw.h"                 // Better process killing

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( build_progress_dialog, process_progress_dialog )
    
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

build_progress_dialog::build_progress_dialog( wxWindow* parent,
                     const wxArrayString& channel_sections_array,
                     const wxArrayString& commandline_array, 
                     long stream_kind,
                     wxSignal kill_signal,
                     bool kill_process_children,   
                     bool automatically_close_when_done,
                     const wxString& show_hide_listbox_configuration_key, 
                     const wxString& log_filename,
                     const wxString& log_path,
                     long scroll_history )
                     :
                     process_progress_dialog( parent,
                                  commandline_array, 
                                  stream_kind,
                                  kill_signal,
                                  kill_process_children,
                                  automatically_close_when_done,
                                  show_hide_listbox_configuration_key,
                                  log_filename,
                                  log_path,                                  
                                  scroll_history )
{
    m_channel_sections_array = channel_sections_array;
}

build_progress_dialog::~build_progress_dialog()
{
    // Free memory used by the array.
    wxLogDebug( "About to clear m_channel_sections_array()" );
    m_channel_sections_array.Clear();   
}

//----------------------------------------------------------------------------------------
// Protected methods (override baseclasses' virtuals)
//----------------------------------------------------------------------------------------

bool build_progress_dialog::on_before_execute_commandline_array()
{
    
    // Set an initial message for the total statictext.
    wxString channels_updated_message;
    channels_updated_message.Printf( _( "Channels updated: 0 of %d" ), 
                                        m_channel_sections_array.GetCount()                                    
                                   );
    get_total_statictext()->SetLabel( channels_updated_message );
    
    // Before calling plucker-build, need to set close_on_error and close_on_exit
    // to true in [<os>] ( and also [DEFAULT] if it is there too ) or the hidden process
    // won't ever terminate (since its waiting for a keypress which can't occur because it 
    // is hidden). 2nd argument only sets=1 if key exists, 3rd argument flushes config.
    set_close_on_exit( "DEFAULT", FALSE, TRUE, FALSE );
    set_close_on_error( "DEFAULT", FALSE, TRUE, FALSE );

    set_close_on_exit( get_os_configuration_section(), FALSE, FALSE, TRUE );
    set_close_on_error( get_os_configuration_section(), FALSE, FALSE, TRUE );

    // If a before_group_command was specified, execute it. TRUE means wait for it to finish   
    wxString before_group_command;
    before_group_command = the_configuration->Read( "/PLUCKER_DESKTOP/before_group_command", wxT( "" ) );
    if ( before_group_command != wxT( "" ) ) {
        get_details_listbox()->append_using_scroll_history( _( "Executing command before spidering channels..." ) );  
        ::wxExecute( before_group_command, TRUE );
    }
    
    // Give a notice that we are starting, as takes a few secs to spark up python 
    get_details_listbox()->append_using_scroll_history( _( "Initializing Plucker spidering engine..." ) );  
    // Set the channels updated gauge to say zero of total complete
    //! \tesr Needed? We do this at the start of eac anyways
    set_flickerfree_gauge_and_range( *get_subtask_gauge(), 0, (int)m_channel_sections_array.GetCount() );
    
    // return a value
    return TRUE;

} 
 
   
bool build_progress_dialog::on_before_execute_commandline_array_item( size_t commandline_array_index )
{
    // Rezero the progress guage from last iteraion
    get_subtask_gauge()->SetValue( 0 ); 

    // Get the current channel section from the array
    wxString current_section_string = m_channel_sections_array.Item( commandline_array_index ); 

    // In case there is a close_on_exit, close on error in this channel's section, 
    // (shouldn't be if created with Plucker Desktop), set those to 1 too.
    // Second argument only writes if keys existed, 3rd argument forces a flush 
    set_close_on_exit( current_section_string, FALSE, TRUE, TRUE );
    set_close_on_error( current_section_string, FALSE, TRUE, TRUE );

    // Write a separator to textctrl that tells what channel we are about to spider
    wxString doc_name_key;
    doc_name_key << '/' << current_section_string << '/' << "doc_name";
    wxString doc_name = the_configuration->Read( doc_name_key, "" );
    get_details_listbox()->append_using_scroll_history( " " );  
    get_details_listbox()->append_using_scroll_history( "-----------------------------------------------------------" );  
    get_details_listbox()->append_using_scroll_history( _( "Updating channel: " ) + doc_name + "..." );  
    get_details_listbox()->append_using_scroll_history( "-----------------------------------------------------------" ); 

    // Set the initial static text for the channel. If verbosity=0, then it will 
    // never change for the entire crawl, so leave a message saying why. Hide the
    // progress gauge if verbosity=0 so user doesn't think something is stuck.
    bool verbosity;
    verbosity = the_configuration->Read( "/" + current_section_string + "/" +
                                         "verbosity", 1L );                    
    if ( verbosity == 0 ) {
        get_subtask_statictext()->SetLabel( _( "Processing " ) + doc_name + " (Updating in quiet mode)" );
        if ( get_subtask_gauge()->IsShown() ) {
            get_subtask_gauge()->Show( FALSE );            
        // Set the stream kind that we want to catch for this channel to none.
        // Reason: if there is so little output, its impossible to yield/update, and 
        // program will appear to have frozen. 
        set_stream_kind( optionPROCESS_PROGRESS_DIALOG_NONE_STREAM_KIND );    
        }
    } else {
        get_subtask_statictext()->SetLabel( _( "Initializing channel: " ) + doc_name + "..." );
        if ( ! get_subtask_gauge()->IsShown() ) {
            get_subtask_gauge()->Show( TRUE );
        }
        // Set the stream kind that we want to catch to be stderr (or stdout if
        // had an old version of the parser).
#if ( setupUSE_CATCH_STDOUT_FROM_PLUCKER_BUILD )
        set_stream_kind( optionPROCESS_PROGRESS_DIALOG_STDOUT_STREAM_KIND );
#else
        set_stream_kind( optionPROCESS_PROGRESS_DIALOG_STDERR_STREAM_KIND );
#endif        
    }

    // Before calling plucker-build, need to set close_on_error and close_on_exit
    // to true or the hidden process won't ever terminate (since its waiting for
    // a keypress which can't occur because it is hidden).
    the_configuration->Write( "/" + current_section_string + "/" + "close_on_exit", 1L );                                 
    the_configuration->Write( "/" + current_section_string + "/" + "close_on_error", 1L );
    the_configuration->Flush();

#if ( setupSET_CWD_TO_PLUCKERHOME_BEFORE_BUILD )
    // See comments in setup.h
    wxString working_directory;
    working_directory = get_plucker_directory( PLUCKERHOME ) + plkrOS_DIR_SEPARATOR ;
    wxSetWorkingDirectory( working_directory );
#endif
    
    // Return a value
    return TRUE;

}

   
bool build_progress_dialog::on_after_commandline_array_item_execution_finished( size_t commandline_array_index )
{
    // Update the message for the total statictext.
    wxString channels_updated_message;
    channels_updated_message.Printf( _( "Channels updated: %d of %d" ),  
                                        commandline_array_index + 1, 
                                        m_channel_sections_array.GetCount()                                    
                                   );
    get_total_statictext()->SetLabel( channels_updated_message );

    // Advance the total gauge (+1 since array starts at zero)
    set_flickerfree_gauge_and_range( *get_total_gauge(), 
                                    (int)commandline_array_index + 1, 
                                    (int)m_channel_sections_array.GetCount() );
                                        
    // Set the subtask gauge to completion, to handle the case of just 
    // spidering a single page (which has no "* to do" events that would advance the
    // subtask gauge.
    set_flickerfree_gauge_and_range( *get_subtask_gauge(),
                                     1,
                                     1 );
    
    // Return a value
    return TRUE;
}


bool build_progress_dialog::on_after_commandline_array_executions_finished()
{

    // Install:    
    
    // ...First show start messages...
    wxString start_install_message = _( "Installing channel output to destinations..." );
    get_details_listbox()->append_using_scroll_history( start_install_message  ); 
    get_subtask_statictext()->SetLabel( start_install_message );

    // ...then do the actuall installing...
    plucker_controller::get()->install_channels( m_channel_sections_array );
    
    // ..then show message that going to set new dates...    
    get_details_listbox()->append_using_scroll_history( "Setting channels new due date" );    
    
    // ...then set the update_base values in configuration to their new settings...
    wxString current_section_string;    
    for ( size_t current_section_array_index = 0; 
          (int) current_section_array_index < (int) m_channel_sections_array.GetCount();
          current_section_array_index++ ) {
        current_section_string = m_channel_sections_array.Item( current_section_array_index ); 
        plucker_controller::get()->set_channel_update_base( current_section_string );  
    }

    // ...then show end messages...
    wxString end_install_message   = _( "Tasks completed for all channels." );
    get_details_listbox()->append_using_scroll_history( end_install_message  );
    get_subtask_statictext()->SetLabel( end_install_message );

    // ...then flush the config file, since changed the update_base values
    the_configuration->Flush();

    // Do the after_group_command.
    // If a after_group_command was specified, execute it. TRUE means wait for it to
    // finish
    wxString after_group_command;
    after_group_command = the_configuration->Read( "/PLUCKER_DESKTOP/after_group_command", wxT( "" ) );
    if ( after_group_command != wxT( "" ) ) {
        get_details_listbox()->append_using_scroll_history( _( "Executing command after spidering channels..." ) );
        wxExecute( after_group_command, TRUE );
    }   
    
    // Return a value
    return TRUE;
}


void build_progress_dialog::on_process_generated_an_output_line( long stream_kind, 
                                                         const wxString& line_text,
                                                         size_t commandline_array_index )
{
    wxLogDebug( "Entering build_progress_dialog::on_process_generated_an_output_line()" );
    // Put the line in the listbox
    get_details_listbox()->append_using_scroll_history( line_text );  

    
    if ( line_text.Matches( "*collected, * to do*" ) ) {
    
        wxString    value_string;
        wxString    range_string;        
        long        value_long;
        long        range_long;
        wxString    searchable_string = line_text;   
    
        // The string starts like this: "---- 1 collected, 9 to do ----"
        searchable_string.Replace( "---- ", "" );
        // The string now looks like this "1 collected, 9 still to do---\n"
        searchable_string.Replace( " collected, ", "R" );         
        // The string now looks like this: "1R9 still to do---\n"        
        value_string = searchable_string.BeforeFirst( 'R' );
        // The value string is now "1".
        range_string = searchable_string.AfterFirst( 'R' );
        // The range string is now "9 still to ----\n"
        range_string = range_string.BeforeFirst( ' ' );
        // The range string is now "9"        

        // Try to convert these strings to longs (returns false if conversion failed).. 
        if ( value_string.ToLong( &value_long ) ) {
            if ( range_string.ToLong( &range_long ) ) {
                // Note that range_long is current still the number remaining. 
                // Add it to value to get a proper range.
                range_long = range_long + value_long;
                // ...If successfully got two longs, cast to ints, and send to 
                // our gauge function.
                set_flickerfree_gauge_and_range( *get_subtask_gauge(),
                                                 (int)value_long,
                                                 (int)range_long );
                
                // Also make it into a message for the subtask statictext:
                wxString files_retrieved_message;
                files_retrieved_message.Printf( _( "Retrieving files (%ld of %ld)" ), 
                                                 value_long,
                                                 range_long                                   
                                               );
                get_subtask_statictext()->SetLabel( files_retrieved_message );                                                 
            }
        }
        
    // If it was a "converting the retrieved files" message...            
    } else if ( line_text.StartsWith( "Convert" ) ) {
        // ...Update the static text with a message
        get_subtask_statictext()->SetLabel( _( "Converting retrieved files...." ) );
    // If it was a "writing the files into pdb" message...
    } else if ( line_text.StartsWith( "Wrote " ) ) {
        // ...Update the static text with a message
        get_subtask_statictext()->SetLabel( _( "Conversion complete. Writing output file..." ) );      
     // It was a "retrieval of files is complete" message:
    } else if ( line_text.StartsWith( "---- all" ) ) {
        // ...Set the progress bar to max (range is always 
        // plkrFIXED_PERCENTAGE_GAUGE_RANGE to stop flicker, see notes in 
        // plucker_defines.h
        get_subtask_gauge()->SetValue( plkrFIXED_PERCENTAGE_GAUGE_RANGE ); 
        // ...Update the static text with a message
        get_subtask_statictext()->SetLabel( _( "All pages retrieved and parsed. Starting conversion..." ) );    
    }    
    
}


void build_progress_dialog::custom_kill( long pid, wxSignal sig, wxKillError *krc )
{   
#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
 
    wxLogDebug( "Entering build_progress_dialog::custom_kill" ); 
      
    // Set up defaults for Windows 9x/Me...    
    // NOTE: the process_kill_children doesn't seem to work anyways, and won't work on NT.     
    long process_wait_for_bmp2tbmp_termination = 0L;
    long process_kill_parent_by_pid            = 1L;
    long process_kill_children                 = 0L;
    long process_kill_python_by_name           = 0L;
    long process_kill_bmp2tbmp_by_name         = 0L;
    
    // The process_kill_using_sigkill was set in the constructor of process_progress_dialog
    
    // ...and override some defaults if it is a NT/2000/XP.
    //! \todo
    
    // Allow the config file to override these settings.  
    process_wait_for_bmp2tbmp_termination = the_configuration->Read( "/PLUCKER_DESKTOP/process_wait_for_bmp2tbmp_termination",
                                                                    process_wait_for_bmp2tbmp_termination );
                                                    
    process_kill_parent_by_pid = the_configuration->Read( "/PLUCKER_DESKTOP/process_kill_parent_by_pid",
                                                          process_kill_parent_by_pid );
                                                          
    process_kill_children = the_configuration->Read( "/PLUCKER_DESKTOP/process_kill_children",
                                                     process_kill_children );
    
    process_kill_python_by_name = the_configuration->Read( "/PLUCKER_DESKTOP/process_kill_python_by_name",
                                                           process_kill_python_by_name );

    
    process_kill_bmp2tbmp_by_name = the_configuration->Read( "/PLUCKER_DESKTOP/process_kill_bmp2tbmp_by_name",
                                                             process_kill_bmp2tbmp_by_name );                                                   

    // Wait for BMP2TBMP to stop, if so requested.
    if ( process_wait_for_bmp2tbmp_termination ) {
        wxLogDebug( "Entering a wait for bmp2tmp termination" );
        // Loop for 5 iterations to see if BMP2TBMP.EXISTS AS A PROCESS, 
        // and wait while it is. If still going after 10, then keep going.        
        for ( int count=0; count < plkrMSW_MAXIMUM_NUMBER_OF_PROCESS_STILL_ACTIVE_CHECKS; count++ ) {
            if ( ! os_msw::is_named_process_running( "BMP2TMP.EXE" ) ) {
                wxLogDebug( "bmp2tbmp not running. Moving on now." );
                break;
            }    
                // Sleep for a second before looking again.
                wxLogDebug( "bmp2tbmp still going. Sleeping." );
                Sleep( plkrMSW_SLEEP_INTERVAL_BETWEEN_PROCESS_STILL_ACTIVE_CHECKS );            
        }        
    }
    
    // Kill python by pid if so requested.
    if ( process_kill_parent_by_pid ) {
        wxLogDebug( "Doing requested process_kill_parent_by_pid" );
        os_msw::kill_properly( pid, sig, krc ); 
    }
    
    // Kill kids if so requested.
    if ( process_kill_children ) { 
        wxLogDebug( "Doing requested process_kill_children" );
        os_msw::kill_process_children( pid, sig, krc );     
    }     
    
    // Kill python by name if so requested
    if ( process_kill_python_by_name ) {
        wxLogDebug( "Doing requested process_kill_python_by_name" );
        os_msw::terminate_process_by_name ( "PYTHON.EXE" );
    }
    
    // Kill bmp2tmp by name if so requested
    if ( process_kill_bmp2tbmp_by_name ) {
        wxLogDebug( "Doing requested process_kill_bmp2tbmp_by_name" );
        os_msw::terminate_process_by_name ( "BMP2TBMP.EXE" );
    }
        
#endif  // setupUSE_MSW_ENHANCED_PROCESS_KILLING
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ENHANCED_PROGRESS_DIALOG
