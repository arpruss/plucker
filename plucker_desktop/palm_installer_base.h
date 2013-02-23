//----------------------------------------------------------------------------------------
/*!
    \file       palm_installer_base.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes palm_installer_base class    
    \author     Robert O'Connor
    \date       2002/10/20    
 */  
// RCS-ID:      $Id: palm_installer_base.h,v 1.13 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PALM_INSTALLER_BASE_H_
#define _PALM_INSTALLER_BASE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "palm_installer_base.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/object.h"
#include "wx/string.h" // Need to include strings and things, else GCC rightly stops.
#include "handheld_dest_type.h"

//----------------------------------------------------------------------------------------
// Struct definition: palm_user_type
//----------------------------------------------------------------------------------------

//! A structure to describe a palm user
/*! 
    On MSW/MAC, these are in the users.dat file in the system's Palm directory
 */
struct palm_user_type {
    //! User's magic number, so the user can identified, even if change screen names.
    wxUint32 magic;
    //! User's name (31 characters max)
    wxString name;
    //! User's subdirectory (31 characters max)
    wxString subdirectory;
};

// ----------------------------------------------------------------------------------------
// Define an array type for our class (an array of pointers only). Note that 
// WX_DEFINE_ARRAY is only for ints/pointers, not objects. 
// This WX macro declares a new array type, called palm_user_array_type,
// which consists of a number of palm_user_type items.
// ----------------------------------------------------------------------------------------

WX_DEFINE_ARRAY( palm_user_type*, palm_user_array_type );

//----------------------------------------------------------------------------------------
// Class definition: palm_installer_base
//----------------------------------------------------------------------------------------

//! Abstract class to query Palm users and install files to a Palm device.
/*!
    A modular non-visual class, describing API functions for the actions to query 
    palm users and install files to their directories, ready to sync.
    
    The OS-specific functions are laid out in a derived palm_installer class.
 */
class palm_installer_base : public wxObject
{

    // wxMacro to declare an abstract class
    DECLARE_ABSTRACT_CLASS( palm_installer_base );

public:
    
    //! Constructor.
    palm_installer_base();
    
    //! Destructor.
    virtual ~palm_installer_base();
    
    //! Get the index number (in our m_users_array) of the last palm user.
    /*!
        \return The index of the last user in the array
     */
    int             get_last_user_index();
    
    //! Get the number of palm users on the machine.
    /*!
        \return Number of palm users.
     */
    int             get_number_of_users();
    
    //! Get the system path to the palm desktop directory
    /*!
        Example C:\Palm\
        \return Path to palm desktop directory.
     */
    wxString        get_palm_desktop_path();
    
    //! Get a palm user's magic number
    /*!
        \param user_index The palm user's index in our m_users_array .
        \return their magic number
     */
    wxUint32        get_user_magic( int user_index );

    //! Get the palm user's name from the index
    /*!
        \param user_index The palm user's index in our m_users_array .
        \return The user's name .
     */
    wxString        get_user_name( int user_index );

    //! Return a string of all palm user names.
    /*!
        \param user_index An array in which to store all the palm user names.
        \return The user's name .
     */
    void            get_user_name_array( wxArrayString& user_name_array );

    //! Return a palm user's subdirectory.
    /*!
        This is only the subdirectory off the OS's Palm directory. To get the fullname
        of the directory, need get_palm_desktop_path() then a directory separator,
        then the value returned by this function.
        \param user_index The palm user's index in our m_users_array .
        \return The user's sudirectory .
     */
    wxString        get_user_subdirectory( int user_index );

    //! Get the palm user's index in our m_users_array, based on their name.
    /*!
        \param user_name The user's name .
        \return user_index The palm user's index in our m_users_array .
     */
    int             get_user_index_from_user_name( const wxString& user_name );

    //! Install a set of files to a certain directory.
    /*!
        Useful to have, as it makes it simpler to support handheld installs other than
        Palm. Most of the smaller share devices are smart enough to look in a certain
        directory for things to sync.
        \param output_directory The output directory to send the files to be installed.
        \param install_fullnames Array of fullnames of the files to be installed.
        \return TRUE if installation was successful .
     */
    bool            install_files_by_directory( const wxString& output_directory,
                                                wxArrayString& install_fullnames
                                              );
    //! Install files to a certain directory.
    /*!
        Useful to have, as it makes it simpler to support handheld installs other than
        Palm. Most of the smaller share devices are smart enough to look in a certain
        directory for things to sync.
        \param output_directory The output directory to send the files to be installed.
        \param install_fullname Fullname of the file to be installed.
        \return TRUE if installation was successful .
    */
    bool            install_file_by_directory( const wxString& output_directory,
                                              const wxString& install_fullname );

    //! Install files via the handheld_dest_type interface
    /*!
        \param handheld_dest The handheld_dest_type where the files should be installed.
        \param install_fullnames An array of fullnames of files to be installed.

        This needs to be in derived classes, not base, even though shared.
        If in base, crashes (wx231).
     */
    virtual bool    install_files_by_handheld_dest( handheld_dest_type& handheld_dest,
                                                    wxArrayString& install_fullnames
                                                  ) = 0;

    //! Install a single file via the handheld_dest_type interface
    /*!
        \param handheld_dest The handheld_dest_type where the files should be installed.
        \param install_fullnames The fullnamem of the file to be installed.

        This needs to be in derived classes, not base, even though shared.
        If in base, crashes (wx231).
     */
    virtual bool    install_file_by_handheld_dest( handheld_dest_type& handheld_dest,
                                                   const wxString& install_fullname
                                                  ) = 0;

protected:

    //! Number of Palm users on the machine.
    wxUint8              m_number_of_users;

    //! Index of the last user in our m_users_array.
    wxUint16             m_last_user_index;

    //! The path to the Palm directory (255 characters max).
    wxString             m_palm_desktop_path;

    //! The name of our last user in our m_users_array (31 characters max).
    wxString             m_last_user_name;             
    
    //! An array of palm_user_type objects, describing palm users on the machine.
    /*! 
        \todo GURU: A * or not on this sucker? This might be the memory leak of strings. 
        This is what exec sample uses though, for its processes.
     */
    
    palm_user_array_type m_users_array;
    
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_PALM_INSTALLER_BASE_H_

