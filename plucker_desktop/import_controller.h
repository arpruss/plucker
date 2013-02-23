//----------------------------------------------------------------------------------------
/*!
    \file       import_controller.h
    \modified                            
    \copyright  Robert O'Connor
    \licence    GPL
    \brief      Describes import_controller class
    \author     Robert O'Connor
    \date       2003/10/20
 */  
// RCS-ID:      $Id: import_controller.h,v 1.2 2003/03/23 17:21:48 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _IMPORT_CONTROLLER_H_
#define _IMPORT_CONTROLLER_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "import_controller.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_IMPORT_CONTROLLER )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/html/helpctrl.h"
#include "wx/confbase.h"
#include "wx/fileconf.h"

//----------------------------------------------------------------------------------------
// Class option flags
//----------------------------------------------------------------------------------------

enum {
    optionIMPORT_CONTROLLER_DELETE_IMPORT_SOURCE_FILE_AFTER_IMPORTED    = 1,
    optionIMPORT_CONTROLLER_ONLY_IMPORT_AFTER_INSTALL_OR_PATCH          = 2
};

//----------------------------------------------------------------------------------------
// Class definition: import_controller
//----------------------------------------------------------------------------------------

//! A class to manage importation of information into plucker.ini/.pluckerrc
class import_controller
{

    // I guess one doesn't use DECLARE_DYNAMIC_CLASS on these.

public:

    //! Constructor.
    import_controller();

    //! Destructor.
    ~import_controller();

    //! Gets the global object or creates one if none exists.
    static import_controller* get();

    //! Sets the global object and returns a pointer to previous one (may be NULL).
    /*!
        \param desired_import_controller The new import_controller.
     */
    static import_controller* set( import_controller* desired_import_controller );

    /*
    //! Checks whether a plkrdata creation datetime is different from configuration key value.

        \param plkrdata_fullname Fullname of the plkrdata file.
        \param configuration_key Key in configuration file that stores the creation date.
        \return TRUE if different.

    bool is_plkrdata_file_creation_datetime_different_from_configuration_key( const wxString& plkrdata_fullname,
                                                                              const wxString& configuration_key
                                                                            );


    //! Writes a plkrdata creation datetime to a configuration key value.

        \param plkrdata_fullname Fullname of the plkrdata file.
        \param configuration_key Key in configuration file that stores the creation date.


    void write_plkrdata_file_creation_datetime_to_configuration_key( const wxString& fullname,
                                                                     const wxString& configuration_key
                                                                   );

    */
    //! Imports the plkrdata files from a new/updated installation
    /*!
        These plkrdat files are located in the
        plucker_desktop/resource/plkrdata_installation directory. They include
        things like preconfigured channels and settings.
        \param options Bitlist of optionIMPORT_CONTROLLER flags
     */
    void import_installation_plkrdata_files( long options );

    //! Imports plkrdata files from a specified directory
    /*!
        \param import_path The path where the plkrdata files are. Does not
            recurse down into and subdirectories.
        \param options Bitlist of optionIMPORT_CONTROLLER flags.
     */
    void import_plkrdata_files_from_directory( const wxString& import_path,
                                               long options
                                             );

    //! Imports sections from a .plkrdata file
    /*!
        \param plkrdata_fullname The fullname of the plkrdata file.
        \param options Bitlist of optionIMPORT_CONTROLLER flags.
     */
    void import_all_sections_from_plkrdata_file( const wxString& plkrdata_fullname,
                                                 long options
                                               );

private:

    // Singleton instance:
    static import_controller* ms_instance;

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_IMPORT_CONTROLLER

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_IMPORT_CONTROLLER_H_
