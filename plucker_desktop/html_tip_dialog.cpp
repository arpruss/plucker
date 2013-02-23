//----------------------------------------------------------------------------------------
// Name:        html_tip_dialog.cpp
// Purpose:     A tip dialog capable of showing tips in HTML format.
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: html_tip_dialog.cpp,v 1.5 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "html_tip_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_HTML_TIP_DIALOG )

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

#include "html_tip_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "utils_controls.h"

//----------------------------------------------------------------------------------------
// Internal constants: control IDs
//----------------------------------------------------------------------------------------

enum {
    ID_NEXT_TIP_BUTTON = wxID_HIGHEST + 1
};

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( html_tip_dialog, wxDialog )
    EVT_BUTTON( ID_NEXT_TIP_BUTTON, html_tip_dialog::on_next_tip_button )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

html_tip_dialog::html_tip_dialog( wxWindow* parent,
                                  wxTipProvider* tip_provider,
                                  bool show_at_startup,
                                  const wxString& header_html,
                                  const wxString& footer_html,  
                                  bool use_system_style,                   
                                  wxSize html_window_size,
                                  int html_window_content_padding,
                                  const wxString& title
                                )  
           : wxDialog( parent, -1, title, wxDefaultPosition, wxDefaultSize,
                      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
    m_tip_provider  = tip_provider;
    m_header_html   = header_html;
    m_footer_html   = footer_html;
    m_use_system_style = use_system_style;

    // Create controls in tab order.

    wxButton* close_button = new wxButton( this, wxID_CANCEL, _( "&Close" ) );

    m_checkbox = new wxCheckBox( this, -1, _( "&Show tips at startup" ) );
    m_checkbox->SetValue( show_at_startup );

    wxButton* next_button = new wxButton( this, ID_NEXT_TIP_BUTTON, _( "&Next Tip" ) );

    // Create a html window.
    m_html_window = new wxHtmlWindow( this, -1, wxDefaultPosition, html_window_size );

    // If the paramater for the internal padding, is not -1 (or any negative),
    // then set it to the desired value.
    // Otherwise just leave it at the wxHtmlWindow default (which looks to be about 
    // 10 or so).
    if ( -1 < html_window_content_padding )
    {
        m_html_window->SetBorders( html_window_content_padding );
    }
    
    // Make a box vertical sizer to hald the html window and another vertical boxsizer
    wxBoxSizer* root_boxsizer = new wxBoxSizer( wxVERTICAL );
    
    // Add html window to boxsizer
    root_boxsizer->Add( m_html_window, 1, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 10 );

    // Make a horizontal boxsizer for the controls at the bottom...
    wxBoxSizer* controls_boxsizer = new wxBoxSizer( wxHORIZONTAL );  
    // ...and then add the bottom controls to this controls boxsizer.
    controls_boxsizer->Add( m_checkbox, 0, wxCENTER );
    controls_boxsizer->Add( 10,10,1 );
    controls_boxsizer->Add( next_button, 0, wxCENTER | wxLEFT, 10 );
    controls_boxsizer->Add( close_button, 0, wxCENTER | wxLEFT, 10 );
    
    // Add the controls boxsizer to the root boxsizer.
    root_boxsizer->Add( controls_boxsizer, 0, wxEXPAND | wxALL, 10 );

    // Set the first tip.
    set_tip_text();

    // Do the standard sizer fitting stuff.
    SetAutoLayout( TRUE );
    SetSizer( root_boxsizer );
    root_boxsizer->SetSizeHints( this );
    root_boxsizer->Fit( this );

    // Center the html tip dialog on its parent.
    Centre( wxBOTH | wxCENTER_FRAME );
    
}

   
html_tip_dialog::~html_tip_dialog()
{
}


bool html_tip_dialog::show_tips_on_startup()
{
    return m_checkbox->GetValue();
}


void html_tip_dialog::set_tip_text()
{
    wxString page_contents;

    // Read the tip. Tip is already translated by this point by the m_tip_provider.
    wxString tip = m_tip_provider->GetTip();

    // Make a string of the header html string, then the text from the tip provider,
    // followed by a footer html string.
    page_contents << m_header_html << tip << m_footer_html;

    // If asked to use the sytem style fonts, then...
    if ( m_use_system_style == TRUE )
    {
        // Call our function for setting a html pages text to system's gui style.
        // Don't use the padding flag, since that was set separately.
        utils_controls::set_html_window_text_in_gui_style( m_html_window,
                                                           page_contents,
                            optionSET_HTML_BACKGROUND_TO_MATCH_GUI_LISTBOX      |
                            optionSET_HTML_FONT_COLOR_TO_MATCH_GUI              |
                            optionSET_HTML_FONT_FACE_AND_POINTSIZE_TO_MATCH_GUI );

    } 
    else 
    {
        // Otherwise, just set the content of the html window to that string.
        m_html_window->SetPage( page_contents );
    }
}


void html_tip_dialog::on_next_tip_button( wxCommandEvent& WXUNUSED( event ) )
{
    set_tip_text();
}

//----------------------------------------------------------------------------------------
// External functions
//----------------------------------------------------------------------------------------

bool show_html_tip( wxWindow *parent,
                    wxTipProvider *tip_provider,
                    bool show_at_startup,
                    const wxString& header_html,
                    const wxString& footer_html,
                    bool use_system_style, 
                    wxSize html_window_size,
                    int html_window_content_padding,
                    const wxString& title
                  )
{
    html_tip_dialog a_html_tip_dialog( parent, tip_provider, show_at_startup,
                                       header_html, footer_html, use_system_style,
                                       html_window_size, html_window_content_padding,
                                       title );
    a_html_tip_dialog.ShowModal();

    bool show_tips_on_startup = a_html_tip_dialog.show_tips_on_startup();

    return show_tips_on_startup;
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_HTML_TIP_DIALOG
