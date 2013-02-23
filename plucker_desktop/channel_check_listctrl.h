//----------------------------------------------------------------------------------------
/*!
    \file       channel_check_listctrl.h
    \modified                        
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes channel_check_listctrl class
    \author     Robert O'Connor
    \date       2001/10/29
 */  
// RCS-ID:      $Id: channel_check_listctrl.h,v 1.4 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _CHANNEL_CHECK_LISTCTRL_H_
#define _CHANNEL_CHECK_LISTCTRL_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "channel_check_listctrl.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_WIZARDS )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/listctrl.h"

//----------------------------------------------------------------------------------------

#include "channel_listctrl_base.h"

//----------------------------------------------------------------------------------------
// Class definition: channel_check_listctrl
//----------------------------------------------------------------------------------------

//! Listctrl of channels that can be selected or deselected.
/*!
    Uses the icons of the listctrl to toggle between checked and unchecked checkboxes.
 */
class channel_check_listctrl : public channel_listctrl_base
{

    DECLARE_DYNAMIC_CLASS( channel_check_listctrl )

public:

    //! Constructor.
        /*!
        \param parent The parent window.
        \param id The id of the progress_listbox. Will usually be -1 unless multiple
            of them on the same dialog.
        \param from_configuration The wxFileConfig to read the channels from.
        \param image_list_index The index of icon in the image_list to use for 
            newly inserted rows.
        \param options Class option flags.
        \param pos The pixel position of the listctrl on its parent window
        \param size The pixel size of the listctrl
        \param style Style of the listbox. See wxWindows wxListBox docs for details.
        \param validator Window validator. See wxWindows docs for details.
        \param name Windows name (rarely used).
     */
    channel_check_listctrl( wxWindow *parent,
                   wxWindowID id = -1,
                   wxFileConfig* from_configuration = the_configuration,
                   int default_image_list_index = plkrSMALL_IMAGE_LIST_ID_CHECKBOX_CHECKED,
                   long options = 0,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   long style = wxLC_REPORT,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString &name = wxT( "the_channel_check_listctrl") );

    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    channel_check_listctrl() {};

    //! Destructor.
    ~channel_check_listctrl();

    //! Gets a wxArrayString of channel sections that are checked in the list.
    /*!
        \param channel_sections Pointer to array to store the channel sections.
        \note Different from get_selected_channel_sections function
            which is the ones that are cursor highlighted.
     */
    void get_checked_channel_sections( wxArrayString* channel_sections );

protected:

    //! Popup a context sensitive menu.
    virtual void    popup_menu( wxMouseEvent& event );

    //! Inserts the columns for the listctrl.
    virtual void    insert_columns();

    //! Sets the column widths. Called during init() an OnSize() .
    virtual void    set_column_widths();

private:

    //! Ask whether row is checked
    /*!
        \param row Row to examine
        \return TRUE if row is checked.
     */
    bool is_row_checked( long row ) const;

    //! Set whether row is checked
     /*!
        \param row Row to set checked or unchecked.
        \return TRUE to check row, FALSE to uncheck row.
     */
    void set_row_checked( long row,
                          bool checked
                         );

    //! Fires during a mouse event
    void on_mouse_event( wxMouseEvent& event );

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_WIZARDS

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _CHANNEL_CHECK_LISTCTRL_H_
