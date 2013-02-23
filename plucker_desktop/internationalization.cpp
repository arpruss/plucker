//----------------------------------------------------------------------------------------
// Name:        internationalization.cpp
// Purpose:     Maintains what translation [aka 'Locale'] (using .mo files) of the 
//              program to use.
//              All LOCALE files are located in the directory called by
//              get_plucker_directory( LOCALE )
//              See TRANSLATING file in plucker-desktop/langs more information.
// Author:      Robert O'Connor
// Modified by:
// Created:     2002/02/16
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: internationalization.cpp,v 1.20 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "internationalization.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTERNATIONALIZATION )

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

#include "internationalization.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "configuration.h"
#include "plucker-desktop.h"        // Since calling wxGetApp(). TODO: Needed?

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

// Generate a string array of languages for the popup multiple choice box
// TODO: Generate this automatically by searching the folder for tranlated .po files

/*
// A struct to hold info about each tranlation,
struct plucker_locale {
   wxString language_name;          // The locales's name, like "French"
   int wx_locale_number;            // The wxWindows number give to the locale     
};
*/

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

bool internationalization::init_internationalization()
{
    int locale_wx_number;
    bool successful = FALSE;
    
    // If configuration file says to use internationalization, then continue, otherwise
    // return FALSE.
    if ( (bool) the_configuration->Read( wxT( "/PLUCKER_DESKTOP/internationalization_enabled"), 0L ) )
    {
       
        if ( the_configuration->Read( wxT( "/PLUCKER_DESKTOP/locale_wx_number"), -1L  ) != -1  ) 
        {
            locale_wx_number = get_wx_locale_number_from_configuration();
        }
        else
        {
            locale_wx_number = get_wx_locale_number_from_choice_dialog(); 
            the_configuration->Write( wxT( "/PLUCKER_DESKTOP/locale_wx_number"), (long)locale_wx_number );
            // Flush the results so will correctly load up the languages settings.
            the_configuration->Flush();       
        }
        successful = set_locale_to_locale_wx_number( locale_wx_number );        
        
    } 
    else 
    {
        successful = FALSE;
    }   
    return successful;
}


// Currently neither deleting m_Locale nor setting to null. 
bool internationalization::finish_internationalization()
{
    return TRUE;
}



bool internationalization::set_locale_to_locale_wx_number( int locale_wx_number )
{
    
#if ( setupUSE_MSW_EN_US_LOCALE_FAILURE_HACK )
    if ( ( locale_wx_number == wxLANGUAGE_ENGLISH_US ) || 
         ( ( locale_wx_number == wxLANGUAGE_DEFAULT ) && ( wxLocale::GetSystemLanguage() == wxLANGUAGE_ENGLISH_US ) )
        ) 
    {
        locale_wx_number = wxLANGUAGE_ENGLISH;     
    }
#endif
    
#if ( setupUSE_INTERNATIONALIZATION )

    // Locale could be -1 from the configuration file, or the popup choice could have
    // set it.
    if ( locale_wx_number == -1 ) 
    {
        return FALSE;
    } 
    else 
    {    
        // By default, wxWindows looks for .po files (the translations) in an application's root
        // directory for .po/.mo files. This adds this directory to the places to look for .po
        // files.
        wxString po_translation_files_path = get_plucker_directory( LOCALE );
        wxGetApp().get_application_locale()->AddCatalogLookupPathPrefix( po_translation_files_path );
        
        // Intializes the locale, to the number we set. The wxLOCALE_LOAD_DEFAULT means
        // to automatically load the file called wxstd.po also, which contains all the messages
        // for wxWindows.
        wxGetApp().get_application_locale()->Init( locale_wx_number, wxLOCALE_LOAD_DEFAULT );
        
        // Adds the catalog of tranlations, looking in the directory set above, in the 
        // subdir of the two letter iso code, and plucker-desktop.po
        wxGetApp().get_application_locale()->AddCatalog( wxT( "plucker-desktop") );  
        
        // Send some debugging info to see what is going on.
        send_debugging_locale_information();
        
        return TRUE; // TODO: error check the Init and AddCatalog.
    }
    
#else   // setupUSE_INTERNATIONALIZATION 
    return FALSE;
#endif  // setupUSE_INTERNATIONALIZATION 
    
}


int internationalization::get_wx_locale_number_from_configuration()
{
    int configuration_locale_wx_number;
    
    configuration_locale_wx_number = (int) the_configuration->Read( wxT( "/PLUCKER_DESKTOP/locale_wx_number"), 0L  );

    return configuration_locale_wx_number;
}



int internationalization::get_wx_locale_number_from_choice_dialog()
{
    // I will initialize it to zero for wxLANGUAGE_DEFAULT
    int selected_locale_wx_number = 0;
    
    // These need to be inside the class in order to get translated at runtime
    // [if was outside class, then lookup is before we set the locale]
    const wxString plkr_languages[] = { _( "[System default]" ),
                                        _( "Catalan" ),
                                        _( "Czech" ),
                                        _( "Danish" ),
                                        _( "German" ),
                                        _( "Spanish" ),
                                        _( "English" ),
                                        _( "English (US)" ),
                                        _( "Faeroese" ),
                                        _( "French" ),
                                        _( "Italian" ),
                                        _( "Japanese" ),
                                        _( "Dutch" ),
                                        _( "Polish" ),
                                        _( "Russian" )
                                      };
    
    /*! \test I don't think this is needed anymore, as wx233 is supposed to not stop
       when close top frame while still inside OnInit()
     */
    // Usual wxWindows behaviour is to kill the application when the top window
    // is closed. This prevents that behavior, until we are done showing this popup.                         
    wxGetApp().SetExitOnFrameDelete( FALSE );    
    
    // Show the multiple choice box, returning the index of the number they selected
    int selected_language_index = wxGetSingleChoiceIndex( _( "Please choose language:" ),
                                                          _( "Language" ), 
                                                          WXSIZEOF( plkr_languages ), 
                                                          plkr_languages );
                                                          
    // Put application's behaviour back to ending on closing the top frame.
    wxGetApp().SetExitOnFrameDelete( TRUE );    
   
    // Set the wxLocale language to the one they selected
    switch ( selected_language_index ) 
    {    
        case 0  : selected_locale_wx_number = wxLANGUAGE_DEFAULT;     break;
		case 1  : selected_locale_wx_number = wxLANGUAGE_CATALAN;     break;
        case 2  : selected_locale_wx_number = wxLANGUAGE_CZECH;       break;
        case 3  : selected_locale_wx_number = wxLANGUAGE_DANISH;      break;
        case 4  : selected_locale_wx_number = wxLANGUAGE_GERMAN;      break;
        case 5  : selected_locale_wx_number = wxLANGUAGE_SPANISH;     break;      
        case 6  : selected_locale_wx_number = wxLANGUAGE_ENGLISH;     break;
        case 7  : selected_locale_wx_number = wxLANGUAGE_ENGLISH_US;  break;
        case 8  : selected_locale_wx_number = wxLANGUAGE_FAEROESE;    break;
        case 9  : selected_locale_wx_number = wxLANGUAGE_FRENCH;      break;
        case 10 : selected_locale_wx_number = wxLANGUAGE_ITALIAN;     break;
        case 11 : selected_locale_wx_number = wxLANGUAGE_JAPANESE;    break;
        case 12 : selected_locale_wx_number = wxLANGUAGE_DUTCH;       break;
        case 13 : selected_locale_wx_number = wxLANGUAGE_POLISH;      break;
        case 14 : selected_locale_wx_number = wxLANGUAGE_RUSSIAN;     break; 
    }
    
    return selected_locale_wx_number;

}

// Print out some debugging information
void internationalization::send_debugging_locale_information()
{

#ifdef __WXDEBUG__
#if ( setupUSE_INTERNATIONALIZATION )
    // Send some debugging info to see what is going on.
    wxString locale_debug_string;

// FOr some reason, ViscualC doesn't like it.
#ifndef __VISUALC__
    locale_debug_string.Printf( wxT( "Language: %s\n"
                                     "System locale name: %s\n"
                                     "Canonical locale name: %s\n" ),
                                wxGetApp().get_application_locale()->GetLocale(),
                                wxGetApp().get_application_locale()->GetSysName().c_str(),
                                wxGetApp().get_application_locale()->GetCanonicalName().c_str() 
                              );
#endif  // __VISUALC__
    wxLogDebug( locale_debug_string );
#endif  // setupUSE_INTERNATIONALIZATION 
#endif  // __WXDEBUG__

}


//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTERNATIONALIZATION
