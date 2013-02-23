//----------------------------------------------------------------------------------------
// Name:        exclusion_dialog.cpp
// Purpose:     Exclusion lists dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: exclusion_listctrl.cpp,v 1.11 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "exclusion_listctrl.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR )

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

#include "exclusion_listctrl.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"              // XRC XML resouces

// ---------------------------------------------------------------------------------------

#include "exclusion_dialog.h"
#include "blocked_dialog.h"
#include "configuration.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

// Popup menu (PU) item IDs 
enum {
    PU_ADD_EXCLUSION        = wxID_HIGHEST + 1, 
    PU_EDIT_EXCLUSION,
    PU_DELETE_EXCLUSION    
};

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( exclusion_listctrl, wxListCtrl )

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( exclusion_listctrl, wxListCtrl ) 
    EVT_LIST_ITEM_ACTIVATED( -1, exclusion_listctrl::on_edit_activated )
    
    // Popup dialog    
    EVT_RIGHT_DOWN( exclusion_listctrl::popup_menu )   
    EVT_MENU( PU_ADD_EXCLUSION, exclusion_listctrl::on_add_command )
    EVT_MENU( PU_EDIT_EXCLUSION, exclusion_listctrl::on_edit_command )
    EVT_MENU( PU_DELETE_EXCLUSION, exclusion_listctrl::on_delete_command )
    
    EVT_SIZE( exclusion_listctrl::OnSize )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

// Constructor, including setting the dialog's m_configuration_section member to the
// incoming configuration_section string.
exclusion_listctrl::exclusion_listctrl( wxWindow *parent, wxWindowID id,
                                        const wxPoint& pos, const wxSize& size,
                                        long style, const wxValidator& validator,
                                        const wxString& name,
                                        const wxString& exclusion_column_caption )
   : wxListCtrl( parent, id, pos, size, style, validator, name )
{
    m_parent            = parent;
    m_listctrl_is_dirty = FALSE;
    
    // This listctrl needs to insert its columns in the constructor, since as soon
    // as the listctrl is built, it is resized and grafted onto an "unknown" XRC 
    // placeholder. This induces an OnSize() event, calling the overrriden OnSize function for 
    // this class, which needs to have 3 columns to resize (else an assert on WXGTK debug
    // build).
    InsertColumn( EXCLUSION_COLUMN, exclusion_column_caption, wxLIST_FORMAT_LEFT, ( 140 )  );
    InsertColumn( ACTION_COLUMN, _( "Action" ), wxLIST_FORMAT_LEFT, ( 70 )  );
    InsertColumn( PRIORITY_COLUMN, _( "Priority" ), wxLIST_FORMAT_LEFT, 70 );    
}


exclusion_listctrl::~exclusion_listctrl()
{
}


bool exclusion_listctrl::get_is_listctrl_dirty()
{
    return m_listctrl_is_dirty;
}


void exclusion_listctrl::popup_menu( wxMouseEvent& event )
{
    wxMenu      a_menu;

    a_menu.Append( PU_ADD_EXCLUSION, _( "Add a new exclusion...") );
    a_menu.Append( PU_EDIT_EXCLUSION, _( "Edit selected exclusion..." ) );
    a_menu.Append( PU_DELETE_EXCLUSION, _( "Delete selected exclusion" ) );
    
    // If no listctrl rows selected, then disable the menu items that require selection
    if ( GetSelectedItemCount() == 0 ) 
    {
        a_menu.Enable( PU_EDIT_EXCLUSION, FALSE );
        a_menu.Enable( PU_DELETE_EXCLUSION, FALSE );
    }

    // Show the popup menu (wxWindow::PopupMenu ), at the x,y position of the click event 
    PopupMenu( &a_menu, event.GetPosition() );
}


void exclusion_listctrl::OnSize( wxSizeEvent &event )
{
    set_column_widths();
    event.Skip();
}


void exclusion_listctrl::set_column_widths()
{
    wxLogDebug( wxT( "About to set main_listctrl column widths") );
    
    // Get width of entire listctrl
    wxLogDebug( wxT( "About to get main_listctrl name column width") );
    int name_column_width = GetSize().x;     
    wxLogDebug( wxT( "Got main_listctrl name column width") );
     
    // Subtract width of other columns, scrollbar, and some padding  
    name_column_width -= GetColumnWidth( ACTION_COLUMN );
    name_column_width -= GetColumnWidth( PRIORITY_COLUMN );
    name_column_width -= wxSystemSettings::GetMetric( wxSYS_VSCROLL_X );     
    name_column_width -= plkrLISTCTRL_EXTRA_WIDTH_PADDING;
     
    // Set the column width to the new value.
    SetColumnWidth( EXCLUSION_COLUMN, name_column_width );  
    
    wxLogDebug( wxT( "Successfully set column widths") );
}


// Inserts an empty row (item), returning the row number of the new insertion
// ready to be passed to the blocked dialog
long exclusion_listctrl::add_empty_row() 
{
    long new_inserted_row_index;
    
    wxListItem  cell_info;
    // Describe the wxListInfo to insert into the cell. 
    cell_info.m_mask = wxLIST_MASK_TEXT;
    cell_info.m_col = EXCLUSION_COLUMN;
    cell_info.m_text = wxT( "");    
    
    // Get the number of the last row
    new_inserted_row_index = InsertItem( cell_info );
    SetItem( new_inserted_row_index, ACTION_COLUMN, _( "Exclude" ) );
    SetItem( new_inserted_row_index, PRIORITY_COLUMN, wxT( "0") );
   
    wxLogDebug( wxT( "Successfully added row with index of %ld"), new_inserted_row_index );
    return new_inserted_row_index;
}


// Edits a row (item)
void exclusion_listctrl::edit_row( long row ) 
{
    wxListItem     info;              // To look up the info for the item.
    wxString       exclusion_string;  // String extracted from exclusion column
    wxString       action_string;     // String stored in listctrl
    long           action;            // Long extracted from action_string
    wxString       priority_string;   // String stored in listctrl.    
    long           priority;          // Long extracted from priority_string
 
    // Look up the properties of wxListItem--first thing is to set the id number
    // (m_itemId) to our current item (row) so we know what item we are talking about
    // in the listctrl grid.
    info.m_itemId = row;
    // Set text mask (don't know why that is here, but seems to be always used).
    info.m_mask = wxLIST_MASK_TEXT;

    // Set the column of the cell to look for information, to the exclsuion column
    info.m_col = EXCLUSION_COLUMN;
    // Get the properties of that cell
    GetItem( info );
    exclusion_string = info.m_text;
    
    // Same strategy for the action column. Include is the second choice in the 
    // choice control of the blocked dialog (index of 1)
    info.m_col = ACTION_COLUMN;
    GetItem( info );        
    if ( info.m_text == _( "Include" ) )
    {
        action = 1;
    } 
    else 
    {
        action = 0;
    }
               
    // And same strategy for the priority column
    info.m_col = PRIORITY_COLUMN;
    GetItem( info );
    priority_string = info.m_text;
    // A method exists to also just set a spinner to a string, not just an int.
    // However, on "Add", can a blank string, so change it to 0 if so.
    if ( priority_string == wxT( "") ) 
    {
        priority_string = wxT( "0" );
    }
    // Try to convert the string to a Long. If fail just set to 0.
    if ( ! priority_string.ToLong( &priority ) ) 
    {
        priority = 0;
    }
    
    // Sucessfully extracted out the 3 column values. Now show a blocked dialog, using
    // these 3 values. Launch as a child of the exclusion_dialog, not 
    // exclusion_listctrl, otherwise get the XRC freeze/error of double parent (see full 
    // comments in main_listctrl::launch_dialog_for_new_channel ).
    blocked_dialog a_blocked_dialog( m_parent, exclusion_string, action, priority );
    
    // Only execute this part, if user pressed OK on the blocked dialog
    if ( a_blocked_dialog.ShowModal() == wxID_OK )
    {
        // Get the values out from the blocked dialog.       
        a_blocked_dialog.transfer_to( exclusion_string, action, priority );
     
        SetItem( info.m_itemId, EXCLUSION_COLUMN, exclusion_string );
                   
        // 0 is exclude, 1 is include
        if ( action == 0 )
        {
            action_string = _( "Exclude" );
        } 
        else 
        {
            action_string = _( "Include" );     
        }                
        SetItem( info.m_itemId, ACTION_COLUMN, action_string );

        priority_string.Printf( wxT( "%ld" ), priority );
        SetItem( info.m_itemId, PRIORITY_COLUMN, priority_string );

        // Mark as dirty, so exclusion dialog knows should save it when close dialog.
        m_listctrl_is_dirty = TRUE; 
    }    
}


// Deletes a row (item)
void exclusion_listctrl::delete_selected_row() 
{    
    // Selected_item_id of -1 means that nothing is selected, so just abort
    if ( get_selected_item_id() == -1 ) 
    {
        return;
    }
    
    // Make a page specific message for the confirm dialog
    wxString confirmation_string;
    confirmation_string = _( "Are you sure you wish to delete this URL/extension from your exclusion list?" );
        
    // Create a confirm dialog
    wxMessageDialog confirm_messagedialog( this, confirmation_string, _( "Confirm delete" ),
                                           wxYES | wxNO|wxYES_DEFAULT|wxICON_QUESTION );
    
    // If they say yes, then delete it, and mark the file as dirty so will know
    // that it needs a save.
    if ( confirm_messagedialog.ShowModal() == wxID_YES ) 
    {
        DeleteItem( get_selected_item_id() );
        m_listctrl_is_dirty = TRUE; 
    }
}


// Returns what row is currently selected
long exclusion_listctrl::get_selected_item_id()
{   
    long selected_item_id;  // Will hold the index of the selected row.  
    
    selected_item_id = GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
                                                       
    wxLogDebug( wxT( "Value of selected item_id is %ld"), selected_item_id );         
    
    // If no rows that fit the flag of being in a selected state, return -1 and stop.
    if ( selected_item_id == -1 ) 
    {
        wxLogDebug( wxT( "No rows in a selected state") );
        return -1;
    } 
    else 
    {   
        return selected_item_id;
    }
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void exclusion_listctrl::on_add_command(wxCommandEvent& WXUNUSED(event))
{
    long new_row;
    
    new_row = add_empty_row() ;
    edit_row( new_row );
}


void exclusion_listctrl::on_edit_command(wxCommandEvent& WXUNUSED(event))
{
    long selected_row = get_selected_item_id();
    
    if ( selected_row != -1 ) 
    {
        edit_row( selected_row );
    }
}


void exclusion_listctrl::on_edit_activated(wxListEvent& WXUNUSED(event))
{
    long selected_row = get_selected_item_id();
    
    if ( selected_row != -1 ) 
    {
        edit_row( selected_row );
    }
}


void exclusion_listctrl::on_delete_command(wxCommandEvent& WXUNUSED(event))
{
    delete_selected_row();
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR
