//----------------------------------------------------------------------------------------
/*!
    \file       commandline_parser.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes commandline_parser class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: commandline_parser.h,v 1.5 2007/06/29 00:58:51 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _COMMANDLINE_PARSER_H__
#define _COMMANDLINE_PARSER_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "commandline_parser.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/cmdline.h"             // wxCmdLineParser

//----------------------------------------------------------------------------------------
// Class definition: commandline_parser
//----------------------------------------------------------------------------------------

//! Static-only class to wrap handling the commandline.
class commandline_parser : wxCmdLineParser
{

public:
    
    //! Constructor.
    commandline_parser();

    // NOTE: NO destructor in this class, as wxCmdLineParser destructor is not virtual. 

    //! Gets the global object or creates one if none exists.
    static commandline_parser* get();
    
    //! Sets the global object and returns a pointer to previous one (may be NULL).
    /*!
        \param desired_plucker_controller The new plucker_controller
     */
    static commandline_parser* set( commandline_parser* desired_commandline_parser );    
    //! Do the things requested on the commandline.  
    /*!
        \param argc The number of commandline arguments.
        \param argv The commandline arguments.
        \return TRUE if the application should continue. FALSE if app should terminate.
     */
    bool    do_commandline( int argc, wxChar** argv );
                        
private:

    // Singleton instance of plucker_controller
    static commandline_parser* ms_instance;   

    //! Prepend some extra info to go before a printout of the 'usage' option list.
    void    prepend_extra_help_notice();

    //! Parse the commandline parameters
    /*!
        \param commandline_parser Commandline parser to use.
     */
    void    parse_parameters();
    
    //! Send an inform message to be written out on the console.
    /*! 
        \param message Message text to be sent to inform the user.
     */
    void    send_inform_message( const wxString& message );   
    
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _COMMANDLINE_PARSER_H_

