//----------------------------------------------------------------------------------------
/*!
    \file       palm_installer_gtk.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes palm_installer class on GTK
    \author     Robert O'Connor
    \date       2002/10/20    
 */  
// RCS-ID:      $Id: palm_installer_gtk.h,v 1.11 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PALM_INSTALLER_GTK_H_
#define _PALM_INSTALLER_GTK_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "palm_installer_gtk.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin OS removal condition
//----------------------------------------------------------------------------------------

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__)

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "palm_installer_base.h"

//----------------------------------------------------------------------------------------
// Class definition: palm_installer for GTK
//----------------------------------------------------------------------------------------

//! Platform-specific implementaton of palm_installer_base.
class palm_installer : public palm_installer_base
{

    // wx Macro to declare a dynamic class (one that can have new runtime instances)
    DECLARE_DYNAMIC_CLASS( palm_installer );

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

};

//----------------------------------------------------------------------------------------
// End OS removal condition
//----------------------------------------------------------------------------------------
    
#endif  // __WXGTK__ || __WXX11__ || __WXMOTIF__

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // PALM_INSTALLER_GTK_H

