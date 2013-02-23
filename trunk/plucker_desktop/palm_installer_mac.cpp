//----------------------------------------------------------------------------------------
// Name:        palm_installer_mac.cpp
// Purpose:     Describes palm_installer class on Macintosh (OSX)
// Author:      Robert O'Connor
// Modified by:
// Created:     2003/05/24
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: palm_installer_mac.cpp,v 1.15 2007/06/29 00:58:54 robertoconnor Exp $
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
#include "wx/dir.h"							// wxDir

// ---------------------------------------------------------------------------------------

#include "configuration.h"                  // Since looking up "show again" checkbox value
#if ( setupUSE_CHECKBOX_MESSAGE_DIALOG ) 
    #include "checkbox_message_dialog.h"    // A MessageDialog with a "don't show again" option
#endif

//----------------------------------------------------------------------------------------
// single_directory_dirtraverser Class for transversing to find translated subdirectory
//----------------------------------------------------------------------------------------

class directory_dirtraverser : public wxDirTraverser
{

public: 

    directory_dirtraverser( wxArrayString& subdirectories, 
                            const wxString& filter ) :
    m_subdirectories(subdirectories), 
    m_filter(filter)
    {}
        

    virtual wxDirTraverseResult OnFile( const wxString& filename )
    {
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir( const wxString& directory_name )
    {
        if ( directory_name.Contains( m_filter ) ) 
        {
            m_subdirectories.Add( directory_name );
        }
        // Using wxDIR_STOP not wxDIR_CONTINUE, since only want one directory.
        return wxDIR_CONTINUE;
    }

private:

    wxArrayString& 	m_subdirectories;
    wxString	    m_filter;

};

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
    m_palm_desktop_path = wxGetHomeDir() + wxT( "/Documents/Palm/Users" );
    wxLogDebug( wxT( "Directory to look for Palm Users dat file is" ) + m_palm_desktop_path );
    
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
    
    // This is a dummy to induce a translatable string, when we want the original to stay as untranslated.
    wxString 	dummy1 = _( "SecureDigital (SD) Card" );

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
             destination_path = get_translated_handheld_destination_path( destination_rootpath,
                                                                          wxT( "SecureDigital (SD) Card" ),
                                                                          wxT( "SecureDigital" )
                                                                        );
                break;
            case plkrHANDHELD_TARGET_STORAGE_MODE_MEMORY_STICK:
                destination_path = destination_rootpath + wxT( "/Files to Install/Memory Stick" );
                break;
            case plkrHANDHELD_TARGET_STORAGE_MODE_COMPACT_FLASH:
                destination_path = destination_rootpath + wxT( "/Files to Install/Compact Flash Card" );
                break;
            // default is the normal RAM one.
            default:
                destination_path = destination_rootpath + wxT( "/Files to Install" );
                break;
        }

        // Check the destination rootpath exists. It may not, if it was a newly created
        // user, it may not have its directory yet
        wxLogDebug( wxT( "destination_rootpath=" ) + destination_rootpath );

        // If this is a brand new user, won't have an "Install" subdirectory
        // in his/her directory yet, so create it, if not there yet.
        if ( ! wxDirExists( destination_path ) ) 
        {
            wxLogDebug( wxT( "destination_path not exist, so making..." ) );

            // If the "Files to Install" doesn't exist, then need to make that, before can
            // make a dir such as "Memory Stick" inside of that.
            if ( ! wxDirExists ( destination_rootpath + wxT( "/Files to Install" ) ) ) 
            {
            	wxMkdir( destination_rootpath + wxT( "/Files to Install" ), 0777 );
            }

            // OK: can now make the SecureDigital (SD) Card or Memory Stick dir 
            // if destination is a card (or the "Files to Install" dir if not card).
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
                                       + wxT( '.' ) + destination_extension;

                // Copy the file over.
                if ( wxCopyFile( install_fullname, destination_fullname, TRUE ) ) 
                {
                    successful = TRUE;
                } 
                else 
                {
                    wxLogError( wxT( "Error: couldn't copy " ) + install_fullname + wxT( " to " ) + destination_fullname );
                }

            }
            else 
            {
                wxLogDebug( wxT( "Error: install_fullname " ) + install_fullname + wxT( " doesn't exist" ) );
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
// Because of the variable number of junk after the username string, I am just extracting
// the usernames, not the magic numbers or the subdir. The magic numbers aren't used,
// and we can deduce the subdir's just by replacing colons with dashes [colons get 
// replaced since that is the directory divider on a mac including OSX].
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
//                                          Username string on OSX has max of 20 letters.
// 00 00 00 00  00 00 00 00  00 00 00 00    A variable number of bytes. How annoying. I don't know
//                                          what the start/stop signal is here for how long this junk is.
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
    wxUint8     unused;
    wxUint8     single_character;
    wxString    name_with_colons_replaced;

    wxLogDebug( wxT( "Attempting to load filestream" ) );

    wxString dat_fullname = get_palm_desktop_path() + wxT( "/Palm Users" );
    wxLogDebug( wxT( "Looking for Palm Users dat file=" ) + dat_fullname );

    // Make sure the file exists
    if ( ! wxFileExists( dat_fullname ) ) 
    {
        // Note: a checkbox message dialog, casuses a crash when opening config dialog
        // if it was active. Therefore, no checkbox message dialog           
        // Abort now. Can't read configuration from Registry.
        return;
    }

    // Create the object
    wxFile dat_file( dat_fullname );

    // Make sure that file was opened properly. I am not going to make a checkbox removable 
    // dialog since there is almost zero chance that it won't open.
    if ( ! dat_file.IsOpened() ) 
    {
        wxLogError( wxT( "Error: unable to load file named " ) + dat_fullname );
        // Need to return here or else will get in trouble later.
        return;
    }

    // Build an input stream from the file. Note we could have skipped right to
    // this function with the wxFileInputStream( wxString filename ) function, but
    // then it is harder to see what went wrong.
    wxFileInputStream userData( dat_file );
    wxLogDebug( wxT( "Loaded filestream" ) );

    // Seek to 7, which will be number of users. 
    //! \todo Should this be reading 4,5,6,7 instead? if more than 128 users?
    userData.SeekI( 7 );
    wxLogDebug( wxT( "SeekI to offset of 7" ) );

    // Make a buffered input stream from file stream
    wxBufferedInputStream buf_input( userData );
    wxLogDebug( wxT( "Loaded Bufferedinput stream from filestream" ) );

    // Make a data input stream from the buffered input stream
    wxDataInputStream data_input( buf_input );
    wxLogDebug( wxT( "Loaded DataInputStream from buffered filestream" ) );

    // Set the Endian Order of the data stream. We want to read "0x00 0x00 0x00 0E" as 
    // 14, not 234881024.
    data_input.BigEndianOrdered( TRUE );
    wxLogDebug( wxT( "NOTE! Setting to Big Endian ordered data file for Palm Users file." ) );

    // //! \todo See the 4,5,6,7 note above.
    // Read the number of users from the dat file.
    m_number_of_users = data_input.Read8();
    wxLogDebug( wxT("Read m_number_of_users=%d"), (int)m_number_of_users );

    if ( 0 < m_number_of_users ) 
    {
        // Fast forward through 4 unneeded bytes by reading a UInt32 
        // This is since I don't care if NUser (user) or a CUser (profile).
        unused = data_input.Read32();

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

            // Read the user's Name
            // ReadString() might work because we have the correct Uint32 to tell how
            // long the string is. So could instead just do
            // m_users_array[ count ]->name = data_input.ReadString();
            // However, I don't know how well it behaves with unicode.
            string_length = data_input.Read32();
            wxLogDebug( wxT( "Read string_length=%u" ), string_length );

            for ( int j = 0; j < string_length; j++ ) 
            {
                single_character = data_input.Read8();
                m_users_array[ count ]->name += (wxChar)single_character;
            }
            wxLogDebug( wxT( "Read user_name=%s" ), m_users_array[ count ]->name.c_str() );

            // Make a dummy magic number of 0
            m_users_array[ count ]->magic = 0;
            wxLogDebug( wxT("On OSX, just using a dummy magic number of %lu"), m_users_array[ count ]->magic );

            // Read the user's subdirectory (I am assuming that all 4 bytes are for the 
            // string length)

            name_with_colons_replaced = m_users_array[ count ]->name;
            name_with_colons_replaced.Replace( wxT( ":" ), wxT( "-" ) );
            m_users_array[ count ]->subdirectory = name_with_colons_replaced;
                
            wxLogDebug( wxT( "user subdirectory calculated from username as=%s" ),
                        m_users_array[ count ]->subdirectory.c_str() );

            // Zip through rest of crap until find the next user entry
            if ( count < m_number_of_users - 1 ) 
            {
                wxLogDebug( wxT( "Entering search for next user..." ) );
                found_user = FALSE;
                // A signal for the next user/profile entry is either the sequence:
                // 0x4E 0x55 0x73 0x72 (78 85 115 114 which spells NUsr), or
				// 0x43 0x55 0x73 0x72 (67 85 115 114 which spells CUsr)
                while ( ! found_user ) 
                {                 
                    // Read the next character
                    single_character = data_input.Read8();                    
                    // If the single character is 78 or 67....  
                    if ( ( single_character == 78 ) || ( single_character == 67 ) ) 
                    {
                        // Read the next character
                        single_character = data_input.Read8();
                        // ...and it is followed by a 85...
                        if ( single_character == 85 ) 
                        {
                            // Read the next character
                            single_character = data_input.Read8();
                            // ...and it is followed by a 85...
                            if ( single_character == 115 )
                            {
                            	// Read the next character
                                single_character = data_input.Read8();
                                // ...and it is followed by a 85...
                                if ( single_character == 114 ) 
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
    }
}


wxString palm_installer::get_translated_handheld_destination_path( const wxString& input_destination_rootpath,
                                                                   const wxString& input_card_subdirectory,
                                                                   const wxString& input_card_subdirectory_guess_filter )
{
    wxString        output_path;
    wxArrayString   found_subdirectories_array;            
    size_t			i;
	wxString		translated_input_card_subdirectory;
	
	translated_input_card_subdirectory = input_card_subdirectory;
    
    // Make a dir transverser.
    directory_dirtraverser a_directory_dirtraverser( found_subdirectories_array,
                                                     input_card_subdirectory_guess_filter );
    
    // Make a wxDir object and traverse it. Note that the full paths are the ones that are loaded into the array.
    wxDir directory_to_search( input_destination_rootpath );
    directory_to_search.Traverse( a_directory_dirtraverser );
    
    // Try the translated card directory first, but only if the translation doesn't match the 
    // untranslated one
    if ( input_card_subdirectory != translated_input_card_subdirectory ) 
    {  
        i = found_subdirectories_array.Index( input_destination_rootpath + wxT( "/Files to Install/" ) + translated_input_card_subdirectory );
        if ( i != wxNOT_FOUND )
        {
            output_path = found_subdirectories_array.Item( i );
        }
    }
    
    // Then try the guess. Note that whichever is the last one in the dir will be the winner      
    if ( output_path == wxEmptyString ) 
    {
        for ( i=0; i < found_subdirectories_array.GetCount(); i++ ) 
        {
            wxLogDebug( wxT( "subdir is=" ) +  found_subdirectories_array.Item( i ) );
            if ( found_subdirectories_array.Item( i ).Contains( input_card_subdirectory_guess_filter  ) ) 
            { 
                output_path = found_subdirectories_array.Item( i );
            }
        }
    }
    
    // Then try the English one. Note the lack of _() around the input_card_subdirectory.
    if ( output_path == wxEmptyString ) 
    {
        i = found_subdirectories_array.Index( input_destination_rootpath + wxT( "/Files to Install/" ) + input_card_subdirectory );
        if ( i != wxNOT_FOUND ) 
        {
            output_path = found_subdirectories_array.Item( i );
        }
    }
    
    // If still none, then just create one, in a translated name (which will be English if is no translation)
    if ( output_path == wxEmptyString ) 
    {
        output_path = input_destination_rootpath + wxT( "/Files to Install/" ) + input_card_subdirectory;
    }
    
    // Free memory
    found_subdirectories_array.Clear();
    
    return output_path;
    
}

//----------------------------------------------------------------------------------------
// End OS removal condition
//----------------------------------------------------------------------------------------
    
#endif  // __WXMAC__
