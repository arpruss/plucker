//----------------------------------------------------------------------------------------
// Name:        channel_check_listctrl.cpp
// Purpose:     Listctrl used in the main frame
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: channel_check_listctrl.cpp,v 1.5 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "channel_check_listctrl.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

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

#include "channel_check_listctrl.h"

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

IMPLEMENT_DYNAMIC_CLASS( channel_check_listctrl, channel_listctrl_base )

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( channel_check_listctrl, channel_listctrl_base )
    EVT_LEFT_DOWN( channel_check_listctrl::on_mouse_event )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Interface
//----------------------------------------------------------------------------------------

channel_check_listctrl::channel_check_listctrl( wxWindow *parent,
                              wxWindowID id,
                              wxFileConfig* configuration,
                              int default_image_list_index,
                              long options,
                              const wxPoint& pos, const wxSize& size,
                              long style, const wxValidator& validator,
                              const wxString& name )
   : channel_listctrl_base( parent, id, configuration, default_image_list_index, options,
                            pos, size, style, validator, name )
{
    wxLogDebug( wxT( "Entering channel_check_listctrl constructor") );
    insert_columns();

    initialize_listctrl_content();
    wxLogDebug( wxT( "Completed channel_check_listctrl constructor") );
}


channel_check_listctrl::~channel_check_listctrl()
{
    wxLogDebug( wxT( "Entering channel_check_listctrl destructor") );

    wxLogDebug( wxT( "Completed channel_check_listctrl destructor") );
}


void channel_check_listctrl::get_checked_channel_sections( wxArrayString* channel_sections )
{
    wxString    section_string;
    long        examined_row_number = -1;  // '-1' includes the first selected row.

    wxLogDebug( wxT( "Entering selected channels function") );

    for ( ;; ) 
    {
        // for( ;; ) with this next line is the recommended way for iterating selected rows.
        // examined_row_number was initialized at -1 to allow inclusion of first selected
        // row.
        examined_row_number = GetNextItem( examined_row_number, wxLIST_NEXT_ALL,
                                           wxLIST_STATE_DONTCARE );

        // No more rows that fit the flag of being in a selected state, so break loop.
        if ( examined_row_number == -1 ) 
        {
            break;
        }

        // Look to see if the row is checked...
        if ( is_row_checked( examined_row_number ) ) 
        {
            // ..and use our function to extract the string contents from a cell
            section_string = get_row_channel_section( examined_row_number );
            channel_sections->Add( section_string );
            wxLogDebug( wxT( "Appended section %s to selected sections"), section_string.c_str() );
        }
    }
}


void channel_check_listctrl::popup_menu( wxMouseEvent& event )
{
}


void channel_check_listctrl::insert_columns()
{
    // Insert the columns for the listctrl. Do in the constructor
    // (see Plucker Desktop's main_listctrl.cpp for details on why).
    InsertColumn( NAME_COLUMN, _( "Channel Name" ), wxLIST_FORMAT_LEFT, 160 );
}


void channel_check_listctrl::set_column_widths()
{
    int name_column_width = GetSize().x;

    // Subtract width of other columns, scrollbar, and some padding
    name_column_width -= wxSystemSettings::GetMetric( wxSYS_VSCROLL_X );
    name_column_width -= plkrLISTCTRL_EXTRA_WIDTH_PADDING;

    // Set the column width to the new value.
    SetColumnWidth( NAME_COLUMN, name_column_width );
}


void channel_check_listctrl::on_mouse_event( wxMouseEvent& event )
{
     if ( event.LeftDown() ) 
     {
         int flags;
         long row = HitTest( event.GetPosition(), flags );
         if ( row > -1 && ( flags & wxLIST_HITTEST_ONITEMICON ) ) 
         {
             set_row_checked( row, ! is_row_checked( row ) );
         } 
         else 
         {
             event.Skip();
         }
     } 
     else 
     {
         event.Skip();
     }
}


bool channel_check_listctrl::is_row_checked( long row ) const
{
     wxListItem cell_info;
     cell_info.m_mask    = wxLIST_MASK_IMAGE ;
     cell_info.m_itemId  = row;

     if ( GetItem( cell_info ) ) 
     {
         return ( cell_info.m_image == plkrSMALL_IMAGE_LIST_ID_CHECKBOX_CHECKED );
     } 
     else 
     {
         return FALSE;
     }
}


void channel_check_listctrl::set_row_checked( long row, bool checked )
{
     int image_index;
     if ( checked ) 
     {
        image_index = plkrSMALL_IMAGE_LIST_ID_CHECKBOX_CHECKED;
     } 
     else 
     {
        image_index = plkrSMALL_IMAGE_LIST_ID_CHECKBOX_UNCHECKED;
     }

     SetItemImage( row, image_index, -1 );
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
