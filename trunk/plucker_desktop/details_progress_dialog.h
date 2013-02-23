//----------------------------------------------------------------------------------------
/*!
    \file       details_progress_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes details_progress_dialog class  
    \author     Robert O'Connor
    \date       2002/01/03    
 */ 
// RCS-ID:      $Id: details_progress_dialog.h,v 1.8 2007/06/30 03:09:28 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _DETAILS_PROGRESS_DIALOG_H_
#define _DETAILS_PROGRESS_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "details_progress_dialog.h"
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

#include "wx/dialog.h"
#include "wx/process.h"
#include "wx/gauge.h"
#include "wx/collpane.h"
#include "wx/sizer.h"
#include "details_progress_listbox.h"

//----------------------------------------------------------------------------------------
// Class definition: details_progress_dialog
//----------------------------------------------------------------------------------------

//! A dialog that shows the progress of channel updates
/*! 
    It has a details_progress_listbox that can record some details (which has a context menu,
    for clearing, selecting and copying). On the right side of the dialog is 
    3 buttons for stop/ok, show/hide details and export details. 
    
    Class can be used directly or be derived from.
    
    \note The listbox in the XRC file _must_ have the grow flags set to expand to fit 
    in all directions, otherwise will impinge on top part when details button cuts off 
    that part of the dialog.
    
    \note The wxStaticText labels that get updated are fixed at width of 350, expand to 
    grow, and have wxST_NO_AUTORESIZE style (one or more of these may be unnecessary, 
    but need to not have the newly added text truncate.
 */
class details_progress_dialog : public wxDialog
{

    DECLARE_DYNAMIC_CLASS( details_progress_dialog )

public: 
 
    //! Constructor.
    /*! 
        \param parent The parent window.
        \param automatically_close_when_done Whether to close when done. This can be 
            overridden later with set_automatically_close_when_done();        
        \param show_hide_listbox_configuration_key Configuration key of active             
            configuration to store show/hide details. wxEmptyString means don't load/save. 
        \param log_filename  Filename of the process logfile. 
            A plkrFILE_FRIENDLY_DATETIME_STAMP in the string will replace with a file-friendly datetime string.
            If using DATETIMESTAMP, recommended to keep the rest of name below 11 characters
            total if need Macintosh 31-character limit requirement.
        \param log_path Default path of the logfile in dialog.
            Default of wxEmptyString means the current directory.
        \param scroll_history Number of lines to keep in the listbox at one time. After this,
            newly appended lines will remove the topmost line. Can be a number, or 
            can use one of the options* to get a recommended for OS or an infinite one.
     */
    details_progress_dialog( wxWindow* parent,
             bool automatically_close_when_done = TRUE,
             const wxString& show_hide_listbox_configuration_key = wxEmptyString, 
             const wxString& log_filename = wxEmptyString,
             const wxString& log_path = wxEmptyString,
             long scroll_history = optionDETAILS_PROGRESS_LISTBOX_RECOMMENDED_SCROLL_HISTORY
           );
    
    // Dummy construtor for RTTI macros
    details_progress_dialog() {}
                     
    //! Destructor.             
    virtual ~details_progress_dialog();        
   
    //! Send an abort signal to the dialog, that will abort as soon as possible.
    /*! 
        \bool TRUE to send an abort signal.
     */
    void set_abort_signal_was_entered( bool abort );
    
    //! Get whether an abort signal was entered.
    bool get_abort_signal_was_entered();
    
    //! Set whether the progress dialog should automatically close when it is finished.
    /*!
        This is a separate option that can be called anytime, such as during your 
        on_process_generated_an_output_line() if you detected an error, you can prevent 
        the dialog from automatically closing until users presses OK. 
        But if things went okay, then you can leave it as true. 
        The default is to automatically close when done.
        \param automatically_close TRUE to automatically close when all done.
     */
    void set_automatically_close_when_done( bool automatically_close_when_done = TRUE );
    
    //! Report that all the chores completed all the way to their end, without an abort.
    /*!
        Change 'Stop" button label to OK, 
        change the return value from wxID_CANCEL to wxID_OK,
        and send off to close_dialog() if m_automatically_close_when_done=TRUE.
     */    
    void all_chores_completed_without_interruption();   
    
    //---Accessors to manipulating the progress controls-----------------------------------------                                   
    
    //! Accessor to details_progress_dialog control
    wxGauge*          get_total_gauge() const { return m_total_gauge; }
    
    //! Accessor to details_progress_dialog control
    wxStaticText*     get_total_statictext() const { return m_total_statictext; }
    
    //! Accessor to details_progress_dialog contr
    wxGauge*          get_subtask_gauge() const { return m_subtask_gauge; }
    
    //! Accessor to details_progress_dialog contr
    wxStaticText*     get_subtask_statictext() const { return m_subtask_statictext; }
    
    //! Accessor to details_progress_dialog contr
    wxButton*         get_stop_button() const { return m_stop_button; }
    
    //! Accessor to details_progress_dialog contr
    wxButton*         get_export_button() const { return m_export_button; }
    
    //! Accessor to details_progress_dialog contr
    details_progress_listbox* get_details_listbox() const{ return m_details_progress_listbox; }
    
    //! Sets a gauge to the specified value and range, without flicker.
    /*!
        \param value The value the gauge should be set to.
        \param range The range the gauge should be set to.
     */
    void set_flickerfree_gauge_and_range( wxGauge& gauge, int value,
                                          int range 
                                        );   

protected:

    //! Start up the actions watched by the progress dialog. Called from on_paint().
    virtual void action_to_do_when_dialog_is_ready() {}
    
    //! Clean up any actions before the dialog vanishes. Called from on_close().
    virtual void action_to_do_before_dialog_finishes_closing() {}

    //! Clean up any actions after the dialog EndModal()s. Called from on_close().
    /*! 
        Needed for MSW to drop in something to terminate program if this is the 
        only window, as otherwise app doesn't seem to quit
     */
    virtual void action_to_do_after_dialog_finishes_closing() {}

private:

    //-----HManipulating the collapsible pane-----------------------------------------------------
    //! Action to do when the user expands and collapses the collapsible pane by clicking 'Details'
    void on_collapsiblepane_changed( wxCollapsiblePaneEvent& event );

    //! Collapse the collapsible panel to hide the listbox
    void collapse_collapsiblepane();

    //! Expand the collapsible panel to show the listbox
    void expand_collapsiblepane();
    
    //! Updates the dialog size (and sizers) after a hide_listbox() or a show_listbox().
    /*!
        \param desired_dialog_height The overall height desired for the dialog
     */
    void update_dialog_size_after_hide_or_show_listbox( int desired_dialog_height );
    
    //! Action to dow when the 'Export' button was clicked.
    void on_export_button( wxCommandEvent &event ); 

    //! Action to do when dialog is being initialized (initially sets to show/hide).
    /*!
        \note This can't be done in the constructor (crashes) and nasty shift of window
            size if done on an on_paint_dialog. Because of the Show() requirement for
            GTK, this makes another InitDialogEvent which could hide the dialog again
            and go into infinite recursion. Therefore, carray a m_is_initial_init_event
            also.
     */
    void on_init_dialog( wxInitDialogEvent &event ); 

    //! Whether it it the initial dialog init event.        
    /*!
        Since the update_dialog_size_after_hide_or_show_listbox for GTK has a Show().
        which would call another InitDialog which would end up calling hide_listbox
        which would call update_dialog_size_after_hide_or_show_list again.
     */
    bool m_is_initial_init_event;

    //! This dialog's height with a listbox.
    int  m_dialog_height_with_listbox;
    
	/*
    //! This dialog's height without a listbox.
    int m_dialog_height_without_listbox;
    */
	
    //! Whether the listbox is shown.
    bool m_details_progress_listbox_is_shown;  
   
    //! Paased key of active configuration to store whether to show/hide details. 
    wxString m_show_hide_listbox_configuration_key;

    //---Managing starting/stopping of the action with open/close of dialog----------------
  
    //! Called during an EVT_FOCUS_EVENT                                            
    void on_paint_dialog( wxPaintEvent& event );
    
    //! Action to do when 'Stop' (aka OK) button was clicked. It just calls close_dialog
    
    void on_stop_button( wxCommandEvent &event );
   
    //! Close the dialog. 
    //void close_dialog();
    void OnCloseWindow( wxCloseEvent& event );

    //! Whether to start a new plucker-build process in the update_channels() loop.
    /*!
        Set this to false to break the loop, for example when hit "Stop" button.
        The derived class also has access to this private variable via the
        set_abort_signal_was_entered() and get_abort_signal_was_entered() functions
     */
    bool m_abort_signal_was_entered;

    //! Whether it is the initial paint event.
    /*!
        Since it is tied to a focus event, and we could have the focus many times.
     */
    bool m_is_initial_paint_event;
   
    //! TRUE to automatically close when all complete.
    bool m_automatically_close_when_done;
   
    //! The return value of the dialog, either wxID_OK or wxID_CANCEL.
    /*! 
        Return value depends on whether successfully made it through all the 
        way through the start_process_execution_chain and onto 
        on_completed_main_execution_loop().
     */   
    int m_dialog_return_value;
           
    //-----Control pointers-----------------------------------------------------------       
    
    wxGauge*			m_total_gauge;
    wxStaticText*		m_total_statictext;
    wxGauge*			m_subtask_gauge;
    wxStaticText*		m_subtask_statictext;
    wxButton*			m_stop_button;
    wxButton*			m_details_button;
    wxButton*			m_export_button; 
    wxCollapsiblePane*	m_collapsiblepane;
	
    //! A details_progress_listbox that will hold the output.
    /*!
        \note A listbox on MSW appears to be about 2x as fast to insert as a textctrl.
     */     
    details_progress_listbox* m_details_progress_listbox;
    
    //-----wxWindows event table macro-------------------------------------------------       
               
    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ENHANCED_PROGRESS_DIALOG

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  // _DETAILS_PROGRESS_DIALOG_H_
