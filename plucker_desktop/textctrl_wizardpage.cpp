//----------------------------------------------------------------------------------------
// Name:        textctrl_wizardpage.cpp
// Purpose:     A wizard page with a labeled checkbox, and a descriptive message
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: textctrl_wizardpage.cpp,v 1.8 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "textctrl_wizardpage.h"
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

#include "textctrl_wizardpage.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( textctrl_wizardpage, wxWizardPageSimple )
    EVT_WIZARD_PAGE_CHANGING( -1, textctrl_wizardpage::on_wizardpage_changing )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor. Parent is the wizard page. The message is what appears in the textctrl
textctrl_wizardpage::textctrl_wizardpage( wxWizard *parent, 
                                          const wxArrayString message_string_array,
                                          const wxString& textctrl_label, 
                                          const wxString& textctrl_value, 
                                          bool entry_required,                                           
                                          wxBitmap bitmap )
       : wxWizardPageSimple( parent )
{
    wxLogDebug( wxT( "Entering textctrl_wizardpage constructor") );
  
    // To set a custom bitmap in a derived wxWizardPageSimple, you just have to 
    // set the wxWizardPageSimple m_bitmap member in the derived constructor.
    // ( see wx examples ). .
    m_bitmap = bitmap;  

    // Save the requirements and error message in members.
    m_entry_required               = entry_required;
    
    // Create a vertical boxsizer
    wxBoxSizer *root_boxsizer = new wxBoxSizer( wxVERTICAL );

    // Now, loop through the array of message lines, adding rows of statictext controls
    // We loop creation of statictext, instead of a single statictext with 
    // newlines, since some platforms (eg MSW) can't make a wrappable statictext    
    for ( int n=0; n < (int)message_string_array.GetCount(); n++ )
    {    
        // We will make the new wxStaticText inside the wxSizer::Add() function, so that
        // we don't have to worry about generating a unique pointer name for each one.
        // wxSizer::Add() syntax is the control, then option(always zero), then
        // orient/border flags, then border size (usually 5).
        root_boxsizer->Add( new wxStaticText( this, -1, message_string_array.Item( n ) ),
                            0, wxALIGN_CENTER_VERTICAL|wxTOP, 5 );                 
    }
    
    // Add a spacer as a row of the boxsizer
    root_boxsizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    // Create a 2 column flexgridsizer to hold the statictext and the textctrl
    wxFlexGridSizer *text_flexgridsizer = new wxFlexGridSizer( 2, 0, 0 );
    text_flexgridsizer->AddGrowableCol( 1 );
    text_flexgridsizer->AddGrowableRow( 0 );

    // Create the static text and add it to the flexgridsizer
    wxStaticText *statictext = new wxStaticText( this, -1, textctrl_label );
    text_flexgridsizer->Add( statictext, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxBOTTOM, 5 );
    
    // Create the member textctrl (initializing it with value of the passed textctrl_value) 
    // and add it to the flexgridsizer
    m_textctrl = new wxTextCtrl( this, -1, textctrl_value );
    text_flexgridsizer->Add( m_textctrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxBOTTOM, 5 );

    // Add the flexgridsizer to the root boxsizer
    root_boxsizer->Add( text_flexgridsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    // Now that everything is added into the boxsizer, send some commands to 
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_boxsizer );
    root_boxsizer->Fit( this );
    root_boxsizer->SetSizeHints( this );                  
 }


// Destructor
textctrl_wizardpage::~textctrl_wizardpage()
{
}


wxString textctrl_wizardpage::get_textctrl_value()
{
    return  m_textctrl->GetValue();
}

//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

void textctrl_wizardpage::on_wizardpage_changing( wxWizardEvent& event )
{   
    // If the event was a "Next>" button event not a "<Prev" button event...
    // (wxWizardEvent::GetDirection for EVT_WIZARD_PAGE_CHANGING returns TRUE if we're
    // going forward or FALSE otherwise and for EVT_WIZARD_PAGE_CHANGED returns TRUE if we
    // came from the previous page and FALSE if we returned from the next one.
    if ( event.GetDirection() == TRUE ) 
    { 
        // ..and if the textcrl is empty, and we said that entry is required...
        if ( m_textctrl->GetValue() == wxEmptyString && m_entry_required )
        {
            wxMessageDialog need_entry_messagedialog( this,
                                _( "Please enter a value in the text box to continue the wizard." ),
                                _( "Text required" ),     
                                wxOK | wxICON_INFORMATION 
                                                    );   
            // ...then show the message dialog...
            need_entry_messagedialog.ShowModal();
            // .. and veto the wizardpagechanging event from moving off this wizardpage.
            event.Veto();
        // ...otherwise, if not empty, just continue normally.
        } 
        else 
        {
            return;
        }    
        
    // ..otherwise it was a "<Prev" button event, which we don't care about.    
    } 
    else 
    {
        return;
    } 
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
