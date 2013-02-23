//----------------------------------------------------------------------------------------
// Name:        ol_dialog.cpp
// Purpose:     Insert an ordered list (ol) dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/01/02
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: ul_dialog.cpp,v 1.10 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "ul_dialog.h"
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

#include "ul_dialog.h"

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

BEGIN_EVENT_TABLE( ul_dialog, wxDialog )
    EVT_BUTTON( wxID_HELP_CONTEXT, ul_dialog::on_help_button )
    EVT_BUTTON( wxID_OK, ul_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor
ul_dialog::ul_dialog( wxWindow* parent )
{
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "ul_dialog") );
    
    XRCCTRL( *this, "ul_dialog_bullet_choice", wxChoice )->SetSelection( 0 );
}


// Destructor
ul_dialog::~ul_dialog()
{
}


void ul_dialog::transfer_to( wxString& starting_text, wxString& ending_text )
{
    starting_text = m_starting_text;
    ending_text = m_ending_text;
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

// Override wxDialog's default behavior for clicking an OK button.
void ul_dialog::OnOK( wxCommandEvent& event )
{
    wxString output_string;

    output_string = wxT( "<ul");    
        
    int bullet_value = XRCCTRL( *this, "ul_dialog_bullet_choice", wxChoice )->GetSelection();
    wxString bullet_string; 
    // Case 0 is 'Default'
    switch ( bullet_value ) 
    {
        case 1:
            bullet_string = wxT( "square");            
            break;
        case 2:
            bullet_string = wxT( "circle");            
            break;
    } 
    if ( bullet_value != 0 )
    {
        output_string += wxT( " type=\"") + bullet_string + wxT( "\"");
    }
    
    bool compact = XRCCTRL( *this, "ul_dialog_compact_checkbox", wxCheckBox )->GetValue(); 
    if ( compact ) 
    {
        output_string += wxT( " compact");
    }
    
    output_string += wxT( ">\n");
    
       // Store the starting and ending strings as class members, ready to be transferred
    // by transfer_to(...) method.
    m_starting_text = output_string;
    m_ending_text = wxT( "</ul>");
        
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow.    
    EndModal( wxID_OK );
}


void ul_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_UL_DIALOG );    
#endif 
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 
