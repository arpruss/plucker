//----------------------------------------------------------------------------------------
// Name:        channel_listctrl_base.cpp
// Purpose:     Listctrl used in the main frame
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: channel_listctrl_base.cpp,v 1.4 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "channel_listctrl_base.h"
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

#include "channel_listctrl_base.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

//---------------------------------------------------------------------------------------

#include "configuration.h"
#include "image_list.h"

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// wxWindows macro: implement abstract class
//----------------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS( channel_listctrl_base, wxListCtrl )

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( channel_listctrl_base, wxListCtrl )
    // A double-click or enter key on a listctrl row.
    EVT_RIGHT_DOWN( channel_listctrl_base::popup_menu )

    EVT_SIZE( channel_listctrl_base::OnSize )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

channel_listctrl_base::channel_listctrl_base( wxWindow *parent,
                              wxWindowID id,
                              wxFileConfig* configuration,
                              int default_image_list_index,
                              long options,
                              const wxPoint& pos, const wxSize& size,
                              long style, const wxValidator& validator,
                              const wxString& name )
   : wxListCtrl( parent, id, pos, size, style, validator, name )
{
    wxLogDebug( wxT( "Entering channel_listctrl_base constructor" ) );

    m_parent                   = parent;
    m_default_image_list_index = default_image_list_index;
    m_configuration            = configuration;
    m_options                  = options;

    wxLogDebug( wxT( "Finished channel_listctrl_base destructor" ) );
}


channel_listctrl_base::~channel_listctrl_base()
{
    wxLogDebug( wxT( "Entering channel_listctrl_base destructor" ) );

    // Clear the memory occupied by the array of section names.
    m_row_channel_section_array.Clear();

    wxLogDebug( wxT( "Completed channel_listctrl_base destructor" ) );
}


void channel_listctrl_base::initialize_listctrl_content()
{
    // wxListCtrl uses a wxImageList to insert its graphics.  Set the wxImageList that the
    // listctrl should use, to the shared image list that is used throughout the application.
    // The second argument just tells the type of image list, that it is a small type.
    // Usually we hide the listctrl while we insert rows (then show when done , but here it
    // is not needed, since the whole main dialog is not visible yet, so therefore the
    // channel_listctrl_base is already hidden too at this stage.
    SetImageList( image_list::get(), wxIMAGE_LIST_SMALL );

    // Load the channels into the rows of the table
    load_rows();

    // Adjust the column widths (recycled on the OnSize event)
    set_column_widths();

    wxLogDebug( wxT( "Completed channel_listctrl_base::initialize_listctrl_content" ) );
}


void channel_listctrl_base::reload_rows()
{
    // Run a busy cursor until we exit this function
    wxBusyCursor a_busy_cursor;

    // Delete the rows from the listctrl
    DeleteAllItems();

    // Clear them from our array
    m_row_channel_section_array.Clear();

    // Reload the rows
    load_rows();
}

//----------------------------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------------------------

void channel_listctrl_base::load_rows()
{
    wxString channel_section;             // Section of channel in configuration file
    long     index               = 0;     // Index counter for looping through the sections
    bool     found_section       = FALSE; // To monitor success of getting first/next group
    wxString key_name;                    // Delme once can all is_channel_section() directly

    wxLogDebug( wxT( "Starting to load channel sections into listctrl rows" ) );

    /* This DOES NOT BELONG HERE. Get it out of the main listctrl's load rows*/
    /* DITTO FOR THE MAKING OF CHANNEL'S DIRECTORIES IN THE LOOP */
    /*
    // Make a "channels" directory to hold home.html, cache and other future stuff
    // for each entry
    if ( ! wxDirExists( get_plucker_directory( CHANNELS ) ) ) {
        wxMkdir( get_plucker_directory( CHANNELS ), 0777 );
    }
    wxLogDebug( wxT( "Finished testing/trying to make channel directory" ) );
    */

    // TODO: also make the default channel one, in case it is gone.

    found_section = m_configuration->GetFirstGroup( channel_section,
                                                    index );
    while ( found_section ) 
    {
        // Load up the each channel section (group) from the
        // plucker.ini/pluckerrc file
        wxLogDebug( wxT( "Config section=%s" ), channel_section.c_str() );

        // \todo put this line back instead of our test below, 
        // once is_channel_section() can be called by any config object
        //if ( is_channel_section( channel_section ) ) {
        key_name = channel_section + wxT( "/home_url" );
        if ( ! is_reserved_section( channel_section ) && m_configuration->HasEntry( key_name ) ) 
        {
            insert_row( channel_section );
        }
        found_section = m_configuration->GetNextGroup( channel_section,
                                                       index );
    }
}


void channel_listctrl_base::insert_row( const wxString &channel_section )
{
    wxLogDebug( wxT( "Entering channel_listctrl_base::insert_row()" ) );
    wxListItem  cell_info;
    long        newly_inserted_row_number;

    // Describe the wxListInfo to insert into the initial cell.of row.
    // Just insert a blank "" string for channel name, as can just recycle refresh_row()
    // to actually do the work.
    cell_info.m_mask    = wxLIST_MASK_TEXT;
    cell_info.m_col     = NAME_COLUMN;
    cell_info.m_image   = m_default_image_list_index;
    wxLogDebug( wxT( "Default image list index=%d" ), cell_info.m_image );
    
    // If refreshing anyways, then just insert a space for now...
    if ( m_options & optionLISTCTRL_INSERT_NAME_ON_REFRESH_NOT_INSERTION ) 
    {
        cell_info.m_text = wxT( " " );
    }
    else
    {
        // ...Otherwise insert the actual channel name.
        cell_info.m_text = m_configuration->Read( channel_section + wxT( "/doc_name" ),
                                                  plkrUNNAMED_CHANNEL_NAME );
    }                                                    

    // Insert the row, thus getting the number of inserted row.
    newly_inserted_row_number = InsertItem( cell_info );

    // Insert the channel section string into our array of strings.
    insert_row_channel_section( channel_section, newly_inserted_row_number );

    // Refresh the row (which will draw in the name and date into its columns).
    // NOTE: it seems that this have to go after insert_row_channel_section(), since
    // otherwise get a weird bug of refreshing the newly inserted row with the values
    // of the row below it. I guess this way gives the GUI time to catch up.
    refresh_row( newly_inserted_row_number );
    wxLogDebug( wxT( "Finished channel_listctrl_base::insert_row()" ) );

#if ( setupUSE_CHANNEL_LISTCTRL_BASE_FORCE_NEW_ITEM_IMAGE_HACK )
     SetItemImage( newly_inserted_row_number, m_default_image_list_index, -1 );
#endif

}


void channel_listctrl_base::refresh_row( long row_number )
{
    // Override as desired in derived classes.
}


void channel_listctrl_base::get_selected_channel_sections( wxArrayString* channel_sections )
{
    wxString    section_string;
    long        selected_row_number = -1;  // '-1' includes the first selected row.

    wxLogDebug( wxT( "Entering selected channels function" ) );

    for ( ;; ) 
    {
        // for( ;; ) with this next line is the recommended way for iterating selected rows.
        // selected_row_number was initialized at -1 to allow inclusion of first selected
        // row.
        selected_row_number = GetNextItem( selected_row_number, wxLIST_NEXT_ALL,
                                           wxLIST_STATE_SELECTED );

        // No more rows that fit the flag of being in a selected state, so break loop.
        if ( selected_row_number == -1 )
        {
            break;
        }

        // Use our function to extract the string contents from a cell
        section_string = get_row_channel_section( selected_row_number );

        channel_sections->Add( section_string );

        wxLogDebug( wxT( "Appended section %s to selected sections" ), section_string.c_str() );
    }
}


void channel_listctrl_base::get_selected_row_numbers( wxArrayInt* row_numbers )
{
    long  selected_row_number = -1;  // '-1' needed to include the first selected row.

    wxLogDebug( wxT( "Entering selected row numbers function" ) );

    for ( ;; ) 
    {
        // for( ;; ) with this next line is the recommended way for iterating selected rows.
        // selected_row_number was initialized at -1 to allow inclusion of first selected
        // row.
        selected_row_number = GetNextItem( selected_row_number, wxLIST_NEXT_ALL,
                                           wxLIST_STATE_SELECTED );

        if ( selected_row_number == -1 ) 
        {
            break;
        } 
        else
        {
            row_numbers->Add( selected_row_number );
            wxLogDebug( wxT( "Appended row %ld to selected rows" ), selected_row_number );
        }

    }
}


wxString channel_listctrl_base::get_row_channel_section( long row_number )
{
    wxString    channel_section;
    size_t      array_index;

    array_index     = (size_t)row_number;
    channel_section = m_row_channel_section_array.Item( array_index );

    return channel_section;
}


void channel_listctrl_base::set_row_channel_section( const wxString& channel_section,
                                                     long row_number )
{
    wxString    new_channel_section;
    size_t      array_index;

    new_channel_section  = channel_section;
    array_index          = (size_t)row_number;
    m_row_channel_section_array.Item( array_index ) = new_channel_section;
}


void channel_listctrl_base::insert_row_channel_section( const wxString& channel_section,
                                                        long row_number )
{
    wxString    new_channel_section;
    size_t      array_index;

    new_channel_section     = channel_section;
    array_index             = (size_t)row_number;
    m_row_channel_section_array.Insert( new_channel_section, array_index );
}


void channel_listctrl_base::remove_row_channel_section( long row_number )
{
    size_t      array_index;

    array_index  = (size_t)row_number;
    m_row_channel_section_array.RemoveAt( array_index );
}


wxString channel_listctrl_base::get_cell_contents_string( long row_number,
                                                          int column )
{
    wxListItem     row_info;
    wxString       cell_contents_string;

    // Set what row it is
    row_info.m_itemId = row_number;
    // Set what column of that row we want to query for information.
    row_info.m_col = column;
    // Set text mask
    row_info.m_mask = wxLIST_MASK_TEXT;

    // Get the info and store it in row_info variable.
    GetItem( row_info );

    // Extract the text out that cell
    cell_contents_string = row_info.m_text;

    return cell_contents_string;
}


void channel_listctrl_base::popup_menu( wxMouseEvent& event )
{
    // Override as desired in derived class. Or not, and it will just
    // do nothing.
}


void channel_listctrl_base::OnSize( wxSizeEvent &event )
{
    set_column_widths();
    event.Skip();
}
