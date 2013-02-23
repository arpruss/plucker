//----------------------------------------------------------------------------------------
/*!
    \file       textctrl_wizardpage.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes textctrl_wizardpage class    
    \author     Robert O'Connor
    \date       2002/05/03    
 */  
// RCS-ID:      $Id: textctrl_wizardpage.h,v 1.6 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _TEXTCTRL_WIZARDPAGE_H_
#define _TEXTCTRL_WIZARDPAGE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "textctrl_wizardpage.h"
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
// Class definition: textctrl_wizardpage
//----------------------------------------------------------------------------------------

//! A wizard page with a labeled textctrl, and a descriptive message.
/*!
    Contains a single textctrl below a custom message. The value of the textctrl can
    be queried by the wizard parent.
 */
class textctrl_wizardpage : public wxWizardPageSimple
{
public:    
    
    //! Constructor.
    /*!
        \note: An implementation using a wxString[] for the messages doesn't seem to 
        work, but wxArrayString works fine.
        \param parent The parent wizard.
        \param message_string_array An array of strings to use as the message. Each 
            array item is placed on a separate line.
        \param textctrl_label The label displayed beside the textctrl.
        \param textctrl_value The initial value of the textctrl. 
        \param entry_required TRUE if an entry is required to go onto the next page.
        \param textctrl_empty_error_message Message to show to user if textctrl was empty.
        \param textctrl_empty_error_title Title of the eror dialog.
        \param bitmap Optional bitmap to show instead of the one the parent wizard is
            using.        
     */
    textctrl_wizardpage( wxWizard* parent,
                         const wxArrayString message_string_array,
                         const wxString& textctrl_label = wxEmptyString, 
                         const wxString& textctrl_value = wxEmptyString,
                         bool entry_required = TRUE,                     
                         wxBitmap bitmap = wxNullBitmap );
                         
    //! Destructor                     
    ~textctrl_wizardpage();    
    
    //! Get the value of the textctrl.
    /*! 
        \return TRUE The string that user entered into the textctrl.
     */
    wxString     get_textctrl_value();
    
    //! Get a pointer to the textctrl.
    /*!
        \return Pointer to the textctrl.
     */
    wxTextCtrl*  get_textctrl() const { return m_textctrl; }

private:

    //! Action called when user tries to leave the page (checks for >1 destination).
    /*!
        This will pop up a dialog if moving forward or backward on the destinations page
        but there is no destinations yet...
     */
    void         on_wizardpage_changing( wxWizardEvent& event );

    wxTextCtrl*  m_textctrl;  
    bool         m_entry_required;  
    
    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_TEXTCTRL_WIZARDPAGE_H_
