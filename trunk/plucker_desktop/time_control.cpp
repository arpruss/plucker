//----------------------------------------------------------------------------------------
// Name:        time_control.cpp
// Purpose:     A control to set a time (hours and minutes)
// Author:      Robert O'Connor
// RCS-ID:      $Id: time_control.cpp,v 1.6 2007/02/02 10:33:47 desrod Exp $
// Copyright:   Robert O'Connor
// Licence:     GPL
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "time_control.h"
#endif

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

#include "time_control.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

enum
{
    ID_HOUR_TEXTCTRL             = wxID_HIGHEST + 500,
    ID_HOUR_SPINBUTTON,
    ID_MINUTE_TEXTCTRL,
    ID_MINUTE_SPINBUTTON
};

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( time_control, wxPanel )

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( time_control, wxPanel )
    // Note: EVT_TEXT gets called both when user changes text, and program does it via 
    // a wxTextCtrl::SetValue()
    EVT_TEXT( ID_HOUR_TEXTCTRL, time_control::on_hour_textctrl )
    EVT_SPIN( ID_HOUR_SPINBUTTON, time_control::on_hour_spinbutton )
    EVT_TEXT( ID_MINUTE_TEXTCTRL, time_control::on_minute_textctrl )
    EVT_SPIN( ID_MINUTE_SPINBUTTON, time_control::on_minute_spinbutton )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

time_control::time_control( wxWindow *parent, wxWindowID id,
                            const wxPoint& pos, const wxSize& size, const wxString& name  )
   : wxPanel( parent, id, pos, size, wxTAB_TRAVERSAL, name )
{

    wxFlexGridSizer *root_flexgridsizer = new wxFlexGridSizer( 5, 0, 1 );
    root_flexgridsizer->AddGrowableCol( 0 );
    root_flexgridsizer->AddGrowableRow( 0 );

    wxFlexGridSizer *time_flexgridsizer = new wxFlexGridSizer( 5, 0, 1 );
    time_flexgridsizer->AddGrowableCol( 0 );
    time_flexgridsizer->AddGrowableCol( 1 );
    time_flexgridsizer->AddGrowableCol( 2 );
    time_flexgridsizer->AddGrowableCol( 3 );
    time_flexgridsizer->AddGrowableCol( 4 );
    time_flexgridsizer->AddGrowableRow( 0 );

    m_hour_textctrl = new wxTextCtrl( this, ID_HOUR_TEXTCTRL, wxT( "" ), wxDefaultPosition, wxSize( 35, -1 ), 0 );
    m_hour_textctrl->SetMaxLength( 2 );
    time_flexgridsizer->Add( m_hour_textctrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );

    m_hour_spinbutton = new wxSpinButton( this, ID_HOUR_SPINBUTTON, wxDefaultPosition, wxSize( -1,10 ), wxSP_WRAP );
    m_hour_spinbutton->SetRange( 0, 23 );
    m_hour_spinbutton->SetValue( 0 );
    time_flexgridsizer->Add( m_hour_spinbutton, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5 );

    m_colon_statictext = new wxStaticText( this, -1, _( ":" ), wxDefaultPosition, wxDefaultSize, 0 );
    time_flexgridsizer->Add( m_colon_statictext, 0, wxALIGN_CENTRE|wxTOP|wxBOTTOM, 5 );

    m_minute_textctrl = new wxTextCtrl( this, ID_MINUTE_TEXTCTRL, wxT( "" ), wxDefaultPosition, wxSize( 35,-1 ), 0 );
    m_minute_textctrl->SetMaxLength( 2 );
    time_flexgridsizer->Add( m_minute_textctrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 );

    m_minute_spinbutton = new wxSpinButton( this, ID_MINUTE_SPINBUTTON, wxDefaultPosition, wxSize( -1,10 ), wxSP_WRAP );
    m_minute_spinbutton->SetRange( 0, 59 );
    m_minute_spinbutton->SetValue( 0 );
    time_flexgridsizer->Add( m_minute_spinbutton, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

    root_flexgridsizer->Add( time_flexgridsizer, 0, wxALIGN_CENTRE, 5 );

    // Now that everything is added into the boxsizer, send some commands to
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_flexgridsizer );
    root_flexgridsizer->Fit( this );
    root_flexgridsizer->SetSizeHints( this );

}

// Destructor
time_control::~time_control()
{
}


wxDateTime time_control::get_time()
{
    wxDateTime output_time;

    output_time.SetHour( get_hour() );
    output_time.SetMinute( get_minute() );

    return output_time;
}


wxDateTime::wxDateTime_t time_control::get_hour()
{
    int hour_value = m_hour_spinbutton->GetValue();
    if ( ( hour_value < 0 )  || ( 23 < hour_value ) ) 
    {
        hour_value = 0;
    }

    wxDateTime::wxDateTime_t output_hour;
    output_hour = (wxDateTime::wxDateTime_t)hour_value;

    return output_hour;
}


wxDateTime::wxDateTime_t time_control::get_minute()
{
    int minute_value = m_minute_spinbutton->GetValue();
    if ( ( minute_value < 0 ) || ( 59 < minute_value ) ) 
    {
        minute_value = 0;
    }

    wxDateTime::wxDateTime_t output_minute;
    output_minute = (wxDateTime::wxDateTime_t)minute_value;

    return output_minute;
}


void time_control::set_time( const wxDateTime& input_time )
{
    set_hour( input_time.GetHour() );
    set_minute( input_time.GetMinute() );
}


void time_control::set_hour( const wxDateTime::wxDateTime_t input_hour )
{
    wxString buf;
    buf.Printf( wxT( "%02d" ), (int)input_hour );
    m_hour_textctrl->SetValue( buf );
    m_hour_spinbutton->SetValue( (int)input_hour );
}


void time_control::set_minute( const wxDateTime::wxDateTime_t input_minute )
{
    wxString buf;
    buf.Printf( wxT( "%02d" ), (int)input_minute );
    m_minute_textctrl->SetValue( buf );
    m_minute_spinbutton->SetValue( (int)input_minute );    
}


void time_control::enable( bool do_enable )
{
    m_hour_textctrl->Enable( do_enable );
    m_hour_spinbutton->Enable( do_enable );
    m_colon_statictext->Enable( do_enable );
    m_minute_textctrl->Enable( do_enable );
    m_minute_spinbutton->Enable( do_enable );
}

//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

void time_control::on_hour_textctrl( wxCommandEvent& event )
{
    wxString spin_string =  m_hour_textctrl->GetValue( );
    // Spinbutton autohandles bad values, by setting it to zero if out of range.
    long spin_value;
    spin_string.ToLong( &spin_value );
    m_hour_spinbutton->SetValue( (int)spin_value );
}


void time_control::on_hour_spinbutton( wxSpinEvent& event )
{
    wxString spin_string;
    spin_string.Printf( wxT( "%02d" ), event.GetPosition() );

    m_hour_textctrl->SetValue( spin_string );
}


void time_control::on_minute_textctrl( wxCommandEvent& event )
{
    wxString spin_string =  m_minute_textctrl->GetValue();
    // Spinbutton autohandles bad values, by setting it to zero if out of range.    long spin_value;
    long spin_value;
    spin_string.ToLong( &spin_value );
    m_minute_spinbutton->SetValue( (int)spin_value );
}


void time_control::on_minute_spinbutton( wxSpinEvent& event )
{
    wxString spin_string;
    spin_string.Printf( wxT( "%02d" ), event.GetPosition() );

    m_minute_textctrl->SetValue( spin_string );
}
