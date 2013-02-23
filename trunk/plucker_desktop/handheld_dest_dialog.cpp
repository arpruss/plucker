//----------------------------------------------------------------------------------------
// Name:        handheld_dest_dialog.cpp
// Purpose:     A dialog for choosing a handheld to add to list of destinations.
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: handheld_dest_dialog.cpp,v 1.8 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "handheld_dest_dialog.h"
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

#include "handheld_dest_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "utils_controls.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Internal constants: control IDs
//----------------------------------------------------------------------------------------

enum {
    ID_USER_NAME_CHOICE                     = wxID_HIGHEST + 1,
    ID_IS_USB_PAUSE_CHECKBOX,
    ID_INSTALL_TO_RAM_RADIOBUTTON,
    ID_INSTALL_TO_SD_CARD_RADIOBUTTON,
    ID_INSTALL_TO_MEMORY_STICK_RADIOBUTTON,
    ID_INSTALL_TO_COMPACT_FLASH_RADIOBUTTON,
    ID_DEFAULT_CARD_DIRECTORY_RADIOBUTTON,
    ID_SPECIFIED_CARD_DIRECTORY_RADIOBUTTON,
    ID_DIRECTORY_ON_CARD_TEXTCTRL
};

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( handheld_dest_dialog, wxDialog )
    EVT_BUTTON( wxID_HELP_CONTEXT, handheld_dest_dialog::on_help_button )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

handheld_dest_dialog::handheld_dest_dialog( wxWindow* parent,
                                          const wxString& title,
                                          wxArrayString& user_name_array
                                          )
           : wxDialog( parent, -1, title, wxDefaultPosition, wxDefaultSize,
                      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{

    wxFlexGridSizer *root_flexgridsizer = new wxFlexGridSizer( 1, 0, 0 );
    root_flexgridsizer->AddGrowableCol( 0 );
    root_flexgridsizer->AddGrowableRow( 1 );

    wxStaticBox *user_name_staticbox = new wxStaticBox( this, -1, _( "Handheld" ) );
    wxStaticBoxSizer *user_name_staticboxsizer = new wxStaticBoxSizer( user_name_staticbox, wxVERTICAL );

    wxFlexGridSizer *user_name_flexgridsizer = new wxFlexGridSizer( 2, 0, 0 );
    user_name_flexgridsizer->AddGrowableCol( 1 );
    user_name_flexgridsizer->AddGrowableRow( 0 );

    wxStaticText *user_name_statictext = new wxStaticText( this, -1, _( "Handheld user:" ), wxDefaultPosition, wxDefaultSize, 0 );
    user_name_flexgridsizer->Add( user_name_statictext, 0, wxALIGN_CENTRE|wxALL, 5 );

    m_user_name_choice = new wxChoice( this, ID_USER_NAME_CHOICE, wxDefaultPosition, wxSize( 200,-1 ) );
    user_name_flexgridsizer->Add( m_user_name_choice, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    user_name_staticboxsizer->Add( user_name_flexgridsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    m_is_usb_pause_checkbox = new wxCheckBox( this, ID_IS_USB_PAUSE_CHECKBOX, _( "This is a USB-connected device" ) );
    user_name_staticboxsizer->Add( m_is_usb_pause_checkbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    root_flexgridsizer->Add( user_name_staticboxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *location_on_handheld_staticbox = new wxStaticBox( this, -1, _( "Location on handheld" ) );
    wxStaticBoxSizer *location_on_handheld_staticboxsizer = new wxStaticBoxSizer( location_on_handheld_staticbox, wxVERTICAL );

    wxStaticText *install_to_statictext = new wxStaticText( this, -1, _( "On the handheld, send the files to:" ), wxDefaultPosition, wxDefaultSize, 0 );
    location_on_handheld_staticboxsizer->Add( install_to_statictext, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxFlexGridSizer *ram_or_card_flexgridsizer = new wxFlexGridSizer( 2, 0, 5 );
    ram_or_card_flexgridsizer->AddGrowableCol( 2 );
    ram_or_card_flexgridsizer->AddGrowableRow( 0 );
    ram_or_card_flexgridsizer->AddGrowableRow( 1 );
    ram_or_card_flexgridsizer->AddGrowableRow( 2 );

    ram_or_card_flexgridsizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 0 );

    m_install_to_ram_radiobutton = new wxRadioButton( this, ID_INSTALL_TO_RAM_RADIOBUTTON, _( "The handheld's RAM memory" ), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    ram_or_card_flexgridsizer->Add( m_install_to_ram_radiobutton, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5 );

    ram_or_card_flexgridsizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 0 );

    m_install_to_sd_card_radiobutton = new wxRadioButton( this, ID_INSTALL_TO_SD_CARD_RADIOBUTTON, _( "The removable SD/MMC card" ), wxDefaultPosition, wxDefaultSize, 0 );
    ram_or_card_flexgridsizer->Add( m_install_to_sd_card_radiobutton, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5 );

    ram_or_card_flexgridsizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 0 );

    m_install_to_memory_stick_radiobutton = new wxRadioButton( this, ID_INSTALL_TO_MEMORY_STICK_RADIOBUTTON, _( "The removable Memory Stick"), wxDefaultPosition, wxDefaultSize, 0 );
    ram_or_card_flexgridsizer->Add( m_install_to_memory_stick_radiobutton , 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5 );

    ram_or_card_flexgridsizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 0 );

    m_install_to_compact_flash_radiobutton = new wxRadioButton( this, ID_INSTALL_TO_COMPACT_FLASH_RADIOBUTTON, _( "The removable CompactFlash card"), wxDefaultPosition, wxDefaultSize, 0 );
    ram_or_card_flexgridsizer->Add( m_install_to_compact_flash_radiobutton , 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5 );

    location_on_handheld_staticboxsizer->Add( ram_or_card_flexgridsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

    wxFlexGridSizer *card_directory_flexgridsizer = new wxFlexGridSizer( 3, 0, 5 );
    card_directory_flexgridsizer->AddGrowableCol( 1 );
    card_directory_flexgridsizer->AddGrowableRow( 0 );

    card_directory_flexgridsizer->Add( 30, 10, 0, wxALIGN_CENTRE|wxALL, 5 );

    m_default_card_directory_radiobutton = new wxRadioButton( this, ID_DEFAULT_CARD_DIRECTORY_RADIOBUTTON, _("Send to the default directory on the card"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    card_directory_flexgridsizer->Add( m_default_card_directory_radiobutton, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    card_directory_flexgridsizer->Add( 1, 1, 0, wxALIGN_CENTRE|wxALL, 5 );
    card_directory_flexgridsizer->Add( 30, 10, 0, wxALIGN_CENTRE|wxALL, 5 );

    m_specified_card_directory_radiobutton = new wxRadioButton( this, ID_SPECIFIED_CARD_DIRECTORY_RADIOBUTTON, _("Send to this specified directory on the card:"), wxDefaultPosition, wxDefaultSize, 0 );
    card_directory_flexgridsizer->Add( m_specified_card_directory_radiobutton, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );

    m_directory_on_card_textctrl = new wxTextCtrl( this, ID_DIRECTORY_ON_CARD_TEXTCTRL, wxT( "" ), wxDefaultPosition, wxSize(140,-1), 0 );
    card_directory_flexgridsizer->Add( m_directory_on_card_textctrl, 0, wxALIGN_CENTER_VERTICAL, 5 );

    location_on_handheld_staticboxsizer->Add( card_directory_flexgridsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    root_flexgridsizer->Add( location_on_handheld_staticboxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    wxBoxSizer *bottom_button_boxsizer = new wxBoxSizer( wxHORIZONTAL );

    wxButton *ok_button = new wxButton( this, wxID_OK, _( "OK" ) );
    bottom_button_boxsizer->Add( ok_button, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxButton *cancel_button= new wxButton( this, wxID_CANCEL, _( "Cancel" ) );
    bottom_button_boxsizer->Add( cancel_button, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *help_button = new wxButton( this, wxID_HELP_CONTEXT, _( "Help" ) );
    bottom_button_boxsizer->Add( help_button, 0, wxALIGN_CENTRE|wxALL, 5 );

    root_flexgridsizer->Add( bottom_button_boxsizer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0 );

    // Do the standard sizer fitting stuff.
    SetAutoLayout( TRUE );
    SetSizer( root_flexgridsizer );
    root_flexgridsizer->SetSizeHints( this );
    root_flexgridsizer->Fit( this );

    // Center the html tip dialog on its parent.
    Centre( wxBOTH | wxCENTER_FRAME );

    // Finally, load the array of possible user names into the choice
    for ( size_t n = 0; n < user_name_array.GetCount(); n++ ) 
    {
        m_user_name_choice->Append( user_name_array.Item( n ) );
    }
    // And set the selection to the first one
    if ( 0 < user_name_array.GetCount() ) 
    {
        m_user_name_choice->SetSelection( 0 );
    }
    
    // A radiobutton should alway have one value selected. Set RAM radiobutton to TRUE.
    m_install_to_ram_radiobutton->SetValue( TRUE );

    // Make the directory controls invisible, since not using yet.
    // Disabled doesn't really matter
    m_default_card_directory_radiobutton->Show( FALSE );
    m_specified_card_directory_radiobutton->Show( FALSE );
    m_directory_on_card_textctrl->Show( FALSE );
}


handheld_dest_dialog::~handheld_dest_dialog()
{
}


void handheld_dest_dialog::transfer_to( handheld_dest_type& handheld_dest  )
{
    handheld_dest.user_name          = m_user_name_choice->GetStringSelection();

    handheld_dest.is_usb_pause		 = m_is_usb_pause_checkbox->GetValue();

    // Mark the value of handheld_target_storage_mode according to the destination of 
    // RAM, SD or MemorySick
    if ( m_install_to_ram_radiobutton->GetValue() )
    {
        handheld_dest.handheld_target_storage_mode = plkrHANDHELD_TARGET_STORAGE_MODE_RAM;
    } 
    else if ( m_install_to_sd_card_radiobutton->GetValue() ) 
    {
        handheld_dest.handheld_target_storage_mode = plkrHANDHELD_TARGET_STORAGE_MODE_SD_CARD;
    } 
    else if (  m_install_to_memory_stick_radiobutton->GetValue() ) 
    {
        handheld_dest.handheld_target_storage_mode = plkrHANDHELD_TARGET_STORAGE_MODE_MEMORY_STICK;
    } 
    else if (  m_install_to_compact_flash_radiobutton->GetValue() ) 
    {
        handheld_dest.handheld_target_storage_mode = plkrHANDHELD_TARGET_STORAGE_MODE_COMPACT_FLASH;
    }
    

    // If selected custom dir, then use that, otherwise keep blank.
    // TODO, test the handheld_target_storage_mode value and only set if !=0.
    handheld_dest.directory_on_card  = m_directory_on_card_textctrl->GetValue();
}


//----------------------------------------------------------------------------------------
// Private members
//----------------------------------------------------------------------------------------

void handheld_dest_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_HANDHELD_DEST_DIALOG );
#endif 
}
