//----------------------------------------------------------------------------------------
/*!
    \file       process_progress_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes process_progress_dialog class  
    \author     Robert O'Connor
    \date       2002/01/03    
 */ 
// RCS-ID:      $Id: process_progress_dialog.h,v 1.7 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PROCESS_PROGRESS_DIALOG_H_
#define _PROCESS_PROGRESS_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "process_progress_dialog.h"
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

#include "details_progress_dialog.h"

// Forward declaration.
class piped_process;
#include "piped_process.h"

//----------------------------------------------------------------------------------------
// Class option flags
//----------------------------------------------------------------------------------------

enum {
    optionPROCESS_PROGRESS_DIALOG_NONE_STREAM_KIND                      = 0,
    optionPROCESS_PROGRESS_DIALOG_STDOUT_STREAM_KIND                    = 1,
    optionPROCESS_PROGRESS_DIALOG_STDERR_STREAM_KIND                    = 2
};

//----------------------------------------------------------------------------------------
// Array definitions
//----------------------------------------------------------------------------------------

// Define an array of pointers to piped_process objects.
WX_DEFINE_ARRAY( piped_process*, piped_process_array );

//----------------------------------------------------------------------------------------
// Class definition: process_progress_dialog
//----------------------------------------------------------------------------------------

//! A details_progress_dialog customized for an array of processes to execute.
/*! 
    This dialog manages the execution of an array of processes. The processes are run 
    one after another: when one terminates, the next one starts. If the class is used
    directly, it will just run the processes and put the output into the progress_listbox.
    However, it is more useful as a derived class, using the pluggable virtual functions 
    for things that are to be done before and after each commandline, and before and
    after the entire suite of commandlines, and most importantly: something to do 
    when a line of output is generated from the process.
    
    The stream kinds that are caught by the process dialog can be either stdout, stderr,
    both or neither. The stream kinds are specified in the constructor, but also can be 
    changed by a derived class while the array of processes is running. 
    IMPORTANTLY, however, don't use a stream of which there will be no/little output 
    generated, unless you don't care about the program freezing while it gets a 
    character from the stream (to see if Eof and other things).
    
    When the dialog is closed, via stop button or window's corner 'X' button it will 
    detach and kill all processes that it started, that are still running (which should
    only be 1 at a time, since they are executed in sequence).
    
    \note: This implementation is the only current approach that works as of wx2.3.3. 
        All other avenues have already been examined.
 */ 
class process_progress_dialog : public details_progress_dialog
{

    DECLARE_DYNAMIC_CLASS( process_progress_dialog )

public: 
 
    //! Constructor.
    /*! 
        \param parent The parent window.
        \param commandline_array An array of commandline commands to be executed.
        \param stream_kind The kind of stream to be analyzed: stdout, stderr, both or none.
        \param kill_signal. The kill signal to be used to kill the commandline. See 
            wxKill() docs for a list of signals.
            \note On MSW, there is really only a wxSIGNONE, wxSIGTERM and wxSIGKILL.
            All others go to wxSIGTERM. Very importantly, on MSW, wxSIGTERM only works
            if the program has windows: it won't do anything if it is a console only app.
        \param automatically_close_when_done Whether to close when done. This can be 
            overridden later with set_automatically_close_when_done();        
        \param show_hide_listbox_configuration_key Configuration key of active             
            configuration to store show/hide details. wxEmptyString means don't load/save. 
        \param log_filename  Filename of the process logfile. 
            A plkrFILE_FRIENDLY_DATETIME_STAMP in the string will replace with a 
            file-friendly datetime string. If using plkrFILE_FRIENDLY_DATETIME_STAMP, 
            recommended to keep the rest of name below 11 characters total if need 
            Macintosh 31-character limit requirement.
        \param log_path Default path of the logfile in dialog.
            Default of wxEmptyString means the current directory.
        \param scroll_history Number of lines to keep in the listbox at one time. After
            this, newly appended lines will remove the topmost line. Can be a number, or 
            can use one of the options* to get a recommended for OS or an infinite one.

     */
    process_progress_dialog( wxWindow* parent,
                     const wxArrayString& commandline_array, 
                     long stream_kind = optionPROCESS_PROGRESS_DIALOG_STDOUT_STREAM_KIND    |
                                        optionPROCESS_PROGRESS_DIALOG_STDERR_STREAM_KIND,
                     wxSignal kill_signal = wxSIGKILL,  
                     bool automatically_close_when_done = TRUE,
                     const wxString& show_hide_listbox_configuration_key = wxEmptyString, 
                     const wxString& log_filename = wxEmptyString,
                     const wxString& log_path = wxEmptyString,
                     long scroll_history = optionDETAILS_PROGRESS_LISTBOX_RECOMMENDED_SCROLL_HISTORY
                   );
    
    // Dummy construtor for RTTI macros
    process_progress_dialog() {}
                     
    //! Destructor.             
    virtual ~process_progress_dialog();     

    //---Optional tasks to do before and after executions of commandlines-----------------------
        
    //! Virtual: optional action to do before starting execution of the commandline array.
    /*! 
        \return TRUE if there was no error.
     */
    virtual bool on_before_execute_commandline_array() { return TRUE; }
    
    //! Virtual: optional action to do before start execution of a commandline in array.
    /*!
        \param commandline_array_index The index of the commandline_array.
        \return TRUE if there was no error.
     */
    virtual bool on_before_execute_commandline_array_item( size_t commandline_array_index ) { return TRUE; }
    
    //! Virtual: optional action to do after a commandline in the array has terminated.
    /*!
        \param commandline_array_index The index of the commandline_array.
        \return TRUE if there was no error.
     */
    virtual bool on_after_commandline_array_item_execution_finished( size_t commandline_array_index ) { return TRUE; }

    //! Virtual: optional action to de after all commands's processes have terminated.
    /*! 
        \return TRUE if there was no error.
     */
    virtual bool on_after_commandline_array_executions_finished() { return TRUE; }

    //---Handling a line of processs output and manipulating the abort signal----------

    //! The vital function to override: actions to do when process made a line of output.
    /*! 
        Most likely you are going to want to parse the added line and based on 
        what the line said, append_line_to_listbox() (or not), set_gauge(), 
        or set_statictext() or maybe throw up a message box or something else.
        
        The Default is to just call_append_line_to_listbox() and then keep going,
        but you'll probably want to override with what you want it to do.
        \param stream_kind the kind of stream that was added. Either
            optionPROCESS_PROGRESS_DIALOG_STDOUT_STREAM_KIND (stdout) or
            optionPROCESS_PROGRESS_DIALOG_STDERR_STREAM_KIND (stderr).
            This allows you to do one thing for an line from one kind of 
            stream, and another for another kind of stream.
        \param line_text The output string line from that stream.   
        \param commandline_array_index Index of the command array that generated 
            this line, (in case you want to handle them differently).

    */   
    virtual void on_process_generated_an_output_line( long stream_kind, 
                                                      const wxString& line_text,
                                                      size_t commandline_array_index
                                                    );
    
    // From details_progress_dialog()
    virtual void action_to_do_when_dialog_is_ready();
    
    // From details_progress_dialog()
    virtual void action_to_do_before_dialog_finishes_closing();

    // From details_progress_dialog()
    virtual void action_to_do_after_dialog_finishes_closing();

    //! Accessor to m_stream_kind,allowing switching of what types of streams to catch.
    void set_stream_kind( long stream_kind );
    
    //! Accessor to query what types of streams should be caught (used by piped_process).
    long get_stream_kind();

    //! Acessor to m_current_commandline_array_index
    size_t get_current_commandline_array_index();   
   
    //! An optional custom kill for the process instead of a stock wxKill()
    /*!
        See wxKill() for parameter list
     */
    virtual void custom_kill( long pid, 
                              wxSignal sig,
                              wxKillError *krc 
                            );
   
private:

    //! Action done when process has terminated.    
    void on_process_terminated( wxProcessEvent& event );

    //! Kill, detach running processes
    void detach_and_kill_all_running_processes();       
    
    //-----Process functions-----------------------------------------------------------
    
    //! Execute the command-line string.
    void execute_commandline_array_item( size_t commandline_array_index );

    //! Action done on each idle. Calls suck_process_output().
    void on_idle( wxIdleEvent& event );

#if ( setupUSE_PROCESS_PROGRESS_DIALOG_IDLE_TIMER )
    //! Action that is performed everytime the timer completes a cycle.
    void on_timer( wxTimerEvent& event );
#endif

    //! An array of commandlines to be executed.
    wxArrayString   m_commandline_array;

    //! Index of the commandline currently being processed (so can be carrried.
    /*!
        So it can be carried off to on_process_generated_an_output_line() argument,
        and also daisy chained via the EVT_PROCESS_TERMINATED event.
     */
    size_t          m_current_commandline_array_index;    
   
    //! Kind of stream
    long            m_stream_kind;           
                
    //! Desired kill signal for the processes when killing them.            
    wxSignal        m_kill_signal;           
                
    //! TRUE to kill the children of a process when it is being killed.
    bool            m_kill_process_children;
                
    //! Array of running piped processes.
    piped_process_array m_running_process_array;

#if ( setupUSE_PROCESS_PROGRESS_DIALOG_IDLE_TIMER )
    // Timer to generate idle events.
    wxTimer         m_idle_timer;
#endif

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ENHANCED_PROGRESS_DIALOG

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _PROCESS_PROGRESS_DIALOG_H_
