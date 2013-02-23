//----------------------------------------------------------------------------------------
// Name:        commandline_parser.cpp
// Purpose:     Wrappers to deal with the commandline interface.
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: commandline_parser.cpp,v 1.10 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "commandline_parser.h"
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

#include "commandline_parser.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/module.h"				// wxModule

//----------------------------------------------------------------------------------------

#include "configuration.h"
#include "plucker_controller.h"
#include "build_progress_wrappers.h"
#include "utils_datetime.h"

//----------------------------------------------------------------------------------------
// Internal variables
//----------------------------------------------------------------------------------------

commandline_parser* commandline_parser::ms_instance = NULL;

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

commandline_parser::commandline_parser()
                   : wxCmdLineParser()
{
}

// NO destructor

// Static
commandline_parser* commandline_parser::get()
{
    wxLogDebug( wxT( "Entering image_list::get()" ) );

    if ( ! ms_instance ) 
    {

        ms_instance = new commandline_parser();
    }

    // Return ms_instance (which will stay as NULL if
    return ms_instance;
}


// Static
commandline_parser* commandline_parser::set( commandline_parser *desired_commandline_parser )
{
    commandline_parser* old_commandline_parser = ms_instance;
    ms_instance = desired_commandline_parser;
    return old_commandline_parser;
}


bool commandline_parser::do_commandline( int argc, wxChar** argv )
{
    int application_return_code;

    // This will get rid of the time stamps preceeding log messages, as they make the
    // console output harder to read.
    // Vaclav has this line inside a !defined(__WXMSW__)
    wxLog::SetTimestamp( NULL );

    // This will put messages to the console instead of Log dialogs.
    // MSW won't print things to the console, so leave it as Log dialogs on that platform.
#ifndef __WXMSW__
    delete wxLog::SetActiveTarget( new wxLogStderr );
#endif

    static const wxCmdLineEntryDesc commandline_description[] =
    {
        { wxCMD_LINE_SWITCH, wxT( "h" ),  wxT( "help" ),              wxT( "Show help message" ),
              wxCMD_LINE_VAL_NONE,   wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, wxT( "s" ),  wxT( "update-selected" ),   wxT( "Update selected channels" ) },
        { wxCMD_LINE_SWITCH, wxT( "p" ),  wxT( "use-sections" ),      wxT( "Specify channels by section, not doc_name" ) },
        { wxCMD_LINE_SWITCH, wxT( "d" ),  wxT( "update-due" ),        wxT( "Update due channels" ) },
        { wxCMD_LINE_SWITCH, wxT( "a" ),  wxT( "update-all" ),        wxT( "Update all channels" ) },
        { wxCMD_LINE_PARAM,  NULL, NULL,  wxT( "Channels" ),
              wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_NONE }
    };

    SetCmdLine( argc, argv );

    SetDesc( commandline_description );

    // Add some extra info in case they call help or there is an error.
    prepend_extra_help_notice();

#ifndef __WXMSW__
    // Put an initial blank line to the console. Makes reading an error easier.
    send_inform_message( wxT( "\n" ) );
#endif

    // Parse the commandline, which will return either a case of -1, 0 or a positive number
    switch ( Parse() ) 
    {
        // Case -1 if it was just a --help request.
        case -1:
            application_return_code = 0;
            break;
        // Case 0 if parsing went OK.
        case 0:
            parse_parameters();
            application_return_code = 0;
            break;
        // Case po
    }

    // Put log target back to where it was.
#ifndef __WXMSW__
    delete wxLog::SetActiveTarget( new wxLogGui );
#endif

    //wxGetApp()->set_application_return_code( application_return_code );
    return TRUE;
}


void commandline_parser::prepend_extra_help_notice()
{
    wxString logo_string;
    logo_string
    << wxT( "\n" )
    << wxT( "------------------------------------------------------------\n" )
    << plkrAPPLICATION_DESKTOP_NAME << wxT( " " ) << _( "Commandline Interface" )
    << wxT( "\n" ) <<   _( "Version" ) << wxT( ": " ) << plkrPLUCKER_DESKTOP_VERSION_STRING
    << wxT( "  " ) << _( "Build Date" ) << wxT( ": " ) << utils_datetime::get_translated_compilation_date_string() << wxT( "\n" )
    // \n put outside the string, so it matches the about.xrc. One less string to translate.
    <<   _( "(c) Copyright 1998-2003" ) << wxT( " " ) << plkrAPPLICATION_PUBLISHER << wxT( " " ) << wxT( "\n" )
    << wxT( "------------------------------------------------------------\n" )
    <<   _( "Updates selected channels (-s), due channels (-d), or all channels (-a)." ) << wxT( " \n\n" )
    <<   _( "If updating selected channels, (-p) specifies channel by their section names" ) << wxT( "\n" )
    <<   _( "in your plucker.ini/.pluckerrc, instead of their channel names." ) << wxT( " \n\n" )
    <<   _( "If you specify no options, then the application will start in GUI mode." ) << wxT( "\n\n" )
    <<   _( "See online help for more information." ) << wxT( "\n" )
    << wxT( "------------------------------------------------------------\n" )
    <<   _( "Summary of options" ) << wxT( ":\n" );

    // wxwxCmdLineParser::SetLogo() will print this string before the usage table.
    SetLogo( logo_string );
}


void commandline_parser::parse_parameters()
{
    bool flag_update_selected = Found( wxT( "s" ) );
    bool flag_use_sections    = Found( wxT( "p" ) );
    bool flag_update_due      = Found( wxT( "d" ) );
    bool flag_update_all      = Found( wxT( "a" ) );

    // If the user tried to put multiple commands in (tested by casting the bools to ints,
    // then adding them all up to see if greater than 1...
    if ( 1 < (int)flag_update_selected + (int)flag_update_due + (int)flag_update_all ) 
    {
        // ... then show a message and abort.
       send_inform_message( _( "Syntax error: You have specified multiple commands. Choose just one of (-s), (-d), or (-a)." ) );
       Usage();
       return;
    }

    // Two arrays. One to hold the commandline ones, and one to hold channel sections.
    wxArrayString commandline_channel_array;
    wxArrayString channel_sections_to_update;

    for ( size_t i = 0; i < GetParamCount(); i++ ) 
    {
        commandline_channel_array.Add( GetParam( i ) );
        wxLogDebug( wxT( "Adding to m_commandline_channel_array: " ) + GetParam( i ) );
    }

    // Handle update selected case
    if ( flag_update_selected ) 
    {
        // If didn't inlclude any channels in commandline...
        if ( GetParamCount() < 1 )
        {
            // Show message
            send_inform_message( _( "Syntax error: To use 'update selected', you must specify some channels." ) );
            // Print the usage info
            Usage();
        } 
        else 
        {
            // If specified sections, we can just update those directly...
            if ( flag_use_sections ) {
                // ...however, we must ensure that none of those sections are phony, as they
                // will result in creating that section and writing an update base,
                // and close_on_exit/error key for that section.
                channel_sections_to_update = commandline_channel_array;
                for ( size_t i=0; i < channel_sections_to_update.GetCount(); i++ )
                {
                    if ( ! the_configuration->HasGroup( channel_sections_to_update.Item( i ) ) )
                    {
                        send_inform_message( _( "Warning: plucker.ini/.pluckerrc has no section=" ) + channel_sections_to_update.Item( i ) );
                        channel_sections_to_update.RemoveAt( i );
                    }
                }
            // ...otherwise, we look up the sections corresponding to the doc_names first.
            } 
            else 
            {
                plucker_controller::get()->get_channel_sections_by_doc_names( commandline_channel_array,
                                                                              channel_sections_to_update );
                if ( channel_sections_to_update.IsEmpty() ) 
                {
                    send_inform_message( _( "Warning: None of those channel doc_names could be found in plucker.ini/.pluckerrc." ) );
                }
            }
        }
    }

    // Handle update due case
    if ( flag_update_due ) 
    {
        plucker_controller::get()->get_due_channels( channel_sections_to_update );
        // If there is none due, we will just exit silently, as the docs say. If do a
        // send_inform_message(), then will get a log box pop up on Windows, which is
        // annoying if automated.
    }

    // Handle update all case
    if ( flag_update_all ) 
    {
        plucker_controller::get()->get_all_channels( channel_sections_to_update );

        if ( channel_sections_to_update.IsEmpty() ) 
        {
            send_inform_message( _( "You have no channels in your plucker.ini/.pluckerrc file. Add some channels first." ) );
        }
    }

    // Lookup desired spidering mode.
    wxString spidering_display_mode;
    spidering_display_mode = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/spidering_display_mode" ),
                                                      wxT( "dialog" ) );

    // If there is at least one channel to update is already handled in the progress dialog and
    // plucker_controller->update_channels() code.

    // If want console...
    if ( spidering_display_mode == wxT( "console" ) ) 
    {
        // ...do it the console way.
        plucker_controller::get()->update_channels( channel_sections_to_update );
    } 
    else 
    {

        // ..else launch progress dialog. Parent window is NULL.
        build_progress_wrappers::launch_build_progress_dialog( (wxWindow* )NULL,
                                                                channel_sections_to_update
# if ( setupUSE_MSW_FORCE_COMMANDLINE_AUTOCLOSE_PROGRESS_CLOSURE )
                                                               , optionFORCE_PROGRAM_TERMINATION_ON_PROGRESS_DIALOG_CLOSE

#endif
                                                              );

    }

    // Clear arrays to free memory.
    channel_sections_to_update.Clear();
    commandline_channel_array.Clear();
}


void commandline_parser::send_inform_message( const wxString& message )
{
    wxLogMessage( message );
}

//----------------------------------------------------------------------------------------
// Module definition
//----------------------------------------------------------------------------------------

class module_commandline_parser : public wxModule
{

    DECLARE_DYNAMIC_CLASS( module_commandline_parser )

public:

    module_commandline_parser() {}

    bool OnInit()
    {
        return TRUE;
    }

    void OnExit()
    {
        // We set it to null, and a pointer returns to the previous one, which we delete.
        delete commandline_parser::set( NULL );
    }

};

IMPLEMENT_DYNAMIC_CLASS( module_commandline_parser, wxModule )
