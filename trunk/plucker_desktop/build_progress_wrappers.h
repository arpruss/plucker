//----------------------------------------------------------------------------------------
/*!
    \file       build_progress_wrappers.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes build_progress_wrappers class   
    \author     Robert O'Connor
    \date       2002/01/03    
 */ 
// RCS-ID:      $Id: build_progress_wrappers.h,v 1.4 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _BUILD_PROGRESS_WRAPPERS_H_
#define _BUILD_PROGRESS_WRAPPERS_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "build_progress_wrappers.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Class option flags
//----------------------------------------------------------------------------------------

# if ( setupUSE_MSW_FORCE_COMMANDLINE_AUTOCLOSE_PROGRESS_CLOSURE )
enum {
    //! Forces program to terminate when progress dialog closes. 
    /*! 
        Needed only for the case of:
        - MSW
        - Commandline mode
        - Progress dialog (vs. console)
        - Autoclose (vs. close when click OK)
     */
    optionFORCE_PROGRAM_TERMINATION_ON_PROGRESS_DIALOG_CLOSE  = 1
};
#endif

//----------------------------------------------------------------------------------------
// Class definition: build_progress_wrappers
//----------------------------------------------------------------------------------------

//! A static-only wrapper class around plucker-build functions.
/*!
    This class is compiled in regardless of the setupUSE_ENHANCED_PROGRESS_DIALOG compile
    setting, since it has the degrade down to using the console.
 */   
class build_progress_wrappers 
{

public:

    //! Launch a build_progress_dialog, or degrade to console
    /*! 
        If setupUSE_ENHANCED_PROGRESS_DIALOG=0, then degrades to console updating.
        \param parent Parent window. May be NULL (and is if use this from commandline).
        \param channel_sections. The channel sections to update.
        \param force_commandline_autoclose (MSW only) Options
     */  
    static int launch_build_progress_dialog( wxWindow* parent,
                                             const wxArrayString& channel_sections

# if ( setupUSE_MSW_FORCE_COMMANDLINE_AUTOCLOSE_PROGRESS_CLOSURE )
                                             ,
                                             long force_commandline_autoclose = 0
#endif

                                           );

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_BUILD_PROGRESS_WRAPPERS_H_
