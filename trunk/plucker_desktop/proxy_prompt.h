//----------------------------------------------------------------------------------------
/*!
    \file       proxy_prompt.h        
    \modified                            
    \copyright  (c) Dominic Mazzoni, Robert O'Connor    
    \licence    GPL
    \brief      Describes proxy_prompt class    
    \author     Robert O'Connor
    \date       2001/10/20
 */  
// RCS-ID:      $Id: proxy_prompt.h,v 1.4 2007/06/16 02:50:01 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PROXY_PROMPT_H_
#define _PROXY_PROMPT_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "proxy_prompt.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/html/helpctrl.h"
#include "wx/confbase.h"
#include "wx/fileconf.h"

//----------------------------------------------------------------------------------------
// Class definition: proxy_prompt
//----------------------------------------------------------------------------------------

//! A class to manage displaying of a proxy prompt, and maintaining its value
class proxy_prompt 
{

    // I guess one doesn't use DECLARE_DYNAMIC_CLASS on these.
    
public:

    //! Constructor.
    proxy_prompt();

    //! Destructor.
    ~proxy_prompt();

    //! Gets the global object or creates one if none exists.
    /*!
        \param parent Parent window of the proxy_prompt password prompt box. May be NULL.
     */   
    static proxy_prompt* get( wxWindow* parent );
    
    //! Sets the global object and returns a pointer to previous one (may be NULL).
    /*!
        \param desired_proxy_prompt The new proxy_prompt.
     */
    static proxy_prompt* set( proxy_prompt* desired_proxy_prompt );
    
    //! Return the string to append to plucker-build for http-proxy-pass
    wxString get_spider_argument();
    
    //! Resets, so that need to reprompt (otherwise, will just prompt if not stored yet)
    /*!
        Called from the preferences dialog. So that can change the proxy password
     */
    void reset();

private:
    
    // Singleton instance:
    static      proxy_prompt* ms_instance; 
    
    //! Whether to prompt this time, or used the stored value from last prompt
    bool        m_should_prompt;
    
    //! Stored http_proxy_password
    wxString    m_http_proxy_password;
    
    //! Parent window (may be NULL, which it is when called from plucker_controller class)
    wxWindow*   m_parent;
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _PROXY_PROMPT_H_
