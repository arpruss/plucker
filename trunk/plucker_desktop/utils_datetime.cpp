//----------------------------------------------------------------------------------------
// Name:        utils_datetime.cpp
// Purpose:     Utils for date and time
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/12/28
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: utils_datetime.cpp,v 1.18 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------
    
//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "utils_datetime.h"
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

#include "utils_datetime.h"

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

wxDateTime utils_datetime::w3c_string_to_datetime( wxString input_w3c_string )
{ 
    wxLogDebug( wxT( "W3C string to convert to datetime=" ) + input_w3c_string );
    
    wxString debug_string;
    debug_string << wxT( "In quotes, the input_wx3_string is now" ) <<
        wxT( "\'" ) << input_w3c_string << wxT( "\'" );
    wxLogDebug( debug_string );
    wxDateTime parsed_datetime;        
    
    // Handle for case of passed an empty string
    if ( input_w3c_string == wxT( "" ) )
    {
        wxLogDebug( wxT( "It is an empty string, so making a W3C string using Now() instead" ) );
        wxDateTime empty_datetime = wxDateTime::Now();
        input_w3c_string = datetime_to_w3c_string( empty_datetime );
    }

#if ( setupUSE_DATETIME_MANUAL_PARSE_HACK )
    
    // The debug version of wxGTK 2.3.2 won't ParseFormat our format. However, it
    // will format a date-alone component and a time-alone component. So process
    // them separately and join them up at the end.
    wxLogDebug( wxT( "Dividing datetime into components..." ) );
    
    wxString    date_component_string;
    wxString    time_component_string;    
    wxDateTime  parsed_date;    
    wxDateTime  parsed_time;    
  
    date_component_string = input_w3c_string.BeforeFirst( 'T' );
    wxLogDebug( wxT( "date_component string=" ) + date_component_string );
    parsed_date           = w3c_string_to_date( date_component_string );
    
    time_component_string = input_w3c_string.AfterFirst( 'T' );
    wxLogDebug( wxT( "time_component string=" ) + time_component_string );
    parsed_time           = w3c_string_to_time( time_component_string );
    
    // Set the datetime's date frrm the date component
    parsed_datetime       = parsed_date;
    // Now set datetime's time from the the hours, minutes, seconds of the time component
    parsed_datetime.SetHour( parsed_time.GetHour() );
    parsed_datetime.SetMinute( parsed_time.GetMinute() );
    parsed_datetime.SetSecond( parsed_time.GetSecond() );       
    
    wxLogDebug( wxT( "parsed_datetime as manually parsed=%s" ),  parsed_datetime.Format().c_str() );

#else   // setupUSE_DATETIME_MANUAL_PARSE_HACK

    // Use the proper ParseFormat() function
    parsed_datetime.ParseFormat( input_w3c_string, wxT( plkrDATE_TIME_FORMAT ) );    
    wxLogDebug( wxT( "parsed_datetime as parsed by ParseFormat=%s" ),  parsed_datetime.Format().c_str() );

#endif  // setupUSE_DATETIME_MANUAL_PARSE_HACK
      
    
    if ( ! parsed_datetime.IsValid() ) 
    {
        wxLogDebug( wxT( "Parsed datetime was found to be invalid, so setting to Now()" ) );
        parsed_datetime = wxDateTime::Now();
    } 

    return parsed_datetime;
}


wxString utils_datetime::datetime_to_w3c_string( wxDateTime input_datetime )
{
    if ( ! input_datetime.IsValid() ) 
    {
        input_datetime = wxDateTime::Now();
    } 

    wxLogDebug( wxT( "DateTime to convert to W3C string=%s" ), input_datetime.Format().c_str() );
    
    wxString output_datetime_string;
    output_datetime_string = input_datetime.Format( plkrDATE_TIME_FORMAT ).c_str();
    wxLogDebug( wxT( "output_datetime_string as formatted by Format=%s" ),  input_datetime.Format().c_str() );

    return output_datetime_string;
}


wxDateTime utils_datetime::w3c_string_to_date( wxString input_w3c_string )
{
    wxLogDebug( wxT( "W3C string to convert to a datetime date=" ) + input_w3c_string );
    
    // Handle for case of passed an empty string
    if ( input_w3c_string == wxT( "" ) ) 
    {
        wxDateTime empty_datetime = wxDateTime::Now();
        input_w3c_string = time_to_w3c_string( empty_datetime );
    }
    
    wxDateTime parsed_date;
   
    // wxGTK wx2.3.2: ParseDate() fails when try it. It reports: ParseDate: no
    // day, no weekday hence no date    
    //parsed_date.ParseFormat( input_w3c_string, wxT( plkrDATE_FORMAT ) );
    parsed_date.ParseDate( input_w3c_string );

    wxLogDebug( wxT( "parsed_date as parsed by ParseFormat%s" ),  parsed_date.Format().c_str() );
    
    if ( ! parsed_date.IsValid() ) 
    {
        parsed_date = wxDateTime::Now();
        wxLogDebug( wxT( "parsed_date is invalid. Converting to Now()" ) );
    }    

    return parsed_date;
}


wxString utils_datetime::date_to_w3c_string( wxDateTime input_date )
{
    wxLogDebug( wxT( "Date to convert to W3C string=%s" ), input_date.Format().c_str() );
    
    if ( ! input_date.IsValid() )
    {
        input_date = wxDateTime::Now();
        wxLogDebug( wxT( "parsed_date is invalid. Converting to Now()" ) );
    } 

    wxString output_date_string;
    output_date_string = input_date.Format( plkrDATE_FORMAT ).c_str();
    wxLogDebug( wxT( "output_date_string as formatted by Format=%s" ),  input_date.Format().c_str() );

    return output_date_string;
}


wxDateTime utils_datetime::w3c_string_to_time( wxString input_w3c_string )
{
    wxLogDebug( wxT( "W3C string to convert to a datetime time=" ) + input_w3c_string );
    
    // Handle for case of passed an empty string
    if ( input_w3c_string == wxT( "" ) ) 
    {
        wxDateTime empty_datetime = wxDateTime::Now();
        input_w3c_string = time_to_w3c_string( empty_datetime );
    }
    
    wxDateTime parsed_time;

    // In jul23 CVS GTK anyways, asserts that times won't parse when the local is other 
    // than english, for some reason. However, it does convert properly.
#if ( setupUSE_TIME_MANUAL_PARSE_HACK )

    parsed_time.ParseFormat( input_w3c_string , plkrTIME_FORMAT );
    wxLogDebug( wxT( "parsed_time as parsed by ParseFormat=%s" ),  parsed_time.Format().c_str() );

#else   

    parsed_time.ParseTime( input_w3c_string );
    wxLogDebug( wxT( "parsed_time as parsed by ParseTime=%s" ),  parsed_time.Format().c_str() );

#endif    

    if ( ! parsed_time.IsValid() )
    {
        parsed_time = wxDateTime::Now();
        wxLogDebug( wxT( "parsed_time is invalid. Converting to Now()" ) );
    }    

    return parsed_time;
}


wxString utils_datetime::time_to_w3c_string( wxDateTime input_time )
{
    wxLogDebug( wxT( "Time to convert to W3C string=%s" ), input_time.Format().c_str() );
    
    if ( ! input_time.IsValid() ) 
    {
        input_time = wxDateTime::Now();
    } 

    wxString output_time_string;
    output_time_string = input_time.Format( plkrTIME_FORMAT ).c_str();
    wxLogDebug( wxT( "output_time_string as formatted by Format=%s" ),  input_time.Format().c_str() );

    return output_time_string;
}


wxDateTime utils_datetime::get_current_datetime()
{ 
    wxDateTime current_datetime = wxDateTime::Now();
    wxLogDebug( wxT( "Now is=%s" ), current_datetime.Format().c_str() );

    return current_datetime;
}

wxString utils_datetime::get_translated_compilation_date_string()
{
    // Update the build date. Would like to do some fancy footwork here, so that we can 
    // translate the month too., but doesn't seem to work. Not in catalog?
    //! \todo Figure out why this doesn't translate the month.
    // Get raw date.
    wxString untranslated_date = wxT( __DATE__ );
    // The _() forces a translation.
    wxString month = untranslated_date.BeforeFirst( ' ' );
    // Rebuild string.
    wxString translated_date;
    translated_date << month << wxT( " " ) << untranslated_date.AfterFirst( ' ' );
    
    return translated_date;
}    
