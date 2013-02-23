//----------------------------------------------------------------------------------------
// Name:        handheld_dest_type.cpp
// Purpose:     A class to hold a description of a channel
// Author:      Robert O'Connor
// Modified by:
// Created:     2002/01/03
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: handheld_dest_type.cpp,v 1.8 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "handheld_dest_type.h"
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

#include "handheld_dest_type.h"

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( handheld_dest_type, wxObject )

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

// Constructor
// Note there are more values than this, these are just ones that get initialized to
// something other than the default.
handheld_dest_type::handheld_dest_type( bool initialize_to_illegal_values )
{
    wxLogDebug( wxT( "Entering a handheld_dest_type's constructor" ) );

    long initial_long = 0;
    if ( initialize_to_illegal_values ) 
    {
        initial_long = -1;
    }

    //! TRUE if is a card destination (versus RAM).
    handheld_target_storage_mode    = initial_long;
    is_usb_pause			        = initial_long;
}

// Destructor
handheld_dest_type::~handheld_dest_type()
{
    wxLogDebug( wxT( "Destructed a handheld_dest_type" ) );
}

