//----------------------------------------------------------------------------------------
/*!
    \file       configuration.h       
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Handles saving and loading of configuration settings    
    \author     Robert O'Connor
    \date       2001/10/20    
 */  
// RCS-ID:      $Id: configuration.h,v 1.22 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "configuration.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/confbase.h"
#include "wx/fileconf.h"

//----------------------------------------------------------------------------------------
// External functions and variables
//----------------------------------------------------------------------------------------

// Option flags
enum {
        optionCONFIGURATION_COPY_SECTION_TREAT_AS_ABSOLUTE = 1,
        optionCONFIGURATION_COPY_SECTION_FLUSH_AT_END      = 2
};

    //! \todo When wx can derive from wxFileConfig, then subclass the configuration.
    // Then Doxygenate configuration.h
    extern wxFileConfig* the_configuration;

    //! Initialize the_configuration
    void init_configuration();

    //! Finish the_configuration
    void finish_configuration();

//----------------------------------------------------------------------------------------

    //! Get a filename of the textfile read and read to by the_configuration.
    wxString get_configuration_filename();

    //! Get a proper platform-specific configuration filename for a given basename.
    wxString get_custom_configuration_filename( const wxString& basename );

    //! Get the section of the configuration file used by the active OS.
    wxString get_os_configuration_section();

    //! Check to see it the section of the configuration file is a channel section.
    bool is_channel_section( wxString section );

    //! Check to see it the section of the configuration file is a reserved section.
    bool is_reserved_section( wxString section );

    //! Check to see it the section of the configuration file is a channel modifier section.
    bool is_modifier_section( wxString section );

    //! Get a legal new channel section, based on a suggested string.
    wxString get_legal_new_channel_section( wxString string_to_convert );

//----------------------------------------------------------------------------------------

    //! Get the directory of pluckerhome.
    wxString get_pluckerhome_directory();

    //! Get directory of the plucker bundle runtime files, so can find resources.
    wxString get_bundle_runtime_files_directory();

    //! Get one of the directories used by plucker.
    wxString get_plucker_directory( long directory_name );

    //! Get the value of pluckerhome from the registry (MSW only).
    wxString get_msw_pluckerhome_from_registry();

    //! Strip deprecialted keys from a section.
    void strip_depreciated_keys( wxString section );

    //! Copy all the keys from one section to another.
    /*! Copies keys from one section to another. Creates to_section if it doesn't exist
        already. Sections are of the format DEFAULT or MYCHANNEL etc. The
        treat_as_absolute will just convert the to_section and from_section to absolute
        paths in the config file, such as /DEFAULT or /MYCHANNEL. [Set treat_as_absolute
        to FALSE if you want to do some sort of copying of sections within a subgroup]
     */
    void copy_section( wxString from_section,
                       wxString to_section,
                       bool treat_as_absolute
                     );

    //! Imports an array of sections from one config to another, not changing section name.
    void import_sections_as_section_names( wxArrayString& selected_sections,
                                       long options,
                                       wxFileConfig* from_configuration,
                                       wxFileConfig* to_configuration
                                     );

    //! A new function to replace copy_section, once works okay
    void copy_section2( const wxString& from_section = wxEmptyString,
                        const wxString& to_section = wxEmptyString,
                        long options = optionCONFIGURATION_COPY_SECTION_TREAT_AS_ABSOLUTE |
                                       optionCONFIGURATION_COPY_SECTION_FLUSH_AT_END,
                        wxFileConfig* from_configuration = the_configuration,
                        wxFileConfig* to_configuration = the_configuration
                      );

    //! Convenience function to copy all sections from one config file to another
    void copy_all_sections( wxFileConfig* from_configuration,
                            wxFileConfig* to_configuration,
                            long options = optionCONFIGURATION_COPY_SECTION_TREAT_AS_ABSOLUTE |
                                           optionCONFIGURATION_COPY_SECTION_FLUSH_AT_END
                          );

    //! Convert a protocol-prefixed filename to a regular fullname
    wxString protocol_filename_to_normal_fullname( const wxString& input_filename );

    //! Get the protocol prefix at the front of a string.
    wxString get_protocol_prefix( wxString& input );

//----------------------------------------------------------------------------------------

    //! Sets the close on exit key for the specified section
    /*!
        \param section The configuration section
        \param enabled TRUE if the close_on_exit key should be set 
        \param only_write_if_key_exists TRUE if only should write the key if it exists
        \param flush Whether to flush the configuration or not
     */
    void set_close_on_exit( const wxString& section, 
                            bool enabled,
                            bool only_write_if_key_exists,
                            bool flush
                          );

    //! Sets the close on exit key for the specified section
    /*!
        \param section The configuration section
        \param enabled TRUE if the close_on_error key should be set to 1
        \param only_write_if_key_exists TRUE if only should write the key if it exists
        \param flush Whether to flush the configuration or not
     */
    void set_close_on_error( const wxString& section, 
                             bool enabled,
                             bool only_write_if_key_exists,
                             bool flush
                           );

//----------------------------------------------------------------------------------------    
    
//! For calling get_plucker_directory
enum {
    PLUCKERHOME                = 0,
    CHANNELS,        
    RESOURCES,
    LOCALE,
    HANDHELD_VIEWER_SOFTWARE,
    DOCUMENTATION,
    WXWINDOWS_CONFIGURATION,
    PYPLUCKER,
    PYTHON_VM
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_CONFIGURATION_H_

