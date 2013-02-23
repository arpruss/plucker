//----------------------------------------------------------------------------------------
// Name:        help_controller.cpp
// Purpose:     Manages displaying of online help
// Author:      Robert O'Connor, Dominic Mazzoni
// Modified by:
// Created:     2001/10/20
// Copyright:   Dominic Mazzoni of Audacity ( audacity.sourgeforge.net ), Robert O'Connor
// Licence:     GPL
// RCS-ID:      $Id: help_controller.cpp,v 1.16 2007/06/13 01:49:49 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "help_controller.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_ONLINE_HELP )

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

#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "configuration.h"

//----------------------------------------------------------------------------------------
// Internal variables
//----------------------------------------------------------------------------------------

help_controller* help_controller::ms_instance = NULL;

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

help_controller::help_controller()
                :
                wxHtmlHelpController( wxHF_ICONS_FOLDER |
                                      wxHF_CONTENTS     |   
                                      wxHF_INDEX        |
                                      wxHF_SEARCH       |
                                      wxHF_FLAT_TOOLBAR |
                                      wxHF_BOOKMARKS
                                    )
{
    // Make a config file to hold the help settings, like bookmarks, etc.         
    wxString help_configuration_fullname = get_plucker_directory( WXWINDOWS_CONFIGURATION )
                  << wxT( "/") << get_custom_configuration_filename( wxT( "plucker-help-saved-settings") );

    // Not going to brand this
    m_help_configuration = new wxFileConfig( wxT( "Plucker Desktop Help"),
                                             wxT( "The Plucker Team"),
                                             help_configuration_fullname,
                                             wxEmptyString,
                                             wxCONFIG_USE_LOCAL_FILE
                                           );

    wxLogDebug( wxT("Help configuration object created") );

    // This reformats the title bar of the Help window to say:
    // Plucker Desktop Help: <section>, instead of the default title of
    // Help <section>. The string contains a single %s, which marks the replacement
    // of the active page section being viewed in the help.
    wxString title_string;
    title_string << plkrAPPLICATION_DESKTOP_NAME << wxT( " " ) << _( "Help" )
                << wxT( ": %s" );
    SetTitleFormat( title_string );

    // Get the filename of the plucker-desktop help file.
    wxString plucker_desktop_help_fullname = get_plucker_directory( DOCUMENTATION )
                                                 << wxT( "/plucker-desktop.htb");

    // Add the book with our custom function (which error handles, etc.)
    add_book( plucker_desktop_help_fullname );

    // Get the filename of the plucker-manual help file.
    wxString plucker_manual_help_fullname = get_plucker_directory( DOCUMENTATION )
                                                 << wxT( "/plucker-manual.htb");

    // Add the book with our custom function (which error handles, etc.)
    add_book( plucker_manual_help_fullname );

    // This both assigns a path to write cached binary versions of the index, and tells
    // the wxHtmlHelpController to use caching to vastly speed up the page to page
    // searchings. The 2 compiled cache files stay there between program executions,
    // and only are regenerated if the help file is replaced with a newer version
    // (similar to how .py and .pyc files work if your are a Python fan).
    // However, the help may not have have permissions in its own directory in
    // /usr/share/doc, so we can write it user's configuration directory for safety.
    //
    // SetTempDir( get_plucker_directory( PLUCKERHOME ) );
    //
    // However, the alternative, which we eventually use is the the hhp2cached utility
    // and distribute the cached files (a cleaner solution as it allows removal of the
    // files in the rpm/installer, though it does add another tool to the toolchain for
    // making the application).

    // This is an option to automatically have the help controller read and write its
    // own configuration properties, such as window position, sash positions, etc.
    // Just tell it what configuration object to read/write to (and optionally a
    // second argument of what path to use in that configuration file).
    UseConfig( m_help_configuration );
}


help_controller::~help_controller()
{
    wxLogDebug( wxT("Entering help_controller destructor") );

    // We will do our own WriteConfiguration, because by the time this destructor
    // finishes, and we go up to the regular wxHtmlHelpController destructor, the
    // config will be deleted, and hence it won't do any saving. Therefore, save
    // it now before we delete the m_help_configuration.
    WriteCustomization( m_help_configuration );

    if ( m_help_configuration ) 
    {
        wxLogDebug( wxT("About to delete the help configuration...") );
        delete m_help_configuration;
        m_help_configuration = NULL;
        wxLogDebug( wxT("Deleted help configuration.") );
    }

   wxLogDebug ( wxT("Finished help_controller destructor") );
}


help_controller* help_controller::get( bool create_on_demand )
{
    wxLogDebug( wxT( "Entering help_controller::get()") );

    if ( ! ms_instance && create_on_demand ) 
    {
        ms_instance = new help_controller();
    }

    // Return ms_instance (which will stay as NULL if
    return ms_instance;
}


// Static
help_controller* help_controller::set( help_controller *desired_help_controller )
{
    help_controller* old_help_controller = ms_instance;
    ms_instance = desired_help_controller;
    return old_help_controller;
}


void help_controller::add_book( const wxString& help_fullname )
{
    // Adds the file as a 'book' in the wxHtmlHelp controller.
    bool add_book_successful = AddBook( help_fullname );

    // If unable to do so, show a message.
    // NOTE: One of the reasons AddBook() might fail is that a zip file system handler
    // was never declared. A ZipFSHandler to be used throughout this application
    // was declared back in the_app::OnInit() [ see plucker-desktop.cpp ]
    if ( ! add_book_successful ) 
    {
        wxString error_message;
        error_message << _( "Couldn't open help file named" ) << wxT( "\n" ) << help_fullname;
        // I am using a log debug instead of a wxMessageBox() since some distro's
        // might not have all help installed.
        wxLogDebug( error_message );
    }
}


void help_controller::close()
{
    // If there is an instance (which may not be if called via get(FALSE)
    if ( ms_instance ) 
    {
        // ...and if there is a frame (ie it is showing its frame, not just existing)...
        if ( GetFrame() ) 
        {
            // ...then close the frame.
            GetFrame()->Close( TRUE ); 
            wxLogDebug( wxT("Closed the help controller frame") );
        }
    }
}


// Shows the help page. Currently accepts the value of an item in the Keyword list, 
// which is a flat list of all the page names.
void help_controller::show_help_topic( const wxString& topic )
{
    Display( topic );       
    wxLogDebug( wxT("Showing help topic for ") + topic );
}


void help_controller::show_help()
{
    // Note this will fail in GTK debug if there is no tree node by this name.
    // (which would happen if book nodes aren't included as top nodes.        
    // Display( wxT( "Plucker Desktop" ) );
    // Plus we defined the plkrAPPLICATION_DESKTOP_NAME as a translate _(), and
    // when translates, won't find that string.
    // A safer way is to used wxHelpController::DisplayContents();
    DisplayContents();      
}


void help_controller::search_help( wxWindow* parent )
{

    wxString keyword_to_search_for = wxGetTextFromUser( _( "Enter keyword to search for:" ),
                                                        _( "Search help for keyword" ),
                                                        wxT( ""),
                                                        parent );

    if ( ! keyword_to_search_for.IsEmpty() ) 
    {
        KeywordSearch( keyword_to_search_for );
    }
}

//----------------------------------------------------------------------------------------
// Module definition
//----------------------------------------------------------------------------------------

class module_help_controller : public wxModule
{

    DECLARE_DYNAMIC_CLASS( module_help_controller )
    
public:

    module_help_controller() {}
    
    bool OnInit()
    {
        return TRUE;
    }
    
    void OnExit()
    {
        wxLogDebug( wxT( "Entered module_help_controller::OnExit()") );
        // We set it to null, and a pointer returns to the previous one, which we delete.
        delete help_controller::set( NULL );
        wxLogDebug( wxT( "Completed module_help_controller::OnExit()") );
    }

};

IMPLEMENT_DYNAMIC_CLASS( module_help_controller, wxModule )

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ONLINE_HELP
