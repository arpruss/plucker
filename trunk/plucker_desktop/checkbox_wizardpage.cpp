//----------------------------------------------------------------------------------------
// Name:        checkbox_wizardpage.cpp
// Purpose:     A wizard page with a labeled checkbox, and a descriptive message
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: checkbox_wizardpage.cpp,v 1.7 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "checkbox_wizardpage.h"
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

#include "checkbox_wizardpage.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( checkbox_wizardpage, wxWizardPageSimple )

END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

checkbox_wizardpage::checkbox_wizardpage( wxWizard *parent, const wxArrayString message_string_array,
                                          const wxString& checkbox_label, bool is_checked,
                                          wxBitmap bitmap )
       : wxWizardPageSimple( parent )
{
    wxLogDebug( wxT( "Entering checkbox_wizardpage constructor") );
  
    // To set a custom bitmap in a derived wxWizardPageSimple, you just have to 
    // set the wxWizardPageSimple m_bitmap member in the derived constructor.
    // ( see wx examples ). .
    m_bitmap = bitmap;  

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
    
    // Now add a spacer row to the boxsizer
    root_boxsizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    // Create the checkbox, based on our passed checkbox_label 
    m_checkbox = new wxCheckBox( this, -1, checkbox_label );
    
    // Set whehter it is checked or not, according to our passed value to the constructor
    m_checkbox->SetValue( is_checked );
    
    // Add the checkbox as a row to the boxsizer
    root_boxsizer->Add( m_checkbox, 0, wxALIGN_BOTTOM|wxTOP, 5 );

    // Now that everything is added into the boxsizer, send some commands to 
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_boxsizer );
    root_boxsizer->Fit( this );
    root_boxsizer->SetSizeHints( this );                  
 }


// Destructor
checkbox_wizardpage::~checkbox_wizardpage()
{
}


bool checkbox_wizardpage::get_checkbox_value()
{
    return m_checkbox->GetValue();
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
