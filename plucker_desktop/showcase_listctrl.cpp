//----------------------------------------------------------------------------------------
// Name:        main_listctrl.cpp
// Purpose:     Listctrl used in the main dialog
// Author:      Robert O'Connor
// RCS-ID:      $Id: showcase_listctrl.cpp,v 1.7 2007/02/02 10:33:46 desrod Exp $
// Copyright:   Robert O'Connor
// Licence:     GPL
//---------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "showcase_listctrl.h"
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

#include "showcase_listctrl.h"  

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"
#include "wx/listctrl.h"

//---------------------------------------------------------------------------------------

#include "showcase_dialog.h"
            
//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( showcase_listctrl, wxListCtrl )

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( showcase_listctrl, wxListCtrl )   
          
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Interface
//----------------------------------------------------------------------------------------

// Constructor
showcase_listctrl::showcase_listctrl( wxWindow *parent, wxWindowID id,
                              const wxPoint& pos, const wxSize& size,
                              long style, const wxValidator& validator,
                              const wxString& name )
   : wxListCtrl( parent, id, pos, size, style, validator, name )
{
    m_parent = parent;
    
    // Set up the m_showcase_field_names wxArrayString;
    const wxString showcase_field_names[] = { _( "Name" ), 
                                              _( "URL" ), 
                                              _( "Description" ),
                                              _( "Language" ), 
                                              _( "Category" ), 
                                              _( "Update Frequency" ),
                                              _( "Update Period" ),
                                              _( "Update Base" ),
                                              _( "Update Enabled" ), 
                                              _( "Image Depth" ), 
                                              _( "Maximum Depth" ), 
                                              _( "Stay On Host" ),                                           
                                              _( "Stay On Domain"),
                                              _( "URL Pattern" )
                                            };   
    
    // This listctrl needs to insert its columns in the constructor, since as soon
    // as the listctrl is built, it is resized and grafted onto an "unknown" XRC 
    // placeholder. This induces an OnSize() event, calling the overrriden OnSize function for 
    // this class, which needs to have 3 columns to resize (else an assert on WXGTK debug
    // build).
    
    // This loop is the same as writing out many lines of the form:
    // InsertColumn( MAXDEPTH_COLUMN, showcase_field_names[ MAXDEPTH_COLUMN ], wxLIST_FORMAT_LEFT, 0 ); 
    for ( int i = 0; i < (int) WXSIZEOF( showcase_field_names ); i++ ) {   
        InsertColumn( i, showcase_field_names[ i ], wxLIST_FORMAT_LEFT, 0 );
    }
}


// Destructor
showcase_listctrl::~showcase_listctrl()
{
    wxLogDebug( wxT( "Entering showcase_listctrl destructor" ) );
}

void showcase_listctrl::set_column_widths( int maximum_listctrl_width )
{
    wxLogDebug( wxT( "Entering showcase_listctrl::set_column_widths" ) );
    wxLogDebug( wxT( "maximum_listctrl_width=%d" ), maximum_listctrl_width );
    // NOTE: Just using 0 and 1 for the first and second columns.
    
    // Set the NAME column automatically to the width of the name column widest entry
    SetColumnWidth( 0, wxLIST_AUTOSIZE );    
    int new_name_column_width = GetColumnWidth( 0 ); 
    wxLogDebug( wxT( "new_name_column_width=%d" ), new_name_column_width );
    
    // Set the size of the listctrl to the available window pane width ( -1 on height,
    // just means use existing ).
    SetSize( maximum_listctrl_width, -1 );
    
    // Now, set the URL column to the rest...
    int new_url_column_width;    
    
    new_url_column_width = maximum_listctrl_width;    
    wxLogDebug( wxT( "new_url_column_width now=%d" ), new_url_column_width );
    new_url_column_width -= new_name_column_width;
    wxLogDebug( wxT( "new_url_column_width is now=%d" ), new_url_column_width );
    new_url_column_width -= wxSystemSettings::GetMetric( wxSYS_VSCROLL_X );     
    wxLogDebug( wxT( "new_url_column_width is now=%d" ), new_url_column_width );
    new_url_column_width -= plkrLISTCTRL_EXTRA_WIDTH_PADDING;
    
    // If dragged the sash over into the name column, then the width of the 
    // new_url_column_width value becomes negative. If so, then just set to zero
    if ( new_url_column_width <= 0 )
    {
        wxLogDebug( wxT( "Negative new_url_column_width, so setting to zero" ) );
        new_url_column_width = 0;
    }
    
    wxLogDebug( wxT( "About to set url_column_width to %d" ), new_url_column_width );
    SetColumnWidth( 1, new_url_column_width );                                  
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_SHOWCASE
