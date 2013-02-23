//----------------------------------------------------------------------------------------
/*!
    \file       plucker_wizard_base.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes plucker_wizard_base class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */   
// RCS-ID:      $Id: plucker_wizard_base.h,v 1.5 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PLUCKER_WIZARD_BASE_H_
#define _PLUCKER_WIZARD_BASE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "plucker_wizard_base.h"
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

// ----------------------------------------------------------------------------------------
// Define an array type for our class (an array of pointers only). Note that 
// WX_DEFINE_ARRAY is only for ints/pointers, not objects. This is just an array of the 
// child wizardpages that were added, so that we can loop through them to chain them,
// set which is the first page, and set the overall wizard size.
// ----------------------------------------------------------------------------------------

WX_DEFINE_ARRAY( wxWizardPageSimple*, wizardpage_array_type );

//----------------------------------------------------------------------------------------
// Class definition: plucker_wizard_base
//----------------------------------------------------------------------------------------

//! Base class for all the wizards used in Plucker Desktop.
/*!
    Cancel buttton shows a dialog with the passed cancel message from the constructor.
 */
class plucker_wizard_base : public wxWizard
{
    DECLARE_ABSTRACT_CLASS( plucker_wizard_base );

public:      
    //! Constructor.
    /*! 
        \param parent The parent window. May be NULL.
        \param title The title of the wizard.
        \param bitmap The default bitmap used in the wizard. May be overriden by a page.
        \param cancel_message The message that appears when user clicks \e Cancel button.
        \todo Enclose cancel message in _() if possible.
     */
    plucker_wizard_base( wxWindow* parent = NULL,
                         int id = -1, 
                         const wxString& title = wxEmptyString,
                         const wxBitmap& bitmap = wxNullBitmap, 
                         const wxString& cancel_message = _( "Abort the wizard?" ) 
                       );
                  
    //! Destructor.
    virtual ~plucker_wizard_base();
    
    //! Call this in constructor of derived class to add a wizardpagesimple to the wizard.
    /*!
        \param wizardpage   The wizardpage to add to the wizard.
     */
    void                    add_wizardpage_to_wizard( wxWizardPageSimple* wizardpage );
       
    //! Call this after you have created an instance of the wizard.
    /*!
        \return TRUE if user clicked the \e Finish button.
     */
    bool                    run_wizard();    
        
    //! Pure virtuals: what to do when the \e Finish button was done, 
    virtual void            on_finish_button()  = 0; 
    
    //! Pure virtual: what to do when the \e Help button was pressed.
    /*!
        param event. The wxWizardEvent, can be queried for GetPage() to know 
            what page was visible when clicked help.
     */
    virtual void            on_help_button( wxWizardEvent& event )    = 0; 
  
private: 

    void                    on_cancel_button( wxWizardEvent& event );
   
    wxWizardPageSimple*     m_first_wizardpage;
        
    wxWindow*               m_parent;
    wxString                m_cancel_message; 
    
    wizardpage_array_type   m_wizardpages_array;
    
    void                    chain_wizardpages();
    void                    set_best_wizard_size();  
    
    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_PLUCKER_WIZARD_BASE_H_
