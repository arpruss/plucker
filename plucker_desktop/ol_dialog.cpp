//----------------------------------------------------------------------------------------
// Name:        ol_dialog.cpp
// Purpose:     Insert an ordered list (ol) dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/01/02
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: ol_dialog.cpp,v 1.12 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "ol_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTEGRATED_HTML_EDITOR )

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

#include "ol_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/spinctrl.h"            // wxSpinCtrl

// ---------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

// ---------------------------------------------------------------------------------------

#include "utils_controls.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( ol_dialog, wxDialog )
    EVT_UPDATE_UI( XRCID( "ol_dialog_specify_start_checkbox" ), ol_dialog::on_update_ui_specify_start_checkbox )
    EVT_BUTTON( wxID_HELP_CONTEXT, ol_dialog::on_help_button )
    EVT_BUTTON( wxID_OK, ol_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor
ol_dialog::ol_dialog( wxWindow* parent )
{
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "ol_dialog") );
    XRCCTRL( *this, "ol_dialog_enumeration_choice", wxChoice )->SetSelection( 0 );
    
}


// Destructor
ol_dialog::~ol_dialog()
{
}


void ol_dialog::transfer_to( wxString& starting_text, wxString& ending_text )
{
    starting_text = m_starting_text;
    ending_text = m_ending_text;
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void ol_dialog::on_update_ui_specify_start_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "ol_dialog_specify_start_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "ol_dialog_start_at_spinctrl", wxSpinCtrl )
            ->Enable( enabled );    
}


// Override wxDialog's default behavior for clicking an OK button.
void ol_dialog::OnOK( wxCommandEvent& event )
{
    wxString output_string;
    wxString buf;
    
    output_string = wxT( "<ol");    

    bool specify_start = XRCCTRL( *this, "ol_dialog_specify_start_checkbox", wxCheckBox )->GetValue(); 

    if ( specify_start ) 
    {
        int start_spin_value = XRCCTRL( *this, "ol_dialog_start_at_spinctrl", wxSpinCtrl )->GetValue(); 
        buf.Printf( wxT( " start=\"%d" ), start_spin_value );
        output_string += buf;   
        output_string += '\"';
    }
        
    int enumeration_value = XRCCTRL( *this, "ol_dialog_enumeration_choice", wxChoice )->GetSelection();
    wxString enumeration_string; 
    // Case 0 is 'Default'
    switch ( enumeration_value ) 
    {
        case 1:
            enumeration_string = wxT( "A");            
            break;
        case 2:
            enumeration_string = wxT( "a");            
            break;
        case 3:
            enumeration_string = wxT( "I");            
            break;
        case 4:
            enumeration_string = wxT( "i");            
            break;
    } 
    if ( enumeration_value != 0 )
    {
        output_string += wxT( " type=\"") + enumeration_string + wxT( "\"");
    }
    
    bool compact = XRCCTRL( *this, "ol_dialog_compact_checkbox", wxCheckBox )->GetValue(); 
    if ( compact )
    {
        output_string += wxT( " compact");
    }
    
    output_string += wxT( ">\n");
    
    // Store the starting and ending strings as class members, ready to be transferred
    // by transfer_to(...) method.
    m_starting_text = output_string;
    m_ending_text = wxT( "</ol>");    
    
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow.    
    EndModal( wxID_OK );
}


void ol_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_OL_DIALOG );
#endif 
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 
