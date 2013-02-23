//----------------------------------------------------------------------------------------
// Name:        email_dialog.cpp
// Purpose:     Insert a email dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: email_dialog.cpp,v 1.10 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "email_dialog.h"
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

#include "email_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

// ---------------------------------------------------------------------------------------

#include "utils_controls.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( email_dialog, wxDialog )
    EVT_BUTTON( wxID_HELP_CONTEXT, email_dialog::on_help_button )
    EVT_BUTTON( wxID_OK, email_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

email_dialog::email_dialog( wxWindow* parent )
{
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "email_dialog") );
}


email_dialog::~email_dialog()
{
}


void email_dialog::transfer_to( wxString& starting_text, wxString& ending_text )
{
    starting_text = m_starting_text;
    ending_text = m_ending_text;
}

//----------------------------------------------------------------------------------------
// Private members: event handlers
//----------------------------------------------------------------------------------------

void email_dialog::OnOK( wxCommandEvent& event )
{
    wxString output_string;
    
    output_string = wxT( "<a");    

    wxString name_string = XRCCTRL( *this, "email_dialog_address_textctrl", wxTextCtrl )->GetValue();
    output_string += wxT( " href=\"mailto:") + name_string + wxT( "\">");
   
    wxString description_string = XRCCTRL( *this, "email_dialog_description_textctrl", wxTextCtrl )->GetValue();
    // If didn't type a description string, then use the email address as description
    if ( description_string == wxT( "") ) 
    {
        description_string = name_string;
    }
    output_string += description_string;
      
    output_string += wxT( "</a>");
    
    // Store the starting and ending strings as class members, ready to be transferred
    // by transfer_to(...) method.
    m_starting_text = output_string;
    m_ending_text = wxT( "");
    
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow.    
    EndModal( wxID_OK );
}


void email_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_EMAIL_DIALOG );
#endif 
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 
