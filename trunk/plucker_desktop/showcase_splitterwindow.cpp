//----------------------------------------------------------------------------------------
// Name:        showcase_splitterwindow.cpp
// Purpose:     Splitterwindow used in the showcase dialog
// Author:      Robert O'Connor
// RCS-ID:      $Id: showcase_splitterwindow.cpp,v 1.7 2007/02/02 10:33:46 desrod Exp $
// Copyright:   Robert O'Connor
// Licence:     GPL
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "showcase_splitterwindow.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_SHOWCASE )

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

#include "showcase_splitterwindow.h"              

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( showcase_splitterwindow, wxSplitterWindow )

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( showcase_splitterwindow, wxSplitterWindow )   
    EVT_SPLITTER_SASH_POS_CHANGED( -1, showcase_splitterwindow::on_splitter_sash_position_changed )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Interface
//----------------------------------------------------------------------------------------

// Constructor
showcase_splitterwindow::showcase_splitterwindow( wxWindow *parent, wxWindowID id,
                              const wxPoint& pos, const wxSize& size,
                              long style, const wxString& name, showcase_dialog* parent_showcase_dialog )
   : wxSplitterWindow( parent, id, pos, size, style, name )
{
    m_parent = parent;
    m_parent_showcase_dialog = parent_showcase_dialog;
}


// Destructor
showcase_splitterwindow::~showcase_splitterwindow()
{
    wxLogDebug( wxT( "Entering showcase_splitterwindow destructor" ) );
}


void showcase_splitterwindow::on_splitter_sash_position_changed( wxSplitterEvent &event )
{
    // NOTE: For some reason (wx bug?) couldn't get a notification sent to showcase_dialog
    // of a sash changing event (even a event.Skip() don't work, but it does notify the 
    // splitterwindow, therefore notifying it manually.
    wxLogDebug( wxT( "Entering showcase_SPLITTERWINDOW::on_splitter_sash_position_changed" ) );
    
    wxLogDebug( wxT( "Splitter is a vertical line, so notifying showcase_dialog to change columns " ) );
                
    int sash_position = event.GetSashPosition();
    m_parent_showcase_dialog->on_splitter_sash_position_changed( sash_position );    
}


bool showcase_splitterwindow::OnSashPositionChange( int new_sash_position )
{
    wxLogDebug( wxT( "Entering showcase_splitterwindow::OnSashPositionChange" ) );    
    return TRUE;
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_SHOWCASE
