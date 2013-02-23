//----------------------------------------------------------------------------------------
/*!
    \file       internationalization.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes add_channel_wizard class    
    \author     Robert O'Connor
    \date       2002/02/16     
 */  
// RCS-ID:      $Id: internationalization.h,v 1.7 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _INTERNATIONALIZATION_H_
#define _INTERNATIONALIZATION_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "internationalization.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTERNATIONALIZATION )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/intl.h"

//----------------------------------------------------------------------------------------
// Class definition: internationalization
//----------------------------------------------------------------------------------------

//! Static-only class to manage the internationalization of the application.
/*!
    Maintains what translation (using .po files) of the program to use.
    All translations are located in plucker-desktop/langs directory. 
    See note in plucker-desktop/langs more information on how to translate.
 */
class internationalization 
{

public:

    //! Initialize internationalization for the application.
    /*!
        \return TURE if the setting of the locale was successful. 
     */
    static bool init_internationalization();
    
    //! Finishes the internationalization. Currently does nothing.
    static bool finish_internationalization();
   
    //! Get the wx_locale_number from the application's configuration file.
    /*!
        \return The wx Local number (defined in wx headers)
     */ 
    static int get_wx_locale_number_from_configuration();
    
    //! Get the wx_locale_number from a language user entered in a choice dialog.
    /*!
        \return The wx Local number (defined in wx headers)
     */
    static int get_wx_locale_number_from_choice_dialog();
    
private: 

    static bool set_locale_to_locale_wx_number( int locale_wx_number );

    static void send_debugging_locale_information();

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_INTERNATIONALIZATION

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_INTERNATIONALIZATION_H_
