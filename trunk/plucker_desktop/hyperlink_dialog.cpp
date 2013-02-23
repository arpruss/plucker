//----------------------------------------------------------------------------------------
// Name:        hyperlink_dialog.cpp
// Purpose:     Insert a hyperlink dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: hyperlink_dialog.cpp,v 1.25 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "hyperlink_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTEGRATED_HTML_EDITOR )

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

#include "hyperlink_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/spinctrl.h"            // wxSpinCtrl

// ---------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

// ---------------------------------------------------------------------------------------

#include "configuration.h"
#include "utils_controls.h"
#include "showcase_dialog.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( hyperlink_dialog, wxDialog )
    EVT_UPDATE_UI( XRCID( "hyperlink_dialog_url_radiobutton" ), hyperlink_dialog::on_update_ui_url_radiobutton )
    EVT_BUTTON( XRCID( "hyperlink_dialog_url_button" ), hyperlink_dialog::on_url_button )
    EVT_UPDATE_UI( XRCID( "hyperlink_dialog_file_radiobutton" ), hyperlink_dialog::on_update_ui_file_radiobutton )
    EVT_BUTTON( XRCID( "hyperlink_dialog_file_button" ), hyperlink_dialog::on_file_button )

    EVT_UPDATE_UI( XRCID( "hyperlink_dialog_maxdepth_checkbox" ), hyperlink_dialog::on_update_ui_maxdepth_checkbox )
    EVT_UPDATE_UI( XRCID( "hyperlink_dialog_url_pattern_checkbox" ), hyperlink_dialog::on_update_ui_url_pattern_checkbox )
    EVT_UPDATE_UI( XRCID( "hyperlink_dialog_images_depth_checkbox" ), hyperlink_dialog::on_update_ui_images_depth_checkbox )
    EVT_UPDATE_UI( XRCID( "hyperlink_dialog_images_maximum_width_checkbox" ), hyperlink_dialog::on_update_ui_images_maximum_width_checkbox )
    EVT_UPDATE_UI( XRCID( "hyperlink_dialog_images_maximum_height_checkbox" ), hyperlink_dialog::on_update_ui_images_maximum_height_checkbox )
    
    EVT_BUTTON( wxID_HELP_CONTEXT, hyperlink_dialog::on_help_button )
    EVT_BUTTON( wxID_OK, hyperlink_dialog::OnOK )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Non-event handler functions
//----------------------------------------------------------------------------------------

// Constructor
hyperlink_dialog::hyperlink_dialog( wxWindow* parent )
{
    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "hyperlink_dialog") );
    
    XRCCTRL( *this, "hyperlink_dialog_images_depth_choice", wxChoice )
        ->SetSelection( 1 ); 
}


// Destructor
hyperlink_dialog::~hyperlink_dialog()
{
}


void hyperlink_dialog::transfer_to( wxString& starting_text, wxString& ending_text )
{
    starting_text = m_starting_text;
    ending_text = m_ending_text;
}

//----------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------

void hyperlink_dialog::on_update_ui_url_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "hyperlink_dialog_url_radiobutton", wxRadioButton )
            ->GetValue();
    XRCCTRL( *this, "hyperlink_dialog_url_textctrl", wxTextCtrl )
            ->Enable( enabled );
    XRCCTRL( *this, "hyperlink_dialog_url_button", wxButton )
            ->Enable( enabled );
}


void hyperlink_dialog::on_url_button( wxCommandEvent &event )
{    

#if ( setupUSE_SHOWCASE )

    showcase_dialog a_showcase_dialog( this );
    
    // Only execute this, if user pressed OK
    if ( a_showcase_dialog.ShowModal() == wxID_OK ) 
    {
        wxString    name;
        wxString    url;
        wxString    description;
        wxString    language;
        wxString    category;
        long        update_frequency;
        wxString    update_period;
        wxDateTime  update_base;
        bool        update_enabled;
        long        bpp;
        long        maxdepth;
        bool        stayonhost;
        bool        stayondomain;
        wxString    url_pattern;
        
        // Show a showcase dialog and collect the selected rows fields into these variables       
        a_showcase_dialog.transfer_to( name, url, description, language, category,
                                       update_frequency, update_period, update_base, 
                                       update_enabled, bpp, maxdepth, stayonhost, 
                                       stayondomain, url_pattern );
                                                                              
        // Set name and url to the returned data
        XRCCTRL( *this, "hyperlink_dialog_description_textctrl", wxTextCtrl )->SetValue( name );        
        XRCCTRL( *this, "hyperlink_dialog_url_textctrl", wxTextCtrl )->SetValue( url );  
    
        // Only set the rest if user so asked, in the configuration file.
        if ( the_configuration->Read( wxT( "showcase_dialog_insert_all_parameters"), 1L ) == 1 ) 
        {          
            int choice;
                       
            if ( bpp != -1 ) 
            {
                switch ( bpp ) 
                { 
                    case 0:
                        choice = 0;
                        break;
                
                    case 1:
                        choice = 1;
                        break;
                
                    case 2:
                        choice = 2;
                        break;
                
                    case 4:
                        choice = 3;
                        break;    
                
                    case 8:
                        choice = 4;
                        break;
                    
                    case 16:
                        choice = 4;
                        break;    
                
                    default:
                        choice = 1;
                        break;                                   
                }                
                XRCCTRL( *this, "hyperlink_dialog_images_depth_choice", wxChoice )
                    ->SetSelection( choice );
                XRCCTRL( *this, "hyperlink_dialog_images_depth_checkbox", wxCheckBox )
                    ->SetValue( TRUE );                
            }            
            
            // Tests for a -1 return. Also prevents an accidental maxdepth=0 that someone
            // wrote in the showcase listings.
            if ( 0 < maxdepth)
            {
                XRCCTRL( *this, "hyperlink_dialog_maxdepth_spinctrl", wxSpinCtrl )
                    ->SetValue( maxdepth ); 
                 XRCCTRL( *this, "hyperlink_dialog_maxdepth_checkbox", wxCheckBox )
                    ->SetValue( TRUE );    
            }
            
            XRCCTRL( *this, "hyperlink_dialog_stayonhost_checkbox", wxCheckBox )
                    ->SetValue( stayonhost );
 
            XRCCTRL( *this, "hyperlink_dialog_stayondomain_checkbox", wxCheckBox )
                    ->SetValue( stayondomain );
                    
            XRCCTRL( *this, "hyperlink_dialog_url_pattern_textctrl", wxTextCtrl )
                    ->SetValue( url_pattern );
            bool enable_checkbox = ! url_pattern.IsEmpty();
            XRCCTRL( *this, "hyperlink_dialog_url_pattern_checkbox", wxCheckBox )
                    ->SetValue( enable_checkbox );  
        }        
    }

#endif
    
}  


void hyperlink_dialog::on_update_ui_file_radiobutton( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "hyperlink_dialog_file_radiobutton", wxRadioButton )
            ->GetValue();
    XRCCTRL( *this, "hyperlink_dialog_file_textctrl", wxTextCtrl )
            ->Enable( enabled );
    XRCCTRL( *this, "hyperlink_dialog_file_button", wxButton )
            ->Enable( enabled );
}


void hyperlink_dialog::on_file_button( wxCommandEvent &event )
{
    // [utils_controls.cpp]    
    utils_controls::textctrl_filename_selection( XRCCTRL( *this, "hyperlink_dialog_file_textctrl", wxTextCtrl ),
                                 _( "Choose file to link to" ),
                                 wxT( ""),
                                 wxT( ""),
                                 FILE_DIALOG_WILDCARDS_PLUCKABLES,
                                 false,
                                 plkrOS_FILE_ARRAY_SEPARATOR,
                                 true );  
}

void hyperlink_dialog::on_update_ui_maxdepth_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "hyperlink_dialog_maxdepth_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "hyperlink_dialog_maxdepth_spinctrl", wxSpinCtrl )
            ->Enable( enabled );      
}

void hyperlink_dialog::on_update_ui_url_pattern_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "hyperlink_dialog_url_pattern_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "hyperlink_dialog_url_pattern_textctrl", wxTextCtrl )
            ->Enable( enabled );      
}

void hyperlink_dialog::on_update_ui_images_depth_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "hyperlink_dialog_images_depth_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "hyperlink_dialog_images_depth_choice", wxChoice )
            ->Enable( enabled );      
}

void hyperlink_dialog::on_update_ui_images_maximum_width_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "hyperlink_dialog_images_maximum_width_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "hyperlink_dialog_images_maximum_width_spinctrl", wxSpinCtrl )
            ->Enable( enabled );     
}

void hyperlink_dialog::on_update_ui_images_maximum_height_checkbox( wxUpdateUIEvent &event )
{
    bool enabled;
        
    enabled = XRCCTRL( *this, "hyperlink_dialog_images_maximum_height_checkbox", wxCheckBox )
            ->GetValue();
    XRCCTRL( *this, "hyperlink_dialog_images_maximum_height_spinctrl", wxSpinCtrl )
            ->Enable( enabled );      
}

// Override wxDialog's default behavior for clicking an OK button.
void hyperlink_dialog::OnOK( wxCommandEvent& event )
{
    wxString buf;
    int spin_value;
    
   //     wxString m_starting_text;
   // wxString m_ending_text;
    
    m_starting_text = wxT( "<a href=\"");
      
    if ( XRCCTRL( *this, "hyperlink_dialog_url_radiobutton", wxRadioButton )->GetValue() )
    {
        m_starting_text +=( XRCCTRL( *this, "hyperlink_dialog_url_textctrl", wxTextCtrl )->GetValue() );
    } 
    else
    {
        m_starting_text +=( XRCCTRL( *this, "hyperlink_dialog_file_textctrl", wxTextCtrl )->GetValue() );
    }    
    m_starting_text += wxT( "\""); 

    if ( XRCCTRL( *this, "hyperlink_dialog_maxdepth_checkbox", wxCheckBox )->GetValue() )
    {
        m_starting_text += wxT( " maxdepth=\"");
        spin_value = XRCCTRL( *this, "hyperlink_dialog_maxdepth_spinctrl", wxSpinCtrl )->GetValue();
        // TODO: Make sure this is the best way to do this. Help says Printf can cause overflow on some ports
        // Note that the _T is requred, or else it won't work.
        buf.Printf( wxT( "%d\"" ), spin_value );
        m_starting_text += buf;
     } 
    
    if( XRCCTRL( *this, "hyperlink_dialog_stayonhost_checkbox", wxCheckBox )->GetValue() )
    {
        m_starting_text += wxT( " stayonhost");
    }

    if( XRCCTRL( *this, "hyperlink_dialog_stayondomain_checkbox", wxCheckBox )->GetValue() )
    {
        m_starting_text += wxT( " stayondomain");
    }

    if( XRCCTRL( *this, "hyperlink_dialog_url_pattern_checkbox", wxCheckBox )->GetValue() ) 
    {
        m_starting_text += wxT( " url_pattern=\"");
        m_starting_text +=( XRCCTRL( *this, "hyperlink_dialog_url_pattern_textctrl", wxTextCtrl )->GetValue() );     
        m_starting_text += wxT( "\"";) 
    }
    
    if ( XRCCTRL( *this, "hyperlink_dialog_images_depth_checkbox", wxCheckBox )->GetValue() ) 
    {
        m_starting_text += wxT( " bpp=\"");
        // The bpp to use is "2 to the exponent of the index of the choice ctrl"
        spin_value = XRCCTRL( *this, "hyperlink_dialog_images_depth_choice", wxChoice )->GetSelection();
        switch ( spin_value ) 
        {
            case 0:
                spin_value = 0;
                break;
            case 1:
                spin_value = 1;
                break;
            case 2:
                spin_value = 2;
                break;
            case 3:
                spin_value = 4;
                break;
            case 4:
                spin_value = 8;
                break;
            case 5:
                spin_value = 16;
                break;
        } 
        // TODO: Make sure this is the best way to do this. Help says Printf can cause overflow on some ports
        // Note that the _T is requred, or else it won't work.
        buf.Printf( wxT( "%d\"" ), spin_value );
        m_starting_text += buf;
     } 
   
     if ( XRCCTRL( *this, "hyperlink_dialog_images_maximum_width_checkbox", wxCheckBox )->GetValue() ) 
     {
        m_starting_text += wxT( " maxwidth=\"");
        spin_value = XRCCTRL( *this, "hyperlink_dialog_images_maximum_width_spinctrl", wxSpinCtrl )->GetValue();
        // TODO: Make sure this is the best way to do this. Help says Printf can cause overflow on some ports
        // Note that the _T is requred, or else it won't work.
        buf.Printf( wxT( "%d\"" ), spin_value );
        m_starting_text += buf;
     } 
  
     if ( XRCCTRL( *this, "hyperlink_dialog_images_maximum_height_checkbox", wxCheckBox )->GetValue() ) 
     {
        m_starting_text += wxT( " maxheight=\"");
        spin_value = XRCCTRL( *this, "hyperlink_dialog_images_maximum_height_spinctrl", wxSpinCtrl )->GetValue();
        // TODO: Make sure this is the best way to do this. Help says Printf can cause overflow on some ports
        // Note that the _T is requred, or else it won't work.
        buf.Printf( wxT( "%d\"" ), spin_value );
        m_starting_text += buf;
     }
     
    // Finish off the anchor tag
    m_starting_text += wxT( ">");
    m_starting_text +=( XRCCTRL( *this, "hyperlink_dialog_description_textctrl", wxTextCtrl )->GetValue() );     
    
    m_ending_text += wxT( "</a>");
    
    EndModal( wxID_OK );
}


void hyperlink_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_HYPERLINK_DIALOG );
#endif 
}


//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 
