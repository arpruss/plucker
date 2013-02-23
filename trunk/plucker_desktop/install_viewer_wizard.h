//----------------------------------------------------------------------------------------
/*!
    \file       install_viewer_wizard.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief          Describes install_viewer_wizard class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: install_viewer_wizard.h,v 1.5 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _INSTALL_VIEWER_WIZARD_H_
#define _INSTALL_VIEWER_WIZARD_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "install_viewer_wizard.h"
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

//----------------------------------------------------------------------------------------
// Class definition: install_viewer_wizard
//----------------------------------------------------------------------------------------

//! A wizard for reinstalling the handheld viewer software.
/*!
    When the user selects 'Install handheld software' from the main_frame menu,
    this wizard will ask questions and install the handheld software.
 */
class install_viewer_wizard : public plucker_wizard_base
{

public:      

    //! Constructor.
    /*!
        \param parent The parent window. May be NULL.
        \param id The id of the wizard. Will usually be just -1 .
        \param title The title of the wizard.
        \param bitmap The default bitmap used in the wizard. May be overriden by a page.
        \param cancel_message The message that appears when user clicks \e Cancel button.
     */
    install_viewer_wizard( wxWindow* parent = NULL, 
                           int id = -1, 
                           const wxString& title = wxEmptyString,
                           const wxBitmap& bitmap = wxNullBitmap, 
                           const wxString& cancel_message = _( "Abort the setup wizard?" )
                         );

    //! Destructor
    ~install_viewer_wizard();  
    
private:

    // The two classes that must be implemented from the base class 
    void                        on_finish_button();    
    void                        on_help_button( wxWizardEvent& event ); 
    
    void                        update_keys_across_all_channels();  

    message_wizardpage*         m_begin_message_wizardpage;
    viewer_wizardpage*          m_viewer_wizardpage;    
    destinations_wizardpage*    m_destinations_wizardpage;
    message_wizardpage*         m_end_message_wizardpage;

    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_INSTALL_VIEWER_WIZARD_H_
