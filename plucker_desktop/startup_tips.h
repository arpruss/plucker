//----------------------------------------------------------------------------------------
/*!
    \file       startup_tips.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes startup_tips class    
    \author     Robert O'Connor
    \date       2002/12/06    
 */  
// RCS-ID:      $Id: startup_tips.h,v 1.10 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _STARTUP_TIPS_H_
#define _STARTUP_TIPS_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "startup_tips.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_STARTUP_TIPS )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/tipdlg.h"              // wxTipProvider and related

//----------------------------------------------------------------------------------------
// Class definition: startup_tips
//----------------------------------------------------------------------------------------

//! Static only class, proving 'Did you know' tips on application startup.
class startup_tips
{

public:

    //! Shows startup tips.
    /*!
        \param parent Parent window.
        \param style of the tips (basic, advanced, rubberchicken, etc)
        \param is_menu_command TRUE if it is being summoned by a menu command
            (Usually Help > Show Tip...).
     */
    static bool show_startup_tips( wxWindow* parent, 
                                   bool is_menu_command = FALSE
                                 );
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_STARTUP_TIPS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_startup_tips_H_
