/*!
    \file       checkbox_message_dialog.h
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes checkbox_message_dialog class
    \author     Robert O'Connor
    \date       2002/10/20    
 */  
// RCS-ID:      $Id: checkbox_message_dialog.h,v 1.9 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _CHECKBOX_MESSAGE_DIALOG_H_
#define _CHECKBOX_MESSAGE_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "checkbox_message_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_CHECKBOX_MESSAGE_DIALOG )

// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#include "wx/dialog.h"
#include "wx/checkbox.h"

//----------------------------------------------------------------------------------------
// Class definition: checkbox_message_dialog
//----------------------------------------------------------------------------------------

//! A message dialog, with a checkbox, that says "Don't ask me again".
/*!  
    The value that user specified is saved the specified path in the currently active 
    wxConfig object.
    Sytle of buttons is an an 'or' (|) of wxOK, wxCANCEL, wxYES | wxNO .
  
    Returns wxYES/NO/OK/CANCEL .
 */
class checkbox_message_dialog : public wxDialog
{
 
public:

    //! Contructor
    /*!
      \param parent The parent window.
      \param message The message of the dialog, shown in the body of the dialog.
      \param caption The caption shown in the titlebar of the dialog.
      \param style The style of the dialog, usually wxOK and wxCENTRE.
      \param pos The x,y screen coordiate to show the dialog.
      \param checkbox_caption The caption shown to the right of the checkbox.
      \param config_path_to_show_key The path in the wxConfig object to save value.
      \param default_check_checkbox_if_key_not_found_in_config Whether to 
            initially check the checkbox if no value exists in the active wxConfig.
     */
    checkbox_message_dialog( wxWindow *parent,
                             const wxString& message,
                             const wxString& caption = _( "Message" ),
                             long style = wxOK | wxCENTRE,
                             const wxPoint& pos = wxDefaultPosition,
                             const wxString& checkbox_caption = _( "Don't remind me again" ),
                             const wxString& config_path_to_show_key = wxT( "/" ),
                             bool default_check_checkbox_if_key_not_found_in_config = TRUE
                            );
    
    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    checkbox_message_dialog() {};        
             
    //! Action executed when user clicks 'Yes' button.
    void        OnYes( wxCommandEvent& event );
    
    //! Action executed when user clicks 'No' button.
    void        OnNo( wxCommandEvent& event );
    
    //! Action executed when user clicks 'Cancel' button.
    void        OnCancel( wxCommandEvent& event );
    
    //! Action executed when user clicks 'OK' button.
    void        OnOK( wxCommandEvent& event );
    
protected:

    //! Saves checkbox value to the currently active wxConfig object.    
    void        save_checkbox_value_to_config();
    
    //! The configuration path to the key where value is loaded/saved.
    wxString    m_config_path_to_show_key;
    
    //! The checkbox on the dialog. 
    wxCheckBox* m_checkbox;
        
private:

    long        m_dialogStyle;

    DECLARE_DYNAMIC_CLASS( checkbox_message_dialog )
    DECLARE_EVENT_TABLE()    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_CHECKBOX_MESSAGE_DIALOG

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _CHECKBOX_MESSAGE_DIALOG_H_
