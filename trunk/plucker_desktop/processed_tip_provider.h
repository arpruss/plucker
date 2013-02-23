//----------------------------------------------------------------------------------------
/*!
    \file       processed_tip_provider.h
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes processed_tip_provider class    
    \author     Robert O'Connor
    \date       2003/03/04
 */
// RCS-ID:      $Id: processed_tip_provider.h,v 1.3 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PROCESSED_TIP_PROVIDER_H_
#define _PROCESSED_TIP_PROVIDER_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "processed_tip_provider.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_STARTUP_TIPS )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/tipdlg.h"              // wxTipProvider and related

// Class definition: processed_tip_provider
//----------------------------------------------------------------------------------------

//! A derived wxTipProvider, which defines some preprocessing of tips.
/*!
    Specifically, it replaces some strings with the application name, and will remove
    some tips that aren't used in a custom deployment.
 */                                                                                                                //----------------------------------------------------------------------------------------

class processed_tip_provider : public wxTipProvider
{

public:

    //! Constructor.
    /*!
        \param filename Full filename to the tips.
        \param current_tip The current tip number in the file
     */
    processed_tip_provider( const wxString& filename,
                            size_t current_tip );

    //! Destructor.
    ~processed_tip_provider();

    //! Gets the tip.
    virtual wxString GetTip();

    /*!
        Allows the derived class to override the wxTipProvider function to
        to modify the tip as soon as it is read. If return wxEmptyString, then
        the tip is skipped, and the next one is read.
     */
    wxString PreprocessTip( const wxString& tip );

    /*!
        Allows the derived class to to modify the tip as after, and just before
        the final showing. Good since we want to keep most of the translation
        the same, except for our replaced part.
     */
    wxString PostprocessTip( const wxString& tip );

private:

    wxTextFile      m_textfile;
    wxArrayString   m_tips_exclusion_filter_array;

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_STARTUP_TIPS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _PROCESSED_TIP_PROVIDER_H_

