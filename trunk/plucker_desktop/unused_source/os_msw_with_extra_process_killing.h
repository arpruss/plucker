//----------------------------------------------------------------------------------------
/*!
    \file       os_msw.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( rob@medicalmnemonics.com )    
    \licence    GPL
    \brief      Describes os_msw class    
    \author     Robert O'Connor
    \date       2002/10/20    
 */  
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _OS_MSW_H_
#define _OS_MSW_H_

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin OS removal condition
//----------------------------------------------------------------------------------------

#ifdef __WXMSW__

//----------------------------------------------------------------------------------------
// Class definition: os_msw
//----------------------------------------------------------------------------------------

//! Static-only class for MSW specific functions
class os_msw
{

public:
    
    //! Looks up the value of pluckerhome from the machnine's registry
    /*!
        return The value of pluckerhome.
     */ 
    static wxString get_pluckerhome_from_registry();
    
#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )        
    //! Kill a process properly on MSW.    
    /*!
        Process killing is very poor in the design of the Microsoft OS's. This function
        will do the best process killing that is possible on this platform.
        Under NT variant OS's (NT, 2000, XP, etc) on a wxSIGKILL signal, it will go 
        inside the process and kill it from within, instead of using the 
        ::TerminateProcess() API call which will not detach DLLs properly.
        On older MSW version's, 95,98, Me, the wxSIGKILL will use the 
        crappy ::TerminateProcess() API call, since that is all that is possible on
        that OS.
        \param pid The pid to process to be killed.
        \param sig The signal it should be killed with (wxSIGKILL, wxSIGTERM and
            wxSIGNONE, are the only MSW relevant ones as of wx2.3.3--Rest are 
            just remapped to wxSIGTERM). wxSIGTERM only works on processes that 
            have a window.
        \param krc The kill error returned.
        \param option Options: any or none of optionOS_MSW_PROCESS_KILL_CHILDREN                            = 1,
            optionOS_MSW_PROCESS_WAIT_FOR_NAMED_APPLICATION_TERMINATION
        \return 0 on success.
     */    
     static int kill_properly( long pid, 
                               wxSignal sig, 
                               wxKillError *krc
                             );                            
       
    //! Kills children of a process.    
    /*!
        Works by taking a freezeframe snapshot of all proces IDs on the OS, then 
        looking at each process and see whether its parent matches the pid from 
        the constructor. If so, then the process is killed.
        \param pid The pid of the process whose children should be killed.
        \param sig The signal it should be killed with (wxSIGKILL, wxSIGTERM and
            wxSIGNONE, are the only MSW relevant ones as of wx2.3.3--Rest are 
            just remapped to wxSIGTERM). wxSIGTERM only works on processes that 
            have a window.
        \param krc The kill error returned.
        \return 0 on success.
     */    
    static int kill_process_children( long pid, 
                                      wxSignal sig,
                                      wxKillError *krc
                                    );
  
     //! Checks whether a named process is running.
     /*!
         \param process_name The name of the process to check if running.
      */
     static bool is_named_process_running( const wxString& process_name );
     
     //! Terminate a process based on its name.
     /*!
         \param named_process_to_terminate The name of process to terminate.
      */
     static int terminate_process_by_name( const wxString& named_process_to_terminate );
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING


        
private:

#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
    //! Perform a terminate process in the best way for the MSW platform.
    /*!
        Does MSW API ::TerminateProcess() on the old versions and a better way
        on later versions of the OS.
        Has the same parameters as MSW API ::TerminateProcess().
        \param process_handle The handle of the process to terminate.
        \param exit_code The exit code that you want the process to end with.   
        \return TRUE on success
            \note that is the opposite to the other process functions her, 
            where a 0 (which would be a FALSE bool) means a successful
            kill. That is screwy, but makes this function the same 
            as ::TerminateProcess().     
     */   
    static BOOL best_terminate_process( HANDLE process_handle,
                                        UINT exit_code
                                      );    
                                      
     
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING
};

//----------------------------------------------------------------------------------------
// End OS removal condition
//----------------------------------------------------------------------------------------
    
#endif  // __WXMSW__

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------
    
#endif  //_OS_MSW_H_
