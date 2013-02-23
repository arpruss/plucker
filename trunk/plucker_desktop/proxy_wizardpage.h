//----------------------------------------------------------------------------------------
/*!
    \file       proxy_wizardpage.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes proxy_wizardpage class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: proxy_wizardpage.h,v 1.11 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PROXY_WIZARDPAGE_H_
#define _PROXY_WIZARDPAGE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "proxy_wizardpage.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/wizard.h"

//----------------------------------------------------------------------------------------
// Class definition: proxy_wizardpage
//----------------------------------------------------------------------------------------

//! A wizard page with controls to configure the proxy.
class proxy_wizardpage : public wxWizardPageSimple
{

public:  

    //! Constructor.
    /*!
        \param parent The parent wizard.
        \param bitmap Optional bitmap to show instead of the one the parent wizard is 
            using.
     */    
    proxy_wizardpage( wxWizard* parent,
                      wxBitmap bitmap = wxNullBitmap
                    );
    
    //! Destructor.
    ~proxy_wizardpage();

    //! Assesses whether proxy is valid.
    /*!
        \return TRUE if proxy is valid.
     */
    bool is_proxy_valid();

    //! Writes the proxy settings collected. into their configuration file keys.         
    void write_configuration(); 
        
private:

    void read_configuration();
    void on_update_ui_use_proxy_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_proxy_password_prompt_checkbox( wxUpdateUIEvent& event );
    void on_wizardpage_changing( wxWizardEvent& event );
    
    wxCheckBox* m_use_proxy_checkbox;
    wxTextCtrl* m_proxy_server_textctrl; 
    wxTextCtrl* m_proxy_port_textctrl;
    wxTextCtrl* m_proxy_login_textctrl;
    wxTextCtrl* m_proxy_password_textctrl;
    wxCheckBox* m_proxy_password_prompt_checkbox;
    
    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _PROXY_WIZARDPAGE_H_
