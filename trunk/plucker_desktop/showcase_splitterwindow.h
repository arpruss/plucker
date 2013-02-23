//----------------------------------------------------------------------------------------
/*!
    \file       showcase_splitterwindow.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes showcase_splitterwindow class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
//   RCS-ID:      $Id: showcase_splitterwindow.h,v 1.9 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _SHOWCASE_SPLITTERWINDOW_H_
#define _SHOWCASE_SPLITTERWINDOW_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "showcase_splitterwindow.h"
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

#include "wx/splitter.h"

// Forward declaration.
class showcase_dialog;
#include "showcase_dialog.h"

//----------------------------------------------------------------------------------------
// Class definition: showcase_splitterwindow
//----------------------------------------------------------------------------------------

//! A splitterwindow used in the showcase_dialog. 
class showcase_splitterwindow : public wxSplitterWindow
{

    DECLARE_DYNAMIC_CLASS( showcase_splitterwindow )

public:

    //! Constructor.
        /*!
        \param parent The parent window.
        \param id The id of the progress_listbox. Will usually be -1 unless multiple
            of them on the same dialog.
        \param pos The pixel position of the listctrl on its parent window
        \param size The pixel size of the listctrl
        \param style Style of the splitterwindow. See wxWindows wxListBox docs for
             details.
        \param validator Window validator. See wxWindows docs for details.
        \param name Windows name (rarely used).
        \param parent_showcase_dialog The parent showcase_dialog
     */
    showcase_splitterwindow( wxWindow* parent,
                             wxWindowID id = -1,
                             const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxDefaultSize,
                             long style = wxSP_3D,
                             const wxString& name = wxT( "the_showcase_splitterwindow" ),
                             showcase_dialog* parent_showcase_dialog = NULL 
                            );                   
    
    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    showcase_splitterwindow() {};
    
    //! Destructor.
    ~showcase_splitterwindow();        
    
    //! Sets the column widths of the showcase_listctrlcolumns.
    void set_column_widths();

protected:   
    
    /*!
        \note This is the overrideable OnSashPostionChange, not the private OnSashPosChanged.
     */
    bool                OnSashPositionChange( int new_sash_position ); 
    void                on_splitter_sash_position_changed( wxSplitterEvent& event );
    wxWindow*           m_parent;  
    showcase_dialog*    m_parent_showcase_dialog;

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

#endif  // _SHOWCASE_SPLITTERWINDOW_H_
