//----------------------------------------------------------------------------------------
// Name:        configuration.cpp
// Purpose:     Handles saving and loading of configuration settings
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor
// Licence:     GPL
// RCS-ID:      $Id: configuration.cpp,v 1.42 2007/06/25 14:12:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "configuration.h"
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

#include "configuration.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/dir.h"
#include "wx/filename.h"

//----------------------------------------------------------------------------------------

#include "plucker-desktop.h"
#include "utils_string.h"
#if ( setupUSE_MSW_PLUCKER_BUNDLE_PATH_REGISTRY_CHECK )
    #include "os_msw.h"
#endif  

//----------------------------------------------------------------------------------------
// Private variables
//----------------------------------------------------------------------------------------

wxFileConfig* the_configuration = NULL;

// Keys no longer supported in configuration file. New versions of keys in comments
// to right of key, if applicable.
const wxString depreciated_channel_keys[] = 
				{ 
					wxT( "db_name" ),           // doc_name
					wxT( "db_file" ),           // doc_file
                    wxT( "zlib_compression" ),  // compression
                    wxT( "is_install_to_card" ) // handheld_target_storage_mode
				};
// More deprecated keys are (non channels):
//		/PLUCKER_DESKTOP/plucker_home (on MSW) -> Replaced with /PLUCKER_DESKTOP/plucker_bundle_directory
//		/PLUCKER_DAEMON/ (all keys)
//		(The desktop doesn't support the exec before/after at channel level

// Keys permitted in a section name, since others will crash parser, be refused by OS as
// a file or directory name, etc. Noteably, the extended characters are removed also, things
// like ASCII number 137, since those cause problems too.                              
const wxString legal_section_name_characters =
               wxT( "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_" );

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

void init_configuration()
{
    wxString configuration_fullname;
    wxString configuration_path;
    
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
	// The config file is stored in ~.
	// A ~ should always exist, so we don't need to do a wxMkdir.
    configuration_path = wxGetHomeDir() << wxT( "/." );
    configuration_fullname = configuration_path << wxT( "/" ) << get_configuration_filename();
#endif

#ifdef __WXMAC__
    configuration_path = wxGetHomeDir() << wxT( "/Library/Plucker" );
    
    // Need to make the folder if it doesn't exist yet.
    //! \todo We should be obeying PLUCKERHOME environment variable instead of forcing a file.
    //! \todo This is assuming that /home/<user>/Library/ always exists.
	if ( ! wxDir::Exists( configuration_path ) ) 
	{
        wxMkdir( configuration_path, 0777 );
    }

    configuration_fullname = configuration_path << wxT( "/" ) << get_configuration_filename();
#endif  

#ifdef __WXMSW__
    configuration_path = wxGetHomeDir() << wxT( "/Application Data/Plucker" );

    wxString	configuration_rootpath = wxGetHomeDir() << wxT( "/Application Data/" );
 	if ( ! wxDir::Exists( configuration_rootpath ) ) 
	{
        wxMkdir( configuration_rootpath, 0777 );
    }   
	if ( ! wxDir::Exists( configuration_path ) ) 
	{
        wxMkdir( configuration_path, 0777 );
    }

    configuration_fullname = configuration_path << wxT( "/" ) << get_configuration_filename();

 #endif

	// This creates a plucker.ini / .pluckerrc on all platforms.
    the_configuration = new wxFileConfig( wxT( "Plucker Desktop" ),
                                          wxT( "The Plucker Team" ), 
                                          configuration_fullname,
                                          wxEmptyString,
                                          wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS );


    wxLogDebug( wxT( "String for main configuration is %s" ), configuration_fullname.c_str() );
    
    // We don't want to expand path variables, otherwise errors when save things
    // with a $ or similar markup, since we are reading/writing without escape 
    // characters.
    the_configuration->SetExpandEnvVars( FALSE );
    
    // Sets the configuration to the currently active one.
    wxConfigBase::Set( the_configuration );
   
    // Write the version string to the preferences
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/plucker_desktop_version" ),
                              plkrPLUCKER_DESKTOP_VERSION_STRING );
                              
#ifdef __WXMSW__
    wxString plucker_bundle_path;
    #if ( setupUSE_MSW_PLUCKER_BUNDLE_PATH_REGISTRY_CHECK )
        // First run on MSW: Get the value of pluckerhome from registry. We will
        // write it to the configuration file, at end of init, for faster reading later.
        plucker_bundle_path = os_msw::get_plucker_bundle_path_from_registry();
    #else
        plucker_bundle_path = wxGetCwd();
        wxLogDebug( wxT( "Current working directory=" ) + plucker_bundle_path );
        plucker_bundle_path = plucker_bundle_path.BeforeLast( xT( "/" ) );
    #endif // setupUSE_MSW_PLUCKER_BUNDLE_PATH_REGISTRY_CHECK
    configuration_fullname = plucker_bundle_path + wxT( "/" ) + get_configuration_filename();

    // Write the location from plucker_bundle_path to config file, so not have to look it
    // up everytime from registry/calculation for rest of program execution.
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/plucker_bundle_path" ), plucker_bundle_path ); 
    the_configuration->Flush();
#endif

#if defined(__WXMAC__) || defined(__WXMSW__)
    // If no image parser specified in their plucker.ini (which is what happens in a new
    // install), then use pil2:
    if ( the_configuration->Read( wxT( "/DEFAULT/image_parser" ), wxT( "" ) ) == wxT( "" ) ) 
    {
        the_configuration->Write( wxT( "/DEFAULT/image_parser" ), wxT( "pil2" ) );
        the_configuration->Flush();
    }    
#endif
    
}


// Delete the configuration object when finished program, otherwise memory leak
void finish_configuration()
{
   if ( the_configuration ) 
   {
      wxConfigBase::Set( NULL );
      delete the_configuration;
      the_configuration = NULL;
   }
}


// Returns the plucker extension type for the OS. Linux returns 'pluckerrc',
// Everyone else returns plucker.ini [Macintosh should be "plucker Preferences" but 
// that is painful to work with.
wxString get_configuration_filename()
{
    wxString configuration_filename;  
           
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
        configuration_filename =wxT( ".pluckerrc" );
#else
        configuration_filename =wxT( "plucker.ini" );   
#endif

    return configuration_filename;   
}


// A separate one for something else, like one the help settings, etc
wxString get_custom_configuration_filename( const wxString& basename )
{
    wxString configuration_filename;
    wxString buffer_basename = basename;
    
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
        configuration_filename << wxT( "." ) << buffer_basename << wxT( "rc" );
#else
        configuration_filename << buffer_basename << wxT( ".ini" );
#endif

    return configuration_filename;
}


// Look up what the group (section) inside the plucker configuration file
// should be for this os. For example exclusion_lists are under [POSIX],
// [WINDOWS], etc.
wxString get_os_configuration_section()
{   
    wxString section = wxT( "DEFAULT" );
    
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
    section =wxT( "POSIX" );
#endif
        
#ifdef __WXMAC__
    section = wxT( "MACINTOSH" );
#endif

#ifdef __WXMSW__
    section = wxT( "WINDOWS" );
#endif
    
    return section;
}


// Checks to see if it is a channel section of the config file, ie not one a reserved
// section ([DEFAULT], [POSIX], [MACINTOSH], [PLUCKER_DESKTOP], etc) and that it has
// a home_url key.
bool is_channel_section( wxString section )
{   
    if ( is_reserved_section( section ) ) 
    {
        return FALSE;
    }
    else 
    {
        wxString key;
        
        key = section << wxT( "/" ) << wxT( "home_url" );
        if ( the_configuration->HasEntry( key ) ) 
        {
            return TRUE;   
        } 
        else 
        {
            return FALSE;
        }   
    }
}


// Checks to see that if it is a modifier section, that is, it is not a reserved section
//([DEFAULT], [POSIX], [MACINTOSH], [PLUCKER_DESKTOP], etc) and that it not a channel
// section since there is no home_url key of its own, so can only be used to set/override
// some settings for an actual channel.
bool is_modifier_section( wxString section )
{
    if ( is_reserved_section( section ) || is_channel_section( section ) ) 
    {
        return FALSE;
    } 
    else 
    {
        return TRUE;
    }   
}


bool is_reserved_section( wxString section )
{
    // Create a list of known non-channel strings. Need to call the last one as NULL.
    // plkrRESERVED_CONFIGURATION_SECTIONS was defined in configuration.h
    wxStringList *reserved_sections = new wxStringList( plkrRESERVED_CONFIGURATION_SECTIONS ,
                                                        NULL );
    // Check to see whether the section passed is a member of the list of strings
    // of reserved sections
    if ( reserved_sections->Member( section ) ) \
    {
        // TODO: Do stringlists need to be destroyed, or is it automatic?
        delete reserved_sections;
        return TRUE;
    } 
    else 
    {
        delete reserved_sections;
        return FALSE;
    }
}


// Returns a legal new section name, based on the input channel name.
wxString get_legal_new_channel_section( wxString string_to_convert )
{  
    wxString buf;
    
    // If it was a string of nothing then set it to plkrUNNAMED_CHANNEL_NAME
    // to avoid any asserts working on NULL strings.
    if ( string_to_convert == wxT( "" ) ) 
    {
        string_to_convert = plkrUNNAMED_CHANNEL_NAME;
    }

    // Remove any illegal characters.
    string_to_convert = utils_string::remove_illegal_characters( string_to_convert,
                                                                 legal_section_name_characters
                                                               );
    
    // If it was a string of nothing but illegal characters, and we now have just
    // an empty string, then set it to plkrUNNAMED_CHANNEL_NAME to avoid any 
    // asserts working on NULL strings.
    if ( string_to_convert == wxT( "" ) ) 
    {
        string_to_convert = plkrUNNAMED_CHANNEL_NAME;
    }
    
    // Strip it down to 25 letters (Mac filesystem stops at 31. That gives us 999999
    // identical section with the same base name, but a different number at the end.
    string_to_convert.Truncate( plkrMAXIMUM_BASE_SECTION_LENGTH );
    
    // If it is a reserved section name by now, then stick a _ after the last character
    if ( is_reserved_section( string_to_convert ) ) 
    {
        string_to_convert += wxT( "_" );        
    }               
    
    // Store how long the base name is (so know where to truncate when adding on numbers.
    size_t base_section_length = string_to_convert.Length();
    
    wxLogDebug( wxT( "Before loop to append unique number, string_to_convert=" ) + string_to_convert );
    
    // Can't use a section name that already exists, so if it exists, keep incrementing the
    // suffix number until get a section (aka "group") that doesn't exist already. Note that
    // sections ("groups") are not case-sensitive. This is fortunate, since can't have
    // different sections with a different case difference, since can't have 2 directories
    // on MSW with only a difference in case between them, so wouldn't be able to make
    // the new channel subdirectory.
    // The loop condition also makes sure that a directory by the new name doesn't already
    // exist in the channels subdir (it shouldn't, but may be an artifact if people
    // cleaned out their .ini file manually, but didn't erase directorys from channels
    // subdirectory. Also the MSW uninstall doesn't remove channels, for protection of 
    // user created files.     
    for ( long n = 1;    
          the_configuration->HasGroup( string_to_convert ) 
          || wxDirExists( get_plucker_directory( CHANNELS ) + wxT( "/" ) + string_to_convert );        
          n++ ) 
    {
        // Cut off any number extensions that may have been added in a previous
        // iteration of the loop.
        wxLogDebug( wxT( "Before truncate, string_to_convert=" ) + string_to_convert );
        string_to_convert.Truncate( base_section_length );
        wxLogDebug( wxT( "After truncate, string_to_convert=" ) + string_to_convert );
        // Append the new number.
        buf.Printf( wxT( "%ld" ), n );        
        string_to_convert = string_to_convert + buf;
        wxLogDebug( wxT( "After appending unique id, string_to_convert=" ) + string_to_convert );
    }
    
    // All done. Ship it.
    return string_to_convert;
}


// Looks up the root directory, as needed by get_plucker_directory.
// Don't use this function directly, use a get_plucker_directory() instead.
wxString get_pluckerhome_directory()
{ 
    wxString pluckerhome_directory; 
    
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
    bool pluckerhome_exists;
    pluckerhome_exists = wxGetEnv( wxT( "PLUCKERHOME" ), &pluckerhome_directory );
    if ( ! pluckerhome_exists ) 
    {
        pluckerhome_directory = wxGetHomeDir() << wxT( "/.plucker" );
    }
#endif  

#ifdef __WXMAC__
    bool pluckerhome_exists;
    pluckerhome_exists = wxGetEnv( wxT( "PLUCKERHOME" ), &pluckerhome_directory );
    if ( ! pluckerhome_exists ) 
    {
        pluckerhome_directory = wxGetHomeDir() << wxT( "/Library/Plucker" );
    }
#endif  

#ifdef __WXMSW__
    bool pluckerhome_exists;
    pluckerhome_exists = wxGetEnv( wxT( "PLUCKERHOME" ), &pluckerhome_directory );
    if ( ! pluckerhome_exists ) 
    {
        pluckerhome_directory = wxGetHomeDir() << wxT( "/Application Data/Plucker" );
    }
#endif  

    return pluckerhome_directory;
}


// Looks up the directory of the plucker bundle so can find stuff in the bundle.
wxString get_bundle_runtime_files_directory()
{	
    wxString bundle_runtime_files_directory;

#ifdef __WXMAC__
    // On OSX, plucker-desktop executable is packaged like this in the bundle:
    // <blahblah>/Plucker.app/Contents/MacOS/plucker-desktop.
    // We want to get <blahblah>/Plucker.app/Contents so we can then deduce things like
    // <blahblah>/Plucker.app/Contents/Resources/plucker_desktop/resource/xrc/main_dialog.xrc
    bundle_runtime_files_directory = wxGetApp().argv[0];
    bundle_runtime_files_directory = bundle_runtime_files_directory.BeforeLast('/');
    bundle_runtime_files_directory = bundle_runtime_files_directory.BeforeLast('/'); 
    bundle_runtime_files_directory = bundle_runtime_files_directory + wxT( "/Resources" );
#endif   

#ifdef __WXMSW__
	// On MSW, look back at what we wrote to the config back in init_config
	// (via looking at the registry). Otherwise we are going to make a pretty poor guess by
	// just doing ../ (up one directory from where we are running.
    bundle_runtime_files_directory = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/plucker_bundle_path" ),
															  wxT( ".." ) );
#endif

	wxLogDebug( wxT( "bundle_runtime_files_directory=" ) + bundle_runtime_files_directory );
	return bundle_runtime_files_directory;    
}


// Looks up the directory for whatever plucker tool directory is needed. 
wxString get_plucker_directory( long directory_name )
{   
    wxString    directory;
    // The ones that are fixed across users can be set and read from the 
    // configuration file. The dynamic ones can't be, since expandpathvars is
    // turned off since would expand everything with a $
    wxString    default_directory;

    switch ( directory_name )
    {
//--------------------------------------------------------------------------------
        case PLUCKERHOME:
            directory = get_pluckerhome_directory();
            wxLogDebug( wxT( "get_plucker_directory( PLUCKERHOME )=" ) + directory );
            break;
//--------------------------------------------------------------------------------
        case CHANNELS:
            directory = get_pluckerhome_directory();
            directory += wxT( "/channels" );
            wxLogDebug( wxT( "get_plucker_directory( CHANNELS )=" ) + directory );
            break;
//--------------------------------------------------------------------------------
        case RESOURCES:
#if defined(__WXMSW__) || defined(__WXMAC__) 
            default_directory = get_bundle_runtime_files_directory();
            default_directory += wxT( "/plucker_desktop/resource" );
#else
            default_directory = wxT( "/usr/share/plucker-desktop/resource" );
#endif

            directory = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/directory_resource" ),
                                                 default_directory );
                                                 
            wxLogDebug( wxT( "get_plucker_directory( RESOURCES )=" ) + directory );
            break;
//--------------------------------------------------------------------------------
        case LOCALE:
#if defined(__WXMSW__) || defined(__WXMAC__) 
            default_directory = get_bundle_runtime_files_directory();
            default_directory += wxT( "/plucker_desktop/langs" );
#else
            default_directory = wxT( "/usr/share/locale" );
#endif
            directory = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/directory_locale" ),
                                                 default_directory );
            wxLogDebug( wxT( "get_plucker_directory( LOCALE )=" ) + directory );
            break;
//--------------------------------------------------------------------------------
        case HANDHELD_VIEWER_SOFTWARE:
#if defined(__WXMSW__) || defined(__WXMAC__) 
            default_directory = get_bundle_runtime_files_directory();
            default_directory += wxT( "/viewer/palmos" );
#else
            default_directory = wxT( "/usr/share/plucker/palm" );
#endif
            directory = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/directory_pda_viewer" ),
                                                 default_directory );
            wxLogDebug( wxT( "get_plucker_directory( HANDHELD_VIEWER_SOFTWARE )=" ) + directory );
            break;
//--------------------------------------------------------------------------------
        case DOCUMENTATION:
#if defined(__WXMSW__) || defined(__WXMAC__) 
            default_directory = get_bundle_runtime_files_directory();
            default_directory += wxT( "/doc" );
#else
            default_directory = wxT( "/usr/share/doc/packages/plucker-desktop" );
#endif
            directory = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/directory_documentation" ),
                                                 default_directory );
            wxLogDebug( wxT( "get_plucker_directory( DOCUMENTATION )=" ) + directory );
            break;
//--------------------------------------------------------------------------------
        // Where to create a configuration file for the online help, and other
        // wxWindows settings.
        // Can't pollute the .pluckerrc/plucker.ini file as it saves its settings as
        // a "wxWindows/wxHtmlHelpController" section and a '/' in the section will
        // crash the distiller. The docs directory would be logical, but on
        // non-MSW systems the user won't have write permission into the system docs.
        // Also, on GTK, might as well let each user have their own bookmarks. .
        case WXWINDOWS_CONFIGURATION:
#ifdef __WXMSW__
            // Note, only MSW here, not MAC, since MAC may not have write permissions.
            default_directory = get_bundle_runtime_files_directory();
            default_directory +=wxT( "/doc" );
#else
            default_directory = get_pluckerhome_directory();
#endif
            directory = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/directory_wxwindows_configuration" ),
                                                 default_directory );

            wxLogDebug( wxT( "get_plucker_directory( WXWINDOWS_CONFIGURATION )=" ) + directory );
            break;
//--------------------------------------------------------------------------------
        // Directory holding PyPlucker Python parser (this is only called on 
        // Mac and MSW. Linux just uses plucker-build.
        case PYPLUCKER:
#if defined(__WXMSW__) || defined(__WXMAC__) 
            default_directory = get_bundle_runtime_files_directory();
            default_directory += wxT( "/parser/python/PyPlucker" );
#else 
            wxLogError( wxT( "Error: You shouldn't have been able to reach code in Plucker Desktop that retrieves PYPLUCKER directory" ) );
#endif
            directory = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/directory_pyplucker" ),
                                                 default_directory );

            wxLogDebug( wxT( "get_plucker_directory( PYPLUCKER )=" ) + directory );
            break;
//--------------------------------------------------------------------------------
        // Directory holding python.exe. Only for MSW. Mac and Linux are available
        // anywhere, as they are in the path.
        case PYTHON_VM:
#if defined(__WXMSW__)
            default_directory = get_bundle_runtime_files_directory();
            default_directory += wxT( "/parser/python/vm" );
#else 
            wxLogError( wxT( "Error: You shouldn't have been able to reach code in Plucker Desktop that retrieves PYPLUCKER directory" ) );
#endif
            directory = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/directory_python_vm" ),
                                                 default_directory );

            wxLogDebug( wxT( "get_plucker_directory( PTYHON_VM )=" ) + directory );
            break;
//--------------------------------------------------------------------------------
             
        default:
            break;
    }

    return directory;
}


void copy_section( wxString from_section, wxString to_section, bool treat_as_absolute )
{
    wxLogDebug( wxT( "About to copy " ) + from_section + wxT( " section to " ) + to_section );

    // Error check that they aren't empty
    if ( from_section.IsEmpty() || to_section.IsEmpty() ) 
    {
        wxLogDebug( wxT( "Error: can't copy a section since either source or target is empty." ) );
        return;
    }
    
    // Store current wxConfig path so we can restore it when we are done this function.
    wxString old_configuration_path = the_configuration->GetPath();
   
    // If asked to treat the from_section and to_section as absolute, then convert them:
    // Of course of they already have a '/' at their start, then we don't want to add another
    // one.
    if ( treat_as_absolute ) 
    {
        if ( ! to_section.StartsWith( wxT( "/" ), NULL ) ) 
        {
            to_section = wxT( "/" ) + to_section;
        }
        if ( ! from_section.StartsWith( wxT( "/" ), NULL ) ) 
        {
            from_section = wxT( "/" ) + from_section;
        }
    }

    // Use a GetNextEntry() to loop through the lines one by one from the 
    // config file. We can treat all key values as if they were string values, 
    // even though some may be a string of '1' to mean a bool of TRUE. If this was
    // a wxRegConfig object though, then we would have to check types.
    
    wxString key_name;           // Name of the key in the index file
    wxString key_value;          // A string to hold the key value 
    long key_index;              // What key number we are at in the section
    bool next_key_exists;        // Whether a next key still exists in the list
 
    // Because of the way wxConfigBase::GetFirstEntry works, we have to set the path to 
    // the from_section to read in the name of a key into the key_name variable
    the_configuration->SetPath( from_section );
    wxLogDebug( wxT( "wxFileConfigh::SetPath{}=" ) + from_section );

    // Now see whether a key exists, and also get that 1st key name into key_name variable.
    next_key_exists = the_configuration->GetFirstEntry( key_name, key_index );    
    wxLogDebug( wxT( "Tested whether first key exists. Result=%d" ), next_key_exists );
    wxLogDebug( wxT( "The name of that key=" ) + key_name );
       
    while ( next_key_exists ) 
    {        
        // Read key_name's value from from_section 
        key_value = the_configuration->Read( key_name, wxT( "" ) );
        
        // Set the path to the to_section and write out the key_name and key_value pair
        the_configuration->SetPath( to_section );
        the_configuration->Write( key_name, key_value );       
        wxLogDebug( wxT( "Copied key name=" ) + key_name + wxT( " of value=" ) + key_value );
        
        // Set path back to from_section and see whether a next key exists, and also get the
        // key name if so.
        the_configuration->SetPath( from_section );
        next_key_exists = the_configuration->GetNextEntry( key_name, key_index );
    }

    //  Restore the configuration to whatever it was before we entered this function
    the_configuration->SetPath( old_configuration_path );

    // Flush the toilet when you are done (writes the changes to config file).
    the_configuration->Flush();

    wxLogDebug( wxT( "Completed copy_section()" ) );
}


void import_sections_as_section_names( wxArrayString& selected_sections,
                                       long options,
                                       wxFileConfig* from_configuration,
                                       wxFileConfig* to_configuration
                                     )
{
    for ( size_t i=0; i < selected_sections.GetCount(); i++ ) 
    {
          copy_section2( selected_sections.Item( i ),
                         selected_sections.Item( i ),
                         options,
                         from_configuration,
                         to_configuration );
    }
}


void copy_section2( const wxString& from_section,
                    const wxString& to_section,
                    long options,
                    wxFileConfig* from_configuration,
                    wxFileConfig* to_configuration
                  )
{
    // Convert our const's to variables.
    wxString a_from_section = from_section;
    wxString a_to_section   = to_section;

    wxLogDebug( wxT( "About to copy " ) + a_from_section + wxT( " section to " ) + a_to_section );

    // Error check that they aren't empty
    if ( a_from_section.IsEmpty() || a_to_section.IsEmpty() )
    {
        wxLogDebug( wxT( "Error: can't copy a section since either source or target section is empty." ) );
        return;
    }

    // Error check that the configuration objects exist
    if ( ! ( from_configuration && to_configuration ) ) 
    {
       wxLogDebug( wxT( "Error: can't copy a section since either source or target wxFileConfig is NULL." ) );
       return;
    }

    // Because of the way wxConfigBase::GetFirstEntry works, we have to set the path to
    // the a_from_section to read in the name of a key into the key_name variable.
    // But most store this and restore it when we are done, else, when looping
    // through all the sections in the from config, we won't get any more since
    // the path would have been changed.
    // Store current wxConfig paths so we can restore it when we are done this function.
    wxString old_to_configuration_path   = to_configuration->GetPath();
    wxString old_from_configuration_path = from_configuration->GetPath();

    // If asked to treat the a_from_section and a_to_section as absolute, then convert them:
    // Of course of they already have a '/' at their start, then we don't want to add another
    // one.
    if ( options & optionCONFIGURATION_COPY_SECTION_TREAT_AS_ABSOLUTE ) 
    {
        if ( ! a_to_section.StartsWith( wxT( "/" ), NULL ) ) 
        {
            a_to_section = wxT( "/" ) + a_to_section;
        }
        if ( ! a_from_section.StartsWith( wxT( "/" ), NULL ) ) 
        {
            a_from_section = wxT( "/" ) + a_from_section;
        }
    }

    // Use a GetNextEntry() to loop through the lines one by one from the
    // config file. We can treat all key values as if they were string values,
    // even though some may be a string of '1' to mean a bool of TRUE. If this was
    // a wxRegConfig object though, then we would have to check types.

    wxString key_name;           // Name of the key in the index file
    wxString key_value;          // A string to hold the key value
    long     key_index;          // What key number we are at in the section
    bool     next_key_exists;    // Whether a next key still exists in the list

    from_configuration->SetPath( a_from_section );
    wxLogDebug( wxT( "wxFileConfig::SetPath{}=" ) + a_from_section );

    // Now see whether a key exists, and also get that 1st key name into key_name variable.
    next_key_exists = from_configuration->GetFirstEntry( key_name, key_index );
    wxLogDebug( wxT( "Tested whether first key exists. Result=%d" ), next_key_exists );
    wxLogDebug( wxT( "The name of that key=" ) + key_name );

    while ( next_key_exists ) 
    {
        // Read key_name's value from a_from_section
        key_value = from_configuration->Read( key_name, wxT( "" ) );

        // Set the path to the a_to_section and write out the key_name and key_value pair
        to_configuration->SetPath( a_to_section );
        to_configuration->Write( key_name, key_value );
        wxLogDebug( wxT( "Copied key name=" ) + key_name + wxT( " of value=" ) + key_value );

        // Set path back to a_from_section and see whether a next key exists, and also get the
        // key name if so.
        from_configuration->SetPath( a_from_section );
        next_key_exists = from_configuration->GetNextEntry( key_name, key_index );
    }

    //  Restore the configurations to whatever they wer before we entered this function
    from_configuration->SetPath( old_from_configuration_path );
    to_configuration->SetPath( old_to_configuration_path );

    if ( options & optionCONFIGURATION_COPY_SECTION_FLUSH_AT_END )
    {
        // Flush the toilet when you are done (writes the changes to config file).
        to_configuration->Flush();
    }

    wxLogDebug( wxT( "Completed copy_section2()" ) );
}

void copy_all_sections( wxFileConfig* from_configuration,
                        wxFileConfig* to_configuration,
                        long options
                      )
{
    wxString section_name;             // Name of the section in the index file
    long     section_index;            // What section number we are at in the section
    bool     next_section_exists;      // Whether a next section still exists in the list
    long     passed_options = options; // Since the copy at end is different

    // Now see whether a section exists, and also get that 1st section name into section_name variable.
    next_section_exists = from_configuration->GetFirstGroup( section_name, section_index );
    wxLogDebug( wxT( "copy_all_sections: Tested whether first section exists. Result=%d" ), next_section_exists );
    wxLogDebug( wxT( "copy_all_sections: The name of that section=%s" ), section_name.c_str() );

    // If flushing at end of all copies, then don't want to flush after each section.
    if (  options & optionCONFIGURATION_COPY_SECTION_FLUSH_AT_END ) 
    {
        passed_options = options - optionCONFIGURATION_COPY_SECTION_FLUSH_AT_END;
    }

    while ( next_section_exists ) 
    {
        wxLogDebug( wxT( "copy channel section before=%s" ), section_name.c_str() );
        // Copy over section (error checks that from_configuration is OK).
        copy_section2( section_name,
                       section_name,
                       passed_options,
                       from_configuration,
                       to_configuration
                    );
        wxLogDebug( wxT( "copy channel section before=%s" ), section_name.c_str() );
        next_section_exists = from_configuration->GetNextGroup( section_name,
                                                                section_index );
    }

    // If told to flush now, then flush the to_configuration
    if ( options & optionCONFIGURATION_COPY_SECTION_FLUSH_AT_END ) 
    {
        to_configuration->Flush();
    }
}


void strip_depreciated_keys( wxString section )
{
    // Store current wxConfig path so we can restore it when we are done this function.
    wxString old_configuration_path = the_configuration->GetPath();

    the_configuration->SetPath( section );

    for ( int i=0; i < (int)WXSIZEOF( depreciated_channel_keys ); i++ )
    {
        if ( the_configuration->Exists( depreciated_channel_keys[ i ] ) ) 
        {
            the_configuration->DeleteEntry( depreciated_channel_keys[ i ], FALSE );
        }
    }

    //  Restore the configuration to whatever it was before we entered this function
    the_configuration->SetPath( old_configuration_path );

}


// Converts a plucker: or file: filename to a regular OS filepath
wxString protocol_filename_to_normal_fullname( const wxString &input_filename )
{
    wxString prefix_to_strip        = wxEmptyString;
    wxString output_fullname        = input_filename;
    bool     prepend_plucker_path   = FALSE;
    
    // Call our function to find the prefix.
    prefix_to_strip = get_protocol_prefix( output_fullname );    

    // If found one, then remove it, placing rest into output_fullname.
    if ( prefix_to_strip != wxEmptyString )
    {
        output_fullname.StartsWith( prefix_to_strip, &output_fullname );        
        // If found one of plucker protocols, then tell to prepend full path to pluckerdir    
        if ( prefix_to_strip.Contains( wxT( "plucker" ) ) ) 
        {
            prepend_plucker_path = TRUE;
        }        
    }    
    
    // Also prepend plucker path to relative filenames. Using wxIsAbsolutePath() instead
    // of checking for a leading '/' as MSW/MAC can start an absolute with a drive.
    if ( ! wxIsAbsolutePath( output_fullname ) )
    {
        prepend_plucker_path = TRUE;
    }

    // Do the prepend if so requested
    if ( prepend_plucker_path )
    {
        // ?TODO: Should this be the user's pluckerdir on POSIX. 
        output_fullname = get_plucker_directory( PLUCKERHOME ) + wxT( "/" ) + output_fullname;
    }
   
    wxLogDebug( wxT( "Filename via protocol_filename_to_normal_fullname=" ) + output_fullname );
    return output_fullname;
}


void set_close_on_exit( const wxString& section, 
                        bool enabled,
                        bool only_write_if_key_exists,
                        bool flush )
{
    wxString close_on_exit_key  = wxT( "/" ) + section + wxT( "/" ) + wxT( "close_on_exit" );
    
    // Write the keys if already exist, or we specified that want to write them anyways.
    if ( the_configuration->Exists( close_on_exit_key ) || ! only_write_if_key_exists ) 
    {
        the_configuration->Write( close_on_exit_key, (long)enabled );
    }
    
    // Flush changes if so requested
    if ( flush ) {
        the_configuration->Flush();
    }
}


void set_close_on_error( const wxString& section, 
                         bool enabled,
                         bool only_write_if_key_exists,
                         bool flush )
{
    wxString close_on_error_key  = wxT( "/" ) + section + wxT( "/" ) + wxT( "close_on_error" );
    
    // Write the keys if already exist, or we specified that want to write them anyways.
    if ( the_configuration->Exists( close_on_error_key ) || ! only_write_if_key_exists ) 
    {
        the_configuration->Write( close_on_error_key, (long)enabled );
    }
    
    // Flush changes if so requested
    if ( flush ) 
    {
        the_configuration->Flush();
    }
}


// Get a protocol prefix of a file
wxString get_protocol_prefix( wxString &input_filename ) 
{
    wxString found_prefix;
    
    wxString prefix_bases[] = { wxT( "plucker:" ),
                                wxT( "PLUCKER:" ),
                                wxT( "file:" ),
                                wxT( "FILE:" ),
                                wxT( "http:" ),
                                wxT( "HTTP:" )
                              };
                          
    for ( int n=0; n < (int)WXSIZEOF( prefix_bases ); n++ ) 
    {    
        // Does it have just the prefix base
        if ( input_filename.StartsWith( prefix_bases[ n ] ) ) 
        {
            found_prefix = prefix_bases[ n ];            
        }
        
        // Does it have the prefix base + 1 slash
        if ( input_filename.StartsWith( prefix_bases[ n ] + wxT( "/" ) ) ) 
        {
            found_prefix = prefix_bases[ n ] + wxT( "/" );
        }
        
        // Does it have the prefix base + 2 slashes
        if ( input_filename.StartsWith( prefix_bases[ n ] + wxT( "//" ) ) ) 
        {
            found_prefix = prefix_bases[ n ] + wxT( "//" );
        }
    }

    wxLogDebug( wxT( "get_protocol_prefix reports finding a " ) + found_prefix + wxT( "prefix" ) );
    return found_prefix;

}
