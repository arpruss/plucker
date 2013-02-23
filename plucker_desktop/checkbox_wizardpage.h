/*!
    \file       checkbox_wizardpage.h
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes checkbox_wizardpage class
    \author     Robert O'Connor
    \date       2002/05/03
 */
// RCS-ID:      $Id: checkbox_wizardpage.h,v 1.6 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _CHECKBOX_WIZARDPAGE_H_
#define _CHECKBOX_WIZARDPAGE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "checkbox_wizardpage.h"
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
// Class definition: checkbox_wizardpage
//----------------------------------------------------------------------------------------

//! A wizard page with a labeled checkbox, and a descriptive message.
/*!
    Contains a single checkbox below a custom message. The value of the checkbox can
    be queried by the wizard parent.
 */
class checkbox_wizardpage : public wxWizardPageSimple
{

public:   
   
    //! Constructor.
    /*!
        \note: An implementation using a wxString[] for the messages doesn't seem to 
        work, but wxArrayString works fine.
        \param parent The parent wizard.
        \param message_string_array An array of strings to use as the message. Each
            array item is placed on a separate line.
        \param checkbox_label The label displayed beside the checkbox.
        \param is_checked TRUE if the the checkbox should be checked initially.
        \param bitmap Optional bitmap to show instead of the one the parent wizard is
            using.
     */
    checkbox_wizardpage( wxWizard* parent,
                         const wxArrayString message_string_array,
                         const wxString& checkbox_label = wxEmptyString,
                         bool is_checked = FALSE,
                         wxBitmap bitmap = wxNullBitmap );

    //! Destructor
    ~checkbox_wizardpage();

    //! Get the value of the checkbox.
    /*!
        \return TRUE if the checkbox is checked.
     */
    bool get_checkbox_value();

private:

    wxCheckBox*  m_checkbox;

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_CHECKBOX_WIZARDPAGE_H_
