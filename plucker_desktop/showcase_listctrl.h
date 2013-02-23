//----------------------------------------------------------------------------------------
/*!
    \file       showcase_listctrl.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes showcase_listctrl class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
//   RCS-ID:      $Id: showcase_listctrl.h,v 1.8 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _SHOWCASE_LISTCTRL_H_
#define _SHOWCASE_LISTCTRL_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "showcase_listctrl.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_SHOWCASE )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/listctrl.h"

//----------------------------------------------------------------------------------------
// Class definition: add_channel_wizard
//----------------------------------------------------------------------------------------

//! A listctrl used in the showcase_dialog
/*!
    Populates rows from a file. Clicking on a row gets the details to show
    in the parent managed wxHtmlWindow.
 */
class showcase_listctrl : public wxListCtrl
{

    DECLARE_DYNAMIC_CLASS( showcase_listctrl )

public:

    //! Constructor.
        /*!
        \param parent The parent window.
        \param id The id of the progress_listbox. Will usually be -1 unless multiple
            of them on the same dialog.
        \param pos The pixel position of the listctrl on its parent window
        \param size The pixel size of the listctrl
        \param style Style of the listbox. See wxWindows wxListBox docs for details.
        \param validator Window validator. See wxWindows docs for details.
        \param name Windows name (rarely used).
     */
    showcase_listctrl( wxWindow* parent,
                       wxWindowID id = -1,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxLC_REPORT,
                       const wxValidator& validator = wxDefaultValidator,
                       const wxString& name = wxT( "the_showcase_listctrl")
                     );   

    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    showcase_listctrl() {};
    
    //! Destructor.                               
    ~showcase_listctrl();    
    
    //! Sets the column widths of the columns.
    /*! 
        Needs to be public so the parent can tell to resize them as needed when the sash
        position has changed.
        \param maximum_listctrl_width The maximum width of the whole listctrl.
     */
    void set_column_widths( int maximum_listctrl_width );

protected:        
    
    wxWindow*     m_parent;  
    
private:

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_SHOWCASE

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_SHOWCASE_LISTCTRL_H_
