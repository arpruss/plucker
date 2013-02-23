//----------------------------------------------------------------------------------------
// Name:        splashscreen.cpp
// Purpose:     Describes splashscreen class
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: splashscreen.cpp,v 1.19 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "splashscreen.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_SPLASHSCREEN )

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

#include "splashscreen.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/image.h"               // wxImage
#include "wx/module.h"				// wxModule

// ---------------------------------------------------------------------------------------

#include "configuration.h"
#include "plucker-desktop.h"

//----------------------------------------------------------------------------------------
// Internal variables
//----------------------------------------------------------------------------------------

splashscreen* splashscreen::ms_instance = NULL;

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

splashscreen::splashscreen( wxWindow* parent, const wxBitmap& bitmap )
            : wxSplashScreen( bitmap,
                              wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_NO_TIMEOUT,
                              50000,
                              parent,
                              -1,
                              wxDefaultPosition, 
                              wxDefaultSize,
                              wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR 
                            )
{    
}


splashscreen::~splashscreen()
{
}


// Static
splashscreen* splashscreen::get( bool create_on_demand )
{
    wxLogDebug( wxT("Entering splashscreen::get()" ));

    if ( ! ms_instance && create_on_demand )
    {
        wxString splash_image_fullname;

        splash_image_fullname = get_plucker_directory( RESOURCES ) 
            << wxT( "/graphics_misc/splash.xpm");

        wxBitmap bitmap;

        if ( bitmap.LoadFile( splash_image_fullname, wxBITMAP_TYPE_XPM ) )
        { 
            ms_instance = new splashscreen( wxTheApp->GetTopWindow(),
                                            bitmap
                                          );
        }
    }            
    
    // Return ms_instance (which will stay as NULL if didn't make one)
    return ms_instance;
}


// Static
splashscreen* splashscreen::set( splashscreen *desired_splashscreen )
{
    wxLogDebug( wxT("Entering splashscreen::set()"));
    
    splashscreen* old_splashscreen = ms_instance;

    ms_instance = desired_splashscreen;

    return old_splashscreen;
}


void splashscreen::close()
{
    if ( ms_instance ) 
    {    
        // TRUE parameter forces a wxWindow::Close(), the wxSplashScreen doesn't get a 
        // chance to veto it.
        Close( TRUE );
    }       
}

//----------------------------------------------------------------------------------------
// Module definition
//----------------------------------------------------------------------------------------

class module_splashscreen : public wxModule
{

    DECLARE_DYNAMIC_CLASS( module_splashscreen )
    
public:

    module_splashscreen() {}
    
    bool OnInit()
    {
        return TRUE;
    }
    
    void OnExit()
    {
        wxLogDebug( wxT("Entering module_splashscreen::OnExit()"));
        
        // We set it to null just for our own satisfaction.
        splashscreen::set( NULL );
        
        // There is no delete, because the wxSplashScreen::OnCloseWindow custom handler
        // already has a this->destroy() which deleted it. If you try to delete it, 
        // it will cause an error.
        wxLogDebug( wxT("Leaving module_splashscreen::OnExit()"));
    }

};

IMPLEMENT_DYNAMIC_CLASS( module_splashscreen, wxModule )

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_SPLASHSCREEN
