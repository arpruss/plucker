//----------------------------------------------------------------------------------------
// Name:        details_progress_dialog.cpp
// Purpose:     Desribes details_process_progress_dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: details_progress_dialog.cpp,v 1.13 2007/06/30 03:09:28 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "details_progress_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_ENHANCED_PROGRESS_DIALOG )

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

#include "details_progress_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/txtstrm.h"
#include "wx/confbase.h"                  
#include "wx/fileconf.h"

// ---------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

//----------------------------------------------------------------------------------------
// wxWindows macro: implement dynamic class
//----------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( details_progress_dialog, wxDialog )

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( details_progress_dialog, wxDialog )
    EVT_BUTTON( XRCID( "details_progress_dialog_stop_button" ), details_progress_dialog::on_stop_button )
    EVT_BUTTON( XRCID( "details_progress_dialog_export_button" ), details_progress_dialog::on_export_button ) 
    
    // The collapse_collapsible_panel was expanded or collapsed
    EVT_COLLAPSIBLEPANE_CHANGED( XRCID( "details_progress_dialog_collapsiblepane" ), details_progress_dialog::on_collapsiblepane_changed )
    
    // An EVT_INIT_DIALOG for when the dialog is being initialized.
    EVT_INIT_DIALOG( details_progress_dialog::on_init_dialog )
    
    // An EVT_PAINT for some code to start as soon as the dialog is shown.
    EVT_PAINT( details_progress_dialog::on_paint_dialog )
   
    // We have to specify a EVT_CLOSE in a dialog-only app to correctly terminate
    // the app, when hit the 'X' button (or equivalent) in dialog's top corner, or turn off
    // the computer.
    EVT_CLOSE( details_progress_dialog::OnCloseWindow )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------------------------

details_progress_dialog::details_progress_dialog( wxWindow* parent,
                                  bool automatically_close_when_done,
                                  const wxString& show_hide_listbox_configuration_key,
                                  const wxString& log_filename,
                                  const wxString& log_path,
                                  long scroll_history
                                )
{   
    //---Load the progress dialog and listbox---------------------------------------------       

    wxXmlResource::Get()->LoadDialog( this, parent, wxT( "details_progress_dialog" ) );
    
	// Make a progress listbox, passing it its parameters
    m_details_progress_listbox  = new details_progress_listbox( this, -1,
                                    log_filename,
                                    log_path, 
                                    scroll_history,
                                    wxDefaultPosition, wxDefaultSize,
                                    0, NULL,
                                    wxLB_EXTENDED | wxLB_HSCROLL | wxLB_ALWAYS_SB );
    
    // Attach the progress listbox to the dialog.                                                        
    wxXmlResource::Get()->AttachUnknownControl( wxT( "details_progress_dialog_listbox" ),
                                           m_details_progress_listbox );                                                                                      
    wxLogDebug( wxT( "m_details_progress_listbox successfully attached as unknown control" ) );     

	//---Store the rest of the construtor information that is kept------------------------
    
    // Store automatically close when done...
    m_automatically_close_when_done         = automatically_close_when_done;
    // ...and the key to load/save the show details status.
    m_show_hide_listbox_configuration_key   = show_hide_listbox_configuration_key;

    //---Set up things for showing/hiding the listbox-------------------------------------
    
    // Save the size of the dialog with listbox for a later restore.
    // MOVED DOWN BELOW
    // m_dialog_height_with_listbox            = GetSize().y;
	
    // Initialize the listbox as visible
    m_details_progress_listbox_is_shown     = TRUE;
    
    // True since it will be the initial init event
    m_is_initial_init_event                 = TRUE;
    
    //---Some more initializers-----------------------------------------------------------

    // Initialize the return value to be wxID_CANCEL. It will become wxID_OK if get to
    // on_all_items_completed_without_interruption().
    m_dialog_return_value                   = wxID_CANCEL;
    // Initialize to FALSE to let more update_channels() loop iterations make more plucker-builds.
    m_abort_signal_was_entered              = FALSE;
    // Initialize to TRUE so that the initial paint event will trigger the execute code
    m_is_initial_paint_event                = TRUE;

    //-----Set up the convenience pointers for the commonly used XRC controls------------       
    
    // This is an alternative to XRCCTRL... each and every time in our protected
    // methods. Obviously this has to come before the flickerless gauge code below.
    m_total_gauge			= XRCCTRL( *this, "details_progress_dialog_total_gauge", wxGauge );
    m_total_statictext		= XRCCTRL( *this, "details_progress_dialog_total_statictext", wxStaticText );
    m_subtask_gauge			= XRCCTRL( *this, "details_progress_dialog_subtask_gauge", wxGauge );
    m_subtask_statictext	= XRCCTRL( *this, "details_progress_dialog_subtask_statictext", wxStaticText );
    m_stop_button			= XRCCTRL( *this, "details_progress_dialog_stop_button", wxButton );
    m_export_button			= XRCCTRL( *this, "details_progress_dialog_export_button", wxButton );
    m_collapsiblepane		= XRCCTRL( *this, "details_progress_dialog_collapsiblepane", wxCollapsiblePane );
    
	//----For adjusting sizers----------------------------------------------------------- 
///----------SHOULDBE ABLE TO DELETE, SINCE SETTING SIZER IN DIALOG OR WHEN EXPAND/COLLAPSE
#if ( setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS )	
	wxLogDebug( wxT( "Running a setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS" ) );
	
	wxSize	default_size = GetSize();
	wxSize  new_size;
	new_size.x     = default_size.x + 1;
    new_size.y     = default_size.y + 200;
	SetSize( new_size );
#endif
///----------SHOULDBE ABLE TO DELETE, SINCE SETTING SIZER IN DIALOG OR WHEN EXPAND/COLLAPSE

    // Save the size of the dialog with listbox for a later restore.
    m_dialog_height_with_listbox            = GetSize().y;

    //----For set_flickerfree_value_and_range---------------------------------------------       
    m_total_gauge->SetRange( plkrFIXED_PERCENTAGE_GAUGE_RANGE );
    m_subtask_gauge->SetRange( plkrFIXED_PERCENTAGE_GAUGE_RANGE );

}


details_progress_dialog::~details_progress_dialog()
{

}


void details_progress_dialog::set_abort_signal_was_entered( bool abort )
{
    m_abort_signal_was_entered = abort;
}    


bool details_progress_dialog::get_abort_signal_was_entered()
{
    return m_abort_signal_was_entered;
}


void details_progress_dialog::set_automatically_close_when_done( bool automatically_close_when_done )
{
    m_automatically_close_when_done = automatically_close_when_done;
}

//---Manipulating the progress controls--------------------------------------------------

void details_progress_dialog::set_flickerfree_gauge_and_range( wxGauge& gauge, int value, int range )
{ 
    // Note that in the constructor, range of gauges was set to
    // plkrFIXED_PERCENTAGE_GAUGE_RANGE.

     // Start at zero, in case range is also zero, then value will become zero.
    int new_value = 0;
    // If not dividing by zero...
    if ( range != 0 )
    {
        // ...Get the new value on a range of 1000, based on value/range that was passed.
        // Needed a double value.
        new_value = ( plkrFIXED_PERCENTAGE_GAUGE_RANGE * value ) / range;
    }
    
    gauge.SetValue( new_value );
}              

//----------------------------------------------------------------------------------------
// Private members
//----------------------------------------------------------------------------------------

//-----Manipulating the collapsible panel ------------------------------------------------

void details_progress_dialog::on_collapsiblepane_changed( wxCollapsiblePaneEvent& event )
{
    // Is this backwards??
    if ( !  m_collapsiblepane->IsCollapsed() )
	{
		expand_collapsiblepane();
    }
}


void details_progress_dialog::collapse_collapsiblepane()
{
    wxLogDebug( wxT( "Entering progress_dialog::collapse_collapsible_pane()" ) );    

    // Don't need to do anything since the collapsible panel can take care of it all.
    // update_dialog_size_after_hide_or_show_listbox( m_dialog_height_without_listbox );
}


void details_progress_dialog::expand_collapsiblepane()
{
    wxLogDebug( wxT( "Entering progress_dialog::expand_collapsible_pane()" ) );   

    // ..and some common code to update the dialog's size hints, etc.
    update_dialog_size_after_hide_or_show_listbox( m_dialog_height_with_listbox );
}


void details_progress_dialog::update_dialog_size_after_hide_or_show_listbox( int desired_dialog_height )
{     
    // NOTE: This is FUNDAMENTAL for collapsing part of a window. 
    // We have to reset min size constraints or Fit() would never reduce the
    // dialog size when collapsing it and we have to reset max constraint
    // because it wouldn't expand it otherwise. Note: these are members of the 
    // wxWindow class.
    m_minHeight = -1;
    m_maxHeight = -1;

    // Set the new size hints of the dialog, before we try to set the size (otherwise
    // won't be able to set the size, since it won't let us set to a size that is 
    // not within the range of size hints. Note that we set m_maxHeight to -1 earlier
    // in this function.
    SetSizeHints( GetSize().x, desired_dialog_height, m_maxWidth, m_maxHeight );
    wxLogDebug( wxT( "SetSizeHints=%d,%d,%d,%d" ), GetSize().x, desired_dialog_height, m_maxWidth, m_maxHeight ); 
    
    // Set the new size of the dialog. The -1 parameter means leave the width to 
    // whatever it was before.
    SetSize( -1, desired_dialog_height );
    
    //! \test Does MSW redraw the item? GTK has a Show(). \Does WXMOTIF need this too?
#ifdef __WXGTK__
    // Vaclav Slavic: This Show(TRUE) is neccessary in order to force dialog redraw under
    // WindowMaker or fvwm2 (and probably other broken WMs). Otherwise, the listbox 
    // wouldn't be displayed.
    Show( TRUE );
#endif // wxGTK

}


void details_progress_dialog::on_export_button( wxCommandEvent &event )
{
    m_details_progress_listbox->export_to_file();
}


void details_progress_dialog::on_init_dialog( wxInitDialogEvent& event )
{    
    wxLogDebug( wxT( "Entering details_progress_dialog::on_init_dialog()" ) );
  
    // Only execute this in the first init dialog event (since GTK has a 
    // Show() in update_dialog_size_after_hide_or_show_listbox() ).
    if ( m_is_initial_init_event ) 
    {
        // Set the initial init event to FALSE, so we can't run this stuff again. 
        m_is_initial_init_event = FALSE;  

        // Look in the configuration and see if they desired to hide it. If so, hide it
        // initially.
        if ( wxConfigBase::Get( FALSE ) ) 
        {
            if ( m_show_hide_listbox_configuration_key != wxEmptyString )
            {
                bool initially_show_details;
                initially_show_details = (bool)wxConfigBase::Get()->Read( m_show_hide_listbox_configuration_key,
                                                                          1L );
                wxLogDebug( wxT( "Read value in m_show_hide_listbox_configuration_key=%d" ), initially_show_details );
                if ( ! initially_show_details ) 
                {
                    // Dialog is constructed with the listbox showing. Therefore, this call
                    // will hide it.                    
                     m_collapsiblepane->Collapse();
                    // If initialized to not show details, then it is no longer vertically centered.
                    // So recenter it.
                    Centre( wxBOTH ); 
                }  
            }
        }
        
        
    }
    
    // Skip up to the next available handler
    event.Skip();
}

//-----Managing start/stop of processes with open/close of dialog-------------------------

void details_progress_dialog::on_paint_dialog( wxPaintEvent& event )
{
    wxLogDebug( wxT( "Entering details_progress_dialog::on_paint_dialog()" ) );

   // If this is the initial paint event (we set it in the constructor to be TRUE)
   if ( m_is_initial_paint_event ) 
   {                     
        // Set the initial paint event to FALSE, so we can't run this stuff again. 
        m_is_initial_paint_event = FALSE;       
        
        // Do the virtual item that we should do when the dialog is ready.
        action_to_do_when_dialog_is_ready();
    }    
    
    // Skip up to the next available handler, regardless of whether or not this 
    // is the initial paint event.
    event.Skip();
    
    wxLogDebug( wxT( "Leaving details_progress_dialog::on_paint_dialog()" ) );
}


void details_progress_dialog::on_stop_button( wxCommandEvent &event )
{   
    wxLogDebug( wxT( "Entering details_progress_dialog::on_stop_button()" ) );
    // Do our close dialog code (kill process, etc)
    Close();    
}


void details_progress_dialog::all_chores_completed_without_interruption()
{
    wxLogDebug( wxT( "Entering details_progress_dialog::all_chores_completed_without_interruption()" ) );
    // Set the return value to wxID_OK, since got all the way through the
    // execution loop.
    m_dialog_return_value = wxID_OK;

    // Update the 'Stop' buttons caption to 'OK'.
    m_stop_button->SetLabel( _( "OK" ) );

    // If automatically close when done, then do the close behavior
    if ( m_automatically_close_when_done )
    {
        // Can't do this: close_dialog(). The program will hang.
        // close_dialog();

        // Instead: post a new close event to the event handling system.

        // Construct a new event...
        wxCloseEvent a_close_event( wxEVT_CLOSE_WINDOW );

        // Since wxDialogs, like most wx things are derived from wxEventHandler,
        // we can just call wxEventHandler::AddPendingEvent().
        // AddPendingEvent() puts event in queue to be dealt with when possible.
        // 'Process'ing an event does it straight away. Note: AddPendingEvent()
        // may change to PostEvent() some time in the future.
        AddPendingEvent( a_close_event );
    }
}


void details_progress_dialog::OnCloseWindow( wxCloseEvent& event )
{
    // Show a busy cursor for the lifetime of this function
    wxBusyCursor busy_cursor;

    wxLogDebug( wxT( "m_automatically_close_when_done=%d" ), (int)m_automatically_close_when_done );
    wxLogDebug( wxT( "close_dialog() code hasn't been run yet, so doing cleanup..." ) );

    // So won't do any more iterations of that look for abort signals.
    m_abort_signal_was_entered = TRUE;

    // Save whether we want to show the details for next time.
    if ( wxConfigBase::Get( FALSE ) ) 
    {
        if ( m_show_hide_listbox_configuration_key != wxEmptyString ) 
        {
            wxConfigBase::Get()->Write( m_show_hide_listbox_configuration_key,
                                      m_collapsiblepane->IsExpanded() );
            wxLogDebug( wxT( "Wrote value to m_show_hide_listbox_configuration_key=%d" ), m_collapsiblepane->IsExpanded() );
        }
    }

    // Do whatever prescribed virtual thing we want done before we finish closing
    action_to_do_before_dialog_finishes_closing();

    // End the modal, with the return value,
    // Note: Destroy doesn't seem to help here for ending in the
    // won't-terminate-app-in-MSW=commandline-progressdialog-autoclose problem.
    EndModal( m_dialog_return_value );

    // This virtual is a spot to put something to run after finishing closing
    // such as the ::PostQuitMessage( 0 ) on MSW to kill the app here (otherwise
    // never get program flow returning back from this dialog's .ShowModal() in
    // the calling .ShowModal in build_progress_wrappers.cpp
    action_to_do_after_dialog_finishes_closing();
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_ENHANCED_PROGRESS_DIALOG
