//----------------------------------------------------------------------------------------
/*!
    \file       starting_page_wizardpage.h          
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes starting_page_wizardpage class    
    \author     Robert O'Connor
    \date       2002/05/03    
 */  
// RCS-ID:      $Id: starting_page_wizardpage.h,v 1.5 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _STARTING_PAGE_WIZARDPAGE_H_
#define _STARTING_PAGE_WIZARDPAGE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "starting_page_wizardpage.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/wizard.h"

//----------------------------------------------------------------------------------------
// Class definition: starting_page_wizardpage
//----------------------------------------------------------------------------------------

//! A wizard page that can collect information about the starting page for a channel.
class starting_page_wizardpage : public wxWizardPageSimple
{

public:      

    //! Constructor.
    /*!
        \note: An implementation using a wxString[] for the messages doesn't seem to 
            work, but wxArrayString works fine.
        \note: We don't use any default values. They are always going to be
            fixed at a http and file protocol prefix, respectively.
        \param parent The parent wizard.
        \param message_string_array An array of strings to use as the message. Each 
            array item is placed on a separate line.
        \param textctrl_label The label displayed beside the url textctrl.
        \param textctrl_value The initial value of the file textctrl. 
        \param bitmap Optional bitmap to show instead of the one the parent wizard is
            using.        
     */
    starting_page_wizardpage( wxWizard* parent,
                              const wxArrayString message_string_array,
                              const wxString& url_label = wxEmptyString, 
                              const wxString& file_label = wxEmptyString,
                              wxBitmap bitmap = wxNullBitmap );
                              
    //! Destructor.
    ~starting_page_wizardpage();    
    
    //! Get the value of the of the home url (starting page).
    /*! 
        \return TRUE The string that user entered into the currently enabled textctrl.
     */
    wxString get_home_url_value();
    
private:

    void on_update_ui_url_radiobutton( wxUpdateUIEvent& event );
    void on_update_ui_file_radiobutton( wxUpdateUIEvent& event );
    void on_url_button( wxCommandEvent& event );
    void on_file_button( wxCommandEvent& event );

    wxRadioButton*  m_url_radiobutton;
    wxTextCtrl*     m_url_textctrl;
    wxButton*       m_url_button;
    wxRadioButton*  m_file_radiobutton;
    wxTextCtrl*     m_file_textctrl;
    wxButton*       m_file_button;
    
    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_STARTING_PAGE_WIZARDPAGE_H_
