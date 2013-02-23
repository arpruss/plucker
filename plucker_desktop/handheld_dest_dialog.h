//----------------------------------------------------------------------------------------
/*!
    \file       handheld_dest_dialog.h
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )
    \licence    GPL
    \brief      Describes handheld_dest_dialog class    
    \author     Robert O'Connor
    \date       2003/02/01
 */  
// RCS-ID:      $Id: handheld_dest_dialog.h,v 1.6 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _HANDHELD_DEST_DIALOG_H_
#define _HANDHELD_DEST_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "handheld_dest_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/dialog.h"
#include "handheld_dest_type.h"

//----------------------------------------------------------------------------------------
// Class definition: handheld_dest_dialog
//----------------------------------------------------------------------------------------

//! A startup tips dialog, similar to wxTipDialog, but that can handle html.
/*!
    This class shouldn't be used directly. Just call the show_html_tip function--this 
    is similar to the wxShowTip function in wxWindows library.
 */
class handheld_dest_dialog : public wxDialog
{

public:

    //! Constructor.
    /*!
        \param parent The parent window.
        \param title The title of the dialog
        \param The user names to include in the list of user names. ``
      */
    handheld_dest_dialog( wxWindow* parent,
                          const wxString& title,
                          wxArrayString& user_name_array
                        );

    //! Destructor.
    ~handheld_dest_dialog();

    //! Gets handheld_dest_type, ready to insert by parent.
    /*!
        \param handheld_dest The handheld_dest_type, to be inserted.
     */
    void transfer_to( handheld_dest_type& handheld_dest  );


private:

    void on_help_button( wxCommandEvent& event );

    wxChoice*        m_user_name_choice;
    wxCheckBox*		 m_is_usb_pause_checkbox;
    wxRadioButton*   m_install_to_ram_radiobutton;
    wxRadioButton*   m_install_to_sd_card_radiobutton;
    wxRadioButton*   m_install_to_memory_stick_radiobutton;
    wxRadioButton*   m_install_to_compact_flash_radiobutton;
    wxRadioButton*   m_default_card_directory_radiobutton;
    wxRadioButton*   m_specified_card_directory_radiobutton;
    wxTextCtrl*      m_directory_on_card_textctrl;

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_HANDHELD_DEST_DIALOG_H_

