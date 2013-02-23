//----------------------------------------------------------------------------------------
/*!
    \file       handheld_dest_type.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes handheld_dest_type class    
    \author     Robert O'Connor
    \date       2002/10/20    
 */  
// RCS-ID:      $Id: handheld_dest_type.h,v 1.6 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _HANDHELD_DEST_TYPE_H_
#define _HANDHELD_DEST_TYPE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "handheld_dest_type.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/datetime.h"

//----------------------------------------------------------------------------------------
// Class definition: handheld_dest_type
//----------------------------------------------------------------------------------------

//! A class to hold a description of a handheld destination.
/*!
    A handheld destination consists of info about user_name, whether to card or not,
    and the directory on the card.
 */
class handheld_dest_type  :  public wxObject
{
    DECLARE_DYNAMIC_CLASS( handheld_dest_type )

public:

    //! Constructor.
    /*!
        \param initialize_to_illegal_values TRUE to put an illegal value in each
            member (such a -1 in a switch that is usually 0 or 1. This allows us to
            track whether each item was actually set by something to 0 or 1--
            otherwise wouldn't know if it started at 0, or something specifically
            said that is should be 0.
     */
    handheld_dest_type( bool initialize_to_illegal_values = TRUE );

    //! Destructor
    ~handheld_dest_type();

    //! The user_name.
    wxString    user_name;

    //! Destination on a handheld: RAM, SD card, or Memory Stick
    /*!
        0=To RAM
        1=To SD card
        2=To Memory Stick
        3=To CompactFlash card
     */
    long        handheld_target_storage_mode;

    //! The directory on the card.
    wxString    directory_on_card;
    
    //! Whether to show a prompt that will ask to press HotSync button first (only used pilotlink +USB on GTK)
    long		is_usb_pause;
    
};

// ----------------------------------------------------------------------------------------
// Define an array type for our class (an array of pointers only). Note that 
// WX_DEFINE_ARRAY is only for ints/pointers, not objects. 
// This WX macro declares a new array type, called handheld_dest_array_type,
// which consists of a number of handheld_dest_type items.
// ----------------------------------------------------------------------------------------

WX_DEFINE_ARRAY( handheld_dest_type*, handheld_dest_array_type );

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_HANDHELD_DEST_TYPE_H_
