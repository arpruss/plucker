//----------------------------------------------------------------------------------------
/*!
    \file       limits_wizardpage.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes limits_wizardpage class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: limits_wizardpage.h,v 1.5 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _LIMITS_WIZARDPAGE_H_
#define _LIMITS_WIZARDPAGE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "limits_wizardpage.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"        // Also for plkrNEW_CHANNEL_TEMPLATE_SECTION

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/wizard.h"
#include "wx/spinctrl.h"

//----------------------------------------------------------------------------------------
// Class definition: limits_wizardpage
//----------------------------------------------------------------------------------------

//! A wizard page with controls for limits of a channel.
/*!
    Contains controls for maximum depth, stayonhost, etc.
 */
class limits_wizardpage : public wxWizardPageSimple
{

public:     
 
    //! Constructor.
    /*!
        \param parent The parent wizard.
        \param channel_section The default channel section from which to initialize 
            the values for the controls.
        \param bitmap Optional bitmap to show instead of the one the parent wizard is 
            using.
     */
    limits_wizardpage( wxWizard *parent, 
                       const wxString& channel_section = plkrNEW_CHANNEL_TEMPLATE_SECTION,
                       wxBitmap bitmap = wxNullBitmap
                     );
    
    //! Destructor.                
    ~limits_wizardpage();
    
    //! Get the value of the m_maxdepth_spinctrl.
    /*! 
        \return The maxdepth value.
     */    
    int get_maxdepth_value();
    
    //! Get the value of the m_stayondomain_radiobutton.
    /*! 
        \return TRUE if user selected to enabled stayondomain.
     */   
    bool get_stayondomain_value();
    
    //! Get the value of the m_stayonhost_radiobutton.
    /*! 
        \return TRUE if user selected to enabled stayonhost.
     */   
    bool get_stayonhost_value();

private:

    void on_update_ui_stayonhost_stayondomain_checkbox( wxUpdateUIEvent& event );
    void read_configuration( const wxString& channel_section );

    wxSpinCtrl*         m_maxdepth_spinctrl;
    wxCheckBox*         m_stayonhost_stayondomain_checkbox;
    wxRadioButton*      m_stayonhost_radiobutton;
    wxRadioButton*      m_stayondomain_radiobutton;
    
    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_LIMITS_WIZARDPAGE_H_
