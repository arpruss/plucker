//----------------------------------------------------------------------------------------
/*!
    \file       utils_clipboard.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes utils_clipboard class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: utils_clipboard.h,v 1.5 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _UTILS_CLIPBOARD_H_
#define _UTILS_CLIPBOARD_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "utils_clipboard.h"
#endif

// ---------------------------------------------------------------------------------------
// Shared defines
// ---------------------------------------------------------------------------------------

#include "plucker_defines.h"

// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#include "wx/string.h"

//----------------------------------------------------------------------------------------
// Class definition: utils_clipboard (static only)
//----------------------------------------------------------------------------------------

//! A static-only class for commonly used functions of the wxWindows clipboard.
class utils_clipboard
{

public:   
    //! Gets a fullname array of files off the clipboard.
    /*! 
        \param output_arraystring An array in which to place the clipboard's filenames
        \return TRUE if successful.
     */
    static bool get_fullname_array_from_clipboard( wxArrayString& output_arraystring );
        
    //! Gets a string off of the clipboard.
    /*! 
        \param output_string An string in which to place clipboard's text
        \return TRUE if successful.
     */
    static bool get_text_from_clipboard( wxString& output_string );    
    
    
    //! Sets the clipboard to a string.
    /*! 
        \param input_string A string to place as the clipboard's text
        \return TRUE if successful.
     */
    static bool set_clipboard_to_text( const wxString& input_string );
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_UTILS_CLIPBOARD_H_

