//----------------------------------------------------------------------------------------
/*!
    \file       plucker-desktop.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes the_app class    
    \author     Robert O'Connor
    \date       2001/09/13   
 */  
// RCS-ID:      $Id: plucker-desktop.h,v 1.16 2007/06/29 00:58:55 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PLUCKER_DESKTOP_H_
#define _PLUCKER_DESKTOP_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "plucker-desktop.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/app.h"             // wxApp
#if ( setupUSE_SINGLE_INSTANCE_CHECKER )
    #include "wx/snglinst.h"    // wxSingleInstanceChecker
#endif
#if ( setupUSE_INTERNATIONALIZATION )
    #include "wx/intl.h"        // wxLocale and other internationalization methods
#endif

//----------------------------------------------------------------------------------------
// Class definition: the_app
//----------------------------------------------------------------------------------------

//! The main Plucker Desktop application class: handles OnInit(), OnExit() of the program.
class the_app : public wxApp
{

public:

    //! Override wxApp base class OnInit() function for startup code.
    /*!
        This is the 'Main program' equivalent: the program execution "starts" here.
        The function is called on application startup and is a good place for the app
        initialization [doing it here and not in the constructor allows to have an error
        \return if OnInit() returns false, the application terminates].
     */
    virtual bool OnInit(); 
       
    //! Override base class wxApp's OnExit() function. 
    /*!
        This code is executed when the application is about to terminate. This is the 
        place to call functions that delete globals, etc.
        \return Any arbitrary integer that is desired. This application returns 0, 
        since that is usually what applications return when they exit successfully.
     */
    virtual int OnExit();

    //! Set the return code returned by application when it terminates.
    void    set_application_return_code( int return_code );

#if ( setupUSE_INTERNATIONALIZATION )
    //! Accessor. Get a pointer to the locale (tranlation) for the application.
    /*!
        \param new_locale The new locale to use.
     */
    wxLocale* get_application_locale() { return &m_locale; }
#endif

protected:

#if ( setupUSE_INTERNATIONALIZATION )
    //! The locale (translation) the application will be using.
    wxLocale m_locale;     
#endif
    
private:

    //! Things to do regardless of whether or started with commandline arguements.    
    void    on_startup_common();

    //! Things to do if started as a commandline utility.  
    /*!
        \param argc The number of commandline arguments.
        \param argv The commandline arguments.
     */
    void    on_startup_with_commandline_arguments( int argc, wxChar** argv );
    
    //! Things to do if started in a normal gui way, ie not as a commandline utility.    
    void    on_startup_without_commandline_arguments();

    //! Helper function. loads the specified XRC XML resource file.
    /*!
        \param xrc_filename XRC file to load. The fullname to the file will be built 
            using this filename.
     */
    void    load_xrc_resource( const wxString& xrc_filename );   

#if ( setupUSE_SINGLE_INSTANCE_CHECKER )
    //! The single instance checker
    wxSingleInstanceChecker* m_single_instance_checker;
#endif

    //! Return code of the application.
    int     m_return_code;

};

//----------------------------------------------------------------------------------------
// wxWindows macro: Declare the application
//----------------------------------------------------------------------------------------

/*! 
    \ingroup wxwindows_macros
    This macro is later implemented by IMPLEMENT_APP. It allows the global single 
    application object to be called with the function wxGetApp(). 
    
    It is used, for example, in internationalization.cpp calls to wxGetApp()->m_locale .    
    \brief wxWindows macro: creates a forward declaration of wxGetApp().    
 */
DECLARE_APP( the_app );

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _ADD_CHANNEL_WIZARD_H_
