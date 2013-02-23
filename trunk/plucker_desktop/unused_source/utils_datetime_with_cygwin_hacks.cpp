//----------------------------------------------------------------------------------------
// Name:        utils_datetime.cpp
// Purpose:     Utils for date and time
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/12/28
// Copyright:   (c) Robert O'Connor ( rob@medicalmnemonics.com )
// Licence:     GPL
// RCS-ID:      $Id: utils_datetime_with_cygwin_hacks.cpp,v 1.1 2002/07/16 14:30:07 robertoconnor Exp $
//----------------------------------------------------------------------------------------

// Many things in time in Cygwin won't work, when parsing a date. It adds or subtracts hours 
// for timezones when parsing a date, and most attempts to stop this won't work: 
// 
// output_datetime.ParseDateTime( output_datetime_string );
// wxLogDebug( "output_datetime parsed by ParseDateTime is=%s", output_datetime.Format().c_str() );
// output_datetime.ParseRfc822Date( output_datetime_string );
// wxLogDebug( "output_datetime parsed by ParseRfc822Date is=%s", output_datetime.Format().c_str() );
// 
// Tm objects won't work either, in correcting for timezone: 
// wxDateTime::Tm tm_representation;
// tm_representation = output_datetime.GetTm( 13 );
// wxDateTime final_date_time( tm_representation );
// wxLogDebug( "final_date_time is=%s", final_date_time.Format().c_str() );
// 
// MakeGMT() does nothing and ToGMT() makes a screwy date
// output_datetime.MakeGMT( FALSE );
// wxLogDebug( "Cygwin adjustment: after MakeGMT(), output_datetime=%s", output_datetime.Format().c_str() );
// output_datetime.ToGMT( FALSE );
// wxLogDebug( "Cygwin adjustment: after ToGMT(), output_datetime=%s", output_datetime.Format().c_str() );
// 
// Cygwin can extract the timezone with this, but the answer is difficult to understand:
//    tzset();
//    wxLogDebug( "_timezone: %d", _timezone );  


// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "utils_datetime.cpp"
    #pragma interface "utils_datetime.cpp"
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
#include "plucker_defines.h"
#include "utils_datetime.h"

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Converts a W3C datetime string to a wxDateTime.
// Returns now if it was invalid.
wxDateTime utils_datetime::w3c_string_to_datetime( wxString input_w3c_string )
{ 
    wxLogDebug( "W3C string to convert to datetime is" + input_w3c_string );
    
    // Handle for case of passed an empty string
    if ( input_w3c_string == "" ) {
        wxDateTime empty_datetime = wxDateTime::Now();
        input_w3c_string = datetime_to_w3c_string( empty_datetime );
    }
    
    wxDateTime parsed_datetime;
    wxDateTime output_datetime;   
   
    parsed_datetime.ParseFormat( input_w3c_string, wxT( plkrDATE_TIME_FORMAT ) );
    wxLogDebug( "parsed_datetime as parsed by ParseFormat=%s",  parsed_datetime.Format().c_str() );
    
#if ( setupUSE_CYGWIN_GMT_ADJUSTMENT ) 
    wxTimeSpan timezone_offset = get_timezone_offset( parsed_datetime );
    output_datetime = parsed_datetime;
    output_datetime += ( timezone_offset );  
    wxLogDebug( "output_datetime after timezone_offset=%s", output_datetime.Format().c_str() );
#else
    output_datetime = parsed_datetime;
#endif

    if ( ! output_datetime.IsValid() ) {
        output_datetime = wxDateTime::Now();
    } 

    return output_datetime;
}


// Takes a wxDateTime string and converts it to a an W3C datetime.
// Uses Now if it was invalid
wxString utils_datetime::datetime_to_w3c_string( wxDateTime input_datetime )
{
    if ( ! input_datetime.IsValid() ) {
        input_datetime = wxDateTime::Now();
    } 

    wxLogDebug( "DateTime to convert to W3C string=%s", input_datetime.Format().c_str() );
    
#if ( setupUSE_CYGWIN_GMT_ADJUSTMENT ) 
    wxTimeSpan timezone_offset = get_timezone_offset( input_datetime );
    input_datetime += ( timezone_offset );  
    wxLogDebug( "input_datetime after timezone_offset=%s", input_datetime.Format().c_str() );
#endif

    wxString output_datetime_string;
    output_datetime_string = input_datetime.Format( wxT( plkrDATE_TIME_FORMAT ) ).c_str();
    wxLogDebug( "output_datetime_string as formatted by Format=%s",  input_datetime.Format().c_str() );

    return output_datetime_string;
}


// Converts a W3C "10:30:00" time string to a wxDateTime.
// Returns now if was invalid
wxDateTime utils_datetime::w3c_string_to_time( wxString input_w3c_string )
{
    wxLogDebug( "W3C string to convert to datetime is" + input_w3c_string );
    
        // Handle for case of passed an empty string
    if ( input_w3c_string == "" ) {
        wxDateTime empty_datetime = wxDateTime::Now();
        input_w3c_string = time_to_w3c_string( empty_datetime );
    }
    
    wxDateTime parsed_time;
    wxDateTime output_time;
   
    //parsed_time.ParseFormat( input_w3c_string, _( "%H:%M:%S" ) );
    parsed_time.ParseTime( input_w3c_string );
    wxLogDebug( "parsed_time as parsed by ParseFormat=%s",  parsed_time.Format().c_str() );
    
#if ( setupUSE_CYGWIN_GMT_ADJUSTMENT ) 
    wxTimeSpan timezone_offset = get_timezone_offset( parsed_time );
    output_time = parsed_time;
    output_time += ( timezone_offset );  
    wxLogDebug( "output_time after timezone_offset=%s", output_time.Format().c_str() );
#else
    output_time = parsed_time;
#endif

    if ( ! output_time.IsValid() ) {
        output_time = wxDateTime::Now();
    }    

    return output_time;
}


// Takes a wxDateTime string and converts it to a an W3C time
wxString utils_datetime::time_to_w3c_string( wxDateTime input_time )
{
    wxLogDebug( "Time to convert to W3C string=%s", input_time.Format().c_str() );
    
    if ( ! input_time.IsValid() ) {
        input_time = wxDateTime::Now();
    } 

    
#if ( setupUSE_CYGWIN_GMT_ADJUSTMENT ) 
    wxTimeSpan timezone_offset = get_timezone_offset( input_time );
    input_time += ( timezone_offset );  
    wxLogDebug( "input_time after timezone_offset=%s", input_time.Format().c_str() );
#endif

    wxString output_time_string;
    output_time_string = input_time.Format( wxT( "%H:%M:%S" ) ).c_str();
    wxLogDebug( "output_time_string as formatted by Format=%s",  input_time.Format().c_str() );

    return output_time_string;
}


// Converts a datetime to the timezone converted datetime.
wxDateTime utils_datetime::to_timezone_corrected( wxDateTime input_datetime )
{
 wxDateTime output_datetime = input_datetime;

#if ( setupUSE_CYGWIN_GMT_ADJUSTMENT ) 
    wxTimeSpan timezone_offset;
    timezone_offset = get_timezone_offset( input_datetime );
    output_datetime += ( timezone_offset );  
#endif

    return output_datetime;
}


// Returns the hours/minutes offset from GMT of the local computer.
wxTimeSpan utils_datetime::get_timezone_offset( wxDateTime input_datetime )
{
#if ( setupUSE_CYGWIN_GMT_ADJUSTMENT ) 
    wxLogDebug( "Parsed datetime is=%s", input_datetime.Format().c_str() );
    
    wxString parsed_string;
    wxDateTime second_input_datetime;
    wxTimeSpan timezone_offset; 

    parsed_string = input_datetime.Format( wxT( plkrDATE_TIME_FORMAT ) ).c_str();
    wxLogDebug( "parsed_string=" + parsed_string );
    second_input_datetime.ParseFormat( parsed_string, wxT( plkrDATE_TIME_FORMAT ) );
    wxLogDebug( "second_input_datetime=%s", second_input_datetime.Format().c_str() );
    
    timezone_offset = ( input_datetime - second_input_datetime );

    if ( input_datetime.IsDST() ) {
        wxTimeSpan dst_hour_offset( 1,0,0,0 );
        // TODO: only apply if in DST and today isnt DST?
        timezone_offset -= dst_hour_offset;
    }
    
    return timezone_offset;
#else
    return ( 0,0,0,0 );
#endif    
}


// If don't care about what day it was time, can use this
wxTimeSpan utils_datetime::get_timezone_offset()
{
#if ( setupUSE_CYGWIN_GMT_ADJUSTMENT ) 
    wxDateTime current_datetime;
    current_datetime = wxDateTime::Now();
    wxTimeSpan timezone_offset;
    timezone_offset = get_timezone_offset( current_datetime );
    return timezone_offset;
#else
    return ( 0,0,0,0 );
#endif    
}


wxDateTime utils_datetime::get_current_datetime()
{ 
    wxDateTime current_datetime = wxDateTime::Now();
    wxLogDebug( "Now is=%s", current_datetime.Format().c_str() );

#if ( setupUSE_CYGWIN_GMT_ADJUSTMENT ) 
    wxTimeSpan timezone_offset = get_timezone_offset();
    current_datetime += ( timezone_offset );
#endif
    return current_datetime;
}


