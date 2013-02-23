//----------------------------------------------------------------------------------------
// Name:        message_wizardpage.cpp
// Purpose:     A wizard page that just shows a message
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: message_wizardpage.cpp,v 1.7 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "message_wizardpage.h"
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

#include "message_wizardpage.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

// ---------------------------------------------------------------------------------------

#include "utils_string.h"
#include "utils_controls.h"

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( message_wizardpage, wxWizardPageSimple )

END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor. Parent is the wizard page. The message is what appears in the textctrl
message_wizardpage::message_wizardpage( wxWizard *parent, const wxString &message, 
                                        wxBitmap bitmap )
       : wxWizardPageSimple( parent )
{
    wxLogDebug( wxT( "Entering message_wizardpage constructor") );
  
    // To set a custom bitmap in a derived wxWizardPageSimple, you just have to
    // set the wxWizardPageSimple m_bitmap member in the derived constructor.
    // ( see wx examples ). .
    m_bitmap = bitmap;

    // Make a new flexgrid sizer ( format # of rows, # of columns, then hor/vert gaps)
    wxFlexGridSizer *root_flexgridsizer = new wxFlexGridSizer( 1, 0, 0 );
    // Set the growable columns and rows.
    root_flexgridsizer->AddGrowableCol( 0 );
    root_flexgridsizer->AddGrowableRow( 0 );

    // Create the HTML window.
    m_html_window = new wxHtmlWindow( this );
    
    // Set the page contents, and in a way to look like part of a panel, unless the 
    // HTML message overrides it with font color tags or a <body bgcolor=""> parameter.
    utils_controls::set_html_window_text_in_gui_style( m_html_window,        
                                                       message );
                                                       
    // Add the htmlwindow to the sizer. Do it after have set the HTML text, so 
    // don't have to waste those precious nanoseconds re-rendering the HTML page 
    // (especially if it was a very very big page).
    root_flexgridsizer->Add( m_html_window, 0, wxGROW | wxALIGN_CENTER_VERTICAL, 0 );
    
    // Now that everything is added into the flexgrid sizer, send some commands to 
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_flexgridsizer );
    root_flexgridsizer->Fit( this );
    root_flexgridsizer->SetSizeHints( this ); 
    wxLogDebug( wxT( "Completed message_wizardpage constructor") );
}


// Destructor
message_wizardpage::~message_wizardpage()
{
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
