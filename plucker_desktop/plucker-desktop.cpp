//----------------------------------------------------------------------------------------
// Name:        plucker-desktop.cpp  
// Purpose:     Core application
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: plucker-desktop.cpp,v 1.48 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------
#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "plucker-desktop.h"
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
#include "plucker-desktop.h"
//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------
#include "wx/image.h"               // wxImage
#include "wx/splash.h"              // wxSplashScreen
#include "wx/msgdlg.h"              // wxMessageDialog
#include "wx/fs_zip.h"              // wxZipFSHandler [for wxHtmlHelpController]
#if ( setupUSE_SHOWCASE )
    #include "wx/fs_inet.h"         // wxInternetFSHandler [wxHtmlWindow->LoadPage(http:)]
#endif
#if ( setupUSE_DRAG_AND_DROP )
    #include "wx/clipbrd.h"         // Setting the clipboard as primary selection
#endif
#if ( setupUSE_MAC_NATIVE_LISTCTRL_INSERTS_IN_WRONG_INDEX_AND_CANT_REMOVE_ARROWS_FROM_HEADERS_HACK )
	#include "wx/sysopt.h"			// wxSystemOptions
#endif
// ---------------------------------------------------------------------------------------
#include "wx/xrc/xmlres.h"          // XRC XML resouces
// ---------------------------------------------------------------------------------------
#include "configuration.h"
#include "plucker_controller.h"
#include "main_frame.h"
#include "plucker_wizard_wrappers.h"
#include "internationalization.h"
#include "splashscreen.h"
#include "startup_tips.h"
#include "art_provider.h"
#include "commandline_parser.h"
#include "import_controller.h"
//----------------------------------------------------------------------------------------
// wxWindows macro: Declare the application.
//----------------------------------------------------------------------------------------
// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. the_app and
// not wxApp).
IMPLEMENT_APP( the_app )
//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------
bool the_app::OnInit()
{
#if ( setupUSE_SINGLE_INSTANCE_CHECKER )
    //! \todo Fill in the second parameter for a better directory for the Unix lock file.
    m_single_instance_checker = new wxSingleInstanceChecker( GetAppName() );
    
    // And if a copy is alreay running, then abort...
    if ( m_single_instance_checker->IsAnotherRunning() ) 
    {
        // Note we can't brand the name of the application here, since it is before
        // the initialization of the config file.
        wxMessageDialog second_instance_messagedialog( (wxWindow*)NULL,
                  _( "Another instance of the application is already running, so aborting startup of second instance." ),
                  _( "Already running" ),
                  wxOK | wxICON_INFORMATION );
        second_instance_messagedialog.ShowModal();
        // Returning FALSE from within wxApp::OnInit() will terminate the application.
        return FALSE;
    }
#endif  // setupUSE_SINGLE_INSTANCE_CHECKER

    // Do a list of things that are done regardless of whether or not there is commandline
    // arguments
    on_startup_common();
    // "argc" is "Argument count". The program executable name itself counts as one.
    // If there was more than this 1 argument...
    if ( 1 < argc ) 
    {
        // ...then do a list of things for commandline execution:
        on_startup_with_commandline_arguments( argc, argv );
        // Returning FALSE from within wxApp::OnInit() now terminates the application,
        // since we are done now and want to exit.
        return FALSE;
    } 
    else 
    {
        // ...else do a list of things for normal gui startup:
        on_startup_without_commandline_arguments();
        // Returning TRUE from within wxApp::OnInit() now continues the application,
        // since we want the GUI application to keep running, responding to button
        // events etc.
        return TRUE;
    }
}


int the_app::OnExit( void )
{
    // Delete the configuration object when finished program, otherwise memory leak
    finish_configuration();
#if ( setupUSE_SINGLE_INSTANCE_CHECKER )
    // Delete the single instance checker
    wxLogDebug( wxT( "Deleting the single instance checker." ) );
    delete m_single_instance_checker;
#endif
    // wxApp::OnExit is virutal. We can return any program-specific integer we want
    // here as we end the program. We could just return '0' but we kept track of a better
    // one with the set_application_return_code() function.
    wxLogDebug ( wxT( "Completed the_app::OnExit(). Program exitting normally." ) );
    return m_return_code;
}

//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

void the_app::on_startup_common()
{
    // Initially have the application return code as 0.
    m_return_code = 0;
	// Add the ability for the program to handle XPM images.
    /*!
        \todo This is absolutely needed for wxGTK2.4.0, but seems to be a duplicate for 
        wxMac and wxWin (a debug message warns that its a duplicate handler, and the 
        wxMac and wxWin will work without adding it).
     */
    wxImage::AddHandler( new wxXPMHandler );
    // Call up our saved configuration file
    init_configuration();

#if ( setupUSE_IMPORT_CONTROLLER )
    // Has to come before the internationalization, so that we can correctly
    // set the internationalization too (or turn it off if so desired).
    import_controller::get()->import_installation_plkrdata_files( 0 );
#endif   // setupUSE_IMPORT_CONTROLLER

#if ( setupUSE_INTERNATIONALIZATION )
    // Called before the main frame creation, so that we can translate it too
    internationalization::init_internationalization();
#endif   // setupUSE_INTERNATIONALIZATION

    // Load up the XML Resource handler, to be able to load XML resources..
    wxXmlResource::Get()->InitAllHandlers();
}


void the_app::on_startup_without_commandline_arguments()
{
    // Load up enough XML resources to construct a main frame
    load_xrc_resource( wxT( "main_frame.xrc" ) );
    load_xrc_resource( wxT( "main_menubar.xrc" ) );
    load_xrc_resource( wxT( "main_toolbar.xrc" ) );
	
#if ( setupUSE_MAC_NATIVE_LISTCTRL_INSERTS_IN_WRONG_INDEX_AND_CANT_REMOVE_ARROWS_FROM_HEADERS_HACK )
	// Start using generic wxListCtrls instead of native wxListCtrls
	wxSystemOptions::SetOption( wxT( "mac.listctrl.always_use_generic") , 1 );
#endif
    
	// Create an instance of the main frame.
    // Using a pointer since behaviour will be when close the frame, it will
    // Destroy() itself, which will safely call "delete" when the time is right.
    // [The constructor is empty, since using the default value for parent, which is
    // NULL].
    main_frame* a_main_frame = new main_frame();
    wxLogDebug( wxT( "Created instance of main frame" ) );
    // This is of dubious usefulness unless an app is a dialog-only app. This
    // is the first created frame, so it is the top window already.
    SetTopWindow( a_main_frame );
    wxLogDebug( wxT( "Set main frame as top window" ) );
    // Create the plucker controller (handles add/save/delete of channels).
    // Needs to be before the display of splashscreen, since if can't access the Palm
    // Desktop software on Mac/MSW, it shows a modal message, and the splashscreen
    // obscures it, making it seem as though the program has frozen.
    plucker_controller::get();

#if ( setupUSE_SPLASHSCREEN )
    // Display the splashscreen, while we load up the handlers, XML resources, and build
    // the main dialog. Has to be after initialized the configuration though, because a
    // configuration setting is whether or not to show the splashscreen.
    // If it was a .png, would has to first added a wxPNGHandler. It is after
    // constructed the main frame, so that the main frame will be the top window,
    // (if splashscreen was top window, app would close when splashscreen goes)
    if ( the_configuration->Read( wxT( "/PLUCKER_DESKTOP/splashscreen_enabled") , 1L ) ) 
    {
        splashscreen::get( TRUE );
    }
#endif

    // Same idea as a InternetFSHandler but accesses files within .zip archives. Instead
    // of http: protocol it uses URLs of a zip: protocol, for example:
    // "myarchive.zip#zip:myfileinside". Used to zip up a suite of html files into a zip
    // archive to use with wxHtmlHelpController.
    wxFileSystem::AddHandler( new wxZipFSHandler );
    // Load up the rest of the XML resources
    load_xrc_resource( wxT( "channel_dialog.xrc" ) );
    load_xrc_resource( wxT( "editor_dialog.xrc" ) );
    load_xrc_resource( wxT( "exclusion_dialog.xrc" ) );
    load_xrc_resource( wxT( "details_progress_dialog.xrc" ) );
    load_xrc_resource( wxT( "preferences_dialog.xrc" ) );
    load_xrc_resource( wxT( "showcase_dialog.xrc" ) );
    load_xrc_resource( wxT( "about_dialog.xrc" ) );
    load_xrc_resource( wxT( "about_simple_dialog.xrc" ) );
    // Do the rest of the main_frame_startup
    a_main_frame->frame_startup_init();

#if ( setupUSE_SPLASHSCREEN )
    // Close the splashscreen, now that all done loading.
    // The FALSE in the get() means not to make one if don't have one. The
    // close has a check for instance so can't close something without an instance.
    splashscreen::get( FALSE )->close();
#endif

    // Show the main frame. Unlike most controls, frames don't show immediately upon
    // construction, but instead wait for a specific Show() function.
    a_main_frame->Show( TRUE );
    wxLogDebug( wxT( "Showing main frame" ) );
	
#if ( setupUSE_WIZARDS )
	// If first execution, then launch the setup wizard [plucker_desktop.cpp wrapper function]
    if ( the_configuration->Read( wxT( "/PLUCKER_DESKTOP/is_first_execution" ), 1L ) )
    {
        // If they clicked "Finish"...
        if ( plucker_wizard_wrappers::run_setup_wizard( a_main_frame ) ) 
        {
            // ...then reload the rows to show the possbile newly added channels.
            a_main_frame->get_main_listctrl()->reload_rows();
            // Note: it is somewhat wasteful to reload the rows after we already
            // loaded them above. But this is the first execution, and there
            // was very likely no rows before now anyways.
        }
        // ... and mark that is no longer the first run of the application.
        the_configuration->Write( wxT( "/PLUCKER_DESKTOP/is_first_execution" ), 0L );
    }
#endif

#if ( setupUSE_STARTUP_TIPS )    
    // Show startup tips.
    startup_tips::show_startup_tips( a_main_frame ); 
#endif

#if ( setupUSE_DRAG_AND_DROP )
    // under X we usually want to use the primary selection by default (which is shared
    // with other apps)
    wxTheClipboard->UsePrimarySelection();
#endif
    
	// Start the art provider, which will provide icons, including overrriding the
    // wxWindows ones I want to override.
    wxArtProvider::Push( new art_provider );
	
    // Now the application has completed wizards and all windows are constructed.
    // Inform the plucker_controller that it is free to start an autoupdate whenever
    // it feels the need (this value is initialized to FALSE in the plucker_controller
    // constructor and will stay so, until the application gives it the green light.
    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
}


void the_app::on_startup_with_commandline_arguments( int argc, wxChar** argv )
{
    // Load the details progress dialog from XRC.
    load_xrc_resource( wxT( "details_progress_dialog.xrc" ) );
    wxChar** temp_argv = argv;
    commandline_parser::get()->do_commandline( argc, temp_argv );
}


void the_app::set_application_return_code( int return_code )
{
    m_return_code = return_code;
}


void the_app::load_xrc_resource( const wxString& xrc_filename )
{
    wxString xrc_subdirectory = get_plucker_directory( RESOURCES ) << wxT( "/xrc" );
    wxString xrc_fullname;
    xrc_fullname << xrc_subdirectory << wxT( "/" ) << xrc_filename;
    if ( ::wxFileExists( xrc_fullname ) ) 
    {
        wxLogDebug( wxT( "Found file=" ) + xrc_fullname + wxT( " so will try to load" ) );
        wxXmlResource::Get()->Load( xrc_fullname );
    } 
    else 
    {
        wxLogError( wxT( "Error: can't find XRC file=" ) + xrc_fullname );
    }
}
