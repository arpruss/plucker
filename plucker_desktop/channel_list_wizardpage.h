/*!
    \file       channel_list_wizardpage.h
    \modified
    \copyright  (c) Robert O'Connor ( www.oconnor.md )
    \licence    GPL
    \brief      Describes channel_list_wizardpage class
    \author     Robert O'Connor
    \date       2002/05/03
 */
// RCS-ID:      $Id: channel_list_wizardpage.h,v 1.2 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _CHANNEL_LIST_WIZARDPAGE_H_
#define _CHANNEL_LIST_WIZARDPAGE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "channel_list_wizardpage.h"
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

#include "configuration.h"
#include "channel_check_listctrl.h"

//----------------------------------------------------------------------------------------
// Class definition: channel_list_wizardpage
//----------------------------------------------------------------------------------------

//! A wizard page with a list of channels that can be checked or unchecked.
/*!
    Contains a listctrl of channels with a check/uncheck graphics. The listctrl is 
    below a custom message. The array of selected channel sections can  be queried 
    by the wizard parent.
 */
class channel_list_wizardpage : public wxWizardPageSimple
{

public:

    //! Constructor.
    /*!
        \note: An implementation using a wxString[] for the messages doesn't seem to
        work, but wxArrayString works fine.
        \param parent The parent wizard.
        \param message_string_array An array of strings to use as the message. Each
            array item is placed on a separate line.
        \param configuration Pointer to the wxFileConfig to use to get the channels
            from.
        \param bitmap Optional bitmap to show instead of the one the parent wizard is
            using.
     */
    channel_list_wizardpage( wxWizard* parent,
                             const wxArrayString message_string_array,
                             wxFileConfig* configuration,
                             wxBitmap bitmap = wxNullBitmap );

    //! Destructor
    ~channel_list_wizardpage();

    //! Gets an array of checked channel sections of the m_channel_check_listctrl .
    /*!
        \param channel_sections Pointer to array to store the channel sections.
     */
    void get_checked_channel_sections( wxArrayString* channel_sections );

private:

    channel_check_listctrl*  m_channel_check_listctrl;

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _CHANNEL_LIST_WIZARDPAGE_H_
