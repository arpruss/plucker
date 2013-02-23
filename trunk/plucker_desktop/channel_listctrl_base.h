//----------------------------------------------------------------------------------------
/*!
    \file       channel_listctrl_base.h
    \modified                        
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes channel_listctrl_base class
    \author     Robert O'Connor
    \date       2001/10/29
 */  
// RCS-ID:      $Id: channel_listctrl_base.h,v 1.5 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _CHANNEL_LISTCTRL_BASE_H_
#define _CHANNEL_LISTCTRL_BASE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "channel_listctrl_base.h"
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

#include "configuration.h"
#include "image_list.h"

//----------------------------------------------------------------------------------------
// Class option flags
//----------------------------------------------------------------------------------------

enum {
    //! Only for main_listctrl
    optionLISTCTRL_DONT_INITIALIZE_LISTCTRL_CONTENT_ON_CONTRUCTION = 1,
    //! Currently only for main_listctrl, but would be for showcase also if multi row.
    optionLISTCTRL_INSERT_NAME_ON_REFRESH_NOT_INSERTION            = 2
};

//! Easier code maintenance if want to add/rearrange listctrl's columns.
enum {
    NAME_COLUMN     = 0,         // Channel's name
    DUE_COLUMN,
    URL_COLUMN,
    DESCRIPTION_COLUMN,
    LANGUAGE_COLUMN,
    CATEGORY_COLUMN,
    UPDATE_FREQUENCY_COLUMN,
    UPDATE_PERIOD_COLUMN,
    UPDATE_BASE_COLUMN,    
    UPDATE_ENABLED_COLUMN,
    BPP_COLUMN,
    MAXDEPTH_COLUMN,
    STAYONHOST_COLUMN,
    STAYONDOMAIN_COLUMN,
    URL_PATTERN_COLUMN          // When channel is due
};

//----------------------------------------------------------------------------------------
// Class definition: channel_listctrl_base
//----------------------------------------------------------------------------------------

//! Listctrl of channels
    /*!
        Abstract class eventually of the main_listctrl, showcase_listctrl, and
        channel_check_listctrl. Currently only baseclass of channel_check_listctrl.
    */
class channel_listctrl_base : public wxListCtrl
{

    DECLARE_ABSTRACT_CLASS( channel_listctrl_base )

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
    channel_listctrl_base( wxWindow *parent,
                   wxWindowID id = -1,
                   wxFileConfig* from_configuration = the_configuration,
                   int default_image_list_index = plkrSMALL_IMAGE_LIST_ID_CHECKBOX_CHECKED,
                   long options = 0,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   long style = wxLC_REPORT,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString &name = wxT( "the_channel_listctrl_base" ) );

    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    channel_listctrl_base() {};

    //! Destructor (virtual, since a base class).
    virtual ~channel_listctrl_base();

    //! Initialize the listctrl content (except the rows).
    /*!
        Does 3 things:
        (1) Set the wxImageList to our global image_list module
        (2) Calls load_rows();
        (3) Calls set_column_widths to adjusts the column widths
        Use the constructor flag of
        optionLISTCTRL_DONT_INITIALIZE_LISTCTRL_CONTENT_ON_CONTRUCTION if
        you don't want it to take place in construction yet (eg for the
        main_listctrl, so that we can show the splashscreen first, and then
        start to initialize the content).
     */
    void            initialize_listctrl_content();

    //! Reloads the rows of the listctrl
    /*!
        Shows a busy cursor, then clears the rows (and section), then
        calls load_rows function.
     */
    void            reload_rows();

protected:

    //! Loads the rows of the listctrl with channel names and due dates
    void            load_rows();

    //! The function that will add a row to the listctrl
    /*!
        This function will just add the a row. If
        optionLISTCTRL_INSERT_NAME_ON_REFRESH_NOT_INSERTION
        was included options, then it will just insert a space instead of the actual
        name, since the name will be inserted on the derived class's refresh_row
        anyways. If optionLISTCTRL_INSERT_NAME_ON_REFRESH_NOT_INSERTION wan't
        included in options, then the name is inserted now.
        The end of this function call refresh_row.
     */
    virtual void    insert_row( const wxString &channel_section );

    //! Refreshes the row.
    /*!
        In base class does nothing. Override as desired.
     */
    virtual void    refresh_row( long row_number );

    //! Get the section of configuration file of channels selected in the listctrl
    /*!
        \param channel_sections Array to hold the channel sections.
     */
    void            get_selected_channel_sections( wxArrayString *channel_sections );


    //! Get the row numbers that are selected in the listctrl (with top row=0).
    /*!
        \param channel_sections Array to hold the channel sections.
        \todo Should go into a more base listctrl.
     */
    void            get_selected_row_numbers( wxArrayInt *row_numbers );

    //! Gets the string contents of a 'cell' [a box of a certain row, in a certain column].
    /*!
        \param row_number The row number of the cell [0=top row].
        \param column The column number of the cell [0=left row].
        \return The content string of the cell
        \todo Should go into a more base listctrl.
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

    //! Popup a context sensitive menu.
    virtual void    popup_menu( wxMouseEvent& event );

    //! Overriden virtual: action to execute in response to a resize event
    void            OnSize( wxSizeEvent &event );

    //! Inserts the columns for the listctrl.
    virtual void    insert_columns() = 0;

    //! Sets the column widths. Called during init() an OnSize() .
    virtual void    set_column_widths() = 0;

    //! Parent window of the listctrl
    wxWindow*       m_parent;

    //! Default image list index for newly inserted rows.
    int             m_default_image_list_index;
    
    //! The file configuration object that we are getting the sections from.
    wxFileConfig*   m_configuration;
    
    //! Options
    long            m_options;

    //! An array of section names of the individual rows of the listctrl.
    wxArrayString   m_row_channel_section_array;

private:

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _CHANNEL_LISTCTRL_BASE_H_
