//----------------------------------------------------------------------------------------
/*!
    \file       message_wizardpage.h
    \modified                        
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes message_wizardpage class
    \author     Robert O'Connor
    \date       2002/05/03
 */  
// RCS-ID:      $Id: message_wizardpage.h,v 1.6 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _MESSAGE_WIZARDPAGE_H_
#define _MESSAGE_WIZARDPAGE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "message_wizardpage.h"
#endif

// ---------------------------------------------------------------------------------------
// Shared defines
// ---------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/wizard.h"
#include "wx/html/htmlwin.h"

//----------------------------------------------------------------------------------------
// Class definition: message_wizardpage
//----------------------------------------------------------------------------------------

//! A wizard page containing only a descriptive message.
/*!
    Displays a single string custom message. The string will wrap automatically to fill
    the size, since it is in a resizing textctrl. 
    \note The background of the textctrl will be white under GTK, form color on MSW.
 */
class message_wizardpage : public wxWizardPageSimple
{

public:      

    //! Constructor.
    /*!
        \param parent The parent wizard.
        \param message A string to use as the message. Will wrap automatically.
        \param bitmap Optional bitmap to show instead of the one the parent wizard is
            using.        
     */
    message_wizardpage( wxWizard* parent,
                        const wxString& message = wxEmptyString,
                        wxBitmap bitmap = wxNullBitmap );

    //! Destructor.
    ~message_wizardpage();

private:

    wxHtmlWindow*  m_html_window;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_MESSAGE_WIZARDPAGE_H_

