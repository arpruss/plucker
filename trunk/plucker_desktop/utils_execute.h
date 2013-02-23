//----------------------------------------------------------------------------------------
/*!
    \file       utils_execute.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes utils_execute class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: utils_execute.h,v 1.4 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _UTILS_EXECUTE_H_
#define _UTILS_EXECUTE_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "utils_execute.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/process.h"

//----------------------------------------------------------------------------------------
// Class definition: utils_execute (static only)
//----------------------------------------------------------------------------------------

//! A static-only class for commonly used functions of the wxWindows wxExecute/wxShell
class utils_execute
{

public:  

    //! Executes a command in a new xterm window, optionally setting the window behaviour.        
    /*! wxShell(command) on WXGTK I don't feel works as advertised (won't open an 
        interactive window--it runs in the background). But calling xterm ourselves
        gives us more control anyways.
        \param command Required. A command string to be executed in xterm 
        \param shell_console_title Title at top of window. If empty, then will use the 
            name of the command's executable as the title.
        \param close_on_exit Whether to close the xterm window when the commandline
            application terminates.
        \param synchrous TRUE to make it a synchronous execution, that is, your 
            application doesn't need for xterm to finish before it continues with its 
            program flow.
        \param show_scrollbar TRUE to show a scrollbar on the side of the window 
            (right left mouse clicks on the scrollbar scroll it, as do 
            shift+page up/down).
        \param saved_line_history Number of lines to keep in memory that user can 
            scroll backwards to.
        \return TRUE if the execution of xterm was successful.
     */     
    static bool launch_xterm_shell( const wxString& command, 
                                    const wxString& shell_console_title = wxEmptyString,
                                    bool close_on_exit = FALSE,
                                    bool synchronous = FALSE,
                                    bool show_scrollbar = TRUE,
                                    long saved_line_history = 10000
                                  );
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_UTILS_EXECUTE_H_
