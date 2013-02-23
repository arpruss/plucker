//----------------------------------------------------------------------------------------
/*!
    \file       palm_installer_mac.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes palm_installer class on Macintosh (OSX)
    \author     Robert O'Connor
    \date       2002/10/20    
 */  
// RCS-ID:      $Id: palm_installer_mac.h,v 1.3 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PALM_INSTALLER_MAC_H_
#define _PALM_INSTALLER_MAC_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "palm_installer_mac.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin OS removal condition
//----------------------------------------------------------------------------------------

#ifdef __WXMAC__

// ---------------------------------------------------------------------------------------
// Headers
// ---------------------------------------------------------------------------------------

#include "palm_installer_base.h"

//----------------------------------------------------------------------------------------
// Class definition: palm_installer for Macintosh
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
       
    //! The install path for cards on OSX depends on translation.
    wxString        get_translated_handheld_destination_path( const wxString& destination_rootpath,
                                                   const wxString& card_subdirectory,
                                                   const wxString& card_subdirectory_guess_filter );


};

//----------------------------------------------------------------------------------------
// End OS removal condition
//----------------------------------------------------------------------------------------
    
#endif  // __WXMAC__

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // PALM_INSTALLER_MAC_H
