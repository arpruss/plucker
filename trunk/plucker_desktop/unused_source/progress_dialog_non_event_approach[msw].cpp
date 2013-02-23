//----------------------------------------------------------------------------------------
// Name:        progress_dialog.cpp
// Purpose:     Plucking channel progress dialog
// Author:      Robert O'Connor, Vaclav Slavik
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( rob@medicalmnemonics.com )
// Licence:     GPL
// RCS-ID:      $Id: progress_dialog_non_event_approach[msw].cpp,v 1.1 2002/08/27 04:28:57 robertoconnor Exp $

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "progress_dialog.h"
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

#include "progress_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/txtstrm.h"
#include "wx/process.h"

// ---------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          //XRC XML resouces

// ---------------------------------------------------------------------------------------

#include "configuration.h"
#include "plucker_controller.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(progress_dialog, wxDialog)
    EVT_BUTTON( XMLID( "progress_dialog_stop_button" ), progress_dialog::on_stop_button )
    EVT_BUTTON( XMLID( "progress_dialog_details_button" ), progress_dialog::on_details_button ) 
    EVT_BUTTON( XMLID( "progress_dialog_export_button" ), progress_dialog::on_export_button ) 
    
    // An EVT_PAINT for some code to start as soon as the dialog is shown
    EVT_PAINT( progress_dialog::on_show )
    
    // We have to specify a EVT_CLOSE in a dialog-only app to correctly terminate
    // the app, when hit the 'X' button (or equivalent) in dialog's top corner, or turn off
    // the computer.
    EVT_CLOSE( progress_dialog::close_dialog ) 
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

progress_dialog::progress_dialog( wxWindow* parent,
                                  wxArrayString& channel_sections,
                                  bool install )
{
    wxTheXmlResource->LoadDialog( this, parent, "progress_dialog" );
    
    m_progress_listbox  = new progress_listbox( this, -1, 
                                  wxDefaultPosition, wxDefaultSize,
                                  0, NULL,
                                  wxLB_EXTENDED | wxLB_HSCROLL | wxLB_ALWAYS_SB );
                                                            
    wxTheXmlResource->AttachUnknownControl( "progress_dialog_listbox",
                                           m_progress_listbox );                                                                                      
    wxLogDebug( "m_progress_listbox successfully attached as unknown control" );     
    
    // Save the size of the dialog for a later restore 
    m_dialog_height_with_listbox            = GetSize().y;
    // Initialize the listbox as visible
    m_progress_listbox_is_shown             = TRUE;
    // Initialize as there is no process running
    m_is_process_running                    = FALSE;
    // Initialize as no error occurred in any of the executions.    
    m_error_occurred                        = FALSE;
    // Initialize to FALSE to let more update_channels() loop iterations make more plucker-builds.
    m_abort_updating_signal_was_entered     = FALSE;
    // Initialize to TRUE so that the initial focus event will trigger the updating code
    m_is_initial_focus_event                = TRUE;
    // Initialize to FALSE that the close_dialog() code has not run yet
    m_did_close_dialog_code_execute_yet     = FALSE;    
    // Initialize the most recent pid to 0, instead of random garbage which might be a pid
    // on the users system
    m_most_recent_pid                       = 0;
    // Initialize these to their passed arguments [ will be used in the on_show() ]
    m_channel_sections                      = channel_sections;
    m_install                               = install;
    
    // Look in the configuration and see if they desired to hide it. If so, hide it
    // initially
    bool show_details;
    show_details = (bool)the_configuration->Read( "/PLUCKER_DESKTOP/progress_dialog_show_details", 1L );
    if ( ! show_details ) {
        hide_listbox();
        // If initialized to not show details, then it is no longer vertically centered.
        // So recenter it.
        Centre( wxVERTICAL ); 
    }  
}


progress_dialog::~progress_dialog()
{
    // Free memory used by the array.
    m_channel_sections.Clear();    
}


void progress_dialog::set_is_process_running( bool is_process_running )
{
    m_is_process_running = is_process_running;
}


void progress_dialog::set_error_occurred( bool error_occurred )
{ 
    m_error_occurred = error_occurred;
}


void progress_dialog::extract_progress_from_stderr( const wxString& stdout_string )
{        
    // If it is a progress string (matches this mask using * and ?)...
    if ( stdout_string.Matches( "*collected, * to do*" ) ) {
    
        wxString    value_string;
        wxString    range_string;        
        long        value_long;
        long        range_long;
        wxString    searchable_string = stdout_string;   
    
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
                // our channel gaugue function
                set_current_channel_gauge( (int)value_long, (int)range_long );
            }
        }
        
    // If it was a "converting the retrieved files" message...            
    } else if ( stdout_string.StartsWith( "Convert" ) ) {
        // ...Update the static text with a message
        XMLCTRL( *this, "progress_dialog_current_channel_value_statictext", wxStaticText )
            ->SetLabel( _( "Converting retrieved files...." ) );
    // If it was a "writing the files into pdb" message...
    } else if ( stdout_string.StartsWith( "Wrote " ) ) {
        // ...Update the static text with a message
        XMLCTRL( *this, "progress_dialog_current_channel_value_statictext", wxStaticText )
            ->SetLabel( _( "Conversion complete. Writing output file..." ) );      
     // It was a "retrieval of files is complete" message:
    } else if ( stdout_string.StartsWith( "---- all" ) ) {
        // ...Set the progress bar to max (range is always plkrFIXED_PERCENTAGE_GAUGE_RANGE 
        // to stop flicker, see notes in plucker_defines.h
        XMLCTRL( *this, "progress_dialog_current_channel_gauge", wxGauge )
            ->SetValue( plkrFIXED_PERCENTAGE_GAUGE_RANGE ); 
        // ...Update the static text with a message
        XMLCTRL( *this, "progress_dialog_current_channel_value_statictext", wxStaticText )
            ->SetLabel( _( "All pages retrieved and parsed. Starting conversion..." ) );    
    }
}

//----------------------------------------------------------------------------------------
// Non-event handlers
//----------------------------------------------------------------------------------------

void progress_dialog::show_listbox()
{
    int screen_height;
    int screen_top;
    int dialog_top;
    int unused;
    
    // This gets the pixels of the usable part of the screen, and the screen height.
    // This accounts for an MSW taskbar taking up part of the real estate, and the 
    // task bar could be at the top. 
    wxClientDisplayRect( &unused, &screen_top, &unused, &screen_height );

    // Get the y position of the dialog
    dialog_top = GetPosition().y;
    
    // If the expanded listbox would go past the drawable screen height.
    if ( screen_height < ( dialog_top + m_dialog_height_with_listbox ) ) {
        
        // Start at the bottom and move it up accordingly.
        dialog_top =  screen_height;
        dialog_top -= m_dialog_height_with_listbox;
        // However, if the top would now impinge into a top taskbar, then set it 
        // down to top of the the usable screen area, so the caption is at least visible.
        // and let the bottom part disappear.
        if ( dialog_top < screen_top ) {
            dialog_top = screen_top;
        }
        
        // Set the y position of the dialog, ready for expansion. (-1 means default x)
        // The function is called Move() instead of SetPosition() for some reason.
        Move( -1, dialog_top );
    }

    // Now set y the size of the dialog to expand the width of the listbox, thus showing 
    // it. (-1 means default x).
    SetSize( -1, m_dialog_height_with_listbox );   
    
    // Update the caption on the "Details" button
    XMLCTRL( *this, "progress_dialog_details_button", wxButton )->SetLabel( _( "<< Details" ) );
    // Mark the listbox as shown.
    m_progress_listbox_is_shown = TRUE;
    
}


void progress_dialog::hide_listbox()
{
    // Get the number of pixels from top of dialog that the listbox is at
    int listbox_height;
    listbox_height = m_progress_listbox->GetSize().y;       
    
    // Start to calculate the new dialog size.
    int new_dialog_size;    
    new_dialog_size =  m_dialog_height_with_listbox;
    // Subtract the height of the listbox.
    new_dialog_size -= listbox_height;
    // Subtract the padding of a standard sizer.
    new_dialog_size -= plkrSTANDARD_SIZER_PADDING;
    
    // Set the height of the progress dialog to the new size, which effectively hides
    // the details textcontrol. The -1 for x means to just use the existing dimension.
    SetSize( -1, new_dialog_size );
    
    // Update the caption on the "Details" button
    XMLCTRL( *this, "progress_dialog_details_button", wxButton )->SetLabel( _( "Details >>" ) );
    // Mark the listbox as not shown.
    m_progress_listbox_is_shown = FALSE;    
}


void progress_dialog::close_dialog( )
{
    wxLogDebug( "Entering progress_dialog::close_dialog()" );
    
    // Without this, it is possible to run this code twice, once from the 
    // dialog shut down and once from the EVT_CLOSE event.
    /*! \todo GURU: Anyother better ways of doing this? */
    
    if ( ! m_did_close_dialog_code_execute_yet ) { 
    
        wxLogDebug( "close_dialog() code hasn't been run yet, so doing cleanup..." );
    
        // So won't do any more iterations of stuff in update_channels()'s loops, if we 
        // are aborting half way.
        m_abort_updating_signal_was_entered = TRUE;

        // Kill the active process if there is one currently going.
        kill_most_recent_process();

        // Save the show_details value for next time.
        the_configuration->Write( "/PLUCKER_DESKTOP/progress_dialog_show_details",
                                 (long)m_progress_listbox_is_shown );
        the_configuration->Flush();

        // If not in command line mode, then was not a null parent, so end modally.
        // Otherwise, destroy it so memory will clear.
        /*! \todo GURU: Okay to have wxWindow* cast of NULL here? */
        if ( GetParent() != NULL ) {
            wxLogDebug( "progress_dialog::close dialog() About to call EndModal(wxID_CANCEL)" );
            EndModal( wxID_CANCEL );
        } else {
            wxLogDebug( "progress_dialog::close dialog() About to call Destroy()" );
            Destroy();
        }
    } else {
        wxLogDebug( "close_dialog() has run once already, so doing nothing" );
    }    

    // Update the fact that this segment has run once now, and shouldn't be
    // run again.
    m_did_close_dialog_code_execute_yet     = TRUE; 
}


bool progress_dialog::update_channels( wxArrayString& channel_sections, bool install )
{
    wxString command_string;
    wxString current_section_string;
    
    size_t number_of_sections           = channel_sections.GetCount();
    size_t current_section_array_index;        
    
    if ( number_of_sections == 0 ) {
        return FALSE;
    }
    
    // If a before_group_command was specified, execute it. TRUE means wait for it to finish   
    wxString before_group_command;
    before_group_command = the_configuration->Read( "/PLUCKER_DESKTOP/before_group_command", wxT( "" ) );
    if ( before_group_command != wxT( "" ) ) {
        m_progress_listbox->Append( _( "Executing command before spidering channels..." ) );  
        wxExecute( before_group_command, TRUE );
    }
      
    if ( m_abort_updating_signal_was_entered ) {  
        return FALSE;
    }
        
    if ( ! m_abort_updating_signal_was_entered ) {  
        // Give a notice that we are starting, as takes a few secs to spark up python 
        m_progress_listbox->Append( _( "Initializing Plucker spidering engine..." ) );  
        // Set the channels updated gauge (and statictext) to say zero of total complete
        set_channels_updated_gauge( 0, (int)number_of_sections );
    }
    
    // Before calling plucker-build, need to set close_on_error and close_on_exit
    // to true in [<os>] ( and also [DEFAULT] if it is there too ) or the hidden process
    // won't ever terminate (since its waiting for a keypress which can't occur because it 
    // is hidden). 2nd argument only sets=1 if key exists, 3rd argument flushes config.
    enable_close_on_exit_and_error( "DEFAULT", TRUE, FALSE );
    enable_close_on_exit_and_error( get_os_configuration_section(), FALSE, TRUE );
    
    // Do our loop of updating
    for ( current_section_array_index = 0; 
          (int) current_section_array_index < (int) number_of_sections;
          current_section_array_index++ ) {
        
        // Abort loop if a desire to close dialog was entered
        if ( m_abort_updating_signal_was_entered ) {  
            break;
        }
        
        // Empty command string from previous iteration of the loop.
        command_string.Empty();
        // Rezero the progress guage from last iteraion
        XMLCTRL( *this, "progress_dialog_current_channel_gauge", wxGauge )->SetValue( 0 ); 
        
        // Get the current channel section from the array
        current_section_string = channel_sections.Item( current_section_array_index ); 
        
        // In case there is a close_on_exit, close on error in this channel's section, 
        // (shouldn't be if created with Plucker Desktop), set those to 1 too.
        // Second argument only writes if keys existed, 3rd argument forces a flush 
        enable_close_on_exit_and_error( current_section_string, FALSE, TRUE );
        
        // Write a separator to textctrl that tells what channel we are about to spider
        wxString doc_name_key;
        doc_name_key << '/' << current_section_string << '/' << "doc_name";
        wxString doc_name = the_configuration->Read( doc_name_key, "" );
        m_progress_listbox->Append( " " );  
        m_progress_listbox->Append( "-----------------------------------------------------------" );  
        m_progress_listbox->Append( _( "Updating channel: " ) + doc_name + "..." );  
        m_progress_listbox->Append( "-----------------------------------------------------------" ); 
                
        // Set the initial static text for the channel. If verbosity=0, then it will 
        // never change for the entire crawl, so leave a message saying why. Hide the
        // progress gauge if verbosity=0 so user doesn't think something is stuck.
        bool verbosity;
        verbosity = the_configuration->Read( "/" + current_section_string + "/" +
                                             "verbosity", 1L );                    
        if ( verbosity == 0 ) {
            XMLCTRL( *this, "progress_dialog_current_channel_value_statictext", wxStaticText )
                ->SetLabel( _( "Processing " ) + doc_name + " (Updating in quiet mode)" );
            if ( XMLCTRL( *this, "progress_dialog_current_channel_gauge", wxGauge )->IsShown() ) {
                XMLCTRL( *this, "progress_dialog_current_channel_gauge", wxGauge )->Show( FALSE );
            }
        } else {
            XMLCTRL( *this, "progress_dialog_current_channel_value_statictext", wxStaticText )
                ->SetLabel( _( "Initializing channel: " ) + doc_name + "..." );
            if ( ! XMLCTRL( *this, "progress_dialog_current_channel_gauge", wxGauge )->IsShown() ) {
                XMLCTRL( *this, "progress_dialog_current_channel_gauge", wxGauge )->Show( TRUE );
            }
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

        // Assemble the command string and call our redirecting execute function.
        command_string << "plucker-build" << " -s " << current_section_string;         
        wxLogDebug( "Executing command=" + command_string );            
        execute_command_redirected_to_listbox( command_string );
        
        // Abort loop if a desire to close dialog was entered
        if ( m_abort_updating_signal_was_entered ) {  
            break;
        }
                
        // Set the update_base to its new value in configuration
        m_progress_listbox->Append( "Setting channel's new due date" );
        the_plucker_controller->set_channel_update_base( current_section_string );  
        
        // Abort loop if a desire to close dialog was entered
        if ( m_abort_updating_signal_was_entered ) {  
            break;
        }
        
        // Advance the gauge (+1 since array starts at zero)
        set_channels_updated_gauge( (int)current_section_array_index + 1, 
                                    (int)number_of_sections );
    }       
    
    // Abort loop if a desire to close dialog was entered
    if ( m_abort_updating_signal_was_entered ) {  
        return FALSE;
    }
    
    // Install or not, depending on the parameter passed into to this function
    if ( install ) {        
        wxString start_install_message = _( "Installing channel output to destinations..." );
        wxString end_install_message   = _( "Installation to destinations complete." );
        
        // Show start messages
        m_progress_listbox->Append( start_install_message  ); 
        XMLCTRL( *this, "progress_dialog_current_channel_value_statictext", wxStaticText )
            ->SetLabel( start_install_message );
            
        // Do the actuall installing    
        the_plucker_controller->install_channels( channel_sections );
        
        // Show end messages
        m_progress_listbox->Append( end_install_message  );
        XMLCTRL( *this, "progress_dialog_current_channel_value_statictext", wxStaticText )
            ->SetLabel( end_install_message );
    }   

    // Flush the config file, since changed the update_base values
    the_configuration->Flush();
    
    // Abort loop if a desire to close dialog was entered
    if ( m_abort_updating_signal_was_entered ) {  
        return FALSE;
    }
    
    // If a after_group_command was specified, execute it. TRUE means wait for it to finish
    wxString after_group_command;
    after_group_command = the_configuration->Read( "/PLUCKER_DESKTOP/after_group_command", wxT( "" ) );
    if ( after_group_command != wxT( "" ) ) {
        m_progress_listbox->Append( _( "Executing command after spidering channels..." ) );
        wxExecute( after_group_command, TRUE );
    }    
    
    return TRUE;
}


bool progress_dialog::execute_command_redirected_to_listbox( const wxString& command_string )
{
    process_data_type   pdata;
    piped_process       *process;

    process = new piped_process( this, &pdata );
    
    // FALSE argument tells to make it execute async instead of sync (meaning dont wait for 
    // the program to be done before continuing).
    // Store the pid (process id) that is returned from it so, can be killed with the stop 
    // button if user wants to abort.
    m_most_recent_pid = wxExecute( command_string, FALSE, process );

#ifdef __WXMSW__    
    // MSW can have long unsigned as process ids. We ifdef just to avoid a compiler
    // warning on GCC that we have mismatched unsigned and long unsigned.
    wxLogDebug( "Started execution. m_most_recent_pid=%lu", m_most_recent_pid );
#else
    wxLogDebug( "Started execution. m_most_recent_pid=%u", m_most_recent_pid );
#endif

    // Show a warning box if couldn't execute it. Also set our m_is_process_running 
    // accordingly, ready for our examination in kill_most_recent_process()
    if ( m_most_recent_pid == 0 ) {
        m_is_process_running = FALSE;
        wxLogError( "Error: cannot execute: " + command_string );
        return FALSE;
    } else {
        m_is_process_running = TRUE;
    }

    while ( pdata.is_running ) {
        wxLogDebug( "Entered pdata.is_running loop. About to call process->has_input" );
        process->has_input();
        // Sleeps for plkrPROGRESS_DIALOG_DETAILS_REFRESH_DELAY milliseconds, 
        // then yields
        wxUsleep( plkrPROGRESS_DIALOG_DETAILS_REFRESH_DELAY ); 
        // Yield to update the display
        wxYield();       
    }
        
    //! \todo SOURCE PUT THIS BACK INSTEAD OF RETURN TRUE. Why was it turned off?
    //return pdata.exit_code == 0;
    return TRUE;
}


void progress_dialog::set_channels_updated_gauge( int value, int range )
{
    // Set the new value and range for the gauge
    XMLCTRL( *this, "progress_dialog_channels_updated_gauge", wxGauge )->SetValue( value );    
    XMLCTRL( *this, "progress_dialog_channels_updated_gauge", wxGauge )->SetRange( range );

    // Build a progress string for the static text
    wxString progress_string;
    progress_string.Printf( _( "Channels updated ( %ld of %ld )"
                             ), 
                               value,
                               range
                          );
    // Set the static text
    XMLCTRL( *this, "progress_dialog_channels_updated_value_statictext", wxStaticText )
        ->SetLabel( progress_string );
}


void progress_dialog::set_current_channel_gauge( int value, int range )
{   
    // To avoid flickering jumps forward and back on the progress bar since we can't
    // simulateously set both a new range and value, for this gauge, we instead always 
    // keep a fixed range of plkrFIXED_PERCENTAGE_GAUGE_RANGE (matching value in the XRC),
    // and calculate a new range based on the % of value to range that was passed. .
    
    int new_value = 0;
    // If not diving by zero...
    if ( range != 0 ) {
        // ...Get the new value on a range of 1000, based on value/range that was passed.
        // Needed a double value, since 
        new_value = ( plkrFIXED_PERCENTAGE_GAUGE_RANGE * value ) / range;
    }    
    // Set the new value
    XMLCTRL( *this, "progress_dialog_current_channel_gauge", wxGauge )->SetValue( new_value ); 
    wxLogDebug( "plkrFIXED_PERCENTAGE_GAUGE_RANGE * value=%d. plkrFIXED_PERCENTAGE_GAUGE_RANGE * range=%d, new_value=%d", value, range, new_value );

    // Build a progress string for the static text
    wxString progress_string;
    progress_string.Printf( _( "Retrieving files for this channel ( %d of %d )"
                             ), 
                               value,
                               range
                          );
    // Set the static text
    XMLCTRL( *this, "progress_dialog_current_channel_value_statictext", wxStaticText )
        ->SetLabel( progress_string );
}


void progress_dialog::enable_close_on_exit_and_error( const wxString& section, 
                                                      bool only_write_if_key_exists,
                                                      bool flush )
{
    wxString close_on_exit_key  = "/" + section + "/" + "close_on_exit";
    wxString close_on_error_key = "/" + section + "/" + "close_on_error";
    
    // Write the keys if already exist, or we specified that want to write them anyways.
    if ( the_configuration->Exists( close_on_exit_key ) || ! only_write_if_key_exists ) {
        the_configuration->Write( close_on_exit_key, 1L );
    }
    if ( the_configuration->Exists( close_on_error_key ) || ! only_write_if_key_exists ) {
        the_configuration->Write( close_on_error_key, 1L );
    }

    // Flush changes if so requested
    if ( flush ) {
        the_configuration->Flush();
    }
}


bool progress_dialog::kill_most_recent_process() 
{
    wxLogDebug( "Entering progress_dialog::kill_most_recent_process()" );
    
    // Check if there is a process still running, by checking our member variable 
    // m_is_process_running. Note that we use this way instead of 
    // wxProcess::Exists( m_most_recent_pid ) for 2 reasons:
    // (1) If our process ended with that pid and another program started up a process
    // and picked the now available pid, we would then kill another programs process.
    // (2) Borland BCC [on wx231 anyways] asserts on wxProcess::Exists( int pid ) 
    // anyways.
    if ( m_is_process_running ) {
    
        wxLogDebug( "Process %u is running. Attempting to kill it...", m_most_recent_pid );
        
        // Try to kill it, retrieving the error number...
        // Note: change wxSIGTERM as required to any of the others.
        wxKillError kill_error_number = wxProcess::Kill( m_most_recent_pid, wxSIGTERM );
        
        // Update m_is_process_running to FALSE, as no sense trying to kill it more
        // than once. 
        m_is_process_running = FALSE;
        
        // ...And put a log message to show a more meaningful message that was returned
        // MSW on BCC is returning -4 as the code, even if successful so don't show any 
        // error popups if you didn't get the desired one.
        static const wxChar *kill_error_text[] = { "No error",
                                                   "Signal not supported",
                                                   "Permission denied",
                                                   "No such process",
                                                   "Unknown ending message",
                                                 };

        wxLogDebug( "Finished kill attempt of process=%u. Message number=%d (%s)",
                    m_most_recent_pid, kill_error_number, kill_error_text[ kill_error_number ] );
        return TRUE; 
        
    // Otherwise, just note in debug that this process doesn't exist.    
    } else {
        wxLogDebug( "m_is_process_running=FALSE, so process pid=%u not exist, or ended. No kill needed.",
                    m_most_recent_pid );
        return TRUE;
    }
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void progress_dialog::on_show( wxPaintEvent& event )
{
    wxLogDebug( "Entering progress_dialog::on_show()" );
    // If the dialog returns true from event's GetShow() function, that means it is 
    // currently hidden now, and this about thus is about to switch it to a shown state
   if ( m_is_initial_focus_event ) { 
        m_is_initial_focus_event = FALSE;                   
        
        // Update the channels
        update_channels( m_channel_sections, m_install );  
        
        // Run the code snippets to run after finished updating channels
        on_update_channels_completion(); 
    }    
    // Skip up to the next available handler
    event.Skip();
}


void progress_dialog::on_update_channels_completion()
{
    // Change the stop button label to OK
    XMLCTRL( *this, "progress_dialog_stop_button", wxButton )->SetLabel( _( "OK" ) );

    // Read close behaviour from config file.
    bool progress_dialog_close_on_exit;
    bool progress_dialog_close_on_error;  
         
    progress_dialog_close_on_exit = 
        (bool)the_configuration->Read( "/PLUCKER_DESKTOP/progress_dialog_close_on_exit", 0L );
    progress_dialog_close_on_error =
        (bool)the_configuration->Read( "/PLUCKER_DESKTOP/progress_dialog_close_on_error", 0L );
    
    if ( progress_dialog_close_on_exit && progress_dialog_close_on_error ) {    
        // Always close the dialog, regardless of whether an error occurred.
        close_dialog();          
    } else if ( progress_dialog_close_on_exit && ! progress_dialog_close_on_error ) {        
        // If no stderr erupted from piped_process::has_input, then shut down the dialog
        if ( ! m_error_occurred ) {
          close_dialog();  
        }        
    } 
    // Otherwise just leave it alone and wait for user to press "OK" or the "X" to close.
}


void progress_dialog::on_stop_button( wxCommandEvent &event )
{   
    wxLogDebug( "Entering progress_dialog::on_stop_button()" );
    // Do our close dialog code (kill process, etc)      
    close_dialog();    
}


void progress_dialog::on_details_button( wxCommandEvent &event )
{
    if ( m_progress_listbox_is_shown ) {
        hide_listbox();
    } else {
        show_listbox();
    }
}


void progress_dialog::on_export_button( wxCommandEvent &event )
{
    m_progress_listbox->export_to_file();
}

//----------------------------------------------------------------------------------------
// Piped process functions
//----------------------------------------------------------------------------------------

piped_process::piped_process( progress_dialog *parent, process_data_type *process_data )
              : m_process_data( process_data )         
{    
    m_parent                   = parent;
    m_process_data->is_running = TRUE;
    // Turn on redirection for the process, so that it can be caught by the GetInputStream()
    // and GetErrorStream() functions in piped_process::has_input().    
    Redirect();            
}


bool piped_process::has_input()
{
    bool has_input = FALSE;

    // Open a stream to stderr. Note: "GetInputStream()" actually gets the stdout of 
    // the external program being executed "GetOutputStream()" would open a line to the 
    // external program being executed.
    wxInputStream* stderr_stream = GetErrorStream();
    if ( stderr_stream && ! stderr_stream->Eof() ) {
        wxTextInputStream text_istream_of_stderr_stream( *stderr_stream );

        // This assumes that the output is always line buffered
        wxString stderr_string;

        // Dump the stream into our string
        stderr_string << text_istream_of_stderr_stream.ReadLine();
        
        // Put the string in the listbox
        m_parent->get_listbox()->Append( stderr_string );        
        // Look for meaningful information in the string to update dialog gauges, etc.
        m_parent->extract_progress_from_stderr( stderr_string );
        
        /* TODO: this doesn't work any more, now that progress is in stderr.
        // Tell the parent dialog that an error occurred
        m_parent->set_error_occurred( TRUE );
        */        
        
        has_input = TRUE;
    }

#if ( setupUSE_CATCH_STDOUT_FROM_PLUCKER_BUILD )
// NOTE: stdout is currently unused, since new plucker-build now sends all the progress
// information to stderr, not stdout. This is just kept in as an example of how someone
// could also catch the stdout of a process and send it to the listbox.  Also useful
// if using the old Plucker 1.1.3 parser, which did write progress to stdout.

    wxInputStream* stdout_stream = GetInputStream();
    if ( stdout_stream && ! stdout_stream->Eof() ) {
        wxTextInputStream text_istream_of_stdout_stream( *stdout_stream );

        // This assumes that the output is always line buffered
        wxString stdout_string;
        // Assemble the string
        stdout_string << text_istream_of_stdout_stream.ReadLine();
        // Place the string in the listbox
        m_parent->get_listbox()->Append( stdout_string );
        m_parent->extract_progress_from_stderr( stdout_string );
        
        has_input = TRUE;
    }    
#endif  // setupUSE_CATCH_STDOUT_FROM_PLUCKER_BUILD 
    

/* Some attempts at refresh in GTK

wxLogDebug( "Top level yield in has input" ); 
#if ( GTK_TEST_OF_APPYIELD )
        wxLogDebug( " Calling wxTheApp()->Yield( TRUE )" );
        wxTheApp->Yield( TRUE );
#endif

*/
    
    return has_input;
}


void piped_process::OnTerminate( int pid, int status )
{
    wxLogDebug( "Entering piped_process::OnTerminate. Showing rest of stdout/stderr" );
    // Go into a endless loop, showing the stderr/stdout, while has_input remains true
    while ( has_input() ) {}    
    
    // Mark this process as no longer running 
    // (needed in progress_dialog::execute_command_redirect_to_listbox() )
    m_process_data->is_running = FALSE;
    
    // Tell the parent progress dialog too that there is nothing running 
    // (used by kill_most_recent_process()
    // TODO-GURU: I think this should go before the OnTerminate, so that don't try 
    // to terminate it twice.
    m_parent->set_is_process_running( FALSE );
    
    m_process_data->exit_code = status;
    wxProcess::OnTerminate( pid, status );
}