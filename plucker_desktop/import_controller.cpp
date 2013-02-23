//----------------------------------------------------------------------------------------
// Name:        import_controller.cpp
// Purpose:     Manages importing of files
// Author:      Robert O'Connor
// Modified by:
// Created:     2003/03/11
// Copyright:   Robert O'Connor
// Licence:     GPL
// RCS-ID:      $Id: import_controller.cpp,v 1.6 2007/06/16 02:52:00 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "import_controller.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_IMPORT_CONTROLLER )

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

#include "import_controller.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/dir.h"                 // wxDir

//----------------------------------------------------------------------------------------

#include "configuration.h"
#include "utils_datetime.h"
#include "utils_string.h"

//----------------------------------------------------------------------------------------
// Internal variables
//----------------------------------------------------------------------------------------

import_controller* import_controller::ms_instance = NULL;

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

import_controller::import_controller()
{

}


import_controller::~import_controller()
{
    wxLogDebug ( wxT( "Finished import_controller destructor" ) );
}


import_controller* import_controller::get()
{
    wxLogDebug( wxT( "Entering import_controller::get()" ) );

    if ( ! ms_instance ) {
        ms_instance = new import_controller();
    }

    // Return ms_instance (which will stay as NULL if
    return ms_instance;
}


import_controller* import_controller::set( import_controller *desired_import_controller )
{
    import_controller* old_import_controller = ms_instance;
    ms_instance = desired_import_controller;
    return old_import_controller;
}





/*

bool import_controller::is_plkrdata_file_modification_datetime_different_from_configuration_key( const wxString& plkrdata_fullname,

                                                                                             const wxString& configuration_key )

{

    wxFileName  plkrdata_filename;

    wxDateTime  plkrdata_modification_datetime;

    wxDateTime  plkrdata_configuration_value_datetime;

    wxString    plkrdata_configuration_value_string;



    plkrdata_filename = plkrdata_fullname;

    plkrdata_filename.GetTimes( &NULL, &NULL, &plkrdata_modification_datetime );

    plkrdata_configuration_value_string = the_configuration->Read( configuration_key, wxT( "" ) );

    plkrdata_configuration_value_datetime = utils_datetime::w3c_string_to_datetime( plkrdata_configuration_value_string );



    if ( plkrdata_modification_datetime.IsEqualTo( plkrdata_datetime_from_configuration ) ) {

        return TRUE;

    } else {

        return FALSE;

    }

}





void import_controller::write_plkrdata_file_modification_datetime_to_configuration_key( const wxString& fullname,

                                                                                    const wxString& configuration_key )

{

    wxDateTime  plkrdata_modification_datetime;

    wxString    plkrdata_configuration_value_string;



    plkrdata_filename = plkrdata_fullname;

    plkrdata_filename.GetTimes( &NULL, &NULL, &plkrdata_modification_datetime );

    plkrdata_configuration_value_string = utils_datetime::datetime_to_w3c_string( plkrdata_modification_datetime );

    the_configuration->Write( configuration_key, plkrdata_configuration_value_string );

}

*/


void import_controller::import_installation_plkrdata_files( long options )
{
    wxLogDebug( wxT( "Entering import_controller::import_installation_plkrdata_files()" ) );

    // Get the dir with the files to set up installation:
    wxString plkrdata_installation_path = get_plucker_directory( RESOURCES )
                                          << wxT( "/" ) << wxT( "plkrdata_installation" );

    // We only want to import the plkrdata files once. On MSW we can just delete
    // the files. However, on others, the resource files are likely installed to a
    // protected directory with only root permissions, so can't delete them. So
    // instead use timestamps to see if they are new files:

#ifdef __WXMSW__
    // On MSW we should have permission to delete the files when done.
    if ( ! options & optionIMPORT_CONTROLLER_DELETE_IMPORT_SOURCE_FILE_AFTER_IMPORTED ) 
    {
        options = options + optionIMPORT_CONTROLLER_DELETE_IMPORT_SOURCE_FILE_AFTER_IMPORTED;

    }
    // On MSW we could just do this next single line, and finish the function here,
    // but might as well take the long road for MSW too, so that debugging is
    // the same on all platforms.

    //import_plkrdata_files_from_directory( plkrdata_installation_path, options );
#endif

    wxArrayString   new_plkrdata_fullname_arraystring;
    wxArrayString   new_plkrdata_datetime_arraystring;
    wxArrayString   old_plkrdata_fullname_arraystring;
    wxArrayString   old_plkrdata_datetime_arraystring;
    wxFileName      plkrdata_fullname;
    wxDateTime      plkrdata_modification_datetime;

    // Load all the files into our array. The "*.plkrdata" is the file mask,
    // and the flag WON'T recurse (add wxDIR_DIRS if you want to recurse).
    
    if ( ! wxDir::Exists( plkrdata_installation_path ) ) 
    {
        wxLogDebug( wxT( "import_controller: The plkrdata installation path could not be found" ) );
    }
    
    //! \todo Does an uppercase plkrdata still work here?
    // NOTE: The .plkrdata files must be less than 32 chars long on OS X, else the filter won't
    // work (since will be #blah filenames). If so, then * will still work.
    wxDir::GetAllFiles( plkrdata_installation_path, &new_plkrdata_fullname_arraystring,
                        wxT( "*.plkrdata" ), wxDIR_FILES | wxDIR_HIDDEN );
    
    wxLogDebug( wxT("import_controller: number of installation plkrdata files=%d"), (int)new_plkrdata_fullname_arraystring.GetCount() );

    // Loop through fullnames in array...
    for ( size_t i = 0; i < new_plkrdata_fullname_arraystring.GetCount(); i++ ) 
    {
        // Setup the wxFileName object
        plkrdata_fullname = new_plkrdata_fullname_arraystring.Item( i );
        // Get its modification datetime.
        plkrdata_modification_datetime = plkrdata_fullname.GetModificationTime();
        // Covert its modification datetime to a string and add it to
        // our array.
        new_plkrdata_datetime_arraystring.Add( utils_datetime::datetime_to_w3c_string( plkrdata_modification_datetime ) );
    }

    // Load the values from the previous time we ran Plucker Desktop
    utils_string::string_to_arraystring( the_configuration->Read( wxT( "/PLUCKER_DESKTOP/installation_plkrdata_fullnames" ), wxT( "" ) ),
                                         ';',
                                         &old_plkrdata_fullname_arraystring,
                                         TRUE
                                       );

    // Load the values from the previous time we ran Plucker Desktop
    utils_string::string_to_arraystring( the_configuration->Read( wxT( "/PLUCKER_DESKTOP/installation_plkrdata_datetimes" ), wxT( "" ) ),
                                         ';',
                                         &old_plkrdata_datetime_arraystring,
                                         TRUE
                                       );

    int  found_index_in_old;
    bool do_import;
    // Loop through the new fullnames...
    for ( size_t n = 0; n < new_plkrdata_fullname_arraystring.GetCount(); n++ )
    {
        wxLogDebug( wxT( "Entering import_controller's loop to import any new installation .plkrdata files" ) );
        
        // Reset whether should import
        do_import = FALSE;
        // Get the index of this fullname in the array of our old ones
        // The old array will be empty, if a new install, since config key
        // is empty.
        found_index_in_old = old_plkrdata_fullname_arraystring.Index( new_plkrdata_fullname_arraystring.Item( n ) );
        // If the new file isn't in the old array, then it should be imported.
        if ( found_index_in_old == wxNOT_FOUND ) 
        {
            do_import = TRUE;
        // But if the new file was in the old array...
        } 
        else 
        {
            // ...if the old file's corresponding W3C string doesn't match the
            // new file's W3C string...
            if ( old_plkrdata_datetime_arraystring.Item( found_index_in_old )
                 != new_plkrdata_datetime_arraystring.Item( n ) ) 
            {
                // ...then it should be imported
                do_import = TRUE;
            }
        }

        // If should import the file
        if ( do_import ) 
        {
                // ...then do the import file function, passing on the options.
                import_all_sections_from_plkrdata_file( new_plkrdata_fullname_arraystring.Item( n ),
                                                    options );
        }
    }

    // Write the values of the new fullname array and datetime array to the
    // config file for the next time.
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/installation_plkrdata_fullnames" ),
                              utils_string::arraystring_to_string( new_plkrdata_fullname_arraystring, ';' ) );

    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/installation_plkrdata_datetimes" ),
                              utils_string::arraystring_to_string( new_plkrdata_datetime_arraystring, ';' ) );

    // Flush to write changes to config file.
    the_configuration->Flush();

    // Clear memory
    old_plkrdata_fullname_arraystring.Clear();
    old_plkrdata_datetime_arraystring.Clear();
    new_plkrdata_fullname_arraystring.Clear();
    new_plkrdata_datetime_arraystring.Clear();
}


void import_controller::import_plkrdata_files_from_directory( const wxString& import_path,
                                                              long options
                                                            )
{
    // Error check that the dir exists.
    if ( ! wxDir::Exists( import_path ) ) 
    {
        wxLogDebug( wxT( "Error: specified plkrdata import directory doesn't exist" ) );
    }

    wxArrayString plkrdata_fullnames_array;
    // Load all the files into our array. The "*.plkrdata" is the file mask,
    // and the flag WON'T recurse (add wxDIR_DIRS if you want to recurse).
    //! \todo Does an uppercase plkrdata still work here?
    wxDir::GetAllFiles( import_path, &plkrdata_fullnames_array,
                        wxT( "*.plkrdata" ), wxDIR_FILES | wxDIR_HIDDEN );

    // Loop through filenames in array, importing each file.
    for ( size_t i = 0; i < plkrdata_fullnames_array.GetCount(); i++ ) 
    {
        // Do it
        import_all_sections_from_plkrdata_file( plkrdata_fullnames_array.Item( i ), options );
    }

    // Free memory
    plkrdata_fullnames_array.Clear();
}


void import_controller::import_all_sections_from_plkrdata_file( const wxString& plkrdata_fullname,
                                                                long options
                                                              )
{
    if ( plkrdata_fullname == wxEmptyString ) 
    {
        wxLogDebug( wxT( "Error: the plkdata file has an empty filename." ) );
        return;
    }

    if ( ! wxFile::Exists( plkrdata_fullname ) ) 
    {
        wxLogDebug( wxT( "Error: the plkrdata file %s does not exist." ), plkrdata_fullname.c_str() );
    }

    // Make the from configuration.
    wxFileConfig*  from_configuration;
    from_configuration = new wxFileConfig( wxT( "Plucker Desktop Import Configuration" ),
                                           wxT( "The Plucker Team" ),
                                           plkrdata_fullname,
                                           wxEmptyString,
                                           wxCONFIG_USE_LOCAL_FILE
                                         );

    copy_all_sections( from_configuration,
                       the_configuration,
                       optionCONFIGURATION_COPY_SECTION_TREAT_AS_ABSOLUTE |
                       optionCONFIGURATION_COPY_SECTION_FLUSH_AT_END
                      );

    // Delete from_configuration if we made one.
    if ( from_configuration ) 
    {
        wxLogDebug( wxT( "About to delete the from configuration..." ) );
        delete from_configuration;
        from_configuration = NULL;
        wxLogDebug( wxT( "Deleted from configuration." ) );
    }

    // If asked to delete the file after import, then do so:
    if ( options & optionIMPORT_CONTROLLER_DELETE_IMPORT_SOURCE_FILE_AFTER_IMPORTED ) 
    {
        ::wxRemoveFile( plkrdata_fullname );
    }
}

//----------------------------------------------------------------------------------------
// Module definition
//----------------------------------------------------------------------------------------

class module_import_controller : public wxModule
{
 
    DECLARE_DYNAMIC_CLASS( module_import_controller )

public:
    
    module_import_controller() {}

    bool OnInit()
    {
        return TRUE;
    }


    void OnExit()
    {
        wxLogDebug( wxT( "Entered module_import_controller::OnExit()" ) );
        // We set it to null, and a pointer returns to the previous one, which we delete.
        delete import_controller::set( NULL );
        wxLogDebug( wxT( "Completed module_import_controller::OnExit()" ) );
    }

};

IMPLEMENT_DYNAMIC_CLASS( module_import_controller, wxModule )

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_IMPORT_CONTROLLER
