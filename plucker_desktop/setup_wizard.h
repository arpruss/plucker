//----------------------------------------------------------------------------------------
/*!
    \file       setup_wizard.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes setup_wizard class
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: setup_wizard.h,v 1.7 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _SETUP_WIZARD_H_
#define _SETUP_WIZARD_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "setup_wizard.h"
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

#include "plucker_wizard_base.h"
#include "message_wizardpage.h"
#include "viewer_wizardpage.h"
#include "destinations_wizardpage.h"
#include "proxy_wizardpage.h"
#include "channel_list_wizardpage.h"

//----------------------------------------------------------------------------------------
// Class definition: setup_wizard
//----------------------------------------------------------------------------------------

//! A setup wizard for Plucker Desktop settings shared across channels.
/*!
    When the user does a first run of the application, or selects 'Rerun setup wizard' 
    from the main_frame menu, this wizard will ask questions and install software and set
    preferences.
 */
class setup_wizard : public plucker_wizard_base
{

public:      

    //! Constructor
    /*!
        \param parent The parent window. May be NULL.
        \param id The id of the wizard. Will usually be just -1 .
        \param title The title of the wizard.
        \param bitmap The default bitmap used in the wizard. May be overriden by a page.
        \param cancel_message The message that appears when user clicks \e Cancel button.
     */
    setup_wizard( wxWindow* parent = NULL,
                 int id = -1, 
                 const wxString& title = wxEmptyString,
                 const wxBitmap& bitmap = wxNullBitmap, 
                 const wxString& cancel_message = _( "Abort the setup wizard?" ) 
                );

    //! Destructor
    ~setup_wizard();  
    
private:

    // The two classes that must be implemented from the base class 
    void                        on_finish_button();    
    void                        on_help_button( wxWizardEvent& event );
    
    void                        update_keys_across_all_channels();  

    message_wizardpage*         m_begin_message_wizardpage;
    viewer_wizardpage*          m_viewer_wizardpage;    
    destinations_wizardpage*    m_destinations_wizardpage;
    proxy_wizardpage*           m_proxy_wizardpage;
    message_wizardpage*         m_end_message_wizardpage;
    channel_list_wizardpage*    m_channel_list_wizardpage;

    wxFileConfig*               m_setup_wizard_configuration;
    wxString                    m_setup_wizard_plkrdata_fullname;

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_SETUP_WIZARD_H_
