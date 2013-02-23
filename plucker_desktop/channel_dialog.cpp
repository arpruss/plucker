//----------------------------------------------------------------------------------------
// Name:        channel_dialogs.cpp
// Purpose:     Configure channel dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: channel_dialog.cpp,v 1.69 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "channel_dialog.h"
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

#include "channel_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/radiobut.h"                // wxRadioButton
#include "wx/spinctrl.h"                // wxSpinCtrl
#include "wx/calctrl.h"                 // wxCalendarCtrl
#include "wx/msgdlg.h"                  // wxMessageDialog
#include "wx/utils.h"                   // wxExecute
#include "wx/textfile.h"                // wxTextFile
#include "wx/notebook.h"                // wxNotebook

// ---------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"              // XRC XML resouces

// ---------------------------------------------------------------------------------------

#include "configuration.h"
#include "utils_datetime.h"
#include "utils_controls.h"
#include "image_list.h"
#include "utils_string.h"
#include "showcase_dialog.h"
#include "editor_dialog.h"
#include "exclusion_dialog.h"
#include "checkbox_message_dialog.h"  // A MessageDialog with a "don't show again" option
#include "help_controller.h"
#include "handheld_dest_wrappers.h"

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

// The order must correspond to the index numbers of the entries in the XRC choice control
// Not translated, so okay to have here instead of inside class.
const wxString suggested_user_agents[] = { wxT( "" ),  // "" means use Plucker user-agent
                                           wxT( "Mozilla/3.0 (compatible; AvantGo 3.2)" )
                                         };

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( channel_dialog, wxDialog )
    EVT_UPDATE_UI( XRCID( "channel_dialog_start_tab_url_radiobutton" ), channel_dialog::on_update_ui_start_tab_url_radiobutton )
    EVT_UPDATE_UI( XRCID( "channel_dialog_start_tab_file_radiobutton" ), channel_dialog::on_update_ui_start_tab_file_radiobutton )
    EVT_UPDATE_UI( XRCID( "channel_dialog_start_tab_links_radiobutton" ), channel_dialog::on_update_ui_start_tab_links_radiobutton )
    EVT_BUTTON( XRCID( "channel_dialog_start_tab_url_button" ), channel_dialog::on_start_tab_url_button )
    EVT_UPDATE_UI( XRCID( "channel_dialog_start_tab_file_textctrl" ), channel_dialog::on_update_ui_start_tab_file_textctrl )
    EVT_BUTTON( XRCID( "channel_dialog_start_tab_file_button" ), channel_dialog::on_start_tab_file_button )
    EVT_BUTTON( XRCID( "channel_dialog_start_tab_edit_file_button" ), channel_dialog::on_start_tab_edit_file_button )
    EVT_BUTTON( XRCID( "channel_dialog_start_tab_links_button" ), channel_dialog::on_start_tab_links_button )

    EVT_UPDATE_UI( XRCID( "channel_dialog_spidering_tab_retrieval_timeout_checkbox" ), channel_dialog::on_update_ui_spidering_tab_retrieval_timeout_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_spidering_tab_status_line_length_checkbox" ), channel_dialog::on_update_ui_spidering_tab_status_line_length_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_spidering_tab_referrer_checkbox" ), channel_dialog::on_update_ui_spidering_tab_referrer_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_spidering_tab_user_agent_browser_radiobutton" ), channel_dialog::on_update_ui_spidering_tab_user_agent_browser_radiobutton )
    EVT_UPDATE_UI( XRCID( "channel_dialog_spidering_tab_user_agent_custom_radiobutton" ), channel_dialog::on_update_ui_spidering_tab_user_agent_custom_radiobutton )

    EVT_UPDATE_UI( XRCID( "channel_dialog_limits_tab_stayonhost_stayondomain_checkbox" ), channel_dialog::on_update_ui_limits_tab_stayonhost_stayondomain_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_limits_tab_url_pattern_checkbox" ), channel_dialog::on_update_ui_limits_tab_url_pattern_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_limits_tab_exclusion_checkbox" ), channel_dialog::on_update_ui_limits_tab_exclusion_checkbox )
    EVT_BUTTON( XRCID( "channel_dialog_limits_tab_exclusion_button" ), channel_dialog::on_limits_tab_exclusion_button )

    EVT_UPDATE_UI( XRCID( "channel_dialog_formatting_tab_charset_checkbox" ), channel_dialog::on_update_ui_formatting_tab_charset_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_formatting_tab_anchor_color_checkbox" ), channel_dialog::on_update_ui_formatting_tab_anchor_color_checkbox )
    EVT_BUTTON( XRCID( "channel_dialog_formatting_tab_anchor_color_button" ), channel_dialog::on_formatting_tab_anchor_color_button )

    EVT_UPDATE_UI( XRCID( "channel_dialog_images_tab_include_images_checkbox" ), channel_dialog::on_update_ui_images_tab_include_images_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_images_tab_only_link_radiobutton" ), channel_dialog::on_update_ui_images_tab_only_link_radiobutton )

    EVT_UPDATE_UI( XRCID( "channel_dialog_output_tab_only_compress_checkbox" ), channel_dialog::on_update_ui_output_tab_only_compress_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_output_tab_category_checkbox" ), channel_dialog::on_update_ui_output_tab_category_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_output_tab_owner_id_checkbox" ), channel_dialog::on_update_ui_output_tab_owner_id_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_output_tab_launchable_checkbox" ), channel_dialog::on_update_ui_output_tab_launchable_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_output_tab_custom_large_icon_checkbox" ), channel_dialog::on_update_ui_output_tab_custom_large_icon_checkbox )
    EVT_UPDATE_UI( XRCID( "channel_dialog_output_tab_custom_small_icon_checkbox" ), channel_dialog::on_update_ui_output_tab_custom_small_icon_checkbox )
    EVT_BUTTON( XRCID( "channel_dialog_output_tab_custom_small_icon_button" ), channel_dialog::on_output_tab_custom_small_icon_button )
    EVT_BUTTON( XRCID( "channel_dialog_output_tab_custom_large_icon_button" ), channel_dialog::on_output_tab_custom_large_icon_button )

    EVT_UPDATE_UI( XRCID( "channel_dialog_scheduling_tab_update_enabled_checkbox" ), channel_dialog::on_update_ui_scheduling_tab_update_enabled_checkbox )
    EVT_BUTTON( XRCID( "channel_dialog_scheduling_tab_now_button" ), channel_dialog::on_scheduling_tab_now_button )

    EVT_BUTTON( wxID_OK, channel_dialog::OnOK)
    EVT_BUTTON( wxID_HELP_CONTEXT, channel_dialog::on_help_button )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

// Constructor includes setting the dialog's m_configuration_section member to the
// incoming configuration_section string.
channel_dialog::channel_dialog( wxWindow* parent, wxString configuration_section,
                                int initial_selected_notebook_tab )
                                : m_configuration_section( configuration_section )
{
	wxXmlResource::Get()->LoadDialog( this, parent, wxT( "channel_dialog" ) );

    // Set the path (section) to look for the values to the passed section name. For example
    // if passed "news", a call to the_configuration->Read("bpp"...) will now get the
    // value of /news/bpp. It also creates this section if it doesn't exist yet in the
    // config file.
    wxString new_configuration_path;
    new_configuration_path << wxT( "/" ) << configuration_section;
    the_configuration->SetPath( new_configuration_path );
    wxLogDebug( wxT( "Set configuration section" ) );

    // Attach the custom wxHandheldInstallerCtrl to its placeholder in XRC resource
    m_handheld_install_ctrl = new wxHandheldInstallCtrl( this, -1,
                                        _( "Send channel output to these handhelds" ),
                                        _( "Send channel output to these directories" )
                                        );
    wxXmlResource::Get()->AttachUnknownControl( wxT( "channel_dialog_destination_tab_handheldinstallctrl" ),
                                            m_handheld_install_ctrl );
    wxLogDebug( wxT( "Attached custom wxHandheldInstaller control" ) );

    // Set up the listctrl icons for the wxHandheldInstallerCtrl
    m_handheld_install_ctrl->SetImageList( image_list::get() );
    m_handheld_install_ctrl->SetUsersImageListImageId( plkrSMALL_IMAGE_LIST_ID_CHANNEL_DIALOG_DESTINATION_TAB_USERS_LISTCTRL );
    m_handheld_install_ctrl->SetDirectoriesImageListImageId( plkrSMALL_IMAGE_LIST_ID_CHANNEL_DIALOG_DESTINATION_TAB_DIRECTORIES_LISTCTRL );

    m_scheduling_time_control = new time_control( this, -1 );
    wxXmlResource::Get()->AttachUnknownControl( wxT( "channel_dialog_scheduling_tab_time_control" ),
                                                m_scheduling_time_control );
    wxLogDebug( wxT( "Attached custom time_control" ) );

    // Read the configuration file to set up the values for the controls.
    read_configuration();

    // Set the initial selected notebook tab, if it isn't the start tab (which is selected
    // already). This allows channel dialog to be called with a certain tab selected,
    // like the destination tab, if about to crawl and there is no destination.
    if ( initial_selected_notebook_tab != CHANNEL_NOTEBOOK_PAGE_INDEX_START ) 
    {
        XRCCTRL( *this, "channel_dialog_notebook", wxNotebook )
            ->SetSelection( initial_selected_notebook_tab );
    }

    /*! \test Test channel name (doc_name) maxlength on Mac */
    XRCCTRL( *this, "channel_dialog_textctrl", wxTextCtrl )->SetMaxLength( plkrMAXIMUM_CHANNEL_NAME_LENGTH );

#if ( setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS )	
    wxLogDebug( wxT( "Running a setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS" ) );
    wxSize	default_size = GetSize();
    wxSize  new_size;
    new_size.x     = default_size.x + 1;
    new_size.y     = default_size.y + 1;
    SetSize( new_size );
#endif
    
    // Set the size, position of the frame using our common function for this dialog
    // [utils_controls.cpp]	
    utils_controls::read_dialog_position_and_size( this, wxT( "channel_dialog" ) );

    // Hide the alt_text control. Not using it
    XRCCTRL( *this, "channel_dialog_images_tab_include_alt_text_checkbox", wxCheckBox )->Enable( FALSE );
    XRCCTRL( *this, "channel_dialog_images_tab_include_alt_text_checkbox", wxCheckBox )->Show( FALSE );

}

channel_dialog::~channel_dialog()
{
    // Save the size and position of the dialog for next time
    // [utils_controls.cpp]
    utils_controls::write_dialog_position_and_size( this, wxT( "channel_dialog" ) );

    // Set the configuration path back to the root.
    the_configuration->SetPath( wxT( "/" ) );
}

//----------------------------------------------------------------------------------------
// Private members: non-event handlers
//----------------------------------------------------------------------------------------

void channel_dialog::read_configuration()
{
    wxLogDebug( wxT( "Starting to read configuration file" ) );

    wxString key;
    wxString doc_name;

    key = wxT( "doc_name" );

    // See comments in main_frame::listctrl_load_rows() about doc_name
    if ( ! the_configuration->Exists( key ) )
        key = wxT( "db_name" );

    if ( ! the_configuration->Exists( key ) )
        key = wxT( "doc_file" );

    if ( ! the_configuration->Exists( key ) )
        key = wxT( "db_file" );

    doc_name = the_configuration->Read( key, plkrUNNAMED_CHANNEL_NAME );
    // If a file key, strip off the path and extension, leaving only the basename
    if ( key == wxT( "doc_file" ) || key == wxT( "db_file" ) )
        wxSplitPath( doc_name.c_str(), NULL, &doc_name, NULL );

    XRCCTRL( *this, "channel_dialog_textctrl", wxTextCtrl )
        ->SetValue( doc_name );
    wxLogDebug( wxT( "Initialized doc_name" ) );

    //------Start Tab-------------------------------------------------------------------

    wxString home_url;

    home_url = the_configuration->Read( wxT( "home_url" ), wxT( "" ) );
    // If no url specified: must be the default plucker home.html HTML page
    if ( home_url == wxT( "" ) || home_url == wxT( "plucker:/home.html" ) ||
         ( home_url.Contains( wxT( "plucker:" ) ) &&
           home_url.Contains( wxT( "channels" ) ) &&
           home_url.Contains( wxT( "home.html" ) ) ) ) 
    {
        XRCCTRL( *this, "channel_dialog_start_tab_links_radiobutton", wxRadioButton )
            ->SetValue( TRUE );
    // Else a file: or plucker: protocol
    } 
    else if ( home_url.StartsWith( wxT( "file:" ) ) || home_url.StartsWith( wxT( "plucker:" ) ) ) 
    {
        XRCCTRL( *this, "channel_dialog_start_tab_file_radiobutton", wxRadioButton )
            ->SetValue( TRUE );
        XRCCTRL( *this, "channel_dialog_start_tab_file_textctrl", wxTextCtrl )
            ->SetValue( home_url );
    // Everything else into a website address
    } 
    else 
    {
        XRCCTRL( *this, "channel_dialog_start_tab_url_radiobutton", wxRadioButton )
            ->SetValue( TRUE );
        XRCCTRL( *this, "channel_dialog_start_tab_url_textctrl", wxTextCtrl )
            ->SetValue( home_url );
    }
    wxLogDebug( wxT( "Initialized home_url" ) );

    //------Spidering Tab------------------------------------------------------------------
    long retrieval_timeout;
    retrieval_timeout = the_configuration->Read( wxT( "retrieval_timeout" ), 15L );
    // The spinctrl values are 3 to 360seconds (more than this will timeout in python anyways).
    // Shorter than 3 seconds is going to pile in bugreports asking why parsing doesn't work.
    // Longer than about 45 seconds will likely add in some bugreports asking why PluckerDesktop
    // seems to not be doing anything, but that is unavoidable (like Firefox:it says contacting...
    // and is a blank page while it waits to timeout.)
    // I tried to handle the cases of retrieval timeout between 0 and minvalue and 
    // over 360seconds, but it seems XRC values won't let that be written after they are created,
    // they just go to their maximum.
    if ( retrieval_timeout != 0 )
    {
        XRCCTRL( *this, "channel_dialog_spidering_tab_retrieval_timeout_checkbox", wxCheckBox )
             ->SetValue ( TRUE );
    } 
    else
    {
        // ...otherwise we put the default value (for the grayed out checkbox) back to 15.
        retrieval_timeout = 15;
    }
    XRCCTRL( *this, "channel_dialog_spidering_tab_retrieval_timeout_spinctrl", wxSpinCtrl )
        ->SetValue( retrieval_timeout );
    wxLogDebug( wxT( "retrieval_timeout" ) );
    
    long depth_first;
    depth_first = the_configuration->Read( wxT( "depth_first" ), 0L );
    XRCCTRL( *this, "channel_dialog_spidering_tab_depth_first_choice", wxChoice )
        ->SetSelection( (int)depth_first );
    wxLogDebug( wxT( "Initialized depth_first" ) );

    long verbosity;
    verbosity = the_configuration->Read( wxT( "verbosity" ), 1L );
    XRCCTRL( *this, "channel_dialog_spidering_tab_verbosity_choice", wxChoice )
        ->SetSelection( (int)verbosity );
    wxLogDebug( wxT( "Initialized verbosity" ) );

    long status_line_length;
    status_line_length = the_configuration->Read( wxT( "status_line_length" ), 60L );
    // If 0<value and != the infinite, we check the checkbox...
    if ( 0 < status_line_length && status_line_length != plkrINFINITE_STATUS_LINE_LENGTH_LENGTH) 
    {
        XRCCTRL( *this, "channel_dialog_spidering_tab_status_line_length_checkbox", wxCheckBox )
             ->SetValue ( TRUE );
    } 
    else 
    {
        // ...otherwise we put the value (for the grayed out checkbox) back to 60.
        status_line_length = 60;
    }
    XRCCTRL( *this, "channel_dialog_spidering_tab_status_line_length_spinctrl", wxSpinCtrl )
        ->SetValue( status_line_length );
    wxLogDebug( wxT( "Initialized status_line_length" ) );

    wxString referrer;
    referrer = the_configuration->Read( wxT( "referrer" ), wxT( "" ) );
    XRCCTRL( *this, "channel_dialog_spidering_tab_referrer_textctrl", wxTextCtrl )
        ->SetValue( referrer );
    if ( referrer != wxT( "" ) ) 
    {
        XRCCTRL( *this, "channel_dialog_spidering_tab_referrer_checkbox", wxCheckBox )
            ->SetValue( TRUE );
    }
    wxLogDebug( wxT( "Initialized referrer" ) );

    wxString user_agent;
    user_agent = the_configuration->Read( wxT( "user_agent" ), wxT( "" ) );
    // Initialize the browser choice to Plucker.
    XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_browser_choice", wxChoice )
        ->SetSelection( 0 );
    bool is_a_suggested_user_agent = FALSE;
    //! \todo This allows a nasty possiblilty of values past the WXSIZEOF somehow.
    for ( int n=0; n < (int)WXSIZEOF( suggested_user_agents ); n++ ) 
    {
        if ( user_agent == suggested_user_agents[ n ] ) 
        {
        XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_browser_choice", wxChoice )
            ->SetSelection( n );
        is_a_suggested_user_agent = TRUE;
        break;
        }
    }
    if ( is_a_suggested_user_agent == TRUE ) 
    {
        XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_browser_radiobutton", wxRadioButton )
                ->SetValue( TRUE );
    }
    else 
    {
        XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_custom_radiobutton", wxRadioButton )
                ->SetValue( TRUE );
        XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_custom_textctrl", wxTextCtrl )
                ->SetValue( user_agent );
    }
    wxLogDebug( wxT( "Initializaed user_agent" ) );

    //------Limits Tab------------------------------------------------------------------

    XRCCTRL( *this, "channel_dialog_limits_tab_maxdepth_spinctrl", wxSpinCtrl )
        ->SetValue( (int)the_configuration->Read( wxT( "home_maxdepth" ), 2L ) );
    wxLogDebug( wxT( "Initialized home_maxdepth" ) );

    bool home_stayonhost;
    bool home_stayondomain;
    home_stayonhost     = (bool)the_configuration->Read( wxT( "home_stayonhost" ), 0L );
    home_stayondomain   = (bool)the_configuration->Read( wxT( "home_stayondomain" ), 0L );

    set_limits_tab_stayonhost_stayondomain_controls( home_stayonhost, home_stayondomain );

    wxString home_url_pattern;
    home_url_pattern = the_configuration->Read( wxT( "home_url_pattern" ), wxT( "" ) );
    XRCCTRL( *this, "channel_dialog_limits_tab_url_pattern_textctrl", wxTextCtrl )
        ->SetValue( home_url_pattern );
    if ( home_url_pattern != wxT( "" ) ) 
    {
        XRCCTRL( *this, "channel_dialog_limits_tab_url_pattern_checkbox", wxCheckBox )
            ->SetValue ( TRUE );
    }
    wxLogDebug( wxT( "Initialized home_url_pattern" ) );

    // Exclusion list handled a bit differently. If the key is blank/doesn't exist
    // then no exclusion list should be appended to the one in the [<OS>] section.
    // If there is an entry and it isn't blank, then there is an exclusion list for this
    // channel.
    if ( the_configuration->Read( wxT( "exclusion_lists" ), wxT( "") ) != wxT( "" ) ) 
    {
        XRCCTRL( *this, "channel_dialog_limits_tab_exclusion_checkbox", wxCheckBox )
            ->SetValue( TRUE );
        wxLogDebug( wxT( "An exclusion_lists key in channel, so checkbox filled" ) );
    }
    wxLogDebug( wxT( "Initialized exclusion_lists" ) );

    //------Formatting Tab--------------------------------------------------------------

    wxString charset;
    charset = the_configuration->Read( wxT( "charset" ), wxT( "" ) );
    XRCCTRL( *this, "channel_dialog_formatting_tab_charset_combobox", wxComboBox )
         ->SetValue( charset );
    if ( charset != wxT( "" ) )
    {
        XRCCTRL( *this, "channel_dialog_formatting_tab_charset_checkbox", wxCheckBox )
            ->SetValue ( TRUE );
    }
    wxLogDebug( wxT( "Initialized charset" ) );

    XRCCTRL( *this, "channel_dialog_formatting_tab_indent_paragraphs_checkbox", wxCheckBox )
         ->SetValue( (bool) the_configuration->Read( wxT( "indent_paragraphs" ), 0L ) );
    wxLogDebug( wxT( "Initialized indent_paragraphs" ) );

    XRCCTRL( *this, "channel_dialog_formatting_tab_tables_checkbox", wxCheckBox )
         ->SetValue( (bool) the_configuration->Read( wxT( "tables" ), 0L ) );
    wxLogDebug( wxT( "Initialized tabless" ) );

    wxString anchor_color;
    anchor_color = the_configuration->Read( wxT( "anchor_color" ), wxT( "#0000FF" ) );
    XRCCTRL( *this, "channel_dialog_formatting_tab_anchor_color_textctrl", wxTextCtrl )
         ->SetValue( anchor_color );
    if ( anchor_color != wxT( "" ) )
    {
        XRCCTRL( *this, "channel_dialog_formatting_tab_anchor_color_checkbox", wxCheckBox )
            ->SetValue ( TRUE );
    }
    wxLogDebug( wxT( "Initialized anchor_color" ) );

    //------Images Tab------------------------------------------------------------------

    // NOTE: If adding another bit depth, also add it to the viewer_wizardpage.
    long bpp;
    bpp = the_configuration->Read( wxT( "bpp" ), 1L );
    set_image_tab_bpp_controls( bpp );

    XRCCTRL( *this, "channel_dialog_images_tab_include_alt_text_checkbox", wxCheckBox )
        ->SetValue( (bool) the_configuration->Read( wxT( "alt_text" ), 1L ) );
    wxLogDebug( wxT( "Initialized alt_text" ) );

    XRCCTRL( *this, "channel_dialog_images_tab_thumbnail_if_bigger_width_spinctrl", wxSpinCtrl )
        ->SetValue( (int) the_configuration->Read( wxT( "maxwidth" ), 150L ) );
    wxLogDebug( wxT( "Initialized maxwidth" ) );

    XRCCTRL( *this, "channel_dialog_images_tab_thumbnail_if_bigger_height_spinctrl", wxSpinCtrl )
        ->SetValue( (int) the_configuration->Read( wxT( "maxheight" ), 250L ) );
    wxLogDebug( wxT( "Initialized maxheight" ) );

    // Some GUI tricks here. Always link by setting a very high number (plkrIMAGES_ONLY_THUMBNAIL)
    // Never link by setting to zero (plkrIMAGES_ALWAYS_LINK) [Both defined above].
    // Else use the specified value.

    long alt_maxwidth;
    alt_maxwidth = the_configuration->Read( wxT( "alt_maxwidth" ), plkrIMAGES_ALWAYS_LINK );
    wxLogDebug( wxT( "Read alt_maxwidth=%ld" ), alt_maxwidth );
    switch ( alt_maxwidth ) {
#ifdef setupUSE_DEPRECATED_ALWAYS_LINK_MAXWIDTH_IS_ALSO_1000000_AS_WELL_AS_0
        case 1000000:       
#endif
        case plkrIMAGES_ALWAYS_LINK:
            XRCCTRL( *this, "channel_dialog_images_tab_always_link_radiobutton", wxRadioButton )
                ->SetValue( TRUE );
            wxLogDebug( wxT( "Initialized alt_maxwidth-height as 0 (plkrIMAGES_ALWAYS_LINK)" ) );
            break;

        case plkrIMAGES_ONLY_THUMBNAIL:
            XRCCTRL( *this, "channel_dialog_images_tab_only_thumbnail_radiobutton", wxRadioButton )
                ->SetValue( TRUE );
            wxLogDebug( wxT( "Initialized alt_maxwidth-height as -1 (plkrIMAGES_ONLY_THUMBNAIL)" ) );
            break;

        default:
            XRCCTRL( *this, "channel_dialog_images_tab_only_link_radiobutton", wxRadioButton )
                ->SetValue( TRUE );
            XRCCTRL( *this, "channel_dialog_images_tab_only_link_width_spinctrl", wxSpinCtrl )
                ->SetValue( (int)the_configuration->Read( wxT( "alt_maxwidth" ), 800L ) );
            XRCCTRL( *this, "channel_dialog_images_tab_only_link_height_spinctrl", wxSpinCtrl )
                ->SetValue( (int)the_configuration->Read( wxT( "alt_maxheight" ), 600L ) );
            wxLogDebug( wxT( "Initialized alt_maxwidth/height as a specified value" ) );
            break;
    }
    wxLogDebug( wxT( "Initializion of alt_maxwidth/height complete" ) );

    int try_reduce_index;
    // Default is neither try_reduce_bpp nor try_reduce_dimension (i.e. use multi-image)
    int try_reduce_bpp       = the_configuration->Read( wxT( "try_reduce_bpp") , 0L );
    int try_reduce_dimension = the_configuration->Read( wxT( "try_reduce_dimension" ), 0L );
    if ( try_reduce_dimension == 0 && try_reduce_bpp == 0 ) 
    {
        try_reduce_index = 0; // Multiimage
    } 
    else if ( try_reduce_dimension == 1 && try_reduce_bpp == 0 ) 
    {
        try_reduce_index = 1; // Try reduce dimensions
    } 
    else 
    {
        try_reduce_index = 2; // Try reduce color depth
    }
    XRCCTRL( *this, "channel_dialog_images_tab_try_reduce_choice", wxChoice )
        ->SetSelection( try_reduce_index );
    wxLogDebug( wxT( "Initialized try_reduce_bpp/try_reduce_dimension" ) );

    //------Output Tab------------------------------------------------------------------

    int compression_choice_index;
    // The new key is a string: compress=[doc|zlib], so look for it first.
    // If not around, then process the old key, which is a bool: zlib_compression=[1|0].
    // Inside the GUI, DOC is choice index 0, ZLIB is choice index 1
    if ( the_configuration->Exists( wxT( "compression" ) ) )
    {
        wxString compression_value_string = the_configuration->Read( wxT( "compression" ), wxT( "zlib" ) );
        if ( compression_value_string == wxT( "doc" ) )
        {
            compression_choice_index = 0;
        } 
        else 
        {
            compression_choice_index = 1;
        }
    } 
    else 
    {
        compression_choice_index = the_configuration->Read( wxT( "zlib_compression" ), 1L );
    }
    XRCCTRL( *this, "channel_dialog_output_tab_compression_choice", wxChoice )
        ->SetSelection( compression_choice_index );
    wxLogDebug( wxT( "Initialized compression" ) );

    long compression_limit;
    compression_limit = the_configuration->Read( wxT( "image_compression_limit" ), 50L );
    XRCCTRL( *this, "channel_dialog_output_tab_only_compress_spinctrl", wxSpinCtrl )
        ->SetValue( compression_limit );
    if ( 0 < compression_limit ) 
    {
        XRCCTRL( *this, "channel_dialog_output_tab_only_compress_checkbox", wxCheckBox )
             ->SetValue ( TRUE );
    }
    wxLogDebug( wxT( "Initialized image_compression" ) );

    wxString category;
    category = the_configuration->Read( wxT( "category" ), wxT( "" ) );
    XRCCTRL( *this, "channel_dialog_output_tab_category_textctrl", wxTextCtrl )
         ->SetValue( category );
    if ( category != wxT( "" ) ) 
    {
        XRCCTRL( *this, "channel_dialog_output_tab_category_checkbox", wxCheckBox )
            ->SetValue ( TRUE );
    }
    wxLogDebug( wxT( "Initialized category" ) );

    // Note ! since asking in the positive
    XRCCTRL( *this, "channel_dialog_output_tab_include_url_checkbox", wxCheckBox )
        ->SetValue( ! (bool)the_configuration->Read( wxT( "no_urlinfo" ), 1L ) );

    wxString owner_id_build;
    owner_id_build = the_configuration->Read( wxT( "owner_id_build" ), wxT( "" ) );
    XRCCTRL( *this, "channel_dialog_output_tab_owner_id_textctrl", wxTextCtrl )
         ->SetValue( owner_id_build );
    if ( owner_id_build != wxT( "" ) ) 
    {
        XRCCTRL( *this, "channel_dialog_output_tab_owner_id_checkbox", wxCheckBox )
            ->SetValue ( TRUE );
    }
    wxLogDebug( wxT( "Initialized owner_id_build" ) );

    // Note ! since asking in the positive
    XRCCTRL( *this, "channel_dialog_output_tab_beamable_checkbox", wxCheckBox )
         ->SetValue( ! (bool)the_configuration->Read( wxT( "copyprevention_bit" ), 0L ) );
    wxLogDebug( wxT( "Initialized copyprevention_bit" ) );

    XRCCTRL( *this, "channel_dialog_output_tab_backup_checkbox", wxCheckBox )
         ->SetValue( (bool)the_configuration->Read( wxT( "backup_bit" ), 0L ) );
    wxLogDebug( wxT( "Initialized backup_bit" ) );

    XRCCTRL( *this, "channel_dialog_output_tab_launchable_checkbox", wxCheckBox )
         ->SetValue( (bool)the_configuration->Read( wxT( "launchable_bit" ), 0L ) );
    wxLogDebug( wxT( "Initialized launchable_bit" ) );

    wxString big_icon;
    big_icon = the_configuration->Read( wxT( "big_icon" ), wxT( "" ) );
    XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_textctrl", wxTextCtrl )
        ->SetValue( big_icon );
    if ( big_icon != wxT( "" ) ) 
    {
        XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_checkbox", wxCheckBox )
            ->SetValue ( TRUE );
    }                       
    wxLogDebug( wxT( "Initialized big_icon" ) );

    wxString small_icon;
    small_icon = the_configuration->Read( wxT( "small_icon" ), wxT( "" ) );
    XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_textctrl", wxTextCtrl )
        ->SetValue( small_icon );
    if ( small_icon != wxT( "" ) ) 
    {
        XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_checkbox", wxCheckBox )
            ->SetValue ( TRUE );
    }
    wxLogDebug( wxT( "Initialized small_icon" ) );

    //------Destination Tab--------------------------------------------------------------------

    // Initialize the handheld destinations:
    handheld_dest_array_type handheld_dest_array;
    handheld_dest_wrappers::read_handheld_dest_array_from_configuration( m_configuration_section,
                                                                         &handheld_dest_array );
    m_handheld_install_ctrl->InsertUserDestinations( handheld_dest_array );

    wxArrayString sync_directories_array;
    wxString sync_directories_string = the_configuration->Read( wxT( "copy_to_dir" ), wxT( "" ) );
    // Converts the read string to an array of integers.
    utils_string::string_to_arraystring( sync_directories_string, plkrOS_FILE_ARRAY_SEPARATOR, &sync_directories_array, true );
    // Insert the users into the control (checked for empty there).
    m_handheld_install_ctrl->InsertDirectoryDestinations( sync_directories_array );

    // Free memory occupied by arrays
    // The WX_CLEAR_ARRAY(array) macro [see dynarray.h] is required to clear out all the
    // elements from memory of an array of structs that was built with WX_DEFINE_ARRAY.
    // All that this macro does is just to loop through the array, "delete" each element
    // to free it from memory, then mark the array as Empty().
    WX_CLEAR_ARRAY( handheld_dest_array );
    sync_directories_array.Clear();

    wxLogDebug( wxT( "Initialized destinations tab" ) );

    //------Scheduling Tab--------------------------------------------------------------

    bool update_enabled;
    update_enabled = (bool)the_configuration->Read( wxT( "update_enabled" ), 0L );

    wxString update_period;
    update_period = the_configuration->Read( wxT( "update_period" ), wxT( "daily" ) );

    long update_frequency;
    update_frequency = the_configuration->Read( wxT( "update_frequency" ), 1L );

    wxString    update_base_string;
    wxDateTime  update_base;
    update_base_string = the_configuration->Read( wxT( "update_base" ), wxT( "" ) );

    update_base = utils_datetime::w3c_string_to_datetime( update_base_string );

    set_scheduling_tab_update_controls( update_frequency, update_period,
                                        update_base, update_enabled );

    wxLogDebug( wxT( "Initialized scheduling tab" ) );
}


void channel_dialog::set_limits_tab_stayonhost_stayondomain_controls(
    bool stayonhost,
    bool stayondomain
    )
{

    // Start by wiping both radiobuttons to FALSE.
    XRCCTRL( *this, "channel_dialog_limits_tab_stayonhost_radiobutton", wxRadioButton)
            ->SetValue( FALSE );
    XRCCTRL( *this, "channel_dialog_limits_tab_stayondomain_radiobutton", wxRadioButton )
            ->SetValue( FALSE );
    // If both stayonhost and stayondomain are true, then only select the stayonhost, since
    // stayondomain is then meaningless. If neither are true, then select the stayondomain,
    // (so a radio will always be selected) and uncheck the checkbox.
    if ( stayonhost ) 
    {
        XRCCTRL( *this, "channel_dialog_limits_tab_stayonhost_radiobutton", wxRadioButton)
            ->SetValue( TRUE );
    }
    else
    {
        XRCCTRL( *this, "channel_dialog_limits_tab_stayondomain_radiobutton", wxRadioButton )
            ->SetValue( TRUE );
    }
    bool check_checkbox = FALSE;
    if ( stayonhost || stayondomain ) 
    {
        check_checkbox = TRUE;
    }
    XRCCTRL( *this, "channel_dialog_limits_tab_stayonhost_stayondomain_checkbox", wxCheckBox )
        ->SetValue( check_checkbox );

    wxLogDebug( wxT( "Initialized home_stayonhost and home_stayondomain" ) );
}


// Sets the update controls. Pulled out into separate function since shared among both the
// read_configuration() and a return from the showcase dialog
void channel_dialog::set_scheduling_tab_update_controls( 
    long             update_frequency, 
    const wxString   &update_period,  
    const wxDateTime &update_base,
    bool             update_enabled 
    )
{ 
    wxDateTime update_base_value = update_base;
    
    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_enabled_checkbox", wxCheckBox )
         ->SetValue( update_enabled );
    wxLogDebug( wxT( "Initialized update_enabled" ) );
    
    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_frequency_spinctrl", wxSpinCtrl )
        ->SetValue( update_frequency );    
    wxLogDebug( wxT( "Initialized update_frequency" ) ); 
        
    int update_choice_index;    
    if ( update_period == wxT( "hourly" ) )
    { 
        update_choice_index = 0;
    }         
    else if ( update_period == wxT( "daily" ) ) 
    {  
        update_choice_index = 1;
    } 
    else if ( update_period == wxT( "weekly" ) ) 
    { 
        update_choice_index = 2;
    }     
    else if ( update_period == wxT( "monthly" ) ) 
    { 
        update_choice_index = 3;
    } 
    else if ( update_period == wxT( "yearly" ) )
    { 
        update_choice_index = 4;
    }  
    else 
    {
        wxLogError( wxT( "Error: unrecognized update_period key called" ) + update_period );
        update_choice_index = 1;
    } 
    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_period_choice", wxChoice )
        ->SetSelection( update_choice_index );
    wxLogDebug( wxT( "Initialized update_period" ) ); 
   
    // If passed a bad date, just use Now()
    if ( ! update_base_value.IsValid() ) 
    {
        update_base_value = wxDateTime::Now();
    }

    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_base_calendarctrl", wxCalendarCtrl )
        ->SetDate( update_base_value );
    wxLogDebug( wxT( "Initialized update_base date component" ) );

    m_scheduling_time_control->set_time( update_base_value );
    wxLogDebug( wxT( "Initialized update_base time component" ) );
}


// Sets the update_frequency. Pulled out into separate function since shared among both the
// read_configuration() and a return from the showcase dialog
void channel_dialog::set_image_tab_bpp_controls( long bpp )
{
    int choice;
    
    if ( 0 < bpp ) 
    {
        XRCCTRL( *this, "channel_dialog_images_tab_include_images_checkbox", wxCheckBox )
            ->SetValue( TRUE );
    } 
    else 
    {
        XRCCTRL( *this, "channel_dialog_images_tab_include_images_checkbox", wxCheckBox )
            ->SetValue( FALSE );
    }    

    switch ( bpp ) 
    { 
        case 1:
            choice = 0;
            break;

        case 2:
            choice = 1;
            break;

        case 4:
            choice = 2;
            break;

        case 8:
            choice = 3;
            break;    

        case 16:
            choice = 4;
            break;

        default:
            choice = 0;
            break;
    }
    XRCCTRL( *this, "channel_dialog_images_tab_depth_choice", wxChoice )
        ->SetSelection( choice );

    wxLogDebug( wxT( "Set bpp of %ld bpp to choice index %d" ), bpp, choice ); 
}

// Write out the values of the dialog's controls to the channel-specific section of
// the configuration file
void channel_dialog::write_configuration()
{

    the_configuration->Write( wxT( "doc_name" ), 
        XRCCTRL( *this, "channel_dialog_textctrl", wxTextCtrl )->GetValue() );
                
    //------Start Tab-------------------------------------------------------------------
    
    wxString start_location;
    
    // A website address
    if ( XRCCTRL( *this, "channel_dialog_start_tab_url_radiobutton", wxRadioButton )
                    ->GetValue() == TRUE ) 
    {         
        start_location = XRCCTRL( *this, "channel_dialog_start_tab_url_textctrl", wxTextCtrl )
                    ->GetValue();
    // A local file
    } 
    else if (  XRCCTRL( *this, "channel_dialog_start_tab_file_radiobutton", wxRadioButton )
                    ->GetValue() == TRUE )
    {         
        start_location = XRCCTRL( *this, "channel_dialog_start_tab_file_textctrl", wxTextCtrl )
                    ->GetValue();
    // List of links
    } 
    else 
    {
        if ( the_configuration->GetPath() != ( wxT( "/" ) plkrNEW_CHANNEL_TEMPLATE_SECTION ) )
        {
            // Make the home_url a string to the channel's home.html file.
            start_location = the_configuration->GetPath( );
            start_location = start_location.AfterFirst('/');
            start_location = wxT( "plucker:/channels/" ) + start_location + wxT( "/home.html" );
        // If plkrNEW_CHANNEL_TEMPLATE_SECTION section, then not a channel, just the
        // template. Leave it the way it was or blank if no key.
        }
        else 
        {
            start_location == the_configuration->Read( wxT( "home_url" ), wxT( "" ) );
        }
    } 
    the_configuration->Write( wxT( "home_url" ), start_location );        
    
    wxLogDebug( wxT( "Wrote channel_dialog starting page tab values to config file" ) );

    //------Spidering Tab------------------------------------------------------------------

    if ( XRCCTRL( *this, "channel_dialog_spidering_tab_retrieval_timeout_checkbox", wxCheckBox )
            ->GetValue() == TRUE ) 
    {
        the_configuration->Write( wxT( "retrieval_timeout" ),
            (long) XRCCTRL( *this, "channel_dialog_spidering_tab_retrieval_timeout_spinctrl", wxSpinCtrl )->GetValue() );
    } 
    else
    {
        the_configuration->Write( wxT( "retrieval_timeout" ) , 0L );
    }
    
    the_configuration->Write( wxT( "depth_first" ),
        (long) XRCCTRL( *this, "channel_dialog_spidering_tab_depth_first_choice", wxChoice )->GetSelection() );

    the_configuration->Write( wxT( "verbosity" ),
        (long) XRCCTRL( *this, "channel_dialog_spidering_tab_verbosity_choice", wxChoice )->GetSelection() );

    if ( XRCCTRL( *this, "channel_dialog_spidering_tab_status_line_length_checkbox", wxCheckBox )
            ->GetValue() == TRUE ) 
    {
        the_configuration->Write( wxT( "status_line_length" ),
            (long) XRCCTRL( *this, "channel_dialog_spidering_tab_status_line_length_spinctrl", wxSpinCtrl )->GetValue() );
    } 
    else
    {
        the_configuration->Write( wxT( "status_line_length" ), plkrINFINITE_STATUS_LINE_LENGTH_LENGTH );
    }

    wxString referrer;
    referrer = XRCCTRL( *this, "channel_dialog_spidering_tab_referrer_textctrl", wxTextCtrl )->GetValue();
    if ( XRCCTRL( *this, "channel_dialog_spidering_tab_referrer_checkbox", wxCheckBox )
            ->GetValue() == FALSE ) 
    {
        referrer = wxT( "" );
    }
    the_configuration->Write( wxT( "referrer" ), referrer );

    wxString user_agent;
    int user_agent_choice_index;
    if ( XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_browser_radiobutton", wxRadioButton )
                ->GetValue() == TRUE ) 
    {
        user_agent_choice_index= XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_browser_choice", wxChoice )
                                      ->GetSelection();
        user_agent = suggested_user_agents[ user_agent_choice_index ];
    } 
    else
    {
        user_agent = XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_custom_textctrl", wxTextCtrl )
                        ->GetValue();
    }
    the_configuration->Write( wxT( "user_agent" ), user_agent );

    wxLogDebug( wxT( "Wrote channel_dialog spidering tab values to config file" ) );

    //------Limits Tab------------------------------------------------------------------

    the_configuration->Write( wxT( "home_maxdepth" ),
        (long) XRCCTRL( *this, "channel_dialog_limits_tab_maxdepth_spinctrl", wxSpinCtrl )->GetValue() );

    bool home_stayonhost;
    bool home_stayondomain;
    if ( XRCCTRL( *this, "channel_dialog_limits_tab_stayonhost_stayondomain_checkbox", wxCheckBox )
        ->GetValue() == TRUE ) 
    {    
        home_stayonhost = (bool)XRCCTRL( *this, "channel_dialog_limits_tab_stayonhost_radiobutton", wxRadioButton )
            ->GetValue();
        home_stayondomain = (bool)XRCCTRL( *this, "channel_dialog_limits_tab_stayondomain_radiobutton", wxRadioButton )
            ->GetValue();    
    } 
    else 
    {
        home_stayonhost     = false;
        home_stayondomain   = false;
    }
    the_configuration->Write( wxT( "home_stayonhost" ), home_stayonhost );
    the_configuration->Write( wxT( "home_stayondomain" ), home_stayondomain );

    if ( XRCCTRL( *this, "channel_dialog_limits_tab_url_pattern_checkbox", wxCheckBox )
            ->GetValue() == TRUE ) 
    {
        the_configuration->Write( wxT( "home_url_pattern" ), 
            XRCCTRL( *this, "channel_dialog_limits_tab_url_pattern_textctrl", wxTextCtrl )->GetValue() );
    } 
    else
    {
        the_configuration->Write( wxT( "home_url_pattern" ), wxT( "" ) );
    }
  
    if ( XRCCTRL( *this, "channel_dialog_limits_tab_exclusion_checkbox", wxCheckBox )
            ->GetValue() == TRUE ) 
    {
        // NOTE: can't have "channels" + '/' ... here or it won't work (I guess since trying
        // to store an extra / in a const wxString)
        wxString channel_exclusion_path = wxT( "channels/" ) + m_configuration_section + _T ("/exclusionlist.txt" );
        the_configuration->Write( wxT( "exclusion_lists" ), channel_exclusion_path );  
    } 
    else
    {
        the_configuration->Write( wxT( "exclusion_lists" ), wxT( "" ) );
        wxLogDebug( wxT( "Checkbox empty, so empty exclusion_lists key written to channel section" ) );
    }
    
    wxLogDebug( wxT( "Wrote channel_dialog limits tab values to config file" ) );

    //------Formatting Tab--------------------------------------------------------------
    
    if ( XRCCTRL( *this, "channel_dialog_formatting_tab_charset_checkbox", wxCheckBox )
            ->GetValue() == TRUE ) 
    { 
        the_configuration->Write( wxT( "charset" ), 
            XRCCTRL( *this, "channel_dialog_formatting_tab_charset_combobox", wxComboBox )->GetValue() );
    } 
    else
    {
        the_configuration->Write( wxT( "charset" ), wxT( "" ) );
    }

    the_configuration->Write( wxT( "indent_paragraphs" ),
        (bool)XRCCTRL( *this, "channel_dialog_formatting_tab_indent_paragraphs_checkbox", wxCheckBox )->GetValue() );

    the_configuration->Write( wxT( "tables" ),
        (bool)XRCCTRL( *this, "channel_dialog_formatting_tab_tables_checkbox", wxCheckBox )->GetValue() );

    if ( XRCCTRL( *this, "channel_dialog_formatting_tab_anchor_color_checkbox", wxCheckBox )
            ->GetValue() == TRUE )
    {
        the_configuration->Write( wxT( "anchor_color" ),
            XRCCTRL( *this, "channel_dialog_formatting_tab_anchor_color_textctrl", wxTextCtrl )->GetValue() );
    } 
    else 
    {
        the_configuration->Write( wxT( "anchor_color" ), wxT( "" ) );
    }

    wxLogDebug( wxT( "Wrote channel_dialog formatting tab values to config file" ) );

    //------Images Tab------------------------------------------------------------------

    long bpp    = 0;

    switch ( XRCCTRL( *this, "channel_dialog_images_tab_depth_choice", wxChoice )->GetSelection() ) 
    {
        case 0:
            bpp = 1;
            break;
        case 1:
            bpp = 2;
            break;
        case 2:
            bpp = 4;
            break;
        case 3:
            bpp = 8;
            break;
        case 4:
            bpp = 16;
            break;
    }
    if ( XRCCTRL( *this, "channel_dialog_images_tab_include_images_checkbox", wxCheckBox )->GetValue() == FALSE )
    {
        bpp = 0;
    }
    the_configuration->Write( wxT( "bpp" ), bpp );

    the_configuration->Write( wxT( "alt_text" ),
        (bool) XRCCTRL( *this, "channel_dialog_images_tab_include_alt_text_checkbox", wxCheckBox )->GetValue() );

    the_configuration->Write( wxT( "maxwidth" ),
        (long) XRCCTRL( *this, "channel_dialog_images_tab_thumbnail_if_bigger_width_spinctrl", wxSpinCtrl )->GetValue() );

    the_configuration->Write( wxT( "maxheight" ),
        (long) XRCCTRL( *this, "channel_dialog_images_tab_thumbnail_if_bigger_height_spinctrl", wxSpinCtrl )->GetValue() );

    // Some GUI tricks here. Always link by setting a very high number, Never link by setting
    // to zero. Else use the specified value.

    long alt_maxwidth_value   = 0;
    long alt_maxheight_value  = 0;

    if ( XRCCTRL( *this, "channel_dialog_images_tab_only_thumbnail_radiobutton", wxRadioButton )
                ->GetValue() == TRUE ) 
    {
        alt_maxwidth_value  = plkrIMAGES_ONLY_THUMBNAIL;
        alt_maxheight_value = plkrIMAGES_ONLY_THUMBNAIL;
    } 
    else if ( XRCCTRL( *this, "channel_dialog_images_tab_only_link_radiobutton", wxRadioButton )
                ->GetValue() == TRUE ) 
    {
        alt_maxwidth_value  = XRCCTRL( *this, "channel_dialog_images_tab_only_link_width_spinctrl", wxSpinCtrl )
                ->GetValue();
        alt_maxheight_value = XRCCTRL( *this, "channel_dialog_images_tab_only_link_height_spinctrl", wxSpinCtrl )
                ->GetValue();
    } 
    else 
    {
        alt_maxwidth_value  = plkrIMAGES_ALWAYS_LINK;
        alt_maxheight_value = plkrIMAGES_ALWAYS_LINK;
    }
    the_configuration->Write( wxT( "alt_maxwidth" ), alt_maxwidth_value );
    the_configuration->Write( wxT( "alt_maxheight" ), alt_maxheight_value );

    int try_reduce_index = XRCCTRL( *this, "channel_dialog_images_tab_try_reduce_choice", wxChoice )
        ->GetSelection();
    switch ( try_reduce_index )
    {
        case 0:
            the_configuration->Write( wxT( "try_reduce_bpp" ), 0L );
            the_configuration->Write( wxT( "try_reduce_dimension" ), 0L );
            break;
        case 1:
            the_configuration->Write( wxT( "try_reduce_bpp" ), 0L );
            the_configuration->Write( wxT( "try_reduce_dimension" ), 1L );
            break;
        case 2:
            the_configuration->Write( wxT( "try_reduce_bpp" ), 1L );
            the_configuration->Write( wxT( "try_reduce_dimension" ), 0L );
            break;
    }

    wxLogDebug( wxT( "Wrote channel_dialog images tab values to config file" ) );

    //------Output Tab------------------------------------------------------------------

    // DOC is choice index 0, ZLIB is choice index 1, so just cast to boolean directly.
    // compression is the new key, zlib_compression is the old one.
    if ( XRCCTRL( *this, "channel_dialog_output_tab_compression_choice", wxChoice )
                            ->GetSelection() == 0 )
    {
        the_configuration->Write( wxT( "compression" ), wxT( "doc" ) );
    } 
    else 
    {
        the_configuration->Write( wxT( "compression" ), wxT( "zlib" ) );
    }

    if ( XRCCTRL( *this, "channel_dialog_output_tab_only_compress_checkbox", wxCheckBox )
            ->GetValue() == TRUE )
    {
        the_configuration->Write( wxT( "image_compression_limit" ),
            (long) XRCCTRL( *this, "channel_dialog_output_tab_only_compress_spinctrl", wxSpinCtrl )->GetValue() );
    }
    else
    {
        the_configuration->Write( wxT( "image_compression_limit" ), 0L );
    }

    if ( XRCCTRL( *this, "channel_dialog_output_tab_category_checkbox", wxCheckBox )
            ->GetValue() == TRUE )
    {
        the_configuration->Write( wxT( "category" ),
            XRCCTRL( *this, "channel_dialog_output_tab_category_textctrl", wxTextCtrl )->GetValue() );
    } 
    else
    {
        the_configuration->Write( wxT( "category" ), wxT( "" ) );
    }

    // Note the ! in there, as the GUI (as GUIs should) asks in the positive, not negative
    the_configuration->Write( wxT( "no_urlinfo" ),
        ! (bool) XRCCTRL( *this, "channel_dialog_output_tab_include_url_checkbox", wxCheckBox )->GetValue() );

    if ( XRCCTRL( *this, "channel_dialog_output_tab_owner_id_checkbox", wxCheckBox )
            ->GetValue() == TRUE ) 
    {
        the_configuration->Write( wxT( "owner_id_build" ),
            XRCCTRL( *this, "channel_dialog_output_tab_owner_id_textctrl", wxTextCtrl )->GetValue() );
    } 
    else 
    {
        the_configuration->Write( wxT( "owner_id_build" ), wxT( "" ) );
    }

    // Note the ! in there, as the GUI (as GUIs should) asks in the positive, not negative
    the_configuration->Write( wxT( "copyprevention_bit" ),
        ! (bool) XRCCTRL( *this, "channel_dialog_output_tab_beamable_checkbox", wxCheckBox )->GetValue() );

    the_configuration->Write( wxT( "backup_bit" ),
        (bool) XRCCTRL( *this, "channel_dialog_output_tab_backup_checkbox", wxCheckBox )->GetValue() );

    the_configuration->Write( wxT( "launchable_bit" ),
        (bool) XRCCTRL( *this, "channel_dialog_output_tab_launchable_checkbox", wxCheckBox )->GetValue() );

    if ( XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_checkbox", wxCheckBox )
            ->GetValue() == TRUE )
    {
        the_configuration->Write( wxT( "big_icon" ),
            XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_textctrl", wxTextCtrl )->GetValue() );
    } 
    else 
    {
        the_configuration->Write( wxT( "big_icon" ), wxT( "" ) );
    }

    if ( XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_checkbox", wxCheckBox )
            ->GetValue() == TRUE ) 
    {
        the_configuration->Write( wxT( "small_icon" ),
            XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_textctrl", wxTextCtrl )->GetValue() );
    } 
    else 
    {
        the_configuration->Write( wxT( "small_icon" ), wxT( "" ) );
    }

    wxLogDebug( wxT( "Wrote channel_dialog output tab values to config file" ) );

    //------Destination Tab--------------------------------------------------------------------

    wxLogDebug( wxT( "About to write user destinations." ) );
    // Write the handheld destinations:
    handheld_dest_array_type handheld_dest_array;
    m_handheld_install_ctrl->GetUserDestinations( handheld_dest_array );

    handheld_dest_wrappers::write_handheld_dest_array_to_configuration( m_configuration_section,
                                                                        handheld_dest_array );
    // The WX_CLEAR_ARRAY(array) macro [see dynarray.h] is required to clear out all the
    // elements from memory of an array of structs that was built with WX_DEFINE_ARRAY.
    // All that this macro does is just to loop through the array, "delete" each element
    // to free it from memory, then mark the array as Empty().
    WX_CLEAR_ARRAY( handheld_dest_array );

    wxLogDebug( wxT( "Wrote user destinations" ) );

    wxString      sync_directories_string;
    wxArrayString sync_directories_array;
    m_handheld_install_ctrl->GetDirectoryDestinations( sync_directories_array );
    sync_directories_string = utils_string::arraystring_to_string( sync_directories_array, plkrOS_FILE_ARRAY_SEPARATOR );
    sync_directories_array.Clear();
    the_configuration->Write( wxT( "copy_to_dir" ), sync_directories_string );

    wxString doc_file;
    // Autogenerate a unique doc_file_filename, based on the section name.
    // Use a relative filename and parser will save in relative to PLUCKERDIR.
    // NOTE: This can't be "channels" + '/' +.. or will fail (since can't store a / in
    // "channels" I guess. If try it, a "test" channel comes out as tctrltest/test
    doc_file << wxT( "channels" ) << wxT( "/" ) << m_configuration_section << wxT( "/" ) << m_configuration_section;

    wxLogDebug( wxT( "Writing out doc_file=" ) + doc_file );
    the_configuration->Write( wxT( "doc_file" ), doc_file );

    wxLogDebug( wxT( "Wrote channel_dialog destinations tab values to config file" ) );

    //------Scheduling Tab--------------------------------------------------------------

    the_configuration->Write( wxT( "update_enabled" ),
        (bool) XRCCTRL( *this, "channel_dialog_scheduling_tab_update_enabled_checkbox", wxCheckBox )->GetValue() );

    int update_frequency;
    update_frequency = XRCCTRL( *this, "channel_dialog_scheduling_tab_update_frequency_spinctrl", wxSpinCtrl )
                           ->GetValue();
    the_configuration->Write( wxT( "update_frequency" ), (long)update_frequency );

    wxString update_period;
    int update_period_index;
    update_period_index = XRCCTRL( *this, "channel_dialog_scheduling_tab_update_period_choice", wxChoice )
                              ->GetSelection();
    switch ( update_period_index ) 
    {
        case 0:
            update_period = wxT( "hourly" );
            break;
        case 1:
            update_period = wxT( "daily" );
            break;
        case 2:
            update_period = wxT( "weekly" );
            break;
        case 3:
            update_period = wxT( "monthly" );
            break;
        case 4:
            update_period = wxT( "yearly" );
            break;
    }
    the_configuration->Write( wxT( "update_period" ), update_period );


    wxDateTime update_base;
    wxString update_base_string;
    update_base = XRCCTRL( *this, "channel_dialog_scheduling_tab_update_base_calendarctrl", wxCalendarCtrl )->GetDate();

    update_base.SetHour( m_scheduling_time_control->get_hour() );
    update_base.SetMinute( m_scheduling_time_control->get_minute() );
    // Just so that all times have a crisp even 0 seconds.
    update_base.SetSecond( 0 );

    // In utils_datetime.h
    update_base_string = utils_datetime::datetime_to_w3c_string( update_base );
    the_configuration->Write( wxT( "update_base" ), update_base_string );

    wxLogDebug( wxT( "Wrote channel_dialog scheduling tab values to config file" ) );

    //----------------------------------------------------------------------------------
    // Flush() writes the things to the file immediately, instead of waiting for program
    // to exit to write them.
    the_configuration->Flush();
}

void channel_dialog::launch_editor( const wxString& filename_to_load, long style )
{
    wxString    filename = filename_to_load;
    wxTextFile  textfile;
    bool        successful = false;
    
    if ( filename == wxT( "" ) ) 
    {
        return;
    }

    // Convert to proper format
    filename = protocol_filename_to_normal_fullname( filename );
    
    // If the file isn't found, then do the desired behaviour    
    if ( ! wxFileExists( filename ) ) 
    {  
        wxMessageDialog prompt_messagedialog( this,
                                _( "The specified file could not be found. Do you wish to create it?" ),
                                _( "Create new html file" ),
                                wxYES | wxNO | wxYES_DEFAULT | wxICON_QUESTION ); 
        switch ( style ) 
        {    
            case plkrPROMPT_IF_FILE_NOT_FOUND:                              
                if ( prompt_messagedialog.ShowModal() == wxID_YES ) 
                {
                    successful = textfile.Create( filename );                    
                }  
                break;    

            case plkrCREATE_IF_FILE_NOT_FOUND:                
                    successful = textfile.Create( filename );           
                break;

            case plkrERROR_IF_FILE_NOT_FOUND:
                wxLogError( wxT( "Error: " ) + filename + wxT( " not found" ) );
                successful = false;
                break;        
        }
    
    // It exists, so open it so we can check it for number of lines
    } 
    else
    {
        successful = textfile.Open( filename ); 
    }
    
    // Abort if failed to create / open file
    if ( ! successful ) 
    {
        textfile.Close();
        return;
    }
    
    // If empty then, add a few lines, so STC doesn't whine about a null string.
    if ( (int)textfile.GetLineCount() == 0 )
    {
        textfile.AddLine( wxT( "<html>" ) );
        textfile.AddLine( wxT( "<body>" ) );
        textfile.AddLine( wxT( "</body>" ) );
        textfile.AddLine( wxT( "</html>" ) );
        // wxTextFile::Write() is like wxConfigBase::Flush(). It saves it to file.
        wxLogDebug( wxT( "About to write changes" ) );
        textfile.Write();
    }
    // Close the file. We are ready to go.
    textfile.Close();
    
    // Read external editor command from config
    wxString html_editor_command = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/html_editor" ), wxT( "" ) );
    
    // If specified a custom editor, or the integrated editor isn't compiled in, then launch that
    if ( ( html_editor_command != wxT( "" ) ) || ! setupUSE_INTEGRATED_HTML_EDITOR )
    {
        // Look to see if the editor has the %f string, and if so then replace the %f
        // with the filename to load
        if ( html_editor_command.Contains( plkrSHELL_COMMAND_REPLACEMENT_FILENAME ) ) 
        {
           html_editor_command.Replace( plkrSHELL_COMMAND_REPLACEMENT_FILENAME, 
                                        filename, TRUE );
        }
        else
        {
           // Wasn't a filename macro. Just tack on a space and then the filename on the end.
           html_editor_command << wxT( " " ) << filename;
        }        
        
        // Command is now ready to go. Execute it. FALSE means don't wait it to 
        // to finish before returning control over to us.
        wxExecute( html_editor_command, FALSE );
                
    } 
    else 
    {
    
#if ( setupUSE_INTEGRATED_HTML_EDITOR  )
        // Otherwise launch the Plucker Desktop editor                
        editor_dialog a_editor_dialog( this, filename );
        a_editor_dialog.ShowModal();        
#endif
        
    }
    
}

//----------------------------------------------------------------------------------------
// Private members: event handlers
//----------------------------------------------------------------------------------------

//------Start Tab-------------------------------------------------------------------------

void channel_dialog::on_update_ui_start_tab_url_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_start_tab_url_radiobutton", wxRadioButton )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_start_tab_url_textctrl", wxTextCtrl )
            ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_start_tab_url_button", wxButton )
            ->Enable( enabled );
            
    wxLogDebug( wxT( "channel_dialog::on_update_ui_start_tab_url_radiobutton" ) ); 
}


void channel_dialog::on_update_ui_start_tab_file_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_start_tab_file_radiobutton", wxRadioButton )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_start_tab_file_textctrl", wxTextCtrl )
            ->Enable( enabled );
   
	XRCCTRL( *this, "channel_dialog_start_tab_file_button", wxButton )
            ->Enable( enabled );
/*
	  XRCCTRL( *this, "channel_dialog_start_tab_edit_file_button", wxButton )
            ->Enable( enabled );
*/            
    wxLogDebug( wxT( "channel_dialog::on_update_ui_start_tab_file_radiobutton" ) );        
}


void channel_dialog::on_update_ui_start_tab_links_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_start_tab_links_radiobutton", wxRadioButton )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_start_tab_links_button", wxButton )
            ->Enable( enabled );
            
    wxLogDebug( wxT( "channel_dialog::on_update_ui_start_tab_links_radiobutton" ) );
}


void channel_dialog::on_start_tab_url_button( wxCommandEvent &event )
{

#if ( setupUSE_SHOWCASE )

    showcase_dialog a_showcase_dialog( this );
    
    // Only execute this, if user pressed OK
    if ( a_showcase_dialog.ShowModal() == wxID_OK ) 
    {
        wxString    name;
        wxString    url;
        wxString    description;
        wxString    language;
        wxString    category;
        long        update_frequency;
        wxString    update_period;
        wxDateTime  update_base;
        bool        update_enabled;
        long        bpp;
        long        maxdepth;
        bool        stayonhost;
        bool        stayondomain;
        wxString    url_pattern;
        
        // Show a showcase dialog and collect the selected rows fields into these variables       
        a_showcase_dialog.transfer_to( name, url, description, language, category,
                                       update_frequency, update_period, update_base, 
                                       update_enabled, bpp, maxdepth, stayonhost, 
                                       stayondomain, url_pattern );
        // Set name and url to the returned data
        XRCCTRL( *this, "channel_dialog_textctrl", wxTextCtrl )->SetValue( name );        
        XRCCTRL( *this, "channel_dialog_start_tab_url_textctrl", wxTextCtrl )->SetValue( url );  
    
        // Only set the rest if user so asked, in the configuration file.
        if ( the_configuration->Read( wxT( "showcase_dialog_insert_all_parameters" ), 1L ) == 1 )
        {    
            XRCCTRL( *this, "channel_dialog_output_tab_category_textctrl", wxTextCtrl )
                ->SetValue ( category );
            if ( category != wxT( "" ) )
            {
                XRCCTRL( *this, "channel_dialog_output_tab_category_checkbox", wxCheckBox )
                    ->SetValue ( TRUE ); 
            }
            
            if ( bpp != -1 ) 
            {
                set_image_tab_bpp_controls( bpp );
            }
            
            if ( update_frequency != -1 ) 
            {
            set_scheduling_tab_update_controls( update_frequency, update_period, 
                                                update_base, update_enabled );            
            }
            
            // Tests for a -1 return. Also prevents an accidental maxdepth=0 that someone
            // wrote in the showcase listings.
            if ( 0 < maxdepth) 
            {
                XRCCTRL( *this, "channel_dialog_limits_tab_maxdepth_spinctrl", wxSpinCtrl )
                    ->SetValue( maxdepth ); 
            }
            
            set_limits_tab_stayonhost_stayondomain_controls( stayonhost, stayondomain );
 
            XRCCTRL( *this, "channel_dialog_limits_tab_url_pattern_textctrl", wxTextCtrl )
                    ->SetValue( url_pattern );
            if ( url_pattern != wxT( "" ) ) 
            {
                XRCCTRL( *this, "channel_dialog_limits_tab_url_pattern_checkbox", wxCheckBox )
                    ->SetValue ( TRUE );
            }
        }        
    }

#endif
    
}


void channel_dialog::on_update_ui_start_tab_file_textctrl( wxUpdateUIEvent &event )
{
    bool desired_button_enabled_state;
    bool button_is_enabled;  
    
    // Enable the edit button, if textctrl is enabled, and it has at least one character,
    // which is not just "file://"         
    desired_button_enabled_state = (bool) XRCCTRL( *this, "channel_dialog_start_tab_file_textctrl", wxTextCtrl )
                                        ->GetLineLength( 0 ) && 
                                   ( XRCCTRL( *this, "channel_dialog_start_tab_file_textctrl", wxTextCtrl )
                                        ->GetValue() != wxT( "file://" ) ) && 
                                   XRCCTRL( *this, "channel_dialog_start_tab_file_textctrl", wxTextCtrl )
                                        ->IsEnabled();                  

    button_is_enabled = XRCCTRL( *this, "channel_dialog_start_tab_edit_file_button", wxButton )
                            ->IsEnabled();
                         
    // Only change the enabled state of the button if it doesn't match the current.
    // Otherwise will get a nasty flicker. 
    if ( button_is_enabled != desired_button_enabled_state ) 
    {
        wxLogDebug( wxT( "Edit file button doesn't match desired state so updating" ) );
        XRCCTRL( *this, "channel_dialog_start_tab_edit_file_button", wxButton )
            ->Enable( desired_button_enabled_state );
    }
            
    wxLogDebug( wxT( "channel_dialog::on_update_ui_start_tab_file_textctrl" ) );
}


void channel_dialog::on_start_tab_file_button( wxCommandEvent &event )
{    
    // [utils_controls.cpp: Includes the code to automatically add/remove the 
    // file:// prefix as required] 
    utils_controls::textctrl_filename_selection( XRCCTRL( *this, "channel_dialog_start_tab_file_textctrl", wxTextCtrl ),
                                 _( "Choose start file" ),
                                 wxT( "" ), 
                                 wxT( "" ),
                                 FILE_DIALOG_WILDCARDS_PLUCKABLES,
                                 false,
                                 plkrOS_FILE_ARRAY_SEPARATOR,
                                 true );       
}


void channel_dialog::on_start_tab_edit_file_button( wxCommandEvent &event )
{
    wxString filename_to_load;
    
    filename_to_load = XRCCTRL( *this, "channel_dialog_start_tab_file_textctrl", wxTextCtrl )
              ->GetValue();

    launch_editor( filename_to_load, plkrPROMPT_IF_FILE_NOT_FOUND );
    
}


void channel_dialog::on_start_tab_links_button( wxCommandEvent &event )
{    
    wxString home_url_key = wxT( "/" ) + m_configuration_section + wxT( "/" ) + wxT( "home_url" );
    wxString home_url = the_configuration->Read( home_url_key, _( "" ) );    

    // If no url specified: must be the default plucker home.html HTML page, unless it
    // is the plkrNEW_CHANNEL_TEMPLATE_SECTION, to which we want it to load its own
    if ( m_configuration_section != plkrNEW_CHANNEL_TEMPLATE_SECTION &&
        ( home_url == wxT( "" ) || home_url == wxT( "plucker:/home.html" ) ) ) 
    {
        home_url = get_plucker_directory( PLUCKERHOME ) << wxT( "/home.html" );
    } 
    else 
    {
        home_url = get_plucker_directory( CHANNELS ) << wxT( "/" ) << m_configuration_section << wxT( "/home.html" );
    }

    // Launch the editor
    launch_editor( home_url, plkrCREATE_IF_FILE_NOT_FOUND );
        
}

//------Spidering Tab-------------------------------------------------------------------------

void channel_dialog::on_update_ui_spidering_tab_retrieval_timeout_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_spidering_tab_retrieval_timeout_checkbox", wxCheckBox )
            ->GetValue();             
    XRCCTRL( *this, "channel_dialog_spidering_tab_retrieval_timeout_spinctrl", wxSpinCtrl )
            ->Enable( enabled );

    wxLogDebug( wxT( "on_update_ui_spidering_tab_retrieval_timeout_checkbox" ) );
}

void channel_dialog::on_update_ui_spidering_tab_status_line_length_checkbox( wxUpdateUIEvent& event )
{
    bool enabled;

    enabled = XRCCTRL( *this, "channel_dialog_spidering_tab_status_line_length_checkbox", wxCheckBox )
                ->GetValue();
    XRCCTRL( *this, "channel_dialog_spidering_tab_status_line_length_spinctrl", wxSpinCtrl )
            ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_spidering_tab_status_line_length_checkbox" ) );
}


void channel_dialog::on_update_ui_spidering_tab_referrer_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;

    enabled = XRCCTRL( *this, "channel_dialog_spidering_tab_referrer_checkbox", wxCheckBox )
                ->GetValue();
    XRCCTRL( *this, "channel_dialog_spidering_tab_referrer_textctrl", wxTextCtrl )
        ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_spidering_tab_referrer_checkbox" ) );
}


void channel_dialog::on_update_ui_spidering_tab_user_agent_browser_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_browser_radiobutton", wxRadioButton )
                ->GetValue();             
    XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_browser_choice", wxChoice )
        ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_spidering_tab_user_agent_browser_radiobutton" ) );
}


void channel_dialog::on_update_ui_spidering_tab_user_agent_custom_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_custom_radiobutton", wxRadioButton )
                ->GetValue();             
    XRCCTRL( *this, "channel_dialog_spidering_tab_user_agent_custom_textctrl", wxTextCtrl )
        ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_spidering_tab_user_agent_custom_radiobutton" ) );
}

//------Limits Tab-------------------------------------------------------------------------

void channel_dialog::on_update_ui_limits_tab_stayonhost_stayondomain_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_limits_tab_stayonhost_stayondomain_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_limits_tab_stayondomain_radiobutton", wxRadioButton)
            ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_limits_tab_stayonhost_radiobutton", wxRadioButton)
            ->Enable( enabled );    

    wxLogDebug( wxT( "channel_dialog::on_update_ui_limits_tab_stayonhost_stayondomain_checkbox" ) );
}


void channel_dialog::on_update_ui_limits_tab_url_pattern_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_limits_tab_url_pattern_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_limits_tab_url_pattern_textctrl", wxTextCtrl )
            ->Enable( enabled );      

    wxLogDebug( wxT( "channel_dialog::on_update_ui_limits_tab_url_pattern_checkbox" ) );
}


void channel_dialog::on_update_ui_limits_tab_exclusion_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_limits_tab_exclusion_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_limits_tab_exclusion_button", wxButton )
            ->Enable( enabled );      

    wxLogDebug( wxT( "channel_dialog::on_update_ui_limits_tab_exclusion_checkbox" ) );
}


void channel_dialog::on_limits_tab_exclusion_button( wxCommandEvent &event )
{
#if ( setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR )
    // Boot up an exclusion dialog, showing the exclusion list for this channel.
    exclusion_dialog a_exclusion_dialog( this, m_configuration_section );
    a_exclusion_dialog.ShowModal();
#endif
}

//------Formatting Tab--------------------------------------------------------------------

void channel_dialog::on_update_ui_formatting_tab_charset_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_formatting_tab_charset_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_formatting_tab_charset_combobox", wxComboBox )
            ->Enable( enabled );      

    wxLogDebug( wxT( "channel_dialog::on_update_ui_formatting_tab_charset_checkbox" ) );
}


void channel_dialog::on_update_ui_formatting_tab_anchor_color_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_formatting_tab_anchor_color_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_formatting_tab_anchor_color_textctrl", wxTextCtrl )
            ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_formatting_tab_anchor_color_button", wxButton )
            ->Enable( enabled );        

    wxLogDebug( wxT( "channel_dialog::on_update_ui_formatting_tab_anchor_color_checkbox" ) );
}


void channel_dialog::on_formatting_tab_anchor_color_button( wxCommandEvent &event )
{
    // [utils_controls.cpp]    
    utils_controls::textctrl_hex_color_selection( XRCCTRL( *this, "channel_dialog_formatting_tab_anchor_color_textctrl", wxTextCtrl ) );
}

//------Images Tab-------------------------------------------------------------------------

void channel_dialog::on_update_ui_images_tab_include_images_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_images_tab_include_images_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_images_tab_depth_choice", wxChoice )
            ->Enable( enabled );      

    wxLogDebug( wxT( "channel_dialog::on_update_ui_images_tab_include_images_checkbox" ) );
}


void channel_dialog::on_update_ui_images_tab_only_link_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_images_tab_only_link_radiobutton", wxRadioButton )
            ->GetValue();
    XRCCTRL( *this, "channel_dialog_images_tab_only_link_width_spinctrl", wxSpinCtrl )
            ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_images_tab_only_link_height_spinctrl", wxSpinCtrl )
            ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_images_tab_only_link_radiobutton" ) );
}

//------Output Tab-------------------------------------------------------------------------

void channel_dialog::on_update_ui_output_tab_only_compress_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_output_tab_only_compress_checkbox", wxCheckBox )
            ->GetValue();             
    XRCCTRL( *this, "channel_dialog_output_tab_only_compress_spinctrl", wxSpinCtrl )
            ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_output_tab_only_compress_checkbox" ) );
}


void channel_dialog::on_update_ui_output_tab_category_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_output_tab_category_checkbox", wxCheckBox )
                ->GetValue();             
    XRCCTRL( *this, "channel_dialog_output_tab_category_textctrl", wxTextCtrl )
        ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_output_tab_category_checkbox" ) );
}


void channel_dialog::on_update_ui_output_tab_owner_id_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_output_tab_owner_id_checkbox", wxCheckBox )
                ->GetValue();             
    XRCCTRL( *this, "channel_dialog_output_tab_owner_id_textctrl", wxTextCtrl )
        ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_output_tab_owner_id_checkbox" ) );
}


void channel_dialog::on_update_ui_output_tab_launchable_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_output_tab_launchable_checkbox", wxCheckBox )
                ->GetValue();             
    XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_checkbox", wxCheckBox )
        ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_checkbox", wxCheckBox )
        ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_output_tab_launchable_checkbox" ) );
}


void channel_dialog::on_update_ui_output_tab_custom_large_icon_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
    
    // Only enable children if this checkbox is both checked and enabled    
    enabled = ( XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_checkbox", wxCheckBox )
                ->GetValue() &&
               XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_checkbox", wxCheckBox )
                ->IsEnabled() );
    XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_textctrl", wxTextCtrl )
        ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_button", wxButton )
        ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_output_tab_custom_large_icon_checkbox" ) );
}


void channel_dialog::on_update_ui_output_tab_custom_small_icon_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
    
    // Only enable children if this checkbox is both checked and enabled  
    enabled = ( XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_checkbox", wxCheckBox )
                ->GetValue() &&
               XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_checkbox", wxCheckBox )
                ->IsEnabled() );      
    XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_textctrl", wxTextCtrl )
        ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_button", wxButton )
        ->Enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_output_tab_custom_small_icon_checkbox" ) );
}


void channel_dialog::on_output_tab_custom_large_icon_button( wxCommandEvent &event )
{
    // There are some sample icons in the plucker desktop resources
    wxString default_directory = get_plucker_directory( RESOURCES ) << wxT( "/launcher_icons_large" );
    
    utils_controls::textctrl_filename_selection( XRCCTRL( *this, "channel_dialog_output_tab_custom_large_icon_textctrl", wxTextCtrl ),
                                 _( "Choose large icon" ),
                                 wxT( "" ),
                                 default_directory,
                                 FILE_DIALOG_WILDCARDS_HANDHELD_LAUNCHER_ICONS,
                                 false,
                                 plkrOS_FILE_ARRAY_SEPARATOR,
                                 true ); 
}


void channel_dialog::on_output_tab_custom_small_icon_button( wxCommandEvent &event )
{ 
    // There are some sample icons in the plucker desktop resources
    wxString default_directory = get_plucker_directory( RESOURCES ) << wxT( "/launcher_icons_small" );
    
    utils_controls::textctrl_filename_selection( XRCCTRL( *this, "channel_dialog_output_tab_custom_small_icon_textctrl", wxTextCtrl ),
                                 _( "Choose small icon" ),
                                 wxT( "" ),
                                 default_directory,
                                 FILE_DIALOG_WILDCARDS_HANDHELD_LAUNCHER_ICONS,
                                 false,
                                 plkrOS_FILE_ARRAY_SEPARATOR,
                                 true ); 

}

//------Scheduling Tab---------------------------------------------------------------------

void channel_dialog::on_update_ui_scheduling_tab_update_enabled_checkbox( wxUpdateUIEvent &event )
{
    
    wxLogDebug( wxT( "*Entering* channel_dialog::on_update_ui_scheduling_tab_update_enabled_checkbox" ) );
    
    bool enabled;
        
    enabled = XRCCTRL( *this, "channel_dialog_scheduling_tab_update_enabled_checkbox", wxCheckBox )
                ->GetValue();             
    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_frequency_spinctrl", wxSpinCtrl )
        ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_period_choice", wxChoice )
        ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_base_calendarctrl", wxCalendarCtrl )
        ->Enable( enabled );
    XRCCTRL( *this, "channel_dialog_scheduling_tab_now_button", wxButton )
        ->Enable( enabled );
/*
    // Commented out since using the time control instead of an hour-choice.
    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_base_choice", wxChoice )
        ->Enable( enabled );
*/
    wxLogDebug( wxT( "About to enable/disable time control" ) );
    m_scheduling_time_control->enable( enabled );

    wxLogDebug( wxT( "channel_dialog::on_update_ui_scheduling_tab_update_enabled_checkbox" ) );
}
    
void channel_dialog::on_scheduling_tab_now_button( wxCommandEvent &event )
{
    wxDateTime current_datetime;
    // In utils_datetime.h  
    current_datetime = utils_datetime::get_current_datetime();
    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_base_calendarctrl", wxCalendarCtrl )->SetDate( current_datetime );
/*
    wxDateTime::wxDateTime_t current_datetime_hour;
    XRCCTRL( *this, "channel_dialog_scheduling_tab_update_base_choice", wxChoice )->SetSelection( (int) current_datetime_hour );
*/
    m_scheduling_time_control->set_time( current_datetime );
}

//----------------------------------------------------------------------------------------

// Override wxDialog's default behavior for clicking an OK button.
void channel_dialog::OnOK( wxCommandEvent& event )
{    
    // Check to see that there is at least one destination chosen. If not, then
    // prevent the close. Otherwise, end the modal.
    if ( ! prevent_close_if_no_destinations_chosen() ) 
    {
        // Write out the values of the dialog's controls to the configuration file.
        write_configuration();
        // Close the dialog
        EndModal( wxID_OK );
    }
}


void channel_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    int selected_notebook_page_index;
    
    selected_notebook_page_index  = XRCCTRL( *this, "channel_dialog_notebook", wxNotebook )
                                        ->GetSelection();    

    switch ( selected_notebook_page_index ) 
    {       
        case CHANNEL_NOTEBOOK_PAGE_INDEX_START:
            help_controller::get()->show_help_topic( plkrHELP_ID_CHANNEL_DIALOG_START_TAB );
            break;
        case CHANNEL_NOTEBOOK_PAGE_INDEX_SPIDERING:
            help_controller::get()->show_help_topic( plkrHELP_ID_CHANNEL_DIALOG_SPIDERING_TAB );
            break;
        case CHANNEL_NOTEBOOK_PAGE_INDEX_LIMITS:
            help_controller::get()->show_help_topic( plkrHELP_ID_CHANNEL_DIALOG_LIMITS_TAB );
            break;
        case CHANNEL_NOTEBOOK_PAGE_INDEX_FORMATTING:
            help_controller::get()->show_help_topic( plkrHELP_ID_CHANNEL_DIALOG_FORMATTING_TAB );
            break;
        case CHANNEL_NOTEBOOK_PAGE_INDEX_IMAGES:
            help_controller::get()->show_help_topic( plkrHELP_ID_CHANNEL_DIALOG_IMAGES_TAB );
            break;
        case CHANNEL_NOTEBOOK_PAGE_INDEX_OUTPUT:
            help_controller::get()->show_help_topic( plkrHELP_ID_CHANNEL_DIALOG_OUTPUT_TAB );
            break;
        case CHANNEL_NOTEBOOK_PAGE_INDEX_DESTINATION:
            help_controller::get()->show_help_topic( plkrHELP_ID_CHANNEL_DIALOG_DESTINATION_TAB );
            break;
        case CHANNEL_NOTEBOOK_PAGE_INDEX_SCHEDULING:
            help_controller::get()->show_help_topic( plkrHELP_ID_CHANNEL_DIALOG_SCHEDULING_TAB );
            break;           
        default:
            break;
    }
    wxLogDebug( wxT( "Selected notebook page index=%d" ), selected_notebook_page_index );

#endif
}


// Checks to see that some destinations have been chosen in the destination tab. Returns
// whether or to to prevent the closing of the dialog
bool channel_dialog::prevent_close_if_no_destinations_chosen()
{

wxLogDebug( wxT( "Entering channel_dialog::prevent_close_if_no_destinations_chosen()" ) );

    bool prevent_close = FALSE;

#if ( setupUSE_CHECKBOX_MESSAGE_DIALOG )

    // If all the desinations are emtpy...
    if ( m_handheld_install_ctrl->IsAllDestinationsEmpty() ) 
    {
        bool default_checked = TRUE;
        wxString show_message_path_key = wxT( "/PLUCKER_DESKTOP/message_dialog_enabled_no_destinations_selected" );

        // ...and if want to show the warnings....
        if ( the_configuration->Read( show_message_path_key, (long)default_checked ) == (long)default_checked )
        {
            checkbox_message_dialog a_checkbox_message_dialog( this,
            _( "No destinations are selected for this channel. Add some destinations now?" ),
            _( "No destinations selected" ), wxYES | wxNO|wxCENTRE|wxICON_QUESTION,
            wxDefaultPosition, _( "Always perform this check when closing this dialog" ),
            show_message_path_key,
            default_checked );

            // .. and they clicked the OK box in the warning...
            if ( a_checkbox_message_dialog.ShowModal() == wxID_YES )
            {
                // ...then set the tab to the destinations tab...
                XRCCTRL( *this, "channel_dialog_notebook", wxNotebook )
                    ->SetSelection( CHANNEL_NOTEBOOK_PAGE_INDEX_DESTINATION );
                // ..and stop the dialog from closing
                prevent_close = TRUE;
            }

        }

    }

#endif  //setupUSE_CHECKBOX_MESSAGE_DIALOG
    wxLogDebug( wxT( "Completing channel_dialog::prevent_close_if_no_destinations_chosen()" ) );
    return prevent_close;
}
