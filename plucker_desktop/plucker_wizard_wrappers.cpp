//----------------------------------------------------------------------------------------
// Name:        plucker_wizard_wrappers.cpp
// Purpose:     Describes plucker_wizard_wrappers class
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: plucker_wizard_wrappers.cpp,v 1.6 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "plucker_wizard_wrappers.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

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

#include "plucker_wizard_wrappers.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "setup_wizard.h"
#include "install_viewer_wizard.h"
#include "add_channel_wizard.h"
#include "configuration.h"

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

bool plucker_wizard_wrappers::run_setup_wizard( wxWindow* parent )
{
    // A bool to return of whether clicked finish, in case parent wants to handle it
    bool clicked_finish_button = FALSE;

    // Create a string for cancel message and the filename of our bitmap
    wxString cancel_message;
    wxString image_fullname;

    cancel_message << _( "The setup wizard is not complete." ) << wxT( "\n\n" )
        << _( "You can run this wizard again at a later time to complete the setup." )
        << wxT( "\n\n" ) << _( "Exit the setup wizard?" );

    image_fullname << get_plucker_directory( RESOURCES )
        << wxT( "/graphics_wizard/wizard_main.xpm" );

    // Make a title stirng
    wxString title_string;
    title_string << plkrAPPLICATION_DESKTOP_NAME << wxT( " " ) << _( "Setup Wizard" );

    // Make a new instance of the setup wizard, generating the wxBitmap, but loading
    // it from the above filename. wxBITMAP_TYPE_XPM tells us what wxImage hander to use.
    setup_wizard *a_setup_wizard = new setup_wizard( parent, -1,
                                   title_string,
                                   wxBitmap( image_fullname, wxBITMAP_TYPE_XPM ),
                                   cancel_message );
    // Tell it to run
    clicked_finish_button = a_setup_wizard->run_wizard();
    
    // All done. Destroy the object to free memory
    a_setup_wizard->Destroy();
    
        
    return clicked_finish_button;
}


bool plucker_wizard_wrappers::run_install_viewer_wizard( wxWindow* parent )
{

    // A bool to return of whether clicked finish, in case parent wants to handle it
    bool clicked_finish_button = FALSE;
    
    // Create a string for cancel message and the filename of our bitmap 
    wxString cancel_message;
    wxString image_fullname;    
     
    cancel_message << _( "The install handheld software wizard is not complete." ) << wxT( "\n\n" )
        << _( "You can run this wizard again at a later time to re-install the software." )
        << wxT( "\n\n" ) << _( "Exit the wizard?" );
        
    image_fullname << get_plucker_directory( RESOURCES ) 
        << wxT( "/graphics_wizard/wizard_main.xpm" );

    // Make a new instance of the setup wizard, generating the wxBitmap, but loading
    // it from the above filename. wxBITMAP_TYPE_XPM tells us what wxImage hander to use.
    install_viewer_wizard *a_install_viewer_wizard = new install_viewer_wizard( parent, -1,
                                   _( "Install Handheld Software Wizard" ),
                                   wxBitmap( image_fullname, wxBITMAP_TYPE_XPM ), 
                                   cancel_message );
    // Tell it to realize itself (chain pages, setup sizes, etc)
    
    // Tell it to run
    clicked_finish_button = a_install_viewer_wizard->run_wizard();
    
    // All done. Destroy the object to free memory
    a_install_viewer_wizard->Destroy();
    
    return clicked_finish_button;
}


// Returns whether finished wizard by clicking "Finish", 
// Collects the channel_section created amd whether to launch a channel dialog afterwards.
bool plucker_wizard_wrappers::run_add_channel_wizard( wxWindow* parent, 
            wxString* generated_channel_section, bool* is_launch_channel_dialog_requested )
{
    // A bool to return of whether clicked finish, in case parent wants to handle it
    bool clicked_finish_button = FALSE;
    
    // Create a string for cancel message and the filename of our bitmap
    wxString cancel_message;
    wxString image_fullname;    
     
    cancel_message << _( "The add channel wizard is not complete." ) << wxT( "\n\n" )
        << _( "You can run this wizard again at a later time to add the channel." )
        << wxT( "\n\n" ) << _( "Exit the wizard?" );
        
    image_fullname << get_plucker_directory( RESOURCES ) 
        << wxT( "/graphics_wizard/wizard_main.xpm" );
   
    // Make a new instance of the setup wizard, generating the wxBitmap, but loading
    // it from the above filename. wxBITMAP_TYPE_XPM tells us what wxImage hander to use.
    add_channel_wizard *a_add_channel_wizard = new add_channel_wizard( parent, -1,
                                   _( "Add New Channel Wizard" ),
                                   wxBitmap( image_fullname, wxBITMAP_TYPE_XPM ),
                                   cancel_message );
    
    // Tell it to run, returning whether it finished all the way to the end
    clicked_finish_button = a_add_channel_wizard->run_wizard();
    
    // If clicked the finish button...
    if ( clicked_finish_button ) 
    {
        // ..,Get the string of the configuration section that was added,
        // and whether to launch a channel dialog, so parent can do something 
        // meaninful with that information.
        *generated_channel_section = a_add_channel_wizard->get_generated_channel_section();
        *is_launch_channel_dialog_requested = a_add_channel_wizard->get_is_channel_dialog_requested();
    }
    
    // All done. Destroy the object to free memory
    a_add_channel_wizard->Destroy();
    
    return clicked_finish_button;
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
