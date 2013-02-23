//----------------------------------------------------------------------------------------
// Name:        destinations_wizardpage.cpp
// Purpose:     A wizardpage of destinations to install output to
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: destinations_wizardpage.cpp,v 1.11 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "destinations_wizardpage.h"
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

#include "destinations_wizardpage.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

// ---------------------------------------------------------------------------------------

#include "configuration.h"
#include "image_list.h"
#include "utils_string.h"
#include "handheld_dest_wrappers.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( destinations_wizardpage, wxWizardPageSimple )
    EVT_WIZARD_PAGE_CHANGING( -1, destinations_wizardpage::on_wizardpage_changing )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

destinations_wizardpage::destinations_wizardpage
    ( 
    wxWizard *parent,
    const wxString& handheld_destinations_label, 
    const wxString& directory_destinations_label,
    const wxString& channel_section,
    wxBitmap bitmap
    )
    : wxWizardPageSimple( parent )
{
    wxLogDebug( wxT( "Entering destinations_wizardpage constructor") );
  
    // To set a custom bitmap in a derived wxWizardPageSimple, you just have to 
    // set the wxWizardPageSimple m_bitmap member in the derived constructor.
    // ( see wx examples ). .
    m_bitmap = bitmap;
    
    // Create the handheldlistctrl    
    m_handheld_install_ctrl = new wxHandheldInstallCtrl( this, -1,
                                            handheld_destinations_label,
                                            directory_destinations_label);                                                            
    wxLogDebug( wxT( "Created a new wxHandheldInstallCtrl") );
    
    // Make a new flexgrid sizer ( format # of rows, # of columns, then hor/vert gaps)
    wxFlexGridSizer *root_flexgridsizer = new wxFlexGridSizer( 1, 0, 0 );
    // Set the growable columns and rows
    root_flexgridsizer->AddGrowableCol( 0 );
    root_flexgridsizer->AddGrowableRow( 0 );

    // Add handheldctrl to sizer.
    root_flexgridsizer->Add( m_handheld_install_ctrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0 );

    // Now that everything is added into the flexgrid sizer, send some commands to 
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_flexgridsizer );
    root_flexgridsizer->Fit( this );
    root_flexgridsizer->SetSizeHints( this );     
  
    // Set up the listctrl icons for the wxHandheldInstallerCtrl
    m_handheld_install_ctrl->SetImageList( image_list::get() );
    m_handheld_install_ctrl->SetUsersImageListImageId( plkrSMALL_IMAGE_LIST_ID_CHANNEL_DIALOG_DESTINATION_TAB_USERS_LISTCTRL );
    m_handheld_install_ctrl->SetDirectoriesImageListImageId( plkrSMALL_IMAGE_LIST_ID_CHANNEL_DIALOG_DESTINATION_TAB_DIRECTORIES_LISTCTRL );
    
    
    // Initialize the values for the control to our passed section:
    handheld_dest_array_type handheld_dest_array;
    handheld_dest_wrappers::read_handheld_dest_array_from_configuration( channel_section,
                                                                       &handheld_dest_array );
    m_handheld_install_ctrl->InsertUserDestinations( handheld_dest_array );

    wxArrayString sync_directories_array;
    wxString sync_directories_string = the_configuration->Read( wxT( "/") + channel_section +
                                                                wxT( "/copy_to_dir"), wxT( "") );
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
}
                                    

destinations_wizardpage::~destinations_wizardpage()
{
}


void destinations_wizardpage::get_user_destinations( handheld_dest_array_type& handheld_dest_array )
{
    m_handheld_install_ctrl->GetUserDestinations( handheld_dest_array );
}
    
    
void destinations_wizardpage::get_directory_destinations( wxArrayString& directory_destinations )
{
    m_handheld_install_ctrl->GetDirectoryDestinations( directory_destinations ); 
}

//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

void destinations_wizardpage::on_wizardpage_changing( wxWizardEvent& event )
{
    // If the event was a "Next>" button event not a "<Prev" button event...
    // (wxWizardEvent::GetDirection for EVT_WIZARD_PAGE_CHANGING returns TRUE if we're
    // going forward or FALSE otherwise and for EVT_WIZARD_PAGE_CHANGED returns TRUE if we
    // came from the previous page and FALSE if we returned from the next one.
    if ( event.GetDirection() == TRUE ) 
    { 
        // ..and if the handheldinstallctrl is empty...
        if ( m_handheld_install_ctrl->IsAllDestinationsEmpty() )
        {
            wxMessageDialog need_destination_messagedialog ( this,
              _( "At least one destination is required. Click 'Add handheld..' or .'Add output directory...'." ),
              _( "Destination required" ),
              wxOK | wxICON_INFORMATION );   
            // ...then show the message dialog...
            need_destination_messagedialog.ShowModal();
            // .. and veto the wizardpagechanging event from moving off this wizardpage.
            event.Veto();
        // ...otherwise, if not empty, just continue normally.
        }
        else
        {
            return;
        }    
        
    // ..otherwise it was a "<Prev" button event, which we don't care about.    
    } else
    {
        return;
    }
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
