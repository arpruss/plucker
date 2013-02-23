//----------------------------------------------------------------------------------------
/*!
    \file       pda_listctrl.h        
    \modified
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes pda_listctrl class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */
// RCS-ID:      $Id: pda_listctrl.h,v 1.6 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PDA_LISTCTRL_H_
#define _PDA_LISTCTRL_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "pda_listctrl.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/listctrl.h"

//----------------------------------------------------------------------------------------
// Class option flags
//----------------------------------------------------------------------------------------

enum {
    //! Whether user or directories listctrl ( since different number of columns )
    optionIS_USERS_LISTCTRL = 1,
    optionIS_DIRECTORIES_LISTCTRL = 2
};

//----------------------------------------------------------------------------------------
// Class definition: pda_listctrl
//----------------------------------------------------------------------------------------

// List control with a single column that auto-resizes its column to the
// width of the control, minus some right hand space for the scrollbar:
//! \todo This needs to go into its own class too.
class pda_listctrl : public wxListCtrl
{
    DECLARE_DYNAMIC_CLASS( pda_listctrl )

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
        \param column_caption The label of 1st column in listctrl
            column.
        \param users_or_directories_listctrl Whether a users or directories listctrl.
    */

    pda_listctrl( wxWindow *parent,
                  wxWindowID id = -1,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long listCtrlStyle = wxLC_ICON,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString &name = wxT( "listctrl" ),
                  const wxString &column_caption = _( "User" ),
                  long  users_or_directories_listctrl = optionIS_USERS_LISTCTRL );

    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    pda_listctrl() {};

    //! Destuctor.
    ~pda_listctrl();

    void set_column_widths();

    //! Gets the string contents of a 'cell' [a box of a certain row, in a certain column].
    /*!
        \param row_number The row number of the cell [0=top row].
        \param column The column number of the cell [0=left row].
        \return The content string of the cell
     */
    wxString get_cell_contents_string( long row_number,
                                       int column );

    void find_matching_rows( wxArrayString& input_arraystring,
                             wxArrayInt *output_matching_rows_arrayint );

protected:

    void OnSize( wxSizeEvent& event );

private:

    long    m_users_or_directories_listctrl;

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// Global functions and variables
//----------------------------------------------------------------------------------------

//! Columns of the pda_listctrl
/*!
    Allows easier code maintenance if want to add/rearrangement of listctrl's
    columns. Shared between pda_listctrl and exclusion_dialog, so brought
    outside of class.
 */
enum
{
    USER_NAME_COLUMN        = 0,
    RAM_OR_CARD_COLUMN,
    //DIRECTORY_ON_CARD_COLUMN,
    IS_USB_PAUSE_COLUMN
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_PDA_LISTCTRL_H_
