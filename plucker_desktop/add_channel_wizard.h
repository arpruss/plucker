//----------------------------------------------------------------------------------------
/*!
    \file       add_channel_wizard.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes add_channel_wizard class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
//  RCS-ID:     $Id: add_channel_wizard.h,v 1.5 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _ADD_CHANNEL_WIZARD_H_
#define _ADD_CHANNEL_WIZARD_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "add_channel_wizard.h"
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
#include "textctrl_wizardpage.h"
#include "starting_page_wizardpage.h"
#include "limits_wizardpage.h"
#include "destinations_wizardpage.h"
#include "checkbox_wizardpage.h"

//----------------------------------------------------------------------------------------
// Class definition: add_channel_wizard
//----------------------------------------------------------------------------------------

//! A wizard for adding a channel
/*!
    When the user selects 'Add new channel using wizard' from a menu or toolbar, this
    wizard will ask questions and add a resulting new channel to the main_listctrl .
 */
class add_channel_wizard : public plucker_wizard_base
{

public:   
   
    //! Constructor.
    /*!
        \param parent The parent window. May be NULL.
        \param id The id of the wizard. Will usually be just -1 .
        \todo SOURCE: Should all the int id's should be WindowIDs instead?
        \param title The title of the wizard.
        \param bitmap The default bitmap used in the wizard. May be overriden by a page.
        \param cancel_message The message that appears when user clicks \e Cancel button.
     */
    add_channel_wizard( wxWindow* parent = NULL, 
                        int id = -1,
                        const wxString& title = wxEmptyString,
                        const wxBitmap& bitmap = wxNullBitmap, 
                        const wxString& cancel_message = _( "Abort the add channel wizard?" )
                      );

    //! Destructor.
    ~add_channel_wizard();  
    
    //! Accessor of the channel section that was created by the wizard.
    /*!
      \return The generated channel section string.
     */
    wxString                    get_generated_channel_section();
    
    //! Accessor of whether user asked to open a channel dialog for more editing.
    /*!
      \return TRUE if user wants to open a channel dialog after wizard closes.
     */
    bool                        get_is_channel_dialog_requested();

private:

    // The two classes that must be implemented from the base class 
    void                        on_finish_button();    
    void                        on_help_button( wxWizardEvent &event );       

    textctrl_wizardpage*        m_channel_name_wizardpage;
    starting_page_wizardpage*   m_starting_page_wizardpage;
    limits_wizardpage*          m_limits_wizardpage;
    destinations_wizardpage*    m_destinations_wizardpage;
    checkbox_wizardpage*        m_launch_channel_dialog_wizardpage;

    wxString                    m_generated_channel_section;
    bool                        m_is_channel_dialog_requested;

    DECLARE_EVENT_TABLE()    

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _ADD_CHANNEL_WIZARD_H_
