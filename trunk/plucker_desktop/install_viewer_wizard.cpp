//----------------------------------------------------------------------------------------
// Name:        install_viewer_wizard.cpp
// Purpose:     A wizard for reinstalling the handheld viewer software
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: install_viewer_wizard.cpp,v 1.9 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "install_viewer_wizard.h"
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

#include "install_viewer_wizard.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

//----------------------------------------------------------------------------------------

#include "configuration.h"
#include "plucker_controller.h"
#include "utils_string.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( install_viewer_wizard, plucker_wizard_base )

END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

install_viewer_wizard::install_viewer_wizard( wxWindow* parent, int id, const wxString& title,
                  const wxBitmap& bitmap, const wxString& cancel_message )
        : plucker_wizard_base( parent, id, title, bitmap, cancel_message )
{      
    //---Begin message wizardpage---------------------------------------------------------      
    
    wxString begin_message_string = wxT( "<p>");
    begin_message_string += wxT( "<font size='+2'>" );
    begin_message_string += _( "Install Handheld Software Wizard" );
    begin_message_string += wxT( "</font>" );    
    begin_message_string += wxT( "<p>" );
    begin_message_string << _( "This wizard will reinstall the" ) << wxT( " " ) << plkrAPPLICATION_SUITE_NAME << wxT( " " ) <<_( "viewer software to your handheld." );
    begin_message_string += wxT( "<p>" );
    begin_message_string += _( "This is helpful if the software was accidentally erased from the device, and now needs to be reinstalled." );
    begin_message_string += wxT( "<p>" );    
    begin_message_string += _( "Click Next to begin, or Cancel to exit the wizard." );    
    m_begin_message_wizardpage = new message_wizardpage( this, 
                                                         begin_message_string );
                                                         
    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_begin_message_wizardpage );
    wxLogDebug( wxT( "Added a begin message_wizardpage") );
    
    //---Viewer wizardpage----------------------------------------------------------------      

    m_viewer_wizardpage = new viewer_wizardpage( this );
    
    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_viewer_wizardpage );
    wxLogDebug( wxT( "Added a viewer_wizardpage") );
    
    //---Destinations wizardpage----------------------------------------------------------      

    m_destinations_wizardpage = new destinations_wizardpage( this,
                    _( "Install handheld software to these handheld destinations" ),
                    _( "Install handheld software to these directory destinations" ) 
                    );    
                    
    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_destinations_wizardpage );
    wxLogDebug( wxT( "Added a destinations_wizardpage") ); 
  
    //---End message wizardpage-----------------------------------------------------------      
  
    wxString end_message_string = wxT( "<p>&nbsp;<p>" );
    end_message_string += _( "The wizard has gathered all information needed to install the handheld software." );
    end_message_string += wxT( "<p>" );
    end_message_string += _( "Click Finish to perform the installation and complete the wizard." ); 
    end_message_string += wxT( "<p>" );
    end_message_string += _( "After clicking Finish, the handheld software will be sent your specified destinations." );
    m_end_message_wizardpage = new message_wizardpage( this,
                                                       end_message_string );
    
    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_end_message_wizardpage );
    wxLogDebug( wxT( "Added an end message_wizardpage") );
}


// Destructor
install_viewer_wizard::~install_viewer_wizard()
{
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

//  Things to do if/when the finish button was clicked.
void install_viewer_wizard::on_finish_button()
{
    wxLogDebug( wxT( "Entering install_viewer_wizard::on_finish_button()") );
    
    //---Install viewer software----------------------------------------------------------

    wxArrayString   viewer_fullnames_to_install;      
    
    m_viewer_wizardpage->get_fullnames_to_install( &viewer_fullnames_to_install );
    
    // Whatever destinations were on the destations wizard page, send the handheld 
    // software there.
    m_destinations_wizardpage->m_handheld_install_ctrl
        ->InstallFilesToDestinations( viewer_fullnames_to_install );        
    
    viewer_fullnames_to_install.Clear();  
		
    //---Write the viewer installation configuration-------------------------------------

    // TODO
    
    //-----------------------------------------------------------------------------------
    // Flush() writes the things to the file immediately, instead of waiting for program 
    // to exit to write them.
    the_configuration->Flush();    
}


// Things to do if the help button was clicked.
void install_viewer_wizard::on_help_button( wxWizardEvent& event )
{
#if ( setupUSE_ONLINE_HELP )
    if        ( event.GetPage() == m_viewer_wizardpage ) 
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_INSTALL_VIEWER_WIZARD_SOFTWARE_SELECTION_PAGE );
    } 
    else if ( event.GetPage() == m_destinations_wizardpage ) 
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_INSTALL_VIEWER_WIZARD_DESTINATION_PAGE );
    } 
    else 
    {
        // Fall through: it was either the start or end message.
        help_controller::get()->show_help_topic( plkrHELP_ID_INSTALL_VIEWER_WIZARD );
    } 
#endif     
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
