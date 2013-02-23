//----------------------------------------------------------------------------------------
/*!
    \file       exclusion_listctrl.h        
    \modified
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes exclusion_listctrl class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */
// RCS-ID:      $Id: exclusion_listctrl.h,v 1.13 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _EXCLUSION_LISTCTRL_H_
#define _EXCLUSION_LISTCTRL_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "exclusion_listctrl.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/listctrl.h"

//----------------------------------------------------------------------------------------
// Class definition: exclusion_listctrl
//----------------------------------------------------------------------------------------

//! Listctrl used in the exclusion_dialog .
class exclusion_listctrl : public wxListCtrl
{
    DECLARE_DYNAMIC_CLASS( exclusion_listctrl )

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
        \param exclusion_column_caption The label of header of listctrl's exclusion
            column.
     */
    exclusion_listctrl( wxWindow* parent,
                   wxWindowID id = -1,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxLC_REPORT,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxT( "the_exclusion_listctrl" ),
                   const wxString& exclusion_column_caption = _( "URL" ) );                   
    
    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    exclusion_listctrl() {};

    //! Destuctor.
    ~exclusion_listctrl();    

    //! Query whether listctrl is dirty, that is a user modified at least one entry.
    /*!
        \return TRUE if user modified at least one entry.
     */   
    bool get_is_listctrl_dirty();
    
    //! Action performed when parent window or popup tells it should do an add command.
    void on_add_command(wxCommandEvent&);
    
    //! Action performed when parent window or popup tells it should do an edit command.
    void on_edit_command(wxCommandEvent&);

    //! Action performed when edit control has focus, and is activated
    void on_edit_activated(wxListEvent&);
    
    //! Action performed when parent window or popup tells it should do a delete command.
    void on_delete_command(wxCommandEvent&);

protected:        

    long get_selected_item_id();
    long add_empty_row();
    void edit_row( long row ) ;
    void delete_selected_row();        
    
    // Override base class functions
    void OnOK( wxCommandEvent& event );
    void on_help_button( wxCommandEvent& event ); 

    void popup_menu( wxMouseEvent& event );       
    void OnSize( wxSizeEvent& event );
    void set_column_widths();    
    
    wxWindow* m_parent;    
    bool m_listctrl_is_dirty; 

private:

    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// Global functions and variables
//----------------------------------------------------------------------------------------

//! Columns of the exclusion dialog
/*! 
    Allows easier code maintenance if want to add/rearrangement of listctrl's 
    columns. Shared between exclusion_listctrl and exclusion_dialog, so brought 
    outside of class.
 */
enum {
    EXCLUSION_COLUMN    = 0,
    ACTION_COLUMN,
    PRIORITY_COLUMN
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_EXCLUSION_LISTCTRL_H_
