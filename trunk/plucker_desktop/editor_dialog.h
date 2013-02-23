//----------------------------------------------------------------------------------------
/*!
    \file       editor_dialog.h
    \modified
    \copyright  (c) Robert O'Connor ( www.oconnor.md )
    \licence    GPL
    \brief      Describes editor_dialog class
    \author     Robert O'Connor
    \date       2001/10/20
 */
//  RCS-ID:     $Id: editor_dialog.h,v 1.18 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _EDITOR_DIALOG_H_
#define _EDITOR_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "editor_dialog.h"
#endif

// ---------------------------------------------------------------------------------------
// Shared defines
// ---------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTEGRATED_HTML_EDITOR )

// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#include "wx/notebook.h"            // wxNotebook (need the wxNotebookEvent)
#include "wx/dialog.h"

// ---------------------------------------------------------------------------------------

#if ( setupUSE_STYLED_TEXT_CONTROL )
    #include "wx/stc/stc.h"         // StyledTextControl widget
#endif

//----------------------------------------------------------------------------------------
// Class definition: editor_dialog
//----------------------------------------------------------------------------------------

//! Simple HTML editor dialog
/*!
 *  The editor dialog can use a regular text control. Historically, it could either be
 *  complied as having either a regular text control or a wxStyledTextControl depending
 *  on the value of a define in setup.h. This is the reason for the functions having 
 *  names like stc_or_textctrl. I left these function names like this in case I revisit
 *  having a styledtextctrl for the editor, but it was cut to reduce code, ifdefs, 
 *  compile difficulty and easier distribution because less dependencies.
 *  wxStyledTextControl was based on Scintilla and lived in the /contrib of wxWindows.
 *
 *  Feature can be compiled out with setupUSE_INTEGRATED_HTML_EDITOR .
 */
class editor_dialog : public wxDialog
{

public:

    //! Constructor
    /*!
      \param parent The parent window.
      \param filename_to_load The filename of the file to load into the text control.
     */
    editor_dialog( wxWindow* parent,
                   wxString filename_to_load 
                 );

    //! Destructor                 
    ~editor_dialog();
    
private:

    // StyledTextCtrl or TextCtrl functions
    void stc_or_textctrl_init();
    void stc_or_textctrl_load_file( wxString filename );
    void stc_or_textctrl_insert_text( wxString text_to_insert, 
                                      wxString ending_text_to_insert,
                                      bool tag_can_insert_newline );
    void stc_or_textctrl_send_text_to_preview();
    void stc_or_textctrl_save_file();

    void on_notebook_changing( wxNotebookEvent &event );
    
    void on_toolbar_hyperlink_button( wxCommandEvent &event );

    void on_toolbar_mailto_button( wxCommandEvent &event );
    void on_toolbar_bookmark_button( wxCommandEvent &event );
    void on_toolbar_popup_button( wxCommandEvent &event );
    void on_toolbar_paragraph_button( wxCommandEvent &event );
    void on_toolbar_paragraph_left_button( wxCommandEvent &event );    
    void on_toolbar_paragraph_center_button( wxCommandEvent &event ); 
    void on_toolbar_paragraph_right_button( wxCommandEvent &event ); 
    void on_toolbar_paragraph_full_button( wxCommandEvent &event );
    void on_toolbar_break_button( wxCommandEvent &event );
    void on_toolbar_image_button( wxCommandEvent &event );
    void on_toolbar_hr_button( wxCommandEvent &event );
    void on_toolbar_nbsp_button( wxCommandEvent &event );
    
    void on_toolbar_span_button( wxCommandEvent &event );
    void on_toolbar_div_left_button( wxCommandEvent &event );
    void on_toolbar_div_center_button( wxCommandEvent &event );
    void on_toolbar_div_right_button( wxCommandEvent &event );
    void on_toolbar_center_button( wxCommandEvent &event );
    void on_toolbar_blockquote_button( wxCommandEvent &event );
    void on_toolbar_q_button( wxCommandEvent &event );
    void on_toolbar_cite_button( wxCommandEvent &event );

    void on_toolbar_font_button( wxCommandEvent &event );
    void on_toolbar_bold_button( wxCommandEvent &event );
    void on_toolbar_strong_button( wxCommandEvent &event );
    void on_toolbar_italic_button( wxCommandEvent &event );
    void on_toolbar_em_button( wxCommandEvent &event );
    void on_toolbar_underline_button( wxCommandEvent &event );
    void on_toolbar_strike_button( wxCommandEvent &event );
    void on_toolbar_tt_button( wxCommandEvent &event );
    void on_toolbar_sup_button( wxCommandEvent &event );
    void on_toolbar_sub_button( wxCommandEvent &event );
    void on_toolbar_preformatted_button( wxCommandEvent &event );
    void on_toolbar_code_button( wxCommandEvent &event );
    void on_toolbar_h1_button( wxCommandEvent &event );
    void on_toolbar_h2_button( wxCommandEvent &event );
    void on_toolbar_h3_button( wxCommandEvent &event );
    void on_toolbar_h4_button( wxCommandEvent &event );
    void on_toolbar_h5_button( wxCommandEvent &event );
    void on_toolbar_h6_button( wxCommandEvent &event );
    void on_toolbar_small_button( wxCommandEvent &event );

    void on_toolbar_table_button( wxCommandEvent &event );
    void on_toolbar_tr_button( wxCommandEvent &event );
    void on_toolbar_th_button( wxCommandEvent &event );
    void on_toolbar_td_button( wxCommandEvent &event );
    
    void on_toolbar_ordered_list_button( wxCommandEvent &event );
    void on_toolbar_unordered_list_button( wxCommandEvent &event );
    void on_toolbar_list_element_button( wxCommandEvent &event );
    void on_toolbar_menu_button( wxCommandEvent &event );
    void on_toolbar_dt_button( wxCommandEvent &event );
    void on_toolbar_dd_button( wxCommandEvent &event );
    void on_toolbar_dl_button( wxCommandEvent &event );
    
    void on_toolbar_html_button( wxCommandEvent &event );
    void on_toolbar_meta_handheldfriendly_button( wxCommandEvent &event );
    void on_toolbar_body_button( wxCommandEvent &event );
    void on_toolbar_comment_button( wxCommandEvent &event );

    // Override base class functions
    void OnOK( wxCommandEvent &event );
    void on_help_button( wxCommandEvent &event ); 
  
    wxTextCtrl*         m_editor_textctrl; 
    
    // Store the file's filename for when time to write the file.
    wxString            m_editted_filename;

    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_EDITOR_DIALOG_H_
