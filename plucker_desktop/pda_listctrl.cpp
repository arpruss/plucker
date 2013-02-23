//----------------------------------------------------------------------------------------
// Name:        pda_listctrl.cpp
// Purpose:     PDA listctrl (part of hhictrl.cpp)
// Author:      Robert O'Connor
// Modified by:
// Created:     2003/02/03
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: pda_listctrl.cpp,v 1.7 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "pda_listctrl.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

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

#include "pda_listctrl.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( pda_listctrl, wxListCtrl )

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( pda_listctrl, wxListCtrl )
   EVT_SIZE( pda_listctrl::OnSize )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

pda_listctrl::pda_listctrl( wxWindow *parent,
                            wxWindowID id,
                            const wxPoint &pos,
                            const wxSize &size,
                            long listCtrlStyle,
                            const wxValidator& validator,
                            const wxString &name,
                            const wxString &column_caption,
                            long users_or_directories_listctrl )
         : wxListCtrl(parent, id, pos, size, listCtrlStyle, validator, name)
{
    // Store
    m_users_or_directories_listctrl = users_or_directories_listctrl;

    // Insert the columns for the listctrl. Do in the constructor
    // (see Plucker Desktop's main_listctrl.cpp for details on why).
    InsertColumn( USER_NAME_COLUMN, column_caption, wxLIST_FORMAT_LEFT, 160 );

    // If users listctrl, more columns.
    if ( m_users_or_directories_listctrl == optionIS_USERS_LISTCTRL ) 
    {
        InsertColumn( RAM_OR_CARD_COLUMN, _( "Storage" ), wxLIST_FORMAT_LEFT, 75 );
        //InsertColumn( DIRECTORY_ON_CARD_COLUMN, _( "Card Directory" ), wxLIST_FORMAT_LEFT, 100 );
        InsertColumn( IS_USB_PAUSE_COLUMN, _( "USB" ), wxLIST_FORMAT_LEFT, 45 );
    }
}


pda_listctrl::~pda_listctrl()
{ 
}


void pda_listctrl::set_column_widths()
{
    // Get width of entire listctrl
    wxLogDebug( wxT( "About to get pda_listctrl name column width" ) );
    int user_name_column_width = GetSize().x;
    wxLogDebug( wxT( "Got pda_listctrl name column width" ) );
   
    // Subtract width of other columns, scrollbar, and some padding
    user_name_column_width -= wxSystemSettings::GetMetric( wxSYS_VSCROLL_X );
    user_name_column_width -= plkrLISTCTRL_EXTRA_WIDTH_PADDING;
    // Users listctrl subtracts some extra columns
    if ( m_users_or_directories_listctrl == optionIS_USERS_LISTCTRL ) 
    {
        user_name_column_width -= GetColumnWidth( RAM_OR_CARD_COLUMN );
        // user_name_column_width -= GetColumnWidth( DIRECTORY_ON_CARD_COLUMN );
        user_name_column_width -= GetColumnWidth( IS_USB_PAUSE_COLUMN );
    }

    // Set the column width to the new value.
    SetColumnWidth( USER_NAME_COLUMN, user_name_column_width );

    wxLogDebug( wxT( "Successfully set column widths" ) );
}


wxString pda_listctrl::get_cell_contents_string( long row_number,
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

void pda_listctrl::find_matching_rows( wxArrayString& input_arraystring,
                                       wxArrayInt *output_matching_rows_arrayint )
{
    bool is_a_matching_row;

    // Loop through the items
    for ( size_t row_number = 0; row_number < (size_t)GetItemCount(); row_number++ )
    {
        // Reset from last iteration
        is_a_matching_row = TRUE;
        // Loop through checking the cell contents of the row to make sure all match.
        for ( int col_number = 0; col_number < (int)input_arraystring.GetCount(); col_number++ ) 
        {
            if ( get_cell_contents_string( row_number, col_number ) != input_arraystring.Item( col_number ) ) {
                // If one cell doesn't match, then not a matching row. Say so and break.
                is_a_matching_row = FALSE;
                break;
            }
        }
        // If it was a matching row, then add to our output array
        if ( is_a_matching_row ) 
        {
            output_matching_rows_arrayint->Add( row_number );
        }
    }
}


//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

void pda_listctrl::OnSize(wxSizeEvent& event)
{
    set_column_widths();
    event.Skip();
}

