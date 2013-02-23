///////////////////////////////////////////////////////////////////////////////////////////
// Name:        main_listctrl.cpp
// Purpose:     Listctrl used in the main dialog
// Author:      Robert O'Connor
// RCS-ID:      $Id: main_listctrl_[using_arraystring_not_zero_column].cpp,v 1.1 2002/04/02 22:55:53 robertoconnor Exp $
// Copyright:   Robert O'Connor
// Licence:     GPL
///////////////////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "main_listctrl.h"
#endif

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
// ---------------------------------------------------------------------------------------
#include "plucker_defines.h"
// ---------------------------------------------------------------------------------------
#include "wx/sizer.h"
#include "wx/listctrl.h"
#if ( setupUSE_DRAG_AND_DROP )
    #include "wx/dnd.h"                 // Drag and drop classes
    #include "wx/clipbrd.h"             // wxClipBoard
#endif
//---------------------------------------------------------------------------------------
#include "main_dialog.h"
#include "main_listctrl.h"              
#include "configuration.h"
#include "channel_dialog.h"
#include "progress_dialog.h"
#include "image_list.h"
#include "plucker_controller.h"
#include "utils_datetime.h"
#include "utils_string.h"
#include "utils_clipboard.h"

// -----------------------------------------------------------------------------------------
// Derive two simple classes which just put in the ListCtrl the strings (text or file 
// names) that we drop on it. The first is a class for text, second is for files. 
// Then need some icons for the dropping.
// --------------------------------------------------------------------------------------

#if ( setupUSE_DRAG_AND_DROP )

class dnd_file : public wxFileDropTarget
{
public:
    dnd_file ( main_listctrl *owner ) { m_droptarget_owner = owner; }
    virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& dropped_fullnames );

private:
    main_listctrl *m_droptarget_owner;
};


bool dnd_file::OnDropFiles( wxCoord, wxCoord, const wxArrayString& dropped_fullnames )
{
    wxArrayString fullnames_to_paste;
    
    fullnames_to_paste = dropped_fullnames;    
    // TODO: Fork here, in preferences as to which method of filename drops they want.
    m_droptarget_owner->paste_fullnames_as_new_separate_channels( fullnames_to_paste );

    fullnames_to_paste.Clear();

    // Okay to return true here, since can never remove the source files. If it 
    // was a TextDropTarget though, we would want to return false, so that can't remove
    // the source if it was a move not a copy.
    return TRUE;    
}


#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__)
    #include "resource/icons_dnd/dnd_copy.xpm"
    #include "resource/icons_dnd/dnd_move.xpm"
    #include "resource/icons_dnd/dnd_none.xpm"
#endif


#endif  //setupUSE_DRAG_AND_DROP
// ---------------------------------------------------------------------------------------

// Easier code maintenance if want to add/rearrangement of listctrl's columns.
enum {
    NAME_COLUMN     = 0,         // Channel's name
    DUE_COLUMN,                  // When channel is due 
};   
                           
// Popup menu (PU) item IDs                           
enum {
    PU_ADD_CHANNEL                              = wxID_HIGHEST + 1, 
    PU_CONFIGURE_CHANNEL,
    PU_DELETE_CHANNELS,
    PU_UPDATE_SELECTED_CHANNELS,
    PU_UPDATE_DUE_CHANNELS,
    PU_UPDATE_ALL_CHANNELS,
    PU_PASTE_URL_AS_NEW_CHANNEL,
    PU_PASTE_FULLNAMES_AS_NEW_SEPARATE_CHANNELS
};

IMPLEMENT_CLASS( main_listctrl, wxListCtrl )

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( main_listctrl, wxListCtrl )
    // A double-click or enter key on a listctrl row.
    EVT_LIST_ITEM_ACTIVATED( -1, main_listctrl::gui_configure_channel ) 
    //EVT_LEFT_DOWN( main_listctrl::drop_text )
    EVT_RIGHT_DOWN( main_listctrl::popup_menu )
   
    EVT_MENU( PU_ADD_CHANNEL, main_listctrl::gui_add_channel )
    EVT_MENU( PU_CONFIGURE_CHANNEL, main_listctrl::gui_configure_channel )
    EVT_MENU( PU_DELETE_CHANNELS, main_listctrl::gui_delete_channels )
    EVT_MENU( PU_UPDATE_SELECTED_CHANNELS, main_listctrl::gui_update_selected_channels )
    EVT_MENU( PU_UPDATE_DUE_CHANNELS, main_listctrl::gui_update_due_channels )
    EVT_MENU( PU_UPDATE_ALL_CHANNELS, main_listctrl::gui_update_all_channels )
    EVT_MENU( PU_PASTE_URL_AS_NEW_CHANNEL, main_listctrl::on_paste_menu_event )
    EVT_MENU( PU_PASTE_FULLNAMES_AS_NEW_SEPARATE_CHANNELS, main_listctrl::on_paste_menu_event )    
   
    EVT_SIZE( main_listctrl::OnSize )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Interface
//----------------------------------------------------------------------------------------

// Constructor
main_listctrl::main_listctrl( wxWindow *parent, wxWindowID id,
                              const wxPoint& pos, const wxSize& size,
                              long style, const wxValidator& validator,
                              const wxString& name )
   : wxListCtrl( parent, id, pos, size, style, validator, name )
{
    m_parent = parent;
    
    // This listctrl needs to insert its columns in the constructor, since as soon
    // as the listctrl is built, it is resized and grafted onto an "unknown" XRC 
    // placeholder. This induces an OnSize() event, calling the overrriden OnSize function for 
    // this class, which needs to have 3 columns to resize (else an assert on WXGTK debug
    // build).
    InsertColumn( NAME_COLUMN, _( "Channel name" ), wxLIST_FORMAT_LEFT, 160 );
    InsertColumn( DUE_COLUMN, _( "Due" ), wxLIST_FORMAT_LEFT, 100 );  
    
#if ( setupUSE_DRAG_AND_DROP )
    // Associate a filename drop targets with this listctrl
    SetDropTarget( new dnd_file( this ) );
#endif
    
}


// Destructor
main_listctrl::~main_listctrl()
{
    // Clear the memory occupied by the array of section names.
    m_row_channel_section_array.Clear();
}


//Initialize the list control.
void main_listctrl::init()
{       
    // wxListCtrl uses a wxImageList to insert its graphics.  Set the wxImageList that the
    // listctrl should use, to the shared image list that is used throughout the application.
    // The second argument just tells the type of image list, that it is a small type.
    // Usually we hide the listctrl while we insert rows (then show when done , but here it
    // is not needed, since the whole main dialog is not visible yet, so therefore the
    // main_listctrl is already hidden too at this stage.
    SetImageList( the_small_image_list, wxIMAGE_LIST_SMALL );
 
    // Load the channels into the rows of the table
    load_rows();
    
    // Adjust the column widths (recycled on the OnSize event)    
    set_column_widths();   
    
    wxLogDebug( "Completed main_listctrl::init()" );
} 


void main_listctrl::gui_add_channel()
{       
    // Get a string by popping a text dialog.
    wxString new_channel_name;    
    
    new_channel_name = get_channel_name_from_text_entry_dialog();
       
    // Makes a valid config section automagically using the parameter, copies all keys 
    // from 'default for new channels' section of config, to this new config, then sets 
    // the doc_name key to this channel_name. The wxString returned is the channel_section
    // that it generated.
    
    // Only do it if not an Empty String returned (ie didn't hit cancel)
    if ( new_channel_name == wxEmptyString ) {    
        return;
    }
       
    wxString        generated_channel_section;    
    channel_type    channel;
    
    channel.doc_name            = new_channel_name;
    generated_channel_section   = the_plucker_controller->add_channel( channel );
    
    launch_dialog_for_new_channel( generated_channel_section );
}


// Note: also used on the list control double-click event
void main_listctrl::gui_configure_channel()
{    
    wxString section_string;
    long selected_row_number = -1;       
    
    // Get first selected item
    selected_row_number = GetNextItem( selected_row_number, wxLIST_NEXT_ALL, 
                                       wxLIST_STATE_SELECTED );
        
    // No rows that fit the flag of being in a selected state, so abort
    if ( selected_row_number == -1 )
        return; 
        
    // Get the section of a row
    section_string = get_row_channel_section( selected_row_number );

    // Create a channel dialog
    channel_dialog a_channel_dialog( m_parent, section_string );            
    
    // Show modal, and if clicked OK, then refresh the row, which will repaint as required.
    if ( a_channel_dialog.ShowModal() == wxID_OK ) {    
        refresh_row( selected_row_number );
    }            
}


void main_listctrl::gui_delete_channels( bool prompt_first )
{    
    bool prompt_returned_ok = false;
    
    if ( prompt_first ) {
        wxMessageDialog confirm_delete_messagedialog ( this,
                  _( "Are you sure you wish to permanently delete the selected channels?" ),
                  _( "Confirm channel delete" ),
                  wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION );       
        
        // Show the dialog, and if the YES button was pressed, then proceed to delete
        if ( confirm_delete_messagedialog.ShowModal() == wxID_YES ) {
            prompt_returned_ok = true;  
            wxLogDebug( "Prompt: delete channels returned true" );
        }      
    } 
    
    // If said okay, or not prompting first, then do the deletion.
    if ( prompt_returned_ok || ! prompt_first ) {
    
        wxLogDebug( "Either no prompt, or prompt returned okay, so starting delete loop" );
        
        // Make a list of the section columns that are currently selected in listctrl
        wxArrayString channel_sections;
        get_selected_channel_sections( &channel_sections );                 
        // Delete their config entries and subdirectories
        the_plucker_controller->delete_channels( channel_sections );
        channel_sections.Clear();
        
        // Make a list of selected row numbers
        wxArrayInt selected_row_numbers;
        get_selected_row_numbers( &selected_row_numbers );
        
        // Delete in reverse order (as list row numbers will slide up as each is 
        // deleted, which causes them to renumber. But if delete bottom to top, this no
        // longer is a concern).
        if ( ! selected_row_numbers.IsEmpty() ) {
            for ( long n = ( selected_row_numbers.GetCount() -1 ); 0 <= n; n-- ) {
                // Remove it from the listctrl
                DeleteItem( selected_row_numbers[ n ] );
                // Remove it from our array of section names
                remove_row_channel_section( selected_row_numbers[ n ] );
            }
        }

        selected_row_numbers.Clear();          
    }    
}


void  main_listctrl::gui_update_selected_channels()
{   
    wxArrayString channel_sections;    
    
    // Make a list of the section columns that are currently selected
    get_selected_channel_sections( &channel_sections );
    
    // Now call our updating function, either console or progress dialog
    wxString spidering_display_mode;
    spidering_display_mode = the_configuration->Read( "/PLUCKER-DESKTOP/spidering_display_mode",    
                                                      "console" );
                                                      
    if ( spidering_display_mode == "console" ) {
        the_plucker_controller->update_channels( channel_sections );    
    } else {  
        progress_dialog a_progress_dialog( m_parent, channel_sections );
        a_progress_dialog.ShowModal();
    }
    
    // Refresh the Due column
    refresh_rows_due_column( channel_sections );    
    
    channel_sections.Clear();    
}


void  main_listctrl::gui_update_due_channels()
{    
    wxArrayString channel_sections;
    
    // Store a list of due channels before update_due(which will change them)
    the_plucker_controller->get_due_channels( channel_sections );            
    
    // Update if some are due, else show a message that none are due
    if ( ! channel_sections.IsEmpty() ) {
        // Do the update
        
        // First, call our updating function, either console or progress dialog
        wxString spidering_display_mode;
        spidering_display_mode = the_configuration->Read( "/PLUCKER-DESKTOP/spidering_display_mode",    
                                                          "console" );                                                      
        if ( spidering_display_mode == "console" ) {
            the_plucker_controller->update_due_channels();    
        } else {
            wxArrayString update_sections_array;
            the_plucker_controller->get_due_channels( update_sections_array );     
            progress_dialog a_progress_dialog( m_parent, update_sections_array );  
            a_progress_dialog.ShowModal();
            update_sections_array.Clear();       
        }       
        
        // Then, refresh the Due column
        refresh_rows_due_column( channel_sections );
    } else {
        wxMessageDialog no_due_messagedialog ( this,
          _( "No channels are currently due at this time." ),
          _( "All due channels are updated" ),
          wxOK | wxICON_INFORMATION );   
        // Show it              
        no_due_messagedialog.ShowModal();
    }
    
    channel_sections.Clear();
}


void  main_listctrl::gui_update_all_channels()
{    
    // Now call our updating function, either console or progress dialog
    wxString spidering_display_mode;
    spidering_display_mode = the_configuration->Read( "/PLUCKER-DESKTOP/spidering_display_mode",    
                                                      "console" );
                                                      
    if ( spidering_display_mode == "console" ) {
        the_plucker_controller->update_all_channels();   
    } else {
        wxArrayString update_sections_array;
        the_plucker_controller->get_all_channels( update_sections_array );    
        progress_dialog a_progress_dialog( m_parent, update_sections_array ); 
        a_progress_dialog.ShowModal();
        update_sections_array.Clear();
    }
    
}


// Only use this for newly created channels
void main_listctrl::launch_dialog_for_new_channel( const wxString& new_channel_section )
{
    if ( new_channel_section.IsEmpty() ) {
        wxLogError( "Error: can't open a channel dialog for an empty section." );
        return;
    }
    
    wxString channel_section = new_channel_section;
    
    // Show the configuration dialog, ready to edit the new channel.
    // Can't make them a child of this window (freezes) or parent (since an
    // XRC unknown container and says 2 things can't occupy it), so needs to be a 
    // child of the main_dialog (probably could be grandparent, but this is more 
    // readable).
    channel_dialog a_channel_dialog( m_parent, channel_section );
    
    // Show modal, and if clicked OK, then refresh the row, which will repaint as required.
    if ( a_channel_dialog.ShowModal() == wxID_OK ) {    
        insert_row( channel_section );
    } else {
        // Cancel button on channel dialog was clicked
        // Cancel button on channel dialog doesn't (and shouldn't, since a cancel) 
        // flush changes.
        // However, a flush is needed to cement the aborted section from memory 
        // into the written file so that a DeleteGroup() will find it in the file,
        // [called from within the plucker controller's delete channel function],
        // instead of the section existing in memory only, so won't see it until 
        // next flush, by which time we will have forgotten what it was. 
        the_configuration->Flush();
        // If didn't return OK, then must have clicked cancel to close the channel dialog
        // Since canceled before finished adding the chennel, delete the channel. No
        // gui delete needed, since never inserted the row.            
        the_plucker_controller->delete_channel( channel_section );       
    }               
}


wxString main_listctrl::get_channel_name_from_text_entry_dialog()
{
     wxString new_channel_name;
     new_channel_name = wxGetTextFromUser( _( "Enter a descriptive name for the new channel:" ),
                                              _( "Add a new channel" ),
                                              _( "Unnamed channel" ),
                                              this
                                            );    
    return new_channel_name;                                        
}


//----------------------------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------------------------

void main_listctrl::load_rows()
{
    wxString channel_section;   // Section of channel in configuration file
    int row_number           = 0;     // List row to insert channel information    
    long index               = 0;     // Index counter for looping through the sections
    bool found_section       = FALSE; // To monitor success of getting first/next group
    
    wxLogDebug( "Starting to load channel sections into listctrl rows" );
    
    // Make a "channels" directory to hold home.html, cache and other future stuff
    // for each entry
    if ( ! wxDirExists( get_plucker_directory( CHANNELS ) ) ) {
        wxMkdir( get_plucker_directory( CHANNELS ), 0777);
    }
    wxLogDebug( "Finished testing/trying to make channel directory" );
    
    // TODO: also make the default channel one, in case it is gone.
    
    found_section = the_configuration->GetFirstGroup( channel_section,
                                                      index );
    while ( found_section ) {
        // Load up the each channel section (group) from the
        // plucker.ini/pluckerrc file
        wxLogDebug( "Config section=%s", channel_section.c_str() );

       if ( is_channel_section( channel_section ) ) {
            // TODO: check for illegal characters in 
            // section name, and rename the section in the config file
            // as necessary before loading it into listctrl.
            
            // TODO: End these depreciated keys, and merge this to insert_row() function
            
            wxString doc_name;

            doc_name = channel_section + "/doc_name";
            
            // If no doc_name key, then use the db_name key
            // The db_name key is the depreciated equivalent of doc_name key
            if ( ! the_configuration->Exists( doc_name ) )
                doc_name = channel_section + "/db_name";
            
            // If no db_name key either, the the doc_file key as the doc_name also
            if ( ! the_configuration->Exists( doc_name ) ) 
                doc_name = channel_section + "/doc_file";
            
            // The no db_name or doc_file key, then use the db_file key.
            // The db_file key is the depreciated equivalent of doc_file key
            if ( ! the_configuration->Exists( doc_name ) ) 
                doc_name = channel_section + "/db_file";
                                      
            // Only include channels with an assigned document name
            if ( the_configuration->Exists( doc_name ) ) {
                wxString channel_name;
                wxString due_date;
                
                // Read the current channel name, from configuration file, 
                // or generate a channel name based on values or /doc_file or 
                // /db_file 
                channel_name = the_configuration->Read( doc_name,
                                                        _( "Unnamed Channel" ) );
                                                        
                // Write a doc_name key, since the depreciated ones aren't going
                // to be looked for after this
                the_configuration->Write( '/' + channel_section + '/' + "doc_name",
                                          channel_name );
                
                // If it was a filename key, then strip off path and the 
                // extension, so only basename left
                if ( doc_name == channel_section + "/doc_file" ||
                     doc_name == channel_section + "/db_file" ) {
                    wxSplitPath( channel_name.c_str(), NULL, &channel_name, NULL ); 
                }
                // Insert a new row (item in wxWindows) in the listctrl using
                // the channel_name value as the string in the 1st column. The
                // 3rd argument of InsertItem is the image index of the shared
                // image list (called 'the_small_image_list'). This 3rd argument
                // creates a little glyph for the row.
                InsertItem( row_number, channel_name,
                            SMALL_IMAGE_LIST_ID_MAIN_DIALOG_LISTCTRL );   

                // Read the due date and put it in its column.       
                wxDateTime  due_datetime;
                wxString    due_string;

                due_datetime   = the_plucker_controller->get_channel_due_datetime( channel_section );    
                due_string     = due_datetime.Format( wxT( plkrPRETTY_COMPACT_DATE_TIME_FORMAT ) ).c_str();                   

                if ( ! the_plucker_controller->is_channel_update_enabled( channel_section ) ) {
                    due_string = _( "Never" );
                }                                               
                
                SetItem( row_number, DUE_COLUMN, due_string );     
            
                // Put the section_name into our array of section names so know what section
                // of configuration file to call when configure/update the channel.
                insert_row_channel_section( channel_section, row_number ); 
            
                // Make a "channel" and a "channel/cache" directory for each entry,
                // and making a home.html and exclusionlist.txt if there isn't one.
                the_plucker_controller->create_new_channel_subdirectory( channel_section );
            
                // Increase the row number where we will insert new one.            
                row_number++;
            }
        }
        found_section = the_configuration->GetNextGroup( channel_section,
                                                         index );
    }
}


// Get an array of sections names from channel rows currently selected in the listctrl
void main_listctrl::get_selected_channel_sections( wxArrayString *channel_sections )
{   
    wxString    section_string;   
    long        selected_row_number = -1;  // '-1' includes the first selected row.
    
    wxLogDebug( "Entering selected channels function" );                                   
        
    for ( ;; ) {
        // for( ;; ) with this next line is the recommended way for iterating selected rows.
        // selected_row_number was initialized at -1 to allow inclusion of first selected
        // row.
        selected_row_number = GetNextItem( selected_row_number, wxLIST_NEXT_ALL,
                                           wxLIST_STATE_SELECTED );
        
        // No more rows that fit the flag of being in a selected state, so break loop.
        if ( selected_row_number == -1 ) {
            break; 
        }
        
        // Use our function to extract the string contents from a cell
        section_string = get_row_channel_section( selected_row_number );
    
        channel_sections->Add( section_string );
        
        wxLogDebug( "Appended section %s to selected sections", section_string.c_str() );
    }
}


// Get an array of row numbers currently selected in the listctrl
void main_listctrl::get_selected_row_numbers( wxArrayInt *row_numbers )
{   
    long  selected_row_number = -1;  // '-1' needed to include the first selected row.
    
    wxLogDebug( "Entering selected row numbers function" );                                   
        
    for ( ;; ) {
        // for( ;; ) with this next line is the recommended way for iterating selected rows.
        // selected_row_number was initialized at -1 to allow inclusion of first selected
        // row.
        selected_row_number = GetNextItem( selected_row_number, wxLIST_NEXT_ALL,
                                           wxLIST_STATE_SELECTED );
        
        if ( selected_row_number == -1 ) {
            break;           
        } else {
            row_numbers->Add( selected_row_number );        
            wxLogDebug( "Appended row %ld to selected rows", selected_row_number );
        }            
        
    }
}


void main_listctrl::insert_row( wxString &channel_section )
{    
    wxString    channel_name;
    wxDateTime  due_datetime;
    wxString    due_string;          
    wxListItem  cell_info;
    long        newly_inserted_row_number;
    
    // Get the cell strings that will be inserting
    channel_name = the_configuration->Read( '/' + channel_section + '/' + "doc_name",
                                            _( "Unnamed Channel" ) );
    
    due_datetime = the_plucker_controller->get_channel_due_datetime( channel_section );    
    due_string   = due_datetime.Format( wxT( plkrPRETTY_COMPACT_DATE_TIME_FORMAT ) ).c_str(); 
    
    // Describe the wxListInfo to insert into the initial cell.of row
    cell_info.m_mask    = wxLIST_MASK_TEXT;
    cell_info.m_col     = NAME_COLUMN;
    cell_info.m_text    = channel_name; 
    cell_info.m_image   = SMALL_IMAGE_LIST_ID_MAIN_DIALOG_LISTCTRL;
    
    // Get the number of the last row, while inserting the 2 columns
    newly_inserted_row_number = InsertItem( cell_info );   
    SetItem( newly_inserted_row_number, DUE_COLUMN, due_string );     
    // Insert the channel section string into our array of strings.
    insert_row_channel_section( channel_section, newly_inserted_row_number ); 
}


void main_listctrl::refresh_row( long row_number )
{
    wxString    channel_name;
    wxDateTime  due_datetime;
    wxString    due_string;
    wxString    channel_section;
    
    channel_section = get_row_channel_section( row_number );
    
    channel_name = the_configuration->Read( '/' + channel_section + '/' + 
                                            "doc_name", _( "Unnamed Channel" ) );
    
    due_datetime = the_plucker_controller->get_channel_due_datetime( channel_section );    
    due_string   = due_datetime.Format( wxT( plkrPRETTY_COMPACT_DATE_TIME_FORMAT ) ).c_str();    

    if ( ! the_plucker_controller->is_channel_update_enabled( channel_section ) ) {
        due_string = _( "Never" );
    }
    
    // Only repaint each if they are different from current (prevents unneeded flicker)    
    if ( channel_name != get_cell_contents_string( row_number, NAME_COLUMN ) ) {                                       
        SetItem( row_number, NAME_COLUMN, channel_name, 
                 SMALL_IMAGE_LIST_ID_MAIN_DIALOG_LISTCTRL );   
    }
    
    if ( due_string != get_cell_contents_string ( row_number, DUE_COLUMN ) ) {       
        SetItem( row_number, DUE_COLUMN, due_string );
    }
}


void main_listctrl::refresh_rows_due_column( wxArrayString& channel_sections )
{  
    size_t      number_of_sections           = channel_sections.GetCount();
    wxDateTime  due_datetime;
    wxString    due_string;
    wxString    channel_section;  
    long        row_number = -1;  // '-1' needed to include the first selected row.                                 

    if ( number_of_sections == 0 ) {
        return;
    }     
        
    for ( ;; ) {
        row_number = GetNextItem( row_number, wxLIST_NEXT_ALL );
        
        if ( row_number == -1 ) {
            break; 
        }
        
        channel_section = get_row_channel_section( row_number );
        
        // Only proceed if the current channel section was in our array
        if ( channel_sections.Index( channel_section ) != wxNOT_FOUND ) {
        
            due_datetime   = the_plucker_controller->get_channel_due_datetime( channel_section );    
            due_string     = due_datetime.Format( wxT( plkrPRETTY_COMPACT_DATE_TIME_FORMAT ) ).c_str();        

            if ( ! the_plucker_controller->is_channel_update_enabled( channel_section ) ) {
                due_string = _( "Never" );
            }       
    
            if ( due_string != get_cell_contents_string ( row_number, DUE_COLUMN ) ) {       
                SetItem( row_number, DUE_COLUMN, due_string );
            }        
        }    
    }
}


// Gets the wxString of a channel's section in the config, based on its row number
wxString main_listctrl::get_row_channel_section( long row_number )
{
    wxString    channel_section;
    size_t      array_index;
    
    array_index     = (size_t)row_number;
    channel_section = m_row_channel_section_array.Item( array_index );
    
    return channel_section;    
}


// Sets a channel's section in the configuration, by its row number
void main_listctrl::set_row_channel_section( const wxString& channel_section,
                                             long row_number )
{
    wxString    new_channel_section;
    size_t      array_index;
    
    new_channel_section  = channel_section;
    array_index          = (size_t)row_number;
    m_row_channel_section_array.Item( array_index ) = new_channel_section;
}


// Inserts a channel section in the array, before the specified index
void main_listctrl::insert_row_channel_section( const wxString& channel_section,
                                                long row_number )
{
    wxString    new_channel_section;
    size_t      array_index;
    
    new_channel_section     = channel_section;
    array_index             = (size_t)row_number;    
    m_row_channel_section_array.Insert( new_channel_section, array_index );
}


// Deletes a channel section in the array, at the specified index
void main_listctrl::remove_row_channel_section( long row_number )
{
    size_t      array_index;  
    
    array_index  = (size_t)row_number;
    m_row_channel_section_array.Remove( array_index );    
}


// Returns the text of a cell, at a certain row and column
wxString main_listctrl::get_cell_contents_string( long row_number, int column ) 
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


void main_listctrl::popup_menu( wxMouseEvent& event )
{
    wxMenu      a_menu;

    a_menu.Append( PU_ADD_CHANNEL, _( "Add a new channel...") );
    a_menu.Append( PU_CONFIGURE_CHANNEL, _( "Configure selected channel..." ) );
    a_menu.Append( PU_DELETE_CHANNELS, _( "Delete selected channels" ) );
    a_menu.AppendSeparator();
    a_menu.Append( PU_UPDATE_SELECTED_CHANNELS, _( "Update selected channels" ) );
    a_menu.Append( PU_UPDATE_DUE_CHANNELS, _( "Update all due channels" ) );
    a_menu.Append( PU_UPDATE_ALL_CHANNELS, _( "Update all channels" ) );
    
    // If no listctrl rows selected, then disable the menu items that require selection
    if ( GetSelectedItemCount() == 0 ) {
        a_menu.Enable( PU_CONFIGURE_CHANNEL, false );
        a_menu.Enable( PU_DELETE_CHANNELS, false );
        a_menu.Enable( PU_UPDATE_SELECTED_CHANNELS, false );
    }
    
#if ( setupUSE_DRAG_AND_DROP )    
    a_menu.AppendSeparator();   
    a_menu.Append( PU_PASTE_URL_AS_NEW_CHANNEL, _( "Paste URL as new channel" ) );
    a_menu.Append( PU_PASTE_FULLNAMES_AS_NEW_SEPARATE_CHANNELS, _( "Paste files as separate new channel" ) );
    // Only enable this menu item if the item on the clipboard has a text format
    // DF_TEXT, DF_FILENAME stands for Data Format Text, Data Format Filename.
    if ( ! wxTheClipboard->IsSupported( wxDF_TEXT ) ) {
        a_menu.Enable( PU_PASTE_URL_AS_NEW_CHANNEL, false );        
    }  
    if ( ! wxTheClipboard->IsSupported( wxDF_FILENAME ) ) {
        a_menu.Enable( PU_PASTE_FULLNAMES_AS_NEW_SEPARATE_CHANNELS, false );        
    }      
#endif  // setupUSE_DRAG_AND_DROP

    // Show the popup menu (wxWindow::PopupMenu ), at the x,y position of the click event 
    PopupMenu( &a_menu, event.GetPosition() );
}


void main_listctrl::on_paste_menu_event( wxCommandEvent& event )
{
#if ( setupUSE_DRAG_AND_DROP ) 
   
    if ( event.GetId() == PU_PASTE_URL_AS_NEW_CHANNEL ) {
        wxString url_to_paste;
        // Only continue if returned successful extraction of text from clipboard
        if ( utils_clipboard::get_text_from_clipboard( url_to_paste ) ) {
            paste_text_url_as_new_channel( url_to_paste ); 
        }
    } else if ( event.GetId() == PU_PASTE_FULLNAMES_AS_NEW_SEPARATE_CHANNELS  ) {
        wxArrayString fullname_array;
        // Only continue if returned successful extraction of fullnames from clipboard
        if ( utils_clipboard::get_fullname_array_from_clipboard( fullname_array ) ) {
            paste_fullnames_as_new_separate_channels( fullname_array );
        }
        fullname_array.Clear();
    }   

#endif  // setupUSE_DRAG_AND_DROP 
}


void main_listctrl::paste_text_url_as_new_channel( const wxString& url )
{
#if ( setupUSE_DRAG_AND_DROP ) 

    wxString        generated_channel_section;
    channel_type    channel;
    
    // Set the channel settings to the item, and add the channel.
    channel.doc_name            = get_channel_name_from_text_entry_dialog();   
    channel.home_url            = url;

    generated_channel_section   = the_plucker_controller->add_channel( channel );   
        
    launch_dialog_for_new_channel( generated_channel_section );
    
#endif  // setupUSE_DRAG_AND_DROP 
}


void main_listctrl::paste_fullnames_as_new_separate_channels( const wxArrayString& fullnames )
{
    size_t      number_of_fullnames           = fullnames.GetCount();
    size_t      current_fullname_array_index;
    wxString    current_fullname;
    
    if ( number_of_fullnames == 0 ) {
        return;
    } 
    
    for ( current_fullname_array_index = 0; 
          (int)current_fullname_array_index < (int)number_of_fullnames;
          current_fullname_array_index++ ) {  
          // Clear the string from the previous loop iteration
          current_fullname.Empty();
          // Append file:// to the front of the file
          current_fullname << "file://" << fullnames.Item( current_fullname_array_index );                  
          // Ship it.if it was a single pasted url
          paste_text_url_as_new_channel( current_fullname );
    }
}


void main_listctrl::OnSize( wxSizeEvent &event )
{
    set_column_widths();
}


void main_listctrl::set_column_widths()
{
    wxLogDebug( "About to set main_listctrl column widths" );
    
    // Get width of entire listctrl
    wxLogDebug( "About to get main_listctrl name column width" );
    int name_column_width = GetSize().x;     
    wxLogDebug( "Got main_listctrl name column width" );
     
    // Subtract width of other columns, scrollbar, and some padding     
    name_column_width -= GetColumnWidth( DUE_COLUMN );
    name_column_width -= wxSystemSettings::GetSystemMetric( wxSYS_VSCROLL_X );     
    name_column_width -= plkrLISTCTRL_EXTRA_WIDTH_PADDING;
     
    // Set the column width to the new value.
    SetColumnWidth( NAME_COLUMN, name_column_width );  
    
    wxLogDebug( "Successfully set column widths" );
}


