//----------------------------------------------------------------------------------------
// Name:        preferences_dialogs.cpp
// Purpose:     Preferences dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2003/03/23
// Copyright:   (c) Robert O'Connor ( rob@medicalmnemonics.com )
// Licence:     GPL
// RCS-ID:      $Id: preferences_dialog_[with_time_controls].cpp,v 1.1 2002/09/28 19:41:58 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "preferences_dialog.h"
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

#include "preferences_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/radiobut.h"                // wxRadioButton
#include "wx/spinctrl.h"                // wxSpinCtrl
#include "wx/notebook.h"                // wxNotebook

// ---------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"              // XRC XML resouces

// ---------------------------------------------------------------------------------------

// Keep this in, even if there is no exclusion dialog compiled in.
#include "exclusion_dialog.h"
// Keep this in, even if there is no showcase compiled in.
#include "showcase_dialog.h"
#include "configuration.h"
#include "internationalization.h"
#include "utils_controls.h"
#include "utils_string.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

// Used to display the correct help page for the tab
enum {
    PREFERENCES_NOTEBOOK_PAGE_INDEX_INTERFACE           = 0, 
    PREFERENCES_NOTEBOOK_PAGE_INDEX_AUTOUPDATE,
    PREFERENCES_NOTEBOOK_PAGE_INDEX_SPIDERING, 
    PREFERENCES_NOTEBOOK_PAGE_INDEX_EDITOR,
    PREFERENCES_NOTEBOOK_PAGE_INDEX_SHOWCASE,
    PREFERENCES_NOTEBOOK_PAGE_INDEX_PROXY
};

// ID's for the time controls. Not caught with events, but don't want to accidently 
// overlap with XRC ids, and cause infinite recursion of updates.
enum {
    ID_PREFERENCES_DIALOG_LOWER_TIME_CONTROL            = wxID_HIGHEST +80,
    ID_PREFERENCES_DIALOG_UPPER_TIME_CONTROL
};

// The order must correspond to the index numbers of the entries in the XRC choice control
const wxString themes[] = { wxT( "" ),  // "" means use the default style
                            wxT( "high" )
                          };

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( preferences_dialog, wxDialog )
    EVT_UPDATE_UI( XRCID( "preferences_dialog_internationalization_enabled_checkbox" ), preferences_dialog::on_update_ui_internationalization_enabled_checkbox )
    EVT_UPDATE_UI( XRCID( "preferences_dialog_internationalization_choose_radiobutton" ), preferences_dialog::on_update_ui_internationalization_choose_radiobutton )
    EVT_BUTTON( XRCID( "preferences_dialog_internationalization_choose_button" ), preferences_dialog::on_internationalization_choose_button )
    EVT_UPDATE_UI( XRCID( "preferences_dialog_startup_tips_checkbox" ), preferences_dialog::on_update_ui_startup_tips_checkbox )
        
    EVT_UPDATE_UI( XRCID( "preferences_dialog_autoupdate_mode_deferred_radiobutton" ), preferences_dialog::on_update_ui_autoupdate_mode_deferred_radiobutton )

    EVT_UPDATE_UI( XRCID( "preferences_dialog_command_before_group_checkbox" ), preferences_dialog::on_update_ui_command_before_group_checkbox)
    EVT_UPDATE_UI( XRCID( "preferences_dialog_command_after_group_checkbox" ), preferences_dialog::on_update_ui_command_after_group_checkbox )
    EVT_BUTTON( XRCID( "preferences_dialog_exclusion_lists_button" ), preferences_dialog::on_exclusion_lists_button )

    EVT_UPDATE_UI( XRCID( "preferences_dialog_builtin_editor_radiobutton" ), preferences_dialog::on_update_ui_builtin_editor_radiobutton )
    EVT_UPDATE_UI( XRCID( "preferences_dialog_external_editor_radiobutton" ), preferences_dialog::on_update_ui_external_editor_radiobutton )
    EVT_BUTTON( XRCID( "preferences_dialog_external_editor_button" ), preferences_dialog::on_external_editor_button )

    EVT_BUTTON( XRCID( "preferences_dialog_showcase_details_button" ), preferences_dialog::on_showcase_details_button )

    EVT_UPDATE_UI( XRCID( "preferences_dialog_use_proxy_checkbox" ), preferences_dialog::on_update_ui_use_proxy_checkbox )

    EVT_BUTTON( wxID_OK, preferences_dialog::OnOK )
    EVT_BUTTON( wxID_HELP_CONTEXT, preferences_dialog::on_help_button )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor
preferences_dialog::preferences_dialog( wxWindow* parent )
{
    wxXmlResource::Get()->LoadDialog( this, parent, "preferences_dialog" );

#if ( ! setupUSE_SHOWCASE_PREVIEW_PANE )
    // If not using the preview, then hide and disable the images checkbox
    // TODO: is enable(false) needed, when already using a show(false)?
    XRCCTRL( *this, "preferences_dialog_showcase_images_checkbox", wxCheckBox )->Enable( FALSE );
    XRCCTRL( *this, "preferences_dialog_showcase_images_checkbox", wxCheckBox )->Show( FALSE );
#endif

    // Attach time controls to their XRC "unknown" placeholders.
    m_autoupdate_lower_time_control = new time_control( this, ID_PREFERENCES_DIALOG_LOWER_TIME_CONTROL  );
    wxXmlResource::Get()->AttachUnknownControl( "preferences_dialog_autoupdate_lower_time_control",
                                                m_autoupdate_lower_time_control );

    m_autoupdate_upper_time_control = new time_control( this, ID_PREFERENCES_DIALOG_UPPER_TIME_CONTROL  );
    wxXmlResource::Get()->AttachUnknownControl( "preferences_dialog_autoupdate_upper_time_control",
                                                m_autoupdate_upper_time_control );

    wxLogDebug( "Attached custom time_controls" );

    // Read the configuration file to set up the values for the controls.
    read_configuration();

    // Set the size, position of the frame using our common function for this dialog
    // [utils_controls.cpp]
    utils_controls::read_dialog_position_and_size( this, "preferences_dialog" );
}

// Destructor
preferences_dialog::~preferences_dialog()
{
    // Save the size and position of the dialog for next time
    // [utils_controls.cpp]
    utils_controls::write_dialog_position_and_size( this, "preferences_dialog" );
}


// Read and set the saved values for the controls from the channel-specific section of
// the configuration file
void preferences_dialog::read_configuration()
{
    
    //------Interface Tab----------------------------------------------------------------
    
    XRCCTRL( *this, "preferences_dialog_internationalization_enabled_checkbox", wxCheckBox )
        ->SetValue( (bool)the_configuration->Read( "/PLUCKER_DESKTOP/internationalization_enabled", 0L ) );
       
    long locale_wx_number;    
    locale_wx_number = the_configuration->Read( "/PLUCKER_DESKTOP/locale_wx_number", 0L  );    
    if ( locale_wx_number == 0 ) {
        XRCCTRL( *this, "preferences_dialog_internationalization_autodetect_radiobutton", wxRadioButton )
            ->SetValue( TRUE );
    } else {
        XRCCTRL( *this, "preferences_dialog_internationalization_choose_radiobutton", wxRadioButton )
            ->SetValue( TRUE );
    }
    
    XRCCTRL( *this, "preferences_dialog_show_toolbar_checkbox", wxCheckBox )
        ->SetValue( (bool)the_configuration->Read( "/PLUCKER_DESKTOP/main_frame_toolbar_enabled", 1L ) );
    
    wxString theme;    
    theme = the_configuration->Read( "/PLUCKER_DESKTOP/theme", wxT( "" ) );
    // Initialize the browser choice to the default
    wxLogDebug( "Theme read from configuration=" + theme );
    XRCCTRL( *this, "preferences_dialog_theme_choice", wxChoice )->SetSelection( 0 ); 
    for ( int n=0; n < (int)WXSIZEOF( themes ); n++ ) {
        wxLogDebug( "Looking to see if theme matches array entry=" + themes[ n ] );
        if ( theme == themes[ n ] ) {
        XRCCTRL( *this, "preferences_dialog_theme_choice", wxChoice )
            ->SetSelection( n );
        wxLogDebug( "Found in array at index %d, so setting choice to match", n );
        break;
        }
    } 
    
    XRCCTRL( *this, "preferences_dialog_show_statusbar_checkbox", wxCheckBox )
        ->SetValue( (bool)the_configuration->Read( "/PLUCKER_DESKTOP/main_frame_statusbar_enabled", 1L ) );

    XRCCTRL( *this, "preferences_dialog_window_placement_choice", wxChoice )
        ->SetSelection( (int)the_configuration->Read( "/PLUCKER_DESKTOP/window_placement_type", 0L ) );     
  
    XRCCTRL( *this, "preferences_dialog_startup_tips_checkbox", wxCheckBox )
        ->SetValue( (bool)the_configuration->Read( "/PLUCKER_DESKTOP/help_startup_tips_enabled", 1L ) );

    XRCCTRL( *this, "preferences_dialog_startup_tips_choice", wxChoice )
        ->SetSelection( (int)the_configuration->Read( "/PLUCKER_DESKTOP/help_startup_tips_type", 0L ) );     

    XRCCTRL( *this, "preferences_dialog_splashscreen_checkbox", wxCheckBox )
        ->SetValue( (bool)the_configuration->Read( "/PLUCKER_DESKTOP/splashscreen_enabled", 1L ) );

    wxLogDebug ( "Finished reading preferences dialog inferface preferences" );

    //------Autoupdate Tab-------------------------------------------------------------------
    
    long autoupdate_mode;
    autoupdate_mode = (long)the_configuration->Read( "/PLUCKER_DAEMON/autoupdate_mode", 0L );
    switch ( autoupdate_mode ) {
        case plkrAUTOUPDATE_MODE_NEVER:
            XRCCTRL( *this, "preferences_dialog_autoupdate_mode_never_radiobutton", wxRadioButton )
                ->SetValue( TRUE );
            break;
        case plkrAUTOUPDATE_MODE_IMMEDIATELY:
            XRCCTRL( *this, "preferences_dialog_autoupdate_mode_immediately_radiobutton", wxRadioButton )
                ->SetValue( TRUE );
            break;
        case plkrAUTOUPDATE_MODE_DEFERRED:
            XRCCTRL( *this, "preferences_dialog_autoupdate_mode_deferred_radiobutton", wxRadioButton )
                ->SetValue( TRUE );
            break;
        case plkrAUTOUPDATE_MODE_ON_SYNC:
            XRCCTRL( *this, "preferences_dialog_autoupdate_mode_on_sync_radiobutton", wxRadioButton )
                ->SetValue( TRUE );
            break;
    }        
    wxLogDebug( "Set autoupdate_mode" );

    //[utils_controls.cpp]
    utils_controls::set_time_control_to_configuration_time_string( m_autoupdate_lower_time_control,
                       "/PLUCKER_DAEMON/autoupdate_deferred_lower_time_limit", plkrAUTOUPDATE_DEFAULT_DEFERRED_LOWER_TIME_LIMIT );

    //[utils_controls.cpp]
    utils_controls::set_time_control_to_configuration_time_string( m_autoupdate_upper_time_control,
                       "/PLUCKER_DAEMON/autoupdate_deferred_upper_time_limit", plkrAUTOUPDATE_DEFAULT_DEFERRED_UPPER_TIME_LIMIT );


    long autoupdate_precision = the_configuration->Read( "/PLUCKER_DAEMON/autoupdate_precision", 10L );
    XRCCTRL( *this, "preferences_dialog_autoupdate_precision_spinctrl", wxSpinCtrl )
        ->SetValue( (int)autoupdate_precision ); 
        
    wxLogDebug ( "Finished reading autoupdate preferences" );

    //------Spidering Tab-------------------------------------------------------------------
    
    int spidering_display_choice_index;
    wxString spidering_display_mode;
    spidering_display_mode = the_configuration->Read( "/PLUCKER_DESKTOP/spidering_display_mode",
                                                      "dialog" );
    if ( spidering_display_mode == "console" ) {
        spidering_display_choice_index = 0;
    } else {
        spidering_display_choice_index = 1;
    }
    XRCCTRL( *this, "preferences_dialog_progress_choice", wxChoice )
        ->SetSelection( spidering_display_choice_index );
    
    int autoclose_choice_index;   
    bool close_on_exit;
    bool close_on_error;       
    // Take which keys to read depending on what display_choice was marked above
    if ( spidering_display_mode == "console" ) {        
        close_on_exit  = (bool)the_configuration->Read( "/" + 
            get_os_configuration_section() + "/" + "close_on_exit", 0L );
        close_on_error = (bool)the_configuration->Read( "/" + 
            get_os_configuration_section() + "/" + "close_on_error", 0L );
    } else {
        close_on_exit  = (bool)the_configuration->Read( "/PLUCKER_DESKTOP/progress_dialog_close_on_exit", 0L );
        close_on_error = (bool)the_configuration->Read( "/PLUCKER_DESKTOP/progress_dialog_close_on_error", 0L );
    }
    // Select the appropriate index    
    if ( close_on_exit && close_on_error ) {
        autoclose_choice_index = 0;
    } else if ( close_on_exit && ! close_on_error ) {
        autoclose_choice_index = 1;
    } else {
        autoclose_choice_index = 2;
    }    
    XRCCTRL( *this, "preferences_dialog_autoclose_progress_choice", wxChoice )
            ->SetSelection( autoclose_choice_index );

    wxString before_group_command;
    before_group_command = the_configuration->Read( "/PLUCKER_DESKTOP/before_group_command", wxT( "" ) );
    XRCCTRL( *this, "preferences_dialog_command_before_group_textctrl", wxTextCtrl )
        ->SetValue( before_group_command );
    if ( before_group_command != wxT( "" ) ) {
        XRCCTRL( *this, "preferences_dialog_command_before_group_checkbox", wxCheckBox )
            ->SetValue ( TRUE );
    }

    wxString after_group_command;
    after_group_command = the_configuration->Read( "/PLUCKER_DESKTOP/after_group_command", wxT( "" ) );
    XRCCTRL( *this, "preferences_dialog_command_after_group_textctrl", wxTextCtrl )
        ->SetValue( after_group_command );
    if ( after_group_command != wxT( "" ) ) {
        XRCCTRL( *this, "preferences_dialog_command_after_group_checkbox", wxCheckBox )
            ->SetValue ( TRUE );
    }

    wxLogDebug ( "Finished reading preferences dialog spidering preferences" );

    //------Editor Tab----------------------------------------------------------------------
 
    if ( the_configuration->Read( "/PLUCKER_DESKTOP/html_editor", wxT( "" ) ) == "" ) {
        XRCCTRL( *this, "preferences_dialog_builtin_editor_radiobutton", wxRadioButton )
            ->SetValue( TRUE );
    } else {
        XRCCTRL( *this, "preferences_dialog_external_editor_radiobutton", wxRadioButton )
            ->SetValue( TRUE );
    }    
    XRCCTRL( *this, "preferences_dialog_external_editor_textctrl", wxTextCtrl )
        ->SetValue( the_configuration->Read( "/PLUCKER_DESKTOP/html_editor", wxT( "" ) ) );    
                    
    // Basic toolset is choice index 0, Advanced toolset is choice index 1   
    int toolset_type = the_configuration->Read( "/PLUCKER_DESKTOP/editor_use_advanced_toolbars", 0L );
    XRCCTRL( *this, "preferences_dialog_editor_toolbar_choice", wxChoice )
        ->SetSelection( toolset_type );      
            
    XRCCTRL( *this, "preferences_dialog_editor_insert_linefeeds_checkbox", wxCheckBox )
        ->SetValue( (bool)the_configuration->Read( "/PLUCKER_DESKTOP/editor_tools_insert_linefeeds", 0L ) );

    wxLogDebug( "Finished reading preferences dialog editor preferences" );

    //------Showcase Tab--------------------------------------------------------------------
        
    // Horizontal layout is choice index 0, Vertical layout is choice index 1
    int layout = the_configuration->Read( "/PLUCKER_DESKTOP/showcase_dialog_layout_type", 0L );     
    XRCCTRL( *this, "preferences_dialog_showcase_layout_choice", wxChoice )
        ->SetSelection( layout );    
    
    // Name/URL only is choice index 0, All fields is choice index 1
    int insert_all_parameters = the_configuration->Read( "/PLUCKER_DESKTOP/showcase_dialog_insert_all_parameters", 1L );     
    XRCCTRL( *this, "preferences_dialog_showcase_insert_all_parameters_choice", wxChoice )
        ->SetSelection( insert_all_parameters );
        
    XRCCTRL( *this, "preferences_dialog_showcase_images_checkbox", wxCheckBox )
        ->SetValue( (bool)the_configuration->Read( "/PLUCKER_DESKTOP/showcase_dialog_show_images", 0L ) );        

    wxLogDebug( "Finished reading preferences dialog showcase preferences" );
    
    //------Proxy Tab----------------------------------------------------------------------
    
    wxString http_proxy;
    http_proxy = the_configuration->Read( "/DEFAULT/http_proxy", wxT( "" ) );
    // Check the 'Use proxy' checkbox if there is some values for http_proxy.
    if ( http_proxy != "") {
        XRCCTRL( *this, "preferences_dialog_use_proxy_checkbox", wxCheckBox )
        ->SetValue( TRUE );
    }
    // Want to parse out the http://www.myproxy.com:port into separate boxes for URL and
    // port number, since that is the better GUI approach. 
    // Start by seeing if there is 2 ':' in the address
    if ( http_proxy.Freq(':') != 1 ) {
        // Fill the server port with everything before the last ':' (takes care of an http:
        // but also https: case).
        XRCCTRL( *this, "preferences_dialog_proxy_server_textctrl", wxTextCtrl )
            ->SetValue( http_proxy.BeforeLast(':') );
        // Port is everything after the second ':'.    
        XRCCTRL( *this, "preferences_dialog_proxy_port_textctrl", wxTextCtrl )
            ->SetValue( http_proxy.AfterLast(':') ); 
    // Else just have a http://www.myproxy.com address, or some kind of bad URL.
    // Stick it all into the server box.
    } else {    
        XRCCTRL( *this, "preferences_dialog_proxy_server_textctrl", wxTextCtrl )
            ->SetValue( http_proxy );    
    }     
    
    XRCCTRL( *this, "preferences_dialog_proxy_login_textctrl", wxTextCtrl )
        ->SetValue( the_configuration->Read( "/DEFAULT/http_proxy_user", wxT( "" ) ) );
    
    XRCCTRL( *this, "preferences_dialog_proxy_password_textctrl", wxTextCtrl )
        ->SetValue( the_configuration->Read( "/DEFAULT/http_proxy_pass", wxT( "" ) ) );
        
    wxLogDebug( "Finished reading preference dialog proxy preferences" );    
}


// Write out the values of the dialog's controls to the channel-specific section of
// the configuration file
void preferences_dialog::write_configuration()
{      
    //------Interface Tab-------------------------------------------------------------------  

    the_configuration->Write( "/PLUCKER_DESKTOP/internationalization_enabled",
        (bool) XRCCTRL( *this, "preferences_dialog_internationalization_enabled_checkbox", wxCheckBox )->GetValue() );
    
    if ( XRCCTRL( *this, "preferences_dialog_internationalization_autodetect_radiobutton", wxRadioButton )
            ->GetValue() == TRUE ) {
        the_configuration->Write( "/PLUCKER_DESKTOP/locale_wx_number", 0L );  
    }

    // If took the radiobutton to force a language, then it was already written when clicked 
    // OK on the popup list of languages.
    
   the_configuration->Write( "/PLUCKER_DESKTOP/main_frame_toolbar_enabled",
        (bool)XRCCTRL( *this, "preferences_dialog_show_toolbar_checkbox", wxCheckBox )->GetValue() );
      
    wxString theme;  
    int theme_choice_index;  
    theme_choice_index= XRCCTRL( *this, "preferences_dialog_theme_choice", wxChoice )->GetSelection();
    theme = themes[ theme_choice_index ];
    the_configuration->Write( "/PLUCKER_DESKTOP/theme", theme );   
     
    the_configuration->Write( "/PLUCKER_DESKTOP/main_frame_statusbar_enabled",
        (bool)XRCCTRL( *this, "preferences_dialog_show_statusbar_checkbox", wxCheckBox )->GetValue() );

    the_configuration->Write( "/PLUCKER_DESKTOP/window_placement_type", 
        (long)XRCCTRL( *this, "preferences_dialog_window_placement_choice", wxChoice )->GetSelection() );
        
    the_configuration->Write( "/PLUCKER_DESKTOP/help_startup_tips_enabled",
        (bool)XRCCTRL( *this, "preferences_dialog_startup_tips_checkbox", wxCheckBox )->GetValue() );

    the_configuration->Write( "/PLUCKER_DESKTOP/help_startup_tips_type", 
        (long)XRCCTRL( *this, "preferences_dialog_startup_tips_choice", wxChoice )->GetSelection() );

    the_configuration->Write( "/PLUCKER_DESKTOP/splashscreen_enabled",
        (bool)XRCCTRL( *this, "preferences_dialog_splashscreen_checkbox", wxCheckBox )->GetValue() );

    wxLogDebug( "Finished writing preferences dialog interface preferences" );
    
    //------Autoupdate Mode-------------------------------------------------------------------  

    long autoupdate_mode;    
    if ( XRCCTRL( *this, "preferences_dialog_autoupdate_mode_never_radiobutton", wxRadioButton )->GetValue() ) 
        autoupdate_mode = plkrAUTOUPDATE_MODE_NEVER;
    else if ( XRCCTRL( *this, "preferences_dialog_autoupdate_mode_immediately_radiobutton", wxRadioButton )->GetValue() )
        autoupdate_mode = plkrAUTOUPDATE_MODE_IMMEDIATELY;
    else if ( XRCCTRL( *this, "preferences_dialog_autoupdate_mode_deferred_radiobutton", wxRadioButton )->GetValue() ) 
        autoupdate_mode = plkrAUTOUPDATE_MODE_DEFERRED; 
    else 
        autoupdate_mode = plkrAUTOUPDATE_MODE_ON_SYNC;
    the_configuration->Write( "/PLUCKER_DAEMON/autoupdate_mode", autoupdate_mode );       
              
    wxLogDebug( "Wrote autoupdate_mode" );
 
    // [utils_controls.cpp]
    utils_controls::set_configuration_time_string_to_time_control( "/PLUCKER_DAEMON/autoupdate_deferred_lower_time_limit",
           m_autoupdate_lower_time_control );

    // [utils_controls.cpp]
    utils_controls::set_configuration_time_string_to_time_control( "/PLUCKER_DAEMON/autoupdate_deferred_upper_time_limit",
           m_autoupdate_upper_time_control );

    wxLogDebug( "Wrote autoupdate upper and lower limits" );

    int autoupdate_precision = XRCCTRL( *this, "preferences_dialog_autoupdate_precision_spinctrl", wxSpinCtrl )
                                   ->GetValue();
    // Error correct if they put in a zero or a negative number
    if ( autoupdate_precision < 1 ) {
        autoupdate_precision = 1;
    }
    the_configuration->Write( "/PLUCKER_DAEMON/autoupdate_precision", (long)autoupdate_precision );
      
    wxLogDebug( "Finished writing autoupdate preferences" );
        
    //------Spidering Tab----------------------------------------------------------------

    int spidering_display_choice_index;
    wxString spidering_display_mode;    
    spidering_display_choice_index = XRCCTRL( *this, "preferences_dialog_progress_choice", wxChoice )
        ->GetSelection();    
    switch ( spidering_display_choice_index ) {
        case 0:
            spidering_display_mode = "console";
            break;        
        case 1:
            spidering_display_mode = "dialog";
            break;
    }
    the_configuration->Write( "/PLUCKER_DESKTOP/spidering_display_mode", 
                              spidering_display_mode );    
        
    int autoclose_choice_index;   
    bool close_on_exit  = FALSE;
    bool close_on_error = FALSE;    
    autoclose_choice_index = XRCCTRL( *this, "preferences_dialog_autoclose_progress_choice", wxChoice )
            ->GetSelection();
    switch ( autoclose_choice_index ) {
        case 0:
            close_on_exit   = 1;
            close_on_error  = 1;
            break; 
        case 1:
            close_on_exit   = 1;
            close_on_error  = 0;
            break; 
        case 2:
            close_on_exit   = 0;
            close_on_error  = 0;
            break; 
    }
    // Just write both console and progress dialog keys, regardless of which they chose above
    the_configuration->Write( "/" + get_os_configuration_section() + "/" + "close_on_exit",
                              close_on_exit );
    the_configuration->Write( "/" + get_os_configuration_section() + "/" + "close_on_error",
                              close_on_error );
    the_configuration->Write( "/PLUCKER_DESKTOP/progress_dialog_close_on_exit", close_on_exit );
    the_configuration->Write( "/PLUCKER_DESKTOP/progress_dialog_close_on_error", close_on_error );

    wxString before_group_command;
    before_group_command = XRCCTRL( *this, "preferences_dialog_command_before_group_textctrl", wxTextCtrl )->GetValue(); 
    if ( XRCCTRL( *this, "preferences_dialog_command_before_group_checkbox", wxCheckBox )
            ->GetValue() == FALSE ) {
        before_group_command = wxT( "" );
    }
    the_configuration->Write( "/PLUCKER_DESKTOP/before_group_command", before_group_command );
    
    wxString after_group_command;
    after_group_command = XRCCTRL( *this, "preferences_dialog_command_after_group_textctrl", wxTextCtrl )->GetValue(); 
    if ( XRCCTRL( *this, "preferences_dialog_command_after_group_checkbox", wxCheckBox )
            ->GetValue() == FALSE ) {
        after_group_command = wxT( "" );
    }     
    the_configuration->Write( "/PLUCKER_DESKTOP/after_group_command", after_group_command );

    wxLogDebug( "Finished writing preferences dialog spidering preferences" );
    
    //------Editor Tab----------------------------------------------------------------

    if ( XRCCTRL( *this, "preferences_dialog_builtin_editor_radiobutton", wxRadioButton )
            ->GetValue() == TRUE ) {
        the_configuration->Write( "/PLUCKER_DESKTOP/html_editor", wxT( "" ) );   
    } else {
        the_configuration->Write ( "/PLUCKER_DESKTOP/html_editor",
            XRCCTRL( *this, "preferences_dialog_external_editor_textctrl", wxTextCtrl )->GetValue() );   
    }
    
    // Basic toolset is choice index 0, advancedis choice index 1, so just cast to boolean directly.
    the_configuration->Write( "/PLUCKER_DESKTOP/editor_use_advanced_toolbars", 
        (bool) XRCCTRL( *this, "preferences_dialog_editor_toolbar_choice", wxChoice )->GetSelection() );     
    
    the_configuration->Write( "/PLUCKER_DESKTOP/editor_tools_insert_linefeeds",
        (bool) XRCCTRL( *this, "preferences_dialog_editor_insert_linefeeds_checkbox", wxCheckBox )->GetValue() );

    wxLogDebug ( "Finished writing preferences dialog editor preferences" );
  
    //------Showcase Tab-------------------------------------------------------------
        
    // Horizontal layout is choice index 0, vertical layout is index 1, so just cast to boolean directly.
    the_configuration->Write( "/PLUCKER_DESKTOP/showcase_dialog_layout_type", 
        (bool) XRCCTRL( *this, "preferences_dialog_showcase_layout_choice", wxChoice )->GetSelection() );     
   
    // Name/URL only is choice index 0, all parameters is index 1, so just cast to boolean directly.
    the_configuration->Write( "/PLUCKER_DESKTOP/showcase_dialog_insert_all_parameters", 
        (bool) XRCCTRL( *this, "preferences_dialog_showcase_insert_all_parameters_choice", wxChoice )->GetSelection() );     
 
    the_configuration->Write( "/PLUCKER_DESKTOP/showcase_dialog_show_images",
        (bool) XRCCTRL( *this, "preferences_dialog_showcase_images_checkbox", wxCheckBox )->GetValue() );

    wxLogDebug ( "Finished writing preferences dialog showcase preferences" ); 
    
    //------Proxy Tab=======---------------------------------------------------------
    
    wxString http_proxy;
    
    // If 'use proxy' checkbox is checked, write out the info, else write out blanks
    if ( XRCCTRL( *this, "preferences_dialog_use_proxy_checkbox", wxCheckBox )
            ->GetValue() == TRUE ) {        
        http_proxy = XRCCTRL( *this, "preferences_dialog_proxy_server_textctrl", wxTextCtrl )->GetValue();              
        if ( XRCCTRL( *this, "preferences_dialog_proxy_port_textctrl", wxTextCtrl )->GetValue() != "") {
            http_proxy += ( ":" + XRCCTRL( *this, "preferences_dialog_proxy_port_textctrl", wxTextCtrl )->GetValue() );
        }
        the_configuration->Write( "/DEFAULT/http_proxy", http_proxy );
                 
        the_configuration->Write( "/DEFAULT/http_proxy_user", 
            XRCCTRL( *this, "preferences_dialog_proxy_login_textctrl", wxTextCtrl )->GetValue() );
           
        the_configuration->Write( "/DEFAULT/http_proxy_pass", 
            XRCCTRL( *this, "preferences_dialog_proxy_password_textctrl", wxTextCtrl )->GetValue() );        
    } else {
        the_configuration->Write( "/DEFAULT/http_proxy", wxT( "" ) );
        
        the_configuration->Write( "/DEFAULT/http_proxy_user", wxT( "" ) );
        
        the_configuration->Write( "/DEFAULT/http_proxy_pass", wxT( "" ) );             
    }

    wxLogDebug ( "Finished writing proxy preferences" );

    //----------------------------------------------------------------------------------
    // Flush() writes the things to the file immediately, instead of waiting for program 
    // to exit to write them.
    the_configuration->Flush();

}


//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void preferences_dialog::on_update_ui_internationalization_enabled_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "preferences_dialog_internationalization_enabled_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "preferences_dialog_internationalization_autodetect_radiobutton", wxRadioButton )
            ->Enable( enabled );
    XRCCTRL( *this, "preferences_dialog_internationalization_choose_radiobutton", wxRadioButton )
            ->Enable( enabled );
}


void preferences_dialog::on_update_ui_internationalization_choose_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
    
    // Only enable children if this radiobutton is both checked and enabled    
    enabled = ( XRCCTRL( *this, "preferences_dialog_internationalization_choose_radiobutton", wxRadioButton )
                ->GetValue() &&
               XRCCTRL( *this, "preferences_dialog_internationalization_choose_radiobutton", wxRadioButton )
                ->IsEnabled() );
    XRCCTRL( *this, "preferences_dialog_internationalization_choose_button", wxButton )
        ->Enable( enabled );
}


void preferences_dialog::on_update_ui_startup_tips_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "preferences_dialog_startup_tips_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "preferences_dialog_startup_tips_choice", wxChoice )
            ->Enable( enabled );
}


void preferences_dialog::on_update_ui_autoupdate_mode_deferred_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;

    enabled = XRCCTRL( *this, "preferences_dialog_autoupdate_mode_deferred_radiobutton", wxRadioButton )
            ->GetValue();
    m_autoupdate_lower_time_control->enable( enabled );
    m_autoupdate_upper_time_control->enable( enabled );
}


void preferences_dialog::on_update_ui_command_before_group_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "preferences_dialog_command_before_group_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "preferences_dialog_command_before_group_textctrl", wxTextCtrl )
            ->Enable( enabled );
}


void preferences_dialog::on_update_ui_command_after_group_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "preferences_dialog_command_after_group_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "preferences_dialog_command_after_group_textctrl", wxTextCtrl )
            ->Enable( enabled );
}


void preferences_dialog::on_update_ui_builtin_editor_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "preferences_dialog_builtin_editor_radiobutton", wxRadioButton )
            ->GetValue();
    XRCCTRL( *this, "preferences_dialog_editor_toolbar_choice", wxChoice )
            ->Enable( enabled );
    XRCCTRL( *this, "preferences_dialog_editor_insert_linefeeds_checkbox", wxCheckBox )
            ->Enable( enabled );
}


void preferences_dialog::on_update_ui_external_editor_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "preferences_dialog_external_editor_radiobutton", wxRadioButton )
            ->GetValue();
    XRCCTRL( *this, "preferences_dialog_external_editor_textctrl", wxTextCtrl )
            ->Enable( enabled );
    XRCCTRL( *this, "preferences_dialog_external_editor_button", wxButton )
            ->Enable( enabled );
}


void preferences_dialog::on_internationalization_choose_button( wxCommandEvent &event )
{
#if ( setupUSE_INTERNATIONALIZATION )

    int locale_wx_number;

    locale_wx_number = internationalization::get_wx_locale_number_from_choice_dialog(); 
    the_configuration->Write( "/PLUCKER_DESKTOP/locale_wx_number", (long) locale_wx_number );

#endif
}


void preferences_dialog::on_external_editor_button( wxCommandEvent &event )
{
    // [utils_controls.cpp]    
    utils_controls::textctrl_filename_selection( XRCCTRL( *this, "preferences_dialog_external_editor_textctrl", wxTextCtrl ),
                                 _( "Choose external editor" ),
                                 "",
                                 "",
                                 FILE_DIALOG_WILDCARDS_EXECUTABLES,
                                 false,
                                 plkrOS_FILE_ARRAY_SEPARATOR,
                                 true ); 
}


void preferences_dialog::on_showcase_details_button( wxCommandEvent &event )
{ 
#if ( setupUSE_SHOWCASE )  
  
    // Make the array of listbox indexes to have selected
    wxArrayInt selected_index_numbers;
    wxString showcase_details_fields_string = the_configuration->Read( "/PLUCKER_DESKTOP/showcase_dialog_details_fields", "7" );    
     
    // Converts the read string to an array of integers.
    utils_string::string_to_arrayint( showcase_details_fields_string, ';', &selected_index_numbers, true );
  
    //! \todo This should be shared somehow with the showcase dialog.
    const wxString showcase_field_names[] = { _( "Name" ), 
                                              _( "URL" ), 
                                              _( "Description" ),
                                              _( "Language" ), 
                                              _( "Category" ), 
                                              _( "Update Frequency" ),
                                              _( "Update Period" ),
                                              _( "Update Base" ),
                                              _( "Update Enabled" ), 
                                              _( "Image Depth" ), 
                                              _( "Maximum Depth" ), 
                                              _( "Stay On Host" ),                                           
                                              _( "Stay On Domain"),
                                              _( "URL Pattern" )
                                            };
  
    // Show the dialog 
    size_t number_of_selected_items = wxGetMultipleChoices( selected_index_numbers,
                                        _( "Fields to display in details pane:" ),
                                        _( "Select desired fields" ),
                                        WXSIZEOF( showcase_field_names ), showcase_field_names,
                                        this );
    if ( number_of_selected_items != 0 )  {
        wxString details_pane_configuration_string;
        for ( size_t n = 0; n < number_of_selected_items; n++ ) {
            details_pane_configuration_string += wxString::Format( wxT( "%u;" ), selected_index_numbers[ n ] );
        }
        // Strips off the trailing ';'
        details_pane_configuration_string.RemoveLast();
        wxLogDebug( "Wrote out showcase_dialog_details_fields to config as=" + details_pane_configuration_string );
        the_configuration->Write( "/PLUCKER_DESKTOP/showcase_dialog_details_fields", details_pane_configuration_string );
        the_configuration->Flush();
    }
    
    selected_index_numbers.Clear();
#endif
}


void preferences_dialog::on_exclusion_lists_button( wxCommandEvent &event )
{
#if ( setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR )
    // Boot up an exclusion dialog, showing the shared exclusion list.
    exclusion_dialog a_exclusion_dialog( this, "DEFAULT" );
    a_exclusion_dialog.ShowModal();
#endif
}


void preferences_dialog::on_update_ui_use_proxy_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "preferences_dialog_use_proxy_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "preferences_dialog_proxy_server_textctrl", wxTextCtrl )
            ->Enable( enabled );
    XRCCTRL( *this, "preferences_dialog_proxy_port_textctrl", wxTextCtrl )
            ->Enable( enabled );
    XRCCTRL( *this, "preferences_dialog_proxy_login_textctrl", wxTextCtrl )
            ->Enable( enabled );
    XRCCTRL( *this, "preferences_dialog_proxy_password_textctrl", wxTextCtrl )
            ->Enable( enabled );      
}


// Override wxDialog's default behavior for clicking an OK button.
void preferences_dialog::OnOK( wxCommandEvent& event )
{
    // Write out the values of the dialog's controls to the configuration file.
    write_configuration();
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow. TODO: could return whether or not the list control
    // needs to be redrawn (since changed channel name or update time), or use a global.
    EndModal( wxID_OK );
}


void preferences_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    int selected_notebook_page_index;
    
    selected_notebook_page_index  = XRCCTRL( *this, "preferences_dialog_notebook", wxNotebook )
                                        ->GetSelection();    

    switch ( selected_notebook_page_index ) {       
        case PREFERENCES_NOTEBOOK_PAGE_INDEX_INTERFACE:
            help_controller::get()->show_help_topic( plkrHELP_ID_PREFERENCES_DIALOG_INTERFACE_TAB );
            break;
        case PREFERENCES_NOTEBOOK_PAGE_INDEX_AUTOUPDATE:
            help_controller::get()->show_help_topic( plkrHELP_ID_PREFERENCES_DIALOG_AUTOUPDATE_TAB );
            break;
        case PREFERENCES_NOTEBOOK_PAGE_INDEX_SPIDERING:
            help_controller::get()->show_help_topic( plkrHELP_ID_PREFERENCES_DIALOG_SPIDERING_TAB );
            break;
        case PREFERENCES_NOTEBOOK_PAGE_INDEX_EDITOR:
            help_controller::get()->show_help_topic( plkrHELP_ID_PREFERENCES_DIALOG_EDITOR_TAB );
            break;
        case PREFERENCES_NOTEBOOK_PAGE_INDEX_SHOWCASE:
            help_controller::get()->show_help_topic( plkrHELP_ID_PREFERENCES_DIALOG_SHOWCASE_TAB );
            break;
        case PREFERENCES_NOTEBOOK_PAGE_INDEX_PROXY:
            help_controller::get()->show_help_topic( plkrHELP_ID_PREFERENCES_DIALOG_PROXY_TAB );
            break;    
        default:
            break;
    }
    wxLogDebug( "Selected notebook page index=%d", selected_notebook_page_index );       
    
#endif 
}

