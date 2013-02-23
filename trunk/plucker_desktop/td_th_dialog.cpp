//----------------------------------------------------------------------------------------
// Name:        td_th_dialog.cpp
// Purpose:     Insert a horizontal rule dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: td_th_dialog.cpp,v 1.5 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "td_th_dialog.h"
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

#include "td_th_dialog.h"

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

BEGIN_EVENT_TABLE( td_th_dialog, wxDialog )
    EVT_BUTTON( wxID_HELP_CONTEXT, td_th_dialog::on_help_button )
    EVT_BUTTON( wxID_OK, td_th_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

// Constructor
td_th_dialog::td_th_dialog( wxWindow* parent, long td_verusus_th_dialog_type )
{
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "td_th_dialog") );

    m_td_versus_th_dialog_type = td_verusus_th_dialog_type;

    XRCCTRL( *this, "td_th_dialog_alignment_choice", wxChoice )->SetSelection( 0 );
}


// Destructor
td_th_dialog::~td_th_dialog()
{
}


void td_th_dialog::transfer_to( wxString& starting_text, wxString& ending_text )
{
    starting_text = m_starting_text;
    ending_text = m_ending_text;
}

//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

// Override wxDialog's default behavior for clicking an OK button.
void td_th_dialog::OnOK( wxCommandEvent& event )
{
    wxString output_string;
    wxString buf;

    if ( m_td_versus_th_dialog_type == optionTD_TH_DIALOG_IS_TD ) 
    {
        output_string = wxT( "<td");
    } 
    else
    {
        output_string = wxT( "<th");
    }

    int alignment_value = XRCCTRL( *this, "td_th_dialog_alignment_choice", wxChoice )->GetSelection();
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
        case 4:
            alignment_string = wxT( "justify");
    }
    if ( alignment_value != 0 ) 
    {
        output_string += wxT( " align=\"") + alignment_string + wxT( "\"");
    }

    int colspan_spin_value = XRCCTRL( *this, "td_th_dialog_colspan_spinctrl", wxSpinCtrl )->GetValue();

    if ( colspan_spin_value != 1 )
    {
        buf.Printf( wxT( " colspan=\"%d" ), colspan_spin_value );
        output_string += buf;
        output_string += '\"';
    }

    int rowspan_spin_value = XRCCTRL( *this, "td_th_dialog_rowspan_spinctrl", wxSpinCtrl )->GetValue();

    if ( rowspan_spin_value != 1 )
    {
        buf.Printf( wxT( " rowspan=\"%d" ), rowspan_spin_value );
        output_string += buf;
        output_string += '\"';
    }

    output_string += wxT( ">");

    // Store the starting and ending strings as class members, ready to be transferred
    // by transfer_to(...) method.
    m_starting_text = output_string;
    
    if ( m_td_versus_th_dialog_type == optionTD_TH_DIALOG_IS_TD ) 
    {
        m_ending_text = wxT( "</td>");
    } 
    else 
    {
        m_ending_text = wxT( "</th>");
    }

    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or
    // wxWindow::TransferDataFromWindow.
    EndModal( wxID_OK );
}

void td_th_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_TD_TH_DIALOG );
#endif
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 
