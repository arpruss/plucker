//----------------------------------------------------------------------------------------
// Name:        channel_list_wizardpage.cpp
// Purpose:     A wizard page with a labeled checkbox, and a descriptive message
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: channel_list_wizardpage.cpp,v 1.5 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "channel_list_wizardpage.h"
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

#include "channel_list_wizardpage.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( channel_list_wizardpage, wxWizardPageSimple )

END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

channel_list_wizardpage::channel_list_wizardpage( wxWizard *parent, 
                                                  const wxArrayString message_string_array,
                                                  wxFileConfig* configuration,
                                                  wxBitmap bitmap )
       : wxWizardPageSimple( parent )
{
    wxLogDebug( wxT( "Entering channel_list_wizardpage constructor") );

    // To set a custom bitmap in a derived wxWizardPageSimple, you just have to
    // set the wxWizardPageSimple m_bitmap member in the derived constructor.
    // ( see wx examples ). .
    m_bitmap = bitmap;

    // Make a new flexgrid sizer ( format # of rows, # of columns, then hor/vert gaps)
    wxFlexGridSizer *root_flexgridsizer = new wxFlexGridSizer( 1, 0, 0 );
    // Set the growable columns and rows
    root_flexgridsizer->AddGrowableCol( 0 );
    root_flexgridsizer->AddGrowableRow( 2 );
    // Create a vertical boxsizer
    wxBoxSizer *messages_boxsizer = new wxBoxSizer( wxVERTICAL );

    // Now, loop through the array of message lines, adding rows of statictext controls
    // We loop creation of statictext, instead of a single statictext with
    // newlines, since some platforms (eg MSW) can't make a wrappable statictext
    for ( int n=0; n < (int)message_string_array.GetCount(); n++ )
    {
        // We will make the new wxStaticText inside the wxSizer::Add() function, so that
        // we don't have to worry about generating a unique pointer name for each one.
        // wxSizer::Add() syntax is the control, then option(always zero), then
        // orient/border flags, then border size (usually 5).
        messages_boxsizer->Add( new wxStaticText( this, -1, message_string_array.Item( n ) ),
                            0, wxALIGN_CENTER_VERTICAL|wxTOP, 5 );
    }

    root_flexgridsizer->Add( messages_boxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    // Now add a spacer row to the boxsizer
    root_flexgridsizer->Add( 5, 5, 0, wxALIGN_CENTRE|wxALL, 1 );

    // Create the checkbox, based on our passed checkbox_label
    m_channel_check_listctrl = new channel_check_listctrl( this, -1, configuration );

    // Add the checkbox as a row to the boxsizer
    root_flexgridsizer->Add( m_channel_check_listctrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    // Now that everything is added into the boxsizer, send some commands to
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_flexgridsizer );
    root_flexgridsizer->Fit( this );
    root_flexgridsizer->SetSizeHints( this );
}


// Destructor
channel_list_wizardpage::~channel_list_wizardpage()
{
    wxLogDebug( wxT( "Entering and exiting channel_list_wizardpage destructor") );
}


void channel_list_wizardpage::get_checked_channel_sections( wxArrayString* channel_sections )
{
    m_channel_check_listctrl->get_checked_channel_sections( channel_sections );
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
