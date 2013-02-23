//----------------------------------------------------------------------------------------
/*!
    \file       html_tip_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes html_tip_dialog class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: html_tip_dialog.h,v 1.3 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _HTML_TIP_DIALOG_H_
#define _HTML_TIP_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "html_tip_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_HTML_TIP_DIALOG )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/dialog.h"
#include "wx/tipdlg.h"
#include "wx/html/htmlwin.h"

//----------------------------------------------------------------------------------------
// Class definition: html_tip_dialog
//----------------------------------------------------------------------------------------

//! A startup tips dialog, similar to wxTipDialog, but that can handle html.
/*!
    This class shouldn't be used directly. Just call the show_html_tip function--this 
    is similar to the wxShowTip function in wxWindows library.
 */
class html_tip_dialog : public wxDialog
{

public:      

    //! Constructor.
    /*!
        \param parent The parent window.
        \param tip_provider The wxTipProvider that this dialog is associated with.
        \param show_at_startup TRUE to initially check the 'Show at startup' checkbox.
        \param header_html Optional HTML snippet to place before the tip.
        \param header_html Optional HTML snippet to place after the tip.
        \param bool use_system_style Use system's gui sytle for fonts and background color.
        \param html_window_size Size of the html window component of the dialog.
        \param html_window_content_padding Padding from the edge of the html window 
            border to its contents (in pixels).-1 will just use the default setting for
            a wxHtmlWindow.
        \param title A title of the dialog (not in the content window though, just the 
            dialog title).
     */
    html_tip_dialog( wxWindow* parent,
                     wxTipProvider* tip_provider,
                     bool show_at_startup,
                     const wxString& header_html = wxEmptyString,
                     const wxString& footer_html = wxEmptyString, 
                     bool use_system_style = TRUE,                    
                     wxSize html_window_size = wxSize( plkrHTML_TIP_DIALOG_CONTENT_WIDTH,
                                                       plkrHTML_TIP_DIALOG_CONTENT_HEIGHT ),
                     int html_window_content_padding = 0,                                  
                     const wxString& title = _( "Tip of the day" )
                   );     
 
    //! Destructor.
    ~html_tip_dialog();

    //! Query if user entered in the checkbox that s/he wants to "Show tips on startup".
    /*!
        \return TRUE if user wants to show tips on startup.
     */
    bool show_tips_on_startup();

    //! Sets the (next) tip text
    void set_tip_text();

    //! "Next" button handler
    void on_next_tip_button( wxCommandEvent& WXUNUSED( event) );

private:

    wxTipProvider*  m_tip_provider;

    wxHtmlWindow*   m_html_window;
    wxCheckBox*     m_checkbox;

    wxString        m_header_html;
    wxString        m_footer_html;
    bool            m_use_system_style;

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// External functions
//----------------------------------------------------------------------------------------

bool show_html_tip( wxWindow *parent,
                   wxTipProvider *tip_provider,
                   bool show_at_startup,
                   const wxString& header_html = wxEmptyString,
                   const wxString& footer_html = wxEmptyString,
                   bool use_system_style = TRUE,
                   wxSize html_window_size = wxSize( plkrHTML_TIP_DIALOG_CONTENT_WIDTH,
                                                     plkrHTML_TIP_DIALOG_CONTENT_HEIGHT ),
                   int html_window_content_padding = 0,   
                   const wxString& title = _( "Tip of the day" )
                   );

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_HTML_TIP_DIALOG 

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_HTML_TIP_DIALOG_H_

