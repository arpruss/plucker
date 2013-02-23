//----------------------------------------------------------------------------------------
/*!
    \file       utils_datetime.h         
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes utils_datetime class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: utils_datetime.h,v 1.12 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _UTILS_DATETIME_H_
#define _UTILS_DATETIME_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "utils_datetime.h"
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
// Class definition: utils_datetime (static only)
//----------------------------------------------------------------------------------------

//! A static-only class for commonly used utilites to manipulate date and time.
/*!
    For simpler sanity checking, invalid inputs and outputs are set to Now().
    The datetime format used is the w3c recommended. They are defined in 
    plucker_defines.h .
    \sa plkrDATE_TIME_FORMAT
    \sa plkrDATE_FORMAT
    \sa plkrTIME_FORMAT
 */
class utils_datetime 
{

public:

    //! Takes a string of an W3C datetime and converts it to a wxDateTime object.
    /*!
        \param input_w3c_string W3C formatted string to convert to a wxDateTime.
        \return The resulting wxDateTime. Returns Now() if input was invalid.
     */
    static wxDateTime  w3c_string_to_datetime( wxString input_w3c_string );
    
    //! Takes a wxDateTime string and converts it to a an W3C datetime.
    /*!
        \param input_datetime The wxDateTime to be converted to a W3C string.
        \return The resulting W3C formatted datetime string. It is Now() if input was
             invalid.
     */
    static wxString    datetime_to_w3c_string( wxDateTime input_datetime );

    //! Takes a string of an W3C date and converts it to a wxDateTime object.
    /*!
        \param input_w3c_string W3C formatted string to convert to a time.
        \return The resulting date, as a wxDateTime. Returns Now() if input was invalid.
     */
    static wxDateTime  w3c_string_to_date( wxString input_w3c_string );
    
    //! Takes a wxDateTime string and converts it to a an W3C time.
    /*!
        \param input_date The date, as a wxDateTime, to be converted to a W3C string.
        \return The resulting W3C formatted date string. It is Now() if input was invalid.
     */
    static wxString    date_to_w3c_string( wxDateTime input_date );
    
    /*!
        \param input_w3c_string W3C formatted string to convert to a wxDateTime.
        \return The resulting time, as a wxDateTime.Returns Now() if input was invalid.
     */
    //! Takes a string of an W3C time and converts it to a wxDateTime object.
    static wxDateTime  w3c_string_to_time( wxString input_w3c_string );
    
    //! Takes a wxDateTime string and converts it to a an W3C time.
    /*!
        \param input_ime The time, as a wxDateTime, to be converted to a W3C string.
        \return The resulting W3C formatted time string. It is Now() if input was invalid.
     */      
    static wxString    time_to_w3c_string( wxDateTime input_time );

    //! Get the current datetime.
    /*!
        \return The current time, as a wxDateTime.
     */
    static wxDateTime  get_current_datetime();
    
    //! \todo Fix the compilation date: it won't translate. Months not in catalog?
    //! Get a string of translated date of when the program was compiled.    
    static wxString    get_translated_compilation_date_string();
    /*
        \return A string of the compilation date of the application.
     */

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_UTILS_DATETIME_H_

