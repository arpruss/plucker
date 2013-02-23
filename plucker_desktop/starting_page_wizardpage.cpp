//----------------------------------------------------------------------------------------
// Name:        starting_page_wizardpage.cpp
// Purpose:     A wizard page that gets a starting url or local file
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: starting_page_wizardpage.cpp,v 1.10 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "starting_page_wizardpage.h"
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

#include "starting_page_wizardpage.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

// ---------------------------------------------------------------------------------------

#include "utils_controls.h"
#include "showcase_dialog.h"

// ---------------------------------------------------------------------------------------
// IDs for the radiobuttons, so we can identify their events
// ---------------------------------------------------------------------------------------

enum {
    ID_STARTING_PAGE_WIZARDPAGE_URL_RADIOBUTTON = wxID_HIGHEST+200,
    ID_STARTING_PAGE_WIZARDPAGE_FILE_RADIOBUTTON,
    ID_STARTING_PAGE_WIZARDPAGE_URL_BUTTON,
    ID_STARTING_PAGE_WIZARDPAGE_FILE_BUTTON
};

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( starting_page_wizardpage, wxWizardPageSimple )
    EVT_UPDATE_UI( ID_STARTING_PAGE_WIZARDPAGE_URL_RADIOBUTTON, starting_page_wizardpage::on_update_ui_url_radiobutton )
    EVT_UPDATE_UI( ID_STARTING_PAGE_WIZARDPAGE_FILE_RADIOBUTTON, starting_page_wizardpage::on_update_ui_file_radiobutton )
    EVT_BUTTON( ID_STARTING_PAGE_WIZARDPAGE_URL_BUTTON, starting_page_wizardpage::on_url_button )
    EVT_BUTTON( ID_STARTING_PAGE_WIZARDPAGE_FILE_BUTTON, starting_page_wizardpage::on_file_button )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

starting_page_wizardpage::starting_page_wizardpage( wxWizard *parent, const wxArrayString message_string_array,
                                          const wxString& url_label, const wxString& file_label,
                                          wxBitmap bitmap )
       : wxWizardPageSimple( parent )
{
    wxLogDebug( wxT( "Entering starting_page_wizardpage constructor") );
  
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
    
    // Add a spacer as a row of the boxsizer
    root_boxsizer->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
       
    // Create a 2 column flexgridsizer to hold the statictext and the textctrl
    wxFlexGridSizer *start_flexgridsizer = new wxFlexGridSizer( 2, 0, 8, 0 );
    start_flexgridsizer->AddGrowableCol( 1 );
    start_flexgridsizer->AddGrowableRow( 0 );
    start_flexgridsizer->AddGrowableRow( 1 );
    start_flexgridsizer->AddGrowableRow( 2 );

    // Make a radiobutton, textctrl, and button for each. Add to flexgridsizer as go along.
    // Note the first radiobutton has the wxRB_GROUP flag to mark the beginning of a 
    // new group.
    m_url_radiobutton = new wxRadioButton( this, ID_STARTING_PAGE_WIZARDPAGE_URL_RADIOBUTTON,
         url_label, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    start_flexgridsizer->Add( m_url_radiobutton, 0, 
                              wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5 );

    m_url_textctrl = new wxTextCtrl( this, -1, wxT( "http://" ) );
    start_flexgridsizer->Add( m_url_textctrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_url_button = new wxButton( this, ID_STARTING_PAGE_WIZARDPAGE_URL_BUTTON, _( "Browse showcase..." ) );
    start_flexgridsizer->Add( m_url_button, 0, 
                              wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );

    m_file_radiobutton = new wxRadioButton( this, ID_STARTING_PAGE_WIZARDPAGE_FILE_RADIOBUTTON,
         file_label );
    start_flexgridsizer->Add( m_file_radiobutton, 0, 
                              wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5 );

    m_file_textctrl = new wxTextCtrl( this, -1, wxT( "file://") );
    start_flexgridsizer->Add( m_file_textctrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_file_button = new wxButton( this, ID_STARTING_PAGE_WIZARDPAGE_FILE_BUTTON, _( "Choose file..." ) );
    start_flexgridsizer->Add( m_file_button, 0, 
                              wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );

    // This radiogroup should have one item or the other always selected. On wxGTK, this line isn't
    // needed. on wxMac it is needed to initialize one of them as selected. I think it is fair 
    // so I will include it.
    m_url_radiobutton->SetValue( TRUE );

    // Add the flexgridsizer to the root boxsizer
    root_boxsizer->Add( start_flexgridsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 0 );
  
    // Now that everything is added into the boxsizer, send some commands to 
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_boxsizer );
    root_boxsizer->Fit( this );
    root_boxsizer->SetSizeHints( this );                  
 }


starting_page_wizardpage::~starting_page_wizardpage()
{
}


wxString starting_page_wizardpage::get_home_url_value()
{
    wxString home_url_value;
    
    // A website address
    if ( m_url_radiobutton->GetValue() == TRUE ) 
    {
        home_url_value = m_url_textctrl->GetValue();
    // A local file
    }
    else 
    {
        home_url_value=m_file_textctrl->GetValue();
    }
    
    return home_url_value;
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void starting_page_wizardpage::on_update_ui_url_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = m_url_radiobutton->GetValue();
    m_url_textctrl->Enable( enabled );
    m_url_button->Enable( enabled );
            
    wxLogDebug( wxT( "starting_page_wizardpage::on_update_ui_m_url_radiobutton") ); 
}


void starting_page_wizardpage::on_update_ui_file_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = m_file_radiobutton->GetValue();
    m_file_textctrl->Enable( enabled );
    m_file_button->Enable( enabled );
            
    wxLogDebug( wxT( "starting_page_wizardpage::on_update_ui_m_file_radiobutton") );
}


void starting_page_wizardpage::on_url_button( wxCommandEvent &event )
{

#if ( setupUSE_SHOWCASE )

    showcase_dialog a_showcase_dialog( this );
    
    // Only execute this, if user pressed OK
    if ( a_showcase_dialog.ShowModal() == wxID_OK ) 
    {
        wxString    name;
        wxString    url;
        wxString    description;
        wxString    language;
        wxString    category;
        long        update_frequency;
        wxString    update_period;
        wxDateTime  update_base;
        bool        update_enabled;
        long        bpp;
        long        maxdepth;
        bool        stayonhost;
        bool        stayondomain;
        wxString    url_pattern;
        
        // Show a showcase dialog and collect the selected rows fields into these variables       
        a_showcase_dialog.transfer_to( name, url, description, language, category,
                                       update_frequency, update_period, update_base, 
                                       update_enabled, bpp, maxdepth, stayonhost, 
                                       stayondomain, url_pattern );
        // Set url to the returned data
        m_url_textctrl->SetValue( url );                      
    }

#endif
    
}


void starting_page_wizardpage::on_file_button( wxCommandEvent &event )
{     
    // [utils_controls.cpp: Includes the code to automatically add/remove the 
    // file:// prefix as required] 
    utils_controls::textctrl_filename_selection( m_file_textctrl,
                                 _( "Choose start file" ),
                                 wxT( ""), 
                                 wxT( ""),
                                 FILE_DIALOG_WILDCARDS_PLUCKABLES,
                                 FALSE,
                                 plkrOS_FILE_ARRAY_SEPARATOR,
                                 TRUE );       
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
