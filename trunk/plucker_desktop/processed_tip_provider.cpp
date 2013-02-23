//----------------------------------------------------------------------------------------
// Name:        processed_tip_provider.cpp
// Purpose:     Add/edit a blocked string for exclusion lists dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2003/03/04
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: processed_tip_provider.cpp,v 1.7 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "processed_tip_provider.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_STARTUP_TIPS )

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

#include "processed_tip_provider.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "configuration.h"
#include "utils_string.h"

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

processed_tip_provider::processed_tip_provider( const wxString& filename,
                                                size_t current_tip )
                 : wxTipProvider( current_tip ), m_textfile( filename )
{
    m_textfile.Open();

    // Set up our array of filters for the tips.
    wxString tips_exclusion_filter_string =
        the_configuration->Read( wxT( "/PLUCKER_DESKTOP/branding_tips_exclusion_filter"), wxT( "" ) );

    utils_string::string_to_arraystring( tips_exclusion_filter_string, ';', &m_tips_exclusion_filter_array, TRUE );
}


processed_tip_provider::~processed_tip_provider()
{
    wxLogDebug( _("Entering process_tip_provider destructor" ));
    // Clear memory
    m_tips_exclusion_filter_array.Clear();
    wxLogDebug( _("Entering process_tip_provider destructor" ));
}


// A pointless rehash from wx/src/generic/tipdlg.cpp [which I also wrote, and am
// unsatsified that I am not allowed to derive from wxFileTipProvider directly]
wxString processed_tip_provider::GetTip()
{
    size_t count = m_textfile.GetLineCount();
    if ( !count ) 
    {
        return _( "Tips not available, sorry!" );
    }

    wxString tip;

    // Comments start with a # symbol.
    // Loop reading lines until get the first one that isn't a comment.
    // The max number of loop executions is the number of lines in the
    // textfile so that can't go into an eternal loop in the [oddball]
    // case of a comment-only tips file, or the developer has vetoed
    // them all via PreprecessTip().
    for ( size_t i=0; i < count; i++ ) 
    {
        // The current tip may be at the last line of the textfile, (or
        // past it, if the number of lines in the textfile changed, such
        // as changing to a different textfile, with less tips). So check
        // to see at last line of text file, (or past it)...
        if ( m_currentTip >= count ) 
        {
            // .. and if so, wrap back to line 0.
            m_currentTip = 0;
        }

        // Read the tip, and increment the current tip counter.
        tip = m_textfile.GetLine( m_currentTip++ );

        // Now do the preprocessing.
        tip = PreprocessTip( tip );

        // Break if tip isn't a comment, and isn't an empty string
        // (or only stray space characters).
        if ( ! tip.StartsWith( wxT( "#" ) ) && ( tip.Trim() != wxEmptyString ) )
        {
            break;
        }
    }

    // If tip starts with '_(', then it is a gettext string of format
    // _("My \"global\" tip text") so first strip off the leading '_("'...
    if ( tip.StartsWith( wxT( "_(\"" ), &tip ) )
    {
        //...and strip off the trailing '")'...
        tip = tip.BeforeLast( wxT( '\"' ) );
        // ...and replace escaped quotes
        tip.Replace( wxT( "\\\"" ), wxT( "\"" ) );
    }

    // Now do the postprocessing after the translation is through
    tip = PostprocessTip( tip );

    return tip;
}


wxString processed_tip_provider::PreprocessTip( const wxString &tip )
{
    wxString processed_tip = tip;

    size_t i;
    // Check to see if it fits the criteria of our filter of tips that we want to
    // show. Loop through our array of exclusion filters...
    for ( i=0; (int)i < (int)m_tips_exclusion_filter_array.GetCount(); i++ ) 
    {
        // ... Checking to see if tip contains the current iteration filter...
        if ( processed_tip.Contains( m_tips_exclusion_filter_array.Item( i ) ) )
        {
            // ...and if so, returning wxEmptyString will skip this tip.
            processed_tip = wxT( "" );
            break;
        }
    }

    return processed_tip;
}


wxString processed_tip_provider::PostprocessTip( const wxString &tip )
{
    // Can't modify a const wxString, now son.
    wxString processed_tip = tip;
    // Do some runtime replacements.
    // It has to be done here since want after translation.
    processed_tip.Replace( wxT( "plkrAPPLICATION_DESKTOP_NAME" ), plkrAPPLICATION_DESKTOP_NAME );
    processed_tip.Replace( wxT( "plkrAPPLICATION_SUITE_NAME" ), plkrAPPLICATION_SUITE_NAME );
    processed_tip.Replace( wxT( "plkrAPPLICATION_PUBLISHER" ), plkrAPPLICATION_PUBLISHER );

    //! \todo strip out the html from the content if the html tip window is compiled out.

    return processed_tip;
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_STARTUP_TIPS
