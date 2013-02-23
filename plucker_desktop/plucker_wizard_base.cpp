//----------------------------------------------------------------------------------------
// Name:        plucker_wizard_base.cpp
// Purpose:     Base class for all the wizards used in Plucker Desktop
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: plucker_wizard_base.cpp,v 1.8 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "plucker_wizard_base.h"
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

#include "plucker_wizard_base.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

//----------------------------------------------------------------------------------------

#include "setup_wizard.h"
#include "install_viewer_wizard.h"
#include "add_channel_wizard.h"
#include "configuration.h"
#include "plucker_controller.h"
#include "utils_string.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// wxWindows class implementation macro
//----------------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS( plucker_wizard_base, wxWizard )

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( plucker_wizard_base, wxWizard )
    EVT_WIZARD_HELP( -1, plucker_wizard_base::on_help_button )
    EVT_WIZARD_CANCEL( -1, plucker_wizard_base::on_cancel_button ) 
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions: interface
//----------------------------------------------------------------------------------------

plucker_wizard_base::plucker_wizard_base( wxWindow* parent, int id, const wxString& title,
                  const wxBitmap& bitmap, const wxString& cancel_message )
        : wxWizard()
{
    // Note above the wxWizard(): we built it with the default constructor.
    
    // Store some constructor variables.
    m_parent            = parent;
    m_cancel_message    = cancel_message;
    
    // Stick a help button on it (reason for 2 step construction).
    wxWindow::SetExtraStyle( wxWIZARD_EX_HELPBUTTON );

    // Help button ready. Create the wizard now.
    wxWizard::Create( parent, -1, title, bitmap );
}


plucker_wizard_base::~plucker_wizard_base()
{
}


void plucker_wizard_base::add_wizardpage_to_wizard( wxWizardPageSimple* wizardpage )
{
    m_wizardpages_array.Add( wizardpage );
}


bool plucker_wizard_base::run_wizard() 
{
    // Chain the wizardpages together
    chain_wizardpages();
    
    // Set the size of the wizard such that the "client area" of the wizard matches the
    // size of the biggest child wizardpage sizer.
    set_best_wizard_size();

    // Have a bool to track whether the finish button was clicked.
    bool finish_button_was_clicked;       
      
    // Runs the wizard, starting at the first page (can't be NULL).   
    wxLogDebug( wxT( "About to plucker_wizard_base::RunWizard()") );    
    finish_button_was_clicked = RunWizard( m_wizardpages_array.Item( 0 ) );    
    wxLogDebug( wxT( "plucker_wizard_base::RunWizard() completed") );    
    
    // If the 'finish' button was clicked on the wizard (ie it finally completed and 
    // should now be applied)....
    if ( finish_button_was_clicked ) 
    {
       wxLogDebug( wxT( "plucker_wizard_base::run wizard(): 'Finish' button clicked...") );
        // ...then now whatever the derived class said it wanted the wizard to do
        // when the finish button was complete.
        on_finish_button();
    }
       
    return finish_button_was_clicked;     
}   


//----------------------------------------------------------------------------------------
// Non-event handler functions: implementation
//----------------------------------------------------------------------------------------

void plucker_wizard_base::chain_wizardpages()
{
    // Specify the wizard page order using the Chain() function, which is just
    // a convenience function, instead of saying the next page of page1 is page2,
    // and the previous page of page2 is page1, can just say Chain( page1, page2 ).
    // If there is 5 pages, we loop the chain command 4 times [hence GetCount()-1]
    for ( size_t n=0; n < ( m_wizardpages_array.GetCount() -1 ) ; n++ ) 
    {
        wxWizardPageSimple::Chain( m_wizardpages_array.Item( n ),
                                   m_wizardpages_array.Item( n+1 ) );
                                   
        wxLogDebug( wxT( "plucker_wizard_base: chained wizardpages %d and %d"), (int)n , (int)n+1 );  
    }
}


// Set the size of the overall wizard to the size of the biggest page.
// By size, we are talking about the size of the "content area" of the wizard,
// excluding titlebar, sidebitmap, buttons, etc.
void plucker_wizard_base::set_best_wizard_size()
{        
    // First start by getting the setting a default size of a wizard.    
    // WARNING: wxWizard::GetPageSize() creates the controls, then when try to 
    // SetPageSize(), it will fail since the controls have already been created.
    // Therefore have to create a size. It doesn't matter much, since I like a larger
    // minimum width anyways. These are defined in plucker_defines.h
    wxSize wizard_size( plkrMINIMUM_WIZARD_WIDTH, plkrMINIMUM_WIZARD_HEIGHT );

    // Now loop through the wizardpages, calling wxSizer::CalcMin() to get the
    // minimum size of the sizer. 
    // Each wizardpage had a setsizer put on it, so we can now call wxWindow::GetSizer()
    // and it will give us a pointer back to that sizer, so we can query it.
    // Note: probably could just query the size of the wizardpage itself, but this is 
    // the recommended wx way.
    
    for ( size_t n=0; n < ( m_wizardpages_array.GetCount() ) ; n++ ) 
    {
        // Get the size of the sizer...        
        wxSize wizardpage_sizer_size;
        // ...using a somewhat long command due to the fact that wxSizer::CalcMin()
        // is abstract, and we don't know what derived type of wxSizer it was, 
        // whether a wxBoxSizer, wxFlexGridSizer, or whatever.
        wizardpage_sizer_size = ( m_wizardpages_array.Item( n )->GetSizer() )->CalcMin();
        
        // Increase wizard's height if sizer exceeds it
        if ( wizard_size.GetHeight() < wizardpage_sizer_size.GetHeight() ) 
        {
            wizard_size.SetHeight( wizardpage_sizer_size.GetHeight() );
        }
    
        // Increase wizard's width if sizer exceeds it
        if ( wizard_size.GetWidth() < wizardpage_sizer_size.GetWidth() ) 
        {
            wizard_size.SetWidth( wizardpage_sizer_size.GetWidth() );
        }    
    }            
    
    wxLogDebug( wxT( "About to set wizard child 'client area' to width=%d, height=%d"), 
                wizard_size.GetWidth(), wizard_size.GetHeight() ); 
    
    // Call wxWizard::SetPageSize() to set the page to calculated size.
    SetPageSize( wizard_size );   
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void plucker_wizard_base::on_cancel_button( wxWizardEvent& event )
{
    wxLogDebug( wxT( "Entering plucker_wizard_base::on_cancel_button()") );
    
    // Make a messagedialog
    wxMessageDialog cancel_messagedialog( this, m_cancel_message, _( "Cancel wizard" ), 
                                          wxYES | wxNO | wxYES_DEFAULT | wxICON_QUESTION );   
    
    // Show the message dialog, and if they clicked YES button...
    if ( cancel_messagedialog.ShowModal() == wxID_YES ) 
    {
        // .. then just keep going and let the wizard be cancelled.
        return;
    } 
    else 
    {
    // ...otherwise, they clicked "NO" button, so veto the wizard's cancel event.
       event.Veto();
    }    
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
