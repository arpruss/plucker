///////////////////////////////////////////////////////////////////////////////////////////
// Name:        main_listctrl.h
// Purpose:     Listctrl used in the main dialog
// Author:      Robert O'Connor
// RCS-ID:      $Id: main_listctrl_[using_arraystring_not_zero_column].h,v 1.1 2002/04/02 22:55:53 robertoconnor Exp $
// Copyright:   (c) Robert O'Connor
// Licence:     GPL
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WX_MAIN_LISTCTRL_H__
#define __WX_MAIN_LISTCTRL_H__

#ifdef __GNUG__
	#pragma interface "main_listctrl.h"
#endif

#include "wx/listctrl.h"

class WXDLLEXPORT wxListCtrl;
class WXDLLEXPORT wxListEvent;

//----------------------------------------------------------------------------------------
// Define a new listctrl type
//----------------------------------------------------------------------------------------
  
class WXDLLEXPORT main_listctrl : public wxListCtrl
{
	DECLARE_CLASS(main_listctrl);

public:
    main_listctrl( wxWindow *parent,
                   wxWindowID id = -1,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   long style = wxLC_REPORT,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString &name = "the_main_listctrl");                   
    ~main_listctrl();
    
    void            init();
    void            gui_add_channel();
    void            gui_configure_channel();
    void            gui_delete_channels( bool prompt_first=TRUE );
    void            gui_update_selected_channels();
    void            gui_update_due_channels();
    void            gui_update_all_channels();
    
    // Public, so it can be used by the drop targets
    void            paste_text_url_as_new_channel( const wxString& url );
    void            paste_fullnames_as_new_separate_channels( const wxArrayString& fullnames );
    
protected:   
    void            load_rows();
    void            insert_row( wxString &channel_section );
    void            refresh_row( long row_number );
    void            refresh_rows_due_column( wxArrayString& channel_sections );
    void            get_selected_channel_sections( wxArrayString *channel_sections );
    void            get_selected_row_numbers( wxArrayInt *row_numbers );    
    wxString        get_cell_contents_string( long row_number, int column );
    wxString        get_row_channel_section( long row_number );
    void            set_row_channel_section( const wxString& channel_section, long row_number );
    void            insert_row_channel_section( const wxString& channel_section, long row_number );
    void            remove_row_channel_section( long row_number );        
    void            popup_menu( wxMouseEvent& event );       
    void            on_paste_menu_event( wxCommandEvent& event );    
    void            launch_dialog_for_new_channel( const wxString& new_channel_section );
    wxString        get_channel_name_from_text_entry_dialog(); 
    void            OnSize( wxSizeEvent &event );
    void            set_column_widths();
    wxWindow        *m_parent;    
    // An array of section names of the individual rows of the listctrl.
    wxArrayString   m_row_channel_section_array;

private:
    DECLARE_EVENT_TABLE()
};

#endif  //__WX_MAIN_LISTCTRL_H__

