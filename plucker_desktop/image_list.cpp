//----------------------------------------------------------------------------------------
// Name:        image_list.cpp
// Purpose:     Maintains the shared image list of icons
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: image_list.cpp,v 1.14 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------
           
//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "image_list.h"
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

#include "image_list.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/imaglist.h"            // wxImageList
#include "wx/module.h"				// wxModule
#include "wx/image.h"				// wxImage

//----------------------------------------------------------------------------------------

#include "configuration.h"

//----------------------------------------------------------------------------------------
// Internal variables
//----------------------------------------------------------------------------------------

image_list* image_list::ms_instance = NULL;

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

// Create the instance of the image list,specifying the size of each item to
// be 16x16 pixels. The 'TRUE' argument specifies that masks should also be created.
image_list::image_list()
            : wxImageList( 16, 16, TRUE )
{

    wxLogDebug( wxT( "Entering image_list constructor" ) );
   
        // Add the icons to image list from .xpm files. Its argument is the variable
        // name contained in the start of the XPM file.
        // The order of adding must match what it is in the image_list.h
        add_file_from_image_list_directory( wxT( "small_satellite_dish_icon" ) );
        add_file_from_image_list_directory( wxT( "small_edit_pencil_icon" ) );
        add_file_from_image_list_directory( wxT( "small_preview_page_icon" ) );
        add_file_from_image_list_directory( wxT( "small_radio_options_icon" ) );
        add_file_from_image_list_directory( wxT( "small_sync_icon" ) );
        add_file_from_image_list_directory( wxT( "small_folder_icon" ) );
        add_file_from_image_list_directory( wxT( "small_checkbox_checked_icon" ) );
        add_file_from_image_list_directory( wxT( "small_checkbox_unchecked_icon" ) );

    wxLogDebug( wxT( "Leaving image_list constructor" ) );
}


image_list::~image_list()
{
}


void image_list::add_file_from_image_list_directory( const wxString& basename )
{
   // Robbed from <wx>/src/common/iconbndl.cpp
   // NOTE: Please don't change this: it was a lot of work to get it working
   // properly.

    wxImage     image;
    wxString    icon_fullname;

    icon_fullname << get_plucker_directory( RESOURCES ) << wxT( "/icons_image_list" )
                 << wxT( "/" ) << basename << wxT( "." ) << wxT( "xpm" );

    if( ! image.LoadFile( icon_fullname, wxBITMAP_TYPE_XPM ) ) 
    {
        wxLogError( wxT( "Failed to load image from file '%s'." ),
                    icon_fullname.c_str() );
    }

    wxIcon* temp_icon = new wxIcon();
    temp_icon->CopyFromBitmap( wxBitmap( image ) );
    Add( *temp_icon );
    delete temp_icon;
}


// Static
image_list* image_list::get()
{
    wxLogDebug( wxT( "Entering image_list::get()" ) );

    if ( ! ms_instance ) 
    {
        ms_instance = new image_list();
    }

    // Return ms_instance (which will stay as NULL if
    return ms_instance;
}


// Static
image_list* image_list::set( image_list *desired_image_list )
{
    image_list* old_image_list = ms_instance;
    ms_instance = desired_image_list;
    return old_image_list;
}

//----------------------------------------------------------------------------------------
// Module definition
//----------------------------------------------------------------------------------------

class module_image_list : public wxModule
{

    DECLARE_DYNAMIC_CLASS( module_image_list )
    
public:

    module_image_list() {}
    
    bool OnInit()
    {
        return TRUE;
    }
    
    void OnExit()
    {
        // We set it to null, and a pointer returns to the previous one, which
        // we delete. (I assume delete NULL is legal?)
        delete image_list::set( NULL );
    }

};

IMPLEMENT_DYNAMIC_CLASS( module_image_list, wxModule )

