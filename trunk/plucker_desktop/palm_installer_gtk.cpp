//----------------------------------------------------------------------------------------
// Name:        palm_installer_gtk.cpp
// Purpose:     The actions that can be performed on a channel: add, delete, update
//              Add/delete does and add/delete of both the section of .ini holding the
//              channel's settings and its subdir within the channel directory.
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: palm_installer_gtk.cpp,v 1.15 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "palm_installer_gtk.h"
#endif

//----------------------------------------------------------------------------------------
// Begin OS removal condition
//----------------------------------------------------------------------------------------

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)

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

#include "palm_installer_gtk.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( palm_installer, palm_installer_base )

//----------------------------------------------------------------------------------------
// Interface
//----------------------------------------------------------------------------------------

// Constructor
palm_installer::palm_installer()
{ 
    m_number_of_users   = 0;
    m_last_user_index   = 0;    
    m_last_user_name    = wxT( "Unused Last User Name" );        
    m_palm_desktop_path = wxT( "Unused Palm Desktop Path" );
    
    load_user_data();
}


// Destructor
palm_installer::~palm_installer()
{
}


bool palm_installer::install_file_by_handheld_dest( handheld_dest_type& handheld_dest,
                                                    const wxString &install_fullname )
{
    bool            successful;
    wxArrayString   install_fullnames;
    wxString        install_file_to_add = install_fullname;

    install_fullnames.Add( install_file_to_add );

    successful = install_files_by_handheld_dest( handheld_dest, install_fullnames );

    return successful;
}


bool palm_installer::install_files_by_handheld_dest( handheld_dest_type& handheld_dest,
                                                     wxArrayString& install_fullnames )
{
    int         user_index;
    wxString    install_fullname;
    wxString    command_string;
    bool        successful              = false;

    wxLogDebug( wxT( "Entering palm_installer::install_files_by_handheld_dest function" ) );

    // Loop through and remove install_fullnames from array that don't exist.
    for ( size_t i = 0; i < install_fullnames.GetCount(); i++ ) 
    {
        install_fullname = install_fullnames.Item( i );
        if ( ! wxFileExists( install_fullname ) ) 
        {
            install_fullnames.RemoveAt( i );
        }
    }

    // If wxArrayString is empty, then abort
    if ( install_fullnames.IsEmpty() ) 
    {
        wxLogDebug( wxT( "Error: no filenamses sent to palm_installer::install_files_by_handheld_dest . Aborting..." ) );
        return false;
    }

    // Look up the user_index of this user_name. Not really needed for WXGTK yet.
    user_index = get_user_index_from_user_name( handheld_dest.user_name );

    if ( get_number_of_users() > 0 )
    {
        // Command string format: pilot-xfer -i(nstall) fullname(s)
        // Note the spaces needed to separate the parameters, filenames
        command_string << wxT( "pilot-xfer" ) << wxT( " -i" );

        // Loop through fullnames, adding to end of command string.
        for ( size_t n = 0; n < install_fullnames.GetCount(); n++ ) 
        {
            wxLogDebug( wxT( "Starting loop to add names to command line string" ) );
            command_string << wxT( " " ) << install_fullnames.Item( n );
        }
    }

    // Give a pause, if it is a USB device, so that they can get the device
    // ready first, before execute pilot-xfer (otherwise, plucker_desktop will hang).
    if ( handheld_dest.is_usb_pause == 1 ) 
    {
        wxString usb_pause_message;
        usb_pause_message << _( "Press the HotSync button on your USB handheld first. Then click OK to continue when you are ready." )
                          << wxT("\n\n") 
                          << _( "(You can bypass this pause dialog by unchecking the USB checkbox in the handheld destination)" );
        
        //! \todo Unfortunately a NULL window here. Can we do something better, like a gettopwindow?
        wxMessageDialog usb_pause_messagedialog( (wxWindow *)NULL,
            usb_pause_message,
            _( "Press HotSync Button First" ),
            wxOK );
        
        usb_pause_messagedialog.ShowModal();
    }

    // Execute the comand
    wxLogDebug( wxT( "Executing: " ) + command_string );
    successful = wxShell( command_string );

    return successful;
}

//----------------------------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------------------------

void palm_installer::load_user_data()
{  
    m_number_of_users = 1;
  
    palm_user_type* palm_user    = new palm_user_type();    
    m_users_array.Add( palm_user );        
    wxLogDebug( wxT( "Added a new palm_user array element" ) );            

    m_users_array[ 0 ]->magic = 0;
    wxLogDebug( wxT( "Set user->magic number=0 (unused)" ));

    m_users_array[ 0 ]->name = wxT( "Pilot-xfer connected device" );               
    wxLogDebug( wxT( "Set user->name" ) );
 
    m_users_array[ 0 ]->subdirectory = wxT( "Unused subdirectory" );  
    wxLogDebug( wxT( "Set user->subdirectory" ) );
}

//----------------------------------------------------------------------------------------
// End OS removal condition
//----------------------------------------------------------------------------------------
    
#endif  // __WXGTK__ || __WXX11__ || __WXMOTIF__
