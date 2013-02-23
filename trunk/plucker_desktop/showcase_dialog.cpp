//----------------------------------------------------------------------------------------
// Name:        showcase_dialog.cpp
// Purpose:     Plucker channel showcase dialog (renders showcase.htm)
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/26
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: showcase_dialog.cpp,v 1.37 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "showcase_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_SHOWCASE )

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

#include "showcase_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/html/m_templ.h"        // Custom HTML tag handler macros
#include "wx/html/htmlwin.h"        // wxHtmlWindow
#include "wx/listctrl.h"            // wxListCtrl
#include "wx/splitter.h"            // wxSplitterWindow
#include "wx/fs_inet.h"             // wxInternetFSHandler [wxHtmlWindow->LoadPage(http:)]
#include "wx/file.h"                // wxFile

// ---------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

// ---------------------------------------------------------------------------------------

#include "configuration.h"
#include "image_list.h"
#include "utils_controls.h"        
#include "utils_string.h"           
#include "utils_datetime.h"
#include "help_controller.h"

// ---------------------------------------------------------------------------------------
// Private variables
// ---------------------------------------------------------------------------------------

//! \todo SOURCE: Remove this once move the loading of listctrl data into its own routine.
long m_showcase_listctrl_row_number      = 0;    // Listctrl row to insert the channel info 

// ---------------------------------------------------------------------------------------
// Internal constants
// ---------------------------------------------------------------------------------------

// Easier code maintenance/translations for listctrl's columns. Keep in the same order as the
// m_showcase_field_names.
enum {
    NAME_COLUMN             = 0,
    URL_COLUMN,
    DESCRIPTION_COLUMN,
    LANGUAGE_COLUMN,
    CATEGORY_COLUMN,
    UPDATE_FREQUENCY_COLUMN,
    UPDATE_PERIOD_COLUMN,
    UPDATE_BASE_COLUMN,    
    UPDATE_ENABLED_COLUMN,
    BPP_COLUMN,
    MAXDEPTH_COLUMN,
    STAYONHOST_COLUMN,
    STAYONDOMAIN_COLUMN,
    URL_PATTERN_COLUMN
};

// Creating some controls not from XML file, so need some IDs for the windows
// Window ids, so can track their events
enum {
    SHOWCASE_LISTCTRL                    = 4401,
    DETAILS_HTMLWINDOW,
    MAIN_SPLITTERWINDOW
};

// Minimum pane width that can be moved. This used to be at 30, but for
// whatever reason, it wasn't saving on MAC and MSW, so at least now it can stay
// with a good default.
#define MINIMUM_PANE_WIDTH      150   

// Whether the prefs said vertical or horizontal layout
#define HORIZONTAL_LAYOUT       0
#define VERTICAL_LAYOUT         1

// ---------------------------------------------------------------------------------------
// Custom handler for 'handheldsite' tag
// ---------------------------------------------------------------------------------------

//! \todo SOURCE: Redo this as a loading of configuration files.
// These are macros to make writing custom tag handers for wxHtmlParser much faster.
// See wx/html/m_templ.h for details of the macro's definitions.
// Note that the parameter names of a tag need to be in uppercase. We are writing a custom
// tag handler to parse the showcase.htm file to load it into the listctrl.
TAG_HANDLER_BEGIN(HANDHELDSITE, "HANDHELDSITE")
    TAG_HANDLER_PROC(tag)
    {
    wxString    name;
    wxString    url;
    wxString    language;              // Two letter character code of site's language.
    wxString    category;              // Suggests a category for viewer's library manager too.
    int         bpp              = -1; // Image depth (bits per pixel)
    int         update_frequency = -1; // Time (in seconds) to next update.
	wxString    update_period;
	wxString    update_base;
	bool        update_enabled = true;
	int         maxdepth       = -1;
	bool        stayonhost     = false;
	bool        stayondomain   = false;
	wxString    url_pattern;	
    wxString    description;           // Sentences which describe the channel's content       
    wxString    buf;                   // Buffer for converting integers.
    wxListCtrl* listctrl;              // The listctrl to insert the info
    
    // Parse out parameters of tag and assign them to variables to create a listctrl row.
    if ( tag.HasParam( wxT( "NAME") ) )
        name        = tag.GetParam( wxT( "NAME") );	
    if ( tag.HasParam( wxT( "URL") ) )
        url             = tag.GetParam( wxT( "URL") );	
    if ( tag.HasParam( wxT( "DESCRIPTION") ) )
        description     = tag.GetParam( wxT( "DESCRIPTION") ); 
    if ( tag.HasParam( wxT( "LANGUAGE") ) )
        language        = tag.GetParam( wxT( "LANGUAGE") ); 
    if ( tag.HasParam( wxT( "CATEGORY") ) )
        category        = tag.GetParam( wxT( "CATEGORY") ); 
    if ( tag.HasParam( wxT( "BPP") ) )
        tag.ScanParam( wxT( "BPP"), wxT( "%i"), &bpp );
    if ( tag.HasParam( wxT( "UPDATE_FREQUENCY") ) )
        tag.ScanParam( wxT( "UPDATE_FREQUENCY"), wxT( "%i"), &update_frequency );
    if ( tag.HasParam( wxT( "UPDATE_PERIOD") ) )
        update_period   = tag.GetParam( wxT( "UPDATE_PERIOD") );
    if ( tag.HasParam( wxT( "UPDATE_BASE") ) )
        update_base     = tag.GetParam( wxT( "UPDATE_BASE") );
    if ( tag.HasParam( wxT( "UPDATE_ENABLED") ) )
        update_enabled  = true;
    if ( tag.HasParam( wxT( "MAXDEPTH") ) )
        tag.ScanParam( wxT( "MAXDEPTH"), wxT( "%i"), &maxdepth );
    if ( tag.HasParam( wxT( "STAYONHOST") ) )
        stayonhost      = true;
        if ( tag.HasParam( wxT( "STAYONDOMAIN") ) )
        stayondomain    = true;
    if ( tag.HasParam( wxT( "URL_PATTERN") ) )
        url_pattern     = tag.GetParam( wxT( "URL_PATTERN") );    
    
    wxLogDebug( wxT( "About to get pointer to window for listctrl" ) );    
    // This calls m_WParser member from the wxWinParser which is part of this macro.
    // wxWinParser has a GetWindow() which will tell us what html window we are in, so 
    // we can deduce the parent and thus get the target listctrl.
    // GetWindow, like this, is deprecatd in wxWidgets 2.8, so moving from this: 
    // wxWindow* parent_window = m_WParser->GetWindow()->wxWindow::GetParent();
    // to this (though it doesn't seem to work on WXMAC commandline, but XCode seems OK):
#ifndef __WXMSW__
    wxWindow* parent_window = m_WParser->GetWindowInterface()->GetHTMLWindow()->wxWindow::GetParent();
#else
	// wxWindow* parent_window = m_WParser->GetWindow()->wxWindow::GetParent();
	wxWindow* parent_window = m_WParser->GetWindowInterface()->GetHTMLWindow()->wxWindow::GetParent();
#endif

#ifdef __WXDEBUG__
    listctrl = ( wxDynamicCast( ( *parent_window ).FindWindow( SHOWCASE_LISTCTRL ), wxListCtrl ) );
#else
    listctrl = ( ( wxListCtrl* )( ( *parent_window ).FindWindow( SHOWCASE_LISTCTRL ) ) );
#endif
    wxLogDebug( wxT( "Got pointer to window for listctrl" ) );   
    
    listctrl->InsertItem( m_showcase_listctrl_row_number, name,
                          SMALL_IMAGE_LIST_ID_SHOWCASE_DIALOG_LISTCTRL );
    listctrl->SetItem( m_showcase_listctrl_row_number, URL_COLUMN, url );
    listctrl->SetItem( m_showcase_listctrl_row_number, DESCRIPTION_COLUMN, description );
    listctrl->SetItem( m_showcase_listctrl_row_number, LANGUAGE_COLUMN, language );
    listctrl->SetItem( m_showcase_listctrl_row_number, CATEGORY_COLUMN, category );
    buf.Printf( wxT( "%d" ), bpp );
    listctrl->SetItem( m_showcase_listctrl_row_number, BPP_COLUMN, buf ); 
    buf.Printf( wxT( "%d" ), update_frequency );
    listctrl->SetItem( m_showcase_listctrl_row_number, UPDATE_FREQUENCY_COLUMN, buf );  
    listctrl->SetItem( m_showcase_listctrl_row_number, UPDATE_PERIOD_COLUMN, update_period );  
    listctrl->SetItem( m_showcase_listctrl_row_number, UPDATE_BASE_COLUMN, update_base ); 
    wxString update_enabled_string;
    if ( update_enabled ) {
        update_enabled_string = _( "Yes" );
    } else {
        update_enabled_string = _( "No" );
    }
    listctrl->SetItem( m_showcase_listctrl_row_number, UPDATE_ENABLED_COLUMN, update_enabled_string );
    buf.Printf( wxT( "%d" ), maxdepth );
    listctrl->SetItem( m_showcase_listctrl_row_number, MAXDEPTH_COLUMN, buf ); 
    wxString stayonhost_string;
    if ( stayonhost ) {
        stayonhost_string = _( "Yes" );
    } else {
        stayonhost_string = _( "No" );
    }
    listctrl->SetItem( m_showcase_listctrl_row_number, STAYONHOST_COLUMN, stayonhost_string );
    wxString stayondomain_string;
    if ( stayondomain ) {
        stayondomain_string = _( "Yes" );
    } else {
        stayondomain_string = _( "No" );
    }
    listctrl->SetItem( m_showcase_listctrl_row_number, STAYONDOMAIN_COLUMN, stayondomain_string );
    listctrl->SetItem( m_showcase_listctrl_row_number, URL_PATTERN_COLUMN, url_pattern );

    // Increase the row number, to know where to do next insert.
    wxLogDebug( wxT( "value of m_showcase_listctrl_row_number is now %ld"), m_showcase_listctrl_row_number );
    m_showcase_listctrl_row_number++;
    
    // This return FALSE is part of the custom tag macro.
    return FALSE;
    }
TAG_HANDLER_END(HANDHELDSITE)


TAGS_MODULE_BEGIN(handheldsite)
    TAGS_MODULE_ADD(HANDHELDSITE)
TAGS_MODULE_END(handheldsite)

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(showcase_dialog, wxDialog)
    // Using defined ID number, since not an XML control.
    EVT_LIST_ITEM_SELECTED( SHOWCASE_LISTCTRL, showcase_dialog::on_listctrl_item_selected )
    // A double-click or enter key on a listctrl row.
    EVT_LIST_ITEM_ACTIVATED( SHOWCASE_LISTCTRL, showcase_dialog::on_listctrl_item_activated ) 
    // Override base class wirtuals
    EVT_BUTTON( wxID_OK, showcase_dialog::OnOK )
    EVT_BUTTON( wxID_HELP_CONTEXT, showcase_dialog::on_help_button )
    EVT_SIZE( showcase_dialog::OnSize )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor
showcase_dialog::showcase_dialog( wxWindow* parent )
{
	wxXmlResource::Get()->LoadDialog( this, parent, wxT( "showcase_dialog") );

    // Set up the m_showcase_field_names wxArrayString;
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
    utils_string::string_array_to_arraystring( WXSIZEOF( showcase_field_names ),
                                               showcase_field_names,
                                               &m_showcase_field_names
                                              );
    
    // Initialize (create) the splitters and child window:
    
    // TODO: This should be done by constructors. Have splitterwindow first in list
    // of members in class desc, so it is made first, then stick the children windows
    // onto it.
    init_splitters();
    
    // Read and set the saved values for the controls from the configuration file
    read_configuration();
      
    // Initialize the listctrl
    listctrl_init();
    
    // Set the details htmlwindow background to look like a listbox. 
    // Otherwise it will always be white when it appears.
    wxLogDebug( wxT( "About to set details list box to gui style" ) );
    utils_controls::set_html_window_text_in_gui_style( details_htmlwindow, wxT( ""), 
                         optionSET_HTML_BACKGROUND_TO_MATCH_GUI_LISTBOX );
    wxLogDebug( wxT( "Finished setting details list box to gui style" ) );

/*
#if ( setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS )	
	wxLogDebug( wxT( "Running a setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS" ) );
	wxSize	default_size = GetSize();
	wxSize  new_size;
	new_size.x     = default_size.x + 1;
    new_size.y     = default_size.y + 1;
	SetSize( new_size );
#endif
*/  
	
    // Set the size, position of the frame using our common function for this dialog
    // [utils_controls.cpp]
    utils_controls::read_dialog_position_and_size( this, wxT( "showcase_dialog") );
}


// Destructor
showcase_dialog::~showcase_dialog()
{
    wxLogDebug( wxT( "About to destruct showcase dialog" ) );
    // Save the size and position of the dialog for next time
    // [utils_controls.cpp]
    wxLogDebug( wxT( "About to write showcase dialog position and size" ) );
    utils_controls::write_dialog_position_and_size( this, wxT( "showcase_dialog" ) );

    // Write out the values of the dialog's controls to the configuration file.
    wxLogDebug( wxT( "About to write configuration" ) );
    write_configuration();
    
    // Clear array to free memory.
    wxLogDebug( wxT( "About to write showcase field names" ) );
    m_showcase_field_names.Clear();
    wxLogDebug( wxT( "Destructor of showcase dialog completed" ) );
}


// Transfer a line of channel data to the dialog that called up the showcase dialog
void showcase_dialog::transfer_to(   
    wxString&   name,
    wxString&   url,
    wxString&   description,
    wxString&   language,
    wxString&   category,
    long&       update_frequency,
    wxString&   update_period,
    wxDateTime& update_base,
    bool&       update_enabled,
    long&       bpp,
    long&       maxdepth,
    bool&       stayonhost,
    bool&       stayondomain,
    wxString&   url_pattern )
    
{
    long selected_row_number;  // Selected row in table.
    wxListItem info;           // Stores the properties to find cell, and store its info
    
    // This function can only be executed from within a wxID_OK return code to the parent 
    // dialog, and wxID_OK won't get returned unless there is a selected row, since we checked
    // in showcase_dialog::OnOK() to see if there was a selected row..

    selected_row_number = m_showcase_listctrl->GetNextItem( -1, wxLIST_NEXT_ALL,
                                                          wxLIST_STATE_SELECTED );
    
    info.m_itemId = selected_row_number;
    
    for ( int i = 0; i < (int) m_showcase_field_names.GetCount(); i++ )
    {
        // Set the column of the cell to look for information
        info.m_col = i;
        // Set text mask (don't know why that is here, but seems to be always used).
        info.m_mask = wxLIST_MASK_TEXT;
        // Now, get the properties of the cell we described, and store them all in info.
        m_showcase_listctrl->GetItem( info );
        // We successfully pulled the value from that vell
        wxLogDebug( m_showcase_field_names.Item( i ) + wxT( " column value is %s"), info.m_text.c_str() );
        // Check to see that that the next text isn't just an empty string
        switch( i ) {
            case NAME_COLUMN:    
                name = info.m_text;
                break;
            case URL_COLUMN:
                url = info.m_text;
                break;
            case DESCRIPTION_COLUMN:
                description = info.m_text;
                break;
            case LANGUAGE_COLUMN:
                language = info.m_text;
                break;
            case CATEGORY_COLUMN: 
                category = info.m_text;
                break;
            case UPDATE_FREQUENCY_COLUMN:
                info.m_text.ToLong( &update_frequency );
                break;
            case UPDATE_PERIOD_COLUMN: 
                update_period = info.m_text;
                break;
            case UPDATE_BASE_COLUMN: 
                update_base = utils_datetime::w3c_string_to_datetime( info.m_text );
                break;
            case UPDATE_ENABLED_COLUMN:
                if ( info.m_text == _( "Yes" ) ) {
                    update_enabled = true;
                } else {
                    update_enabled = false;
                }
                break;
            case BPP_COLUMN:
                info.m_text.ToLong( &bpp );
                break; 
            case MAXDEPTH_COLUMN:
                info.m_text.ToLong( &maxdepth );   
                break;
            case STAYONHOST_COLUMN:
                if ( info.m_text == _( "Yes" ) ) {
                    stayonhost = true;
                } else {
                    stayonhost = false;
                }
                break;
            case STAYONDOMAIN_COLUMN:
                if ( info.m_text == _( "Yes" ) ) {
                    stayondomain = true;
                } else {
                    stayondomain = false;
                }
                break;
            case URL_PATTERN_COLUMN:
                url_pattern = info.m_text;
                break;
        }    
    }
    wxLogDebug( wxT(" Finished showcase_dialog tranfer_to " ) );
}


void showcase_dialog::init_splitters()
{
    // Construct the m_showcase_splitterwindow
    // wxCLIP_CHILDREN commented out since it seemed to not redraw the listctrl when
    // returning from obscuring the dialog with another window/dialog. Need a refresh?
    // wxSP_3DBORDER is just there as a placeholder so won't default to SP_3D and 
    // make the 3D sash, which I think looks like ass.
    m_showcase_splitterwindow = new showcase_splitterwindow( this, MAIN_SPLITTERWINDOW,
                                        wxDefaultPosition, wxDefaultSize,
                                        wxSP_3DBORDER /*wxCLIP_CHILDREN*/,                                        
                                        wxT( "the_showcase_splitterwindow" ),
                                        this );
    wxLogDebug( wxT( "Constructed the_showcase_splitterwindow" ) );

    // Construct the listctrl and details htmlwindow.
    // Note their specified parent (the 1st argument in each of their constructors)
    details_htmlwindow = new wxHtmlWindow( m_showcase_splitterwindow, DETAILS_HTMLWINDOW,
                                           wxDefaultPosition, wxDefaultSize,
                                           wxHW_SCROLLBAR_AUTO|wxSUNKEN_BORDER );     
    
    m_showcase_listctrl  = new showcase_listctrl( m_showcase_splitterwindow, SHOWCASE_LISTCTRL,
                                                  wxDefaultPosition, wxDefaultSize,
                                                  wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
                                                                         

    wxLogDebug( wxT( "Constructed listctrl and details htmlwindow") );
    
    // The interesting part: want to attach the main_splitter into its placeholder inside
    // the XRC resource file. This is done with AttachUnknownControl (see xmlres.h and
    // xmlres.cpp for details). Premise is to use the <class="unknown"> to make an empty panel
    // the correct size, then use this function to reparent the actual desired control to that
    // panel at runtime.   
    wxXmlResource::Get()->AttachUnknownControl( wxT( "showcase_dialog_main_splitterwindow" ),
                                            m_showcase_splitterwindow );
                                            
    wxLogDebug( wxT( "Attached unknown control (m_showcase_splitterwindow)" ) );

    // Set the sash sizes and borders of splitter windows for prettiness. As of 
    // wxWindows 2.3.1, these aren't in docs yet, see /include/wx/splitter.h for details.
    //XRCCTRL( *this, "showcase_dialog_main_splitterwindow", wxSplitterWindow )
    //    ->SetSashSize( 7 );
    //details_preview_splitterwindow->SetSashSize( 7 );
    
    wxLogDebug( wxT( "Set the sash sizes and borders" ) );    

    // Set minimum pane widths
    XRCCTRL( *this, "showcase_dialog_main_splitterwindow", wxSplitterWindow )
        ->SetMinimumPaneSize( MINIMUM_PANE_WIDTH );
    
    wxLogDebug( wxT( "Set the minimum pane width" ) );  
    
    // Note: the splitterwindows now are ready to be split and windows assigned to their
    // panes. This will be done in the read_configuration().
}


// Read and set the saved values for the controls from the configuration file
void showcase_dialog::read_configuration()
{    
    //------Checkboxes, Choices---------------------------------------------------------
 
    //------SplitterWindows-------------------------------------------------------------

    // Get the values for the sash positions of the splitter windows from the preferences    . 
    long m_showcase_splitterwindow_sash_position = 
        the_configuration->Read( wxT( "/PLUCKER_DESKTOP/showcase_dialog_main_splitterwindow_sash_position"), 430L );

    // Split the windows according to the integer value read from preferences. Using
    // a switch/case to allow for more layouts later if desired.
    int layout_type = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/showcase_dialog_layout_type"), 0L );     
    switch ( layout_type ) {
// ---------------------------------------------------------------------------------------
        case HORIZONTAL_LAYOUT:
          
            XRCCTRL( *this, "showcase_dialog_main_splitterwindow", wxSplitterWindow )
                ->SplitVertically( m_showcase_listctrl, details_htmlwindow,
                                   m_showcase_splitterwindow_sash_position );
                                                                     
            break;    
// ---------------------------------------------------------------------------------------    
        case VERTICAL_LAYOUT:
                     
            XRCCTRL( *this, "showcase_dialog_main_splitterwindow", wxSplitterWindow )
                ->SplitHorizontally( m_showcase_listctrl, details_htmlwindow,
                                     m_showcase_splitterwindow_sash_position );
                                     
            break;
// ---------------------------------------------------------------------------------------
        default:
          
            XRCCTRL( *this, "showcase_dialog_main_splitterwindow", wxSplitterWindow )
                ->SplitVertically( m_showcase_listctrl, details_htmlwindow,
                                   m_showcase_splitterwindow_sash_position );
                                                                     
            break;    
// ---------------------------------------------------------------------------------------
    }    
    
    // Fix them if their sashes are set out of range (this can happen especially
    // if changed Vertical->Horizontal layout in the preferences. This is from
    // utils_controls.h
    utils_controls::splitterwindow_fix_sash_out_of_range( XRCCTRL( *this, 
                                          "showcase_dialog_main_splitterwindow", wxSplitterWindow ) );   
        
    wxLogDebug( wxT( "Split splitterwindows" ) );
}


// Write out the values of the dialog's controls to the configuration file.
void showcase_dialog::write_configuration()
{
    //------SplitterWindow-------------------------------------------------------------

    long m_showcase_splitterwindow_sash_position =
        XRCCTRL( *this, "showcase_dialog_main_splitterwindow", wxSplitterWindow )
        ->GetSashPosition();
    the_configuration->Write( wxT( "/PLUCKER_DESKTOP/showcase_dialog_main_splitterwindow_sash_position"), 
        m_showcase_splitterwindow_sash_position );
  
}


//Initialize the list control
void showcase_dialog::listctrl_init()
{
    // Columns are inseted in the listctrl instructor (see 
    
    // To speed up inserting we hide the control temporarily
    m_showcase_listctrl->Hide();

    // wxListCtrl uses a wxImageList to insert its graphics.  Set the wxImageList that the
    // listctrl should use, to the shared image list that is used throughout the application.
    // The second argument just tells the type of image list, that it is a small type.
    m_showcase_listctrl->SetImageList( image_list::get(), wxIMAGE_LIST_SMALL );

    wxLogDebug( wxT( "About to make a dummy html window" ) );
    // Create a dummy htmlwindow to parse out the gallery tags.
    dummy_htmlwindow = new wxHtmlWindow( this, -1, wxDefaultPosition, 
                                      wxSize( 1,1 ), wxHW_SCROLLBAR_NEVER );
    
    // Load a page to the dummy_htmlwindow. As it parses, it sticks rows into the listctrl    
    // First, get the path to the resource html file  Note htmlwindow needs htm or html
    // extension to render it [without adding filters].
    wxString showcase_resource_file;
    showcase_resource_file = get_plucker_directory( RESOURCES ) << wxT( "/plkrdata_showcase/showcase.htm");
    // Now load the page into the dummy htmlwindow.
    wxLogDebug( wxT( "About to load the dummy html window" ) );
    dummy_htmlwindow->LoadPage( showcase_resource_file );
    
    // Reset the row number. Required.
    m_showcase_listctrl_row_number = 0;
    
    // Stretch the columns, now that all the name entries are available. Stretch to 
    // the pane width if split with a vertical line, otherwise stretch to the full size of
    // the splitterwindow.    
    int maximum_listctrl_width;
    if ( m_showcase_splitterwindow->GetSplitMode() == wxSPLIT_VERTICAL )
    {
        maximum_listctrl_width = XRCCTRL( *this, "showcase_dialog_main_splitterwindow", wxSplitterWindow )
                                        ->GetSashPosition();
    }
    else 
    {
        maximum_listctrl_width = m_showcase_splitterwindow->GetSize().x;
    }
    m_showcase_listctrl->set_column_widths( maximum_listctrl_width );
    
    // Finished inserting. Show the control
    m_showcase_listctrl->Show();    
} 

// Loads up details window form a table row ( item_id )
void showcase_dialog::load_details_htmlwindow_content( long item_id )
{
    wxLogDebug( wxT( "Details htmlwindow about to load. Item id (row) is %ld"), item_id ); 

    // Read the index of fields to show, and convert the strings to an array of integers [utils_string.cpp].
    wxString showcase_details_fields_string = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/showcase_dialog_details_fields"), 
                                              wxT( "0;2;3;4;9;10;11;12;13") ); 
    wxArrayInt columns_to_display;
    utils_string::string_to_arrayint( showcase_details_fields_string, ';', &columns_to_display, true );
    
    wxString details_text;      // Output text to put into details pane.
    wxListItem info;            // Stores the properties to find cell, and store its info
    
    // Get what row was selected, by looking at the event's properties
    info.m_itemId = item_id;
    for ( int i = 0; i < (int)m_showcase_field_names.GetCount(); i++ ) 
    {
        // Check to see if the current column index is in our list from config file, of columns to show.
        if ( columns_to_display.Index( i ) != wxNOT_FOUND ) 
        {
            // Set the column of the cell to look for information
            info.m_col = i;
            // Set text mask (don't know why that is here, but seems to be always used).
            info.m_mask = wxLIST_MASK_TEXT;
            // Now, get the properties of the cell we described, and store them all in info.
            m_showcase_listctrl->GetItem( info );
            // We successfully pulled the value from that vell
            wxLogDebug( m_showcase_field_names.Item( i ) + wxT( " column value is %s"), info.m_text.c_str() );
            // Check to see that that the next text isn't just an empty string, or 
            // a "-1" which means an empty number.
            if ( info.m_text !=  ( "" || wxT( "-1" ) ) ) {
                details_text    += wxT( "<b>") + m_showcase_field_names.Item( i ) + wxT( "</b>: ");
                details_text    += info.m_text;
                details_text    += wxT( "<br>");
            }
        }
        wxLogDebug( wxT( "details_text thus far=") + details_text );
    }
  
    // Set the details pane html text to the string we generated, and in a way to look like part of a panel, unless the 
    // HTML message overrides it with font color tags or a <body bgcolor=""> parameter.
    // Note that the padding flag is off.
    utils_controls::set_html_window_text_in_gui_style( 
        details_htmlwindow,        
        details_text,
        optionSET_HTML_BACKGROUND_TO_MATCH_GUI_LISTBOX         |
        optionSET_HTML_FONT_COLOR_TO_MATCH_GUI                 |
        optionSET_HTML_FONT_FACE_AND_POINTSIZE_TO_MATCH_GUI
 );
    
    columns_to_display.Clear();
    
    wxLogDebug( wxT( "Finished displaying details.") );
}


//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void showcase_dialog::on_splitter_sash_position_changed( int sash_position )
{
    wxLogDebug( wxT( "Entering showcase_DIALOG::on_splitter_sash_position_changed") );    
        // It if is a vertical split...
    if ( m_showcase_splitterwindow->GetSplitMode() == wxSPLIT_VERTICAL ) 
    {                
        // ...set listctrl control column widths to the sash
        m_showcase_listctrl->set_column_widths( sash_position );
    } 
    else 
    {
        // Otherwise, set listctrl control columns to the full width of the window.  
        int listctrl_pane_width = m_showcase_splitterwindow->GetWindow1()->GetSize().x;
        m_showcase_listctrl->set_column_widths( listctrl_pane_width );
    }    
}


// When click a row in the listctrl, show that gallery site's info in the details and
// preview panel if those panes are currently visible
void showcase_dialog::on_listctrl_item_selected( wxListEvent& event )
{
    // Get the row number (m_itemIndex) that comes in with the event
    int item_id = event.m_itemIndex;

    // Wipe the window, so an unchanging content doesn't remain, if changed checkboxes
    // between the item selected events.
    details_htmlwindow->SetPage( wxT( "<html></html>") ); 

    // Load up the details pane, if so requested by user
    if ( the_configuration->Read( wxT( "/PLUCKER_DESKTOP/showcase_dialog_show_details" ), 1L ) ) 
    {
        load_details_htmlwindow_content( item_id );
    }
    
}


// Just call OnOK() when activated
void showcase_dialog::on_listctrl_item_activated( wxListEvent& event )
{
    OnOK( dynamic_cast<wxCommandEvent&>( event ) );
}


// Override wxDialog's default behavior for clicking an OK button.
void showcase_dialog::OnOK( wxCommandEvent& event )
{
     // Get what row was selected, 
    long selected_row_number = m_showcase_listctrl->GetNextItem( -1, wxLIST_NEXT_ALL, 
                                                          wxLIST_STATE_SELECTED );
    // If no selected row, then end as a cancel. If a selected, then end with an OK..
    if ( selected_row_number == -1 )
    {
        EndModal( wxID_CANCEL ); 
    } 
    else 
    {
        EndModal( wxID_OK );        
    }
}


void showcase_dialog::OnSize( wxSizeEvent &event )
{
    /// This stuff can't fire, if dialog isn't shown, (will crash on at least WXMSW)
    if ( IsShown() ) 
    {
	wxLogDebug( wxT( "Entering showcase_DIALOG::OnSize") );    
            // It if is a vertical split...
        if ( m_showcase_splitterwindow->GetSplitMode() == wxSPLIT_VERTICAL ) 
        {                
            // ...set listctrl control column widths to the sash
            m_showcase_listctrl->set_column_widths( m_showcase_splitterwindow->GetSashPosition() );
        } 
        else 
        {
            // Otherwise, set listctrl control columns to the full width of the window.  
            int listctrl_pane_width = m_showcase_splitterwindow->GetWindow1()->GetSize().x;
            m_showcase_listctrl->set_column_widths( listctrl_pane_width );
        }    
    }
    event.Skip();    
}



void showcase_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_SHOWCASE_DIALOG );
#endif 
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_SHOWCASE
