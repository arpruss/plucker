//----------------------------------------------------------------------------------------
// Name:        utils_string.cpp
// Purpose:     Utils for String management and conversion
// Author:      Robert O'Connor
// Modified by:
// Created:     2002/01/01
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: utils_string.cpp,v 1.17 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "utils_string.h"
#endif

//----------------------------------------------------------------------------------------
// Standard wxWindows headers
//----------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//----------------------------------------------------------------------------------------
// Header of this .cpp file
//----------------------------------------------------------------------------------------

#include "utils_string.h"

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

void utils_string::string_to_arrayint( wxString input_string,
                                       wxChar delimiter,
                                       wxArrayInt *output_arrayint,
                                       bool ignore_trailing_delimiter )
{
    // Abort if empty, else asserts.
    if ( input_string == wxEmptyString ) 
    {
        return;
    }

    // Strip off the trailing delimiter if so requested
    if ( ignore_trailing_delimiter ) 
    {
        if ( input_string.Last() == delimiter )
        {
            input_string.RemoveLast();
        }
    }

    // Get number of array items, by number of delimiters + 1
    int number_of_array_items = input_string.Freq( delimiter ) + 1;

    // Loop through the string parsing
    wxString item_string;
    long item_long = 0;
    for ( int i = 0; i < number_of_array_items; i++ ) 
    {
        item_string = input_string.BeforeFirst( delimiter );
        // If the item is empty then abort this iteration
        if ( item_string.IsEmpty() ) 
        {
            continue;
        }
        // Check to see if string is of a number. If so, convert to a number type and add to string.
        if ( item_string.IsNumber() )
        {
            // Use the ",10" version in wx2.3.2+ to specify base
            //item_string.ToLong( &item_long, 10 );
            item_string.ToLong( &item_long );
            output_arrayint->Add( (int)item_long );
        } 
        else 
        {
           wxLogMessage( wxT( "Error: Non-numerical item in a int array key in configuration file." ) );
        }
        input_string = input_string.AfterFirst( delimiter );
    }
}


wxString utils_string::arrayint_to_string( wxArrayInt& input_arrayint, wxChar delimiter )
{
    wxString output_string;
    wxString temp_string;

    for ( int i = 0; i < (int)input_arrayint.GetCount(); i++ ) 
    {
        temp_string = wxT( "" );
        temp_string.Printf( wxT( "%d" ), input_arrayint[ i ] );
        output_string << temp_string << delimiter;
    }

    // Strip off the delimiter from the end
    output_string = output_string.BeforeLast( delimiter );

    return output_string;
}

void utils_string::string_to_arraystring( wxString input_string, wxChar delimiter, 
                                          wxArrayString *output_arraystring,
                                          bool ignore_trailing_delimiter )
{
    // Abort if empty, else asserts.
    if ( input_string == wxEmptyString ) 
    {
        return;
    }

    // Strip off the trailing delimiter if so requested
    if ( ignore_trailing_delimiter ) 
    {
        if ( input_string.Last() == delimiter ) 
        {
            input_string.RemoveLast();
        }
    }
    
    // Get number of array items, by number of delimiters + 1
    int number_of_array_items = input_string.Freq( delimiter ) + 1;

    // Loop through the string parsing
    wxString item_string;
    for ( int i = 0; i < number_of_array_items; i++ ) 
    {
        item_string = input_string.BeforeFirst( delimiter );
        output_arraystring->Add( item_string );
        input_string = input_string.AfterFirst( delimiter );
    }
}


wxString utils_string::arraystring_to_string( wxArrayString& input_arraystring, wxChar delimiter )
{    
    wxString output_string;

    for ( int i = 0; i < (int)input_arraystring.GetCount(); i++ )
    {
        output_string += ( input_arraystring[i] << delimiter );        
    }
    
    // Strip off the delimiter from the end    
    output_string = output_string.BeforeLast( delimiter );
    
    return output_string;
}


void utils_string::string_array_to_arraystring( int array_size,
                                               const wxString input_string_array[],
                                               wxArrayString *output_arraystring
                                              )
{
    // Empty the outputstring just in case it has something in it.
    output_arraystring->Empty();

    for ( int i = 0; i < array_size; i++ ) 
    {
        // add one by one
        output_arraystring->Add( input_string_array[ i ] );
    }
}


void utils_string::arraystring_to_string_array( const wxArrayString& input_arraystring,
                                                wxString output_string_array[] )
{
    wxString *dummy_string;
    helper_arraystring_to_string_array( input_arraystring, &output_string_array );
    delete [] dummy_string;

}


wxString utils_string::color_to_hex_string( wxColor input_color )
{
    // See wx/src/common/utilscmn.cpp for wxDecToHex utility
    wxString red_hex = wxDecToHex( (int) input_color.Red() );
    wxString green_hex = wxDecToHex( (int) input_color.Green() );
    wxString blue_hex = wxDecToHex( (int) input_color.Blue() );

    wxString output_color_string = wxT( "#" ) + red_hex + green_hex + blue_hex;
    return output_color_string;
}


wxColor utils_string::hex_string_to_color( wxString input_string )
{
    // Shave off the leading #.
    input_string = input_string.AfterFirst( '#' );

    // Extract the substrings using wxString::Mid();
    wxString red_hex_string = input_string.Mid( 0, 2 );
    wxString green_hex_string = input_string.Mid( 2, 2 );
    wxString blue_hex_string = input_string.Mid( 4, 2 );

    // See wx/src/common/utilscmn.cpp for wxHexToDec utility
    int red_hex_int = wxHexToDec( red_hex_string );
    int green_hex_int = wxHexToDec( green_hex_string );
    int blue_hex_int = wxHexToDec( blue_hex_string );

    wxColor output_color( red_hex_int, green_hex_int,  blue_hex_int );

    return output_color;

/* A shorter one, from wxWmlResourceHandler::GetColour()
    wxString v = input_string;
    unsigned long tmp = 0;

    if ( v.Length() != 7 || v[ 0u ] != wxT( "#" ) ||
        wxSscanf( v.c_str(), wxT( "#%lX" ), &tmp ) != 1 )
    {
        wxLogDebug( _( "XRC resource: Invorrect colour specification '%s'."),
                   v.c_str() );
        return wxNullColour;
    }

    return wxColour( (unsigned char)( ( tmp & 0xFF0000 ) >> 16 ),
                     (unsigned char)( ( tmp & 0x00FF00 ) >> 8 ),
                     (unsigned char)( ( tmp & 0x0000FF ) )
                   );
*/

}

// Checks to see whether the string is a valid hex color string of the form  '#ffffff',
// ( with a preceeding # )
bool utils_string::is_valid_hex_color_string ( wxString input_string )
{
    wxString v = input_string;
    unsigned long tmp = 0;

    if ( v.Length() != 7 || v[ 0u ] != '#' ||
        wxSscanf( v.c_str(), wxT( "#%lX" ), &tmp ) != 1 ) 
    {            
        return FALSE;
    } 
    else 
    {
        return TRUE;
    }
    
}

wxString utils_string::remove_illegal_characters( const wxString& input_string,
                                                  const wxString& legal_characters
                                                )
{
    
    // Make an empty output string.    
    wxString output_string;    
    // Make a string that will hold the char to examine.
    wxString test_char;
    // Loop through the string:
    for ( size_t i=0; i < input_string.Length(); i++ )
    {
        // Get the char.
        test_char = input_string.GetChar( i );
        /// If that char is in our list of legal characters...
        if ( legal_characters.Contains( test_char ) )
        {
            // ...then append it to the output string.
            output_string += test_char;
        }
    }

    return output_string;
}


wxString utils_string::remove_trailing_dir_separator( const wxString& input_directory )
{
    wxString output_directory = input_directory;
    // Get the last character
    wxString end_char         = output_directory.Right( 1 );

    // If the last character is either the generic dir separator, or the
    // the platform specific dir separtator...
    if ( end_char == wxT( "/" ) || end_char == plkrOS_DIR_SEPARATOR )
    {
        // ...Then remove the last character.
        output_directory.RemoveLast();
    }
    return output_directory;
}


void utils_string::helper_arraystring_to_string_array( const wxArrayString& input_arraystring,
                                                       wxString **output_string_pointer )
{
    int n = input_arraystring.GetCount();
    *output_string_pointer = new wxString[ n ];

    for ( int i = 0; i < n; i++ ) 
    {
        (*output_string_pointer)[ i ] = input_arraystring[ i ];
    }
}

