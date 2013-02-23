//----------------------------------------------------------------------------------------
/*!
    \file       exclusion_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes exclusion_dialog class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  // RCS-ID:      $Id: exclusion_dialog.h,v 1.18 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _EXCLUSION_DIALOG_H_
#define _EXCLUSION_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

//! \test Why wasn't this put in earlier?
#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "exclusion_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/dialog.h"
#include "wx/listctrl.h"                // wxListCtrl
#include "wx/textfile.h"                // wxTextFile
#include "wx/notebook.h"                // wxNotebook
// ---------------------------------------------------------------------------------------
#include "exclusion_listctrl.h"

//----------------------------------------------------------------------------------------
// Class definition: exclusion_dialog
//----------------------------------------------------------------------------------------

//! A dialog to edit exclusion lists.
/*! 
    Used in preferences_dialog and channel_dialog. Composed of two tabs, one each for
    URL and file exclusions, each having an exclusion_listctrl .
    Edits list entries via a blocked_dialog .
 */
class exclusion_dialog : public wxDialog
{

public: 
    
    //! Constructor.
    /*!
      \param parent The parent window.
      \param configuration_section Configuration section with key of exclusion list
            filename that should be edited. If it is DEFAULT, then it loads the 
            shared exclusion list.
     */    
    exclusion_dialog( wxWindow* parent, 
                      wxString configuration_section
                    );
    
    //! Destructor.                  
    ~exclusion_dialog();

protected:

    void listctrl_init();
    void listctrl_load_rows();
    
    /*!
        This has to be a separate function from the load_rows function because of the way 
        wxTextFile::GetFirstLine looping works, it only gets all but the last line.
     */
    void listctrl_parse_single_loaded_row( wxString string_to_parse );
    
    void listctrl_save_rows( exclusion_listctrl* listctrl );
    void save_exclusion_file();
    
    void on_extensions_tab_add_button( wxCommandEvent& event );
    void on_extensions_tab_edit_button( wxCommandEvent& event );
    void on_extensions_tab_delete_button( wxCommandEvent& event );
    
    void on_urls_tab_add_button( wxCommandEvent& event );
    void on_urls_tab_edit_button( wxCommandEvent& event );
    void on_urls_tab_delete_button( wxCommandEvent& event );        

#if ( setupUSE_PARENT_WINDOW_CATCH_UPDATE_UI_EVENT_FROM_DERIVED_LISTCTRL_HACK )
    void on_update_ui_all_listctrls( wxUpdateUIEvent& event );
#endif
    void on_update_ui_extensions_tab_listctrl( wxUpdateUIEvent& event );
    void on_update_ui_urls_tab_listctrl( wxUpdateUIEvent& event );
    
    // Override base class functions
    void OnOK( wxCommandEvent& event );
    void on_help_button( wxCommandEvent& event );

    exclusion_listctrl* m_extensions_exclusion_listctrl;
    exclusion_listctrl* m_urls_exclusion_listctrl;
    wxString            m_configuration_section;
    wxString            m_exclusion_filename;
    wxTextFile          m_textfile;  
        
private:

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_EXCLUSION_DIALOG_H_
