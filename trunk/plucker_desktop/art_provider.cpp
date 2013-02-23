//----------------------------------------------------------------------------------------
// Name:        art_provider.cpp
// Purpose:     Add/edit a blocked string for exclusion lists dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: art_provider.cpp,v 1.9 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "art_provider.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

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

#include "art_provider.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "configuration.h"

//----------------------------------------------------------------------------------------
// Protected members
//----------------------------------------------------------------------------------------

wxBitmap art_provider::CreateBitmap( const wxArtID& id,
                                     const wxArtClient& client,
                                     const wxSize& WXUNUSED(size))
{
    // Both wxArtClient and wxArtID is typdefed as a string not a long or int, so can't 
    // use a switch/case, just lots and lots of ifs ('else' is redundant since we
    // are returning anyways).
    // Placed in alphabetical order.
    if ( client == wxART_HELP_BROWSER ) 
    {
        if ( id == wxART_ADD_BOOKMARK )
            return get_art_by_basename( wxT( "wx_art_add_bookmark" ) );
        if ( id == wxART_DEL_BOOKMARK )
            return get_art_by_basename( wxT( "wx_art_del_bookmark" ) );
        if ( id == wxART_GO_BACK )
            return get_art_by_basename( wxT( "wx_art_go_back" ) );
        if ( id == wxART_GO_DOWN )
            return get_art_by_basename( wxT( "wx_art_go_down" ) );
        if ( id == wxART_GO_FORWARD )
            return get_art_by_basename( wxT( "wx_art_go_forward" ) );
        if ( id == wxART_GO_TO_PARENT )
            return get_art_by_basename( wxT( "wx_art_go_to_parent" ) );
        if ( id == wxART_GO_UP )
            return get_art_by_basename( wxT( "wx_art_go_up" ) );
        if ( id == wxART_HELP_BOOK )
            return get_art_by_basename( wxT( "wx_art_help_book" ) );
        if ( id == wxART_HELP_PAGE )
            return get_art_by_basename( wxT( "wx_art_help_page" ) );
        if ( id == wxART_HELP_SIDE_PANEL )
            return get_art_by_basename( wxT( "wx_art_help_side_panel" ) );
        if ( id == wxART_HELP_SETTINGS )
            return get_art_by_basename( wxT( "wx_art_help_settings" ) );
    }    
        
    // Fall through     
    return wxNullBitmap;
}


wxBitmap art_provider::get_art_by_basename( const wxString& image_basename )
{    
    wxString image_fullname;    
    
    image_fullname << get_plucker_directory( RESOURCES ) 
        << wxT( "/icons_art_provider/" ) << image_basename << wxT( ".xpm" );

    if ( ! wxFile::Exists( image_fullname ) ) 
    {
        // Nothing bad will happen. It will just use the default one.
        wxLogDebug( wxT( "Error: Can't find the image " ) + image_fullname );
        return wxNullBitmap;
    }

    return wxBitmap( image_fullname, wxBITMAP_TYPE_XPM );
}
