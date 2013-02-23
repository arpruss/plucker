//----------------------------------------------------------------------------------------
// Name:        about_dialog.cpp
// Purpose:     Describes about_dialog class
// Author:      Robert O'Connor
// Modified by:
// Created:     2002/08/22
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: about_dialog.cpp,v 1.16 2007/06/29 00:58:49 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "about_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_ENHANCED_ABOUT_DIALOG )

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

#include "about_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/ffile.h"                // wxFFile   

//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

//----------------------------------------------------------------------------------------

#include "utils_controls.h"
#include "utils_string.h"
#include "utils_datetime.h"
#include "configuration.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( about_dialog, wxDialog )

END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

about_dialog::about_dialog( wxWindow* parent )
{	
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "about_dialog" ) );
    
    // Set up the contents
    set_plucker_credits_html();
    set_plucker_desktop_credits_html();
    set_version_and_build_date_text();    
}


about_dialog::~about_dialog()
{
}


void about_dialog::set_plucker_credits_html()
{   
    // The whole string
    wxString total_string;

    // Open up the body
    total_string << wxT( "<body bgcolor='#FFFFFF' text='" ) << plkrGUI_ELEMENT_DARK_COLOR_RGB_STRING << wxT( "'>" );

    // Put in an authors header
    total_string << create_section_header( _( "Authors" ) );

    // Put in the authors
    total_string << create_string_from_file( get_plucker_directory( RESOURCES ) + wxT( "/html_about/plucker_authors.htm" ) );

    // Put in an empty space
    total_string << wxT( "<p>&nbsp;" );
    
    // Put in a credits header
    total_string << create_section_header( _( "Credits" ) );    

    // Put a note about the credits
    total_string << wxT( "<p>" ) << _( "The following kind folks provided support, ideas or conversation as part of the" )
                << wxT( " " ) << plkrAPPLICATION_SUITE_NAME << wxT( " " ) << _( "project. May they live long and prosper:" );

    // Put in the credits
    total_string << create_string_from_file( get_plucker_directory( RESOURCES ) + wxT( "/html_about/plucker_credits.htm" ) );
    
    // Put a note about adding yourself
    total_string << wxT( "<p>" ) << _( "If you feel left out, then please add yourself to the file, or send e-mail to <u>plucker-dev@rubberchicken.org</u>! " );
    
    // Close the font color
    total_string 
    << wxT( "</font>" );
    
    wxHtmlWindow* plucker_credits_htmlwindow = XRCCTRL( *this, "about_dialog_plucker_credits_tab_htmlwindow", wxHtmlWindow );
    
    utils_controls::set_html_window_text_in_gui_style( plucker_credits_htmlwindow,        
                                                       total_string,
                                                       optionSET_HTML_BACKGROUND_TO_MATCH_GUI_LISTBOX      |
                                                       optionSET_HTML_FONT_COLOR_TO_MATCH_GUI              |
                                                       optionSET_HTML_FONT_FACE_AND_POINTSIZE_TO_MATCH_GUI
 
                                                     );

}


void about_dialog::set_plucker_desktop_credits_html()
{
    // The whole string
    wxString total_string;

    // Open the body
    total_string << wxT( "<body bgcolor='#FFFFFF' text='" ) << plkrGUI_ELEMENT_DARK_COLOR_RGB_STRING << wxT( "'>" );
    
    // Put in a translators header
    total_string << create_section_header( _( "Translators" ) );   
    
    // Put a note about the translaotrs
    total_string << wxT( "<p>" ) << _( "The following kind folks brought" ) << wxT( " ")
        << plkrAPPLICATION_DESKTOP_NAME << wxT( " " ) << _( "to their speakers of their language:" );

    // Put in the translators
    total_string << create_string_from_file( get_plucker_directory( RESOURCES ) + wxT( "/html_about/plucker_desktop_translators.htm" ) );

    // Put in an empty space
    total_string << wxT( "<p>&nbsp;" );
    
    // Put in a credits header
    total_string << create_section_header( _( "Credits" ) );    

    // Put a note about the credits
    total_string << wxT( "<p>" ) << _( "The following kind folks provided support, ideas or conversation as part of the" )
        << wxT( " " ) << plkrAPPLICATION_DESKTOP_NAME << wxT( " " ) << _( "project. May they live long and plucker:" );

    // Put in the credits
    total_string << create_string_from_file( get_plucker_directory( RESOURCES ) + wxT( "/html_about/plucker_desktop_credits.htm" ) );
    
    // Put in an empty space
    total_string << wxT( "<p>&nbsp;" );

    // Put in an authors header
    total_string << create_section_header( _( "Authors" ) );

    // Put in the authors
    total_string << create_string_from_file( get_plucker_directory( RESOURCES ) + wxT( "/html_about/plucker_desktop_authors.htm" ) );
    
    // Put a note about adding yourself
    total_string << wxT( "<p>" ) << _( "If you feel left out, then please add yourself to the file, or send e-mail to <u>plucker-dev@rubberchicken.org</u>!" );
    
    // Close the body
    total_string 
    << wxT( "</body>" );
    
    wxHtmlWindow* plucker_desktop_credits_htmlwindow = XRCCTRL( *this, "about_dialog_desktop_credits_tab_htmlwindow", wxHtmlWindow );

    utils_controls::set_html_window_text_in_gui_style( plucker_desktop_credits_htmlwindow,        
                                                       total_string,
                                                       optionSET_HTML_BACKGROUND_TO_MATCH_GUI_LISTBOX      |
                                                       optionSET_HTML_FONT_COLOR_TO_MATCH_GUI              |
                                                       optionSET_HTML_FONT_FACE_AND_POINTSIZE_TO_MATCH_GUI 
                                                     );

}


wxString about_dialog::create_section_header( const wxString& section )
{
    wxString output_string;
    
    output_string
    << wxT( "<table border='0' cellpadding='0' cellspacing='0' width='100%'>" )
    <<   wxT( "<tr>" )
    <<      wxT( "<td>" )
    <<         wxT( "<img src='" ) << get_plucker_directory( RESOURCES ) 
    <<                      wxT( "/graphics_dialog/about_dialog_header.xpm" ) << wxT( "'>" )
    <<      wxT( "</td>" )
    <<   wxT( "</tr>" )
    <<   wxT( "<tr>" )
    <<      wxT( "<td bgcolor='" ) << plkr_GUI_LIGHT_COLOR_RGB_STRING <<wxT( "' valign='middle' align='left'>" )
    // Small hack here to have a bit of padding around the subsequent text.
    // by making an invisible character of +1 size
    <<          wxT( "<font color='" ) << plkr_GUI_LIGHT_COLOR_RGB_STRING << wxT( "'>" ) << wxT( "I" ) << wxT( "</font>" )
    <<          wxT( "<font color='#FFFFFF'><b>" ) << section << wxT( "</b></font>" )
    <<      wxT( "</td>" )
    <<   wxT( "</tr>" )
    << wxT( "</table>" );
    
    return output_string;
}


wxString about_dialog::create_string_from_file( const wxString& fullname )
{
    wxString output_string;
    
    wxFFile file( fullname );
    wxLogDebug( wxT( "Filename to open is" ) + fullname ); 

    if ( file.IsOpened() )  
    {

        if ( ! file.ReadAll( &output_string ) ) 
        {
            wxLogDebug( wxT( "Error: couldn't read file" ) + fullname );
        }
    }
    else
    {
        wxLogDebug( wxT( "Error: couldn't open file" ) + fullname );    
    }
    
    wxLogDebug( wxT ( "Output string from read html file=" + output_string ) );
    return output_string;
}
            
void about_dialog::set_version_and_build_date_text()
{     
    // Update the version
    XRCCTRL( *this, "about_dialog_details_tab_version_value_statictext", wxStaticText )
        ->SetLabel( plkrPLUCKER_DESKTOP_VERSION_STRING );
   
    wxSize new_version_statictext_size;
    new_version_statictext_size = XRCCTRL( *this, "about_dialog_details_tab_version_value_statictext", wxStaticText )
                                       ->GetBestSize();
    
    XRCCTRL( *this, "about_dialog_details_tab_version_value_statictext", wxStaticText )
        ->SetSize( new_version_statictext_size );     
        
    // Update the compilation date.
    XRCCTRL( *this, "about_dialog_details_tab_build_date_value_statictext", wxStaticText )
        ->SetLabel( utils_datetime::get_translated_compilation_date_string() );

    wxSize new_build_date_statictext_size;
    new_build_date_statictext_size = XRCCTRL( *this, "about_dialog_details_tab_build_date_value_statictext", wxStaticText )
                                         ->GetBestSize();
    
    XRCCTRL( *this, "about_dialog_details_tab_build_date_value_statictext", wxStaticText )
        ->SetSize( new_build_date_statictext_size );    
    
 
    // Tell the window to recalculate itself according to its sizer
    GetSizer()->RecalcSizes();

    GetSizer()->Fit( this );

    GetSizer()->SetSizeHints( this );

    Layout();
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_ENHANCED_ABOUT_DIALOG
