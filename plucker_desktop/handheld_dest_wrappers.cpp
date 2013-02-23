//----------------------------------------------------------------------------------------
// Name:        handheld_dest_wrappers.cpp
// Purpose:     Describes handheld_dest_wrappers
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: handheld_dest_wrappers.cpp,v 1.9 2007/06/29 00:58:52 robertoconnor Exp $

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "handheld_dest_wrappers.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

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

#include "handheld_dest_wrappers.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "utils_string.h"
#include "configuration.h"

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

void handheld_dest_wrappers::read_handheld_dest_array_from_configuration(
                                          const wxString& channel_section,
                                          handheld_dest_array_type *handheld_dest_array )
{
    wxLogDebug( wxT( "Entering handheld_dest_wrappers::read_handheld_dest_array_from_configuration()" ) );
    wxString            user_name_string;
    wxString            handheld_target_storage_mode_string;
    wxString            directory_on_card_string;
    wxString			is_usb_pause_string;
    wxArrayString       user_name_array;
    wxArrayInt          handheld_target_storage_mode_array;
    wxArrayString       directory_on_card_array;
    wxArrayInt          is_usb_pause_array;

    wxString            channel_section_rootpath;
    channel_section_rootpath << wxT( "/" ) << channel_section << wxT( "/" );

    user_name_string          = the_configuration->Read( channel_section_rootpath + wxT( "user" ), wxT( "" ) );
    handheld_target_storage_mode_string = the_configuration->Read( channel_section_rootpath + wxT( "handheld_target_storage_mode" ), wxT( "" ) );
    directory_on_card_string  = the_configuration->Read( channel_section_rootpath + wxT( "directory_on_card" ), wxT( "" ) );
    is_usb_pause_string       = the_configuration->Read( channel_section_rootpath + wxT( "is_usb_pause" ), wxT( "" ) );
    utils_string::string_to_arraystring( user_name_string, ';', &user_name_array, TRUE );
    utils_string::string_to_arrayint( handheld_target_storage_mode_string, ';', &handheld_target_storage_mode_array, TRUE );
    utils_string::string_to_arraystring( directory_on_card_string, ';', &directory_on_card_array, TRUE );
    utils_string::string_to_arrayint( is_usb_pause_string, ';', &is_usb_pause_array, TRUE );

    for ( size_t n=0; n < user_name_array.GetCount(); n++ ) 
    {
        // Add a new member to our handheld_dest_array.
        // The "new" we create here, gets "delete"ed whereever we called this from.
        // with a WX_CLEAR_ARRAY macro.
        handheld_dest_type* handheld_dest    = new handheld_dest_type();
        // Now add it to the array
        handheld_dest_array->Add( handheld_dest );
        // Now can play with the newly added array item, and change its user_name
        handheld_dest_array->Item( n )->user_name = user_name_array.Item( n );
        // Checking to make sure that the card array is at least as long as the user
        // name array, so don't try to access an element past the end.
        if ( n < handheld_target_storage_mode_array.GetCount() ) 
        {
            handheld_dest_array->Item( n )->handheld_target_storage_mode = (long)handheld_target_storage_mode_array.Item( n );
        }
        // Checking to make sure that the card directory array is at least as long as the
        // user name array, so don't try to access an element past the end.
        if ( n < directory_on_card_array.GetCount() ) 
        {
            handheld_dest_array->Item( n )->directory_on_card = directory_on_card_array.Item( n );
        }
        // Checking to make sure that the card directory array is at least as long as the
        // user name array, so don't try to access an element past the end.
        if ( n < is_usb_pause_array.GetCount() ) 
        {
            handheld_dest_array->Item( n )->is_usb_pause = is_usb_pause_array.Item( n );
        }
        
    }
    // Free memory
    user_name_array.Clear();
    handheld_target_storage_mode_array.Clear();
    directory_on_card_array.Clear();
    is_usb_pause_array.Clear();
    wxLogDebug( wxT( "Finished handheld_dest_wrappers::read_handheld_dest_array_from_configuration" ) );
}


void handheld_dest_wrappers::write_handheld_dest_array_to_configuration(
                                          const wxString& channel_section,
                                          handheld_dest_array_type& handheld_dest_array
                                          )
{
    wxLogDebug( wxT( "Entering handheld_dest_wrappers::write_handheld_dest_array_from_configuration" ) );
    wxString        user_name_string;
    wxString        handheld_target_storage_mode_string;
    wxString        directory_on_card_string;
    wxString		is_usb_pause_string;
    wxArrayString   user_name_array;
    wxArrayInt      handheld_target_storage_mode_array;
    wxArrayString   directory_on_card_array;
    wxArrayInt      is_usb_pause_array;

    wxString        channel_section_rootpath;
    channel_section_rootpath << wxT( "/" ) << channel_section << wxT( "/" );

    // Build arrays
    for ( size_t n=0; n < handheld_dest_array.GetCount(); n++ ) 
    {
        user_name_array.Add( handheld_dest_array.Item( n )->user_name );
        handheld_target_storage_mode_array.Add( (int)handheld_dest_array.Item( n )->handheld_target_storage_mode );
        directory_on_card_array.Add( handheld_dest_array.Item( n )->directory_on_card );
        is_usb_pause_array.Add( (int)handheld_dest_array.Item( n )->is_usb_pause );
    }

    // Convert
    user_name_string          = utils_string::arraystring_to_string( user_name_array, ';' );
    handheld_target_storage_mode_string = utils_string::arrayint_to_string( handheld_target_storage_mode_array, ';' );
    directory_on_card_string  = utils_string::arraystring_to_string( directory_on_card_array, ';' );
    is_usb_pause_string = utils_string::arrayint_to_string( is_usb_pause_array, ';' );

    // Write
    the_configuration->Write( channel_section_rootpath + wxT( "user" ), user_name_string );
    the_configuration->Write( channel_section_rootpath + wxT( "handheld_target_storage_mode" ), handheld_target_storage_mode_string );
    the_configuration->Write( channel_section_rootpath + wxT( "directory_on_card" ), directory_on_card_string );
    the_configuration->Write( channel_section_rootpath + wxT( "is_usb_pause" ), is_usb_pause_string );

    // Free memory
    user_name_array.Clear();
    handheld_target_storage_mode_array.Clear();
    directory_on_card_array.Clear();
    is_usb_pause_array.Clear();
    wxLogDebug( wxT( "Finished handheld_dest_wrappers::write_handheld_dest_array_from_configuration" ) );

}
