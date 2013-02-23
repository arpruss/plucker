//----------------------------------------------------------------------------------------
/*!
    \file       destinations_wizardpage.h
    \modified
    \copyright  (c) Robert O'Connor ( www.oconnor.md )
    \licence    GPL
    \brief      Describes destinations_wizardpage class
    \author     Robert O'Connor
    \date       2002/05/03
 */
// RCS-ID:      $Id: destinations_wizardpage.h,v 1.7 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _DESTINATIONS_WIZARDPAGE_H_
#define _DESTINATIONS_WIZARDPAGE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "destinations_wizardpage.h"
#endif

// ---------------------------------------------------------------------------------------
// Shared defines
// ---------------------------------------------------------------------------------------

#include "plucker_defines.h"        // Also for plkrNEW_CHANNEL_TEMPLATE_SECTION

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#include "plucker_wizard_base.h"
#include "hhictrl.h"
#include "handheld_dest_type.h"

//----------------------------------------------------------------------------------------
// Class definition: destinations_wizardpage
//----------------------------------------------------------------------------------------

//! A wizardpage of destinations to install output to
/*!
    Contains a wxHandheldInstallCtrl showing handhelds and directories that can be
    installed to. Can be queried by parent wizard to get the destinations.
 */
class destinations_wizardpage : public wxWizardPageSimple
{

public:

    //! Constructor.
    /*!
        \param parent The parent wizard.
        \param handheld_destinations_label The label displayed over the handheld
            listctrl.
        \param directory_destinations_label The label displayed over the directory
            listctrl.
        \param channel_section The default channel section from which to initialize
            its destinations.
        \param bitmap Optional bitmap to show instead of the one the parent wizard is
            using.
     */
    destinations_wizardpage( wxWizard* parent,
                             const wxString& handheld_destinations_label
                                 = _( "Send it to these handhelds" ),
                             const wxString& directory_destinations_label
                                 = _( "Sent it to these directories" ),
                             const wxString& channel_section
                                 = plkrNEW_CHANNEL_TEMPLATE_SECTION,
                             wxBitmap bitmap = wxNullBitmap
                           );

    //! Destructor.
    ~destinations_wizardpage();

    //! Get array of the user destinations
    /*!
        \param handheld_dest_array Array to hold the user destination strings.
     */
    void get_user_destinations( handheld_dest_array_type& handheld_dest_array );
    
    //! Get an array of the directory destinations
    /*!
        \param user_destinations Array to hold the directory destination strings.
     */
    void get_directory_destinations( wxArrayString& directory_destinations );
    
    //! The wxHandheldInstallCtrl of the wizardpage. 
    /*!
        Public, so that properties and methods are available to parent wizard.
        \todo SOURCE: We could have a pointer here to point to the control, instead.
            see progress_dialog::get_listbox() for details.  
    
     */    
    wxHandheldInstallCtrl*    m_handheld_install_ctrl;
    
private:

    //! Action called when user tries to leave the page (checks for >1 destination).
    /*!
        This will pop up a dialog if moving forward or backward on the destinations page
        but there is no destinations yet...
     */
    void on_wizardpage_changing( wxWizardEvent& event );
    
    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _DESTINATIONS_WIZARDPAGE_H_
