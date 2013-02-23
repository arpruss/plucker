//----------------------------------------------------------------------------------------
// Name:        image_dialog.cpp
// Purpose:     Insert an image dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: image_dialog.cpp,v 1.14 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "image_dialog.h"
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

// ---------------------------------------------------------------------------------------
// Header of this .cpp file
//----------------------------------------------------------------------------------------

#include "image_dialog.h"

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

BEGIN_EVENT_TABLE( image_dialog, wxDialog )
    EVT_BUTTON( XRCID( "image_dialog_src_button" ), image_dialog::on_src_button )
    EVT_BUTTON( XRCID( "image_dialog_color_button" ), image_dialog::on_color_button )
    EVT_BUTTON( wxID_HELP_CONTEXT, image_dialog::on_help_button )
    EVT_BUTTON( wxID_OK, image_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor
image_dialog::image_dialog( wxWindow* parent )
{
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "image_dialog") );
}


// Destructor
image_dialog::~image_dialog()
{
}


void image_dialog::transfer_to( wxString& starting_text, wxString& ending_text )
{
    starting_text = m_starting_text;
    ending_text = m_ending_text;
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void image_dialog::on_src_button( wxCommandEvent &event )
{
    // [utils_controls.cpp]    
    utils_controls::textctrl_filename_selection( XRCCTRL( *this, "image_dialog_src_textctrl", wxTextCtrl ),
                                 _( "Choose image source file" ),
                                 wxT( ""),
                                 wxT( ""),
                                 FILE_DIALOG_WILDCARDS_IMAGES,
                                 false,
                                 plkrOS_FILE_ARRAY_SEPARATOR,
                                 true );                                
}


void image_dialog::on_color_button( wxCommandEvent &event )
{
    // [utils_controls.cpp]    
    utils_controls::textctrl_hex_color_selection( XRCCTRL( *this, "image_dialog_color_textctrl", wxTextCtrl ) );
}


// Override wxDialog's default behavior for clicking an OK button.
void image_dialog::OnOK( wxCommandEvent& event )
{
    wxString output_string;
    wxString buf;
    
    wxString href_string = XRCCTRL( *this, "image_dialog_href_textctrl", wxTextCtrl )->GetValue();
    if ( href_string != wxT( "") ) 
    {
        output_string += wxT( "<a href=\"") + href_string + wxT( "\">");
    }    
    
    output_string += wxT( "<img");    

    wxString src_string = XRCCTRL( *this, "image_dialog_src_textctrl", wxTextCtrl )->GetValue();    
    output_string += wxT( " src=\"") + src_string + wxT( '\"');
    // TODO: relative, using the editor dialogs page
        
    wxString alt_string = XRCCTRL( *this, "image_dialog_alt_textctrl", wxTextCtrl )->GetValue();
    if ( alt_string != wxT( "") ) 
    {
        output_string += wxT( " alt=\"") + alt_string + wxT( "\"");
    }    
    
    wxString color_string = XRCCTRL( *this, "image_dialog_color_textctrl", wxTextCtrl )->GetValue();
    if ( color_string != wxT( "") )
    {
        output_string += wxT( " color=\"") + color_string + wxT( "\"");
    }         
     
    output_string += wxT( ">");
    
    if ( href_string != wxT( "") )
    {
        output_string += wxT( "</a>");
    }
    
    // Store the starting and ending strings as class members, ready to be transferred
    // by transfer_to(...) method.
    m_starting_text = output_string;
    m_ending_text = wxT( "");
    
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow.    
    EndModal( wxID_OK );
}


void image_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_IMAGE_DIALOG );
#endif 
}


//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 
