//----------------------------------------------------------------------------------------
// Name:        proxy_wizardpage.cpp
// Purpose:     A wizardpage of destinations to install output to
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: proxy_wizardpage.cpp,v 1.12 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "proxy_wizardpage.h"
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

#include "proxy_wizardpage.h"
//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

// ---------------------------------------------------------------------------------------

#include "configuration.h"

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

enum {
    ID_USE_PROXY_CHECKBOX = wxID_HIGHEST + 1,
    ID_PROXY_PASSWORD_PROMPT_CHECKBOX
};

// ---------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
// ---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( proxy_wizardpage, wxWizardPageSimple )
    EVT_UPDATE_UI( ID_USE_PROXY_CHECKBOX, proxy_wizardpage::on_update_ui_use_proxy_checkbox )
    EVT_UPDATE_UI( ID_PROXY_PASSWORD_PROMPT_CHECKBOX, proxy_wizardpage::on_update_ui_proxy_password_prompt_checkbox )
    EVT_WIZARD_PAGE_CHANGING( -1, proxy_wizardpage::on_wizardpage_changing )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor. Parent is the wizard page
proxy_wizardpage::proxy_wizardpage( wxWizard *parent,
                                    wxBitmap bitmap )
       : wxWizardPageSimple( parent )
{
    wxLogDebug( wxT( "Entering proxy_wizardpage constructor") );
    
    // To set a custom bitmap in a derived wxWizardPageSimple, you just have to 
    // set the wxWizardPageSimple m_bitmap member in the derived constructor.
    // ( see wx examples ). .
    m_bitmap = bitmap;
    
    wxFlexGridSizer *root_flexgridsizer = new wxFlexGridSizer( 1, 0, 0 );
    root_flexgridsizer->AddGrowableCol( 0 );
    root_flexgridsizer->AddGrowableRow( 1 );

    wxStaticBox *enable_staticbox = new wxStaticBox( this, -1, _("Enable proxy") );
    wxStaticBoxSizer *enable_staticboxsizer = new wxStaticBoxSizer( enable_staticbox, wxVERTICAL );

    // This control needs an ID number, so that update events only get processed if
    // it is this checkbox that calls it, instead of all of them calling the update handler.
    m_use_proxy_checkbox = new wxCheckBox( this, ID_USE_PROXY_CHECKBOX, _("This computer uses a proxy to access the Internet"), wxDefaultPosition, wxDefaultSize, 0 );
    enable_staticboxsizer->Add( m_use_proxy_checkbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *unsure_statictext = new wxStaticText( this, -1, _( "(If you are not sure whether or not you have a proxy, then just click 'Next')" ) );
    enable_staticboxsizer->Add( unsure_statictext, 0, wxALIGN_CENTRE|wxALL, 5 );

    root_flexgridsizer->Add( enable_staticboxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticBox *settings_staticbox = new wxStaticBox( this, -1, _( "Proxy settings" ) );
    wxStaticBoxSizer *settings_staticboxsizer = new wxStaticBoxSizer( settings_staticbox, wxVERTICAL );

    wxFlexGridSizer *settings_flexgridsizer = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *proxy_server_statictext = new wxStaticText( this, -1, _( "Proxy server:" ) );
    settings_flexgridsizer->Add( proxy_server_statictext, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_proxy_server_textctrl = new wxTextCtrl( this, -1, wxT( ""), wxDefaultPosition, wxSize(290,-1), 0 );
    settings_flexgridsizer->Add( m_proxy_server_textctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *proxy_port_statictext = new wxStaticText( this, -1, _( "Port:" ) );
    settings_flexgridsizer->Add( proxy_port_statictext, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_proxy_port_textctrl = new wxTextCtrl( this, -1, wxT( ""), wxDefaultPosition, wxSize(50,-1), 0 );
    settings_flexgridsizer->Add( m_proxy_port_textctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *proxy_login_statictext = new wxStaticText( this, -1, _( "Proxy login:" ) );
    settings_flexgridsizer->Add( proxy_login_statictext, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_proxy_login_textctrl = new wxTextCtrl( this, -1, wxT( ""), wxDefaultPosition, wxSize(150,-1), wxTE_PASSWORD );
    settings_flexgridsizer->Add( m_proxy_login_textctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *proxy_password_statictext = new wxStaticText( this, -1, _( "Proxy password:" ) );
    settings_flexgridsizer->Add( proxy_password_statictext, 0, wxALIGN_CENTRE|wxALL, 5 );

    m_proxy_password_textctrl = new wxTextCtrl( this, -1, wxT( ""), wxDefaultPosition, wxSize(150,-1), wxTE_PASSWORD );
    settings_flexgridsizer->Add( m_proxy_password_textctrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    settings_staticboxsizer->Add( settings_flexgridsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 0 );

    m_proxy_password_prompt_checkbox = new wxCheckBox( this, ID_PROXY_PASSWORD_PROMPT_CHECKBOX, _( "Prompt for proxy password each time (more secure)" ) );
    settings_staticboxsizer->Add( m_proxy_password_prompt_checkbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    root_flexgridsizer->Add( settings_staticboxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    // Now that everything is added into the flexgrid sizer, send some commands to
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_flexgridsizer );
    root_flexgridsizer->Fit( this );
    root_flexgridsizer->SetSizeHints( this );         
 
    // Set up the initial values
    read_configuration();
 }


// Destructor
proxy_wizardpage::~proxy_wizardpage()
{
}


void proxy_wizardpage::read_configuration()
{
    // Set the initial values of the proxy, based on the current pluckerrc/plucker.ini
    wxString http_proxy;
    http_proxy = the_configuration->Read( wxT( "/DEFAULT/http_proxy"), wxT( "" ) );
    // Check the 'Use proxy' checkbox if there is some values for http_proxy.
    if ( http_proxy != wxT( "") ) 
    {
        m_use_proxy_checkbox->SetValue( TRUE );
    }
    
    // Want to parse out the http://www.myproxy.com:port into separate boxes for URL and
    // port number, since that is the better GUI approach. 
    // Start by seeing if there is 2 ':' in the address
    if ( http_proxy.Freq(':') != 1 ) 
    {
        // Fill the server port with everything before the last ':' (takes care of an http:
        // but also https: case).
        m_proxy_server_textctrl->SetValue( http_proxy.BeforeLast(':') );
        // Port is everything after the second ':'.    
        m_proxy_port_textctrl->SetValue( http_proxy.AfterLast(':') ); 
    } 
    else 
    // Else just have a http://www.myproxy.com address, or some kind of bad URL.
    // Stick it all into the server box.
    {    
        m_proxy_server_textctrl->SetValue( http_proxy );    
    }     
    
    m_proxy_login_textctrl->SetValue( the_configuration->Read( wxT( "/DEFAULT/http_proxy_user"), wxT( "" ) ) );    
    m_proxy_password_textctrl->SetValue( the_configuration->Read( wxT( "/DEFAULT/http_proxy_pass"), wxT( "" ) ) );
    m_proxy_password_prompt_checkbox->SetValue( the_configuration->Read( wxT( "/DEFAULT/http_proxy_pass_prompt"), 0L ) );    
        
    wxLogDebug ( wxT( "Finished reading configuration for proxy wizard preferences") );   
}


void proxy_wizardpage::write_configuration()
{
    wxString http_proxy;
        
    // If 'use proxy' checkbox is checked, write out the info, else write out blanks
    if ( m_use_proxy_checkbox->GetValue() == TRUE )
    {        
        http_proxy = m_proxy_server_textctrl->GetValue();              
        if ( m_proxy_port_textctrl->GetValue() != wxT( "")) 
        {
            http_proxy += ( wxT( ":") + m_proxy_port_textctrl->GetValue() );
        }
        the_configuration->Write( wxT( "/DEFAULT/http_proxy"),
                                  http_proxy );
                 
        the_configuration->Write( wxT( "/DEFAULT/http_proxy_user"), 
                                  m_proxy_login_textctrl->GetValue() );

        the_configuration->Write( wxT( "/DEFAULT/http_proxy_pass"),
                                  m_proxy_password_textctrl->GetValue() );
    } 
    else 
    {
        the_configuration->Write( wxT( "/DEFAULT/http_proxy"), wxT( "" ) );
        
        the_configuration->Write( wxT( "/DEFAULT/http_proxy_user"), wxT( "" ) );
        
        the_configuration->Write( wxT( "/DEFAULT/http_proxy_pass"), wxT( "" ) );
    }

    wxLogDebug ( wxT( "Finished writing proxy wizard settings") );
}


bool proxy_wizardpage::is_proxy_valid()
{
    bool is_valid = TRUE;

    // If 'use proxy' checkbox is checked...
    if ( m_use_proxy_checkbox->GetValue() == TRUE ) 
    {
        wxString http_proxy = m_proxy_server_textctrl->GetValue();
        // And the http_proxy doesn't starts with http:// or https:// mark it invalid.
        if ( ! ( http_proxy.StartsWith( wxT( "http://" ) ) ||
                 http_proxy.StartsWith( wxT( "https://" ) ) ) ) 
        {
            is_valid = FALSE;
        }
    }

    return is_valid;
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void proxy_wizardpage::on_update_ui_use_proxy_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;

    enabled = m_use_proxy_checkbox->GetValue();

    m_proxy_server_textctrl->Enable( enabled );
    m_proxy_port_textctrl->Enable( enabled );
    m_proxy_login_textctrl->Enable( enabled );
    // Don't disable the proxy_password_textctrl, instead we disable the prompt checkbox, 
    // which will properly cascade down an enable/disable
    m_proxy_password_prompt_checkbox->Enable( enabled );
}


void proxy_wizardpage::on_update_ui_proxy_password_prompt_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
    
    // For cascade, only enable child if this one is both checked and enabled
    enabled = ( m_proxy_password_prompt_checkbox->GetValue() == 0 ) && 
              ( m_proxy_password_prompt_checkbox->IsEnabled() );
    
    m_proxy_password_textctrl->Enable( enabled );  
}


void proxy_wizardpage::on_wizardpage_changing( wxWizardEvent& event )
{
    // If proxy wizardpage values are not valid...
    if ( ! is_proxy_valid() ) 
    {
            wxMessageDialog invalid_proxy_messagedialog ( this,
              _( "Your proxy server is invalid. It must begin with either http:// or https://" ),
              _( "Proxy invalid" ),
              wxOK | wxICON_INFORMATION );
            // ...then show the message dialog...
            invalid_proxy_messagedialog.ShowModal();
            // .. and veto the wizardpagechanging event from moving off this wizardpage.
            event.Veto();
            return;
    }
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
