//----------------------------------------------------------------------------------------
// Name:        utils_clipboard.cpp
// Purpose:     Utils for Clipboard and Drag and Drop
// Author:      Robert O'Connor
// Modified by:
// Created:     2002/01/01
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: utils_clipboard.cpp,v 1.7 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "utils_clipboard.h"
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

#include "utils_clipboard.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#if ( setupUSE_DRAG_AND_DROP )
    #include "wx/dnd.h"                 // Drag and drop classes
    #include "wx/clipbrd.h"             // wxClipBoard
#endif

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Gets a fullname array of files off the clipboard. Returns successful
bool utils_clipboard::get_fullname_array_from_clipboard( wxArrayString& output_arraystring )
{
#if ( setupUSE_DRAG_AND_DROP ) 
   // Try to get the data from the clipboard, returning an appropriate error message
    // if was a problem somewhere along the way. Be sure to close the clipboard after
    // opening it.
    if ( ! wxTheClipboard->Open() ) 
    {
        wxLogError( wxT( "Error: Can't open clipboard." ) );
        return FALSE;
    }

    // DF_TEXT stands for DataFormat_Text. Looks to see whether it is text on the clipboard
    if ( ! wxTheClipboard->IsSupported( wxDF_FILENAME ) ) 
    {
        wxLogWarning( wxT( "Error: No filename array data on clipboard" ) );
        wxTheClipboard->Close();
        return FALSE;
    }

    // Create a text data object    
    wxFileDataObject file_data;

    // Try to extract it from the clipboard
    if ( ! wxTheClipboard->GetData( file_data ) )
    {
        wxLogError( wxT( "Error: Can't paste data from the clipboard" ) );
        wxTheClipboard->Close();
        return FALSE;
    } 
    else 
    {
        // Set the output arraystring to the arraystring in the file data object
        output_arraystring = file_data.GetFilenames();
        wxLogDebug( wxT( "Array of filenames has been extracted from clipboard" ) );
        wxTheClipboard->Close();
        return TRUE;               
    }
    
#else   // setupUSE_DRAG_AND_DROP
    return FALSE;
#endif  // setupUSE_DRAG_AND_DROP
}

    
// Gets a string off of the clipboard. Returns successful .
bool utils_clipboard::get_text_from_clipboard( wxString& output_string )
{
#if ( setupUSE_DRAG_AND_DROP ) 
    // Try to get the data from the clipboard, returning an appropriate error message
    // if was a problem somewhere along the way. Be sure to close the clipboard after
    // opening it.
    if ( ! wxTheClipboard->Open() )
    {
        wxLogError( wxT( "Error: Can't open clipboard." ) );
        return FALSE;
    }

    // DF_TEXT stands for DataFormat_Text. Looks to see whether it is text on the clipboard
    if ( ! wxTheClipboard->IsSupported( wxDF_TEXT ) ) 
    {
        wxLogWarning( wxT( "Error: No text data on clipboard" ) );
        wxTheClipboard->Close();
        return FALSE;
    }

    // Create a text data object, and get it from the clipboard.
    wxTextDataObject text_data;
    
    // Try to get it from the clipboard
    if ( ! wxTheClipboard->GetData( text_data ) ) 
    {
        wxLogError( wxT( "Error: Can't paste data from the clipboard" ) );
        wxTheClipboard->Close();
        return FALSE;
    } 
    else 
    {
        // Set the output string to the string in the text data object
        output_string = text_data.GetText();        
        wxLogDebug( wxT( "Text extracted from the clipboard=" ) + output_string );
        wxTheClipboard->Close();
        return TRUE;               
    }
    
#else   // setupUSE_DRAG_AND_DROP
    return FALSE;
#endif  // setupUSE_DRAG_AND_DROP
}

// Sets the clipboard to a string. Returns successful .
bool utils_clipboard::set_clipboard_to_text( const wxString& input_string )
{
#if ( setupUSE_DRAG_AND_DROP ) 
    // Try to get the data from the clipboard, returning an appropriate error message
    // if was a problem somewhere along the way. Be sure to close the clipboard after
    // opening it.
    if ( ! wxTheClipboard->Open() )
    {
        wxLogError( wxT( "Error: Can't open clipboard." ) );
        return FALSE;
    }
    
    // Make a new textdata object from the input string and try to it on the clipboard
    if ( ! wxTheClipboard->SetData( new wxTextDataObject( input_string ) ) )
    {
        wxLogError( wxT( "Error: Can't paste data from the clipboard" ) );
        wxTheClipboard->Close();
        return FALSE;
    } 
    else 
    {     
        wxLogDebug( wxT( "Text successfully added to clipboard" ) );
        wxTheClipboard->Close();
        return TRUE;               
    }
    
#else   // setupUSE_DRAG_AND_DROP
    return FALSE;
#endif  // setupUSE_DRAG_AND_DROP
}
