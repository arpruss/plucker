//----------------------------------------------------------------------------------------
/*!
    \file       span_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes span_dialog class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */
// RCS-ID:      $Id: span_dialog.h,v 1.7 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _SPAN_DIALOG_H_
#define _SPAN_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "span_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTEGRATED_HTML_EDITOR )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/dialog.h"

//----------------------------------------------------------------------------------------
// Class definition: span_dialog
//----------------------------------------------------------------------------------------

//! A dialog for a "span" tag for the HTML editor.
class span_dialog : public wxDialog
{

public:      

    //! Constructor.
    /*!
        \param parent The parent window.
     */
    span_dialog( wxWindow* parent );
    
    //! Destructor.
    ~span_dialog();

    //! Gets the tag strings generated by the dialog, ready to insert by parent
    /*!
        \param starting_text The text to place at the start of parent's selected text.
        \param ending_text The text to place at the end of parent's selected text.
     */
    void transfer_to( wxString& starting_text,
                      wxString& ending_text
                    );
      
private:

    void on_help_button( wxCommandEvent& event );  
    //Override base class functions
    void OnOK( wxCommandEvent& event );
    
    wxString m_starting_text;
    wxString m_ending_text;
    
    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_HTML_EDITOR 

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_SPAN_DIALOG_H_

