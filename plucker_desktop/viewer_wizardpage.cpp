//----------------------------------------------------------------------------------------
// Name:        viewer_wizardpage.cpp
// Purpose:     A wizardpage of destinations to install output to
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: viewer_wizardpage.cpp,v 1.14 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "viewer_wizardpage.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

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

#include "viewer_wizardpage.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/sizer.h"

// ---------------------------------------------------------------------------------------

#include "configuration.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( viewer_wizardpage, wxWizardPageSimple )

END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

/*
const wxString viewer_lores_basename = the_configuration->Read("/PLUCKER_DESKTOP/viewer_lores_basename", "_");
const wxString viewer_hires_basename = the_configuration->Read("/PLUCKER_DESKTOP/viewer_hires_basename", "hires_");

// The name of the prc file before the iso code and the file extension.
// Note, the get_is_hires_device() selected looks for hires in the filename here. 
const wxString device_prefix_basename_array[] = { viewer_lores_basename,
                                                  viewer_hires_basename
                                                };
*/                                                
const wxString bpp_key_array[] = { wxT( "1bpp" ),
                                   wxT( "2bpp" ),
                                   wxT( "4bpp" ),
                                   wxT( "8bpp" ),
                                   wxT( "16bpp" )
                                 };

const wxString viewer_language_iso_codes_array[] = { wxT( "ca" ),
													 wxT( "cz" ),
                                                     wxT( "da" ),
                                                     wxT( "de" ),
                                                     wxT( "en" ),
                                                     wxT( "es" ),
                                                     wxT( "fo" ),
                                                     wxT( "fr" ),
                                                     wxT( "it" ),
                                                     wxT( "ja" ),
                                                     wxT( "pl" ),
                                                     wxT( "ru" ),
                                                     wxT( "th" ),
                                                     wxT( "tr" ),
                                                     wxT( "zh_CN" )
                                                   };

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

// Constructor. Parent is the wizard page
viewer_wizardpage::viewer_wizardpage( wxWizard *parent,
                                      wxBitmap bitmap
                                    )
       : wxWizardPageSimple( parent )
{
    wxLogDebug( wxT( "Entering viewer_wizardpage constructor") );

    // To set a custom bitmap in a derived wxWizardPageSimple, you just have to
    // set the wxWizardPageSimple m_bitmap member in the derived constructor.
    // ( see wx examples ). .
    m_bitmap = bitmap;

    // Set up the arrays of the list ctrls. Note can't be external to class, otherwise
    // won't get translated, since locale isn't set at that point.
    const wxString device_descriptive_array[] = { _( "Normal resolution Palm OS device" ),
                                                  _( "High resolution Palm OS device" )
                                                };

    // The order of bpp descriptive names and their key parameters obviously have to match
    const wxString bpp_descriptive_array[] = { _( "Black and white" ),
                                               _( "4 shade grayscale" ),
                                               _( "16 shade grayscale" ),
                                               _( "256 colors" ),
                                               _( "Thousands of colors" )
                                             };

    // The order of languages and their iso-codes obviously have to match
    const wxString viewer_languages_descriptive_array[] = { _( "Catalan" ),
															_( "Czech" ),
                                                            _( "Danish" ),
                                                            _( "German" ),
                                                            _( "English" ),
                                                            _( "Spanish" ),
                                                            _( "Faeroese" ),
                                                            _( "French" ),
                                                            _( "Italian" ),
                                                            _( "Japanese" ),
                                                            _( "Polish" ),
                                                            _( "Russian" ),
                                                            _( "Thai" ),
                                                            _( "Turkish" ),
                                                            _( "Simplified Chinese" )
                                                          };

    /*
    // Now remove the unavailable viewers from the array (ie they weren't installed
    // as part of the installation package.
    // Make an int as the array size is going to change as we remove things
    // This is way off base. Crashes and I mean crashes hard!
    int total_viewers = WXSIZEOF( viewer_language_iso_codes_array );

    for ( int i=total_viewers; 0 <= i; i-- ) {
        // Look to see if the file exists.
        // We will assume that both the hires and normal res for a language are
        // always installed or removed together.
        if ( ! wxFile::Exists( get_plucker_directory( HANDHELD_VIEWER_SOFTWARE ) + "/"
                               + device_prefix_basename_array[ 0 ]
                               + viewer_language_iso_codes_array[ i ]
                               + ".prc" ) ) {
            // Remove them from array
            delete viewer_languages_descriptive_array[ i ];
            delete viewer_language_iso_codes_array[ i ];

        }
    }
    */

    // Make a new flexgrid sizer ( format # of rows, # of columns, then hor/vert gaps )
    // A zero in the rows or columns makes that parameter increase as things are added
    wxFlexGridSizer *root_flexgridsizer = new wxFlexGridSizer( 1, 0, 0 );
    // Set the growable columns and rows
    root_flexgridsizer->AddGrowableCol( 0 );
    root_flexgridsizer->AddGrowableRow( 1 );

    wxStaticBox *software_staticbox = new wxStaticBox( this, -1, _( "Software options" ) );
    wxStaticBoxSizer *software_staticboxsizer = new wxStaticBoxSizer( software_staticbox, wxVERTICAL );

    // Make a new flexgrid sizer ( format # of rows, # of columns, then hor/vert gaps )
    // A zero in the rows or columns makes that parameter increase as things are added
    wxFlexGridSizer *software_flexgridsizer = new wxFlexGridSizer( 0, 2, 0, 0 );
    software_flexgridsizer->AddGrowableCol( 0 );
    software_flexgridsizer->AddGrowableCol( 1 );
    software_flexgridsizer->AddGrowableRow( 0 );
    software_flexgridsizer->AddGrowableRow( 1 );

    wxStaticText *viewer_wizardpage_device_statictext = new wxStaticText( this, -1, _( "Type of handheld device:" ), wxDefaultPosition, wxDefaultSize, 0 );
    software_flexgridsizer->Add( viewer_wizardpage_device_statictext, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_device_choice = new wxChoice( this, -1, wxDefaultPosition, wxSize( 240,-1 ), WXSIZEOF( device_descriptive_array ), device_descriptive_array, 0 );
    software_flexgridsizer->Add( m_device_choice, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *viewer_wizardpage_bpp_statictext = new wxStaticText( this, -1, _( "Color depth of handheld device:" ), wxDefaultPosition, wxDefaultSize, 0 );
    software_flexgridsizer->Add( viewer_wizardpage_bpp_statictext, 0, wxALIGN_CENTRE_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    m_bpp_choice = new wxChoice( this, -1, wxDefaultPosition, wxSize( 240,-1 ), WXSIZEOF( bpp_descriptive_array), bpp_descriptive_array, 0 );
    software_flexgridsizer->Add( m_bpp_choice, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    wxStaticText *viewer_wizardpage_language_statictext = new wxStaticText( this, -1, _( "Language for handheld software:" ), wxDefaultPosition, wxDefaultSize, 0 );
    software_flexgridsizer->Add( viewer_wizardpage_language_statictext, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    m_language_choice = new wxChoice( this, -1, wxDefaultPosition, wxSize( 240,-1 ), WXSIZEOF( viewer_languages_descriptive_array ), viewer_languages_descriptive_array, 0 );
    software_flexgridsizer->Add( m_language_choice, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    software_staticboxsizer->Add( software_flexgridsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    root_flexgridsizer->Add( software_staticboxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticBox *install_staticbox = new wxStaticBox( this, -1, _( "Items to install" ) );
    wxStaticBoxSizer *install_staticboxsizer = new wxStaticBoxSizer( install_staticbox, wxVERTICAL );

    wxString install_viewer_string;
    install_viewer_string << _( "Install" ) << wxT( " " ) << plkrAPPLICATION_SUITE_NAME << wxT( " " ) << _( "Viewer" ) << wxT( "  " ) << _( "(required)" );
    m_install_viewer_checkbox = new wxCheckBox( this, -1, install_viewer_string );
    install_staticboxsizer->Add( m_install_viewer_checkbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString install_zlib_string;
    install_zlib_string << _( "Install ZLIB decompression tool" ) << wxT( "  " ) << _( "(strongly recommended)" );
    m_install_zlib_checkbox = new wxCheckBox( this, -1, install_zlib_string );
    install_staticboxsizer->Add( m_install_zlib_checkbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString install_manual_string;
    install_manual_string << _( "Install" ) << wxT( " " ) << plkrAPPLICATION_SUITE_NAME << wxT( " " ) << _( "user's manual" ) << wxT( "  " ) << _( "(optional)" );
    m_install_manual_checkbox = new wxCheckBox( this, -1, install_manual_string );
    install_staticboxsizer->Add( m_install_manual_checkbox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    root_flexgridsizer->Add( install_staticboxsizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );

    // Now that everything is added into the flexgrid sizer, send some commands to
    // tell it calculate all the control sizes nicely to fill up the available space.
    SetAutoLayout( TRUE );
    SetSizer( root_flexgridsizer );
    root_flexgridsizer->Fit( this );
    root_flexgridsizer->SetSizeHints( this );

    // Initialize the default value of the controls
    wizardpage_initalize();
    wxLogDebug( wxT( "Completed viewer_wizardpage constructor") );
}


// Destructor
viewer_wizardpage::~viewer_wizardpage()
{
}


void viewer_wizardpage::wizardpage_initalize()
{
    // Set up the initial value of the controls
    m_device_choice->SetSelection( 1 );
    // \todo need better ways of calling the defaults here instead of just "3".
    // I think that it is okay to set a selection beyond the range (for example if
    // just 2 entries, okay to set selection to the translated English.
    m_language_choice->SetStringSelection( _( "English" ) );
    m_bpp_choice->SetSelection( 4 );
    m_install_viewer_checkbox->SetValue( TRUE );
    m_install_zlib_checkbox->SetValue( TRUE );
    m_install_manual_checkbox->SetValue( TRUE );
}


void viewer_wizardpage::get_fullnames_to_install( wxArrayString* fullnames_to_install )
{
    int             device_index;
    int             language_index;
    wxString        viewer_basename;
    wxString        viewer_fullname;
    wxString        zlib_fullname;
    wxString        userguide_fullname;

	wxString viewer_lores_basename = the_configuration->Read(wxT( "/PLUCKER_DESKTOP/viewer_lores_basename"), wxT( "viewer_"));
	wxString viewer_hires_basename = the_configuration->Read(wxT( "/PLUCKER_DESKTOP/viewer_hires_basename"), wxT( "viewer_hires_"));

	// The name of the prc file before the iso code and the file extension.
	// Note, the get_is_hires_viewer_selected() function looks for 2nd viewer in the dropdown. 
	const wxString device_prefix_basename_array[] = { 
													  viewer_lores_basename,
			                                          viewer_hires_basename
													};
	
    language_index      = m_language_choice->GetSelection();
    device_index        = m_device_choice->GetSelection();

    if ( device_index != -1 )
    {
        viewer_fullname     = get_plucker_directory( HANDHELD_VIEWER_SOFTWARE ) + wxT( "/" )
                              + device_prefix_basename_array[ device_index ]
                              + viewer_language_iso_codes_array[ language_index ]
                              + wxT( ".prc");
        wxLogDebug( wxT( "viewer_wizardpage: viewer_fullname=") + viewer_fullname );
    }
    else
    {
        wxLogDebug( wxT( "Error: no viewers on system. Can't install") );
    }

#if defined(__WXMSW__) || defined(__WXMAC__)
    // We install a SysZLib_hires.prc on Mac and MSW bundles, if needed but not on GTK yet.
    if ( get_is_hires_viewer_selected() ) 
    {
        zlib_fullname     = get_plucker_directory( HANDHELD_VIEWER_SOFTWARE ) + wxT( "/" )
                          + wxT( "SysZLib_hires.prc");
    } 
    else 
    {
        zlib_fullname     = get_plucker_directory( HANDHELD_VIEWER_SOFTWARE ) + wxT( "/" )
                          + wxT( "SysZLib.prc");
    }
#else // (__WXMSW__) || (__WXMAC__)
	zlib_fullname     = get_plucker_directory( HANDHELD_VIEWER_SOFTWARE ) + wxT( "/" )
                          + wxT( "SysZLib.prc");
#endif // (__WXMSW__) || (__WXMAC__)
    
    wxLogDebug( wxT( "viewer_wizardpage: zlib_fullname=") + zlib_fullname );

    userguide_fullname  = get_plucker_directory( HANDHELD_VIEWER_SOFTWARE ) + wxT( "/" )
                          + wxT( "PluckerUserGuide.pdb");
    wxLogDebug( wxT( "viewer_wizardpage: userguid_fullname=") + userguide_fullname );

    if ( m_install_viewer_checkbox->GetValue() == TRUE )
    {
        fullnames_to_install->Add( viewer_fullname );
    }

    if ( m_install_zlib_checkbox->GetValue() == TRUE ) 
    {
        fullnames_to_install->Add( zlib_fullname );
    }
    if ( m_install_manual_checkbox->GetValue() == TRUE )
    {
        fullnames_to_install->Add( userguide_fullname );
    }
}


long viewer_wizardpage::get_bpp()
{
    long bpp = 0;

    switch ( m_bpp_choice->GetSelection() ) 
    {
        case 0:
            bpp = 1;
            break;
        case 1:
            bpp = 2;
            break;
        case 2:
            bpp = 4;
            break;
        case 3:
            bpp = 8;
            break;
        case 4:
            bpp = 16;
            break;
    }

    return bpp;
}

bool viewer_wizardpage::get_is_hires_viewer_selected()
{
	bool is_hires_viewer_selected = FALSE;
    int device_index;
    
    device_index  = m_device_choice->GetSelection();
    
	// device_index == 1 is the hires viewer.
    if ( device_index == 1 ) 
    {
		is_hires_viewer_selected = TRUE;
    }
    
    return is_hires_viewer_selected;
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_WIZARDS
