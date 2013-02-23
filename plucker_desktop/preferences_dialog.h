//----------------------------------------------------------------------------------------
/*!
    \file       preferences_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes preferences_dialog class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: preferences_dialog.h,v 1.11 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PREFERENCES_DIALOG_H_
#define _PREFERENCES_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "preferences_dialog.h"
#endif

// ---------------------------------------------------------------------------------------
// Shared defines
// ---------------------------------------------------------------------------------------

#include "plucker_defines.h"

// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#include "wx/dialog.h"

//----------------------------------------------------------------------------------------
// Class definition: preferences_dialog
//----------------------------------------------------------------------------------------

//! A preferences dialog for settings that apply to all channels.
class preferences_dialog : public wxDialog
{

public: 

    //! Constructor
    /*!      
      \param parent The parent window.
     */
    preferences_dialog( wxWindow* parent );
    
    //! Destructor
    ~preferences_dialog();
       
private:

    void read_configuration();
    void write_configuration();        
     
    void on_update_ui_internationalization_enabled_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_internationalization_choose_radiobutton( wxUpdateUIEvent& event );
    void on_internationalization_choose_button( wxCommandEvent& event );
    void on_update_ui_show_toolbar_checkbox( wxUpdateUIEvent &event );
        
    void on_update_ui_command_before_group_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_command_after_group_checkbox( wxUpdateUIEvent& event );   
    void on_exclusion_lists_button( wxCommandEvent& event );  
        
    void on_update_ui_builtin_editor_radiobutton( wxUpdateUIEvent& event );
    void on_update_ui_external_editor_radiobutton( wxUpdateUIEvent& event );
    void on_external_editor_button( wxCommandEvent& event );
    
    void on_showcase_details_button( wxCommandEvent& event );
        
    void on_update_ui_use_proxy_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_proxy_password_prompt_checkbox( wxUpdateUIEvent& event );
    
    // Override base class functions.
    void OnOK( wxCommandEvent& event );
    void on_help_button( wxCommandEvent& event );
    
    //! Array of tabs that are hidden from the user.
    wxArrayInt m_hidden_notebook_pages_array;
    
    //! Array of notebook titles (to keep help straight)
    wxArrayString m_notebook_page_titles_array;

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif      // _PREFERENCES_DIALOG_H_
