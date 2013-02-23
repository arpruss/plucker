//----------------------------------------------------------------------------------------
// Name:        setup_wizard.cpp
// Purpose:     A setup wizard for Plucker Desktop settings shared across channels
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: setup_wizard.cpp,v 1.16 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "setup_wizard.h"
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

#include "setup_wizard.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/dir.h"

//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

//----------------------------------------------------------------------------------------

#include "configuration.h"
#include "plucker_controller.h"
#include "utils_string.h"
#include "help_controller.h"
#include "handheld_dest_wrappers.h"

//----------------------------------------------------------------------------------------
// Event body: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( setup_wizard, plucker_wizard_base )

END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor
setup_wizard::setup_wizard( wxWindow* parent, int id, const wxString& title,
                  const wxBitmap& bitmap, const wxString& cancel_message )
        : plucker_wizard_base( parent, id, title, bitmap, cancel_message )
{
    wxLogDebug( wxT( "Entering setup_wizard constructor") );

    //---Begin message wizardpage-------------------------------------------------------      
 
    wxString begin_message_string = wxT( "<p>");
    begin_message_string += wxT( "<font size='+2'>" );
    begin_message_string += wxT( " " );
    begin_message_string += plkrAPPLICATION_DESKTOP_NAME;
    begin_message_string += wxT( " " );
    begin_message_string += wxT( "Setup Wizard" );
    begin_message_string += wxT( "</font>" );
    begin_message_string += wxT( "<p>" );
    begin_message_string += _( "This wizard will guide you through setting up" );
    begin_message_string += wxT( " " );
    begin_message_string += plkrAPPLICATION_DESKTOP_NAME;
    begin_message_string += wxT( " " );
    begin_message_string += _( "for your system." );
    begin_message_string += wxT( "<p>" );
    begin_message_string += _( "You can specify what handheld software you would like installed, what handhelds or directories you want channels and software installed to, and set up your proxy" );
    begin_message_string += wxT( "<p>" );
    begin_message_string += _( "Click Next to begin, or Cancel to exit the wizard." );
    m_begin_message_wizardpage = new message_wizardpage( this, begin_message_string );

    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_begin_message_wizardpage );
    wxLogDebug( wxT( "Added a begin message_wizardpage") );

    //---Viewer wizardpage---------------------------------------------------------------

    m_viewer_wizardpage = new viewer_wizardpage( this );

    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_viewer_wizardpage );
    wxLogDebug( wxT( "Added a viewer_wizardpage") );

    //---Destinations wizardpage---------------------------------------------------------

    m_destinations_wizardpage = new destinations_wizardpage( this,
                    _( "Install software/channels to these handheld destinations" ),
                    _( "Install software/channels to these directory destinations" )
                    );

    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_destinations_wizardpage );
    wxLogDebug( wxT( "Added a destinations_wizardpage") );

    //---Proxy wizardpage-----------------------------------------------------------------

    m_proxy_wizardpage = new proxy_wizardpage( this );

    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_proxy_wizardpage );

    wxLogDebug( wxT( "Added a proxy_wizardpage") );

    //---Channel list wizardpage----------------------------------------------------------

    // Only do one plkrdata.
    m_setup_wizard_plkrdata_fullname << get_plucker_directory( RESOURCES )
        << wxT( "/plkrdata_setup_wizard/setup_wizard_channels.plkrdata");

    //\ !todo Add an error handler here for file not existing

    m_setup_wizard_configuration = new wxFileConfig( wxT( "Plucker Desktop Setup Wizard Configuration"),
                                                     wxT( "The Plucker Team"),
                                                     m_setup_wizard_plkrdata_fullname,
                                                     wxEmptyString,
                                                     wxCONFIG_USE_LOCAL_FILE
                                                   );

    wxArrayString channel_list_message_arraystring;
    channel_list_message_arraystring.Add( _( "You can have some default channels automatically configured." ) );
    channel_list_message_arraystring.Add( _( "Use the checkboxes to select your desired channels." ) );

    m_channel_list_wizardpage = new channel_list_wizardpage( this,
                                                             channel_list_message_arraystring,
                                                             m_setup_wizard_configuration );

    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_channel_list_wizardpage );

    // Free memory.
    channel_list_message_arraystring.Clear();

    //---End message wizardpage-----------------------------------------------------------

    wxString end_message_string = wxT( "<p>&nbsp;<p>" );
    end_message_string += _( "The wizard has gathered all information needed to configure" );
    end_message_string += wxT( " " );
    end_message_string += plkrAPPLICATION_DESKTOP_NAME;
    end_message_string += wxT( ".<p>" );
    end_message_string += _( "Click Finish to apply these settings and complete the wizard." );
    m_end_message_wizardpage = new message_wizardpage( this, end_message_string );

    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_end_message_wizardpage );
    wxLogDebug( wxT( "Added an end message_wizardpage") );
}


// Destructor
setup_wizard::~setup_wizard()
{
   if ( m_setup_wizard_configuration ) {
      delete m_setup_wizard_configuration;
   }
}


void setup_wizard::update_keys_across_all_channels()
{
    // Make an array of all channel sections
    wxArrayString channel_sections;
    plucker_controller::get()->get_all_channels( channel_sections );
    // Add plkrNEW_CHANNEL_TEMPLATE_SECTION to the array, so that new channels will
    // use these destinations too.
    channel_sections.Add( plkrNEW_CHANNEL_TEMPLATE_SECTION );

    size_t number_of_sections           = channel_sections.GetCount();
    size_t current_section_array_index;
    wxString current_section_string;

    if ( number_of_sections == 0 ) 
    {
        return;
    }

    // Get the values of 'user', 'copy_to_dir', and 'bpp' that will be applied to all
    // channels.
    handheld_dest_array_type handheld_dest_array;
    m_destinations_wizardpage->m_handheld_install_ctrl->GetUserDestinations( handheld_dest_array );
    // NOTE: Don't clear the handheld_dest_array till the end obviously, since still need to write.

    wxString      sync_directories_string;
    wxArrayString sync_directories_array;
    m_destinations_wizardpage->m_handheld_install_ctrl->GetDirectoryDestinations( sync_directories_array );
    sync_directories_string = utils_string::arraystring_to_string( sync_directories_array, plkrOS_FILE_ARRAY_SEPARATOR );
    sync_directories_array.Clear();

    long          bpp;
    bpp = m_viewer_wizardpage->get_bpp();

     // Do our loop of updating
    for ( current_section_array_index = 0;
          (int) current_section_array_index < (int)number_of_sections;
          current_section_array_index++ ) 
    {
        current_section_string = channel_sections.Item( current_section_array_index );
        
        // Write all the keys for handheld_dest
        handheld_dest_wrappers::write_handheld_dest_array_to_configuration( current_section_string,
                                                                            handheld_dest_array );

        the_configuration->Write( wxT( "/" ) + current_section_string + wxT( "/" ) + wxT( "copy_to_dir"),
                                  sync_directories_string );

        // We only want to lower the bpp if it is too high. For example if a
        // an installed example is 1bpp, we want to leave it at 1bpp, not pump up to 8bpp.
        if ( bpp < the_configuration->Read( wxT( "/" ) + current_section_string + wxT( "/" ) + wxT( "bpp"), 1L ) ) 
        {
            the_configuration->Write( wxT( "/" ) + current_section_string + wxT( "/" ) + wxT( "bpp"), bpp );
        }

        // If the viewer wizardpage has a hires viewer selected, then pump up the maxwidth
        // and maxheight. 
        if ( m_viewer_wizardpage->get_is_hires_viewer_selected() == TRUE )
        {
            the_configuration->Write( wxT( "/" ) + current_section_string + wxT( "/" ) + wxT( "maxheight"), plkrIMAGES_DEFAULT_MAXHEIGHT_HIRES_VIEWER );
            the_configuration->Write( wxT( "/" ) + current_section_string + wxT( "/" ) + wxT( "maxwidth"), plkrIMAGES_DEFAULT_MAXWIDTH_HIRES_VIEWER );  
        }

        wxString doc_file;
        // Autogenerate a unique doc_file_filename, based on the section name.
        // Use a relative filename and parser will save in relative to PLUCKERDIR.
        // NOTE: This can't be "channels" + '/' +.. or will fail (since can't store a / in
        // "channels" I guess. If try it, a "test" channel comes out as tctrltest/test
        doc_file << wxT( "channels") << wxT( "/" ) << current_section_string << wxT( "/" ) << current_section_string;

        wxLogDebug( wxT( "Writing out doc_file=") + doc_file );
        the_configuration->Write( wxT( "/" ) + current_section_string + wxT( "/" ) + wxT( "doc_file"), doc_file );
    }

    // Free memory
    channel_sections.Clear();
    // The WX_CLEAR_ARRAY(array) macro [see dynarray.h] is required to clear out all the 
    // elements from memory of an array of structs that was built with WX_DEFINE_ARRAY.
    // All that this macro does is just to loop through the array, "delete" each element
    // to free it from memory, then mark the array as Empty().
    WX_CLEAR_ARRAY( handheld_dest_array );
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

//  Things to do if/when the finish button was clicked.
void setup_wizard::on_finish_button()
{
    //---Install viewer software--------------------------------------------------------

    wxArrayString   viewer_fullnames_to_install;

    m_viewer_wizardpage->get_fullnames_to_install( &viewer_fullnames_to_install );

    // Whatever desitations were on the destations wizard page, send the handheld
    // software there.
    m_destinations_wizardpage->m_handheld_install_ctrl
        ->InstallFilesToDestinations( viewer_fullnames_to_install );

    viewer_fullnames_to_install.Clear();

    //---Proxy wizardpage---------------------------------------------------------------

    m_proxy_wizardpage->write_configuration();

    //---Channel list wizardpage--------------------------------------------------------

    wxArrayString channel_sections_to_import;
    m_channel_list_wizardpage->get_checked_channel_sections( &channel_sections_to_import );

    ::import_sections_as_section_names( channel_sections_to_import,
                                        optionCONFIGURATION_COPY_SECTION_TREAT_AS_ABSOLUTE,
                                        m_setup_wizard_configuration,
                                        the_configuration );

    channel_sections_to_import.Clear();

    //---Update configuration keys across _all_ channels--------------------------------

    // NOTE: This has to come after the channel section import above, so that the
    // channels are in the plucker.ini/.pluckerrc so that they can be updated with
    // bpp, destination, etc.
    update_keys_across_all_channels();

    //----------------------------------------------------------------------------------
    // Flush() writes the things to the file immediately, instead of waiting for program
    // to exit to write them.
    the_configuration->Flush();
}


// Things to do if the help button was clicked.
void setup_wizard::on_help_button( wxWizardEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    if        ( event.GetPage() == m_viewer_wizardpage ) 
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_SETUP_WIZARD_SOFTWARE_SELECTION_PAGE );
    } 
    else if ( event.GetPage() == m_destinations_wizardpage )
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_SETUP_WIZARD_DESTINATION_PAGE );
    } 
    else if ( event.GetPage() == m_proxy_wizardpage ) 
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_SETUP_WIZARD_PROXY_PAGE );
    }
    else if ( event.GetPage() == m_channel_list_wizardpage ) 
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_SETUP_WIZARD_CHANNEL_LIST_PAGE );
    } 
    else 
    {
        // Fall through: it was either the start or end message.
        help_controller::get()->show_help_topic( plkrHELP_ID_SETUP_WIZARD );
    }
#endif
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
