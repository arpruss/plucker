//----------------------------------------------------------------------------------------
/*!
    \file       progress_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( rob@medicalmnemonics.com )    
    \licence    GPL
    \brief      Describes progress_dialog, process_data_type, and piped_process   
    \author     Robert O'Connor
    \date       2002/01/03    
 */ 
// RCS-ID:      $Id: progress_dialog_non_event_approach[msw].h,v 1.1 2002/08/27 04:28:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PROGRESS_DIALOG_H_
#define _PROGRESS_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "progress_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/dialog.h"
#include "wx/process.h"
#include "progress_listbox.h"

//----------------------------------------------------------------------------------------
// Class definition: progress_dialog
//----------------------------------------------------------------------------------------

//! A dialog that shows the progress of channel updates
/*! 
    It has a listbox that recieves piped_proces information from stdout, and writes it 
    to a listbox. The text is parsed, and some gauges move accordingly. There are some
    buttons on the side to show and hide the listbox, abort the piped process, or save
    the listbox contents to a file.
    
    \note The listbox in the XRC file _must_ have the grow flags set to expand to fit 
    in all directions, otherwise will impinge on top part when details button cuts off 
    that part of the dialog.
    
    \note The wxStaticText labels that get updated are fixed at width of 350, expand to 
    grow, and have wxST_NO_AUTORESIZE style (one or more of these may be unnecessary, 
    but need to not have the newly added text truncate.
 */
class progress_dialog : public wxDialog
{

public: 
 
    //! Constructor
    /*! Should have ability to have a NULL parent if executed from command line.
        \param parent The parent window. May be NULL.
        \param channel_sections The configuration file's channel sections of the 
            channels which will be updated.
        \param install TRUE if should install the channels when done.
    */
    progress_dialog( wxWindow* parent,
                     wxArrayString& channel_sections, 
                     bool install = TRUE );
                     
    //! Destructor                 
    ~progress_dialog();

    //! Returns pointer to listbox. Called by piped process to append the text.
    wxListBox   *get_listbox() const { return m_progress_listbox; }  
        
    //! Let a process tell the dialog that an error occurred
    /*! 
        \todo REMOVE: This won't work as no longer sending stderr.
        Public for piped_process to tell progress_dialog about what is going on.
     */
    void        set_error_occurred( bool error_occurred );    
    
    //! Set whether a process is running.
    /*! 
        Public for piped_process to tell progress_dialog about what is going on.
     */
    void        set_is_process_running( bool is_process_running );
        
    //! Get progress info from stdout to update gauge.
    /*!
        Looks for a "---- 76 collected, 50 to do ----" string and if so, uses it 
        or a update current channel gauge. Also converting and writing data.
     */
    void        extract_progress_from_stderr( const wxString& stdout_string );
    
private:

    bool        update_channels( wxArrayString& channel_sections, bool install=TRUE );  
    bool        execute_command_redirected_to_listbox( const wxString& command_string );
    void        show_listbox();
    void        hide_listbox();
    void        close_dialog();
    void        set_channels_updated_gauge( int value, int range );
    void        set_current_channel_gauge( int value, int range );   
    
    //! Writes TURE or FALSE to the close_on_exit and close_on_error configuration keys.
    /*!
        Need to write close_on_exit=1 / close_on_error=1 for sections to force 
        plucker-build to terminate.
        \todo These certainly shouldn't be here. There are functions in plucker-controller
        for this now.
     */
    void        enable_close_on_exit_and_error( const wxString& section, 
                                                bool only_write_if_key_exists=TRUE, 
                                                bool flush=TRUE );
    
    //! Kills the most recently started plucker-build process
    /*!
        Knows which process to kill, via the process id that we kept track of. 
        \return TRUE if successfully killed or process didn't exist anyways.
     */
    bool        kill_most_recent_process();
    
    //! Called during an EVT_FOCUS_EVENT                                            
    void        on_show( wxPaintEvent& event );
    void        on_update_channels_completion();
    void        on_stop_button( wxCommandEvent &event );
    void        on_details_button( wxCommandEvent &event ); 
    void        on_export_button( wxCommandEvent &event ); 
        
    bool            m_error_occurred;
    int             m_dialog_height_with_listbox;
    bool            m_progress_listbox_is_shown;    
    wxArrayString   m_channel_sections;
    bool            m_install;
    
    //! Pointer to listbox, so piped process knows where to append the text.
    /*!
        Note a listbox appears to be about 2x as fast to insert as a textctrl.
     */
    progress_listbox *m_progress_listbox;
    
    //! TRUE if the progress_dialog::close_dialog code segment has already been executed.
    /*!
        This is needed because somehow have to deal with the fact that don't know
        how we are shutting down the dialog (corner X or with a stop button), and
        need to prevent from the closemodal code executing twice, but still do the 
        kill process stuff when close via the 'X'.        
     */
    bool            m_did_close_dialog_code_execute_yet;
    
    //! TRUE if a process is still running.    
    bool            m_is_process_running;
    
    //! The "pid" (process id) of the most recent process that was initiated.    
    int             m_most_recent_pid;        
    
    //! Whether to start a new plucker-build process in the update_channels() loop.
    /*!
        Set this to false to break the loop, for example when hit "Stop" button.
     */
    bool            m_abort_updating_signal_was_entered;
    
    //! Whether we already initiated the update on start 
    /*!
        Since it is tied to a focus event, and we could have the focus many times.
     */
    bool            m_is_initial_focus_event;
    
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------------------
// Struct definition: process_data_type 
//----------------------------------------------------------------------------------------

//! Holds information about a wxProcess.
/*!
    Used by piped_process
 */
struct process_data_type
{

    //! TRUE if the process is running
    bool    is_running;
    
    //! The exit code that the process returned when it finished.
    int     exit_code;
    
};

//----------------------------------------------------------------------------------------
// Class definition: piped_process
//----------------------------------------------------------------------------------------

//! A class to manage a piped process 
class piped_process : public wxProcess
{

public:

    //! Constructor. 
    /*!
        Reason we need a progress_dialog parameter [and not a wxWindow],
        is so we can find its listbox for output.
        \param parent The parent progress dialog.
        \param process_data Pointer to process data to hold information about the process.
     */
    piped_process( progress_dialog *parent,
                   process_data_type *process_data
                 );

    //! Override wxProcess::OnTerminate virtual
    /*!
        Executed when the process \e pid finishes.
        \param pid The 'p'rocess 'id' (pid) of process that terminated.        
        \param status The exit code of the process.
     */
    virtual void        OnTerminate( int pid,
                                     int status
                                   );
    
    //! Query whether the process has more input remaining in its queue.
    /*!
        \return TRUE if the process has more input remaining.
     */
    bool                has_input(); 

private:

    //! Stores information about the process in a process_data_type.
    process_data_type   *m_process_data;
    
    //! Keep the parent, so can find the listbox to send the output to.
    progress_dialog     *m_parent;
    
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_PROGRESS_DIALOG_H_
