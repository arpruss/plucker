//----------------------------------------------------------------------------------------
// Name:        os_msw.cpp
// Purpose:     Functions that are MSW specific, like accessing the registry to find 
//              PluckerHome.
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor 
// Licence:     
// RCS-ID:      $Id: os_msw.cpp,v 1.12 2007/06/25 15:34:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "os_msw.h"
#endif

//----------------------------------------------------------------------------------------
// Begin OS removal condition
//----------------------------------------------------------------------------------------

#ifdef __WXMSW__

//----------------------------------------------------------------------------------------
// Standard wxWindows headers
//----------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//----------------------------------------------------------------------------------------
// Header of this .cpp file
//----------------------------------------------------------------------------------------

#include "os_msw.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/confbase.h"                // wxConfigBase
#include "wx/msw/regconf.h"             // wxRegConfig. NOTE: only using it to get value
                                        // of pluckerhome at startup. Rest of program will
                                        // use wxFileConfig   
#include "wx/utils.h"                   // wxDos2UnixFilename  

//----------------------------------------------------------------------------------------

#include "configuration.h"

//----------------------------------------------------------------------------------------
// Internal constants and helper functions
//----------------------------------------------------------------------------------------

#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
// From wxWindows msw/src/utils.cpp, used by the kill_properly() function.
// Structure used to pass parameters from wxKill() to wxEnumFindByPidProc()
struct plkrFindByPidParams
{
    plkrFindByPidParams() { hwnd = 0; pid = 0; }

    // the HWND used to return the result
    HWND hwnd;

    // the PID we're looking from
    DWORD pid;
};
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING


#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
// From wxWindows msw/src/utils.cpp, used by the kill_properly() function.
// kill_properly() Helper: EnumWindows() callback which is used to find the first (top
// level) window belonging to the given process
BOOL CALLBACK plkrEnumFindByPidProc(HWND hwnd, LPARAM lParam)
{
    DWORD pid;
    (void)::GetWindowThreadProcessId(hwnd, &pid);

    plkrFindByPidParams *params = (plkrFindByPidParams *)lParam;
    if ( pid == params->pid )
    {
        // remember the window we found
        params->hwnd = hwnd;

        // return FALSE to stop the enumeration
        return FALSE;
    }

    // continue enumeration
    return TRUE;
}
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

// Note: currently this is only executed at startup, before the real configuration object
// is started.
wxString os_msw::get_plucker_bundle_path_from_registry()
{
    // Will hold pointer to previous active config (either NULL or 'the_configuration'
    // wxFileConfig object from configuration.cpp).
    wxConfigBase* previous_configuration = NULL;

    wxRegConfig* the_registry_configuration = NULL;
	// Note that as of Plucker Desktop 1.8, we are setting the flag
	// wxCONFIG_USE_GLOBAL_FILE instead of wxCONFIG_USE_LOCAL_FILE.
	// This forces the program to read the setting from registry's HKLM instead
	// of HKCU. HKLM is the registry reading that all users can read, and 
	// HKCU is stuff that only the current user can read. Note that on 
	// wxRegConfig it is built so that users can only read to the registry with 
	// wxCONFIG_USE_GLOBAL_FILE, but can read/write to it if it is
	// wxCONFIG_USE_LOCAL_FILE (this makes sense since in a proper WinNT
	// deployment the limited users can't write to HKLM anyways).
	// This is the proper use of wxCONFIG_USE_GLOBAL_FILE, to have the administrator
	// set a global setting, that the end users can then read.
    the_registry_configuration = new wxRegConfig( wxT( "Plucker" ), wxT( "The Plucker Team" ),
                                                  wxEmptyString, wxEmptyString,
                                                  wxCONFIG_USE_GLOBAL_FILE );
    
    // Sets the configuration to the currently active one, and holds pointer to the
    // previous config object in previous_configuration;
    previous_configuration = wxConfigBase::Set( the_registry_configuration );
                                                  
    // Read the value of pluckerhome into our string                                              
    wxString plucker_bundle_path;
    if ( the_registry_configuration->Exists( wxT( "/Path" ) ) )
    {    
        plucker_bundle_path = the_registry_configuration->Read( wxT( "/Path" ), wxT( "/Progra~1/Plucker" ) );    
        wxLogDebug( wxT( "Accessed Registry: PluckerHome=%s" ), plucker_bundle_path.c_str() );
    } 
    else
    {
        wxLogError( wxT( "Error: Unable to find The Plucker Team key in registry" ) );
    }
    // Strip off the trailing backslash, since our directories are stored sans
    // trailing slashes.
    plucker_bundle_path.RemoveLast();
    
    wxSetWorkingDirectory( plucker_bundle_path );
    // A GetCwd() returns path in a /Program Files/Plucker syntax instead of the  
    // annoying C:\\ type of entry. Note this means have to run Plucker-Desktop
    // and Plucker from the same drive but I will trade this for convenience 
    // elsewhere in code maintenance.
    
    //! \todo SOURCE: a better way than this.
    plucker_bundle_path = wxGetCwd();
/*
    // Convert the backslashes to forward slashes. This would accomplish similar
    // to SetGet above, but with the original drive letter.
    //wxDos2UnixFilename( plucker_bundle_path );
*/
    // Set the active config to the previous one:
    wxConfigBase::Set( previous_configuration );
    // If successfully created the_registry_configuration, then delete it now.
    if ( the_registry_configuration )
    {
        delete the_registry_configuration;
        the_registry_configuration = NULL;
    }
    
    // Return the value of pluckerhome
    return plucker_bundle_path;
}


#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
// This is the same as the wxKill() for MSW as in /src/msw/utils.cpp except for 
// the call to ::TerminateProcess(..), and the fact that we have a call to 
// kill_process_children also.
int os_msw::kill_properly( long pid, wxSignal sig, wxKillError *krc )
{
    wxLogDebug( wxT( "Entering os_msw::kill_properly()" ) );
#ifdef __WIN32__

    // get the process handle to operate on
    HANDLE hProcess = ::OpenProcess(SYNCHRONIZE |
                                    PROCESS_TERMINATE |
                                    PROCESS_QUERY_INFORMATION,
                                    FALSE, // not inheritable
                                    (DWORD)pid);
    if ( hProcess == NULL )
    {
        if ( krc )
        {
            if ( ::GetLastError() == ERROR_ACCESS_DENIED )
            {
                *krc = wxKILL_ACCESS_DENIED;
            }
            else
            {
                *krc = wxKILL_NO_PROCESS;
            }
        }

        return -1;
    }

    bool ok = TRUE;
    switch ( sig )
    {
        case wxSIGKILL:
            // kill the process forcefully returning -1 as error code
            // RO: This is where we differ from wxWindows 2.3.3 version that just
            // does a stock ::TerminateProcess().
            //---Start modification----------------------------------------------------
            if ( ! best_terminate_process(hProcess, (UINT)-1) )
            //---End modification------------------------------------------------------
            {
                wxLogDebug( wxT( "best_terminate_process returned !=0" ) );
                wxLogSysError( wxT( "Failed to kill process %d" ), pid  );

                if ( krc )
                {
                    // this is not supposed to happen if we could open the
                    // process
                    *krc = wxKILL_ERROR;
                }

                ok = FALSE;
            }
            break;

        case wxSIGNONE:
            // do nothing, we just want to test for process existence
            break;

        default:
            // any other signal means "terminate"
            {
                plkrFindByPidParams params;
                params.pid = (DWORD)pid;

                // EnumWindows() has nice semantics: it returns 0 if it found
                // something or if an error occured and non zero if it
                // enumerated all the window
                if ( !::EnumWindows(plkrEnumFindByPidProc, (LPARAM)&params) )
                {
                    // did we find any window?
                    if ( params.hwnd )
                    {
                        // tell the app to close
                        //
                        // NB: this is the harshest way, the app won't have
                        //     opportunity to save any files, for example, but
                        //     this is probably what we want here. If not we
                        //     can also use SendMesageTimeout(WM_CLOSE)
                        if ( !::PostMessage(params.hwnd, WM_QUIT, 0, 0) )
                        {
                            wxLogLastError(wxT( "PostMessage(WM_QUIT)"));
                        }
                    }
                    else // it was an error then
                    {
                        wxLogLastError(wxT( "EnumWindows"));

                        ok = FALSE;
                    }
                }
                else // no windows for this PID
                {
                    if ( krc )
                    {
                        *krc = wxKILL_ERROR;
                    }

                    ok = FALSE;
                }
            }
    }

    // the return code
    DWORD rc;

    if ( ok )
    {
        // as we wait for a short time, we can use just WaitForSingleObject()
        // and not MsgWaitForMultipleObjects()
        switch ( ::WaitForSingleObject(hProcess, 500 /* msec */) )
        {
            case WAIT_OBJECT_0:
                // process terminated
                if ( !::GetExitCodeProcess(hProcess, &rc) )
                {
                    wxLogLastError(wxT( "GetExitCodeProcess"));
                }
                break;

            default:
                wxFAIL_MSG( wxT( "unexpected WaitForSingleObject() return") );
                // fall through

            case WAIT_FAILED:
                wxLogLastError(wxT( "WaitForSingleObject"));
                // fall through

            case WAIT_TIMEOUT:
                if ( krc )
                {
                    *krc = wxKILL_ERROR;
                }

                rc = STILL_ACTIVE;
                break;
        }
    }
    else // !ok
    {
        // just to suppress the warnings about uninitialized variable
        rc = 0;
    }

    ::CloseHandle(hProcess);

    // the return code is the same as from Unix kill(): 0 if killed
    // successfully or -1 on error
    if ( sig == wxSIGNONE )
    {
        if ( ok && rc == STILL_ACTIVE )
        {
            // there is such process => success
            return 0;
        }
    }
    else // not SIGNONE
    {
        if ( ok && rc != STILL_ACTIVE )
        {
            // killed => success
            return 0;
        }
    }
#else // Win16
    wxFAIL_MSG( wxT( "not implemented") );
#endif // Win32/Win16

    // error
    return -1;
}
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING


#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
// This define might be needed for VC++ I don't know
// #define STRICT
//! \test Work better if not in the class?
BOOL os_msw::best_terminate_process( HANDLE process_handle, UINT exit_code )
{
    wxLogDebug( wxT( "Entering os_msw_best_terminate_process" ) );
    BOOL successfully_killed         = FALSE;

    // If A 95/98/Me version of Windows...
    //! \test Does this also exclude Me, and include XP and laters?
    if ( wxGetOsVersion() != wxWINDOWS_NT ) 
    {
        //! \test make sure you can pass these handles around OK between functions.
        // ...then just kill it with the crappy ::TerminateProcess()...
        successfully_killed = ::TerminateProcess( process_handle, exit_code );  
        // ... and return success here, and abort the function.
        return successfully_killed;  
    }

    // Otherwise it was a 2000/NT/XP version of windows, and we should use
    // our better way than MSW's ::TerminateProcess().
    DWORD thread_id                 = 0;
    DWORD temp_exit_code            = 0;
    DWORD error_number              = 0;
    HANDLE duplicate_process_handle = INVALID_HANDLE_VALUE;
    HANDLE remote_thread_handle     = NULL;
    HINSTANCE kernel_handle         = GetModuleHandle( wxT( "Kernel32" ) );
    // Try to duplicate a handle, storing whether successful or not.
    BOOL got_a_duplicate_handle_handle = DuplicateHandle( GetCurrentProcess(),
                                                          process_handle,
                                                          GetCurrentProcess(),
                                                          &duplicate_process_handle,
                                                          PROCESS_ALL_ACCESS,
                                                          FALSE,
                                                          0 );
                                                       
    // If it isn't the special case where the process is already dead...
    if ( GetExitCodeProcess( ( got_a_duplicate_handle_handle ) 
         ? duplicate_process_handle : process_handle, &temp_exit_code ) 
         && ( temp_exit_code == STILL_ACTIVE ) ) 
    {
       // Get the process address.
       FARPROC process_address;         
       process_address = GetProcAddress( kernel_handle, "ExitProcess" );  
       
       // Create a remote thread using that process address.
       remote_thread_handle = CreateRemoteThread( ( got_a_duplicate_handle_handle ) ? duplicate_process_handle : process_handle,
                                                  NULL,
                                                  0,
                                                  (LPTHREAD_START_ROUTINE)process_address,
                                                  (PVOID)exit_code, 0, &thread_id );
       
       // If couldn't get a handle to the remote thread... 
       if ( remote_thread_handle == NULL ) 
       {
            // ...then set the last error number for the debugger.
            error_number = GetLastError();
       }
       
    // ...otherwise it is already dead.  
    } 
    else 
    {
        error_number = ERROR_PROCESS_ABORTED;
    }
    
    // If we did create manage to a remote thread handle...
    if ( remote_thread_handle ) 
    {
        // ...must wait for a process to terminate to guarantee that it has exited...
        //! \todo INFINITE seems a bit high of a number...
        WaitForSingleObject( ( got_a_duplicate_handle_handle ) ? duplicate_process_handle : process_handle,
                             INFINITE );
        // ...then close handle...
        CloseHandle( remote_thread_handle );
        // ...and report that it was successfully killed.
        successfully_killed = TRUE;
    }
    
    // If we made a duplicate handle...
    if ( got_a_duplicate_handle_handle ) 
    {
        // ...close that handle too.
        CloseHandle( duplicate_process_handle );
    }
    
    // If we couldn't kill, set the last error number for the MSW debugger.
    if ( ! successfully_killed ) 
    {
        SetLastError( error_number );
    }
        
    // Return success or not.    
    return successfully_killed;
}
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING

//----------------------------------------------------------------------------------------
// End OS removal condition
//----------------------------------------------------------------------------------------
    
#endif  // __WXMSW__
