//----------------------------------------------------------------------------------------
// Name:        utils_controls.cpp
// Purpose:     Utils for wx Controls
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: utils_controls.cpp,v 1.27 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "utils_controls.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"      // Only till can remove the persistant controls hack.    

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

#include "utils_controls.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/colordlg.h"            // wxColorDialog
#include "wx/filedlg.h"             // wxFileDialog
#include "wx/dirdlg.h"              // wxDirDialog

// ---------------------------------------------------------------------------------------

#include "utils_string.h"
#include "utils_datetime.h"
#include "configuration.h"

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

// Fixes a a wxSplitterWindow sash position that is is such that it exceeds a
// minimum pane width of the right/bottom window, or is in fact set beyond the
// wxSplitterWindow dimension. If so, it is out of range, it backtracks the 
// sash it to the minimum pane width. Returns whether or not it needed 
// to be fixed.
bool utils_controls::splitterwindow_fix_sash_out_of_range( wxSplitterWindow* splitterwindow )
{   
    // If its not split, impossible to be out of range, so abort
    if ( ! splitterwindow->IsSplit() ) 
    {
        wxLogDebug( wxT( "Window not split, so sash fix aborting." ) );
        return FALSE;
    }   
  
    int ctrl_width;
    int ctrl_height;    
    splitterwindow->GetSize( &ctrl_width, &ctrl_height ); 
 
    int split_mode = splitterwindow->GetSplitMode();
    
    int window_maximum;
    if ( split_mode == wxSPLIT_VERTICAL ) 
    {
        window_maximum = ctrl_width;
    } 
    else
    {
        window_maximum = ctrl_height;
    }
    
    int sash_position = splitterwindow->GetSashPosition();
    // Minimum pane size is zero if was never set.
    int minimum_pane_size = splitterwindow->GetMinimumPaneSize();
    // Check to see if the sash position is legal, ie, within the minimum pane size.    
    if ( sash_position < ( window_maximum - minimum_pane_size ) )
    {
        // It's okay, so abort and return FALSE.
        wxLogDebug( wxT( "Sash within allowable parameters, so sash fix aborting." ) );
        return FALSE;
    } 
    else 
    {
        // Ilegal, so set the sash back so its within a minimum pane width.
        // TRUE redraws the sash.
        splitterwindow->SetSashPosition( ( window_maximum - minimum_pane_size ), TRUE );
        wxLogDebug( wxT( "Sash exceeds allowable parameters, so fixing sash." ) );
        // Return TRUE, since it needed a fix.
        return TRUE;
    }
}


// When a "Choose color..." button beside a hex color textctrl is clicked, show
// a color dialog, initialize to textctrl's previous value if possible, get the
// color, convert to hex, put it back in the textctrl. Return FALSE if there
// was a problem.
bool utils_controls::textctrl_hex_color_selection( wxTextCtrl* target_color_textctrl )
{
    wxColour col;
    wxColourData color_data;
    
    // Set the default selected color to the hex one already in the textbox, 
    // if possible:
    // 1. Extract the old hex value
    wxString old_hex_color_string = target_color_textctrl->GetValue();    
    // 2. Check to see if valid
    if ( utils_string::is_valid_hex_color_string( old_hex_color_string ) ) 
    {
        wxLogDebug( wxT( "Valid old_hex_color_string=" ) + old_hex_color_string );
        // Convert textboxes previous color from its hex value [utils_string.cpp]
        col = utils_string::hex_string_to_color( old_hex_color_string );
    } 
    else 
    {
        // MSW's Cygwin,BCC (at least) as of Wx 2.4 needed to have an initialization 
        // to something other than black in order to get the matrix part of the dialog 
        // going properly.
		// It was wxColour( "MEDIUM BLUE" ) but compile causes cough on both OSX and 
		// Linux/GTK at least. 
		// So changing "MEDIUM BLUE" to an octal number insteaad        
        col = wxColour( 000066 );
    }
    // Put that value in color_data for the initial color of the color dialog.        
    color_data.SetColour( col );
   
    // Tells MSW to show the full color dialog, not just the smaller one. No
    // effect on other platforms.
    color_data.SetChooseFull( TRUE );

    // Show the color dialog, using the parent of the textctrl as parent of the 
    // new color dialog
    wxColourDialog color_dialog( target_color_textctrl->GetParent(), &color_data );
    // Set the title
    color_dialog.SetTitle( _( "Choose the colour" ) );
    // Only do the action if returned hitting an OK button.
    if ( color_dialog.ShowModal() == wxID_OK ) 
    {
        wxColourData returned_color_data = color_dialog.GetColourData();
        col = returned_color_data.GetColour();
        // Convert to hex [utils_string.cpp]
        wxString output_hex_color_string = utils_string::color_to_hex_string( col );
        // Set the textbox to that hex value
        target_color_textctrl->SetValue( output_hex_color_string );
    }
    return TRUE;
}


// When a "Choose file..." button beside a filename textctrl is clicked, show
// a filename dialog, initialize to textctrl's previous value if possible, get the
// filename and  put it back in the textctrl. Return FALSE if there was a problem.
bool utils_controls::textctrl_filename_selection 
    ( 
    wxTextCtrl* target_filename_textctrl,       // TextCtrl to read/write filename(s)
    wxString message,                           // Message for the textctrl
    wxString default_file,                      // Default filename
    wxString default_directory,                 // Default directory
    wxString wildcard,                          // Wildcard filters
    bool allow_multiple,                        // Allow multiple selections
    wxChar multiple_selection_delimiter,        // Delimiter in textctrl to use to 
                                                // separate multiple files (usu ;)
    bool uses_file_protocol                     // Whether to remove a potiential "file://" or
                                                // prefix from the incoming string, then add 
                                                // to output.
    )
{


    // Extract the old filename value to override default file and default directory
    wxString old_filename_string = target_filename_textctrl->GetValue();   
    if ( allow_multiple == TRUE ) 
    {
        // Can only use the first entry to set dir/file.
        old_filename_string = old_filename_string.BeforeFirst ( multiple_selection_delimiter );
    }
    
    wxString file_protocol_prefix = wxT( "" );
    
    // It uses_file_protocol flag is on, then strip off any leading file:// or file:
    // Look for/strip off a "file://" first, then if not one, then look for/strip off 
    // a "file:" prefix next. If no file: either, then keep going. Keep the 
    // file_protocol_prefix so we can tack it on again at the end.
    if ( uses_file_protocol ) 
    {
        if ( old_filename_string.StartsWith( wxT( "file://" ), &old_filename_string ) ) 
        {
            file_protocol_prefix = wxT( "file://" );
        } 
        else 
        {
            if ( old_filename_string.StartsWith( wxT( "file:" ), &old_filename_string ) )
            {
                file_protocol_prefix = wxT( "file:" );
            }
        }    
    }       

    // Get some strings ready to split the path.
    wxString old_directory;
    wxString old_base;
    wxString old_extension;
    
    // Must check for non-null string before calling wxSplitPath
    if ( old_filename_string != wxT( "" ) )
    {
        wxSplitPath( old_filename_string, &old_directory, &old_base, &old_extension );
    }
    
    // If there was a directory part, make that default directory
    if ( old_directory != wxT( "" ) ) 
    {
        default_directory = old_directory;
    }
    
    // If was a base, make that default filename
    if ( old_base != wxT( "" ) ) 
    {
        default_file = old_base;
        // If was an extension, add a '.' and the extension to filename
        if ( old_extension != wxT( "" ) ) {
            default_file += wxT( "." ) + old_extension;
        } 
    }

#if ( setupUSE_NO_CONTROL_PERSISTANCE_HACK )
    // Stop wxFileDialog from saving to plucker.ini by setting the active config
    // to NULL.
    wxConfigBase* previous_configuration = NULL;
    previous_configuration = wxConfigBase::Set( NULL );        
#endif 

    // As of wx2.8, can only set the style on the file dialog before it is created. No later SetStyle()
	// If specified that to allow multiple, then set that flag, otherwise, leave as default.
	long style;
    if ( allow_multiple == TRUE )
    {
        style = wxMULTIPLE;
    }  
	else
	{
		style = wxFD_DEFAULT_STYLE;
	}

    wxFileDialog a_file_dialog( target_filename_textctrl->GetParent(), 
                              message,
                              default_directory,
                              default_file,
                              wildcard, 
							  style );          
                                  
    wxString output_string;
    // Show the filename dialog modally, and only do the action if returned
    // hitting an OK button.
    if ( a_file_dialog.ShowModal() == wxID_OK )
    {
        if ( allow_multiple == TRUE )
        {
            // Get the array of selections, and loop through writing to output_string 
            // and a delimiter at the end of each.
            wxArrayString selected_paths;
            a_file_dialog.GetPaths( selected_paths );
            for ( size_t i=0; i < selected_paths.Count(); i++ ) {
                output_string += selected_paths.Item( i ) + multiple_selection_delimiter;
            }
            selected_paths.Clear(); 
            // Remove the last delimiter
            output_string.RemoveLast();                 
        } 
        else 
        {
            output_string = a_file_dialog.GetPath();
        }
        
        // If flag set to load/save file:// extension, then bolt it back on.
        if ( uses_file_protocol ) 
        {
            output_string = file_protocol_prefix + output_string;
        }
        
        target_filename_textctrl->SetValue( output_string );
    }    

#if ( setupUSE_NO_CONTROL_PERSISTANCE_HACK )
    // Destroy the wxFileDialog to force saving of its settings.
    a_file_dialog.Destroy();
    // Set the active config to the previous one:
    wxConfigBase::Set( previous_configuration );
#endif
    
    return TRUE;
}


// When a "Choose directory..." button beside a filename textctrl is clicked, show
// a directory dialog, initialize to textctrl's previous value if possible, get the
// directory, and put it back in the textctrl. Return FALSE if there was a problem.
bool utils_controls::textctrl_directory_selection 
    ( 
    wxTextCtrl* target_filename_textctrl,       // TextCtrl to read/write filename(s)
    wxString message,                           // Message for the textctrl
    wxString default_directory,                 // Default directory
    bool allow_multiple,                        // Allow multiple selections
    wxChar multiple_selection_delimiter         // Delimiter in textctrl to use to 
                                                // separate multiple files (usu ;)
    )
{
    // Extract the old filename value to override default file and default directory
    wxString old_directory_string = target_filename_textctrl->GetValue();   
    if ( allow_multiple == TRUE ) 
    {
        // Can only use the first entry to set the directory
        old_directory_string = old_directory_string.BeforeFirst ( multiple_selection_delimiter );
    }
    
   
    // If there was a directory in the text control, make that default directory,
    // otherwise we will be using the default directory that we were passed (to be used
    // when there is no directory currently in the textctrl.
    if ( old_directory_string != wxT( "" ) ) 
    {
        default_directory = old_directory_string;
    }

    wxDirDialog directory_dialog( target_filename_textctrl->GetParent(), 
                                  message,
                                  default_directory );
  
    wxString output_string;
    // Show the filename dialog modally, and only do the action if returned
    // hitting an OK button.
    if ( directory_dialog.ShowModal() == wxID_OK ) 
    {
        output_string = directory_dialog.GetPath();
        target_filename_textctrl->SetValue( output_string );
    }
    
    return TRUE;
}


void utils_controls::set_html_window_text_in_gui_style( wxHtmlWindow* target_html_window,        
                                                        const wxString& source,
                                                        long options )
   
{

    wxString replaced_html_window_source;

    // These used for a text color and bgcolor inside a body tag, will 
    // strings start at empty. If the options aren't used, they will stay empty.
    wxString text_colors_attribute_string;
    wxString background_color_attribute_string;

    // Set padding from htmlwindow edges to it contents to be zero, so looks same as
    // a normal control.    
    if ( options & optionSET_HTML_WINDOW_INTERNAL_PADDING_TO_ZERO )
    {    
        target_html_window->SetBorders( 0 );
    
    }
 
    // Set the font face and size of the HTML window to match the GUI's font.
    if ( options & optionSET_HTML_FONT_FACE_AND_POINTSIZE_TO_MATCH_GUI )
    { 
        // Get face and point size of the font used for GUI controls.
        wxFont system_gui_font( *wxNORMAL_FONT ); 
        system_gui_font = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );
        wxString system_gui_font_face_name = system_gui_font.GetFaceName();
        const int system_gui_font_point_size = system_gui_font.GetPointSize();
        // Make an array of the 7 point sizes to use for each of <font size="-2">,
        // up to <font size="+4">, relative to the GUI font. We will do this separate
        // for each platform, so that a +4 in the HTLM on GTK, looks the same as a 
        // +4 on MSW.   

#ifdef __WXMSW__
        const int html_window_point_sizes[] = { system_gui_font_point_size-3,
                                                system_gui_font_point_size-2,
                                                system_gui_font_point_size, 
                                                system_gui_font_point_size+2, 
                                                system_gui_font_point_size+6,
                                                system_gui_font_point_size+12,
                                                system_gui_font_point_size+20
                                               };
#else
        const int html_window_point_sizes[] = { system_gui_font_point_size-4,
                                                system_gui_font_point_size-2,
                                                system_gui_font_point_size, 
                                                system_gui_font_point_size+2, 
                                                // wx has 5 here, but it looks fuzzy.
                                                system_gui_font_point_size+6, 
                                                system_gui_font_point_size+10,
                                                system_gui_font_point_size+18
                                               };
#endif   
        // Set font faces and size of the html window.
        // The second param is empty brackets, says to just use the default face for fixed width.
        target_html_window->SetFonts( system_gui_font_face_name, 
                                      wxT( "" ),
                                      html_window_point_sizes  
                                    );   
    } 
    
        
    if ( options & optionSET_HTML_FONT_COLOR_TO_MATCH_GUI ) 
    {    
        // Look up the system's color for text in windows. Use this for the text.
        wxColor  system_window_text_color            = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
        // Convert to a rgb string
        wxString system_window_text_color_rgb_string = utils_string::color_to_hex_string( system_window_text_color  );
        
        // Look up the system's color for highlighted items. Use this for anchors.
        wxColor  system_highlight_color           = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT );
        // Convert to a rgb string
        wxString system_highlight_color_rgb_string = utils_string::color_to_hex_string( system_highlight_color );

        // Build the attribute string to put into the body tag
        text_colors_attribute_string
            << wxT( "text='" )  << system_window_text_color_rgb_string << wxT( "' " )
            << wxT( "link='" )  << system_highlight_color_rgb_string   << wxT( "' " )
            << wxT( "vlink='" ) << system_highlight_color_rgb_string   << wxT( "' " )
            << wxT( "alink='" ) << system_highlight_color_rgb_string   << wxT( "' " );
    }

    // Set the background of the HTML window to the color of the GUI's dialog, and put
    // the HTML content in the window.
    // Note: wxWindow::SetBackgroundColour() doesn't work (the wxHtmlWindow probably
    // resets it back to white), so include teh background color inside the body tag,
    // since we are doing the text anyways.
    if ( options & optionSET_HTML_BACKGROUND_TO_MATCH_GUI_PANEL  ||
         options & optionSET_HTML_BACKGROUND_TO_MATCH_GUI_LISTBOX ) 
    {
        wxColor desired_background_color;
        if ( options & optionSET_HTML_BACKGROUND_TO_MATCH_GUI_PANEL )
        {
            desired_background_color = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
        } 
        else 
        {
            desired_background_color = wxSystemSettings::GetColour( wxSYS_COLOUR_LISTBOX );
        }

        // Look up the system's color for that widget. Use this for the background.
        // Convert to a rgb string.
        wxString desired_background_color_rgb_string = utils_string::color_to_hex_string( desired_background_color );

        // Assemble the background attribute.
        background_color_attribute_string << wxT( "bgcolor='" ) << desired_background_color_rgb_string << wxT( "'" );

    }

    // Assemble the new source inside body tags, and any attributes as required.
    replaced_html_window_source
        << wxT( "<body " ) << text_colors_attribute_string << wxT( " " ) << background_color_attribute_string << wxT( " >" )
        << source
        << wxT( "</body>" );

    // Put the new HTML content string into the HTML window.
    target_html_window->SetPage( replaced_html_window_source );

    // A debug message (commented out, since will crash BCC in debug mode. Too long?).
    // wxLogDebug( wxT( "utils_controls::set_html_window_text_in_gui_style() Source=" )
    //              + replaced_html_window_source );

}


// Sets a time choice to the time specified in a configuration key string
void utils_controls::set_choice_to_configuration_time_string( wxChoice* target_choice, 
                                                     const wxString& time_configuration_key,
                                                     const wxString& default_time_string )
{                                                   
    wxString                    input_time_string;
    wxDateTime                  output_time;
    wxDateTime::wxDateTime_t    output_hour;
    int choice_index;
    
    input_time_string = the_configuration->Read( time_configuration_key, 
                                                 default_time_string );
    output_time       = utils_datetime::w3c_string_to_time( input_time_string );
    output_hour       = output_time.GetHour();    
    choice_index      = (int)output_hour;
    
    target_choice->SetSelection( choice_index );    
    wxLogDebug( wxT( "Set a choice box to choice_index=%i" ), choice_index );
}


void utils_controls::set_configuration_time_string_to_choice_index( const wxString& time_configuration_key,
                                                    wxChoice* input_choice )
{
    int choice_index;   
    wxDateTime::wxDateTime_t output_hour;
    wxDateTime output_time = wxDateTime::Now();
    wxString output_time_string;
    
    choice_index = input_choice->GetSelection();                      
    output_hour = (wxDateTime::wxDateTime_t)choice_index;
    
    // Sets hours, minutes, seconds to zero
    output_time.ResetTime();
    output_time.SetHour( output_hour );    
    output_time_string = utils_datetime::time_to_w3c_string( output_time );
    the_configuration->Write( time_configuration_key, output_time_string );
}


void utils_controls::read_dialog_position_and_size( wxWindow* target_window,
                                    const wxString& configuration_key_basename,
                                    wxPoint default_position, wxSize default_size )
{
    wxPoint new_position;
    wxSize  new_size;
    wxPoint screen_lower_left;
    int     unused;    
    
    // Read settings from configuration
    new_position.x = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/" ) + configuration_key_basename 
                        + wxT( "_position_x" ), default_position.x );
    new_position.y = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/" ) + configuration_key_basename 
                        + wxT( "_position_y" ), default_position.y );
    new_size.x     = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/" ) + configuration_key_basename
                        + wxT( "_size_x" ), default_size.x );
    new_size.y     = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/" ) + configuration_key_basename
                        + wxT( "_size_y" ), default_size.y );

    // If the window is now totally offscreen (this could happen for example by:
    // putting the dialog all the way to the lower right corner of the screen in 800x600,
    // closing the dialog, dropping the display size down to 680x480, then reopening.
    // Also, if you close the app while the dialog is "minimized" on MSW, then its window
    // position is something like (3000,3000) [credit to Ichimusai <ichi@ichimusai.org>].
    // If would be offscreen, just rewind back to the defaults that were passed.
    // First, get the point of the lower left part of the viewable part of the screen,
    // taking into account a MSW taskbar, banner applications running on the computer, etc.
    wxClientDisplayRect( &unused, &unused, &screen_lower_left.x, &screen_lower_left.y );

    // Now if the new position would be to the left of the width of the viewable area,
    // or below the viewable area...
    if ( ( screen_lower_left.x < new_position.x ) || ( screen_lower_left.y < new_position.y ) ) 
    {
        // ...then wind back to the defaults.
        new_position = default_position;
        new_size     = default_size;
    }

    // Set the window to the new size.
	target_window->SetSize( new_size );

    // Different user-defined behaviour of how want the windows to behave.
    long window_placement_type= the_configuration->Read( wxT( "/PLUCKER_DESKTOP/window_placement_type" ), 0L );

    switch ( window_placement_type ) 
    {
        case plkrWINDOW_PLACEMENT_STYLE_CENTER_ON_PARENT:
            target_window->CenterOnParent();
            break;
    
        case plkrWINDOW_PLACEMENT_STYLE_CENTER_ON_SCREEN:

			// wxWindow somewhere between 2.4 and 2.8 now no longer has CenterOnScreen().
			// This has a similar effect:
            target_window->Center( wxBOTH | wxCENTRE_ON_SCREEN );
            break;

        case plkrWINDOW_PLACEMENT_STYLE_LAST_LOCATION:
            target_window->Move( new_position );
            break;
        default:
            wxLogDebug( wxT( "Error: unknown window_placement type" ) );
            break;
    }        
}


void utils_controls::write_dialog_position_and_size( wxWindow* target_window, 
                                        const wxString& configuration_key_basename )
{
    wxPoint new_position;
    wxSize  new_size;
    
    // Get the window position.and size
    new_position = target_window->GetPosition();
    new_size     = target_window->GetSize();    
    
    // Write settings to configuration
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/" ) + configuration_key_basename + wxT( "_position_x" ),
                           (long)new_position.x );
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/" ) + configuration_key_basename + wxT( "_position_y" ),
                           (long)new_position.y );
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/" ) + configuration_key_basename + wxT( "_size_x" ), 
                           (long)new_size.x );
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/" ) + configuration_key_basename + wxT( "_size_y" ),
                           (long)new_size.y );    
}


void utils_controls::strip_excess_separators_from_menubar( wxMenuBar* target_menubar )
{
    // Index of the menu in the menubar
    size_t      current_menu_index;
    wxMenu*     current_menu;
    // The current menuitem's node in the list of menuitems.
    // [See 'wxNode' in wxWindows class reference for a list of possible methods.]
    wxMenuItemList::Node* current_menuitem_node;
    // The current menuitem, as extracted from the node.
    wxMenuItem* current_menuitem;

    for ( current_menu_index = 0; current_menu_index < target_menubar->GetMenuCount(); current_menu_index++ ) 
    {
        // Get the current menu.
        current_menu = target_menubar->GetMenu( current_menu_index );
        // Loop through the menuitemnodes on this menu, from bottom to top. Going bottom
        // to top as the menuitem index numbers will shift to smaller numbers as menuitems are deleted.
        wxLogDebug( wxT( "Number of menuitems in current_menu is %d" ), (int)current_menu->GetMenuItemCount() );
        current_menuitem_node = current_menu->GetMenuItems().GetLast();
        while ( current_menuitem_node ) 
        {
            current_menuitem = current_menuitem_node->GetData();
            // If a separator...
            if ( current_menuitem->IsSeparator() ) 
            {
                // ..and it is....
                // ...the last menuitem in the menu....
                if ( ! current_menuitem_node->GetNext() 
                    // ...or the first menuitem in the menu...
                    || ! current_menuitem_node->GetPrevious()
                    // ...or the menuitem prior to it in the menu is also a separator... 
                    || current_menuitem_node->GetPrevious()->GetData()->IsSeparator() ) 
                {
                    // ...then delete it.
                    current_menu->Delete( current_menuitem );    
                }
            }
            current_menuitem_node = current_menuitem_node->GetPrevious();
        }
     }     
}
