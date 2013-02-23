//----------------------------------------------------------------------------------------
// Name:        limits_wizardpage.cpp
// Purpose:     A wizard page with controls for limits of a channel
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: limits_wizardpage.cpp,v 1.8 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "limits_wizardpage.h"
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

#include "limits_wizardpage.h"

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
    ID_LIMITS_WIZARDPAGE_STAYONHOST_STAYONDOMAIN_CHECKBOX = wxID_HIGHEST+100
};

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( limits_wizardpage, wxWizardPageSimple )
   EVT_UPDATE_UI( ID_LIMITS_WIZARDPAGE_STAYONHOST_STAYONDOMAIN_CHECKBOX, limits_wizardpage::on_update_ui_stayonhost_stayondomain_checkbox )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

limits_wizardpage::limits_wizardpage( wxWizard *parent, const wxString& channel_section,
                                      wxBitmap bitmap )
       : wxWizardPageSimple( parent )
{
    wxLogDebug( wxT( "Entering limits_wizardpage constructor") );
 
    // To set a custom bitmap in a derived wxWizardPageSimple, you just have to 
    // set the wxWizardPageSimple m_bitmap member in the derived constructor.
    // ( see wx examples ). .
    m_bitmap = bitmap;  

    wxFlexGridSizer *root_flexgridsizer = new wxFlexGridSizer( 1, 0, 0 );
    root_flexgridsizer->AddGrowableCol( 0 );
    root_flexgridsizer->AddGrowableRow( 4 );

    // Add a Insruction message and add to boxsizer
    wxStaticText *maxdepth_instructions_statictext = new wxStaticText( this, -1,
        _( "Enter the depth of pages that you want to retrieve (1=retrieve starting page only):" ) );
    root_flexgridsizer->Add( maxdepth_instructions_statictext, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

    // Add the maxdepth controls
    wxStaticBox *maxdepth_staticbox = new wxStaticBox( this, -1, _( "Maximum depth" ) );
    wxStaticBoxSizer *maxdepth_staticboxsizer = new wxStaticBoxSizer( maxdepth_staticbox, wxHORIZONTAL );

    wxStaticText *maxdepth1_statictext = new wxStaticText( this, -1, 
        _( "Retreive linked pages to a depth of" ) );
    maxdepth_staticboxsizer->Add( maxdepth1_statictext, 0, wxALIGN_CENTRE|wxALL, 5 );

    m_maxdepth_spinctrl = new wxSpinCtrl( this, -1, wxT( "2"), wxDefaultPosition, wxSize(70,-1), 0, 1, 100000, 2 );
    maxdepth_staticboxsizer->Add( m_maxdepth_spinctrl, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *maxdepth2_statictext = new wxStaticText( this, -1, 
        _( "from the starting page" ) );
    maxdepth_staticboxsizer->Add( maxdepth2_statictext, 0, wxALIGN_CENTRE|wxALL, 5 );

    root_flexgridsizer->Add( maxdepth_staticboxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0 );

    // Add a spacer
    root_flexgridsizer->Add( 20, 10, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 0 );

    // Add the stayonhost/stayondomain controls
    wxStaticText *stayonhost_stayondomain_instructions = new wxStaticText( this, -1,
        _( "Enter whether you want to exclude linked pages that are not on the server:" ) );
    root_flexgridsizer->Add( stayonhost_stayondomain_instructions, 0, 
                             wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

    wxStaticBox *stayonhost_stayondomain_staticbox = new wxStaticBox( this, -1, 
        _( "'Stay on host' / 'Stay on domain' filters" ) );
    wxStaticBoxSizer *stayonhost_stayondomain_staticboxsizer = new wxStaticBoxSizer( stayonhost_stayondomain_staticbox, wxVERTICAL );

    m_stayonhost_stayondomain_checkbox = new wxCheckBox( this, ID_LIMITS_WIZARDPAGE_STAYONHOST_STAYONDOMAIN_CHECKBOX,
        _( "Ignore links to a server that is different from starting page's server" ) );
    stayonhost_stayondomain_staticboxsizer->Add( m_stayonhost_stayondomain_checkbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *stayonhost_boxsizer = new wxBoxSizer( wxHORIZONTAL );

    stayonhost_boxsizer->Add( 10, 10, 0, wxALIGN_CENTRE|wxALL, 5 );

    m_stayonhost_radiobutton = new wxRadioButton( this, -1,
        _( "Limit to the exact server only" ),
        wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    stayonhost_boxsizer->Add( m_stayonhost_radiobutton, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    stayonhost_stayondomain_staticboxsizer->Add( stayonhost_boxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxBoxSizer *stayondomain_boxsizer = new wxBoxSizer( wxHORIZONTAL );

    stayondomain_boxsizer->Add( 10, 10, 0, wxALIGN_CENTRE|wxALL, 5 );

    m_stayondomain_radiobutton = new wxRadioButton( this, -1,
        _( "Only restrict to domain (eg allow www.plkr.org and www2.plkr.org)" ) );
    stayondomain_boxsizer->Add( m_stayondomain_radiobutton, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5 );

    stayonhost_stayondomain_staticboxsizer->Add( stayondomain_boxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    // Add the stayonhost/stayondomain controls to root boxizer
    root_flexgridsizer->Add( stayonhost_stayondomain_staticboxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 0 );
  
    // Set the default values
    read_configuration( channel_section );
  
    // Now that everything is added into the boxsizer, send some commands to 
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_flexgridsizer );
    root_flexgridsizer->Fit( this );
    root_flexgridsizer->SetSizeHints( this );                
 }


limits_wizardpage::~limits_wizardpage()
{
}

int limits_wizardpage::get_maxdepth_value()
{
    return m_maxdepth_spinctrl->GetValue();
}


bool limits_wizardpage::get_stayonhost_value()
{
    return m_stayonhost_radiobutton->GetValue();
}


bool limits_wizardpage::get_stayondomain_value()
{
    return m_stayondomain_radiobutton->GetValue();
}


void limits_wizardpage::read_configuration( const wxString& channel_section )
{
    // Read its default values from our passed section name
    m_maxdepth_spinctrl->SetValue( (int)the_configuration->Read( wxT( "/") + channel_section 
                                                                 + wxT( "/home_maxdepth"), 2L ) );
    
    bool stayonhost     = (bool)the_configuration->Read( wxT( "/") + channel_section 
                                                         + wxT( "/home_stayonhost"), 0L );
    bool stayondomain   = (bool)the_configuration->Read( wxT( "/") + channel_section 
                                                         + wxT( "/home_stayondomain"), 0L );
   
    // Start by wiping both radiobuttons to FALSE.
    m_stayonhost_radiobutton->SetValue( FALSE );
    m_stayondomain_radiobutton->SetValue( FALSE ); 
    
    // If both stayonhost and stayondomain are true, then only select the stayonhost, since
    // stayondomain is then meaningless. If neither are true, then select the stayondomain,
    // (so a radio will always be selected) and uncheck the checkbox.
    if ( stayonhost ) 
    {
        m_stayonhost_radiobutton->SetValue( TRUE );
    } 
    else 
    {
        m_stayondomain_radiobutton->SetValue( TRUE );
    }
    bool check_checkbox = false;    
    if ( stayonhost || stayondomain ) 
    {
        check_checkbox = true;
    }
    m_stayonhost_stayondomain_checkbox->SetValue( check_checkbox );
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void limits_wizardpage::on_update_ui_stayonhost_stayondomain_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled =  m_stayonhost_stayondomain_checkbox->GetValue();
    m_stayonhost_radiobutton->Enable( enabled );
    m_stayondomain_radiobutton->Enable( enabled );    

    wxLogDebug( wxT( "limits_wizardpage::on_update_ui_stayonhost_stayondomain_checkbox") );
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
