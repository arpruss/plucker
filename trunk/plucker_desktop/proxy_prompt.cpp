//----------------------------------------------------------------------------------------
// Name:        proxy_prompt.cpp
// Purpose:     Manages showing a proxy prompt
// Author:      Robert O'Connor]
// Modified by:
// Created:     2003/05/09
// Copyright:   Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: proxy_prompt.cpp,v 1.5 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "proxy_prompt.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_ONLINE_HELP )

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

#include "proxy_prompt.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/textdlg.h"
//----------------------------------------------------------------------------------------
#include "configuration.h"

//----------------------------------------------------------------------------------------
// Internal variables
//----------------------------------------------------------------------------------------

proxy_prompt* proxy_prompt::ms_instance = NULL;

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

proxy_prompt::proxy_prompt()
{
    m_should_prompt = TRUE;
}


proxy_prompt::~proxy_prompt()
{

}


proxy_prompt* proxy_prompt::get( wxWindow* parent )
{
    wxLogDebug( wxT( "Entering proxy_prompt::get()" ) );

    if ( ! ms_instance ) 
    {
        ms_instance = new proxy_prompt();
    }

    ms_instance->m_parent = parent;

    return ms_instance;
}


// Static
proxy_prompt* proxy_prompt::set( proxy_prompt *desired_proxy_prompt )
{
    proxy_prompt* old_proxy_prompt = ms_instance;
    ms_instance = desired_proxy_prompt;
    return old_proxy_prompt;
}


wxString proxy_prompt::get_spider_argument()
{
    wxString spider_argument;
        
    // If config says to prompt...
    if ( the_configuration->Read( wxT( "/DEFAULT/http_proxy_pass_prompt" ), 0L ) ) 
    {    
        if ( m_should_prompt ) 
        {
        	// When hit cancel m_http_proxy_password will be empty which is fine.
            m_http_proxy_password = wxGetPasswordFromUser( _( "Enter your proxy password:" ),
                                                           _( "Enter Proxy Password" ),
                                                           wxT( "" ),
                                                           m_parent
                                                          );
        }
        
        // So that don't have to prompt next time.
        m_should_prompt = FALSE;
    
        spider_argument = wxT( " --http-proxy-pass=" ) + m_http_proxy_password;
    }
    
    return spider_argument;
}


void proxy_prompt::reset()
{
    m_should_prompt = FALSE;
}

//----------------------------------------------------------------------------------------
// Module definition
//----------------------------------------------------------------------------------------

class module_proxy_prompt : public wxModule
{

    DECLARE_DYNAMIC_CLASS( module_proxy_prompt )
    
public:

    module_proxy_prompt() {}
    
    bool OnInit()
    {
        return TRUE;
    }
    
    void OnExit()
    {
        wxLogDebug( wxT( "Entered module_proxy_prompt::OnExit()" ) );
        // We set it to null, and a pointer returns to the previous one, which we delete.
        delete proxy_prompt::set( NULL );
        wxLogDebug( wxT( "Completed module_proxy_prompt::OnExit()" ) );
    }

};

IMPLEMENT_DYNAMIC_CLASS( module_proxy_prompt, wxModule )

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ONLINE_HELP
