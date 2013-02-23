//----------------------------------------------------------------------------------------
/*!
    \file       main_frame.h        
    \modified                        
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes main_frame class    
    \author     Robert O'Connor
    \date       2001/10/20
 */  
// RCS-ID:      $Id: main_frame.h,v 1.8 2007/06/29 00:58:53 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _MAIN_FRAME_H_
#define _MAIN_FRAME_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "main_frame.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/image.h"
#include "wx/frame.h"
#include "main_listctrl.h"        // main_listctrl

//----------------------------------------------------------------------------------------
// Class definition: main_frame
//----------------------------------------------------------------------------------------

//! The main frame of the application
/*!
    The main frame is composed of a menubar and toolbar loaded from XRC, a main_listctrl
    and a statusbar and the bottom.
 */
class main_frame : public wxFrame
{

public:
    
    //! Constructor
    /*!
        \param parent Parent window of the frame. 
     */   
    main_frame( wxWindow* parent=(wxWindow *)NULL);
    
    //! Destructor            
    ~main_frame();
    
    //! Startup code for the frame
    /*! 
        Public, since called from application startup [ OnInit() in plucker-desktop.cpp ]
     */ 
    void frame_startup_init();
    
    //! Get a pointer to the main_listctrl.
    /*!
        \return Pointer to the main_listctrl.
     */
    main_listctrl*  get_main_listctrl() const { return m_main_listctrl; }
    
private:

    void on_add_channel_tool_or_menuitem( wxCommandEvent& event );
    void on_add_channel_wizard_tool_or_menuitem( wxCommandEvent& event );   
    void on_configure_channel_tool_or_menuitem( wxCommandEvent& event );
    void on_delete_channels_tool_or_menuitem( wxCommandEvent& event );
    void on_exit_tool_or_menuitem( wxCommandEvent& event );
    
    void on_paste_url_as_new_channel_tool_or_menuitem( wxCommandEvent& event );
    void on_update_ui_paste_url_as_new_channel_tool_or_menuitem( wxUpdateUIEvent& event );
    void on_paste_fullnames_as_new_separate_channels_tool_or_menuitem( wxCommandEvent& event );    
    void on_update_ui_paste_fullnames_as_new_separate_channels_tool_or_menuitem( wxUpdateUIEvent& event );
    
    void on_update_selected_tool_or_menuitem( wxCommandEvent& event );
    void on_update_due_tool_or_menuitem( wxCommandEvent& event );
    void on_update_all_tool_or_menuitem( wxCommandEvent& event );

    void on_preferences_tool_or_menuitem( wxCommandEvent& event );
    void on_channel_defaults_tool_or_menuitem( wxCommandEvent& event );
    void on_install_software_tool_or_menuitem( wxCommandEvent& event );
    void on_setup_wizard_tool_or_menuitem( wxCommandEvent& event );

    void on_help_contents_tool_or_menuitem( wxCommandEvent& event );
    void on_help_on_this_dialog_tool_or_menuitem( wxCommandEvent& event );
    void on_search_help_tool_or_menuitem( wxCommandEvent& event );
    void on_tip_of_the_day_tool_or_menuitem( wxCommandEvent& event );
    void on_about_tool_or_menuitem( wxCommandEvent& event );

    void on_update_ui_main_listctrl( wxUpdateUIEvent& event );

    //! Action that is performed everytime the timer completes a cycle.
    void on_timer( wxTimerEvent& event );

    //! Need to stop the timer as part of the things to do when closing the frame
    //! \test Test the stop timer on close approach.
    void on_close( wxCloseEvent& event );

    //! An icon for the corner of dialog and application's taskbar button
    wxIcon          m_icon;
    
    //! An instance of the 'main_listctrl' derived class
    main_listctrl*  m_main_listctrl;
    
    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _MAIN_FRAME_H_
