//----------------------------------------------------------------------------------------
// Name:        palm_doc2txt.h
// Purpose:     Converts a Palm DOC file to a text file and vice versa
// Author:      Paul J. Lucas, Christopher Heschong, Harold Bamford, Rick Bram, 
//              and Florent Pillet
// Modified by: Robert O'Connor ( Implemented for wxWindows C++ )
// Created:     1998
// Licence:     GPL
// RCS-ID:      $Id: palm_doc2txt.h,v 1.1 2002/04/02 22:55:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

#ifndef PALM_DOC2TXT_H
#define PALM_DOC2TXT_H

//----------------------------------------------------------------------------------------
//  START of customization of palmdoc2txt.cpp compilation
//----------------------------------------------------------------------------------------
// Can optionally set one of these to zero to remove the txt->doc functionality
// (if only want to do doc->txt conversions ) or remove the doc->txt functionality 
// (if only want to do txt->doc conversions ), and want to reduce the size of your 
// executable by not using the conversion you don't need.
#define USE_DOC_TO_TXT_CONVERSION_FUNCTIONALITY   1
#define USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY   1

// Whether or not to use bzero(). Set to 0 if your compiler doesn't recognize bzero().
// [ bzero() is a function that initially fills a char array with zeros. It is the 
// succcessor to memset(). If define USE_BZERO_FUNCTION 0, a loop is used instead 
// ( but probably could be memset ].
#define USE_BZERO_FUNCTION                        0
//----------------------------------------------------------------------------------------
//  END of customization of palmdoc2txt.cpp compilation
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
//  Defines used in class description
//----------------------------------------------------------------------------------------

// Define integral types PalmByte, PalmWord, and PalmDWord to match those on the Palm,
// which are 8, 16, and 32 bits, respectively.
typedef unsigned char  PalmByte;
typedef unsigned short PalmWord;
typedef unsigned long  PalmDWord;

typedef struct {
    PalmByte    *data;
    unsigned len;
} buffer;

//----------------------------------------------------------------------------------------
//  Define the class
//----------------------------------------------------------------------------------------

class palm_doc2txt 
{
public:
    // Constructor
    palm_doc2txt( wxString source_filename, wxString destination_filename, 
                  bool strip_binary_characters, bool verbose_messages );
    // Destructor
    ~palm_doc2txt();

    #if ( USE_DOC_TO_TXT_CONVERSION_FUNCTIONALITY ) 
        // Convert a doc file to a txt file. Returns success of the operation.
        bool convert_doc_to_txt();
    #endif 

    #if ( USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY ) 
        // Convert a doc file to a txt file. Returns success of the operation.
        bool convert_txt_to_doc( wxString title_on_palm );
    #endif
    
private:
    #if ( USE_DOC_TO_TXT_CONVERSION_FUNCTIONALITY ) 
        void        uncompress( buffer* );
    #endif

    #if ( USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY )
        void        compress( buffer *b );
        void        remove_binary( buffer* );
        void        put_byte( buffer*, PalmByte, bool *space );
        PalmByte*   mem_find( PalmByte *t, int t_len, PalmByte *m, int m_len );
    #endif

    FILE*       open_src_file ( char const *file_name );
    FILE*       open_dest_file( char const *file_name );

    bool        m_strip_binary_flag;
    bool        m_verbose_messages_flag;
    bool        m_compress_output_doc_flag;
    char const *m_source_filename;
    char const *m_destination_filename;
};


#endif  // PALM_DOC2TXT_H
