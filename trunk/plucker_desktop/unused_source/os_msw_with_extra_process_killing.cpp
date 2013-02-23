//----------------------------------------------------------------------------------------
// Name:        os_msw.cpp
// Purpose:     Functions that are MSW specific, like accessing the registry to find 
//              PluckerHome.
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor. kill_process_children approad thanks to John Skiff.
// Licence:     
// RCS-ID:      $Id: os_msw_with_extra_process_killing.cpp,v 1.1 2002/08/31 23:42:25 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#ifdef __GNUG__
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

#include <tlhelp32.h>                   // MSW Toolhelper. For kill_process_children()

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
wxString os_msw::get_pluckerhome_from_registry()
{
    // Will hold pointer to previous active config (either NULL or 'the_configuration'
    // wxFileConfig object from configuration.cpp).
    wxConfigBase* previous_configuration = NULL;

    wxRegConfig* the_registry_configuration = NULL;
    the_registry_configuration = new wxRegConfig( "Plucker", "The Plucker Team",
                                                  wxEmptyString, wxEmptyString,
                                                  wxCONFIG_USE_LOCAL_FILE );
    
    // Sets the configuration to the currently active one, and holds pointer to the
    // previous config object in previous_configuration;
    previous_configuration = wxConfigBase::Set( the_registry_configuration );
                                                  
    // Read the value of pluckerhome into our string                                              
    wxString pluckerhome;
    if ( the_registry_configuration->Exists( "/Path" ) ) {    
        pluckerhome = the_registry_configuration->Read( "/Path", "/Progra~1/Plucker" );    
        wxLogDebug( "Accessed Registry: PluckerHome=%s", pluckerhome.c_str() );
    } else {
        wxLogError( "Error: Unable to find The Plucker Team key in registry" );
    }
    // Strip off the trailing backslash, since our directories are stored sans
    // trailing slashes.
    pluckerhome.RemoveLast();
    
    wxSetWorkingDirectory( pluckerhome );
    // A GetCwd() returns path in a /Program Files/Plucker syntax instead of the  
    // annoying C:\\ type of entry. Note this means have to run Plucker-Desktop
    // and Plucker from the same drive but I will trade this for convenience 
    // elsewhere in code maintenance.
    
    //! \todo SOURCE: a better way than this.
    pluckerhome = wxGetCwd();
/*
    // Convert the backslashes to forward slashes. This would accomplish similar
    // to SetGet above, but with the original drive letter.
    //wxDos2UnixFilename( pluckerhome );
*/
    // Set the active config to the previous one:
    wxConfigBase::Set( previous_configuration );
    // If successfully created the_registry_configuration, then delete it now.
    if ( the_registry_configuration ) {
        delete the_registry_configuration;
        the_registry_configuration = NULL;
    }
    
    // Return the value of pluckerhome
    return pluckerhome;
}


#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
// This is the same as the wxKill() for MSW as in /src/msw/utils.cpp except for 
// the call to ::TerminateProcess(..), and the fact that we have a call to 
// kill_process_children also.
int os_msw::kill_properly( long pid, wxSignal sig, wxKillError *krc )
{
    wxLogDebug( "Entering os_msw::kill_properly()" );
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
                wxLogDebug( "best_terminate_process returned !=0" );
                wxLogSysError("Failed to kill process %d", pid);

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
                            wxLogLastError(_T("PostMessage(WM_QUIT)"));
                        }
                    }
                    else // it was an error then
                    {
                        wxLogLastError(_T("EnumWindows"));

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
                    wxLogLastError(_T("GetExitCodeProcess"));
                }
                break;

            default:
                wxFAIL_MSG( _T("unexpected WaitForSingleObject() return") );
                // fall through

            case WAIT_FAILED:
                wxLogLastError(_T("WaitForSingleObject"));
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
    wxFAIL_MSG( _T("not implemented") );
#endif // Win32/Win16

    // error
    return -1;
}
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING

//----------------------------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------------------------

#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
int os_msw::kill_process_children( long pid, wxSignal sig, wxKillError *krc )
{
    wxLogDebug( "Entering os_msw::kill_process_children" );
    // Take a snapshot of all processes in the system.
    //! \todo Is this the right TH32CS_ enum to use here?
    HANDLE process_snapshot_handle = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    // If couldn't get a handle, then abort here.
    if ( process_snapshot_handle == INVALID_HANDLE_VALUE ) {
        if ( krc ) {
            *krc = wxKILL_ERROR;
        }
        return -1;
    }

    // Fill in the size of the PROCESSENTRY32 structure before using it.
    PROCESSENTRY32 process_entry = { 0 };
    process_entry.dwSize = sizeof( PROCESSENTRY32 );

    // If can't get the first process, then abort.
    if ( ! Process32First( process_snapshot_handle, &process_entry ) ) {
        if ( krc ) {
            *krc = wxKILL_ERROR;
        }
        CloseHandle( process_snapshot_handle );
        return -1;
    }

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    do {
        wxLogDebug( "Examining process_entry" );
        if ( process_entry.th32ParentProcessID == (DWORD)pid ) {
            wxLogDebug( "pid matches parent. About to kill." );
            // Kill them using our good method.
            // Interesting we could kill the grandkids too, by killing the 
            // kids of the kids here, by using TRUE instead of FALSE.
            //! \todo Decide if want/possible to have grandkid killing             
            if ( kill_properly( process_entry.th32ProcessID, sig, krc ) ) {
                //! \todo Why would we want to return -1 here? Doesn't that 
                // stop loop at the first child if something went wrong with
                // one of them?                
#if 0                
                return -1;
#endif
                wxLogDebug( "Couldn't kill properly" );            
            } else {
                wxLogDebug( "Killed properly" );
            }
        }
    } while ( Process32Next ( process_snapshot_handle, &process_entry ) );

    // Return 0 on success.
    return 0;
}
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING


#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
// This define might be needed for VC++ I don't know
// #define STRICT
//! \test Work better if not in the class?
BOOL os_msw::best_terminate_process( HANDLE process_handle, UINT exit_code )
{
    wxLogDebug( "Entering os_msw_best_terminate_process" );
    BOOL successfully_killed         = FALSE;

    // If A 95/98/Me version of Windows...
    //! \test Does this also exclude Me, and include XP and laters?
    if ( wxGetOsVersion() != wxWINDOWS_NT ) {
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
    HINSTANCE kernel_handle         = GetModuleHandle( "Kernel32" );
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
         && ( temp_exit_code == STILL_ACTIVE ) ) {
    
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
       if ( remote_thread_handle == NULL ) {
            // ...then set the last error number for the debugger.
            error_number = GetLastError();
       }
       
    // ...otherwise it is already dead.  
    } else {
        error_number = ERROR_PROCESS_ABORTED;
    }
    
    // If we did create manage to a remote thread handle...
    if ( remote_thread_handle ) {
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
    if ( got_a_duplicate_handle_handle ) {
        // ...close that handle too.
        CloseHandle( duplicate_process_handle );
    }
    
    // If we couldn't kill, set the last error number for the MSW debugger.
    if ( ! successfully_killed ) {
        SetLastError( error_number );
    }
        
    // Return success or not.    
    return successfully_killed;
}
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING


#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
bool os_msw::is_named_process_running( const wxString& process_name )
{

    // A more wxWindows type interface
    const char *szToTerminate = process_name.c_str();

    // Created: 6/23/2000  (RK)
    // Last modified: 3/10/2002  (RK)
    // Please report any problems or bugs to kochhar@physiology.wisc.edu
    // The latest version of this routine can be found at:
    //     http://www.neurophys.wisc.edu/ravi/software/killproc/
    // Terminate the process "szToTerminate" if it is currently running
    // This works for Win/95/98/ME and also Win/NT/2000/XP
    // The process name is case-insensitive, i.e. "notepad.exe" and "NOTEPAD.EXE"
    // will both work (for szToTerminate)
    // Return codes are as follows:
    //   0   = Process was successfully terminated
    //   603 = Process was not currently running
    //   604 = No permission to terminate process
    //   605 = Unable to load PSAPI.DLL
    //   602 = Unable to terminate process for some other reason
    //   606 = Unable to identify system type
    //   607 = Unsupported OS
    //   632 = Invalid process name
    //   700 = Unable to get procedure address from PSAPI.DLL
    //   701 = Unable to get process list, EnumProcesses failed
    //   702 = Unable to load KERNEL32.DLL
    //   703 = Unable to get procedure address from KERNEL32.DLL
    //   704 = CreateToolhelp32Snapshot failed
    // Change history:
    //   modified 3/8/2002  - Borland-C compatible if BORLANDC is defined as
    //                        suggested by Bob Christensen
    //   modified 3/10/2002 - Removed memory leaks as suggested by
    //					      Jonathan Richard-Brochu (handles to Proc and Snapshot
    //                        were not getting closed properly in some cases)
	
	BOOL bResult,bResultm;
	DWORD aiPID[1000],iCb=1000,iNumProc,iV2000=0;
	DWORD iCbneeded,i,iFound=0;
	char szName[MAX_PATH],szToTermUpper[MAX_PATH];
	HANDLE hProc,hSnapShot,hSnapShotm;
	OSVERSIONINFO osvi;
    HINSTANCE hInstLib;
	int iLen,iLenP,indx;
    HMODULE hMod;
	PROCESSENTRY32 procentry;      
	MODULEENTRY32 modentry;

	// Transfer Process name into "szToTermUpper" and
	// convert it to upper case
	iLenP=strlen(szToTerminate);
	if(iLenP<1 || iLenP>MAX_PATH) return FALSE;
	for(indx=0;indx<iLenP;indx++)
		szToTermUpper[indx]=toupper(szToTerminate[indx]);
	szToTermUpper[iLenP]=0;

     // PSAPI Function Pointers.
     BOOL (WINAPI *lpfEnumProcesses)( DWORD *, DWORD cb, DWORD * );
     BOOL (WINAPI *lpfEnumProcessModules)( HANDLE, HMODULE *,
        DWORD, LPDWORD );
     DWORD (WINAPI *lpfGetModuleBaseName)( HANDLE, HMODULE,
        LPTSTR, DWORD );

      // ToolHelp Function Pointers.
      HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD) ;
      BOOL (WINAPI *lpfProcess32First)(HANDLE,LPPROCESSENTRY32) ;
      BOOL (WINAPI *lpfProcess32Next)(HANDLE,LPPROCESSENTRY32) ;
      BOOL (WINAPI *lpfModule32First)(HANDLE,LPMODULEENTRY32) ;
      BOOL (WINAPI *lpfModule32Next)(HANDLE,LPMODULEENTRY32) ;

	// First check what version of Windows we're in
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    bResult=GetVersionEx(&osvi);
	if(!bResult)     // Unable to identify system version
	    return FALSE;

	// At Present we only support Win/NT/2000/XP or Win/9x/ME
	if((osvi.dwPlatformId != VER_PLATFORM_WIN32_NT) &&
		(osvi.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS))
		return FALSE;

    if(osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		// Win/NT or 2000 or XP

         // Load library and get the procedures explicitly. We do
         // this so that we don't have to worry about modules using
         // this code failing to load under Windows 9x, because
         // it can't resolve references to the PSAPI.DLL.
         hInstLib = LoadLibraryA("PSAPI.DLL");
         if(hInstLib == NULL)
            return FALSE;

         // Get procedure addresses.
         lpfEnumProcesses = (BOOL(WINAPI *)(DWORD *,DWORD,DWORD*))
            GetProcAddress( hInstLib, "EnumProcesses" ) ;
         lpfEnumProcessModules = (BOOL(WINAPI *)(HANDLE, HMODULE *,
            DWORD, LPDWORD)) GetProcAddress( hInstLib,
            "EnumProcessModules" ) ;
         lpfGetModuleBaseName =(DWORD (WINAPI *)(HANDLE, HMODULE,
            LPTSTR, DWORD )) GetProcAddress( hInstLib,
            "GetModuleBaseNameA" ) ;

         if(lpfEnumProcesses == NULL ||
            lpfEnumProcessModules == NULL ||
            lpfGetModuleBaseName == NULL)
            {
               FreeLibrary(hInstLib);
               return FALSE;
            }
		 
		bResult=lpfEnumProcesses(aiPID,iCb,&iCbneeded);
		if(!bResult)
		{
			// Unable to get process list, EnumProcesses failed
            FreeLibrary(hInstLib);
			return FALSE;
		}

		// How many processes are there?
		iNumProc=iCbneeded/sizeof(DWORD);

		// Get and match the name of each process
		for(i=0;i<iNumProc;i++)
		{
			// Get the (module) name for this process

	        strcpy(szName,"Unknown");
			// First, get a handle to the process
	        hProc=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,
				aiPID[i]);
	        // Now, get the process name
	        if(hProc)
			{
               if(lpfEnumProcessModules(hProc,&hMod,sizeof(hMod),&iCbneeded) )
			   {
                  iLen=lpfGetModuleBaseName(hProc,hMod,szName,MAX_PATH);
			   }
			}
	        CloseHandle(hProc);
			// We will match regardless of lower or upper case
#ifdef __BORLANDC__
            if(strcmp(strupr(szName),szToTermUpper)==0)
#else
			if(strcmp(_strupr(szName),szToTermUpper)==0)
#endif
			{
				// Process found
				iFound=1;
                FreeLibrary(hInstLib);
    			return TRUE;
			}			
		}
	}

	if(osvi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
	{
		// Win/95 or 98 or ME
			
		hInstLib = LoadLibraryA("Kernel32.DLL");
		if( hInstLib == NULL )
			return FALSE;

		// Get procedure addresses.
		// We are linking to these functions of Kernel32
		// explicitly, because otherwise a module using
		// this code would fail to load under Windows NT,
		// which does not have the Toolhelp32
		// functions in the Kernel 32.
		lpfCreateToolhelp32Snapshot=
			(HANDLE(WINAPI *)(DWORD,DWORD))
			GetProcAddress( hInstLib,
			"CreateToolhelp32Snapshot" ) ;
		lpfProcess32First=
			(BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
			GetProcAddress( hInstLib, "Process32First" ) ;
		lpfProcess32Next=
			(BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
			GetProcAddress( hInstLib, "Process32Next" ) ;
		lpfModule32First=
			(BOOL(WINAPI *)(HANDLE,LPMODULEENTRY32))
			GetProcAddress( hInstLib, "Module32First" ) ;
		lpfModule32Next=
			(BOOL(WINAPI *)(HANDLE,LPMODULEENTRY32))
			GetProcAddress( hInstLib, "Module32Next" ) ;
		if( lpfProcess32Next == NULL ||
			lpfProcess32First == NULL ||
		    lpfModule32Next == NULL ||
			lpfModule32First == NULL ||
			lpfCreateToolhelp32Snapshot == NULL )
		{
			FreeLibrary(hInstLib);
			return FALSE;
		}
			
		// The Process32.. and Module32.. routines return names in all uppercase

		// Get a handle to a Toolhelp snapshot of all the systems processes.

		hSnapShot = lpfCreateToolhelp32Snapshot(
			TH32CS_SNAPPROCESS, 0 ) ;
		if( hSnapShot == INVALID_HANDLE_VALUE )
		{
			FreeLibrary(hInstLib);
			return FALSE;
		}
		
        // Get the first process' information.
        procentry.dwSize = sizeof(PROCESSENTRY32);
        bResult=lpfProcess32First(hSnapShot,&procentry);

        // While there are processes, keep looping and checking.
        while(bResult)
        {
		    // Get a handle to a Toolhelp snapshot of this process.
		    hSnapShotm = lpfCreateToolhelp32Snapshot(
			    TH32CS_SNAPMODULE, procentry.th32ProcessID) ;
		    if( hSnapShotm == INVALID_HANDLE_VALUE )
			{
				CloseHandle(hSnapShot);
			    FreeLibrary(hInstLib);
			    return FALSE;
			}
			// Get the module list for this process
			modentry.dwSize=sizeof(MODULEENTRY32);
			bResultm=lpfModule32First(hSnapShotm,&modentry);

			// While there are modules, keep looping and checking
			while(bResultm)
			{
		        if(strcmp(modentry.szModule,szToTermUpper)==0)
				{
				    // Process found
				    iFound=1;
				    // Found one, return TRUE
					CloseHandle(hSnapShotm);
					CloseHandle(hSnapShot);
		            FreeLibrary(hInstLib);
				    return TRUE;					
				}
				else
				{  // Look for next modules for this process
					modentry.dwSize=sizeof(MODULEENTRY32);
					bResultm=lpfModule32Next(hSnapShotm,&modentry);
				}
			}

			//Keep looking
			CloseHandle(hSnapShotm);
            procentry.dwSize = sizeof(PROCESSENTRY32);
            bResult = lpfProcess32Next(hSnapShot,&procentry);
        }
		CloseHandle(hSnapShot);
	}
	if(iFound==0)
	{
		FreeLibrary(hInstLib);
		return FALSE;
	}
	FreeLibrary(hInstLib);
	return FALSE;
}
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING

#if ( setupUSE_MSW_ENHANCED_PROCESS_KILLING )
int os_msw::terminate_process_by_name( const wxString& named_process_to_terminate )
{

    // A more wxWindows type interface
    const char *szToTerminate = named_process_to_terminate.c_str();

    // See notes to is_named_process_running for credits and info.

	BOOL bResult,bResultm;
	DWORD aiPID[1000],iCb=1000,iNumProc,iV2000=0;
	DWORD iCbneeded,i,iFound=0;
	char szName[MAX_PATH],szToTermUpper[MAX_PATH];
	HANDLE hProc,hSnapShot,hSnapShotm;
	OSVERSIONINFO osvi;
    HINSTANCE hInstLib;
	int iLen,iLenP,indx;
    HMODULE hMod;
	PROCESSENTRY32 procentry;      
	MODULEENTRY32 modentry;

	// Transfer Process name into "szToTermUpper" and
	// convert it to upper case
	iLenP=strlen(szToTerminate);
	if(iLenP<1 || iLenP>MAX_PATH) return 632;
	for(indx=0;indx<iLenP;indx++)
		szToTermUpper[indx]=toupper(szToTerminate[indx]);
	szToTermUpper[iLenP]=0;

     // PSAPI Function Pointers.
     BOOL (WINAPI *lpfEnumProcesses)( DWORD *, DWORD cb, DWORD * );
     BOOL (WINAPI *lpfEnumProcessModules)( HANDLE, HMODULE *,
        DWORD, LPDWORD );
     DWORD (WINAPI *lpfGetModuleBaseName)( HANDLE, HMODULE,
        LPTSTR, DWORD );

      // ToolHelp Function Pointers.
      HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD) ;
      BOOL (WINAPI *lpfProcess32First)(HANDLE,LPPROCESSENTRY32) ;
      BOOL (WINAPI *lpfProcess32Next)(HANDLE,LPPROCESSENTRY32) ;
      BOOL (WINAPI *lpfModule32First)(HANDLE,LPMODULEENTRY32) ;
      BOOL (WINAPI *lpfModule32Next)(HANDLE,LPMODULEENTRY32) ;

	// First check what version of Windows we're in
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    bResult=GetVersionEx(&osvi);
	if(!bResult)     // Unable to identify system version
	    return 606;

	// At Present we only support Win/NT/2000/XP or Win/9x/ME
	if((osvi.dwPlatformId != VER_PLATFORM_WIN32_NT) &&
		(osvi.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS))
		return 607;

    if(osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		// Win/NT or 2000 or XP

         // Load library and get the procedures explicitly. We do
         // this so that we don't have to worry about modules using
         // this code failing to load under Windows 9x, because
         // it can't resolve references to the PSAPI.DLL.
         hInstLib = LoadLibraryA("PSAPI.DLL");
         if(hInstLib == NULL)
            return 605;

         // Get procedure addresses.
         lpfEnumProcesses = (BOOL(WINAPI *)(DWORD *,DWORD,DWORD*))
            GetProcAddress( hInstLib, "EnumProcesses" ) ;
         lpfEnumProcessModules = (BOOL(WINAPI *)(HANDLE, HMODULE *,
            DWORD, LPDWORD)) GetProcAddress( hInstLib,
            "EnumProcessModules" ) ;
         lpfGetModuleBaseName =(DWORD (WINAPI *)(HANDLE, HMODULE,
            LPTSTR, DWORD )) GetProcAddress( hInstLib,
            "GetModuleBaseNameA" ) ;

         if(lpfEnumProcesses == NULL ||
            lpfEnumProcessModules == NULL ||
            lpfGetModuleBaseName == NULL)
            {
               FreeLibrary(hInstLib);
               return 700;
            }
		 
		bResult=lpfEnumProcesses(aiPID,iCb,&iCbneeded);
		if(!bResult)
		{
			// Unable to get process list, EnumProcesses failed
            FreeLibrary(hInstLib);
			return 701;
		}

		// How many processes are there?
		iNumProc=iCbneeded/sizeof(DWORD);

		// Get and match the name of each process
		for(i=0;i<iNumProc;i++)
		{
			// Get the (module) name for this process

	        strcpy(szName,"Unknown");
			// First, get a handle to the process
	        hProc=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,
				aiPID[i]);
	        // Now, get the process name
	        if(hProc)
			{
               if(lpfEnumProcessModules(hProc,&hMod,sizeof(hMod),&iCbneeded) )
			   {
                  iLen=lpfGetModuleBaseName(hProc,hMod,szName,MAX_PATH);
			   }
			}
	        CloseHandle(hProc);
			// We will match regardless of lower or upper case
#ifdef __BORLANDC__
            if(strcmp(strupr(szName),szToTermUpper)==0)
#else
			if(strcmp(_strupr(szName),szToTermUpper)==0)
#endif
			{
				// Process found, now terminate it
				iFound=1;
				// First open for termination
				hProc=OpenProcess(PROCESS_TERMINATE,FALSE,aiPID[i]);
				if(hProc)
				{
					if(TerminateProcess(hProc,0))
					{
						// process terminated
						CloseHandle(hProc);
                        FreeLibrary(hInstLib);
						return 0;
					}
					else
					{
						// Unable to terminate process
						CloseHandle(hProc);
                        FreeLibrary(hInstLib);
						return 602;
					}
				}
				else
				{
					// Unable to open process for termination
                    FreeLibrary(hInstLib);
					return 604;
				}
			}
		}
	}

	if(osvi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
	{
		// Win/95 or 98 or ME
			
		hInstLib = LoadLibraryA("Kernel32.DLL");
		if( hInstLib == NULL )
			return 702;

		// Get procedure addresses.
		// We are linking to these functions of Kernel32
		// explicitly, because otherwise a module using
		// this code would fail to load under Windows NT,
		// which does not have the Toolhelp32
		// functions in the Kernel 32.
		lpfCreateToolhelp32Snapshot=
			(HANDLE(WINAPI *)(DWORD,DWORD))
			GetProcAddress( hInstLib,
			"CreateToolhelp32Snapshot" ) ;
		lpfProcess32First=
			(BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
			GetProcAddress( hInstLib, "Process32First" ) ;
		lpfProcess32Next=
			(BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
			GetProcAddress( hInstLib, "Process32Next" ) ;
		lpfModule32First=
			(BOOL(WINAPI *)(HANDLE,LPMODULEENTRY32))
			GetProcAddress( hInstLib, "Module32First" ) ;
		lpfModule32Next=
			(BOOL(WINAPI *)(HANDLE,LPMODULEENTRY32))
			GetProcAddress( hInstLib, "Module32Next" ) ;
		if( lpfProcess32Next == NULL ||
			lpfProcess32First == NULL ||
		    lpfModule32Next == NULL ||
			lpfModule32First == NULL ||
			lpfCreateToolhelp32Snapshot == NULL )
		{
			FreeLibrary(hInstLib);
			return 703;
		}
			
		// The Process32.. and Module32.. routines return names in all uppercase

		// Get a handle to a Toolhelp snapshot of all the systems processes.

		hSnapShot = lpfCreateToolhelp32Snapshot(
			TH32CS_SNAPPROCESS, 0 ) ;
		if( hSnapShot == INVALID_HANDLE_VALUE )
		{
			FreeLibrary(hInstLib);
			return 704;
		}
		
        // Get the first process' information.
        procentry.dwSize = sizeof(PROCESSENTRY32);
        bResult=lpfProcess32First(hSnapShot,&procentry);

        // While there are processes, keep looping and checking.
        while(bResult)
        {
		    // Get a handle to a Toolhelp snapshot of this process.
		    hSnapShotm = lpfCreateToolhelp32Snapshot(
			    TH32CS_SNAPMODULE, procentry.th32ProcessID) ;
		    if( hSnapShotm == INVALID_HANDLE_VALUE )
			{
				CloseHandle(hSnapShot);
			    FreeLibrary(hInstLib);
			    return 704;
			}
			// Get the module list for this process
			modentry.dwSize=sizeof(MODULEENTRY32);
			bResultm=lpfModule32First(hSnapShotm,&modentry);

			// While there are modules, keep looping and checking
			while(bResultm)
			{
		        if(strcmp(modentry.szModule,szToTermUpper)==0)
				{
				    // Process found, now terminate it
				    iFound=1;
				    // First open for termination
				    hProc=OpenProcess(PROCESS_TERMINATE,FALSE,procentry.th32ProcessID);
				    if(hProc)
					{
					    //ROB: Replaced with best_terminate_process
					    //if(TerminateProcess(hProc,0))
					    if ( best_terminate_process( hProc, 0 ) )
						{
						    // process terminated
							CloseHandle(hSnapShotm);
							CloseHandle(hSnapShot);
							CloseHandle(hProc);
			                FreeLibrary(hInstLib);
						    return 0;
						}
					    else
						{
						    // Unable to terminate process
							CloseHandle(hSnapShotm);
							CloseHandle(hSnapShot);
							CloseHandle(hProc);
			                FreeLibrary(hInstLib);
						    return 602;
						}
					}
				    else
					{
					    // Unable to open process for termination
						CloseHandle(hSnapShotm);
						CloseHandle(hSnapShot);
			            FreeLibrary(hInstLib);
					    return 604;
					}
				}
				else
				{  // Look for next modules for this process
					modentry.dwSize=sizeof(MODULEENTRY32);
					bResultm=lpfModule32Next(hSnapShotm,&modentry);
				}
			}

			//Keep looking
			CloseHandle(hSnapShotm);
            procentry.dwSize = sizeof(PROCESSENTRY32);
            bResult = lpfProcess32Next(hSnapShot,&procentry);
        }
		CloseHandle(hSnapShot);
	}
	if(iFound==0)
	{
		FreeLibrary(hInstLib);
		return 603;
	}
	FreeLibrary(hInstLib);
	return 0;
}
#endif // setupUSE_MSW_ENHANCED_PROCESS_KILLING

//----------------------------------------------------------------------------------------
// End OS removal condition
//----------------------------------------------------------------------------------------
    
#endif  // __WXMSW__