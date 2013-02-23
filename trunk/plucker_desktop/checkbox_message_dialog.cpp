//----------------------------------------------------------------------------------------
// Name:        checkbox_message_dialog.h
// Purpose:     A message dialog, with a checkbox, that says "Don't ask me again", which
//              saves the boolean show_next_time value to the specified path in the 
//              currently active wxConfig object.
// Author:      Julian Smart, Robert O'Connor
// Modified by:
// Created:     2002/06/02
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: checkbox_message_dialog.cpp,v 1.8 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "checkbox_message_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_CHECKBOX_MESSAGE_DIALOG )

//----------------------------------------------------------------------------------------
// Standard wxWindows headers
//----------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//! \todo Trim these things from the source*/
#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/statbmp.h"
    #include "wx/checkbox.h"
    #include "wx/layout.h"
    #include "wx/intl.h"
    #include "wx/icon.h"
    #include "wx/sizer.h"
    #include "wx/app.h"
#endif

//----------------------------------------------------------------------------------------
// Header of this .cpp file
//----------------------------------------------------------------------------------------

#include "checkbox_message_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

//! \todo SOURCE: These 3 don't belong here in a wx way.
#include <stdio.h>
#include <string.h>

#if wxUSE_STATLINE
  #include "wx/statline.h"          // wxStaticLine
#endif

// ----------------------------------------------------------------------------

#include "wx/artprov.h"             // wxArtProvider

// ----------------------------------------------------------------------------

#include "wx/confbase.h"                  
#include "wx/fileconf.h"

//-----------------------------------------------------------------------------
// Internal constants
//-----------------------------------------------------------------------------

enum {
    ID_SHOW_CHECKBOX = wxID_HIGHEST + 1
};

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( checkbox_message_dialog, wxDialog )
    EVT_BUTTON( wxID_YES, checkbox_message_dialog::OnYes )
    EVT_BUTTON( wxID_NO, checkbox_message_dialog::OnNo )
    EVT_BUTTON( wxID_CANCEL, checkbox_message_dialog::OnCancel )
    EVT_BUTTON( wxID_OK, checkbox_message_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( checkbox_message_dialog, wxDialog )

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

checkbox_message_dialog::checkbox_message_dialog( wxWindow *parent,
                                                  const wxString& message,
                                                  const wxString& caption,
                                                  long style,
                                                  const wxPoint& pos,
                                                  const wxString& checkbox_caption, 
                                                  const wxString& config_path_to_show_key,
                                                  bool default_check_checkbox_if_key_not_found_in_config )
                      : wxDialog( parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
    m_dialogStyle = style;
    m_config_path_to_show_key = config_path_to_show_key;

    wxBoxSizer *root_sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *icon_and_text_sizer = new wxBoxSizer( wxHORIZONTAL );

    // 1) icon
    if ( style & wxICON_MASK )
    {
         wxStaticBitmap *icon = new wxStaticBitmap( this, -1,
                                    wxArtProvider::GetBitmap( wxART_QUESTION, wxART_MESSAGE_BOX )
                                                   );
         icon_and_text_sizer->Add( icon, 0, wxCENTER );
    }

    // 2) text
    icon_and_text_sizer->Add( CreateTextSizer( message ), 0, wxCENTER | wxLEFT, 10 );

    root_sizer->Add( icon_and_text_sizer, 0, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );

    // 3) Add the checkbox ( already defined in the class definition, so
    // just m_checkbox, not wxCheckbox *m_checkbox.
    m_checkbox = new wxCheckBox( this, ID_SHOW_CHECKBOX, checkbox_caption );
    root_sizer->Add( m_checkbox, 0, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );

    // 4) buttons
    root_sizer->Add( CreateButtonSizer( style ), 0, wxCENTRE | wxALL, 10 );

    // Have the passed initial value for the checkbox, if it is not in config...
    long default_check_long = (long)default_check_checkbox_if_key_not_found_in_config;   
    bool dialog_checkbox_value = FALSE; 
    // ... and if there is an active config object (FALSE means don't create on demand)
    if ( wxConfigBase::Get( FALSE ) )
    {
        // ..read the key, using the default_check_long value if key not exist
        dialog_checkbox_value = (bool)wxConfigBase::Get()->Read( config_path_to_show_key, 
                                                           default_check_long );
    }
    // ..and set the checkbox to that initial value
    m_checkbox->SetValue( dialog_checkbox_value );

    SetAutoLayout( TRUE );
    SetSizer( root_sizer );

    root_sizer->SetSizeHints( this );
    root_sizer->Fit( this );
    wxSize size( GetSize() );
    if (size.x < size.y*3/2)
    {
        size.x = size.y*3/2;
        SetSize( size );
    }

    Centre( wxBOTH | wxCENTER_FRAME);
}


void checkbox_message_dialog::OnYes(wxCommandEvent& WXUNUSED(event))
{
    save_checkbox_value_to_config();
    EndModal( wxID_YES );
}


void checkbox_message_dialog::OnNo(wxCommandEvent& WXUNUSED(event))
{
    save_checkbox_value_to_config();
    EndModal( wxID_NO );
}


void checkbox_message_dialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    // Allow cancellation via ESC/Close button except if only YES and NO are specified.
    save_checkbox_value_to_config();  
    if ( ( m_dialogStyle & wxYES | wxNO ) != wxYES | wxNO || ( m_dialogStyle & wxCANCEL ) ) 
    {
        EndModal( wxID_CANCEL );
    }
}


void checkbox_message_dialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    save_checkbox_value_to_config();
    EndModal( wxID_OK );
}

//----------------------------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------------------------

void checkbox_message_dialog::save_checkbox_value_to_config()
{
    bool checkbox_value;   
    checkbox_value = m_checkbox->GetValue();

    // Only proceed if there is an active wxConfig [ie don't create one if none already]
    if ( wxConfigBase::Get( FALSE ) ) 
    {
        wxConfigBase::Get()->Write( m_config_path_to_show_key, checkbox_value );
        // Flus() actaully writes it into the config file
        wxConfigBase::Get()->Flush();
    }
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_CHECKBOX_MESSAGE_DIALOG 
