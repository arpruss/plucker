//----------------------------------------------------------------------------------------
/*!
    \file       channel_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes channel_dialog class    
    \author     Robert O'Connor
    \date       2001/10/20    
 */  
// RCS-ID:      $Id: channel_dialog.h,v 1.29 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _CHANNEL_DIALOG_H_
#define _CHANNEL_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "channel_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#include "wx/dialog.h"
#include "wx/datetime.h"
// ---------------------------------------------------------------------------------------
#include "hhictrl.h"                    // wxHandheldInstallCtrl
#include "time_control.h"               // time_control

//----------------------------------------------------------------------------------------
// External functions and variables
//----------------------------------------------------------------------------------------

//! The index number of each notebook page (tab) on the channel_dialog.
/*
    For checking that we have entered the channels tab, so know to save configuration.

    Also used to display the correct help page for the tab. Also for starting the
    dialog with a different tab.

    Placed before channel_dialog contructor since used as a default param value.
 */
enum {
    //! Channel dialog 'Starting page' tab.
    CHANNEL_NOTEBOOK_PAGE_INDEX_START           = 0,
    //! Channel dialog 'Spidering' tab.
    CHANNEL_NOTEBOOK_PAGE_INDEX_SPIDERING, 
    //! Channel dialog 'Limits' tab.
    CHANNEL_NOTEBOOK_PAGE_INDEX_LIMITS,
    //! Channel dialog 'Formatting' tab.
    CHANNEL_NOTEBOOK_PAGE_INDEX_FORMATTING,
    //! Channel dialog 'Images' tab.
    CHANNEL_NOTEBOOK_PAGE_INDEX_IMAGES, 
    //! Channel dialog 'Output' tab.
    CHANNEL_NOTEBOOK_PAGE_INDEX_OUTPUT,
    //! Channel dialog 'Destinations' tab.
    CHANNEL_NOTEBOOK_PAGE_INDEX_DESTINATION,
    //! Channel dialog 'Scheduling' tab.
    CHANNEL_NOTEBOOK_PAGE_INDEX_SCHEDULING    
};

//----------------------------------------------------------------------------------------
// Class definition: channel_dialog
//----------------------------------------------------------------------------------------

//! A dialog to configure a channel.
class channel_dialog : public wxDialog
{

public: 
    //! Constructor
    /*!      
      \param parent The parent window.
      \param configuration_section The section of configuration file of channel to edit.
      \param initial_selected_notebook_tab Initial tab selected when dialog is shown. 
            This param allows warnings of "There is no destinations for this channel.
            Do you wish to add some now?" to jump directly to the Destinations tab.
     */
    channel_dialog( wxWindow* parent, 
                    wxString configuration_section, 
                    int initial_selected_notebook_tab = CHANNEL_NOTEBOOK_PAGE_INDEX_START );
    
    //! Destructor
    ~channel_dialog();
           
private:

    //! Read and set controls' values from this channel's section of configuration file.
    void read_configuration();
    void set_image_tab_bpp_controls( long bpp );
    //! Sets stayonhost and stayondomain controls
    void set_limits_tab_stayonhost_stayondomain_controls( bool stayonhost,
                                                          bool stayondomain );
    void set_scheduling_tab_update_controls( long update_frequency,
                                             const wxString& update_period,
                                             const wxDateTime& update_base,
                                             bool update_enabled );    
    void write_configuration();    
    void launch_editor( const wxString& filename_to_load, 
                        long style=plkrPROMPT_IF_FILE_NOT_FOUND );
    
    bool prevent_close_if_no_destinations_chosen();

    void on_update_ui_start_tab_url_radiobutton( wxUpdateUIEvent& event );
    void on_update_ui_start_tab_file_radiobutton( wxUpdateUIEvent& event );
    void on_update_ui_start_tab_links_radiobutton( wxUpdateUIEvent& event );
    void on_start_tab_url_button( wxCommandEvent& event );
    void on_update_ui_start_tab_file_textctrl( wxUpdateUIEvent& event );
    void on_start_tab_file_button( wxCommandEvent& event );
    void on_start_tab_edit_file_button( wxCommandEvent& event );
    void on_start_tab_links_button( wxCommandEvent& event );

    void on_update_ui_spidering_tab_retrieval_timeout_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_spidering_tab_status_line_length_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_spidering_tab_referrer_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_spidering_tab_user_agent_browser_radiobutton( wxUpdateUIEvent& event );
    void on_update_ui_spidering_tab_user_agent_custom_radiobutton( wxUpdateUIEvent& event );

    void on_update_ui_limits_tab_stayonhost_stayondomain_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_limits_tab_url_pattern_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_limits_tab_exclusion_checkbox( wxUpdateUIEvent& event );
    void on_limits_tab_exclusion_button( wxCommandEvent& event );

    void on_update_ui_formatting_tab_charset_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_formatting_tab_anchor_color_checkbox( wxUpdateUIEvent& event );
    void on_formatting_tab_anchor_color_button( wxCommandEvent& event );

    void on_update_ui_images_tab_include_images_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_images_tab_only_link_radiobutton( wxUpdateUIEvent& event );
    
    void on_update_ui_output_tab_only_compress_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_output_tab_category_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_output_tab_owner_id_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_output_tab_launchable_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_output_tab_custom_large_icon_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_output_tab_custom_small_icon_checkbox( wxUpdateUIEvent& event );    
    void on_output_tab_custom_small_icon_button( wxCommandEvent& event );
    void on_output_tab_custom_large_icon_button( wxCommandEvent& event );    

    void on_update_ui_scheduling_tab_update_enabled_checkbox( wxUpdateUIEvent& event );  
    void on_scheduling_tab_now_button( wxCommandEvent& event );
    
    // Override base class functions.
    void OnOK( wxCommandEvent& event );
    void on_help_button( wxCommandEvent& event );
    
    // Member variables
    wxString                m_configuration_section;
    wxHandheldInstallCtrl*  m_handheld_install_ctrl;    
    time_control*           m_scheduling_time_control;
    
    DECLARE_EVENT_TABLE()
    
};

#endif      //_CHANNEL_DIALOG_H_
