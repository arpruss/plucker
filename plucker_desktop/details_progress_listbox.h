//----------------------------------------------------------------------------------------
/*!
    \file       details_progress_listbox.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes details_progress_listbox class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
//  RCS-ID:     $Id: details_progress_listbox.h,v 1.8 2007/06/30 03:09:28 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _DETAILS_PROGRESS_LISTBOX_H_
#define _DETAILS_PROGRESS_LISTBOX_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "details_progress_listbox.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_ENHANCED_PROGRESS_DIALOG )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/listbox.h"

//----------------------------------------------------------------------------------------
// Class option flags
//----------------------------------------------------------------------------------------

// Sent to the progress listbox constructor. Not this classes job.
enum {
    //! Max workable scroll history for the OS (GTK =1999, rest is int).
    optionDETAILS_PROGRESS_LISTBOX_MAXIMUM_SCROLL_HISTORY               = -1,
    //! Recommended scroll history to keep things from slowing down.    
    optionDETAILS_PROGRESS_LISTBOX_RECOMMENDED_SCROLL_HISTORY           = -2,
    //! Inifite scroll history.
    optionDETAILS_PROGRESS_LISTBOX_INFINITE_SCROLL_HISTORY              = -3
};

//----------------------------------------------------------------------------------------
// Class definition: progress_dialog
//----------------------------------------------------------------------------------------

//! A list box used in the progress_dialog
/*!
    It can take data recieved from piped_process and display it inside the listbox.
 */
class details_progress_listbox : public wxListBox
{

    DECLARE_DYNAMIC_CLASS( details_progress_listbox )

public:

    //! Constructor.
        /*!
        \param parent The parent window. May be NULL.
        \param id The id of the details_progress_listbox. Will usually be -1 unless multiple
            of them on the same dialog.
        \param log_filename  Filename of the process logfile. 
            A plkrFILE_FRIENDLY_DATETIME_STAMP in the string will replace with a file-friendly datetime string.
            If using DATETIMESTAMP, recommended to keep the rest of name below 11 characters
            total if need Macintosh 31-character limit requirement.
        \param log_path Default path of the logfile in dialog.
            Default of wxEmptyString means the current directory.
        \param scroll_history Number of lines to keep in the listbox at one time. 
            After this, newly appended lines will remove the topmost line. 
            Can be a number, or can use one of the options* to get a recommended for 
            OS or an infinite one.
        \param pos The pixel position of the listbox on its parent window
        \param size The pixel size of the listbox
        \param n Number of strings to initialize in the control
        \param choices An array of strings to initialize the control
        \param style Style of the listbox. See wxWindows wxListBox docs for details.
        \param validator Window validor. See wxWindows docs for details.
        \param name Windows name (rarely used).
     */
    details_progress_listbox( wxWindow* parent,
                      wxWindowID id = -1,
                      const wxString& log_filename = wxEmptyString,
                      const wxString& log_path = wxEmptyString,    
                      long  scroll_history = optionDETAILS_PROGRESS_LISTBOX_RECOMMENDED_SCROLL_HISTORY,                              
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      int n = 0, 
                      const wxString choices[] = NULL,
                      long style = wxLB_EXTENDED | wxLB_HSCROLL | wxLB_ALWAYS_SB, 
                      const wxValidator& validator = wxDefaultValidator, 
                      const wxString& name = wxT( "a_details_progress_listbox" ) );           

    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    details_progress_listbox() {};

    //! Destructor.
    ~details_progress_listbox();   
     
    //! Append() but will scroll off the top line if past the max scroll history
    /*!
        \param item The [text] item to append.
     */ 
    void append_using_scroll_history( const wxString& item );

    
    //! Handles the exporation of the listbox contents to a user specified file.
    /*! 
        This is called when user clicks the 'Export to File' button on the side
        of the progress_dialog .
        \return TRUE if operation was successful.
     */
    bool            export_to_file();

protected:      
    
    //! Pops up a menu
    void            popup_menu( wxMouseEvent& event );       
    
    //! Code to execute when user selects 'Copy' from the context-sensitive menu.
    void            on_copy_menu_event( wxCommandEvent& event );
    
    //! Code to execute when user selects 'Clear' from the context-sensitive menu.
    void            on_clear_menu_event( wxCommandEvent& event );
    
    //! Code to execute when user selects 'Select All' from the context-sensitive menu.
    void            on_select_all_menu_event( wxCommandEvent& event );

    //! Parent window
    wxWindow        *m_parent;    
    
    //! An array of selected rows to copy (so don't have to look it up twice)
    wxArrayInt      m_selected_row_numbers_array;  

    //! Passed default filename of the log file when user wants to export.
    wxString        m_log_filename;
    
    //! Passed default path of the log file when user wants to export.
    wxString        m_log_path;
    
    //! Store the scroll history (number of lines that can be in the listbox at once)
    long            m_scroll_history;

private:

    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ENHANCED_PROGRESS_DIALOG

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_DETAILS_PROGRESS_LISTBOX_H_
