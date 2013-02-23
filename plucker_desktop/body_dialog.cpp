//----------------------------------------------------------------------------------------
// Name:        body_dialog.cpp
// Purpose:     Insert a body dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: body_dialog.cpp,v 1.11 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "body_dialog.h"
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

#include "body_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

//----------------------------------------------------------------------------------------

#include "utils_controls.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( body_dialog, wxDialog )
    EVT_BUTTON( XRCID( "body_dialog_text_color_button" ), body_dialog::on_text_color_button )
    EVT_BUTTON( XRCID( "body_dialog_link_color_button" ), body_dialog::on_link_color_button )
    EVT_BUTTON( wxID_HELP_CONTEXT, body_dialog::on_help_button )
    EVT_BUTTON( wxID_OK, body_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

body_dialog::body_dialog( wxWindow* parent )
{
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "body_dialog") );
}


body_dialog::~body_dialog()
{
}


void body_dialog::transfer_to( wxString& starting_text, wxString& ending_text )
{
    starting_text = m_starting_text;
    ending_text = m_ending_text;
}

//----------------------------------------------------------------------------------------
// Private members
//----------------------------------------------------------------------------------------

void body_dialog::on_text_color_button( wxCommandEvent &event )
{
    // [utils_controls.cpp]    
    utils_controls::textctrl_hex_color_selection( XRCCTRL( *this, "body_dialog_text_color_textctrl", wxTextCtrl ) );
}


void body_dialog::on_link_color_button( wxCommandEvent &event )
{
    // [utils_controls.cpp]    
    utils_controls::textctrl_hex_color_selection( XRCCTRL( *this, "body_dialog_link_color_textctrl", wxTextCtrl ) );
}


void body_dialog::OnOK( wxCommandEvent& event )
{
    wxString output_string;
   
    output_string += wxT( "<body");

    wxString text_color_string = XRCCTRL( *this, "body_dialog_text_color_textctrl", wxTextCtrl )->GetValue();
    if ( text_color_string != wxT( "") )
    {
        output_string += wxT( " text=\"") + text_color_string + wxT( "\"");
    }     
    
    wxString link_color_string = XRCCTRL( *this, "body_dialog_link_color_textctrl", wxTextCtrl )->GetValue();
    if ( link_color_string != wxT( "") ) 
    {
        output_string += wxT( " link=\"") + link_color_string + wxT( "\"");
    }     
    
    output_string += wxT( ">\n");
    
    // Store the starting and ending strings as class members, ready to be transferred
    // by transfer_to(...) method.
    m_starting_text = output_string;
    m_ending_text = wxT( "</body>");
            
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow.    
    EndModal( wxID_OK );
}


void body_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_BODY_DIALOG );
#endif     
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 
