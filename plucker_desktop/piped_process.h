//----------------------------------------------------------------------------------------
/*!
    \file       piped_process.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes piped_process class  
    \author     Robert O'Connor
    \date       2002/08/03    
 */ 
// RCS-ID:      $Id: piped_process.h,v 1.4 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PIPED_PROCESS_H_
#define _PIPED_PROCESS_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "piped_process.h"
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

// Forward declaration.
class process_progress_dialog;
#include "process_progress_dialog.h"

//----------------------------------------------------------------------------------------
// Class definition: piped_process
//----------------------------------------------------------------------------------------

//! A class to manage a piped process 
class piped_process : public wxProcess
{

    DECLARE_DYNAMIC_CLASS( piped_process )

public:

    //! Constructor. 
    /*!
        Reason we need a progress_dialog parameter [and not a wxWindow],
        is so we can find its listbox for output.
        \param parent The parent process_progress_dialog.
        \param id The ID of the control on the process_progress_dialog that 
            will recieve event of this process terminating. 
            (This control id should not to be confused with a pid).
    */
    piped_process( process_progress_dialog* parent = NULL,
                   int id = -1
                  );

    //! Suck some output from the process, and query whether more input remains queued.
    /*!
        param after_process_terminated TRUE if the suck is after the process is
            terminated (used to trace program flow during debuggging).
        return TRUE if there is more output to be sucked, so that OnIdle, knows
            to suck it again.
     */
    bool suck_process_output( bool after_process_terminated );

    //! Accessor to a process's pid.
    int get_process_pid();

    //! Accessor to a process's pid.
    void set_process_pid( int process_pid );

protected:

    //! Keep the parent, so can find the listbox to send the output to.
    process_progress_dialog*    m_parent;
    int                         m_process_pid;

};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ENHANCED_PROGRESS_DIALOG

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _PIPED_PROCESS_H_
