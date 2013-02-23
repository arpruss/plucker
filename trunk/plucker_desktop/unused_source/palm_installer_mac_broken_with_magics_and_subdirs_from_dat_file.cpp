//----------------------------------------------------------------------------------------
// Name:        palm_installer_mac.cpp
// Purpose:     Describes palm_installer class on Macintosh (OSX)
// Author:      Robert O'Connor
// Modified by:
// Created:     2003/05/24
// Copyright:   (c) Robert O'Connor ( rob@medicalmnemonics.com )
// Licence:     GPL
// RCS-ID:      $Id: palm_installer_mac_broken_with_magics_and_subdirs_from_dat_file.cpp,v 1.1 2003/07/27 22:08:19 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "palm_installer_mac.h"
#endif

//----------------------------------------------------------------------------------------
// Begin OS removal condition
//----------------------------------------------------------------------------------------

#ifdef __WXMAC__

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

#include "palm_installer_mac.h"

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
    m_number_of_users = 0;
    
    // Set the m_palm_desktop_path to where it is on a MacOSX machine.
    m_palm_desktop_path = wxGetHomeDir() + "/Documents/Palm/Users";
    wxLogDebug( "Directory to look for Palm Users dat file is" + m_palm_desktop_path );
    
    // All set, so now read the "Palm Users" file in the OSX's user's home directory 
    // (this is the equivalent to users.dat equivalent on MSW). 
    load_user_data();
}


// Destructor
// Delete the configuration object when finished program, otherwise memory leak
palm_installer::~palm_installer()
{
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
    int         user_index;
    wxString    install_fullname;
    bool        install_fullname_exists = false;
    bool        successful              = false;

    wxLogDebug( wxT( "Entering palm_installer::install_files_by_handheld_dest function" ) );

    // If wxArrayString is empty, then abort
    if ( install_fullnames.IsEmpty() ) {
        wxLogDebug( "Error: no filenames sent to install_files_by_handheld_dest. Aborting..." );
        return false;
    }

    // Look up the user_index of this user_name
    user_index = get_user_index_from_user_name( handheld_dest.user_name );

    if ( get_number_of_users() > 0 ) {

        destination_rootpath = get_palm_desktop_path() + "/"
                               + get_user_subdirectory( user_index );

        // If installing to card, use that path...
        switch (  handheld_dest.handheld_target_storage_mode ) {
            case plkrHANDHELD_TARGET_STORAGE_MODE_SD_CARD:
                destination_path = destination_rootpath + "/Files to Install/SecureDigital (SD) Card";
                break;
            case plkrHANDHELD_TARGET_STORAGE_MODE_MEMORY_STICK:
                destination_path = destination_rootpath + "/Files to Install/Memory Stick";
                break;
            case plkrHANDHELD_TARGET_STORAGE_MODE_COMPACT_FLASH:
                destination_path = destination_rootpath + "/Files to Install/Compact Flash Card";
                break;
            // default is the normal RAM one.
            default:
                destination_path = destination_rootpath + "/Files to Install";
                break;
        }

        // Check the destination rootpath exists. It may not, if it was a newly created
        // user, it may not have its directory yet
        wxLogDebug( "destination_rootpath=" + destination_rootpath );
        if ( ! wxDirExists( destination_rootpath ) ) {
                wxMkdir( destination_rootpath, 0777 );
        }

        // If this is a brand new user, won't have an "Install" subdirectory
        // in his/her directory yet, so create it, if not there yet.
        if ( ! wxDirExists( destination_path ) ) {
            wxLogDebug( "destination_path not exist, so making..." );

            // OK: can now make the SecureDigital (SD) Card or Memory Stick dir 
            // if destination is a card (or the "Files to Install" dir if not card).
            
            wxMkdir( destination_path, 0777 );
        }

        for ( size_t n = 0; n < install_fullnames.GetCount(); n++ ) {

            wxLogDebug( "Starting loop to install" );
            install_fullname = install_fullnames.Item( n );
            wxLogDebug( "install_fullname=" + install_fullname );
            install_fullname_exists = wxFileExists( install_fullname );

            if ( install_fullname_exists ) {

                // Get the basename and extension, so it can have the same basename + extension
                // in the install directory.
                wxSplitPath( install_fullname.c_str(), NULL, &destination_basename,
                             &destination_extension );

                destination_fullname = destination_path + "/" + destination_basename 
                                       + '.' + destination_extension;

                // Copy the file over.
                if ( wxCopyFile( install_fullname, destination_fullname, TRUE ) ) {
                    successful = TRUE;
                } else {
                    wxLogError( "Error: couldn't copy " + install_fullname + " to " + destination_fullname );
                }

            } else {
                wxLogDebug( "Error: install_fullname " + install_fullname + " doesn't exist" );
            }

        }
    }

    return successful;
}

//----------------------------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------------------------

// The OSX Palm Users file looks like this: The <> are the numbers that change.
// Note that is read as BigEndian Ordered (see the call to 
// wxDataInputStream::BigEndianOrdered( TRUE ) in the source above).
//
// 66 66 30 32								All seem to start with these.
// 00 00 00 <02> 						    The number of Palm users (02 in this example),
//                                          I don't know if all bytes are used.
//                                          I expect so, since the magic numbers are so high.
// <4E 55 73 72> 							Either 4E 55 73 72 (which spells NUsr) or  
// 											43 55 73 72 (which spells CUsr). NUsr means a 
//											HotSync "User". CUsr means a HotSync "Profile".
//											Profiles are used for flashing many blank PDAs 
//											in a company. Users are the usual thing.
// 00 00 00 								Three empty zeros) Can't be used for username
//                                          strings yet, since limit on length
// <13> 									Number of letters in the user name string
// <The username string> 					Example isA:Colon:Is:Replaced
// 00 00 00 00  00 00 00 00  00 00 00 00    Twelve empty bytes.
// <BA ED AC 50>							A magic number for the user.
// 00 00 00 								Three empty bytes. Can't be used for directory
//                                          strings yet, since limit on length.
// <13>										Number of letters in the Palm user's subdir.
// <The Palm user's subdir>					Name of subdir for Palm user. Colons get 
//											replaced with dashes. Example is A-Colon-Is-Replaced
// 00 00 00 00  00 00 00 00  00 00 			Ten empty zeros.
// <4E 55 73 72> 							(NUsr or CUsr as above). Tells start of next user/profile.

void palm_installer::load_user_data()
{
    wxUint32    string_length;
    wxUint8     count;
    bool        found_user;
    wxUint8     empty_spaces;
    wxUint8     unused;
    wxUint8     single_character;

    wxLogDebug( "Attempting to load filestream" );

    wxString dat_fullname = get_palm_desktop_path() + "/Palm Users";
    wxLogDebug( "Looking for Palm Users dat file=" + dat_fullname );

    // Make sure the file exists
    if ( ! wxFileExists( dat_fullname ) ) {
        wxLogError( "Palm Users file unable to be located at " + dat_fullname );
    }

    // Create the object
    wxFile dat_file( dat_fullname );

    // Make sure that file was opened properly
    if ( ! dat_file.IsOpened() ) {
        wxLogError( "Error: unable to load file named " + dat_fullname );
    }

    // Build an input stream from the file. Note we could have skipped right to
    // this function with the wxFileInputStream( wxString filename ) function, but
    // then it is harder to see what went wrong.
    wxFileInputStream userData( dat_file );
    wxLogDebug( "Loaded filestream" );

    // Seek to 7, which will be number of users. 
    //! \todo Should this be reading 4,5,6,7 instead? if more than 128 users?
    userData.SeekI( 7 );
    wxLogDebug( "SeekI to offset of 7" );

    // Make a buffered input stream from file stream
    wxBufferedInputStream buf_input( userData );
    wxLogDebug( "Loaded Bufferedinput stream from filestream" );

    // Make a data input stream from the buffered input stream
    wxDataInputStream data_input( buf_input );
    wxLogDebug( "Loaded DataInputStream from buffered filestream" );

    // Set the Endian Order of the data stream. We want to read "0x00 0x00 0x00 0E" as 
    // 14, not 234881024.
    data_input.BigEndianOrdered( TRUE );
    wxLogDebug( "NOTE!! Setting to Big Endian ordered data file for Palm Users file." );

    // //! \todo See the 4,5,6,7 note above.
    // Read the number of users from the dat file.
    m_number_of_users = data_input.Read8();
    wxLogDebug( wxT("Read m_number_of_users=%d"), (int)m_number_of_users );

    if ( 0 < m_number_of_users ) {

        // Fast forward through 4 unneeded bytes by reading a UInt32 
        // This is since I don't care if NUser (user) or a CUser (profile).
        unused = data_input.Read32();

        // Execute a loop enough times to query information about each user.
        for ( count = 0; count < m_number_of_users; count++ ) {

            // Add a new member to our array of users.
            // The "new" we create here, gets "delete"ed in the base class's destructor
            // as part of the WX_CLEAR_ARRAY macro
            palm_user_type* palm_user    = new palm_user_type();
            m_users_array.Add( palm_user );
            wxLogDebug( "Added a new array element" );

            // Reset value of string_length to 0, from last iteration of loop, as
            // a precaution.
            string_length = 0;

            // Read the user's Name
            // ReadString() might work because we have the correct Uint32 to tell how
            // long the string is. So could instead just do
            // m_users_array[ count ]->name = data_input.ReadString();
            // However, I don't know how well it behaves with unicode.
            string_length = data_input.Read32();
            wxLogDebug( "Read string_length=%u", string_length );

            for ( int j = 0; j < string_length; j++ ) {
                single_character = data_input.Read8();
                m_users_array[ count ]->name += (wxChar)single_character;
            }
            wxLogDebug( wxT( "Read user_name=%s" ), m_users_array[ count ]->name.c_str() );

            // Fast forward through 12 unneeded bytes
            empty_spaces = 12;
                for ( int i = 0; i < empty_spaces; i++ ) {
                unused = data_input.Read8();
            }

            // Read the user's magic number. It is 4 bytes long (a wxUint32).
            // On a MSW Palm install, only the first 2 bytes are used, second 2 bytes are zeros.
            // On a MSW Handspring however, all 4 bytes are used.
            // On a MAC Palm install, all 4 bytes are used. 
            m_users_array[ count ]->magic = data_input.Read32();
            wxLogDebug( wxT("Read magic number=%lu"), m_users_array[ count ]->magic );

            // Read the user's subdirectory (I am assuming that all 4 bytes are for the 
            // string length)
            string_length = data_input.Read32();

            for ( int k = 0; k < string_length; k++ ) {
                single_character = data_input.Read8();
                m_users_array[ count ]->subdirectory += (wxChar)single_character;
            }
            wxLogDebug( wxT( "Read user subdirectory=%s" ),
                        m_users_array[ count ]->subdirectory.c_str() );

            single_character = data_input.Read8();
            wxLogDebug( wxT("Read single_character=%d"), (int)single_character );

            // Zip through rest of crap until find the next user entry
            if ( count < m_number_of_users - 1 ) {
                wxLogDebug( "Entering search for next user..." );
                found_user = FALSE;
                // A signal for the next user/profile entry is either the sequence:
                // 0x4E 0x55 0x73 0x72 (78 85 115 114 which spells NUsr), or
				// 0x43 0x55 0x73 0x72 (67 85 115 114 which spells CUsr)
                while ( ! found_user ) {                 
                    // Read the next character
                    single_character = data_input.Read8();                    
                    // If the single character is 78 or 67....  
                    if ( ( single_character == 78 ) || ( single_character == 67 ) ) {
                        // Read the next character
                        single_character = data_input.Read8();
                        // ...and it is followed by a 85...
                        if ( single_character == 85 ) {
                            // Read the next character
                            single_character = data_input.Read8();
                            // ...and it is followed by a 85...
                            if ( single_character == 115 ) {
                            	// Read the next character
                                single_character = data_input.Read8();
                                // ...and it is followed by a 85...
                                if ( single_character == 114 ) {
                                    wxLogDebug( "Found next user. Restarting loop to query info" );
                                    // ...Got a user. break.   
                                    found_user = TRUE;  
                                }
                            }
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
    
#endif  // __WXMAC__
