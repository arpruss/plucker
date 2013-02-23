//----------------------------------------------------------------------------------------
// Name:        piped_process.cpp
// Purpose:     Desribes piped_process
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: piped_process.cpp,v 1.5 2007/06/29 00:58:54 robertoconnor Exp $

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "piped_process.h"
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

#include "piped_process.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/txtstrm.h"
#include "wx/process.h"

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( piped_process, wxProcess )

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

piped_process::piped_process( process_progress_dialog *parent, int id ) 
                   : wxProcess( parent, id )
{
    // Save the pointer to the parent.
    m_parent = parent;
   
    // Turn on redirection.
    Redirect();
}


bool piped_process::suck_process_output( bool after_process_terminated )
{
    wxLogDebug( wxT( "Entering piped_process::suck_process_output..." ) );
    wxLogDebug( wxT( "Parent's stream kind=%ld," ), m_parent->get_stream_kind() );

    // We will return TRUE if one of the streams had some input.
    bool has_input = FALSE;

    // If we specified we wanted to capture this type of stream...
    if ( m_parent->get_stream_kind() & optionPROCESS_PROGRESS_DIALOG_STDERR_STREAM_KIND ) 
    {    
        // ...call wxProcess::GetErrorStream() to get the stream...
        wxInputStream* captured_stderr_stream = GetErrorStream();
        wxLogDebug( wxT( "Got captured_stderr_stream" ) );
                
        // While the stream isn't NULL and isn't at the end...
        // NOTE: The test of captured_stderr_stream != NULL is FUNDAMENTAL here.
        // Otherwise on the initial iteration, the wxTextInputStream may be empty and
        // thus go on forever looking for a character to read. The exec sample in wx
        // doesn't have this, but it absolutely needs to be here.
        if ( captured_stderr_stream && ! captured_stderr_stream->Eof() )
        {
            // Make a wxTextInputStream from our captured stream.
            wxLogDebug( wxT( "Entered while captured_stderr_stream loop" ) );
	        wxTextInputStream textinput_stderr_stream( *captured_stderr_stream );

            // Read a line from our text input stream.
	        wxString line_text = textinput_stderr_stream.ReadLine();

            // Abort if requested.
            if ( m_parent->get_abort_signal_was_entered() ) 
            {
                return FALSE;
            }

            // Yield to all the display to be updated. (Not used in a OnIdle approach).
            // wxTheApp->Yield( TRUE );

            // If the length of the line isn't zero...
	        if ( line_text.Length() ) 
            {    
                // Then notify that an oputput line was generated, telling what stream it 
                // was, the text line generated, and what commandline generated it.
                m_parent->on_process_generated_an_output_line(
                                 optionPROCESS_PROGRESS_DIALOG_STDERR_STREAM_KIND, 
                                 line_text,
                                 m_parent->get_current_commandline_array_index()
                                 );
                wxLogDebug( wxT( "piped_process::suck_process_output (after_terminated=%d) finished generating another stderr line." ), (int)after_process_terminated );        
            }
            // Return TRUE, so we know to call this again.
            has_input = TRUE;
        }           
    }

    // Similar flow as stderr above, but with stdout.
    if ( m_parent->get_stream_kind() & optionPROCESS_PROGRESS_DIALOG_STDOUT_STREAM_KIND ) 
    {
        // Note that it is wxProcess::GetInputStream() to capture stdOUT of the external 
        // program being executed. wxProcess::GetOutputStream() would open a stdIN to the 
        // external program being executed.
        wxInputStream* captured_stdout_stream = GetInputStream();
        wxLogDebug( wxT( "Got captured_stdout_stream" ) );

        if ( captured_stdout_stream && ! captured_stdout_stream->Eof() ) 
        {
            wxLogDebug( wxT( "Entered while captured_stdout_stream loop" ) );
	        wxTextInputStream textinput_stdout_stream( *captured_stdout_stream );

            wxLogDebug( wxT( "Made TextInputStream" ) );
  
  	        wxString line_text = textinput_stdout_stream.ReadLine();
            wxLogDebug( wxT( "Read a line." ) );
            if ( m_parent->get_abort_signal_was_entered() ) 
            {
                return FALSE;
            }

            // Yield to all the display to be updated. (Not used in a OnIdle approach).
            // wxTheApp->Yield( TRUE );
            
	        if ( line_text.Length() ) 
            {
                m_parent->on_process_generated_an_output_line( 
                                optionPROCESS_PROGRESS_DIALOG_STDOUT_STREAM_KIND, 
                                line_text,
                                m_parent->get_current_commandline_array_index()
                                );
                wxLogDebug( wxT( "piped_process::suck_process_output (after_terminated=%d) finished generating another stderr line." ), (int)after_process_terminated );        
            }        

            has_input = TRUE;
        
        }           
    }
    
    wxLogDebug( wxT( "piped_process::suck_process_output() reached end of function" ) );
    return has_input;
}


int piped_process::get_process_pid()
{
    return m_process_pid;
}
    

void piped_process::set_process_pid( int process_pid )
{
    m_process_pid = process_pid;
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ENHANCED_PROGRESS_DIALOG
