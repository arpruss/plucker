//----------------------------------------------------------------------------------------
/*!
    \file       utils_string.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes utils_string class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: utils_string.h,v 1.13 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _UTILS_STRING_H_
#define _UTILS_STRING_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "utils_string.h"
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
// Class definition: utils_string
//----------------------------------------------------------------------------------------

//! A static-only class for utils for string management and conversion
class utils_string 
{

public:
    
    //! Converts a wxString to a wxArrayInt, based on the delimiter. 
    /*!
        \param input_string A string of the array items, each separated by the delimiter.
        \param delimiter The single character delimiter that is between each item.
        \param output_arrayint The wxArrayInt to place the converted string's values.
        \param ignore_trailing_delimiter TRUE if a trailing delimiter should be ignored.
            This prevents an empty element at the end of an array if there was a delimiter
            at end of the string.
     */    
    static void string_to_arrayint( wxString input_string, 
                                    wxChar delimiter,
                                    wxArrayInt* output_arrayint,
                                    bool ignore_trailing_delimiter 
                                  );
                                 
    //! Coverts a wxArrayInt to a wxString, separated by the delimiter.
    /*!
        \param input_arraystring The wxArrayString to be converted.
        \param delimiter The single character delimiter to put between each items.
        \return A string of the array items, each separated by the delimiter.
     */
    static wxString arrayint_to_string( wxArrayInt& input_arrayint,
                                        wxChar delimiter
                                      );

    //! Converts a wxString to a wxArrayString, based on the delimiter.
    /*!
        \param input_string A string of the array items, each separated by the delimiter.
        \param delimiter The single character delimiter that is between each item.
        \param output_arraystring The wxArrayString to place the converted string's values.
        \param ignore_trailing_delimiter TRUE if a trailing delimiter should be ignored.
            This prevents an empty element at the end of an array if there was a delimiter
            at end of the string.
     */
    static void string_to_arraystring( wxString input_string,
                                       wxChar delimiter,
                                       wxArrayString *output_arraystring,
                                       bool ignore_trailing_delimiter 
                                     );
    
    //! Coverts a wxArrayString to a wxString, separated by the delimiter.
    /*!
        \param input_arraystring The wxArrayString to be converted.
        \param delimiter The single character delimiter to put between each items.
        \return A string of the array items, each separated by the delimiter.
     */
    static wxString arraystring_to_string( wxArrayString& input_arraystring,
                                           wxChar delimiter
                                         );

    //! Converts a wxString[] array to a wxArrayString.
    /*!
        \param array_size The size of the wxString[] array.
        \param input_stringarray The wxString[] to be converted.
        \param output_arraystring wxArrayString to hold the output.
     */
    static void string_array_to_arraystring( int array_size,
                                             const wxString input_string_array[],
                                             wxArrayString *output_arraystring
                                           );
    
    //! Converts a wxArrayString to a wxString[] array.
    static void arraystring_to_string_array( const wxArrayString& input_arraystring,
                                             wxString output_string_array[] 
                                           );

    //! Takes a wxColor type and converts it to a string like '#ffffff',
    /*!     
        \param input_color The wxColor object to convert.
        \return A hex color string, including a preceeding # .
    */    
    static wxString color_to_hex_string( wxColor input_color );
    
    //! Takes a string of a hex color, and converts it to a wxColor,
    /*!     
        \param input_string A hex color string, including a preceeding # .
        \return The wxColor object to convert
    */    
    static wxColor hex_string_to_color( wxString input_string );
    
    //! Checks to see whether the string is a valid hex color string
    /*! 
        Valid means of the form  '#ffffff', ( with a preceeding # ).
        \param input_string A hex color string, including a preceeding # .
        \return TRUE if the hex color string is valid.
     */
    static bool is_valid_hex_color_string ( wxString input_string );
    
    //! Removes any characters from a string that aren't in the list of legal characters.
    /*!         
        \param input_string An input string that should be stripped.
        \param legal_characters A list of legal characters allowed in the string. 
            For example "ABCDEFGHI1234567_-".
        \return The string with the illegal characters removed.
     */
    static wxString remove_illegal_characters( const wxString& input_string,
                                               const wxString& legal_characters
                                             );

    //! Removes a trailing dir separator from a directory string, if there was one.
    /*!
        Needed since top-level directories returned from a wxDirDialog have a trailing
        dir separator. Such as in a '/' on Linus and 'C:\' on MSW.
        \param input_directory An input directory string that should be stripped.
        \return The directory string with the trainling dir separator removed.
    */
    static wxString remove_trailing_dir_separator( const wxString& input_directory );

    //! Helper function for arraystring_to_string_array
    static void helper_arraystring_to_string_array( const wxArrayString& input_arraystring,
                                                    wxString **choices );

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_UTILS_STRING_H_

