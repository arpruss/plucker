//----------------------------------------------------------------------------------------
// Name:        channel_type.cpp
// Purpose:     A class to hold a description of a channel
// Author:      Robert O'Connor
// Modified by:
// Created:     2002/01/03
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: channel_type.cpp,v 1.12 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "channel_type.h"
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

#include "channel_type.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "utils_datetime.h"

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( channel_type, wxObject )

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

// Constructor
// Note there are more values than this, these are just ones that get initialized to
// something other than the default.
channel_type::channel_type( bool initialize_to_illegal_values )
{
    wxLogDebug( wxT( "Entering a channel_type's constructor" ) );

    long initial_long = 0;
    if ( initialize_to_illegal_values )
    {
        initial_long = -1;
    }
    
    depth_first                 = initial_long;
    verbosity                   = initial_long;
    wxLogDebug( wxT( "In constructor, set channel_type.verbosity=%ld" ), verbosity );
    status_line_length          = initial_long;

    home_maxdepth               = initial_long;
    home_stayonhost             = initial_long;
    home_stayondomain           = initial_long;

    indent_paragraphs           = initial_long;

    bpp                         = initial_long;
    alt_text                    = initial_long;
    maxheight                   = initial_long;
    maxwidth                    = initial_long;
    alt_maxheight               = initial_long;
    alt_maxwidth                = initial_long;
    image_compression_limit     = initial_long;
    try_reduce_bpp              = initial_long;
    try_reduce_dimension        = initial_long;

    no_urlinfo                  = initial_long;
    copyprevention_bit          = initial_long;
    backup_bit                  = initial_long;
    launchable_bit              = initial_long;

    update_frequency            = initial_long;
    update_enabled              = initial_long;

    if ( initialize_to_illegal_values ) 
    {
        update_base             = wxInvalidDateTime;
    } 
    else 
    {
        update_base             = utils_datetime::get_current_datetime();
    }
}

// Destructor
channel_type::~channel_type()
{
    wxLogDebug( wxT( "Destructed channel_type" ) );
}

