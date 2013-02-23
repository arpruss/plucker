//----------------------------------------------------------------------------------------
/*!
    \file       build_progress_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes build_progress_dialog class   
    \author     Robert O'Connor
    \date       2002/01/03    
 */ 
// RCS-ID:      $Id: build_progress_dialog.h,v 1.6 2007/06/29 00:58:50 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _BUILD_PROGRESS_DIALOG_H_
#define _BUILD_PROGRESS_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "build_progress_dialog.h"
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

#include "process_progress_dialog.h"

//----------------------------------------------------------------------------------------
// Class definition: build_progress_dialog
//----------------------------------------------------------------------------------------

//! A process dialog customized for plucker-build processes.
/*! 
    Manages plucker-build processes, updating progress guages and statictext according to
    the output lines, sets the updated time of a channel when it completes, installs all 
    of the channels when successfully execute all processes. For verbosity=0 channels it
    sets the stream_kind to none, so that program won't freeze--for all others, it 
    sets the stream_kind to stderr.
    
    Since derived from derived from process_progress_dialog, see that class for a
    description of the virtual functions and parameters.
 */
class build_progress_dialog : public process_progress_dialog
{

public: 
 
    //! Constructor.
    /*! 
        \param channel_sections_array Array of channel sections
        \sa process_progress_dialog::process_progress_dialog() for rest.
     */ 
    build_progress_dialog( wxWindow* parent,
                     const wxArrayString& channel_sections_array,
                     const wxArrayString& commandline_array, 
                     long stream_kind = optionPROCESS_PROGRESS_DIALOG_STDOUT_STREAM_KIND    |
                                        optionPROCESS_PROGRESS_DIALOG_STDERR_STREAM_KIND,
                     wxSignal kill_signal = wxSIGKILL,   
                     bool automatically_close_when_done = TRUE,
                     const wxString& show_hide_listbox_configuration_key = wxEmptyString,
                     const wxString& log_filename = wxEmptyString,
#if ( setupUSE_MSW_FORCE_COMMANDLINE_AUTOCLOSE_PROGRESS_CLOSURE )
                     long force_commandline_autoclose = 0,
#endif
                     const wxString& log_path = wxEmptyString,
                     long scroll_history = optionDETAILS_PROGRESS_LISTBOX_RECOMMENDED_SCROLL_HISTORY
                     );

    //! Destructor
    virtual ~build_progress_dialog();

    // Described in process_progress_dialog

    virtual bool on_before_execute_commandline_array();
   
    virtual bool on_before_execute_commandline_array_item( size_t commandline_array_index );
    
    virtual bool on_after_commandline_array_item_execution_finished( size_t commandline_array_index );

    virtual bool on_after_commandline_array_executions_finished();

    void on_process_generated_an_output_line( long stream_kind,
                                              const wxString& line_text,
                                              size_t commandline_array_index
                                            );

    // From details_progress_dialog()
    void action_to_do_after_dialog_finishes_closing();

    void custom_kill( long pid,
                      wxSignal sig,
                      wxKillError *krc
                    );
private:

    wxArrayString m_channel_sections_array;

    //! A recycled function to install channels and post messages about it.
    /*!
        Shared since possible for program to either install all at end, or
        install as go. Note it has to be an wxArrayString so that the Linux
        can install as a batch during HotSync.
        \param sections_to_install Channel sections to install.
     */
    void send_to_destinations_and_inform( wxArrayString& section_to_install );

#if ( setupUSE_MSW_FORCE_COMMANDLINE_AUTOCLOSE_PROGRESS_CLOSURE )
    //! Store whether to do a forcing of commandline autoclose (MSW only)
    long m_force_commandline_autoclose;
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

#endif  //_BUILD_PROGRESS_DIALOG_H_
