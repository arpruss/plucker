//----------------------------------------------------------------------------------------
/*!
    \file       viewer_wizardpage.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes viewer_wizardpage class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */
// RCS-ID:      $Id: viewer_wizardpage.h,v 1.8 2007/06/29 00:58:58 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _VIEWER_WIZARDPAGE_H_
#define _VIEWER_WIZARDPAGE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "viewer_wizardpage.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/wizard.h"

//----------------------------------------------------------------------------------------
// Class definition: viewer_wizardpage
//----------------------------------------------------------------------------------------

//! A wizard page that can collect information about the viewer and software to install.
class viewer_wizardpage : public wxWizardPageSimple
{

public:     

    //! Constructor.
    /*!
        \param parent The parent wizard.
        \param bitmap Optional bitmap to show instead of the one the parent wizard is 
            using.
     */     
    viewer_wizardpage( wxWizard* parent,
                       wxBitmap bitmap = wxNullBitmap
                     );
    //! Destructor
    ~viewer_wizardpage();
    
    //! Get a list of the fullnames of the files to be installed.
    /*! 
        \param wxArrayString* fullnames_to_install An array to hold the fullnames.
     */
    void    get_fullnames_to_install( wxArrayString* fullnames_to_install );    
    
    //! Gets the selected bpp (bits per pixel of their selected device).
    /*! 
        This is used for example in the setup_wizard to automatically set the 
        bpp value of any default channels that they have.
        \return The selected bpp.
     */
    long    get_bpp();
        
    //! Returns true if is a hires device that is selected
    /*
        If an error somehow, and nothing is selected, it will also return false
     */
    bool	get_is_hires_viewer_selected();
        
private:

    //! Initialize the wizardpage
    void        wizardpage_initalize();
    
    wxChoice*   m_device_choice; 
    wxChoice*   m_bpp_choice;       // bpp is 'bits per pixel' (color depth of device)
    wxChoice* 	m_is_usb_pause_choice;
    wxChoice*   m_language_choice;
    wxCheckBox* m_install_viewer_checkbox;    
    wxCheckBox* m_install_zlib_checkbox;
    wxCheckBox* m_install_manual_checkbox;  

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _VIEWER_WIZARDPAGE_H_
