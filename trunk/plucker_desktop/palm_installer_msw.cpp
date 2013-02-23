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
// RCS-ID:      $Id: palm_installer_msw.cpp,v 1.23 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "palm_installer_msw.h"
#endif

//----------------------------------------------------------------------------------------
// Begin OS removal condition
//----------------------------------------------------------------------------------------

#ifdef __WXMSW__

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

#include "palm_installer_msw.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/wfstream.h"                    // wxInputFileStream (includes stream.h)
#include "wx/datstrm.h"                     // wxInputDataStream
#include "wx/file.h"                        // wxFile

// ---------------------------------------------------------------------------------------

#include "configuration.h"                  // Since looking up "show again" checkbox value
#if ( setupUSE_CHECKBOX_MESSAGE_DIALOG ) 
    #include "checkbox_message_dialog.h"    // A MessageDialog with a "don't show again" option
#endif

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( palm_installer, wxObject )

//----------------------------------------------------------------------------------------
// Interface
//----------------------------------------------------------------------------------------

// Constructor
palm_installer::palm_installer()
{ 
    //m_users_array = NULL;
    m_number_of_users = 0;
    m_last_user_index = 0;
    m_last_user_name = wxT( "" );

    // Store a pointer to the old wxConfig object, so can restore it when done this
    // constructor
    wxConfigBase* previous_configuration = NULL;

    // This will build the key, whether it exists or not.
    
    m_palm_user_config = new wxConfig( wxT( "Pilot Desktop" ) , wxT( "U.S. Robotics" ) );
    
    previous_configuration = wxConfigBase::Set( m_palm_user_config );
    
    // Check for a Desktop 1.0 entry
    if ( ! m_palm_user_config->HasGroup( wxT( "HotSync Manager" ) ) ) 
    {
        if ( m_palm_user_config ) 
        {
            delete m_palm_user_config;
        }
        // Else check for a Desktop 2.0 entry
        // TODO: Don't know what sotware name is under Desktop 2.0
        m_palm_user_config = new wxConfig( wxT( "Pilot Desktop" ), wxT( "Palm Computing" ) );
        if ( ! m_palm_user_config->HasGroup( wxT( "HotSync Manager" ) ) ) 
        {
            // Return to whatever wxConfig oject was running before (NULL is OK).
            wxConfigBase::Set( previous_configuration );            
            
#if ( setupUSE_CHECKBOX_MESSAGE_DIALOG )
            bool default_checked = TRUE;
            wxString show_message_path_key = ( wxT( "/PLUCKER_DESKTOP/message_dialog_enabled_palm_desktop" ) );
            if ( the_configuration->Read( show_message_path_key, (long)default_checked ) == (long)default_checked ) 
            {
                // Unfortuately using a NULL for window parent
                checkbox_message_dialog a_checkbox_message_dialog( NULL, 
                _( "Unable to access Palm Desktop software.\nYou will need to choose directories, not HotSync handhelds, as the destinations for channels." ),
                _( "Palm Desktop detection warning" ), wxOK|wxCENTRE|wxICON_EXCLAMATION, 
                wxDefaultPosition, _( "Always perform this check in the future" ),
                show_message_path_key,
                default_checked );
                
                a_checkbox_message_dialog.ShowModal();
            }      
#else 
            wxLogError( _( "Unable to access Palm Desktop software.\nYou will need to choose directories, not HotSync handhelds, as the destinations for channels." ) );
#endif                   
            // Abort now. Can't read configuration from Registry.
            return;
        }
    }
    
    if ( ! m_palm_user_config->Read( wxT( "/Preferences/LastUserName" ), &m_last_user_name ) ) 
    {
        // wxLogDebug, not a wxLogError, since perhaps never synced yet, since 
        // they installed the Palm Desktop (unlikely, but possible).
        wxLogDebug( _( "Error: unable to get Lastuser_name from Registry" ) );    
    }
    
    if ( ! m_palm_user_config->Read( wxT( "/Core/Path" ), &m_palm_desktop_path ) )
    {
        wxLogError( _( "Error: unable to get Palm path from Registry" ) );  
    }
    
    // Return to whatever wxConfig oject was running before (NULL is OK).
    wxConfigBase::Set( previous_configuration );    
    wxLogDebug( wxT( "Palm path=%s" ), m_palm_desktop_path.c_str() );
    
    // All set, so now read the users.dat file to load up the user Array values 
    load_user_data();
}


// Destructor
// Delete the configuration object when finished program, otherwise memory leak
palm_installer::~palm_installer()
{
   if ( m_palm_user_config ) 
   {
      delete m_palm_user_config;
      m_palm_user_config = NULL;
   }
}


bool palm_installer::install_file_by_handheld_dest( handheld_dest_type& handheld_dest,
                                                    const wxString &install_fullname )
{
    bool            successful;
    wxArrayString   install_fullnames;
    wxString        install_file_to_add = install_fullname;

    install_fullnames.Add( install_file_to_add );

    successful = install_files_by_handheld_dest( handheld_dest, install_fullnames );

    return successful;
}


bool palm_installer::install_files_by_handheld_dest( handheld_dest_type& handheld_dest,
                                                     wxArrayString& install_fullnames )
{
    wxString    destination_fullname;
    wxString    destination_path;
    wxString    destination_rootpath;
    wxString    destination_basename;
    wxString    destination_extension;
    wxString    registry_key_name;
    int         user_index;
    wxString    install_fullname;
    bool        install_fullname_exists = false;
    bool        update_registry         = false;
    bool        successful              = false;

    wxLogDebug( wxT( "Entering palm_installer::install_files_by_handheld_dest function" ) );

    // If wxArrayString is empty, then abort
    if ( install_fullnames.IsEmpty() ) 
    {
        wxLogDebug( wxT( "Error: no filenames sent to install_files_by_handheld_dest. Aborting..." ) );
        return false;
    }

    // Look up the user_index of this user_name
    user_index = get_user_index_from_user_name( handheld_dest.user_name );

    if ( get_number_of_users() > 0 ) 
    {
        destination_rootpath = get_palm_desktop_path() + wxT( "/" )
                               + get_user_subdirectory( user_index );

        // If installing to card, use that path...
        switch (  handheld_dest.handheld_target_storage_mode )
        {
            case plkrHANDHELD_TARGET_STORAGE_MODE_SD_CARD:
                destination_path = destination_rootpath + wxT( "/CardInst/Slot-SD" );
                break;
            case plkrHANDHELD_TARGET_STORAGE_MODE_MEMORY_STICK:
                destination_path = destination_rootpath + wxT( "/CardInst/Slot-MemoryStick" );
                break;
            case plkrHANDHELD_TARGET_STORAGE_MODE_COMPACT_FLASH:
                destination_path = destination_rootpath + wxT( "/CardInst/Slot-CompactFlash" );
                break;
            // default is the normal RAM one.
            default:
                destination_path = destination_rootpath + wxT( "/Install" );
                break;
        }

        // Check the destination rootpath exists. If not, then abort the whole function
        wxLogDebug( wxT( "destination_rootpath=" ) + destination_rootpath );
        if ( ! wxDirExists( destination_rootpath ) ) 
        {
            wxLogDebug( wxT( "Error: destination_rootpath " ) + destination_rootpath
                       + wxT( " doesn't exist" ) );
            return FALSE;
        }

        // If this is a brand new user, won't have an "Install" subdirectory
        // in his/her directory yet, so create it, if not there yet.
        if ( ! wxDirExists( destination_path ) )
        {
            wxLogDebug( wxT( "destination_path not exist, so making..." ) );
            // Of course, the wxMkdir won't work recursively, so if the destination
            // is a card, and the dir doesn't exist, then have to make the 'CardInst' dir
            // first, before either the Slot-SD or Slot-MemoryStick directory:
            if ( ! handheld_dest.handheld_target_storage_mode == plkrHANDHELD_TARGET_STORAGE_MODE_RAM ) 
            {
                // Can't make the CardInst dir if it already exists
                if ( ! wxDirExists( destination_rootpath  + wxT( "/CardInst" ) ) )
                {
                    wxMkdir( destination_rootpath  + wxT( "/CardInst" ), 0777 );
                }
            }
            // OK: can now make the "SD-Card", "SD-MemoryStick" dir if card (or the "Install"
            // dir if not card).
            wxMkdir( destination_path, 0777 );
        }

        for ( size_t n = 0; n < install_fullnames.GetCount(); n++ ) 
        {
            wxLogDebug( wxT( "Starting loop to install" ) );
            install_fullname = install_fullnames.Item( n );
            wxLogDebug( wxT( "install_fullname=" ) + install_fullname );
            install_fullname_exists = wxFileExists( install_fullname );

            if ( install_fullname_exists ) 
            {
                // Get the basename and extension, so it can have the same basename + extension
                // in the install directory.
                wxSplitPath( install_fullname.c_str(), NULL, &destination_basename,
                             &destination_extension );

                destination_fullname = destination_path + wxT( "/" ) + destination_basename 
                                       + wxT( "." ) + destination_extension;

                // Copy the file over. If successful, then set the update registry flag to
                // true if it isn't already (done this way, so that even if one fails later
                // on, still turn on the registry flag for the ones that were successful).
                if ( wxCopyFile( install_fullname, destination_fullname, TRUE ) )
                {
                    if ( update_registry == FALSE )
                    {
                        update_registry = TRUE;
                    }
                }

            } 
            else 
            {
                wxLogDebug( wxT( "Error: install_fullname " ) + install_fullname + wxT( " doesn't exist" ) );
            }

        }
    }

    // If at least one file was copied over succesfully, then update_registry variable
    // was marked true. Therefore, set the switch in registry telling that there is a
    // file in the person's install directory.
    if ( update_registry ) 
    {
        wxConfigBase* previous_configuration = NULL;
        previous_configuration = wxConfigBase::Set( m_palm_user_config );

        // Create a string for the flag to tell that there is files to install
        // for this user. For example to install to magicnumber user 23145 would
        // "Install23145 = 1" to install to RAM, and "Install23145 = 5" to install
        // to SD-Card. Recall that the magic number was a wxUint32
        // (ie an unsigned long), and for some decorticate reason, Palm writes 
        // them in registry as a signed long, so that people can have negative 
        // user magic numbers. For example, instead of an unsigned long of 
        // Install3138288777 they write Install-1156678519.
        registry_key_name.Printf( wxT("Install%ld"), get_user_magic( user_index ) );

        // Our flag will either be a '1' for RAM, or a '5' for SD-Card
        long registry_key_value;
        if ( handheld_dest.handheld_target_storage_mode ) 
        {
            registry_key_value = 5;
        } 
        else 
        {
            registry_key_value = 1;
        }

        if ( ! m_palm_user_config->Write( wxT( "/HotSync Manager/" ) + registry_key_name,
                                          registry_key_value ) ) 
        {
            wxLogError( wxT( "Error: unable to write update flag to Registry" ) );
        }

        // Return to whatever wxConfig oject was running before (NULL is OK).
        wxConfigBase::Set( previous_configuration );
        successful = true;
    }

    return successful;
}

//----------------------------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------------------------

void palm_installer::load_user_data()
{
    wxUint8     string_length;
    wxUint8     count;
    bool        found_user;
    wxUint16    empty_spaces;
    wxUint8     unused;
    wxUint8     single_character;

    wxLogDebug( wxT( "Attempting to load filestream" ) );

    // An unusual file path case in wxWindows, since using the backslashes
    // of the registry.
    wxString dat_fullname = m_palm_desktop_path + wxT( "\\users.dat" );
    wxLogDebug( wxT( "Queried m_palm_desktop_path from registry. It is=" ) + dat_fullname );

    // Make sure the file exists
    if ( ! wxFileExists( dat_fullname ) ) 
    {
        wxLogError( wxT( "Palm users.dat file unable to be located at " ) + dat_fullname );
    }

    // Create the object
    wxFile dat_file( dat_fullname );

    // Make sure that file was opened properly
    if ( ! dat_file.IsOpened() ) 
    {
        wxLogError( wxT( "Error: unable to load file named " ) + dat_fullname );
    }

    // Build an input stream from the file. Note we could have skipped right to
    // this function with the wxFileInputStream( wxString filename ) function, but
    // then it is harder to see what went wrong.
    wxFileInputStream userData( dat_file );
    wxLogDebug( wxT( "Loaded filestream" ) );

    // Seek to 0.
    userData.SeekI( 0 );
    wxLogDebug( wxT( "SeekI to offset of 0" ) );

    // Make a buffered input stream from file stream
    wxBufferedInputStream buf_input( userData );
    wxLogDebug( wxT( "Loaded Bufferedinput stream from filestream" ) );

    // Make a data input stream from the buffered input stream
    wxDataInputStream data_input( buf_input );
    wxLogDebug( wxT( "Loaded DataInputStream from buffered filestream" ) );

    // Read the number of users from the dat file.
    m_number_of_users = data_input.Read8();
    wxLogDebug( wxT("Read m_number_of_users=%d"), (int)m_number_of_users );

    if ( 0 < m_number_of_users ) 
    {
        // Fast forward through 12 unneeded bytes
        empty_spaces = 12;
        for ( int i = 0; i < empty_spaces; i++ )
        {
            unused = data_input.Read8();
        }

        // Execute a loop enough times to query information about each user.
        for ( count = 0; count < m_number_of_users; count++ ) 
        {
            // Add a new member to our array of users.
            // The "new" we create here, gets "delete"ed in the base class's destructor
            // as part of the WX_CLEAR_ARRAY macro
            palm_user_type* palm_user    = new palm_user_type();
            m_users_array.Add( palm_user );
            wxLogDebug( wxT( "Added a new array element" ) );

            // Reset value of string_length to 0, from last iteration of loop, as
            // a precaution.
            string_length = 0;

            // Read the user's magic number. It is 4 bytes long (a wxUint32).
            // On a Palm install, only the first 2 bytes are used, second 2 bytes are zeros.
            // Handspring however, uses the full 4 bytes.
            //! \test Check Endianness
            m_users_array[ count ]->magic = data_input.Read32();
            wxLogDebug( wxT( "Read magic Number=%lu" ), m_users_array[ count ]->magic );

            // Read the user's Name
            // ReadString() won't work because the ReadString expects a Uint32 to tell how
            // long the string is, and users.dat only has a Uint8 :(. So can't do
            // m_users_array[ count ]->name = data_input.ReadString();
            // Could either derive a subclass of wxInputDataStream to do a ReadString()
            // that looks for a Uint8 at the start for length, or loop chars like this:
            string_length = data_input.Read8();

            for ( int j = 0; j < string_length; j++ ) 
            {
                single_character = data_input.Read8();
                m_users_array[ count ]->name += (wxChar)single_character;
            }
            wxLogDebug( wxT( "Read user_name=%s" ), m_users_array[ count ]->name.c_str() );

            // Set value of m_last_user_index if it is this name is same as the Lastuser_name
            // key in the MSW registry.
            if ( m_last_user_name == m_users_array[ count ]->name ) 
            {
                m_last_user_index = count;
            }

            // Read the user's subdirectory
            string_length = data_input.Read8();

            for ( int k = 0; k < string_length; k++ ) 
            {
                single_character = data_input.Read8();
                m_users_array[ count ]->subdirectory += (wxChar)single_character;
            }
            wxLogDebug( wxT( "Read user subdirectory=%s" ),
                        m_users_array[ count ]->subdirectory.c_str() );

            single_character = data_input.Read8();
            wxLogDebug( wxT("Read single_character=%d"), (int)single_character );

            // Zip through rest of crap until find the next user entry
            if ( count < m_number_of_users - 1 ) 
            {
                wxLogDebug( wxT( "Entering search for next user..." ) );
                found_user = FALSE;
                // I believe a user entry is at least the sequence 0x01 0x80
                // ( 1 128 ).
                //! \todo Confirm this, see if should be even longer, such as
                // to 0x00 0x01 0x80 to be even safer.
                while ( ! found_user ) 
                {                 
                    // Read the next character
                    single_character = data_input.Read8();                    
                    // If the single character is 1....  
                    if ( single_character == 1 ) 
                    {
                        // Read the next character
                        single_character = data_input.Read8();
                        // ...and it is followed by a 128...
                        if ( single_character == 128 )
                        {
                            wxLogDebug( wxT( "Found next user. Restarting loop to query info" ) );
                            // ...Got a user. break.   
                            found_user = TRUE;                       
                        }     
                    }           
                }               
            }
        }
    }
}

//----------------------------------------------------------------------------------------
// End OS removal condition
//----------------------------------------------------------------------------------------
    
#endif  // __WXMSW__
