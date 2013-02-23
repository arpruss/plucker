//----------------------------------------------------------------------------------------
/*!
    \file       help_controller.h        
    \modified                            
    \copyright  (c) Dominic Mazzoni, Robert O'Connor    
    \licence    GPL
    \brief      Describes help_controller class    
    \author     Robert O'Connor
    \date       2001/10/20
 */  
// RCS-ID:      $Id: help_controller.h,v 1.8 2003/03/17 17:43:20 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _HELP_CONTROLLER_H_
#define _HELP_CONTROLLER_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "help_controller.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_ONLINE_HELP )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/html/helpctrl.h"
#include "wx/confbase.h"
#include "wx/fileconf.h"

//----------------------------------------------------------------------------------------
// Class definition: help_controller
//----------------------------------------------------------------------------------------

//! A class to manage displaying of online help
class help_controller : public wxHtmlHelpController
{

    // I guess one doesn't use DECLARE_DYNAMIC_CLASS on these.
    
public:

    //! Constructor.
    /*!
        \param parent Parent window of the help controller.
     */   
    help_controller();

    //! Destructor.
    ~help_controller();

    //! Gets the global object or creates one if none exists.
    /*! 
        \param create_on_demand TRUE to create if not exist. Otherwise
            don't create one [useful if you just want to get() to close it.]
     */
    static help_controller* get( bool create_on_demand = TRUE );
    
    //! Sets the global object and returns a pointer to previous one (may be NULL).
    /*!
        \param desired_help_controller The new help_controller.
     */
    static help_controller* set( help_controller* desired_help_controller );

    //! Adds a specified "book" (.htb file) to the help controller
    /*!
        Also error handles if something went wrong.
        \param help_fullname The fullname of the .htb file to add.
     */
    void add_book( const wxString& help_fullname );
    
    //! Shows the specified help topic
    /*!
        \param parent Parent window of the help controller.
        \param topic The topic to be shown.
     */   
    void show_help_topic( const wxString& topic );
    
    //! Shows the help frame, with no topics initially selected.
    /*!
        \param parent Parent window of the help controller.
     */   
    void show_help();
    
    //! Search help for a keyword, and display results.
    /*! Prompts for a text to search for via a text entry dialog, then shows help 
        controller selected to the search tab. It starts searching help, and displays 
        the pages containing that phrase in the display window.
        \param parent Parent window of the help controller.
     */  
    void search_help( wxWindow* parent );
    
    //! Closes the help (frame), but doesn't delete the help controller 
    /*!
        Deletion of the whole help controller is done via module. The frame is 
        destroyed by the base class wxHelpHelpController
     */
    void close();

private:
    
    // Singleton instance:
    static help_controller* ms_instance; 
    
    // Config file to store help settings.
    wxFileConfig*           m_help_configuration;  
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ONLINE_HELP

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_HELP_CONTROLLER_H_
