//----------------------------------------------------------------------------------------
// Name:        main_listctrl.cpp
// Purpose:     Listctrl used in the main frame
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: main_listctrl.cpp,v 1.38 2007/06/30 14:55:43 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "main_listctrl.h"
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

#include "main_listctrl.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"
#include "wx/listctrl.h"
#if ( setupUSE_DRAG_AND_DROP )
    #include "wx/dnd.h"                 // Drag and drop classes
    #include "wx/clipbrd.h"             // wxClipBoard
#endif

//---------------------------------------------------------------------------------------

#include "main_frame.h"
#include "configuration.h"
#include "channel_dialog.h"
#include "build_progress_wrappers.h"
#include "image_list.h"
#include "plucker_controller.h"
#include "plucker_wizard_wrappers.h"
#include "utils_datetime.h"
#include "utils_string.h"
#include "utils_clipboard.h"
#include "utils_string.h"
#include "checkbox_message_dialog.h" 

//------------------------------------------------------------------------------------------
// Derive a class for drag and drop of files
//------------------------------------------------------------------------------------------

#if ( setupUSE_DRAG_AND_DROP )

class dnd_file : public wxFileDropTarget
{
public:
    dnd_file ( main_listctrl *owner ) { m_droptarget_owner = owner; }
    virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& dropped_fullnames );

private:
    main_listctrl *m_droptarget_owner;
};

// To do: no x and y here?? (check dnd example)
bool dnd_file::OnDropFiles( wxCoord, wxCoord, const wxArrayString& dropped_fullnames )
{
    wxArrayString fullnames_to_paste;
    
    fullnames_to_paste = dropped_fullnames;    
    // TODO: Fork here, in preferences as to which method of filename drops they want.
    m_droptarget_owner->paste_fullnames_as_new_separate_channels( fullnames_to_paste );

    fullnames_to_paste.Clear();

    // Okay to return TRUE here, since can never remove the source files. If it 
    // was a TextDropTarget though, we would want to return FALSE, so that can't remove
    // the source if it was a move not a copy.
    return TRUE;    
}

#endif  //setupUSE_DRAG_AND_DROP

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

// Easier code maintenance if want to add/rearrange listctrl's columns.
enum {
    NAME_COLUMN     = 0,         // Channel's name
    DUE_COLUMN                   // When channel is due 
};   
                           
// Popup menu (PU) item IDs                           
enum {
    PU_ADD_CHANNEL                              = wxID_HIGHEST + 2, 
    PU_ADD_CHANNEL_WIZARD,
    PU_CONFIGURE_CHANNEL,
    PU_DELETE_CHANNELS,
    PU_UPDATE_SELECTED_CHANNELS,
    PU_UPDATE_DUE_CHANNELS,
    PU_UPDATE_ALL_CHANNELS,
    PU_PASTE_URL_AS_NEW_CHANNEL,
    PU_PASTE_FULLNAMES_AS_NEW_SEPARATE_CHANNELS
};

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( main_listctrl, wxListCtrl )

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( main_listctrl, wxListCtrl )
    // A double-click or enter key on a listctrl row.
    EVT_LIST_ITEM_ACTIVATED( -1, main_listctrl::list_item_activated ) 
    EVT_RIGHT_DOWN( main_listctrl::popup_menu )
   
    EVT_MENU( PU_ADD_CHANNEL, main_listctrl::gui_add_channel )
    EVT_MENU( PU_ADD_CHANNEL_WIZARD, main_listctrl::gui_add_channel_wizard )
    EVT_MENU( PU_CONFIGURE_CHANNEL, main_listctrl::gui_configure_channel )
    EVT_MENU( PU_DELETE_CHANNELS, main_listctrl::gui_delete_channels )
    EVT_MENU( PU_UPDATE_SELECTED_CHANNELS, main_listctrl::gui_update_selected_channels )
    EVT_MENU( PU_UPDATE_DUE_CHANNELS, main_listctrl::gui_update_due_channels )
    EVT_MENU( PU_UPDATE_ALL_CHANNELS, main_listctrl::gui_update_all_channels )
    EVT_MENU( PU_PASTE_URL_AS_NEW_CHANNEL, main_listctrl::on_paste_url_as_new_channel_menuitem )
    EVT_MENU( PU_PASTE_FULLNAMES_AS_NEW_SEPARATE_CHANNELS, main_listctrl::on_paste_fullnames_as_new_separate_channels_menuitem )    
   
    EVT_SIZE( main_listctrl::OnSize )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Interface
//----------------------------------------------------------------------------------------

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


main_listctrl::~main_listctrl()
{
    wxLogDebug( wxT( "Entering main_listctrl destructor" ) );

    // Clear the memory occupied by the array of section names.
    m_row_channel_section_array.Clear();
    
    wxLogDebug( wxT( "Completed main_listctrl destructor" ) );
}


void main_listctrl::init()
{       
    // wxListCtrl uses a wxImageList to insert its graphics.  Set the wxImageList that the
    // listctrl should use, to the shared image list that is used throughout the application.
    // The second argument just tells the type of image list, that it is a small type.
    // Usually we hide the listctrl while we insert rows (then show when done , but here it
    // is not needed, since the whole main dialog is not visible yet, so therefore the
    // main_listctrl is already hidden too at this stage.
    SetImageList( image_list::get(), wxIMAGE_LIST_SMALL );
 
    // Load the channels into the rows of the table
    load_rows();
    
    // Adjust the column widths (recycled on the OnSize event)    
    set_column_widths();   
    
    wxLogDebug( wxT( "Completed main_listctrl::init()" ) );
} 


void main_listctrl::gui_add_channel( wxCommandEvent& event )
{  
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );
     
    // Get a string by popping a text dialog.
    wxString new_channel_name;

    new_channel_name = get_channel_name_from_text_entry_dialog();
       
    // Makes a valid config section automagically using the parameter, copies all keys 
    // from 'default for new channels' section of config, to this new config, then sets 
    // the doc_name key to this channel_name. The wxString returned is the channel_section
    // that it generated.
    
    // Only do it if not an Empty String returned (ie didn't hit cancel)
    if ( new_channel_name == wxEmptyString ) 
    {    
        return;
    }
       
    wxString        generated_channel_section;    
    channel_type    channel;
    
    channel.doc_name            = new_channel_name;
    generated_channel_section   = plucker_controller::get()->add_channel( channel );
    
    launch_dialog_for_new_channel( generated_channel_section );
	
    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
}


void main_listctrl::gui_add_channel_wizard( wxCommandEvent& event )
{
#if ( setupUSE_WIZARDS )
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );
    
    bool        clicked_finish_button;
    wxString    generated_channel_section;
    bool        is_launch_channel_dialog_requested;
    
    clicked_finish_button = plucker_wizard_wrappers::run_add_channel_wizard( m_parent,
                &generated_channel_section, &is_launch_channel_dialog_requested );
    
    // If they clicked the finish button...
    if ( clicked_finish_button )
    {
        // ...and they want to launch a channel dialog...
        if ( is_launch_channel_dialog_requested )
        {
            // Launch a dialog for the new channel
            launch_dialog_for_new_channel( generated_channel_section );
        // ..or if they didn't want to launch a channel dialog...
        } 
        else
        {
            // ...just insert the row
            insert_row( generated_channel_section );
        }
    }    

    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
#endif			// setupUSE_WIZARDS



}


void main_listctrl::list_item_activated( wxListEvent& event )
{  
    gui_configure_channel( dynamic_cast<wxCommandEvent&>(event) );
}


void main_listctrl::gui_configure_channel( wxCommandEvent& event )
{    
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );

    wxString section_string;
    long selected_row_number = -1;

    // Get first selected item
    selected_row_number = GetNextItem( selected_row_number, wxLIST_NEXT_ALL, 
                                       wxLIST_STATE_SELECTED );
        
    // No rows that fit the flag of being in a selected state, so abort
    if ( selected_row_number == -1 ) 
    {
        plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
        return; 
    }
        
    // Get the section of a row
    section_string = get_row_channel_section( selected_row_number );

    wxLogDebug( wxT( "About to show a channel dialog using m_parent as parent" ) );
    // Create a channel dialog
    channel_dialog a_channel_dialog( m_parent, section_string );            
    wxLogDebug( wxT( "Showed a channel dialog using m_parent as parent" ) );
    
    // Show modal, and if clicked OK, then refresh the row, which will repaint as required.
    if ( a_channel_dialog.ShowModal() == wxID_OK ) 
    {
        refresh_row( selected_row_number );
    }            
    
    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
}


void main_listctrl::gui_delete_channels_now( bool prompt_first )
{   
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );
 
    bool prompt_returned_ok = FALSE;

    if ( prompt_first ) 
    {
        wxMessageDialog confirm_delete_messagedialog( this,
                  _( "Are you sure you wish to permanently delete the selected channels?" ),
                  _( "Confirm channel delete" ),
                  wxYES | wxNO | wxYES_DEFAULT | wxICON_QUESTION );

        // Show the dialog, and if the YES button was pressed, then proceed to delete
        if ( confirm_delete_messagedialog.ShowModal() == wxID_YES ) 
        {
            prompt_returned_ok = TRUE;  
            wxLogDebug( wxT( "Prompt: delete channels returned TRUE" ) );
        }      
    } 
    
    // If said okay, or not prompting first, then do the deletion.
    if ( prompt_returned_ok || ! prompt_first )
    {
        wxLogDebug( wxT( "Either no prompt, or prompt returned okay, so starting delete loop" ) );
        
        // Make a list of the section columns that are currently selected in listctrl
        wxArrayString channel_sections;
        get_selected_channel_sections( &channel_sections );                 
        // Delete their config entries and subdirectories
        plucker_controller::get()->delete_channels( channel_sections );
        channel_sections.Clear();
        
        // Make a list of selected row numbers
        wxArrayInt selected_row_numbers;
        get_selected_row_numbers( &selected_row_numbers );
        
        // Delete in reverse order (as list row numbers will slide up as each is 
        // deleted, which causes them to renumber. But if delete bottom to top, this no
        // longer is a concern).
        if ( ! selected_row_numbers.IsEmpty() ) 
        {
            for ( long n = ( selected_row_numbers.GetCount() -1 ); 0 <= n; n-- ) 
            {
                // Remove it from the listctrl
                DeleteItem( selected_row_numbers[ n ] );
                // Remove it from our array of section names
                remove_row_channel_section( selected_row_numbers[ n ] );
            }
        }

        selected_row_numbers.Clear();          
    }    
    
    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
}


void main_listctrl::gui_delete_channels( wxCommandEvent& event )
{   
    gui_delete_channels_now();
}


void  main_listctrl::gui_update_selected_channels( wxCommandEvent& event )
{   
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );

    wxArrayString channel_sections;    

    // Make a list of the section columns that are currently selected
    get_selected_channel_sections( &channel_sections );
    
    // Ensure that there is destinations provided for all the channels    
    ensure_destinations_provided( channel_sections );
    
    // Now call our updating function, either "console" or "dialog"
    wxString spidering_display_mode;
    spidering_display_mode = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/spidering_display_mode" ),    
                                                      wxT( "dialog" ) );

    if ( spidering_display_mode == wxT( "console" ) ) 
    {
        plucker_controller::get()->update_channels( channel_sections );
    } 
    else 
    {
        build_progress_wrappers::launch_build_progress_dialog( m_parent, channel_sections );
    }

    // Refresh the rows. Have to update name too, in case they changed it in the 
    // channel dialog when changing the destinations.
    refresh_rows( channel_sections );    
    
    channel_sections.Clear();
    
    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
}


void  main_listctrl::gui_update_due_channels( wxCommandEvent& event )
{
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );

    wxArrayString channel_sections;

    // Store a list of due channels before update_due(which will change them)
    plucker_controller::get()->get_due_channels( channel_sections );

    // Update if some are due, else show a message that none are due
    if ( ! channel_sections.IsEmpty() ) 
    {
        // Ensure that there is destinations provided for all the channels
        ensure_destinations_provided( channel_sections );
        
        // Do the update
        // First, call our updating function, either console or progress dialog
        wxString spidering_display_mode;
        spidering_display_mode = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/spidering_display_mode" ),
                                                          wxT( "dialog" ) );
        if ( spidering_display_mode == wxT( "console" ) ) 
        {
            plucker_controller::get()->update_due_channels();
        } 
        else 
        {
            wxArrayString update_sections_array;
            plucker_controller::get()->get_due_channels( update_sections_array );
            build_progress_wrappers::launch_build_progress_dialog( m_parent, update_sections_array );
            update_sections_array.Clear();
        }

    // Refresh the rows. Have to update name too, in case they changed it in the
    // channel dialog when changing the destinations.
        refresh_rows( channel_sections );
    } 
    else
    {
        wxMessageDialog no_due_messagedialog ( this,
          _( "No channels are currently due at this time." ),
          _( "All due channels are updated" ),
          wxOK | wxICON_INFORMATION );   
        // Show it              
        no_due_messagedialog.ShowModal();
    }
    
    channel_sections.Clear();

    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
}


void main_listctrl::gui_update_all_channels( wxCommandEvent& event )
{   
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );
 
    // Now call our updating function, either console or progress dialog
    wxString        spidering_display_mode;
    spidering_display_mode = the_configuration->Read( wxT( "/PLUCKER_DESKTOP/spidering_display_mode" ),    
                                                      wxT( "dialog" ) );
                                                      
    wxArrayString   update_sections_array;
    plucker_controller::get()->get_all_channels( update_sections_array );    
                                                    
    // Ensure that there is destinations provided for all the channels    
    ensure_destinations_provided( update_sections_array );
    
    // If there is no channels, then just show a message box instead of 
    // starting up an empty progress dialog.
    if ( update_sections_array.GetCount() == 0 ) 
    {
          // Make a message dialog
          wxMessageDialog no_channels_messagedialog ( this,
          _( "You currently have no channels in your list. Would you like to add a channel now?" ),
          _( "No channels to update" ),
          wxYES | wxNO | wxYES_DEFAULT | wxICON_QUESTION );        
        // Show the dialog, and if the YES button was pressed, then proceed to delete
        if ( no_channels_messagedialog.ShowModal() == wxID_YES ) 
        {        
#if ( setupUSE_WIZARDS )        
            gui_add_channel_wizard( event );
#else   //setupUSE_WIZARDS
            gui_add_channel( event );
#endif  //setupUSE_WIZARDS
        }
        else 
        {
            // Abort, to stop a progress dialog/console from occurring.
            return;
        }
    }

    if ( spidering_display_mode == wxT( "console" ) ) 
    {
        plucker_controller::get()->update_all_channels();   
    } 
    else 
    {    
        build_progress_wrappers::launch_build_progress_dialog( m_parent, update_sections_array );     
    }
    
    // Refresh the rows. Have to update name too, in case they changed it in the 
    // channel dialog when changing the destinations.
    refresh_rows( update_sections_array );
    
    update_sections_array.Clear();
    
    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );    
}


void main_listctrl::launch_dialog_for_new_channel( const wxString& new_channel_section )
{
    if ( new_channel_section.IsEmpty() ) 
    {
        wxLogError( wxT( "Error: can't open a channel dialog for an empty section." ) );
        return;
    }
    
    wxString channel_section = new_channel_section;
    
    // Show the configuration dialog, ready to edit the new channel.
    // Can't make them a child of this window (freezes) or parent (since an
    // XRC unknown container and says 2 things can't occupy it), so needs to be a 
    // child of the main_frame (probably could be grandparent, but this is more 
    // readable).
    channel_dialog a_channel_dialog( m_parent, channel_section );
    
    // Show modal, and if clicked OK, then refresh the row, which will repaint as required.
    if ( a_channel_dialog.ShowModal() == wxID_OK ) 
    {    
        // insert_row also adds it to our array
        insert_row( channel_section );
    } 
    else
    {
		// If didn't return OK, then must have clicked cancel to close the channel dialog
        // Since canceled before finished adding the channel, delete the channel. No
        // gui delete needed, since never inserted the row.   
		wxLogDebug( wxT( "main_listctrl::launch_dialog_for_new_channel != wxID_OK so flushing..." ) );
	 
		// This part of the code section gets run before the channel_dialog::~channel_dialog()
		// has completed [The destructor would be run when the a_channel_dialog goes out of scope?]
		// We need this the_configuration->SetPath( wxT( "/" ) ); [is also located in 
		// channel_dialog::~channel_dialog()], but we need it now, since otherwise we'd 
		// still be inside the /WHATEVER_CHANNEL_PATH of the config, and can't when go
		// to plucker_controller::get()->delete_channel, it would be thus looking to delete 
		// /WHATEVER_CHANNEL_PATH/WHATEVER_CHANNEL_PATH, instead of /WHATEVER_CHANNEL_PATH
		the_configuration->SetPath( wxT( "/" ) );
		wxLogDebug( wxT( "...and configuration path was reset to /..." ) );
		
        plucker_controller::get()->delete_channel( channel_section );       
        wxLogDebug( wxT( "...and plucker_controller::delete_channel was called..." ) );        
        
        // Then flush again to write the new configuration with the section now removed.
        the_configuration->Flush();                
        wxLogDebug( wxT( ".. and finally Flush()'ed the configuration to write the deletion" ) );
    }               
}


wxString main_listctrl::get_channel_name_from_text_entry_dialog()
{
    // Make adefault value of the add button, the name that was specified in the
    // New Channel Template
    wxString doc_name_key;
    doc_name_key << wxT( "/" ) << plkrNEW_CHANNEL_TEMPLATE_SECTION << wxT( "/" ) << wxT( "doc_name" );
    wxString default_channel_name = the_configuration->Read( doc_name_key, 
                                                             plkrUNNAMED_CHANNEL_NAME );

    // Get the text entered by the user
    wxString new_channel_name;
    new_channel_name = wxGetTextFromUser( _( "Enter a descriptive name for the channel (maximum of 26 letters):" ),
                                          _( "Add a new channel" ),
                                          default_channel_name,
                                          this
                                        );

    // Shave it down to max length of the channel name to plkrMAXIMUM_CHANNEL_NAME_LENGTH
    new_channel_name.Truncate( plkrMAXIMUM_CHANNEL_NAME_LENGTH );

    // Return the text
    return new_channel_name;
}

//----------------------------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------------------------

void main_listctrl::load_rows()
{
    wxString channel_section;             // Section of channel in configuration file
    int      row_number          = 0;     // List row to insert channel information    
    long     index               = 0;     // Index counter for looping through the sections
    bool     found_section       = FALSE; // To monitor success of getting first/next group
    
    wxLogDebug( wxT( "Starting to load channel sections into listctrl rows" ) );
    
    // Make a "channels" directory to hold home.html, cache and other future stuff
    // for each entry.
    //! \todo Still need this? I think this is already done in plucker_controller::create_channels_directory()
    /* if ( ! wxDirExists( get_plucker_directory( CHANNELS ) ) ) 
    {
        wxMkdir( get_plucker_directory( CHANNELS ), 0777 );
    }
    wxLogDebug( wxT( "Finished testing/trying to make channel directory" ) );
    */
    
    found_section = the_configuration->GetFirstGroup( channel_section,
                                                      index );
    while ( found_section ) 
    {
        // Load up the each channel section (group) from the
        // plucker.ini/pluckerrc file
        wxLogDebug( wxT( "Config section=%s" ), channel_section.c_str() );

       if ( is_channel_section( channel_section ) )
       {
            // TODO: check for illegal characters in 
            // section name, and rename the section in the config file
            // as necessary before loading it into listctrl.
            
            // TODO: End these depreciated keys, and merge this to insert_row() function
            
            wxString doc_name;

            doc_name = channel_section + wxT( "/doc_name" );
            
            // If no doc_name key, then use the db_name key
            // The db_name key is the depreciated equivalent of doc_name key
            if ( ! the_configuration->Exists( doc_name ) )
                doc_name = channel_section + wxT( "/db_name" );
            
            // If no db_name key either, the the doc_file key as the doc_name also
            if ( ! the_configuration->Exists( doc_name ) ) 
                doc_name = channel_section + wxT( "/doc_file" );
            
            // The no db_name or doc_file key, then use the db_file key.
            // The db_file key is the depreciated equivalent of doc_file key
            if ( ! the_configuration->Exists( doc_name ) ) 
                doc_name = channel_section + wxT( "/db_file" );
                                      
            // Only include channels with an assigned document name
            if ( the_configuration->Exists( doc_name ) ) {
                wxString channel_name;
                wxString due_date;
                
                // Read the current channel name, from configuration file, 
                // or generate a channel name based on values or /doc_file or 
                // /db_file 
                channel_name = the_configuration->Read( doc_name,
                                                        plkrUNNAMED_CHANNEL_NAME );
                                                        
                // Write a doc_name key, since the depreciated ones aren't going
                // to be looked for after this
                the_configuration->Write( wxT( "/" ) + channel_section + wxT( "/" ) + wxT( "doc_name" ),
                                          channel_name );
                
                // If it was a filename key, then strip off path and the 
                // extension, so only basename left
                if ( doc_name == channel_section + wxT( "/doc_file" ) ||
                     doc_name == channel_section + wxT( "/db_file" ) ) 
                {
                    wxSplitPath( channel_name.c_str(), NULL, &channel_name, NULL ); 
                }
                // Insert a new row (item in wxWindows) in the listctrl using
                // the channel_name value as the string in the 1st column. The
                // 3rd argument of InsertItem is the image index of the shared
                // image list. This 3rd argument creates a little glyph for the row.
                InsertItem( row_number, channel_name,
                            SMALL_IMAGE_LIST_ID_MAIN_FRAME_LISTCTRL );   

                // Read the due date and put it in its column.       
                wxDateTime  due_datetime;
                wxString    due_string;

                due_datetime   = plucker_controller::get()->get_channel_due_datetime( channel_section, FALSE );    
                due_string     = due_datetime.Format( plkrPRETTY_COMPACT_DATE_TIME_FORMAT ).c_str();                   

                if ( ! plucker_controller::get()->is_channel_update_enabled( channel_section ) )
                {
                    due_string = _( "Never" );
                }                                               
                
                SetItem( row_number, DUE_COLUMN, due_string );     
            
                // Put the section_name into our array of section names so know what section
                // of configuration file to call when configure/update the channel.
                insert_row_channel_section( channel_section, row_number ); 
            
                // Make a "channel" and a "channel/cache" directory for each entry,
                // and making a home.html and exclusionlist.txt if there isn't one.
                plucker_controller::get()->create_new_channel_subdirectory( channel_section );
            
                // Increase the row number where we will insert new one.            
                row_number++;
            }
        }
        found_section = the_configuration->GetNextGroup( channel_section,
                                                         index );
    }
}


void main_listctrl::get_selected_channel_sections( wxArrayString* channel_sections )
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


void main_listctrl::get_selected_row_numbers( wxArrayInt* row_numbers )
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


void main_listctrl::insert_row( wxString &channel_section )
{        
    wxListItem  cell_info;
    long        newly_inserted_row_number; 
   
    // Describe the wxListInfo to insert into the initial cell.of row.
    // Just insert a blank "" string for channel name, as can just recycle refresh_row()
    // to actually do the work.
    cell_info.m_mask    = wxLIST_MASK_TEXT;
    cell_info.m_col     = NAME_COLUMN;
    cell_info.m_text    = wxT( " " ); 
    cell_info.m_image   = SMALL_IMAGE_LIST_ID_MAIN_FRAME_LISTCTRL;
    
    // Insert the row, thus getting the number of inserted row.
    newly_inserted_row_number = InsertItem( cell_info );  
    wxLogDebug( wxT( "main_listctrl: newly_inserted_row_number=%ld" ), newly_inserted_row_number );
    
    // Insert the channel section string into our array of strings.
    insert_row_channel_section( channel_section, newly_inserted_row_number ); 
    
    // Refresh the row (which will draw in the name and date into its columns).
    // NOTE: it seems that this have to go after insert_row_channel_section(), since
    // otherwise get a weird bug of refreshing the newly inserted row with the values 
    // of the row below it. I guess this way gives the GUI time to catch up.
    refresh_row( newly_inserted_row_number );         
}


void main_listctrl::refresh_row( long row_number )
{
    wxString    channel_name;
    wxDateTime  due_datetime;
    wxString    due_string;
    wxString    channel_section;

    channel_section = get_row_channel_section( row_number );
    channel_name = the_configuration->Read( wxT( "/" ) + channel_section + wxT( "/" ) +
                                            wxT( "doc_name" ), plkrUNNAMED_CHANNEL_NAME );


    due_datetime = plucker_controller::get()->get_channel_due_datetime( channel_section );
    due_string   = due_datetime.Format( plkrPRETTY_COMPACT_DATE_TIME_FORMAT ).c_str();

    if ( ! plucker_controller::get()->is_channel_update_enabled( channel_section ) ) 
    {
        due_string = _( "Never" );
    }

    // Only repaint each if they are different from current (prevents unneeded flicker)
    if ( channel_name != get_cell_contents_string( row_number, NAME_COLUMN ) )
    {
        SetItem( row_number, NAME_COLUMN, channel_name,
                 SMALL_IMAGE_LIST_ID_MAIN_FRAME_LISTCTRL );
    }

    if ( due_string != get_cell_contents_string ( row_number, DUE_COLUMN ) )
    {
        SetItem( row_number, DUE_COLUMN, due_string );
    }

}


void main_listctrl::refresh_rows( wxArrayString& channel_sections )
{  
    size_t      number_of_sections           = channel_sections.GetCount();
    wxString    channel_section;  
    long        row_number = -1;  // '-1' needed to include the first selected row.                                 

    if ( number_of_sections == 0 ) 
    {
        return;
    }     
        
    for ( ;; ) 
    {
        row_number = GetNextItem( row_number, wxLIST_NEXT_ALL );
        
        if ( row_number == -1 )
        {
            break; 
        }
        
        channel_section = get_row_channel_section( row_number );
        
        // Only proceed if the current channel section was in our array
        if ( channel_sections.Index( channel_section ) != wxNOT_FOUND )
        {                
            refresh_row( row_number );
        }    
    }
}


wxString main_listctrl::get_row_channel_section( long row_number )
{
    wxString    channel_section;
    size_t      array_index;
    
    array_index     = (size_t)row_number;
    channel_section = m_row_channel_section_array.Item( array_index );
    
    return channel_section;    
}


void main_listctrl::set_row_channel_section( const wxString& channel_section,
                                             long row_number )
{
    wxString    new_channel_section;
    size_t      array_index;
    
    new_channel_section  = channel_section;
    array_index          = (size_t)row_number;
    m_row_channel_section_array.Item( array_index ) = new_channel_section;
}


void main_listctrl::insert_row_channel_section( const wxString& channel_section,
                                                long row_number )
{
    wxString    new_channel_section;
    size_t      array_index;
    
    new_channel_section     = channel_section;
    array_index             = (size_t)row_number;    
    m_row_channel_section_array.Insert( new_channel_section, array_index );
}


void main_listctrl::remove_row_channel_section( long row_number )
{
    size_t      array_index;  
    
    array_index  = (size_t)row_number;
    m_row_channel_section_array.RemoveAt( array_index );    
}


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

    a_menu.Append( PU_ADD_CHANNEL, _( "Add a new channel..." ) );
    
#if ( setupUSE_WIZARDS )    
    a_menu.Append( PU_ADD_CHANNEL_WIZARD, _( "Add a new channel using wizard..." ) );
#endif    
    
    a_menu.Append( PU_CONFIGURE_CHANNEL, _( "Configure selected channel..." ) );
    a_menu.Append( PU_DELETE_CHANNELS, _( "Delete selected channels" ) );
    a_menu.AppendSeparator();
    a_menu.Append( PU_UPDATE_SELECTED_CHANNELS, _( "Update selected channels" ) );
    a_menu.Append( PU_UPDATE_DUE_CHANNELS, _( "Update all due channels" ) );
    a_menu.Append( PU_UPDATE_ALL_CHANNELS, _( "Update all channels" ) );
    
    // If no listctrl rows selected, then disable the menu items that require selection
    if ( GetSelectedItemCount() == 0 ) 
    {
        a_menu.Enable( PU_CONFIGURE_CHANNEL, FALSE );
        a_menu.Enable( PU_DELETE_CHANNELS, FALSE );
        a_menu.Enable( PU_UPDATE_SELECTED_CHANNELS, FALSE );
    }
    
#if ( setupUSE_DRAG_AND_DROP )    
    a_menu.AppendSeparator();   
    a_menu.Append( PU_PASTE_URL_AS_NEW_CHANNEL, _( "Paste URL as new channel" ) );
    a_menu.Append( PU_PASTE_FULLNAMES_AS_NEW_SEPARATE_CHANNELS, _( "Paste files as separate new channels" ) );
    // Enable/ disable this menu item, and the main frame menuitems if the thing on the 
    // clipboard has a text format. DF_TEXT, DF_FILENAME stands for Data Format Text,
    //  Data Format Filename.
    bool text_enabled       = wxTheClipboard->IsSupported( wxDF_TEXT ) ;
    bool filename_enabled   = wxTheClipboard->IsSupported( wxDF_FILENAME );
    a_menu.Enable( PU_PASTE_URL_AS_NEW_CHANNEL, text_enabled );
    a_menu.Enable( PU_PASTE_FULLNAMES_AS_NEW_SEPARATE_CHANNELS, filename_enabled );        
#endif  // setupUSE_DRAG_AND_DROP

    // Show the popup menu (wxWindow::PopupMenu ), at the x,y position of the click event 
    PopupMenu( &a_menu, event.GetPosition() );
}


void main_listctrl::on_paste_url_as_new_channel_menuitem( wxCommandEvent& event )
{
#if ( setupUSE_DRAG_AND_DROP ) 
    wxString url_to_paste;
    // Only continue if returned successful extraction of text from clipboard
    if ( utils_clipboard::get_text_from_clipboard( url_to_paste ) ) 
    {
        paste_text_url_as_new_channel( url_to_paste ); 
    }   
#endif  // setupUSE_DRAG_AND_DROP 
}


void main_listctrl::on_paste_fullnames_as_new_separate_channels_menuitem( wxCommandEvent& event )
{
#if ( setupUSE_DRAG_AND_DROP ) 
    wxArrayString fullname_array;
    // Only continue if returned successful extraction of fullnames from clipboard
    if ( utils_clipboard::get_fullname_array_from_clipboard( fullname_array ) )
    {
        paste_fullnames_as_new_separate_channels( fullname_array );
    }
    fullname_array.Clear();
#endif  // setupUSE_DRAG_AND_DROP 
}


void main_listctrl::paste_text_url_as_new_channel( const wxString& url )
{
#if ( setupUSE_DRAG_AND_DROP ) 
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );
    
    wxString        generated_channel_section;
    channel_type    channel;
    
    // Set the channel settings to the item, and add the channel.
    channel.doc_name            = get_channel_name_from_text_entry_dialog();   
    channel.home_url            = url;

    // It could well be empty, since using the "add channel" get text from user
    // call, which returns an empty string when press cancel.
    if ( channel.doc_name != wxEmptyString ) 
    {
        generated_channel_section   = plucker_controller::get()->add_channel( channel );           
        launch_dialog_for_new_channel( generated_channel_section );
    }    
    
    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
#endif  // setupUSE_DRAG_AND_DROP
}


void main_listctrl::paste_fullnames_as_new_separate_channels( const wxArrayString& fullnames )
{
#if ( setupUSE_DRAG_AND_DROP ) 
    plucker_controller::get()->set_is_new_update_execution_allowed( FALSE );
    
    size_t      number_of_fullnames           = fullnames.GetCount();
    size_t      current_fullname_array_index;
    wxString    current_fullname;
    
    if ( number_of_fullnames == 0 ) 
    {
        return;
    } 
    
    for ( current_fullname_array_index = 0; 
          (int)current_fullname_array_index < (int)number_of_fullnames;
          current_fullname_array_index++ ) 
    {  
          // Clear the string from the previous loop iteration
          current_fullname.Empty();
          // Append file:// to the front of the file
          current_fullname << wxT( "file://" ) << fullnames.Item( current_fullname_array_index );                  
          // Ship it.if it was a single pasted url
          paste_text_url_as_new_channel( current_fullname );
    }
    
    plucker_controller::get()->set_is_new_update_execution_allowed( TRUE );
#endif  // setupUSE_DRAG_AND_DROP 
}


void main_listctrl::OnSize( wxSizeEvent &event )
{
    set_column_widths();
    event.Skip();
}


void main_listctrl::set_column_widths()
{
    wxLogDebug( wxT( "About to set main_listctrl column widths" ) );
    
    // Get width of entire listctrl
    wxLogDebug( wxT( "About to get main_listctrl name column width" ) );
    int name_column_width = GetSize().x;     
    wxLogDebug( wxT( "Got main_listctrl name column width" ) );
     
    // Subtract width of other columns, scrollbar, and some padding     
    name_column_width -= GetColumnWidth( DUE_COLUMN );
    name_column_width -= wxSystemSettings::GetMetric( wxSYS_VSCROLL_X );     
    name_column_width -= plkrLISTCTRL_EXTRA_WIDTH_PADDING;
     
    // Set the column width to the new value.
    SetColumnWidth( NAME_COLUMN, name_column_width );  
    
    wxLogDebug( wxT( "Successfully set column widths" ) );
}


bool main_listctrl::ensure_destinations_provided( const wxArrayString& channel_sections )
{
    // Note: DO NOT put a set_is_is_new_update_execution_allowed() at start and end of 
    // this function, since it is called from within the gui_update codes, and the 
    // set to TRUE at the end of this function would thus set flag to TRUE before 
    // the gui_update_* function had completed.

    bool            destination_was_modified = FALSE;
    
#if ( setupUSE_CHECKBOX_MESSAGE_DIALOG ) 

    bool            default_checked = TRUE;
    wxString        show_message_path_key = wxT( "/PLUCKER_DESKTOP/message_dialog_enabled_no_destinations_selected_update" );
    size_t          number_of_sections           = channel_sections.GetCount();
    size_t          current_section_array_index;    
    wxString        current_section_string;
    wxString        dialog_message_string;
    wxString        channel_name;
    
    if ( number_of_sections == 0 ) 
    {
        return FALSE;
    }

    // Do our loop of updating
    for ( current_section_array_index = 0;
          (int)current_section_array_index < (int)number_of_sections;
          current_section_array_index++ ) 
    {
        current_section_string = channel_sections.Item( current_section_array_index );

        if ( ( the_configuration->Read( wxT( "/" ) + current_section_string + wxT( "/" ) + wxT( "user" ), wxT( "" ) ) == wxT( "" ) ) &&
            ( the_configuration->Read( wxT( "/" ) + current_section_string + wxT( "/" ) + wxT( "copy_to_dir" ), wxT( "" ) ) == wxT( "" ) ) ) 
        {
            // Flick the switch to let know that at least one destination was added.
            destination_was_modified = TRUE;

            channel_name = the_configuration->Read( wxT( "/" ) + current_section_string + wxT( "/" ) +
                                                    wxT( "doc_name" ), plkrUNNAMED_CHANNEL_NAME );

            dialog_message_string << _( "No destinations are selected for channel \'" )
                << channel_name << _( "\'. Add some destinations now?" );

            // ...and if want to show the warnings....
            if ( the_configuration->Read( show_message_path_key, (long)default_checked ) == (long)default_checked ) 
            {
                checkbox_message_dialog a_checkbox_message_dialog( this,
                    dialog_message_string,
                    _( "No destinations selected" ), wxYES | wxNO|wxCENTRE|wxICON_QUESTION,
                    wxDefaultPosition, _( "Always perform this check when about to update channels" ),
                    show_message_path_key,
                    default_checked );
                // .. and they clicked the OK box in the warning...
                if ( a_checkbox_message_dialog.ShowModal() == wxID_YES ) 
                {
                    // Create a channel dialog, jumping straight to the destinations tab
                    channel_dialog a_channel_dialog( m_parent, current_section_string,
                                                     CHANNEL_NOTEBOOK_PAGE_INDEX_DESTINATION );

                    a_channel_dialog.ShowModal();

                }

            }

        }
        
    }

#endif  //setupUSE_CHECKBOX_MESSAGE_DIALOG

    return destination_was_modified;
}


void main_listctrl::reload_rows()
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
