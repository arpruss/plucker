//----------------------------------------------------------------------------------------
// Name:        plucker_controller.cpp
// Purpose:     The actions that can be performed on a channel: add, delete, update
//              Add/delete does and add/delete of both the section of .ini holding the
//              channel's settings and its subdir within the channel directory.
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: plucker_controller.cpp,v 1.61 2007/06/30 14:55:43 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "plucker_controller.h"
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

#include "plucker_controller.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/file.h"                    // wxFile

// ---------------------------------------------------------------------------------------

#include "configuration.h"
#include "utils_string.h"
#include "utils_datetime.h"
#include "utils_execute.h"
#include "handheld_dest_wrappers.h"
#include "proxy_prompt.h"

//----------------------------------------------------------------------------------------
// Internal variables
//----------------------------------------------------------------------------------------

plucker_controller* plucker_controller::ms_instance = NULL;

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

plucker_controller::plucker_controller()
{   
    // Create a new instance of palm_installer to get data for the different controls
    m_palm_installer                = new palm_installer();     
    
    // Create (if not exist) a ./plucker directory if there is no PLUCKERHOME 
    // (currently GTK only)
    create_pluckerhome_directory();        
    
    // Copy over the systemwide exclusionlist.txt to the user's home dir if needed
    create_default_exclusion_list();    
                
    // Create (if not exist) the "/channels" directory in PLUCKERHOME, which will have
    // subdirectories of the template channel, and other channels.
    create_channels_directory();
        
    // The new channel template section and subdirs may not exist, so create if don't   
    // exist. 
    create_channel_template_section_and_subdirectory();   
    
    // Initially, say that is not okay to initiate autoupdates. The startup rountine of
    // the application will let is know when it is ready building all the needed 
    // dialogs, etc.
    set_is_new_update_execution_allowed( FALSE );     
}


plucker_controller::~plucker_controller()
{
    if ( m_palm_installer ) 
    {
        delete m_palm_installer;
        m_palm_installer = NULL;
    }
    wxLogDebug( wxT( "Destructed palm_installer" ) );
}


plucker_controller* plucker_controller::get()
{
    wxLogDebug( wxT( "Entering image_list::get()" ) );

    if ( ! ms_instance ) 
    {
        ms_instance = new plucker_controller();
    }            
    
    // Return ms_instance (which will stay as NULL if 
    return ms_instance;
}


plucker_controller* plucker_controller::set( plucker_controller *desired_plucker_controller )
{
    plucker_controller* old_plucker_controller = ms_instance;
    ms_instance = desired_plucker_controller;
    return old_plucker_controller;
}


wxString plucker_controller::add_channel( channel_type &channel )
{
    // If doc_name is empty, then set the doc_name to unnamed channel.
    if ( channel.doc_name == wxEmptyString ) 
    {
        channel.doc_name = plkrUNNAMED_CHANNEL_NAME;
    }
    
    // Name of the new channel's section in configuration file
    wxString channel_section;
    // Give it a legal name for its section in the config file.
    channel_section = get_legal_new_channel_section( channel.doc_name );
    wxLogDebug( channel.doc_name + wxT( " converted to section name of " ) + channel_section );
    
    // Copy all the relevant keys from the plkrNEW_CHANNEL_TEMPLATE_SECTION section, to 
    // the new section. TRUE parameter just tells to treat these section names as absolute.
    // plkrNEW_CHANNEL_TEMPLATE section should have been created in the constructor,
    // but copy_section has error checking that will just abort if the section doesn't 
    // exist.
    copy_section( plkrNEW_CHANNEL_TEMPLATE_SECTION, channel_section, TRUE );    
    
    // Strip any depreciated keys that got into the template
    strip_depreciated_keys( channel_section );  
    
    // Alot of keys here. Changing the default config path for clarity. Storing the current
    // path so can restore it when done.    
    wxString old_configuration_path = the_configuration->GetPath();
    wxString new_configuration_path;
    new_configuration_path << wxT( "/" ) << channel_section;
    the_configuration->SetPath( new_configuration_path );    
    
    // We don't want them all new channels to start on plucker:home.html. Set it's own 
    // home_url in its channel subdirectory.
    wxString default_home_url_value = wxT( "plucker:/channels/" ) + channel_section + wxT( "/home.html" );
    the_configuration->Write( wxT( "home_url" ), default_home_url_value );

    // Set the doc_name key to the doc_name parameter that was passed 
    // into this funcition.    
    the_configuration->Write( wxT( "doc_name" ), channel.doc_name );
    wxLogDebug( wxT( "Wrote new channel's doc_name=" ) + channel.doc_name );
    
    // Set the doc_file key to the channel_section, so it won't be the same as the 
    // template (can't hotsync two files with same filename into the HotSync user directory). 
    wxString doc_file_string;
    doc_file_string << wxT( "channels" ) << wxT( "/" ) << channel_section << wxT( "/" ) << channel_section;
    the_configuration->Write( wxT( "doc_file" ), doc_file_string );
    wxLogDebug( wxT( "Wrote new channel's doc_file=" ) + doc_file_string );
    
    // If template said to use a channel-specific exclusion list, then build a channel-
    // specific exclusion list in the new channel too. Note that this is just writing
    // a key to the file. The exclusionlist.txt file itself will be copied over as 
    // part of the create_new_channel_subdirectory()
    
    wxString template_exclusion_key;
    template_exclusion_key << wxT( "/" ) << plkrNEW_CHANNEL_TEMPLATE_SECTION << wxT( "/exclusion_lists" );
                                          
    if ( the_configuration->Read( template_exclusion_key, wxT( "" ) ) != wxT( "" ) ) 
    {
        wxString default_exclusion_lists_value = wxT( "channels/" ) + channel_section 
                                                  + wxT( "/exclusionlist.txt" );
                                                 
        the_configuration->Write( wxT( "exclusion_lists" ), default_exclusion_lists_value );                                
        wxLogDebug( wxT( "Wrote new channel's exclusion_lists=" ) + default_exclusion_lists_value );
    }     
    
    // Check all keys that come in on the channel_type argument sent to this function,
    // and replace those that don't have a -1/wxEmptyString/wxInvalidDateTime    
    
    if ( ! channel.doc_name.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "doc_name" ), channel.doc_name );
    }

    if ( ! channel.home_url.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "home_url" ), channel.home_url );
    }

    if ( channel.depth_first != -1 ) 
    {
        the_configuration->Write( wxT( "depth_first" ), channel.depth_first );
    }

    if ( channel.verbosity != -1 )
    {
        the_configuration->Write( wxT( "verbosity" ), channel.verbosity );
    }

    if ( channel.status_line_length != -1 ) 
    {
        the_configuration->Write( wxT( "status_line_length" ), channel.status_line_length );
    }

    if ( ! channel.command_before.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "command_before" ), channel.command_before );
    }    
    
    if ( ! channel.command_after.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "command_after" ), channel.command_after );
    }                                          
    
    if ( channel.home_maxdepth != -1 ) 
    {
        the_configuration->Write( wxT( "home_maxdepth" ), channel.home_maxdepth );
    }

    if ( channel.home_stayonhost != -1 ) 
    {
        the_configuration->Write( wxT( "home_stayonhost" ), channel.home_stayonhost );
    }

    if ( channel.home_stayondomain != -1 ) 
    {
        the_configuration->Write( wxT( "home_stayondomain" ), channel.home_stayondomain );
    }
    
    if ( ! channel.home_url_pattern.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "home_url_pattern" ), channel.home_url_pattern );
    }   
 
    if ( ! channel.charset.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "charset" ), channel.charset );
    }      
      
    if ( channel.bpp != -1 )
    {
        the_configuration->Write( wxT( "bpp" ), channel.bpp );
    }
    
    if ( channel.alt_text != -1 ) 
    {
        the_configuration->Write( wxT( "alt_text" ), channel.alt_text );
    }
    
    if ( channel.maxheight != -1 ) 
    {
        the_configuration->Write( wxT( "maxheight" ), channel.maxheight );
    }

    if ( channel.maxwidth != -1 ) 
    {
        the_configuration->Write( wxT( "maxwidth" ), channel.maxwidth );
    }

    if ( channel.image_compression_limit != -1 ) {
        the_configuration->Write( wxT( "image_compression_limit" ), channel.image_compression_limit );
    }

    if ( channel.try_reduce_bpp != -1 ) {
        the_configuration->Write( wxT( "try_reduce_bpp" ), channel.try_reduce_bpp );
    }

    if ( channel.try_reduce_dimension != -1 ) {
        the_configuration->Write( wxT( "try_reduce_dimension" ), channel.try_reduce_dimension );
    }

    //! Sanity check: we never want both try_reduce_bpp and try_reduce_dimension to both be 1.
    if ( channel.try_reduce_bpp == 1 && channel.try_reduce_dimension == 1 ) 
    {
        the_configuration->Write( wxT( "try_reduce_bpp" ), 0 );
    }

    if ( ! channel.charset.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "charset" ), channel.charset );
    }

    if ( channel.indent_paragraphs != -1 ) 
    {
        the_configuration->Write( wxT( "indent_paragraphs" ), channel.indent_paragraphs );
    }
    
    if ( ! channel.compression.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "compression" ), channel.compression );
    }                     
    
    if ( ! channel.category.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "category" ), channel.category );
    }                     
    
    if ( channel.no_urlinfo != -1 ) 
    {
        the_configuration->Write( wxT( "no_urlinfo" ), channel.no_urlinfo );
    }
    
    if ( ! channel.owner_id_build.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "owner_id_build" ), channel.owner_id_build );
    }   
    
    if ( channel.copyprevention_bit != -1 )
    {
        the_configuration->Write( wxT( "copyprevention_bit" ), channel.copyprevention_bit );
    }
    
    if ( channel.backup_bit != -1 ) 
    {
        the_configuration->Write( wxT( "backup_bit" ), channel.backup_bit );
    }
    
    if ( channel.launchable_bit != -1 )
    {
        the_configuration->Write( wxT( "launchable_bit" ), channel.launchable_bit );
    }    
    
    if ( ! channel.big_icon.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "big_icon" ), channel.big_icon );
    }   
    
    if ( ! channel.small_icon.IsEmpty() )
    {
        the_configuration->Write( wxT( "small_icon" ), channel.small_icon );
    }   
   
    if ( ! channel.doc_file.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "doc_file" ), channel.doc_file );
    }   
    
    if ( ! channel.user.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "user" ), channel.user );        
    }  

    if ( ! channel.copy_to_dir.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "copy_to_dir" ), channel.copy_to_dir );
    }  
      ;    
    if ( channel.update_frequency != -1 ) 
    {
        the_configuration->Write( wxT( "update_frequency" ), channel.update_frequency );
    }      
   
    if ( ! channel.update_period.IsEmpty() ) 
    {
        the_configuration->Write( wxT( "update_period" ), channel.update_period );
    }  
    
    if ( channel.update_base.IsValid() ) 
    {
        wxDateTime update_base;
        wxString update_base_string;
        update_base = channel.update_base;
        update_base_string = utils_datetime::datetime_to_w3c_string( update_base );
        the_configuration->Write( wxT( "update_base" ), update_base_string );
    }
    
    if ( channel.update_enabled != -1 ) 
    {
        the_configuration->Write( wxT( "update_enabled" ), channel.update_enabled );
    }        
         
    // Done with config. Restore it to what it was before.
    the_configuration->SetPath( old_configuration_path );                               
                                         
    // Make the channel's subdirectory within the /channels folder.
    create_new_channel_subdirectory( channel_section );

    // Return the section_name that we created.
    return channel_section;    
}


bool plucker_controller::delete_channel( const wxString& channel_section )
{       
    wxLogDebug ( wxT( "Entering plucker_controller::delete_channel" ) ); 
     
    bool config_deletion_successful         = FALSE;
    bool subdirectory_deletion_successful   = FALSE;

    //! \todo. Should this be a HasGroup( "/channel_section" ) (perhaps that is what caused crash).

    if ( ( channel_section != wxEmptyString ) && the_configuration->HasGroup( channel_section ) ) 
    {
        wxLogDebug ( wxT( "plucker_controller::delete_channel: About to delete " ) + channel_section );
        config_deletion_successful = the_configuration->DeleteGroup( channel_section );
        wxLogDebug ( wxT( "plucker_controller::delete_channel() deleted " ) + channel_section );
    } 
    else 
    {
        wxLogError( wxT( "Error: plucker_controller::delete_channel(): can't find section " ) + channel_section );
    }
    
    // Now remove its directory.
    subdirectory_deletion_successful = delete_channel_subdirectory( channel_section );
    
    return ( config_deletion_successful && subdirectory_deletion_successful );
}


bool plucker_controller::delete_channels( wxArrayString& channel_sections )
{
    wxLogDebug( wxT( "Entering plucker_controller::delete_channels" ) );
    
    size_t number_of_sections           = channel_sections.GetCount();
    size_t current_section_array_index;    
    wxString current_section_string;
    bool all_successful                 = TRUE;
    
    if ( number_of_sections == 0 ) 
    {
        return FALSE;
    }
    
    for ( current_section_array_index = 0;
          (int)current_section_array_index < (int)number_of_sections;
          current_section_array_index++ ) 
    {        
        current_section_string = channel_sections.Item( current_section_array_index );
        wxLogDebug( wxT( "plucker_controller:delete_channel for section=" ) + current_section_string );
        
        // Delete each one by calling delete channel functions. If ever an error,
        // return FALSE.
        if ( ! delete_channel( current_section_string ) ) 
        {
            all_successful = FALSE;
            wxLogDebug( wxT( "Error: plucker_controller::delete_channels(): can't delete channel=" )            
                         + current_section_string );
        }        
    }       
    return all_successful;
}


bool plucker_controller::update_channel( const wxString &channel_section, bool install )
{
    wxArrayString channel_sections;        
  
    channel_sections.Add( channel_section );
    update_channels( channel_sections, install );
    
    channel_sections.Clear();
    
    return TRUE;   
}


bool plucker_controller::update_channels( wxArrayString& channel_sections, bool install )
{
    size_t number_of_sections           = channel_sections.GetCount();
    size_t current_section_array_index;    
    wxString current_section_string;
    
    if ( number_of_sections == 0 ) 
    {
        return FALSE;
    }    
    
    // Prevent a new autoupdate from taking place before finish these ones.
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );

    // If a before_group_command was specified, execute it
    wxString before_group_command;
    before_group_command = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/before_group_command" ), wxT( "" ) );
    if ( before_group_command != wxT( "" ) ) 
    {
        // TRUE means wait for the program to finish before continuing.
        wxExecute( before_group_command, TRUE );
    }

    // Do our loop of updating
    for ( current_section_array_index = 0;
          (int) current_section_array_index < (int) number_of_sections;
          current_section_array_index++ ) 
    {
        current_section_string = channel_sections.Item( current_section_array_index );

        // Check to see if the section exists (might not if came from a update
        // selected sections commandline [which would flush update_bases to that
        // section, and those section names might have unsafe characters in them
        if ( the_configuration->HasGroup( current_section_string ) )
        {
            // Can't error handle well, since MSW always returns an error
            execute_plucker_build( current_section_string );
            // If we are installing immediately (vs in a batch at end)...
#if ( ! setupSEND_CHANNEL_PDBS_TO_DESTINATIONS_IN_BATCH_AT_END_NOT_IMMEDIATELY )
            // ...install or not, depending on the parameter passed into to this function
            if ( install ) 
            {
                install_channels( channel_sections );
            }
#endif
            // Now set the update base.
            set_channel_update_base( current_section_string );
        }
    }

    // If we are installing in a batch at end...
#if ( setupSEND_CHANNEL_PDBS_TO_DESTINATIONS_IN_BATCH_AT_END_NOT_IMMEDIATELY )
    // ...install or not, depending on the parameter passed into to this function
    if ( install ) 
    {
        install_channels( channel_sections );
    }
#endif

    // Flush the config file, since changed the update_base values
    the_configuration->Flush();
    
    // If a after_group_command was specified, execute it
    wxString after_group_command;
    after_group_command = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/after_group_command" ), wxT( "" ) );
    if ( after_group_command != wxT( "" ) ) 
    {
        // TRUE means wait for the program to finish before continuing.
        wxExecute( after_group_command, TRUE );
    }  
    
    // All done. Okay to do a new execution whenever want.
    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );

    return TRUE;
}


bool plucker_controller::update_all_channels( bool install )
{    
    wxArrayString update_sections_array;   // Array to pass to update_channels function.

    wxLogDebug( wxT( "Starting to update_all_channels()" ) );
    get_all_channels( update_sections_array );    
    update_channels( update_sections_array, install );
    update_sections_array.Clear();
    
    return TRUE;
}


bool plucker_controller::update_due_channels( bool install )
{    
    wxArrayString update_sections_array;
    
    get_due_channels( update_sections_array );     
    update_channels( update_sections_array, install );    
    update_sections_array.Clear();
    
    return TRUE;
}


void plucker_controller::get_all_channels( wxArrayString &channel_sections )
{
    wxString      section;                 // Section of channel in configuration file
    long          index           = 0;     // Index counter for looping through the sections
    bool          found_section   = FALSE; // To monitor success of getting first/next group
    
    wxLogDebug( wxT( "Entering plucker_controller::get_all_channels()" ) );

    found_section = the_configuration->GetFirstGroup( section, index );
    while ( found_section ) 
    {
        if ( is_channel_section( section ) )
        {
            channel_sections.Add( section );
            wxLogDebug( wxT( "Added %s section to array of all channels" ), section.c_str() ); 
        }            
    
        found_section = the_configuration->GetNextGroup( section, index );
    }    
}


void plucker_controller::get_due_channels( wxArrayString &channel_sections )
{
    wxString      section;                 // Section of channel in configuration file
    long          index           = 0;     // Index counter for looping through the sections
    bool          found_section   = FALSE; // To monitor success of getting first/next group
    
    wxLogDebug( wxT( "Entering plucker_controller::get_due_channels()" ) );

    found_section = the_configuration->GetFirstGroup( section, index );
    while ( found_section )
    {
        if ( is_channel_section( section ) && is_channel_due( section ) ) 
        {
            channel_sections.Add( section );
            wxLogDebug( wxT( "Added %s section to array of due channels" ), section.c_str() );         
        }            
    
        found_section = the_configuration->GetNextGroup( section, index );
    }    
}


void plucker_controller::get_channel_sections_by_doc_names( wxArrayString& doc_names,
                                                            wxArrayString& channel_sections
                                                          )
{
    wxArrayString all_channel_sections;

    // Temps, to make code clearer.
    wxString current_channel_section;
    wxString current_doc_name;

    // Get all sections
    get_all_channels( all_channel_sections );
    
    // Loop through all the sections
    for ( size_t i=0; i < all_channel_sections.GetCount(); i++ ) 
    {
        current_channel_section = all_channel_sections.Item( i );

        // Get the doc_name of the current section.
        current_doc_name = the_configuration->Read( wxT( "/" ) + current_channel_section + wxT( "/" ) 
                                                    + wxT( "doc_name" ), wxEmptyString );
        
        //! \test Crash if emptystring?
        // If that doc_name is in our passed array of doc_names...                                               
        if ( doc_names.Index( current_doc_name ) != wxNOT_FOUND ) 
        {
            // ...add the current section to our list
            channel_sections.Add( current_channel_section );
        }
        
    }

    all_channel_sections.Clear();
}     
                                                           


bool plucker_controller::install_channels( wxArrayString& channel_sections ) 
{
    size_t      number_of_sections           = channel_sections.GetCount();
    size_t      current_section_array_index;
    wxString    current_section;
    wxString    current_section_config_path;
    wxString    doc_file_fullname;
    wxString    doc_file_extension           = wxT( "pdb" );

    wxLogDebug( wxT( "Entering plucker_controller::install_channels function" ) );

    if ( number_of_sections == 0 ) 
    {
        return FALSE;
    }

    //------WXGTK/X11: users---------------------------------------------------------------
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)

    wxArrayString   doc_file_fullnames_to_install;

    for ( current_section_array_index = 0;
          (int) current_section_array_index < (int) number_of_sections;
          current_section_array_index++ )
    {
        current_section = channel_sections.Item( current_section_array_index );
        current_section_config_path = wxT( "/" ) + current_section + wxT( "/" );

        if ( the_configuration->Exists( current_section_config_path + wxT( "doc_file" ) ) ) 
        {
            the_configuration->Read( current_section_config_path + wxT( "doc_file" ),
                                     &doc_file_fullname );

            // Convert a file:// or plucker:// protocol to something normal
            doc_file_fullname = protocol_filename_to_normal_fullname( doc_file_fullname );

            // The doc_file filepath doesn't have the trailing .pdb so need to add it.
            doc_file_fullname += wxT( "." ) + doc_file_extension;

            // WXGTK doesn't do usernames. So if just any non-zero username, then
            // add to list of things to send to handheld.
            if ( the_configuration->Read( current_section_config_path + wxT( "user" ), wxT( "" ) ) != wxT( "" ) ) 
            {
                doc_file_fullnames_to_install.Add( doc_file_fullname );
            }
        } 
        else 
        {
            //TODO-OPTIONAL: could handle all the depreciated keys but I don't feel like
            //it anymore.
            wxLogError( wxT( "No 'doc_file=' key found in channel to install" ) );
        }
    }

    // Add a new member to our handheld_dest_array.
    // The "new" we create here, gets "delete"ed lower down
    handheld_dest_type* handheld_dest    = new handheld_dest_type();
    handheld_dest->user_name = wxT( "dummy_user_name" );
    m_palm_installer->install_files_by_handheld_dest( *handheld_dest,
                                                      doc_file_fullnames_to_install );
    // Delete to free memory:
    delete handheld_dest;

    // Clear the memory allocated by array.
    doc_file_fullnames_to_install.Clear();

    //------Non-WXGTK: users----------------------------------------------------------
#else
    // Will clear with WX_CLEAR_ARRAY macro
    handheld_dest_array_type   handheld_dest_array;

    for ( current_section_array_index = 0;
          (int) current_section_array_index < (int) number_of_sections;
          current_section_array_index++ ) 
    {
        current_section = channel_sections.Item( current_section_array_index );
        current_section_config_path = wxT( '/' ) + current_section + wxT( '/' );

        wxLogDebug( wxT( "current_section_config_path=" + current_section_config_path ) );

        if ( the_configuration->Exists( current_section_config_path + wxT( "doc_file" ) ) ) 
        {
            the_configuration->Read( current_section_config_path + wxT( "doc_file" ),
                                     &doc_file_fullname );

            // Convert a file:// or plucker:// protocol to something normal
            doc_file_fullname = protocol_filename_to_normal_fullname( doc_file_fullname );

            // The doc_file filepath doesn't have the trailing .pdb so need to add it.
            doc_file_fullname += wxT( '.' ) + doc_file_extension;

            wxLogDebug( wxT( "doc_file_fullname converted to =" + doc_file_fullname ) );

            // The WX_CLEAR_ARRAY(array) macro [see dynarray.h] is required to clear out all the 
            // elements from memory of an array of structs that was built with WX_DEFINE_ARRAY.
            // All that this macro does is just to loop through the array, "delete" each element
            // to free it from memory, then mark the array as Empty().
            // This clearing is from the previous iteration of the loop
            WX_CLEAR_ARRAY( handheld_dest_array );

            // Read the handheld destinations:
            handheld_dest_wrappers::read_handheld_dest_array_from_configuration( current_section,
                                                                                 &handheld_dest_array );
            // Loop through the dest's sending the file to each one.
            for ( size_t n=0; n < handheld_dest_array.GetCount(); n++ ) 
            {
                m_palm_installer->install_file_by_handheld_dest( *handheld_dest_array.Item( n ),
                                                                  doc_file_fullname );
            }
        } 
        else 
        {
            //TODO-OPTIONAL: could handle all the depreciated keys but I don't feel like
            //it anymore.
            wxLogError( wxT( "No 'doc_file=' key found in channel to install" ) );
        }
    }

    // Clear the memory allocated by array.
    // The WX_CLEAR_ARRAY(array) macro [see dynarray.h] is required to clear out all the 
    // elements from memory of an array of structs that was built with WX_DEFINE_ARRAY.
    // All that this macro does is just to loop through the array, "delete" each element
    // to free it from memory, then mark the array as Empty().
    WX_CLEAR_ARRAY( handheld_dest_array );

#endif  // (__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)

    //------All: Directories----------------------------------------------------------
    wxArrayString   directory_array;
    wxString        directory_configuration_string;
    wxString        directory_item;
    wxString        output_fullname;
    wxString        doc_file_filename;
    wxString        doc_file_basename;
    
    for ( current_section_array_index = 0; 
          (int) current_section_array_index < (int) number_of_sections;
          current_section_array_index++ ) 
    {
          
        current_section = channel_sections.Item( current_section_array_index );
        current_section_config_path = wxT( "/" ) + current_section + wxT( "/" );
        
        wxLogDebug( wxT( "current_section_config_path=" + current_section_config_path ) );
        
        if ( the_configuration->Exists( current_section_config_path + wxT( "doc_file" ) ) ) 
        {
            the_configuration->Read( current_section_config_path + wxT( "doc_file" ),
                                     &doc_file_fullname );

            // Convert a file:// or plucker:// protocol to something normal
            doc_file_fullname = protocol_filename_to_normal_fullname( doc_file_fullname );

            // The doc_file filepath doesn't have the trailing .pdb so need to add it.
            doc_file_fullname += wxT( "." ) + doc_file_extension;

            wxLogDebug( wxT( "doc_file_fullname converted to =" + doc_file_fullname ) );

            // Remove all elements of array from last loop iteration.
            directory_array.Empty();

            if ( the_configuration->Read( current_section_config_path + wxT( "copy_to_dir" ),
                                          &directory_configuration_string ) )
            {
                // Only proceed if directory_configuration_string isn't blank and the file to copy exists.
                if ( ( directory_configuration_string != wxT( "" ) )
                     && wxFileExists( doc_file_fullname ) ) 
                {
                    utils_string::string_to_arraystring( directory_configuration_string, plkrOS_FILE_ARRAY_SEPARATOR,
                                           &directory_array, TRUE );

                    // Loop through all the directories...
                    for ( int n=0; n < (int)directory_array.GetCount(); n++ ) 
                    {
                         // ...installing the file to the directory
                         m_palm_installer->install_file_by_directory( directory_array.Item( n ),
                                                                      doc_file_fullname );
                    }
                }
            }
        }
        else 
        {
            //TODO-OPTIONAL: could handle all the deprecated keys but I don't feel like
            //it anymore.
            wxLogError( wxT( "No 'doc_file=' key found in channel to install" ) );
        }
    }

    directory_array.Clear();

    return TRUE;
}


bool plucker_controller::create_new_channel_subdirectory(
    const wxString &channel_section,
    bool           copy_template_files 
    )
{      
    bool creation = FALSE;
    
    if ( ! wxDirExists( get_plucker_directory( CHANNELS ) + wxT( "/" ) + channel_section ) )
    {   
        creation = wxMkdir( get_plucker_directory( CHANNELS ) + wxT( "/" ) + channel_section, 0777 );
        wxLogDebug( wxT( "Finished testing/trying to make channel-specific directory" ) );
    
        creation = wxMkdir( get_plucker_directory( CHANNELS ) + wxT( "/" ) + channel_section 
                            + wxT( "/cache" ), 0777 );
        wxLogDebug( wxT( "Finished testing or trying to make channel's cache directory" ) );
    } 
    else 
    {
        creation = FALSE;
    }    
    
    // Copy over the template files, if so specified. Optional to do here would be a 
    // wxDirTraveller that would automatically iterate over all files in the directory
    if ( copy_template_files ) 
    {
        // Since 4 argument (overwrite) is FALSE, these things will only do anything if the 
        // file doesn't already exist.
        copy_file_between_channels( plkrNEW_CHANNEL_TEMPLATE_SECTION, channel_section, 
                                        wxT( "home.html" ), FALSE );
        copy_file_between_channels( plkrNEW_CHANNEL_TEMPLATE_SECTION, channel_section,
                                    wxT( "exclusionlist.txt"), FALSE );           
    }
    return creation;
}


bool plucker_controller::delete_channel_subdirectory( const wxString& channel_section )
{
    bool successful = FALSE;
    wxString directory_to_delete;
    wxString file_to_delete;

    // Get the plucker:/channel/channel_section/cache/ directory
    directory_to_delete =  get_plucker_directory( CHANNELS ) + wxT( "/" ) + channel_section 
                           + wxT( "/" ) + wxT( "cache" );
                           
    // If that cache directory exists [otherwise error when try to enumerate files 
    // in a non-existant directory... 
    if ( wxDirExists( directory_to_delete ) ) 
    {
        // Delete the contents of the plucker:/channel/channel_section/cache/ 
        // Using a * wildcard (this automatically doesn't include "." or ".." directories,
        // and also a wxFILE filter, so no directories.
        file_to_delete= wxFindFirstFile( get_plucker_directory( CHANNELS ) + wxT( "/" ) 
                                         + channel_section + wxT( "/" ) + wxT( "cache" ) + wxT( "/" ) + wxT( "*" ), wxFILE );
        while ( ! file_to_delete.IsEmpty() ) 
        {
            wxRemoveFile( file_to_delete );
            file_to_delete = wxFindNextFile(); 
        } 

        // Now delete the cache directory itself, now that it is empty.
        // The 0 is a flags, reserved by wx for future use. 
        successful = wxRmdir( directory_to_delete, 0 );
    }
    wxLogDebug( wxT( "Ability to remove directory %s = %d" ), directory_to_delete.c_str(), successful );

    // Delete the contents of the plucker:/channel/channel_section/
    // Using a * wildcard (this automatically doesn't inlcude "." or ".." directories, and also a 
    // wxFILE filter, so no directories.
    file_to_delete= wxFindFirstFile( get_plucker_directory( CHANNELS ) + wxT( "/" ) + channel_section 
                                                        + wxT( "/" ) + wxT( "*" ), wxFILE  );
    while ( ! file_to_delete.IsEmpty() ) 
    {
        wxRemoveFile( file_to_delete );
        file_to_delete = wxFindNextFile(); 
    } 

    // Delete the now empty plucker:/channel/channel_section/ directory itself
    directory_to_delete =  get_plucker_directory( CHANNELS ) + wxT( "/" ) + channel_section;
    if ( wxDirExists( directory_to_delete ) ) 
    {
        // The 0 is a flags, reserved by wx for future use
        successful = wxRmdir( directory_to_delete, 0 );
    }
    wxLogDebug( wxT( "Ability to remove directory %s = %d" ), directory_to_delete.c_str(), successful );
    return successful;
}


bool plucker_controller::is_channel_due( const wxString &channel_section )
{
    wxDateTime now_datetime   = utils_datetime::get_current_datetime();
    // Get the currently-set duedatetime, don't increment it yet.  That's done
    // after getting it.
    wxDateTime due_datetime   = get_channel_due_datetime( channel_section, FALSE );    
    
    if ( is_channel_update_enabled( channel_section ) &&
         due_datetime.IsEarlierThan( now_datetime ) ) 
    {
        return TRUE;
    }
    else 
    {
        return FALSE;
    }
}


bool plucker_controller::is_any_channel_due()
{
    bool is_any_channel_due;    
    wxArrayString channel_sections;
    
    // Store a list of due channels before update_due(which will change them)
    get_due_channels( channel_sections );            
    
    // Store whether array is empty or not
    if ( channel_sections.IsEmpty() ) 
    {
        is_any_channel_due = FALSE;
    } 
    else 
    {
        is_any_channel_due = TRUE;
    }
    
    channel_sections.Clear();
    
    return is_any_channel_due;
}    


bool plucker_controller::is_channel_update_enabled( const wxString &channel_section )
{
    bool update_enabled;
    
    update_enabled = (the_configuration->Read( wxT( "/" ) + channel_section + wxT( "/" ) + wxT( "update_enabled" ), 0L ) != 0);
    wxLogDebug( wxT( "Channel update_enabled for channel " ) + channel_section + wxT( "=%ld" ), (long)update_enabled );
    return update_enabled;
}


wxDateTime plucker_controller::get_channel_due_datetime( const wxString &channel_section, const bool shall_increment )
{
    wxDateTime  current_datetime;
    wxString    update_base_string; 
    wxDateTime  update_base_datetime;  
    wxDateTime  incremented_datetime;
    
    current_datetime        = utils_datetime::get_current_datetime();
    update_base_string      = the_configuration->Read( wxT( "/" ) + channel_section + wxT( "/" )
                                                       + wxT( "update_base" ), wxT( "" ) );
    update_base_datetime    = utils_datetime::w3c_string_to_datetime( update_base_string );
    incremented_datetime    = get_incremented_channel_due_datetime( channel_section, 1 );

    if ( ( ! shall_increment ) || ( update_base_datetime.IsLaterThan( current_datetime ) ) ) 
    {
        return update_base_datetime;
    } 
    else 
    {
        return incremented_datetime;
    }    
}


void plucker_controller::set_channel_update_base( const wxString &channel_section )
{
    wxDateTime  current_datetime;
    wxDateTime  new_update_base;
    wxString    new_update_base_string;    
    
    new_update_base_string  = the_configuration->Read( wxT( "/" ) + channel_section + wxT( "/" )
                                                       + wxT( "update_base" ), wxT( "" ) );
    new_update_base         = utils_datetime::w3c_string_to_datetime( new_update_base_string );                                  
    current_datetime        = utils_datetime::get_current_datetime();

    // Keep incrementing update_base until it is a datetime in the future    
    for ( int i=1; new_update_base.IsEarlierThan( current_datetime ); i++ )
    {
        new_update_base = get_incremented_channel_due_datetime( channel_section, i );
#ifdef __WXDEBUG__
        new_update_base_string  = utils_datetime::datetime_to_w3c_string( new_update_base );
        wxLogDebug( wxT( "set_channel_update_base: new update_base=" ) + new_update_base_string );
#endif
    }
    
    new_update_base_string  = utils_datetime::datetime_to_w3c_string( new_update_base );
    
    // Set the value in the config. No flush, that is the calling function's job,
    // as usually will be doing updating a few channels at once
    the_configuration->Write( wxT( "/" ) + channel_section + wxT( "/" ) + wxT( "update_base" ),
                              new_update_base_string );       
}


bool plucker_controller::is_all_criteria_met_to_execute_an_autoupdate()
{
    wxLogDebug( wxT( "Entering plucker_controller::is_all_criteria_met_to_execute_an_autoupdate()" ) );
    
    // First, check and abort if a new update is forbidden (since a progress dialog
    // is already running, etc)
    if ( get_is_new_update_execution_allowed() == FALSE ) 
    {
        wxLogDebug( wxT( "get_is_new_update_execution_allowed()==FALSE so aborting possible autoupdate" ) );
        return FALSE;
    }

    // We can only get here if passed the above check. Now look through the config
    // to see if there is any channels due. This is done at the end since it can be
    // a rather hefty operation with possible long disk access if tons of channels.
    wxLogDebug( wxT( "Autoupdate mode time criteria met...Checking to see if any channel is due" ) );
    if ( plucker_controller::get()->is_any_channel_due() ) {
        wxLogDebug( wxT( "A channel is due...informing application to begin update" ) );
        return TRUE;
    } 
    else 
    {
        wxLogDebug( wxT( "No channels are due at this time. So doing nothing until next check." ));
        return FALSE;
    }    
}


bool plucker_controller::get_is_new_update_execution_allowed()
{
    return m_is_new_update_execution_allowed;
}


void plucker_controller::set_is_new_update_execution_allowed( bool is_allowed )
{
    m_is_new_update_execution_allowed = is_allowed;
}

//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

bool plucker_controller::copy_file_between_channels(
     const wxString& from_section,
     const wxString& to_section,
     const wxString& filename,
     bool overwrite
     )
{
    bool successful = FALSE;
    
    // Set the fullnames of the file, based on the filename
    wxString from_fullname =  get_plucker_directory( CHANNELS ) + wxT( "/" ) + 
                              from_section + wxT( "/" ) + filename;
                            
    wxString to_fullname = get_plucker_directory( CHANNELS ) + wxT( "/" ) +
                           to_section + wxT( "/" ) + filename; 
 
    if ( ! wxFileExists( to_fullname ) || overwrite ) 
    {
        wxLogDebug( wxT( "Attempting copy file between channels: From=" ) + from_fullname );
        wxLogDebug( wxT( "Attempting copy file between channels. To=" ) + to_fullname );
        // First try to copy the file from the "from channel"'s dir to the "to channel"'s subdir.
        // FALSE argument just means won't overwrite it.   
        if ( wxFileExists ( from_fullname ) ) 
        {
            wxCopyFile( from_fullname, to_fullname, FALSE );
            successful = TRUE;   
            wxLogDebug( wxT( "Copied new channel's file called " ) + filename );
        // Else no "from file" found in the "from channel"'s subdirectory.
        // Just generate a blank one in channel.
        } 
        else 
        {
            wxFile empty_file;
            successful = empty_file.Create( to_fullname );
            empty_file.Close();
            wxLogDebug( wxT( "Finished trying to make new channel's file called " ) + filename );
        }
    }     
    
    return successful;
}


bool plucker_controller::execute_plucker_build( const wxString &channel_section )
{

    bool execute_error_occurred;

//----------------------------------------------------------------------------------------

    // First order of ops is to set all 3 key sections [<OS>] [DEFAULT] and 
    // [<channel section>] to whatever the values say in the [<OS>] section.
    // This is to prevent overrides as it decends.
    bool enable_close_on_exit = (the_configuration->Read( wxT( "/" ) + get_os_configuration_section() + wxT( "/" ) + wxT( "close_on_exit" ), 0L ) != 0);    
                            
    // Include writing it to the OS section, if there was none.
    set_close_on_exit( get_os_configuration_section(), enable_close_on_exit, FALSE, FALSE );
    set_close_on_exit( wxT( "DEFAULT" ), enable_close_on_exit, TRUE, FALSE );
    set_close_on_exit( channel_section, enable_close_on_exit, TRUE, TRUE );
    
    bool enable_close_on_error = (the_configuration->Read( wxT( "/" ) + get_os_configuration_section() + wxT( "/" ) + wxT( "close_on_error" ), 0L ) != 0);
                            
    // Include writing it to the OS section, if there was none.
    set_close_on_error( get_os_configuration_section(), enable_close_on_error, FALSE, FALSE );
    set_close_on_error( wxT( "DEFAULT" ), enable_close_on_error, TRUE, FALSE );
    set_close_on_error( channel_section, enable_close_on_error, TRUE, TRUE );
    
//---------------------------------------------------------------------------------------- 

#if ( setupSET_CWD_TO_PLUCKERHOME_BEFORE_BUILD )
    // See comments in setup.h
    wxString working_directory;
    working_directory = get_plucker_directory( PLUCKERHOME ) + wxT( "/" );
    wxSetWorkingDirectory( working_directory );
#endif

//---------------------------------------------------------------------------------------- 

    // Execute plucker-build in a spawned shell window, passing it the channel section of 
    // the configuration file (to look for to override any similar settings in the 
    // DEFAULT section).    
    wxString plucker_build_command_string;

    // This will either return a " --http-proxy-pass=somepassword" or will just 
    // be an empty string if not using proxy prompts.
    //! \todo Would be nicer to have a wxApp->GetTopWindow here instead of just a null parent
    wxString http_proxy_password_argument = proxy_prompt::get( (wxWindow* )NULL )->get_spider_argument();

    // Mac and Win port doesn't have a plucker-build, so have to build commandline by hand.
#ifdef __WXMAC__  
    plucker_build_command_string << wxT( "python" ) << wxT( " \"" ) 
                << get_plucker_directory( PYPLUCKER ) << wxT( "/Spider.py\"" ) 
                << wxT( " -P" ) << wxT( " \"" ) << get_plucker_directory( PLUCKERHOME ) 
                << wxT( "\" -s" ) << wxT( " " ) << channel_section
                << http_proxy_password_argument; 
#elif defined(__WXMSW__)
        plucker_build_command_string << wxT( "\"" ) << get_plucker_directory( PYTHON_VM ) 
                << wxT( "/python" ) << wxT( " \"" ) 
                << get_plucker_directory( PYPLUCKER ) << wxT( "/Spider.py\"" ) 
                << wxT( " -P" ) << wxT( " \"" ) << get_plucker_directory( PLUCKERHOME ) 
                << wxT( "\" -s" ) << wxT( " " ) << channel_section
                << http_proxy_password_argument; 
#else
    // Otherwise just use plucker-build.
    plucker_build_command_string << wxT( "plucker-build" ) << wxT( " -s " ) << channel_section << http_proxy_password_argument;   
#endif	//__WXMAC__

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)

    // Create a nice title for the terminal...:
    // ...First read the channel name....
    wxString channel_name = the_configuration->Read( wxT( "/" ) + channel_section + wxT( "/" ) +    
                                                     wxT( "doc_name" ), plkrUNNAMED_CHANNEL_NAME );                                                     
    // ...then give it a nice prefix.
    wxString shell_console_title;
    shell_console_title << plkrAPPLICATION_SUITE_NAME << wxT( " " ) << _( "channel update status" ) << wxT( ": " ) << channel_name;
    
    // If this channel is specified to close on exit [ recall the code at the top of this 
    // function that checked ]... then make our xterm shell stay open when done.   
    // The TRUE is to execute synchronously--that is, wait until it completes before 
    // proceeding. This is required if we are going to copy the output that it creates.
    execute_error_occurred = utils_execute::launch_xterm_shell( plucker_build_command_string, 
                                                                shell_console_title,
                                                                enable_close_on_exit,
                                                                TRUE );
       
#else    
    // WXMSW does the expected behaviour, and there is nothing else that can be set.
    // There is no xterm on Macintosh. There is only /Applications/Utilities/Terminal.app/Contents/MacOS/Terminal
    execute_error_occurred = wxShell( plucker_build_command_string );  
#endif

	//! \todo Ensure that this doesn't screw up stderr, etc.
	wxLogDebug ( wxT( "plucker-build string=" ) + plucker_build_command_string );	
    return execute_error_occurred;
}


wxDateTime plucker_controller::get_incremented_channel_due_datetime
(
    const   wxString &channel_section,
    int     increments    
)
{
    wxString    update_period;
    long        update_frequency;
    wxString    update_base_string;    
    wxDateTime  incremented_datetime;
    
    update_period           = the_configuration->Read( wxT( "/" ) + channel_section + wxT( "/" )
                                                   + wxT( "update_period" ), wxT( "daily" ) );
    
    update_frequency        = the_configuration->Read( wxT( "/" ) + channel_section + wxT( "/" )
                                                   + wxT( "update_frequency" ), 1L );
    
    update_base_string      = the_configuration->Read( wxT( "/" ) + channel_section + wxT( "/" )
                                                   + wxT( "update_base" ), wxT( "" ) );
    
    incremented_datetime    = utils_datetime::w3c_string_to_datetime( update_base_string );
    
    // wxDateSpan wasn't in docs. See calctrl for example usage
    // TODO. Cleaner implementation with multiplication of update_period * update_frequency?
    
    // NOTE: Using the method used for update_period=="daily", and update_period=="weekly"
    // doesn't seem to work on wx2.3.1 (* operator not implemented correctly or something).
    //  Therefore attempting just to construct the timespan directly (the hours in the 
    // constructor aren't limited to 0...24 range). This way is faster anyways.
    if ( update_period == wxT( "hourly" ) )
    { 
        long        number_of_hours = update_frequency * increments;
        wxTimeSpan  update_frequency_timespan( number_of_hours, 0, 0, 0 );
        incremented_datetime += update_frequency_timespan;        
    // Some debugging info, to show that it worked
#ifdef __WXDEBUG__
        wxString    incremented_datetime_string;
        incremented_datetime_string = utils_datetime::datetime_to_w3c_string( incremented_datetime );
        wxLogDebug( wxT( "get_incremented_channel_due_datetime(): incremented datetime=" )
                    + incremented_datetime_string );
#endif        
    } 
    else if ( update_period == wxT( "daily" ) ) 
    {  
        for ( int i=0; i < update_frequency * increments; i++ ) 
        {      
            incremented_datetime += wxDateSpan::Day();
        }
    } 
    else if ( update_period == wxT( "weekly" ) ) 
    { 
        for ( int i=0; i < update_frequency * increments; i++ ) 
        {
            incremented_datetime += wxDateSpan::Week();
        }
    }     
    else if ( update_period == wxT( "monthly" ) ) 
    { 
        for ( int i=0; i < update_frequency * increments; i++ ) 
        {
            incremented_datetime += wxDateSpan::Month();
        }
    } 
    else if ( update_period == wxT( "yearly" ) ) 
    { 
        for ( int i=0; i < update_frequency * increments; i++ ) 
        {
            incremented_datetime += wxDateSpan::Year();
        }
    }  
    else 
    {
        wxLogError( wxT( "Error: unrecognized update_frequency key called" ) + update_period
                    + wxT( "in section" ) + channel_section );
        incremented_datetime += wxDateSpan::Day();
    } 
    
    return incremented_datetime;
}


bool plucker_controller::create_channels_directory()
{
    bool creation = FALSE;
    
    if ( ! wxDirExists( get_plucker_directory( CHANNELS ) ) ) 
    {   
        /*! \todo GTK: 777 proper permissions here? */
        creation = wxMkdir( get_plucker_directory( CHANNELS ), 0777 );        
        wxLogDebug( wxT( "Finished testing/trying to make /channels directory" ) );
    } 
    else 
    {
        creation = FALSE;
    }    
    
    return creation;
}


bool plucker_controller::create_pluckerhome_directory()
{
    bool creation = FALSE;

#if defined(__WXGTK__) || defined(__WXX11__)  || defined(__WXMOTIF__) || defined(__WXMAC__)
    if ( ! wxDirExists( get_plucker_directory( PLUCKERHOME ) ) )
    {   
        /*! \todo GTK: 777 proper permissions here? */
        creation = wxMkdir( get_plucker_directory( PLUCKERHOME ), 0777 );        
        wxLogDebug( wxT( "Finished testing/trying to make a pluckerhome directory." ) );
    } 
    else 
    {
        creation = FALSE;
    }    
#endif
    
    return creation;   
}


bool plucker_controller::create_default_exclusion_list()
{
    bool creation = FALSE;
    
    wxString system_default_exclusion_list   = get_plucker_directory( RESOURCES ) + wxT( "/strings_default_exclusion_list/exclusionlist.txt" );
    wxString user_default_exclusion_list     = get_plucker_directory( PLUCKERHOME ) + wxT( "/exclusionlist.txt" );
    
    if ( ! wxFileExists( user_default_exclusion_list ) )
    {   
        if ( wxFileExists ( system_default_exclusion_list ) )
        {   
            // False means don't overwrite
            creation = wxCopyFile( system_default_exclusion_list, user_default_exclusion_list, FALSE );    
            wxLogDebug( wxT( "Finished creating user's default exclusionlist.txt" ) );
        }
    } 
    else 
    {
        creation = FALSE;
    }    
    
    return creation;
}


void plucker_controller::create_channel_template_section_and_subdirectory()
{    
    // If the group doesn't exist, then create it by writing in a key.
    if ( ! the_configuration->HasGroup( plkrNEW_CHANNEL_TEMPLATE_SECTION ) ) 
    {
        wxString a_default_key;
        a_default_key << wxT( "/" ) << plkrNEW_CHANNEL_TEMPLATE_SECTION << wxT( "/" ) << wxT( "doc_name" );
        the_configuration->Write( a_default_key, _( "Unnamed Channel" ) );
        wxLogDebug( wxT( "Created a channel template section in configuration file" ) );
    }
    
    // FALSE as second argument here, as don't want to copy over the template files
    // to the target as this is the template, so none can exist.
    create_new_channel_subdirectory( plkrNEW_CHANNEL_TEMPLATE_SECTION, FALSE );
    wxLogDebug( wxT( "Finished checking/adding a template channel sections / subdirectory" ) );    
}

//----------------------------------------------------------------------------------------
// Module definition
//----------------------------------------------------------------------------------------

class module_plucker_controller : public wxModule
{

    DECLARE_DYNAMIC_CLASS( module_plucker_controller )
    
public:

    module_plucker_controller() {}
    
    bool OnInit()
    {
        return TRUE;
    }
    
    void OnExit()
    {
        // We set it to null, and a pointer returns to the previous one, which we delete.
        delete plucker_controller::set( NULL );
    }

};

IMPLEMENT_DYNAMIC_CLASS( module_plucker_controller, wxModule )
