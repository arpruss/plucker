//----------------------------------------------------------------------------------------
// Name:        hhictrl.cpp
// Purpose:     A control to allow install to Palm handheld Usernames or
//              directories.
// Author:      Robert O'Connor
// RCS-ID:      $Id: hhictrl.cpp,v 1.21 2007/02/02 10:33:44 desrod Exp $
// Copyright:   Robert O'Connor
// Licence:     GPL
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "hhictrl.h"
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

#include "hhictrl.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"
#include "wx/listctrl.h"
#include "wx/dirdlg.h"                  // wxDirDialog

//----------------------------------------------------------------------------------------

#include "handheld_dest_dialog.h"

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

enum
{
    wxID_HANDHELDINSTALLCTRL_USERS_LISTCTRL             = wxID_HIGHEST + 1,
    wxID_HANDHELDINSTALLCTRL_USERS_ADD_BUTTON,
    wxID_HANDHELDINSTALLCTRL_USERS_REMOVE_BUTTON,
    wxID_HANDHELDINSTALLCTRL_DIRECTORIES_LISTCTRL,
    wxID_HANDHELDINSTALLCTRL_DIRECTORIES_ADD_BUTTON,
    wxID_HANDHELDINSTALLCTRL_DIRECTORIES_REMOVE_BUTTON
};

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxHandheldInstallCtrl, wxPanel )

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( wxHandheldInstallCtrl, wxPanel )
    EVT_UPDATE_UI( wxID_HANDHELDINSTALLCTRL_USERS_LISTCTRL, wxHandheldInstallCtrl::OnUpdateUiUsersListCtrl )
    EVT_BUTTON( wxID_HANDHELDINSTALLCTRL_USERS_ADD_BUTTON, wxHandheldInstallCtrl::OnUsersAddButton )
    EVT_BUTTON( wxID_HANDHELDINSTALLCTRL_USERS_REMOVE_BUTTON, wxHandheldInstallCtrl::OnRemoveButtons )

    EVT_UPDATE_UI( wxID_HANDHELDINSTALLCTRL_DIRECTORIES_LISTCTRL, wxHandheldInstallCtrl::OnUpdateUiDirectoriesListCtrl )
    EVT_BUTTON( wxID_HANDHELDINSTALLCTRL_DIRECTORIES_ADD_BUTTON, wxHandheldInstallCtrl::OnDirectoriesAddButton )
    EVT_BUTTON( wxID_HANDHELDINSTALLCTRL_DIRECTORIES_REMOVE_BUTTON, wxHandheldInstallCtrl::OnRemoveButtons )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

// Constructor
wxHandheldInstallCtrl::wxHandheldInstallCtrl(wxWindow *parent, wxWindowID id,
                          const wxString &users_message, const wxString &directories_message,
                          const wxPoint& pos, const wxSize& size, const wxString& name  )
   : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL, name)
{
    m_usersListCtrlImageId = 0;
    m_directoriesListCtrlImageId = 0;

    // Create a new instance of palm_installer to get data for the different controls
    m_palmInstaller = new palm_installer;

    // See constructor of winflip.cpp for comments
    wxFlexGridSizer *mainFlexGridSizer = new wxFlexGridSizer( 2, 3, 0 );

    mainFlexGridSizer->AddGrowableCol( 0 );
    mainFlexGridSizer->AddGrowableRow( 1 );
    mainFlexGridSizer->AddGrowableRow( 3 );

    wxStaticText *usersStaticText = new wxStaticText( this, -1,
                                         users_message,
                                         wxDefaultPosition, wxDefaultSize, 0 );

    mainFlexGridSizer->Add( usersStaticText, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    mainFlexGridSizer->Add( 1, 1, 0, wxALIGN_CENTRE, 25 );

    m_usersListCtrl = new pda_listctrl( this, wxID_HANDHELDINSTALLCTRL_USERS_LISTCTRL,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxLC_REPORT|wxSUNKEN_BORDER,
                                          wxDefaultValidator, wxT( "users_listctrl" ),
                                          _( "Handheld destination" ),
                                          optionIS_USERS_LISTCTRL );

    mainFlexGridSizer->Add( m_usersListCtrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );

    wxFlexGridSizer *usersFlexGridSizer = new wxFlexGridSizer( 1, 0, 0 );
    usersFlexGridSizer->AddGrowableCol( 0 );

    m_usersAddButton = new wxButton( this, wxID_HANDHELDINSTALLCTRL_USERS_ADD_BUTTON,
                                     _( "Add handheld..." ),
                                     wxDefaultPosition, wxDefaultSize, 0 );

    usersFlexGridSizer->Add( m_usersAddButton, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_usersRemoveButton = new wxButton( this, wxID_HANDHELDINSTALLCTRL_USERS_REMOVE_BUTTON,
                                        _( "Remove selected handhelds" ),
                                        wxDefaultPosition, wxDefaultSize, 0 );

    usersFlexGridSizer->Add( m_usersRemoveButton, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    mainFlexGridSizer->Add( usersFlexGridSizer, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    wxStaticText *directoriesStaticText = new wxStaticText( this, -1,
                                          directories_message,
                                          wxDefaultPosition, wxDefaultSize, 0 );


    mainFlexGridSizer->Add( directoriesStaticText, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    mainFlexGridSizer->Add( 1, 1, 0, wxALIGN_CENTRE, 5 );

    m_directoriesListCtrl = new pda_listctrl( this, wxID_HANDHELDINSTALLCTRL_DIRECTORIES_LISTCTRL,
                                                wxDefaultPosition, wxDefaultSize,
                                                wxLC_REPORT|wxSUNKEN_BORDER,
                                                wxDefaultValidator, wxT( "directories_listctrl" ),
                                                _( "Directory destination" ),
                                                optionIS_DIRECTORIES_LISTCTRL );

    mainFlexGridSizer->Add( m_directoriesListCtrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );

    wxFlexGridSizer *directoryFlexGridSizer = new wxFlexGridSizer( 1, 0, 0 );

    directoryFlexGridSizer->AddGrowableCol( 0 );

    m_directoriesAddButton = new wxButton( this, wxID_HANDHELDINSTALLCTRL_DIRECTORIES_ADD_BUTTON,
                                           _( "Add output directory..." ),
                                           wxDefaultPosition, wxDefaultSize, 0 );

    directoryFlexGridSizer->Add( m_directoriesAddButton, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_directoriesRemoveButton = new wxButton( this, wxID_HANDHELDINSTALLCTRL_DIRECTORIES_REMOVE_BUTTON,
                                              _( "Remove selected directories" ),
                                              wxDefaultPosition, wxDefaultSize, 0 );

    directoryFlexGridSizer->Add( m_directoriesRemoveButton, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    mainFlexGridSizer->Add( directoryFlexGridSizer, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    SetAutoLayout( TRUE );
    SetSizer( mainFlexGridSizer );
    mainFlexGridSizer->Fit( this );
    mainFlexGridSizer->SetSizeHints( this );

    // TODO: Don't know whether this Layout() is needed
	Layout();
}

// Destructor
wxHandheldInstallCtrl::~wxHandheldInstallCtrl()
{
    if ( m_palmInstaller ) {
        delete m_palmInstaller;
        m_palmInstaller = NULL;
    }
    wxLogDebug( wxT( "Destructed wxHandheldInstallCtrl" ) );
}


void wxHandheldInstallCtrl::InsertUserDestinations( handheld_dest_array_type& handheld_dest_array )
{
    wxLogDebug( wxT( "Entering wxHandheldInstallCtrl::InsertUserDestinations()" ) );
    if ( handheld_dest_array.IsEmpty() ) {
        return;
    }

    // Insert by rows, starting at index zero. Achieve by getting number
    // of items in listctrl.
    int             row_to_insert;
    wxString        ram_or_card_string;
    wxString		is_usb_pause_string;
    wxArrayString   row_cells_contents;
    wxArrayInt      matching_row_numbers;
    // Insert them
    for ( size_t n = 0; n < handheld_dest_array.GetCount(); n++ ) 
    {
        row_to_insert =  m_usersListCtrl->GetItemCount();
            // Convert our handheld_target_storage_mode to the string in the column
            if( handheld_dest_array[ n ]->handheld_target_storage_mode == plkrHANDHELD_TARGET_STORAGE_MODE_SD_CARD )
            {
                ram_or_card_string = _( "SD Card" );
            } 
            else if( handheld_dest_array[ n ]->handheld_target_storage_mode == plkrHANDHELD_TARGET_STORAGE_MODE_MEMORY_STICK ) 
            {
                ram_or_card_string = _( "M.Stick" );
            } 
            else if( handheld_dest_array[ n ]->handheld_target_storage_mode == plkrHANDHELD_TARGET_STORAGE_MODE_COMPACT_FLASH )
            {
                ram_or_card_string = _( "CF Card" );
            } 
            else 
            {
                // All others are RAM.
                ram_or_card_string = _( "RAM" );
            }
            // Convert our is_usb_pause to the string in the column
            if( handheld_dest_array[ n ]->is_usb_pause == 1 )
            {
                is_usb_pause_string = _( "Yes" );
            } 
            else 
            {
                is_usb_pause_string = _( "No" ); 
            }
        // Only insert it if that user/RAM combo doesn't doesn't already exists in the
        // Empty matches from last iteration
        matching_row_numbers.Empty();
        row_cells_contents.Empty();
        
        // Make the row cell contents to look for:
        // NOTE: these must be added in the order of the columns!
        row_cells_contents.Add( handheld_dest_array[ n ]->user_name );
        row_cells_contents.Add( ram_or_card_string );
        // The card directory column goes in here
        row_cells_contents.Add( is_usb_pause_string );
        
        // Look to see if there is matching rows...
        m_usersListCtrl->find_matching_rows( row_cells_contents, &matching_row_numbers );
        // ...and if there isn't any matches...
        if ( matching_row_numbers.IsEmpty() ) 
        {
            // ..then insert row:
            m_usersListCtrl->InsertItem( row_to_insert, handheld_dest_array[ n ]->user_name, m_usersListCtrlImageId );
            m_usersListCtrl->SetItem( row_to_insert, RAM_OR_CARD_COLUMN, ram_or_card_string );
            // The 3rd column, DIRECTORY_ON_CARD_COLUMN, would go here.
            m_usersListCtrl->SetItem( row_to_insert, IS_USB_PAUSE_COLUMN, is_usb_pause_string );
        }
    }
    // Clear arrays to free memory
    matching_row_numbers.Clear();
    row_cells_contents.Clear();
    wxLogDebug( wxT( "Finished wxHandheldInstallCtrl::InsertUserDestinations()" ) );
}


void wxHandheldInstallCtrl::GetUserDestinations( handheld_dest_array_type& handheld_dest_array )
{
    wxLogDebug( wxT( "Entering wxHandheldInstallCtrl::GetUserDestinations() " ) );

    long selected_row_number = -1; // -1 includes first row

    for ( ;; ) 
    {
        // Default flags of all directions, and don't care the state.
        selected_row_number = m_usersListCtrl->GetNextItem( selected_row_number );

        // No more rows that fit the flag of being in a selected state, so break loop.
        if ( selected_row_number == -1 ) 
        {
            wxLogDebug( wxT( "No more items fitting flags so breaking GetUserDestinations()" ) );
            break;
        }

        // Add a new member to our handheld_dest_array.
        // The "new" we create here, gets "delete"ed wherever we called this from.
        // with a WX_CLEAR_ARRAY macro.
        handheld_dest_type* handheld_dest    = new handheld_dest_type();

        handheld_dest->user_name = m_usersListCtrl->get_cell_contents_string( selected_row_number, USER_NAME_COLUMN );
        if ( m_usersListCtrl->get_cell_contents_string( selected_row_number, RAM_OR_CARD_COLUMN ) == _( "RAM" ) ) 
        {
            handheld_dest->handheld_target_storage_mode = plkrHANDHELD_TARGET_STORAGE_MODE_RAM;
        } 
        else if ( m_usersListCtrl->get_cell_contents_string( selected_row_number, RAM_OR_CARD_COLUMN ) == _( "SD Card" ) )
        {
            handheld_dest->handheld_target_storage_mode = plkrHANDHELD_TARGET_STORAGE_MODE_SD_CARD;
        } 
        else if ( m_usersListCtrl->get_cell_contents_string( selected_row_number, RAM_OR_CARD_COLUMN ) == _( "CF Card" ) ) 
        {
            handheld_dest->handheld_target_storage_mode = plkrHANDHELD_TARGET_STORAGE_MODE_COMPACT_FLASH;
        } 
        else 
        {
            // Memory Stick
            handheld_dest->handheld_target_storage_mode = plkrHANDHELD_TARGET_STORAGE_MODE_MEMORY_STICK;
        }
        // row_directory_on_card would be similar

        if ( m_usersListCtrl->get_cell_contents_string( selected_row_number, IS_USB_PAUSE_COLUMN ) == _( "Yes" ) ) 
        {
            handheld_dest->is_usb_pause = 1;
        } 
        else 
        {
            handheld_dest->is_usb_pause = 0;
        }

        handheld_dest_array.Add( handheld_dest );

        wxLogDebug( wxT( "Added" ) + handheld_dest->user_name + wxT( "to GetusersDestination() result" ) );
    }
    
    wxLogDebug( wxT( "Completed wxHandheldInstallCtrl::GetUserDestinations() " ) );

}


void wxHandheldInstallCtrl::InsertDirectoryDestinations( wxArrayString& directories )
{
    if ( directories.IsEmpty() ) 
    {
        return;
    }
    // Insert by rows, starting at index zero. Achieve by getting number
    // of items in listctrl.
    int rowToInsert;
    
    // Insert them
    for ( size_t n = 0; n < directories.GetCount(); n++ ) 
    {
        rowToInsert =  m_directoriesListCtrl->GetItemCount();
        // Only insert it if the directoryname already exists in the list 
        // -1 starts from start, looks for string, and returns -1 if not found
        if ( m_directoriesListCtrl->FindItem(-1, directories[n]) == -1 )
        {
            m_directoriesListCtrl->InsertItem(rowToInsert, directories[n], m_directoriesListCtrlImageId);
        }
    }
}

void wxHandheldInstallCtrl::GetDirectoryDestinations( wxArrayString& directoryDestinations )
{      
    long            selectedRowNumber       = -1; // -1 includes first row
    wxListItem      rowInfo;

    for ( ;; )
    {
        // Default flags of all directions, and don't care the state.
        selectedRowNumber = m_directoriesListCtrl->GetNextItem( selectedRowNumber );
                                                                
        // No more rows that fit the flag of being in a selected state, so break loop.
        if ( selectedRowNumber == -1 ) 
        {
            break; 
        }    
        rowInfo.m_itemId = selectedRowNumber;
        rowInfo.m_col = 0;
        rowInfo.m_mask = wxLIST_MASK_TEXT;
        m_directoriesListCtrl->GetItem( rowInfo );
        directoryDestinations.Add(rowInfo.m_text);                    
    }
}


bool wxHandheldInstallCtrl::InstallFilesToDestinations( wxArrayString& install_fullnames )
{
    if ( install_fullnames.IsEmpty() )
    {
        return false;
    }

    // We will clear this array later with WX_CLEAR_ARRAY macro
    handheld_dest_array_type handheld_dest_array;    
    wxArrayString directory_destinations;

    GetUserDestinations( handheld_dest_array );
    GetDirectoryDestinations( directory_destinations );    

    for ( int n=0; n < (int)handheld_dest_array.GetCount(); n++ )
    {
        wxLogDebug( wxT( "user_array index is %d" ), n );          
        m_palmInstaller->install_files_by_handheld_dest( *handheld_dest_array.Item( n ),
                                                         install_fullnames );
    }

    for ( int i=0; i < (int)directory_destinations.GetCount(); i++ ) 
    {
        wxLogDebug( wxT( "directory_array index is %d" ), i );
        m_palmInstaller->install_files_by_directory( directory_destinations.Item( i ),
                                                     install_fullnames );
    }

    // The WX_CLEAR_ARRAY(array) macro [see dynarray.h] is required to clear out all the 
    // elements from memory of an array of structs that was built with WX_DEFINE_ARRAY.
    // All that this macro does is just to loop through the array, "delete" each element
    // to free it from memory, then mark the array as Empty().
    WX_CLEAR_ARRAY( handheld_dest_array );
    directory_destinations.Clear();
  
    return true;
}


bool wxHandheldInstallCtrl::IsAllDestinationsEmpty()
{
    wxLogDebug( wxT( "Entering wxHandheldInstallCtrl::IsAllDestinationsEmpty() " ) );

    bool                        is_empty = FALSE;
    handheld_dest_array_type    handheld_dest_array;
    wxArrayString               sync_directories_array;

    GetUserDestinations( handheld_dest_array );
    GetDirectoryDestinations( sync_directories_array );

    if ( handheld_dest_array.IsEmpty() && sync_directories_array.IsEmpty() ) 
    {
        wxLogDebug( wxT( "Both hhictrl array's returning empty." ) );
        is_empty = TRUE;
    }

    //Free memory
    sync_directories_array.Clear();
    // The WX_CLEAR_ARRAY(array) macro [see dynarray.h] is required to clear out all the
    // elements from memory of an array of structs that was built with WX_DEFINE_ARRAY.
    // All that this macro does is just to loop through the array, "delete" each element
    // to free it from memory, then mark the array as Empty().
    WX_CLEAR_ARRAY( handheld_dest_array );

    wxLogDebug( wxT( "Completing wxHandheldInstallCtrl::IsAllDestinationsEmpty()" ) );

    return is_empty;
}


//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

//--------Common to users and directories controls----------------------------------------

void wxHandheldInstallCtrl::SetImageList(wxImageList* imageList)
{
    m_usersListCtrl->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    m_directoriesListCtrl->SetImageList(imageList, wxIMAGE_LIST_SMALL);
}

void wxHandheldInstallCtrl::OnRemoveButtons(wxCommandEvent &event)
{
    // Recycle this function for both listctrls, depending on which button was hit
    wxListCtrl* listCtrl;    
    if ( event.GetId() == wxID_HANDHELDINSTALLCTRL_USERS_REMOVE_BUTTON )
    {
        listCtrl = m_usersListCtrl;
    } 
    else 
    {
        listCtrl = m_directoriesListCtrl;
    }

    long  selectedRowNumber = -1;  
    wxArrayInt selectedRowNumbersArray;
        
    for ( ;; ) 
    {
        // for( ;; ) with this next line is the recommended way for iterating selected rows.
        // selectedRowNumber was initialized at -1 to allow inclusion of first selected
        // row.
        selectedRowNumber = listCtrl->GetNextItem( selectedRowNumber, 
                                                   wxLIST_NEXT_ALL,
                                                   wxLIST_STATE_SELECTED );
                                                                
        // No more rows that fit the flag of being in a selected state, so break loop.
        if ( selectedRowNumber == -1 ) 
        {
            break; 
        }    
        // Assemble a list of indexes so we can loop though them backwards to delete them
        selectedRowNumbersArray.Add(selectedRowNumber);                    
    }
    
    // Loop through the row numbers in reverse, since once remove one, the rest will
    // move up, but if remove them bottom to top, then this doesn't matter.
    if ( ! selectedRowNumbersArray.IsEmpty() )
    {
        for ( long i=(selectedRowNumbersArray.GetCount()-1); 0 <= i; i-- ) 
        {
            listCtrl->DeleteItem( selectedRowNumbersArray[i] );
        }
    }
    
    selectedRowNumbersArray.Clear();
}

//--------users controls------------------------------------------------------------------

void wxHandheldInstallCtrl::SetUsersImageListImageId(int imageId)
{
    m_usersListCtrlImageId = imageId;
}


void wxHandheldInstallCtrl::OnUpdateUiUsersListCtrl(wxUpdateUIEvent &event)
{
    //! \todo WTF? What is the IsEnabled() for?
    bool enable_remove_button = ( (bool)m_usersListCtrl->GetSelectedItemCount() &&
                                  m_usersListCtrl->IsEnabled() ); 
                      
    m_usersRemoveButton->Enable( enable_remove_button );   
}


void wxHandheldInstallCtrl::OnUsersAddButton( wxCommandEvent& event )
{
    handheld_dest_type          handheld_dest;
    handheld_dest_array_type    handheld_dest_array;
    wxArrayString               all_available_user_names;

    wxLogDebug( wxT( "Entering OnUsersAddButton() function" ) );
    m_palmInstaller->get_user_name_array( all_available_user_names );
    wxLogDebug( wxT( "Got user_nameArray" ) );

    // Show a handheld_dest_dialog...
    handheld_dest_dialog a_handheld_dest_dialog( this,
        _( "Select additional handheld to receive the output" ),
        all_available_user_names );

    // .. and they clicked the OK box...
    if ( a_handheld_dest_dialog.ShowModal() == wxID_OK ) 
    {
        a_handheld_dest_dialog.transfer_to( handheld_dest );
        // Insert the destinations:
        handheld_dest_array.Add( &handheld_dest );
        InsertUserDestinations( handheld_dest_array );
    }

    all_available_user_names.Clear();

    // NOTE: Do NOT have a WX_CLEAR_ARRAY(array) macro here, since in this
    // function the handheld_dest wasn't created with a "new". If we put a
    // WX_CLEAR_ARRAY() macro in, then we will delete the handheld_dest once with
    // that macro, but then try to delete it again, as the current function
    // finishes and handheld_dest goes out of scope. This will cause an
    // "Can't verify memory struct. All bets are off!" Assert and an
    // instability/crash.
    wxLogDebug( wxT( "Completed wxHandheldInstallCtrl::OnUsersAddButton()" ) );
}

//--------Directories controls------------------------------------------------------------

void wxHandheldInstallCtrl::SetDirectoriesImageListImageId(int imageId)
{
    m_directoriesListCtrlImageId = imageId;
}


void wxHandheldInstallCtrl::OnUpdateUiDirectoriesListCtrl(wxUpdateUIEvent &event)
{
    bool enable_remove_button = ( (bool)m_directoriesListCtrl->GetSelectedItemCount() &&
                                  m_directoriesListCtrl->IsEnabled() ); 
                      
    m_directoriesRemoveButton->Enable(enable_remove_button); 
}


void wxHandheldInstallCtrl::OnDirectoriesAddButton( wxCommandEvent& event )
{
    wxArrayString   selectedDirectoryName;
    
    wxDirDialog directoryDlg( this, 
                              _( "Select directory in which to place the output" ),
                              wxT( "" ) );
  
    if ( directoryDlg.ShowModal() == wxID_OK )
    {
        selectedDirectoryName.Add(directoryDlg.GetPath()); 
        InsertDirectoryDestinations(selectedDirectoryName);
    }     
    
    selectedDirectoryName.Clear();
}
