//----------------------------------------------------------------------------------------
// Name:        palm_doc2txt.cpp
// Purpose:     Converts a Palm DOC file to a text file and vice versa
// Author:      Paul J. Lucas, Christopher Heschong, Harold Bamford, Rick Bram, 
//              and Florent Pillet
// Modified by: Robert O'Connor ( Implemented for wxWindows C++ )
// Created:     1998
// Licence:     GPL
// RCS-ID:      $Id: palm_doc2txt.cpp,v 1.1 2002/04/02 22:55:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "palm_doc2txt.cpp"
    #pragma interface "palm_doc2txt.cpp"
#endif
// ---------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------
#include "wx/datetime.h"                // wxDateTime
// ---------------------------------------------------------------------------------------
#include "palm_doc2txt.h"


//----------------------------------------------------------------------------------------
// Defines and structures to describe a generic PDB database records, as currently 
// outlined by Palm, Inc. For more information, see:
// Christopher Bey and Kathleen Dupre.  "Palm File Format Specification,"
// Document Number 3008-003, Palm, Inc., May 16, 2000.
//----------------------------------------------------------------------------------------

#define dmDBNameLength  32              // 31 chars + 1 null terminator
#define RECORD_SIZE_MAX 4096            // Palms have a max 4K record size

//----------------------------------------------------------------------------------------

struct RecordEntryType
{
    PalmDWord   localChunkID;           // Offset to where record starts
    struct {
        int palm_delete   : 1;
        int dirty         : 1;
        int busy          : 1;
        int secret        : 1;
        int category      : 4;
    } attributes;
    PalmByte    uniqueID[3];
};
typedef struct RecordEntryType RecordEntryType;

// Some compilers pad structures out to PalmDWord boundaries so using sizeof() doesn't 
// give the right result. (This is also done for the rest of the structs that follow.)
#define RecordEntrySize     8

//----------------------------------------------------------------------------------------

struct RecordListType       // 6 bytes total 
{
    PalmDWord   nextRecordListID;
    PalmWord    numRecords;
};
typedef struct RecordListType RecordListType;

#define RecordListSize      6

//----------------------------------------------------------------------------------------

struct DatabaseHdrType      // 78 bytes total
{
    char            name[ dmDBNameLength ];
    PalmWord        attributes;
    PalmWord        version;
    PalmDWord       creationDate;
    PalmDWord       modificationDate;
    PalmDWord       lastBackupDate;
    PalmDWord       modificationNumber;
    PalmDWord       appInfoID;
    PalmDWord       sortInfoID;
    char            type[4];
    char            creator[4];
    PalmDWord       uniqueIDSeed;
    RecordListType  recordList;
};
typedef struct DatabaseHdrType DatabaseHdrType;

#define DatabaseHdrSize     78

//----------------------------------------------------------------------------------------

// The palm_date() is used to write the creation and modification date into the header.
// when making a DOC file record. If these dates in header are left blank, the DOC won't
// HotSync, since the manager will catch it as invalid and not install it.
// BTW, wxDateTime::GetTicks() returns number of seconds since Jan 1, 1970.
#define palm_date() (PalmDWord) ( wxDateTime::Now().GetTicks() + 2082844800ul )

//----------------------------------------------------------------------------------------
// Constants specific for palmdoc2txt.cpp
//----------------------------------------------------------------------------------------

#define BUFFER_SIZE 6000        // Big enough for uncompressed record
#define COMPRESSED  2
#define COUNT_BITS  3           // Unknown why this is 3. Must be lunar alignment related
#define DISP_BITS   11          // Ditto
#define DOC_CREATOR "REAd"
#define DOC_TYPE    "TEXt"
#define UNCOMPRESSED    1

//----------------------------------------------------------------------------------------
//  Record 0 structure: In a DOC file, contain information about the document as a whole.
//----------------------------------------------------------------------------------------

struct doc_record_zero          // 16 bytes total
{
    PalmWord    version;        // 1 = plain text, 2 = compressed
    PalmWord    reserved1;
    PalmDWord   doc_size;       // In bytes, when uncompressed
    PalmWord    num_records;    // PDB header numRecords - 1
    PalmWord    rec_size;       // Usually RECORD_SIZE_MAX
    PalmDWord   reserved2;
};
typedef struct doc_record_zero doc_record_zero;

//----------------------------------------------------------------------------------------
// Macros used in palmdoc2txt.cpp
//----------------------------------------------------------------------------------------

#define NEW_BUFFER(b)   (b)->data = (PalmByte*)malloc( (b)->len = BUFFER_SIZE )

#define GET_PalmWord(f,n) \
    { if ( fread( &n, 2, 1, f ) != 1 ) wxLogDebug( "Error: can't read" ); n = ntohs(n); }

#define GET_PalmDWord(f,n) \
    { if ( fread( &n, 4, 1, f ) != 1 ) wxLogDebug( "Error: can't read" ); n = ntohl(n); }

#define PUT_PalmWord(f,n) \
    {  PalmWord t = htons(n); if ( fwrite( &t, 2, 1, f ) != 1 ) wxLogDebug( "Error: can't write" ); }

#define PUT_PalmDWord(f,n) \
    { PalmDWord t = htonl(n); if ( fwrite( &t, 4, 1, f ) != 1 ) wxLogDebug( "Error: can't write" ); }

#define SEEK_REC_ENTRY(f,i) \
    fseek( f, DatabaseHdrSize + RecordEntrySize * (i), SEEK_SET )

//----------------------------------------------------------------------------------------
//  Constructor
//----------------------------------------------------------------------------------------

palm_doc2txt::palm_doc2txt( wxString source_filename, wxString destination_filename, 
                            bool strip_binary_characters, bool verbose_messages )                            
                            :m_source_filename( source_filename ),
                             m_destination_filename ( destination_filename ),
                             m_strip_binary_flag( strip_binary_characters ),
                             m_verbose_messages_flag ( verbose_messages )
{
    // I don't see any reason why would ever want to not compress the DOC output
    // file, so not putting an interface to it..(Plucker has one, and was eventually
    // faded in importance since never turned off compression).
    m_compress_output_doc_flag = true;
}

//----------------------------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------------------------

palm_doc2txt::~palm_doc2txt( )
{
}

//----------------------------------------------------------------------------------------
// Convert the source DOC file to a text file.
//
// PARAMETERS:
// src_file_name    The name of the Doc file.
// dest_file_name   The name of the text file.  If null, text is sent to standard output.
//----------------------------------------------------------------------------------------
#if ( USE_DOC_TO_TXT_CONVERSION_FUNCTIONALITY ) 
bool palm_doc2txt::convert_doc_to_txt()
{
    buffer          buf;
    int             compression;
    PalmDWord       file_size, offset, rec_size;
    FILE            *fin, *fout;
    DatabaseHdrType header;
    int             num_records, rec_num;
    doc_record_zero rec0;
    
    //--------- Open files, read header, ensure source is a Doc file -----

    fin = open_src_file( m_source_filename );

    wxLogDebug( "Finished opening src file" );

    if ( fread( &header, DatabaseHdrSize, 1, fin ) != 1 ||
         strncmp( header.type,    DOC_TYPE,    sizeof header.type ) ||
         strncmp( header.creator, DOC_CREATOR, sizeof header.creator )
    ) {
        wxLogDebug( "Error: %s is not a valid DOC File", m_destination_filename );
        return false;
    }
    num_records = ntohs( header.recordList.numRecords ) - 1; // w/o rec 0 
    wxLogDebug( "Number of records=%i", num_records );

    fout = m_destination_filename ? open_dest_file( m_destination_filename ) : stdout;
    wxLogDebug( "Setting output target" );

    //--------- Read record 0 --------------------------------------------

    SEEK_REC_ENTRY( fin, 0 );
    GET_PalmDWord( fin, offset );       // get offset of rec 0 
    fseek( fin, offset, SEEK_SET );
    if ( fread( &rec0, sizeof rec0, 1, fin ) != 1 ) {
        wxLogDebug( "Error: Can't read" );
        return false;
    }
    wxLogDebug( "Read record 0" );
    
    compression = ntohs( rec0.version );
    if ( compression != COMPRESSED && compression != UNCOMPRESSED ) {
        wxLogDebug( "Error: Unknown compression type %ld", compression );
        return false;
    }
    wxLogDebug( "Read compression type from record 0" );

    //--------- Read Doc file record-by-record ---------------------------

    fseek( fin, 0, SEEK_END );
    file_size = ftell( fin );

    if ( m_verbose_messages_flag ) {
        wxLogDebug( "Decoding DOC file %s", header.name );
    }

    NEW_BUFFER( &buf ); 
    for ( rec_num = 1; rec_num <= num_records; ++rec_num ) {
        PalmDWord next_offset;

        // Read the record offset
        SEEK_REC_ENTRY( fin, rec_num );
        GET_PalmDWord( fin, offset );

        // Read the next record offset to compute the record size
        if ( rec_num < num_records ) {
            SEEK_REC_ENTRY( fin, rec_num + 1 );
            GET_PalmDWord( fin, next_offset );
        } else {
            next_offset = file_size;
        }
        rec_size = next_offset - offset;

        // Read the record
        fseek( fin, offset, SEEK_SET );
        buf.len = fread( buf.data, 1, rec_size, fin );
        if ( buf.len != rec_size ) {
            wxLogDebug( "Error: Cant Read" );
            return false;
        }
        
        if ( compression == COMPRESSED ) {
            uncompress( &buf );
        }

        if ( fwrite( buf.data, buf.len, 1, fout ) != 1 ) {      
            wxLogDebug( "Error: Cant Read" );
            return false;
        }
        
        if ( m_verbose_messages_flag ) {
            wxLogDebug( "Converting records number %ld", num_records - rec_num );
        }
    }

    fclose( fin );
    fclose( fout );
    return true;
}
#endif // USE_DOC_TO_TXT_CONVERSION_FUNCTIONALITY 

//----------------------------------------------------------------------------------------
// Replace the given buffer with an uncompressed version of itself.
//
// PARAMETERS:
// b    The buffer to be uncompressed.
//----------------------------------------------------------------------------------------
#if ( USE_DOC_TO_TXT_CONVERSION_FUNCTIONALITY ) 
void palm_doc2txt::uncompress( register buffer *b )
{
    PalmByte *const new_data = (PalmByte*)malloc( BUFFER_SIZE );
    int i, j;

    for ( i = j = 0; i < (int) b->len; ) {
        register unsigned c = b->data[ i++ ];

        if ( c >= 1 && c <= 8 )
            while ( c-- )               // copy 'c' bytes
                new_data[ j++ ] = b->data[ i++ ];

        else if ( c <= 0x7F )           // 0,09-7F = self
            new_data[ j++ ] = c;

        else if ( c >= 0xC0 )           // space + ASCII char
            new_data[ j++ ] = ' ', new_data[ j++ ] = c ^ 0x80;

        else {                          // 80-BF = sequences
            register int di, n;
            c = (c << 8) + b->data[ i++ ];
            di = (c & 0x3FFF) >> COUNT_BITS;
            for ( n = (c & ((1 << COUNT_BITS) - 1)) + 3; n--; ++j )
                new_data[ j ] = new_data[ j - di ];
        }
    }
    free( b->data );
    b->data = new_data;
    b->len = j;
}
#endif // USE_DOC_TO_TXT_CONVERSION_FUNCTIONALITY 

//----------------------------------------------------------------------------------------
// Encode the source text file into a Palm DOC file.
// All of the Palm DB flags in the header are off (backup, beam). Only the required 
// things are written into the header.
// 
// PARAMETERS:
// document_name    The name of the document as it is to appear in a Documents List view of
//                  a Doc reader application on the Palm.
// src_file_name    The name of the text file.
// dest_file_name   The name of the Doc file.
//----------------------------------------------------------------------------------------
#if ( USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY )
bool palm_doc2txt::convert_txt_to_doc( wxString title_on_palm )
{
    PalmDWord           date;
    FILE                *fin, *fout;
    PalmDWord           file_size;
    DatabaseHdrType     header;
    doc_record_zero     rec0;
    buffer              buf;
    int                 num_records, rec_num;
    PalmDWord           num_offsets, offset;
    unsigned long       index;
    int                 total_before, total_after;
    char const          *document_name;
    
    document_name = title_on_palm;

    fin  = open_src_file ( m_source_filename  );
    fout = open_dest_file( m_destination_filename );

    fseek( fin, 0, SEEK_END );
    file_size = ftell( fin );
    num_records = file_size / RECORD_SIZE_MAX;
    if ( (long) num_records * RECORD_SIZE_MAX < (long) file_size )
        ++num_records;

    //--------- Create and write header ---------------------------------
#if ( USE_BZERO_FUNCTION )
    bzero( header.name, sizeof header.name );
#else
    //memset( header.name, 0, sizeof header.name );
    for ( int zz=0; zz < sizeof header.name; zz++ ) {
        header.name[ zz ] = 0;
    }
#endif    

    strncpy( header.name, document_name, sizeof header.name - 1 );
    if ( strlen( document_name ) > sizeof header.name - 1 )  {
        strncpy( header.name + sizeof header.name - 4, "...", 3 );
    }
    header.attributes                   = 0;
    header.version                      = 0;
    date = htonl( palm_date() );
    memcpy( &header.creationDate,       &date, 4 );
    date = htonl( palm_date() );
    memcpy( &header.modificationDate,   &date, 4 );
    header.lastBackupDate               = 0;
    header.modificationNumber           = 0;
    header.appInfoID                    = 0;
    header.sortInfoID                   = 0;
    strncpy( header.type,    DOC_TYPE,    sizeof header.type );
    strncpy( header.creator, DOC_CREATOR, sizeof header.creator );
    header.uniqueIDSeed                 = 0;
    header.recordList.nextRecordListID  = 0;
    header.recordList.numRecords        = htons( num_records + 1 );
                                         // +1 for rec 0

    fseek( fin, 0, SEEK_SET );
    if ( fwrite( &header, DatabaseHdrSize, 1, fout ) != 1 ) {
        wxLogDebug( "Error: Cant write database header" );
        return false;
    }

    //-------- Write record offsets -------------------------------------

    num_offsets = num_records + 1;      // +1 for rec 0 
    offset = DatabaseHdrSize + RecordEntrySize * num_offsets;
    index = 0x40 << 24 | 0x6F8000;      // Dirty + unique ID

    PUT_PalmDWord( fout, offset );      // Offset for rec 0
    PUT_PalmDWord( fout, index++ );

    while( --num_offsets ) {
        PUT_PalmDWord( fout, 0 );       // Placeholder
        PUT_PalmDWord( fout, index++ );
    }

    //--------- Write record 0 -------------------------------------------

    rec0.version        = htons( m_compress_output_doc_flag + 1 );
    rec0.reserved1      = 0;
    rec0.doc_size       = htonl( file_size );
    rec0.num_records    = htons( num_records );
    rec0.rec_size       = htons( RECORD_SIZE_MAX );
    rec0.reserved2      = 0;

    if ( fwrite( &rec0, sizeof rec0, 1, fout ) != 1 ) {
        wxLogDebug( "Error: Unable to write to zero record" );
        return false;
    }

    //--------- Write text ---------------------------------------------

    NEW_BUFFER( &buf );
    total_before = total_after = 0;
    for ( rec_num = 1; rec_num <= num_records; ++rec_num ) {
        int bytes_read;

        offset = ftell( fout );
        SEEK_REC_ENTRY( fout, rec_num );
        PUT_PalmDWord( fout, offset );

        bytes_read = fread( buf.data, 1, RECORD_SIZE_MAX, fin );       
        if ( bytes_read == 0 ) {
            break;
        }
    
        if ( ferror( fin ) ) {
            wxLogDebug( "Error: unable to read" );
            return false;
        }
        
        buf.len = bytes_read;

        if ( m_strip_binary_flag ) {
            remove_binary( &buf );
        }
        
        if ( m_compress_output_doc_flag ) {
            compress( &buf );
        }
        
        fseek( fout, offset, SEEK_SET );
        if ( fwrite( buf.data, buf.len, 1, fout ) != 1 ){
            wxLogDebug( "Error: unable to write" );
            return false;
        }

        if ( !m_verbose_messages_flag ) {
            continue;
        }

        // These byte_read report as 0.0000 in the wxLogDebug. Don't know why
        if ( m_compress_output_doc_flag ) {
            wxLogDebug( "Record %2d: %5f bytes -> %5d (%2d%%)",
                        rec_num, bytes_read, buf.len,
                        (int)( 100.0 * buf.len / bytes_read ) );
            total_before += bytes_read;
            total_after  += buf.len;
        } else
            wxLogDebug( " %d", num_records - rec_num + 1 );
    }
    if ( m_verbose_messages_flag )
        if ( m_compress_output_doc_flag ) {
            wxLogDebug( "-------------------------\nCompression summary: %2d%%",
                       (int)( 100.0 * total_after / total_before ) ); 
        }

    fclose( fin );
    fclose( fout );
    return true;
}
#endif  // USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY 


//----------------------------------------------------------------------------------------
// Put a byte into a buffer.
//
// PARAMETERS:
// b        The buffer to be affected.
// c        The byte.
// space    Is it a space?
//----------------------------------------------------------------------------------------
#if ( USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY )
void palm_doc2txt::put_byte( register buffer *b, PalmByte c, bool *space )
{

    if ( *space ) {
        *space = false;
        //
        // There is an outstanding space char: see if we can squeeze it
        // in with an ASCII char.
        //
        if ( c >= 0x40 && c <= 0x7F ) {
            b->data[ b->len++ ] = c ^ 0x80;
            return;
        }
        b->data[ b->len++ ] = ' ';  // couldn't squeeze it in
    } else if ( c == ' ' ) {
        *space = true;
        return;
    }

    if ( c >= 1 && c <= 8 || c >= 0x80 )
        b->data[ b->len++ ] = '\1';

    b->data[ b->len++ ] = c;
}
#endif // USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY

//----------------------------------------------------------------------------------------
// Replace the given buffer with one that has had ASCII characters (0-8) removed and 
// carriage-returns and form-feeds converted to newlines.
//
// PARAMETERS:
// b    The buffer to be affected.
//----------------------------------------------------------------------------------------
#if ( USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY )
void palm_doc2txt::remove_binary( register buffer *b )
{
    register PalmByte *const new_data = (PalmByte*)malloc( b->len );
    int i, j;

    for ( i = j = 0; i < (int) b->len; ++i ) {
        if ( b->data[ i ] < 9 )     // Discard really low ASCII
            continue;
        switch ( b->data[ i ] ) {

            case '\r':
                if ( i < (int) b->len - 1 && b->data[ i+1 ] == '\n' )
                    continue;   // CR+LF -> LF
                // No break;

            case '\f':
                new_data[ j ] = '\n';
                break;

            default:
                new_data[ j ] = b->data[ i ];
        }
        ++j;
    }
    free( b->data );
    b->data = new_data;
    b->len = j;
}
#endif   // USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY


//----------------------------------------------------------------------------------------
// Replace the given buffer with a compressed version of itself. 
//
// PARAMETERS:
// b    The buffer to be compressed.
//----------------------------------------------------------------------------------------
#if ( USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY )
void palm_doc2txt::compress( buffer *b )
{
    int i, j;
    bool space = false;

    PalmByte *buf_orig;
    PalmByte *p;            // Walking test hit; works up on successive matches
    PalmByte *p_prev;
    PalmByte *head;         // Current test string
    PalmByte *tail;         // 1 past the current test buffer
    PalmByte *end;          // 1 past the end of the input buffer

    p = p_prev = head = buf_orig = b->data;
    tail = head + 1;
    end = b->data + b->len;

    NEW_BUFFER( b );
    b->len = 0;

    // Loop, absorbing one more char from the input buffer on each pass
    while ( head != end ) {
        // Establish where the scan can begin
        if ( head - p_prev > (( 1 << DISP_BITS )-1) )
            p_prev = head - (( 1 << DISP_BITS )-1);

        // Scan in the previous data for a match
        p = mem_find( p_prev, tail - p_prev, head, tail - head );

        // On a mismatch or end of buffer, issued codes
        if ( !p || p == head || tail - head > ( 1 << COUNT_BITS ) + 2
            || tail == end
        ) {
            // Issued the codes
            // First, check for short runs
            if ( tail - head < 4 )
                put_byte( b, *head++, &space );
            else {
                unsigned dist = head - p_prev;
                unsigned compound = (dist << COUNT_BITS)
                    + tail - head - 4;

                if ( dist >= ( 1 << DISP_BITS ) || tail - head - 4 > 7 ) {
                    wxLogDebug( "Error: dist overflow" );
                }

                // For longer runs, issue a run-code
                // issue space char if required 
                if ( space ) {
                    b->data[ b->len++ ] = ' ';
                    space = false;
                }

                b->data[ b->len++ ] = 0x80 + ( compound >> 8 );
                b->data[ b->len++ ] = compound & 0xFF;
                head = tail - 1;    // And start again
            }
            p_prev = buf_orig;      // Start search again
        } else
            p_prev = p;             // Got a match

        // When we get to the end of the buffer, don't inc past the
        // end; this forces the residue chars out one at a time
        if ( tail != end )
            ++tail;
    }
    free( buf_orig );

    if ( space )
        b->data[ b->len++ ] = ' ';  // Add left-over space

    // Final scan to merge consecutive high chars together
    for ( i = j = 0;  i < (int)b->len; ++i, ++j ) {
        b->data[ j ] = b->data[ i ];

        // Skip run-length codes
        if ( b->data[ j ] >= 0x80 && b->data[ j ] < 0xC0 )
            b->data[ ++j ] = b->data[ ++i ];

        // If we hit a high char marker, look ahead for another
        else if ( b->data[ j ] == '\1' ) {
            b->data[ j + 1 ] = b->data[ i + 1 ];
            while ( i + 2 < (int)b->len &&
                b->data[ i + 2 ] == 1 && b->data[ j ] < 8
            ) {
                b->data[ j ]++;
                b->data[ j + b->data[ j ] ] = b->data[ i + 3 ];
                i += 2;
            }
            j += b->data[ j ];
            ++i;
        }
    }
    b->len = j;
}
#endif   // USE_TXT_TO_DOC_CONVERSION_FUNCTIONALITY

//----------------------------------------------------------------------------------------
// Miscellaneous functions
//----------------------------------------------------------------------------------------

// Replacement for strstr() which deals with 0's in the data. Used in compression
PalmByte* palm_doc2txt::mem_find( register PalmByte *t, int t_len, register PalmByte *m, int m_len ) {
    register int i;
    for ( i = t_len - m_len + 1; i > 0; --i, ++t )
        if ( *t == *m && !memcmp( t, m, m_len ) )
            return t;
    return 0;
}

FILE* palm_doc2txt::open_src_file( char const *file_name ) {
    FILE *f = fopen( file_name, "rb" );
    if ( f ) {
        return f;
    } else {
        wxLogDebug( "Error: can not open source file %s for input", file_name );
        return NULL;
    }
}

FILE* palm_doc2txt::open_dest_file( char const *file_name ) {
    FILE *f = fopen( file_name, "wb" );
    if ( f ) {
        return f;
    } else {
        wxLogDebug( "Error: can not open output file %s for output", file_name ); 
        return NULL;
    }
}
