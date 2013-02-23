//----------------------------------------------------------------------------------------
/*!
    \file       about_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes about_dialog class    
    \author     Robert O'Connor
    \date       2002/08/22    
 */  
// RCS-ID:      $Id: about_dialog.h,v 1.3 2007/06/29 00:58:49 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _ABOUT_DIALOG_H_
#define _ABOUT_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "about_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_ENHANCED_ABOUT_DIALOG )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/dialog.h"

//----------------------------------------------------------------------------------------
// Class definition: about_dialog
//----------------------------------------------------------------------------------------

//! A dialog for a "body" tag for the HTML editor.
class about_dialog : public wxDialog
{

public:      

    //! Constructor.
    /*!
        \param parent The parent window.
     */
    about_dialog( wxWindow* parent );
  
    //! Destructor.
    ~about_dialog();

private:

    //! Sets the html of the Plucker credits.
    void set_plucker_credits_html();  
    
    //! Sets the html of the Plucker Desktop credits.
    void set_plucker_desktop_credits_html();
 
    //! Make a section header of an html page.
    wxString create_section_header( const wxString& section );
 
    //! Get a string of an entire textfile.
    wxString create_string_from_file( const wxString& fullname );
 
    //! Does a runtime replacement of the version and build date.text
    void set_version_and_build_date_text();

    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif   // setupUSE_ENHANCED_ABOUT_DIALOG 

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_ABOUT_DIALOG_H_

