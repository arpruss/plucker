//----------------------------------------------------------------------------------------
/*!
    \file       art_provider.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes art_provider class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: art_provider.h,v 1.5 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _ART_PROVIDER_H_
#define _ART_PROVIDER_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "art_provider.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/artprov.h"

//----------------------------------------------------------------------------------------
// Class definition: art_provider
//----------------------------------------------------------------------------------------

//! A derived wxArtProvider, which allows runtime replacement of default wxWindows icons.
/*! 
    See wxArtProvider section in wxWindows documentation for details.
 */
class art_provider : public wxArtProvider
{

protected:

    //! The single function required to override from base class to use wxArtProvider.
    /*!
        See wxWindows documentation for full details, but a description of the parameters:
        \param id The ID number of the bitmap being requested
        \param client A hint of who asked for the icon (a toolbar, messagebox, etc)
        \parma size What size they are asking for.
     */
    virtual wxBitmap CreateBitmap( const wxArtID& id,
                                   const wxArtClient& client,
                                   const wxSize& size
                                 );

private:
    
    //! Gets an art file based on its basename. Used internally by CreateBitmap() .
    /*!
        \param image_basename The basename of image to be looked up and created.
     */
    wxBitmap get_art_by_basename( const wxString& image_basename );

                                 
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _ART_PROVIDER_H_

