//----------------------------------------------------------------------------------------
// Name:        palm_installer_base.cpp
// Purpose:     The actions that can be performed on a channel: add, delete, update
//              Add/delete does and add/delete of both the section of .ini holding the
//              channel's settings and its subdir within the channel directory.
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: palm_installer_base.cpp,v 1.14 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "palm_installer_base.h"
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

#include "palm_installer_base.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/file.h"                    // wxFile

//----------------------------------------------------------------------------------------

#include "utils_string.h"

//----------------------------------------------------------------------------------------
// wxWindows macro: implement abstract class
//----------------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS( palm_installer_base, wxObject )

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

// Constructor
palm_installer_base::palm_installer_base()
{ 
}

// Destructor
palm_installer_base::~palm_installer_base()
{
    // Clear out the array of users, or will see a memory leak for each member
    wxLogDebug( wxT( "Entering palm_installer_base() destructor. About to clear user array" ) );    
    // The WX_CLEAR_ARRAY(array) macro [see dynarray.h] is required to clear out all the 
    // elements from memory of an array of structs that was built with WX_DEFINE_ARRAY.
    // All that this macro does is just to loop through the array, "delete" each element
    // to free it from memory, then mark the array as Empty().
    WX_CLEAR_ARRAY( m_users_array );
    wxLogDebug( wxT( "Successfully cleared user_array." ) );
}

int palm_installer_base::get_number_of_users()
{
    return (int)m_number_of_users;
}


int palm_installer_base::get_last_user_index()
{
    return (int)m_last_user_index;
}


wxString palm_installer_base::get_palm_desktop_path()
{
    return m_palm_desktop_path;
}


wxUint32 palm_installer_base::get_user_magic( int user_index )
{
    if ( ( ! m_users_array.IsEmpty () ) && ( user_index >= 0 ) && 
         ( user_index < m_number_of_users ) ) 
    {
        return m_users_array[ user_index ]->magic;
    } 
    else 
    {
        return 0;
    }
}


wxString palm_installer_base::get_user_name( int user_index )
{
    if ( ( ! m_users_array.IsEmpty() ) && ( 0 <= user_index ) && 
         ( user_index < m_number_of_users ) ) 
    {
        wxLogDebug( wxT( "Gotuser_name as" ) + m_users_array[ user_index ]->name );
        return m_users_array[ user_index ]->name;
    }
    else 
    {
        return wxEmptyString;
    }
}


void palm_installer_base::get_user_name_array( wxArrayString& user_name_array)
{    
    if ( get_number_of_users() != 0 )
    {
        for ( int count = 0; count < get_number_of_users(); count++ ) 
        {
            user_name_array.Add( get_user_name( count ) );           
        }
    }
}


wxString palm_installer_base::get_user_subdirectory( int user_index )
{
    if ( ( ! m_users_array.IsEmpty () ) && ( 0 <= user_index ) &&
         ( user_index < m_number_of_users ) ) 
    { 
        return m_users_array[ user_index ]->subdirectory;
    } 
    else 
    {
        return wxEmptyString;
    }
}


bool palm_installer_base::install_files_by_directory( const wxString &output_directory,
                                                      wxArrayString &install_fullnames )
{
    wxString    output_fullname;
    wxString    install_fullname;
    wxString    install_filename;
    wxString    install_basename;
    wxString    install_extension;
    bool        copy_successful     = FALSE;

    // Abort if output directory is empty, or doesn't exist
    if ( output_directory == wxT( "" ) || ! wxDirExists( output_directory ) ) 
    {
        return FALSE;
    }

    // Remove any trailing dir separators from the output directory string
    // (which can happen for example in 'C:\' returned from wxDirDialog.
    wxString stripped_output_directory = utils_string::remove_trailing_dir_separator( output_directory );

    // Loop through all the input_files copying them over to the directory
    for ( int n=0; n < (int)install_fullnames.GetCount(); n++ ) 
    {
        install_fullname = install_fullnames.Item( n );

        // Only proceed if this install_fullname exists.
        if ( wxFileExists( install_fullname ) )
        {
            // Extract the basename and extension from input_fullname, so 
            // can use it for the target files to copy.
            wxSplitPath( install_fullname.c_str(), NULL, &install_basename,
                         &install_extension );
            install_filename = install_basename + wxT( "." ) + install_extension;                
            output_fullname = stripped_output_directory + wxT( "/" ) + install_filename;
    
            // TRUE as 3rd parameter, since certainly want to overwrite it.
            copy_successful = wxCopyFile( install_fullname, output_fullname, TRUE );

            wxLogDebug( wxT( "Success=%ld in attempt to copy %s to %s" ), (long)copy_successful,
                        install_fullname.c_str(), output_fullname.c_str() );
        } 
        else
        {
            wxLogDebug( wxT( "Error: Can't find file %s to copy" ), install_fullname.c_str() );
        }
    }

    return copy_successful;
}


bool palm_installer_base::install_file_by_directory( const wxString &output_directory,
                                                     const wxString &install_fullname )
{
    bool successful;

    wxArrayString install_fullnames;

    wxString install_file_to_add = install_fullname;

    install_fullnames.Add( install_file_to_add );

    successful = install_files_by_directory( output_directory, install_fullnames );

    install_fullnames.Clear();

    return successful;
}

//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

int palm_installer_base::get_user_index_from_user_name( const wxString& user_name )
{
    int array_index = 0;
    
    wxLogDebug( wxT( "Looking up user_index for user_name=" ) + user_name );
    
    for ( int i=0; i < get_number_of_users(); i++ )
    {
        if ( user_name == m_users_array[ i ]->name ) 
        {
            wxLogDebug( wxT( "user_name=%s found at user_index=%d" ), 
                        m_users_array[ i ]->name.c_str(), i );
            array_index = i;
            break;
        }
    } 
    
    return array_index;
}
