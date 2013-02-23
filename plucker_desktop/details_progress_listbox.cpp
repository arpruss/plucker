//----------------------------------------------------------------------------------------
// Name:        details_progress_listbox.cpp
// Purpose:     Describes details_progress_listbox class.
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: details_progress_listbox.cpp,v 1.9 2007/06/30 02:10:13 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "details_progress_listbox.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_ENHANCED_PROGRESS_DIALOG )

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

#include "details_progress_listbox.h"  

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/listbox.h"
#include "wx/settings.h"                // wxSystemSettings (for color)
#include "wx/textfile.h"                // wxTextFile
#include "wx/filedlg.h"                 // wxFileDialog
#if ( setupUSE_DRAG_AND_DROP )
    #include "wx/clipbrd.h"             // wxClipBoard
#endif

//---------------------------------------------------------------------------------------
            
#include "utils_clipboard.h"
#include "utils_datetime.h"

#if ( setupUSE_NO_CONTROL_PERSISTANCE_HACK )
    #include "wx/confbase.h"
#endif

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------
                           
// Popup menu (PU) item IDs                           
enum {
    PU_PROGRESS_COPY         = wxID_HIGHEST + 1, 
    PU_PROGRESS_CLEAR,
    PU_PROGRESS_SELECT_ALL
};

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( details_progress_listbox, wxListBox )

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( details_progress_listbox, wxListBox )
    EVT_RIGHT_DOWN( details_progress_listbox::popup_menu )   
    EVT_MENU( PU_PROGRESS_COPY, details_progress_listbox::on_copy_menu_event )
    EVT_MENU( PU_PROGRESS_CLEAR, details_progress_listbox::on_clear_menu_event )
    EVT_MENU( PU_PROGRESS_SELECT_ALL, details_progress_listbox::on_select_all_menu_event )    
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

details_progress_listbox::details_progress_listbox( wxWindow* parent, wxWindowID id, 
                          const wxString& log_filename,
                          const wxString& log_path,
                          long  scroll_history,
                          const wxPoint& pos, const wxSize& size, 
                          int n, const wxString choices[],
                          long style, const wxValidator& validator, 
                          const wxString& name )
   : wxListBox( parent, id, pos, size, n, choices, style, validator, name )
{
    m_parent         = parent;
    m_log_filename   = log_filename;    
    m_log_path       = log_path;
    
    // Set the background color of the listbox to the color of buttonfaces.
    wxColor form_background_color = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
    SetBackgroundColour( form_background_color );  
    
    switch ( scroll_history )
    {
        case optionDETAILS_PROGRESS_LISTBOX_MAXIMUM_SCROLL_HISTORY:
        // Use 1999 for GTK (problems at 2000 in GTK 1.x, don't know if this
        // still causes a problem in GTK 2), and the int value for everyone else).
#ifdef __WXGTK__
            m_scroll_history = 1999;
#else
            m_scroll_history = 32767;
#endif
            break;
        
        // It appears that if the listbox allows to scroll to a giant 
        // number of lines, then it starts slowing things down to keep updating
        // them. A 500 will keep things moving brisk.
        case optionDETAILS_PROGRESS_LISTBOX_RECOMMENDED_SCROLL_HISTORY:
            m_scroll_history = 500;
            break;
                        
        default:
            m_scroll_history = scroll_history;
            break;
    }
}


details_progress_listbox::~details_progress_listbox()
{ 
    // Clear the memory occupied by the array of selected rows.
    m_selected_row_numbers_array.Clear();
    
    wxLogDebug( wxT( "Completed details_progress_listbox destructor" ) );
}


void details_progress_listbox::append_using_scroll_history( const wxString& item )
{
    // Append the text item.
    Append( item );

    // If an infinite scroll history, then just stop here.
    if ( m_scroll_history == optionDETAILS_PROGRESS_LISTBOX_INFINITE_SCROLL_HISTORY ) 
    {
        return;
    }        
    
    // Otherwise, check to see if at max lines of history, and if so... 
    if ( m_scroll_history <= GetCount() ) 
    {
        wxLogDebug( wxT( "details_progress_listbox::append_using_scroll_history: deleting top item of listbox since max scrollback" ) );
        // ...then delete the first item
        Delete( 0 );
    }    
}


bool details_progress_listbox::export_to_file()
{
    // We were passed a default filename of the log file when user wants 
    // to export. We stored it as m_log_file_filename.
    // Hover, it may have the datetimestamp in it, so need to do replacing.
    // First make a new string and set it to the stored member, as we
    // don't want to modify the member as we want to keep it for next time
    // too.    
    wxString processed_default_filename;
    processed_default_filename = m_log_filename;

    // Build a current time string that should replace our placeholder.
    wxDateTime  current_datetime = utils_datetime::get_current_datetime();
    wxString datetime_string_to_insert = current_datetime.Format( wxT( "%Y-%m-%d_%H%M%S" ) );


    // Do the replacing.    
    processed_default_filename.Replace( plkrFILE_FRIENDLY_DATETIME_STAMP,
                                        datetime_string_to_insert );

#if ( setupUSE_NO_CONTROL_PERSISTANCE_HACK )
    // Stop wxFileDialog from saving to plucker.ini by setting the active config
    // to NULL.
    wxConfigBase* previous_configuration = NULL;
    previous_configuration = wxConfigBase::Set( NULL );        
#endif 

    // Make a filename extension filter based on the processed_default_filename
    // Note on OSX, seems that the extension filter will also determine the extension
    // of the file (so *.* will make filename end in .), so we need to do a good one.
    wxString filename_extension;
    if ( processed_default_filename.Contains( wxT( "." ) ) ) 
    {
        filename_extension << wxT( "*." ) << processed_default_filename.AfterLast( '.' );
    }
    
    // Show the file dialog
    wxFileDialog a_file_dialog( this, 
                                _( "Save details log file" ),
                                m_log_path,
                                processed_default_filename,
                                filename_extension,
                                wxSAVE | wxOVERWRITE_PROMPT );                                    

    // Only do this if clicked OK on file dialog
    if ( a_file_dialog.ShowModal() == wxID_OK )
    {    
        wxTextFile  textfile;
        wxString    log_fullname;
        int         number_of_rows;
        int         row;    
            
        log_fullname    = a_file_dialog.GetPath();
        
        // Delete that file if it already exists
        if ( wxFileExists( log_fullname ) ) 
        {
            wxRemoveFile( log_fullname );            
        }
        
        // Create the textfile object, abort if can't 
        if ( ! textfile.Create( log_fullname ) )
        {
            return FALSE;
        }
        
        // The function to get the number of rows in listbox changed in wx2.3.2. 
        number_of_rows = GetCount();
    
        // Error handling if there are no rows in listbox yet
        if ( number_of_rows == 0 ) 
        {
            return FALSE;
        }
        
        // Loop through listbox, adding each line to the textfile.
        for ( row = 0; row < number_of_rows; row++ )
        {
            // Add the row to the textfile
            textfile.AddLine( GetString( row ) ); 
        }                       
        
        // Write the changes to disk. 
        textfile.Write();
    
        // Close the file and free the memory.
        textfile.Close();


#if ( setupUSE_NO_CONTROL_PERSISTANCE_HACK )
    // Destroy the wxFileDialog to force saving of its settings.
    a_file_dialog.Destroy();
    // Set the active config to the previous one:
    wxConfigBase::Set( previous_configuration );
#endif

        
        return TRUE;
        
    } 
    else 
    {
    


#if ( setupUSE_NO_CONTROL_PERSISTANCE_HACK )
    // Destroy the wxFileDialog to force saving of its settings.
    a_file_dialog.Destroy();
    // Set the active config to the previous one:
    wxConfigBase::Set( previous_configuration );
#endif



        return FALSE;
    }
}

//----------------------------------------------------------------------------------------
// Protected members
//----------------------------------------------------------------------------------------

void details_progress_listbox::popup_menu( wxMouseEvent& event )
{
#if ( setupUSE_DRAG_AND_DROP ) 

    wxMenu      a_menu;
    int         number_of_selected_rows;
                   
    a_menu.Append( PU_PROGRESS_COPY, _( "Copy selected" ) );
    a_menu.Append( PU_PROGRESS_CLEAR, _( "Clear all" ) );
    a_menu.Append( PU_PROGRESS_SELECT_ALL, _( "Select all" ) );
    
    // Store the selected rows in our member array, so we don't have to look them up again
    // when time to copy.
    number_of_selected_rows = GetSelections( m_selected_row_numbers_array );
    
    // If no listbox rows selected, then disable the copy menu item
    if ( number_of_selected_rows == 0 ) 
    {
        a_menu.Enable( PU_PROGRESS_COPY, FALSE );
    }
    
    // Show the popup menu (wxWindow::PopupMenu ), at the x,y position of the click event 
    PopupMenu( &a_menu, event.GetPosition() );
    
#endif  // setupUSE_DRAG_AND_DROP 
}


void details_progress_listbox::on_copy_menu_event( wxCommandEvent& event )
{
#if ( setupUSE_DRAG_AND_DROP ) 

    wxString    text_to_copy;
    int         number_of_selected_rows;
    size_t      array_index;
    int         row;
    
    number_of_selected_rows = m_selected_row_numbers_array.GetCount();
    
    // Error handling if somehow got here with a zero array (which should be impossible)
    if ( number_of_selected_rows == 0 ) 
    {
        return;
    }

    for ( array_index = 0; (int)array_index < number_of_selected_rows; array_index++ ) 
    {
        row          =  m_selected_row_numbers_array.Item( array_index );
        // wxListBox::GetString: gets the string from the current row in listbox, add it 
        text_to_copy += GetString( row );
        // Add an OS-appropriate carriage return too
        text_to_copy += plkrEOL_CHARACTER ;
    }
   
    // Send that string to the clipboard
    utils_clipboard::set_clipboard_to_text( text_to_copy );
   
#endif  // setupUSE_DRAG_AND_DROP 
}


void details_progress_listbox::on_clear_menu_event( wxCommandEvent& event )
{
    // wxListBox::Clear() wipes it clean.
    Clear();
}


void details_progress_listbox::on_select_all_menu_event( wxCommandEvent& event )
{
    int         number_of_rows;
    int         row;
    
    // The function to get the number of rows in listbox changed in wx2.3.2. 
    // A small fork to keep wx2.3.1 compilation intact for now. TODO: Remove as time goes by
#if ( wxCHECK_VERSION( 2, 3, 2 ) )
    number_of_rows = GetCount();
#else
    number_of_rows = Number();
#endif
    
    // Error handling if there are no rows in listbox yet
    if ( number_of_rows == 0 )
    {
        return;
    }

    for ( row = 0; row < number_of_rows; row++ ) 
    {
       // Selects the row
       SetSelection( row, TRUE );
    }
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ENHANCED_PROGRESS_DIALOG
