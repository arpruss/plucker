//----------------------------------------------------------------------------------------
// Name:        blocked_dialog.cpp
// Purpose:     Add/edit a blocked string for exclusion lists dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: blocked_dialog.cpp,v 1.18 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "blocked_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR  )

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

#include "blocked_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/spinctrl.h"            // wxSpinCtrl

//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

//----------------------------------------------------------------------------------------

#include "help_controller.h"

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( blocked_dialog, wxDialog )
    EVT_BUTTON( wxID_OK, blocked_dialog::OnOK )
    EVT_BUTTON( wxID_HELP_CONTEXT, blocked_dialog::on_help_button )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

blocked_dialog::blocked_dialog( wxWindow* parent, wxString exclusion_string, 
                                long& action, long& priority )
{
	wxXmlResource::Get()->LoadDialog( this, parent, wxT( "blocked_dialog") );

    // Set the initial values of the controls
    XRCCTRL( *this, "blocked_dialog_exclusion_textctrl", wxTextCtrl )
        ->SetValue( exclusion_string );
    
    XRCCTRL( *this, "blocked_dialog_action_choice", wxChoice )
        ->SetSelection( action );   
                
    XRCCTRL( *this, "blocked_dialog_priority_spinctrl", wxSpinCtrl )
        ->SetValue( priority );
}


blocked_dialog::~blocked_dialog()
{
}


void blocked_dialog::transfer_to( wxString& exclusion_string, long& action, long& priority )
{   
   exclusion_string = XRCCTRL( *this, "blocked_dialog_exclusion_textctrl", wxTextCtrl )
                        ->GetValue();
   action           = XRCCTRL( *this, "blocked_dialog_action_choice", wxChoice )
                        ->GetSelection();  
   priority         = XRCCTRL( *this, "blocked_dialog_priority_spinctrl", wxSpinCtrl )
                        ->GetValue();
}


//----------------------------------------------------------------------------------------
// Private members
//----------------------------------------------------------------------------------------

void blocked_dialog::OnOK( wxCommandEvent& event )
{   
    // Prevent ending the dialog if the exclusion box is empty.
    wxString exclusion_string;

    exclusion_string = XRCCTRL( *this, "blocked_dialog_exclusion_textctrl", wxTextCtrl )->GetValue();

    if ( exclusion_string == wxT( "") ) 
    {
         wxMessageDialog message_dlg( this, _( "You must enter some text in the \"Item to block\" edit box." ),
                                      _( "Enter exclusion" ),
                                      wxOK|wxICON_INFORMATION  );
        message_dlg.ShowModal();
        // Abort the OnOK function
        return;                   
    }
    
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow.    
    EndModal( wxID_OK );
}


void blocked_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_BLOCKED_DIALOG );
#endif 
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR
