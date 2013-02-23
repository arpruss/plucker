//----------------------------------------------------------------------------------------
// Name:        table_dialog.cpp
// Purpose:     Insert an table dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: table_dialog.cpp,v 1.11 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "table_dialog.h"
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

#include "table_dialog.h"

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

BEGIN_EVENT_TABLE( table_dialog, wxDialog )
    EVT_UPDATE_UI( XRCID( "table_dialog_border_checkbox" ), table_dialog::on_update_ui_border_checkbox )
    EVT_BUTTON( XRCID( "table_dialog_border_color_button" ), table_dialog::on_border_color_button )
    EVT_BUTTON( wxID_HELP_CONTEXT, table_dialog::on_help_button )
    EVT_BUTTON( wxID_OK, table_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor
table_dialog::table_dialog( wxWindow* parent )
{
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "table_dialog") );
}


// Destructor
table_dialog::~table_dialog()
{
}


void table_dialog::transfer_to( wxString& starting_text, wxString& ending_text )
{
    starting_text = m_starting_text;
    ending_text = m_ending_text;
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void table_dialog::on_update_ui_border_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "table_dialog_border_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "table_dialog_border_color_textctrl", wxTextCtrl )
            ->Enable( enabled );      
    XRCCTRL( *this, "table_dialog_border_color_button", wxButton )
            ->Enable( enabled );    
}

void table_dialog::on_border_color_button( wxCommandEvent &event )
{
    // [utils_controls.cpp]    
    utils_controls::textctrl_hex_color_selection( XRCCTRL( *this, "table_dialog_border_color_textctrl", wxTextCtrl ) );
}


// Override wxDialog's default behavior for clicking an OK button.
void table_dialog::OnOK( wxCommandEvent& event )
{
    wxString output_string;
   
    output_string += wxT( "<table");    
   
    bool use_border = XRCCTRL( *this, "table_dialog_border_checkbox", wxCheckBox )->GetValue();
    if ( use_border ) 
    {
        output_string += wxT( " border=\"1\"");
    } 
    else 
    {
        output_string += wxT( " border=\"0\"");
    }
    
    wxString border_color_string = XRCCTRL( *this, "table_dialog_border_color_textctrl", wxTextCtrl )->GetValue();
    if ( use_border && border_color_string != wxT( "") )
    {
        output_string += wxT( " bordercolor=\"") + border_color_string + wxT( "\"");
    }     
    
    output_string += wxT( ">\n");   

    int number_of_rows = XRCCTRL( *this, "table_dialog_rows_spinctrl", wxSpinCtrl )->GetValue();
    int number_of_columns = XRCCTRL( *this, "table_dialog_columns_spinctrl", wxSpinCtrl )->GetValue();
    
    // Note that the _T is requred, or else it won't work.
    for ( int i=0; i < number_of_rows; i++ ) 
    {
        output_string += wxT( "<tr>\n");
        for ( int n=0; n < number_of_columns; n++ )
        {
            output_string += wxT( "<td></td>\n");
        }
        output_string += wxT( "</tr>\n");
    }
    
    output_string += wxT( "</table>");
    
    // Store the starting and ending strings as class members, ready to be transferred
    // by transfer_to(...) method.
    m_starting_text = output_string;
    m_ending_text = wxT( "");    
    
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow.    
    EndModal( wxID_OK );
}


void table_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_TABLE_DIALOG );
#endif 
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 
