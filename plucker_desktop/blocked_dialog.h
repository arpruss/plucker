//----------------------------------------------------------------------------------------
/*!
    \file       blocked_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes blocked_dialog class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: blocked_dialog.h,v 1.11 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _BLOCKED_DIALOG_H_
#define _BLOCKED_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "blocked_dialog.h"
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
#include "wx/listctrl.h"

//----------------------------------------------------------------------------------------
// Class definition: blocked_dialog
//----------------------------------------------------------------------------------------

//! Add/edit a blocked string for exclusion lists dialog
/*! 
 *  This dialog is shown when user clicks an 'Add exclusion...' or 'Edit exclusion...'
 *  button on an exclusion_dialog .
 */
class blocked_dialog : public wxDialog
{

public:    

    //! Constructor
    /*!
      \param parent The parent window.
      \param exclusion_string If editing an existing, puts this string into the textctrl.
      \param action If editing an existing, puts this string into the action choice.
      \param priority If editing an existing, puts this string into the priority spinctrl.
     */
    blocked_dialog( wxWindow* parent, 
                    wxString exclusion_string, 
                    long& action,
                    long& priority
                  );

    //! Destructor
    ~blocked_dialog();
    
    //! Transfers the data to the parent dialog.
    /*!
      \param exclusion_string Exclusion string that user entered into the textctrl.
      \param action Action that user entered into the action choice.
      \param priority Priority that user entered into the priority spinctrl.
     */
    void transfer_to( wxString& exclusion_string,
                      long& action,
                      long& priority 
                    );
    
private:
    
    // Override base class functions
    void OnOK( wxCommandEvent& event );
    void on_help_button( wxCommandEvent& event ); 

    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_BLOCKED_DIALOG_H_
