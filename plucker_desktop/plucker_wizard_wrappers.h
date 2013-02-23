//----------------------------------------------------------------------------------------
/*!
    \file       plucker_wizard_wrappers.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes plucker_wizard_wrappers class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */   
// RCS-ID:      $Id: plucker_wizard_wrappers.h,v 1.5 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PLUCKER_WIZARD_WRAPPERS_H_
#define _PLUCKER_WIZARD_WRAPPERS_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "plucker_wizard_wrappers.h"
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
// Class definition: Plucker wizard wrappers (static only)
//----------------------------------------------------------------------------------------

//! A static only wrapper class for building and running the different wizards.
class plucker_wizard_wrappers
{

public:

    //! Run a setup_wizard .
    /*! 
       \param parent The parent window. May be NULL.
     */     
    static bool run_setup_wizard( wxWindow* parent );
    
    //! Run an install_wizard .
    /*! 
       \param parent The parent window. May be NULL.
     */     
    static bool run_install_viewer_wizard( wxWindow* parent );
    
    //! Run a add_channel_wizard .
    /*! 
       \param parent The parent window. May be NULL.
       \param generated_channel_section The generated channel section of the new channel.
       \param is_launch_channel_dialog_requested TRUE if user checked the 'Open channel
            dialog when I click \e Finish on the final wizardpage of the wizard.
     */
    static bool run_add_channel_wizard( wxWindow* parent,
                                        wxString* generated_channel_section,
                                        bool* is_launch_channel_dialog_requested );
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_PLUCKER_WIZARD_WRAPPERS_H_
