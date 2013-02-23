//----------------------------------------------------------------------------------------
// Name:        add_channel_wizard.cpp
// Purpose:     A wizard for adding a channel
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: add_channel_wizard.cpp,v 1.14 2007/06/29 00:58:49 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "add_channel_wizard.h"
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

#include "add_channel_wizard.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

//----------------------------------------------------------------------------------------

#include "configuration.h"
#include "plucker_controller.h"
#include "utils_string.h"
#include "help_controller.h"
#include "handheld_dest_wrappers.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( add_channel_wizard, plucker_wizard_base )

END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------
            
add_channel_wizard::add_channel_wizard( wxWindow* parent, int id, const wxString& title,
                  const wxBitmap& bitmap, const wxString& cancel_message )
        : plucker_wizard_base( parent, id, title, bitmap, cancel_message )
{ 
    //---Channel name wizardpage---------------------------------------------------------      
  
    wxArrayString channel_name_message_strings;
    channel_name_message_strings.Add( _( "Enter a name for the new channel." ) );
    channel_name_message_strings.Add( _( "The channel name can be up to 26 letters long." ) );

    wxString channel_name_textctrl_label = _( "Channel name:" );
    
    wxString doc_name_key;
    doc_name_key << wxT( "/") << plkrNEW_CHANNEL_TEMPLATE_SECTION << wxT( "/") << wxT( "doc_name");
    wxString channel_name_textctrl_value = the_configuration->Read( doc_name_key,
                                                                    plkrUNNAMED_CHANNEL_NAME );

    m_channel_name_wizardpage = new textctrl_wizardpage( this,
                    channel_name_message_strings,
                    channel_name_textctrl_label,
                    channel_name_textctrl_value,
                    TRUE );

    channel_name_message_strings.Clear();

    // Set the max length of the channel name textctrl to plkrMAXIMUM_CHANNEL_NAME_LENGTH
    /*! \test Test channel name (doc_name) maxlength on Mac */
    m_channel_name_wizardpage->get_textctrl()->SetMaxLength( plkrMAXIMUM_CHANNEL_NAME_LENGTH );

    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_channel_name_wizardpage );
    wxLogDebug( wxT( "Added a checkbox_wizardpage") );

    //---Starting page wizardpage--------------------------------------------------------

    wxArrayString starting_message_strings;
    starting_message_strings.Add( _( "Enter a starting page for the channel." ) );
    starting_message_strings.Add( _( "The starting page and the pages it links to will be retrieved." ) );
    starting_message_strings.Add( _( "You can choose to start on either a website URL or a local file on your hard drive." ) );
            
    wxString url_label  = _( "Start on a website URL:" );
    wxString file_label = _( "Start on a local file:" );
        
    m_starting_page_wizardpage = new starting_page_wizardpage( this, 
                    starting_message_strings, url_label, file_label ); 
                    
    starting_message_strings.Clear();  
                  
    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_starting_page_wizardpage );
    wxLogDebug( wxT( "Added a starting_page_wizardpage") );

    //---Limits wizardpage--------------------------------------------------------------      

    m_limits_wizardpage = new limits_wizardpage( this );    
    
    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_limits_wizardpage );    
    wxLogDebug( wxT( "Added a limits_wizardpage") ); 

    //---Destinations wizardpage---------------------------------------------------------      
    
    m_destinations_wizardpage = new destinations_wizardpage( this,
                    _( "Send channel output to these handheld destinations:" ),
                    _( "Send channel output to these directory destinations:" ) 
                    );    
    
    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_destinations_wizardpage );   
    wxLogDebug( wxT( "Added a destinations_wizardpage") ); 

    //---Launch Channel Dialog when click Finish wizardpage------------------------------      
  
    wxArrayString launch_message_strings;
    launch_message_strings.Add( _( "The wizard has gathered all the information needed to create a new channel." ) );
    launch_message_strings.Add( _( "Click Finish to add the channel and complete the wizard." ) );
            
    wxString launch_checkbox_label = _( "Launch a 'Configure channel' dialog when I click Finish." );    
    
    bool do_launch_channel_dialog = (bool)the_configuration->Read( 
                        wxT( "/PLUCKER_DESKTOP/add_channel_wizard_do_launch_channel_dialog"), 0L );
    
    m_launch_channel_dialog_wizardpage = new checkbox_wizardpage( this, 
                    launch_message_strings, launch_checkbox_label, do_launch_channel_dialog );                                 
                    
    launch_message_strings.Clear();                
    
    // Add this wizardpage into final chain (also used to calculate final wizard size)
    add_wizardpage_to_wizard( m_launch_channel_dialog_wizardpage );   
    wxLogDebug( wxT( "Added a checkbox_wizardpage") );
}


add_channel_wizard::~add_channel_wizard()
{
}


wxString add_channel_wizard::get_generated_channel_section()
{
    return m_generated_channel_section;
}


bool add_channel_wizard::get_is_channel_dialog_requested()
{
    return m_is_channel_dialog_requested;
}
   
//----------------------------------------------------------------------------------------
// Private members
//----------------------------------------------------------------------------------------

void add_channel_wizard::on_finish_button()
{
    //---Create a channel specification from collected data-------------------------------
 
    channel_type    channel;
    
    channel.doc_name             = m_channel_name_wizardpage->get_textctrl_value();
    channel.home_url             = m_starting_page_wizardpage->get_home_url_value();
    channel.home_maxdepth        = (long)m_limits_wizardpage->get_maxdepth_value();
    channel.home_stayondomain    = (long)m_limits_wizardpage->get_stayondomain_value(); 
    channel.home_stayonhost      = (long)m_limits_wizardpage->get_stayonhost_value();
    
    // Will clear later with WX_CLEAR_ARRAY macro
    handheld_dest_array_type handheld_dest_array;
    m_destinations_wizardpage->get_user_destinations( handheld_dest_array );

    wxArrayString sync_directories_array;
    m_destinations_wizardpage->get_directory_destinations( sync_directories_array );
    channel.copy_to_dir = utils_string::arraystring_to_string( sync_directories_array, plkrOS_FILE_ARRAY_SEPARATOR );
    sync_directories_array.Clear();

    //---Add the channel and store information for the parent to handle-------------------

    // Create the channel. Get the collected name of the new channel section and
    // store it in a member variables so parent window of the wizard can do something with
    // this information, by accessing this member variable
    m_generated_channel_section     = plucker_controller::get()->add_channel( channel );

    // TODO: Technically, this handheld_dest_array should be part of the channel, that is 
    // then added above. This way will do for now.
    handheld_dest_wrappers::write_handheld_dest_array_to_configuration( m_generated_channel_section,
                                                                        handheld_dest_array );
    // The WX_CLEAR_ARRAY(array) macro [see dynarray.h] is required to clear out all the 
    // elements from memory of an array of structs that was built with WX_DEFINE_ARRAY.
    // All that this macro does is just to loop through the array, "delete" each element
    // to free it from memory, then mark the array as Empty().
    WX_CLEAR_ARRAY( handheld_dest_array );

    // Also store whether they wanted to launch a configuration dialog, so wizard parent
    // can do something with that information
    m_is_channel_dialog_requested   = m_launch_channel_dialog_wizardpage->get_checkbox_value();

    // Write to the configration that we said we like to launch channel dialogs af the
    // end of the wizard
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/add_channel_wizard_do_launch_channel_dialog"),
                              m_is_channel_dialog_requested );

    //-------------------------------------------------------------------------------------
    // Flush() writes the things to the file immediately, instead of waiting for program
    // to exit to write them (required since we added a new channel).
    the_configuration->Flush();
}


void add_channel_wizard::on_help_button( wxWizardEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    if        ( event.GetPage() == m_channel_name_wizardpage ) 
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_ADD_CHANNEL_WIZARD_CHANNEL_NAME_PAGE );
    } 
    else if ( event.GetPage() == m_starting_page_wizardpage ) 
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_ADD_CHANNEL_WIZARD_STARTING_PAGE_PAGE );
    } 
    else if ( event.GetPage() == m_limits_wizardpage ) 
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_ADD_CHANNEL_WIZARD_LIMITS_PAGE );
    } 
    else if ( event.GetPage() == m_destinations_wizardpage ) 
    {
        help_controller::get()->show_help_topic( plkrHELP_ID_ADD_CHANNEL_WIZARD_DESTINATION_PAGE );
    } 
    else 
    {
        // Fall through: it was the end message.
        help_controller::get()->show_help_topic( plkrHELP_ID_ADD_CHANNEL_WIZARD );
    }     
#endif      
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
