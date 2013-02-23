//----------------------------------------------------------------------------------------
/*!
    \file       palm_installer_msw.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes palm_installer class on MSW
    \author     Robert O'Connor
    \date       2002/10/20    
 */  
// RCS-ID:      $Id: palm_installer_msw.h,v 1.9 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PALM_INSTALLER_MSW_H_
#define _PALM_INSTALLER_MSW_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "palm_installer_msw.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin OS removal condition
//----------------------------------------------------------------------------------------

#ifdef __WXMSW__

// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#include "palm_installer_base.h"
#include "wx/config.h"               // wxRegConfig

//----------------------------------------------------------------------------------------
// Class definition: palm_installer for MSW
//----------------------------------------------------------------------------------------

//! Platform-specific implementaton of palm_installer_base.
class palm_installer : public palm_installer_base
{

    // wx Macro to declare a dynamic class (one that can have new runtime instances)
    DECLARE_DYNAMIC_CLASS(palm_installer);

public:

    //! Constructor.    
    palm_installer();
    
    //! Destructor.
    ~palm_installer();
    
    virtual void    load_user_data();
    
    virtual bool    install_files_by_handheld_dest( handheld_dest_type& handheld_dest,
                                                    wxArrayString& install_fullnames );

    virtual bool    install_file_by_handheld_dest( handheld_dest_type& handheld_dest,
                                                   const wxString& install_fullname );

private:   

    wxConfig*       m_palm_user_config;       // A wxRegConfig under WXMSW

};

//----------------------------------------------------------------------------------------
// End OS removal condition
//----------------------------------------------------------------------------------------
    
#endif  // __WXMSW__

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // PALM_INSTALLER_MSW_H
