//----------------------------------------------------------------------------------------
/*!
    \file       main_listctrl.h
    \modified                        
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes main_listctrl class
    \author     Robert O'Connor
    \date       2001/10/29
 */  
// RCS-ID:      $Id: main_listctrl.h,v 1.20 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _MAIN_LISTCTRL_H_
#define _MAIN_LISTCTRL_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "main_listctrl.h"
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
// Class definition: main_listctrl
//----------------------------------------------------------------------------------------

//! Listctrl used in the main_frame .
/*! 
    Currently the hub of the gui management of the non_visual plucker_controller object.
    The main_frame parent just calls this listctrl's methods in response to main toolbar
    and main menubar events.
    
    It works via keeping a non-visible array of strings of the channel sections in 
    m_row_channel_section_array that is kept in sync with the order of the channels in 
    the listctrl.
 */
class main_listctrl : public wxListCtrl
{

    DECLARE_DYNAMIC_CLASS( main_listctrl )

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
    main_listctrl( wxWindow *parent,
                   wxWindowID id = -1,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   long style = wxLC_REPORT,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString &name = wxT( "the_main_listctrl" ) );

    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    main_listctrl() {};
    
    //! Destructor.
    ~main_listctrl();

    //! Initialize the listctrl.    
    void            init();
    
    //! Add a channel in the GUI manner.
    void            gui_add_channel( wxCommandEvent& event );
    
    //! Add a channel using an add_channel_wizard object
    void            gui_add_channel_wizard( wxCommandEvent& event );
    
    //! Handle lauching of a channel_dialog to configure channel currently selected channel..
    /*!
        Note: also used on the list control double-click event.
     */
    void            gui_configure_channel( wxCommandEvent& event );
    void            list_item_activated( wxListEvent& event );
    
    //! Delete the channels selected in the listctrl, in the GUI manner.
    /*!
        \param prompt_first TRUE if should show a confirm message dialog before deleting.
     */     
    void            gui_delete_channels_now ( bool prompt_first = TRUE );
    void            gui_delete_channels( wxCommandEvent& event );
    
    //! Update the selected channels in a GUI way.    
    void            gui_update_selected_channels( wxCommandEvent& event );
    
    //! Update all due channels in a GUI way.    
    void            gui_update_due_channels( wxCommandEvent& event );
    
    //! Update all channels in a GUI way.
    void            gui_update_all_channels( wxCommandEvent& event );
    
    //! Paste text from clipboard as a new channel.
    /*!
        Public, so it can be used by the drop targets.
     */
    void            paste_text_url_as_new_channel( const wxString& url );
    
    //! Paste files from clipboard as separate new channels.
    void            paste_fullnames_as_new_separate_channels( const wxArrayString& fullnames );

    //! Action to execute when user chose 'Paste url...' from a menu. 
    /*!
        Public, so can be used both by the main_frame menuitems, and the popup menu
        item on the main_listctrl.
     */
    void            on_paste_url_as_new_channel_menuitem( wxCommandEvent& event );
    
    //! Action to execute when user chose 'Paste filenames...' from a menu. 
    /*!
        Public, so can be used both by the main_frame menuitems, and the popup menu
        item on the main_listctrl.
     */
    void            on_paste_fullnames_as_new_separate_channels_menuitem( wxCommandEvent& event );
    
    //! Reload rows 
    /*!
        Eg after click "Finish" on setup wizard, to show newly added channels.
     */
    void            reload_rows();

protected:

    //! Loads the rows of the listctrl with channel names and due dates
    void            load_rows();
    
    //! Inserts an individual row into the listctrl.    
    /*!
        \param channel_section Channel section in configuration file, holding the 
            row's channel data to be inserted.
     */
    void            insert_row( wxString &channel_section );
    
    //! Refreshes a row, updating a row's channel name and due date if they have changed.
    /*!
        \param row_number The row number of the listctrl to refresh (0 is top row).
     */
    void            refresh_row( long row_number );
    
    //! Refreshes rows, updating rows' channel name and due date if they have changed.
    /*!
        \param channel_sections The configuration file's sections of the channels to 
            be refreshed.
     */
    void            refresh_rows( wxArrayString& channel_sections );
    
    //! Get the section of configuration file of channels selected in the listctrl
    /*!
        \param channel_sections Array to hold the channel sections.     
     */
    void            get_selected_channel_sections( wxArrayString *channel_sections );
    
    //! Get the row numbers that are selected in the listctrl (with top row=0).
    /*!
        \param channel_sections Array to hold the channel sections.     
     */
    void            get_selected_row_numbers( wxArrayInt *row_numbers );    
    
    //! Gets the string contents of a 'cell' [a box of a certain row, in a certain column].    
    /*!
        \param row_number The row number of the cell [0=top row].
        \param column The column number of the cell [0=left row].
        \return The content string of the cell
     */
    wxString        get_cell_contents_string( long row_number,
                                              int column
                                            );
                                            
    //! Get the wxString of a channel's section in the config, based on its row number.
    /*!
        \param row_number The row number of listctrl to query for a channel_section
            to add to the array.
        \return A channel section name.
     */  
    wxString        get_row_channel_section( long row_number );
    
    //! Set a channel's section in the configuration, by its row number.
    /*! 
        \param channel_section The configruation file's channel section to insert.
        \param row_number The row number of the channel_section to delete from array.
     */    
    void            set_row_channel_section( const wxString& channel_section,
                                             long row_number
                                           );
                                           
    //! Insert a channel section in the array, before the specified index.    
    /*!
        \param channel_section The configruation file's channel section to insert.
        \param row_number The row number of the channel_section to delete from array.
     */                                                    
    void            insert_row_channel_section( const wxString& channel_section,
                                                long row_number
                                              );
    
    //! Deletes a channel section in the array, at the specified index
    /*!
        \param row_number The row number of the channel_section to delete from array.
     */                                         
    void            remove_row_channel_section( long row_number );        
    
    //! A GUI check to ensure that a channel about to be updated has a destination.
    /*!
        Checks to see that some destinations have been chosen for the selected channels
        If not, pops up a channel_dialog for that channel, with the destination tab 
        selected.
        \param channel_sections Array of configuration file channel sections to 
            ensure have each have a destination.
        \return Whether one was needed
     */
    bool            ensure_destinations_provided( const wxArrayString& channel_sections );
    
    //! Popup a context sensitive menu.
    void            popup_menu( wxMouseEvent& event );   
    
    //! Launch a channel_dialog for a newly created channel.
    /*!
        \warning Only use this for newly created channels.
     */
    void            launch_dialog_for_new_channel( const wxString& new_channel_section );

    //! Gets a channel name from a popup text entrry dialog.
    /*!
        \return The channel name that user entered.
     */
    wxString        get_channel_name_from_text_entry_dialog(); 
    
    //! Overriden virtual: action to execute in response to a resize event
    void            OnSize( wxSizeEvent &event );

    //! Sets the column widths. Called during init() an OnSize() .
    void            set_column_widths();
    
    //! Parent window of the listctrl
    wxWindow*       m_parent;    
    
    //! An array of section names of the individual rows of the listctrl.
    wxArrayString   m_row_channel_section_array;  
    
private:

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _MAIN_LISTCTRL_H_
