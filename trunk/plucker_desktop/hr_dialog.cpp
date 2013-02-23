//----------------------------------------------------------------------------------------
// Name:        hr_dialog.cpp
// Purpose:     Insert a horizontal rule dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: hr_dialog.cpp,v 1.14 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "hr_dialog.h"
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

#include "hr_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/spinctrl.h"            // wxSpinCtrl

// ---------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

// ---------------------------------------------------------------------------------------

#include "plucker_defines.h"

#include "utils_controls.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( hr_dialog, wxDialog )
    EVT_BUTTON( XRCID( "hr_dialog_color_button" ), hr_dialog::on_color_button )
    EVT_BUTTON( wxID_HELP_CONTEXT, hr_dialog::on_help_button )
    EVT_BUTTON( wxID_OK, hr_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

// Constructor
hr_dialog::hr_dialog( wxWindow* parent )
{
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "hr_dialog") );
        
    XRCCTRL( *this, "hr_dialog_width_choice", wxChoice )->SetSelection( 0 );       
    XRCCTRL( *this, "hr_dialog_alignment_choice", wxChoice )->SetSelection( 0 ); 
}


// Destructor
hr_dialog::~hr_dialog()
{
}


void hr_dialog::transfer_to( wxString& starting_text, wxString& ending_text )
{
    starting_text = m_starting_text;
    ending_text = m_ending_text;
}

//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

void hr_dialog::on_color_button( wxCommandEvent &event )
{
    // [utils_controls.cpp]    
    utils_controls::textctrl_hex_color_selection( XRCCTRL( *this, "hr_dialog_color_textctrl", wxTextCtrl ) );
}


// Override wxDialog's default behavior for clicking an OK button.
void hr_dialog::OnOK( wxCommandEvent& event )
{
    wxString output_string;
    wxString buf;
    
    output_string = wxT( "<hr");    

    int width_spin_value = XRCCTRL( *this, "hr_dialog_width_spinctrl", wxSpinCtrl )->GetValue();    
    wxString trailing_sign = wxT( ""); 
    if ( XRCCTRL( *this, "hr_dialog_width_choice", wxChoice )->GetSelection() == 0 ) 
    {
#ifdef __VISUALC__
		trailing_sign = wxT( "%" ); 
#else
        trailing_sign = wxT( '\%'); 
#endif
    }    
    // Check to see if a 100%, and skip width, if so.
    if ( ! ( width_spin_value == 100 && trailing_sign == wxT( "%") ) ) 
    {
        buf.Printf( wxT( " width=\"%d" ), width_spin_value );
        output_string += buf;   
        output_string += trailing_sign;
        output_string += '\"';
    }

    XRCCTRL( *this, "hr_dialog_size_spinctrl", wxSpinCtrl )->GetValue();
    int size_spin_value = XRCCTRL( *this, "hr_dialog_size_spinctrl", wxSpinCtrl )->GetValue();    
    buf.Printf( wxT( " size=\"%d\"" ), size_spin_value );
    output_string += buf;    

    int alignment_value = XRCCTRL( *this, "hr_dialog_alignment_choice", wxChoice )->GetSelection();
    wxString alignment_string; 
    // Case 0 is 'Default'
    switch ( alignment_value ) 
    {
        case 1:
            alignment_string = wxT( "left");            
            break;
        case 2:
            alignment_string = wxT( "right");            
            break;
        case 3:
            alignment_string = wxT( "center");
            break;
    } 
    if ( alignment_value != 0 ) 
    {
        output_string += wxT( " align=\"") + alignment_string + wxT( "\"");
    }
    
    wxString color_string = XRCCTRL( *this, "hr_dialog_color_textctrl", wxTextCtrl )->GetValue();
    if ( color_string != wxT( "") ) 
    {
        output_string += wxT( " color=\"") + color_string + wxT( "\"");
    }    
   
    output_string += wxT( ">");
    
    // Store the starting and ending strings as class members, ready to be transferred
    // by transfer_to(...) method.
    m_starting_text = output_string;
    m_ending_text = wxT( "");
    
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow.    
    EndModal( wxID_OK );
}

void hr_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_HR_DIALOG );
#endif 
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 
