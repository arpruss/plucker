//----------------------------------------------------------------------------------------
/*!
    \file       handheld_dest_wrappers.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes handheld_dest_wrappers class
    \author     Robert O'Connor
    \date       2003/02/02
 */
// RCS-ID:      $Id: handheld_dest_wrappers.h,v 1.3 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _HANDHELD_DEST_WRAPPERS_H_
#define _HANDHELD_DEST_WRAPPERS_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "handheld_dest_wrappers.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"


//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "handheld_dest_type.h"

//----------------------------------------------------------------------------------------
// Class definition: handheld_dest_wrappers
//----------------------------------------------------------------------------------------

//! A static-only wrapper class around wrappers for handheld_dest functions.
class handheld_dest_wrappers
{

public:
    //! Reads a configuration section and builds a handheld_dest array.
    /*!
        \param channel_section The section of plucker.ini to read from.
        \param handheld_dest_array A handheld_dest_array to hold the results of the
            conversion.
     */
    static void read_handheld_dest_array_from_configuration( const wxString& channel_section,
                                              handheld_dest_array_type *handheld_dest_array
                                                           );
    //! Writes a configuration section and builds a handheld_dest array.
    /*!
        \param channel_section The section of plucker.ini to write to.
        \param handheld_dest_array A handheld_dest_array of the data to be written
            to the plucker.ini
     */
    static void write_handheld_dest_array_to_configuration( const wxString& channel_section,
                                              handheld_dest_array_type& handheld_dest_array
                                                           );

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_HANDHELD_DEST_WRAPPERS_H_
