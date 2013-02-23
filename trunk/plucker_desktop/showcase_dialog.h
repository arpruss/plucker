//----------------------------------------------------------------------------------------
/*!
    \file       showcase_dialog.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes showcase_dialog class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: showcase_dialog.h,v 1.20 2007/06/29 00:58:56 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _SHOWCASE_DIALOG_H_
#define _SHOWCASE_DIALOG_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "showcase_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_SHOWCASE )

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/dialog.h"
#include "wx/listctrl.h"
#include "wx/splitter.h"
#include "wx/html/htmlwin.h"

#include "showcase_listctrl.h"
// Chicken and egg
class showcase_splitterwindow;
#include "showcase_splitterwindow.h"

//----------------------------------------------------------------------------------------
// Class definition: showcase_dialog
//----------------------------------------------------------------------------------------

//! A showcase dialog to list some available plucker channels.
/*!
    Composed of a showcase_splitterwindow that has a showcase_listctrl in one pane.
    Details of the selected item appear in another panes wxHtmlWindow.
 */
class showcase_dialog : public wxDialog
{

public: 

    //! Constructor.
    /*!
        \param parent The parent window.
     */
    showcase_dialog( wxWindow* parent );
    
    //! Destructor.
    ~showcase_dialog();
    
    //! Transfer a line of channel data to the dialog that called up the showcase dialog.
    /*!
        See channel_type for a description of the parameters.
     */
    void transfer_to(   wxString&   name,
                        wxString&   url,
                        wxString&   description,
                        wxString&   language,
                        wxString&   category,
                        long&       update_frequency,
                        wxString&   update_period,
                        wxDateTime& update_base,
                        bool&       update_enabled,
                        long&       bpp,
                        long&       maxdepth,
                        bool&       stayonhost,
                        bool&       stayondomain,
                        wxString&   url_pattern 
                    );  
    
    //! Action to do when child spitter's sash postion changed.
    /*!   
        Event is sent upwards so the dialog can handle it.
        \param sash_position The new sash position.
     */
    void on_splitter_sash_position_changed( int sash_position );   
    
private:    

    void read_configuration();
    void write_configuration();
    
    void listctrl_init();
    void listctrl_stretch_columns_to_fit();
    void init_splitters();
    void load_details_htmlwindow_content( long item_id );
    
    void on_listctrl_item_selected( wxListEvent& event ); 
    void on_listctrl_item_activated( wxListEvent& event ); 
    void on_update_ui_preview_checkbox( wxUpdateUIEvent& event );
    void on_update_ui_details_checkbox( wxUpdateUIEvent& event );
    // Override base class functions
    void OnOK( wxCommandEvent& event );
    void OnSize( wxSizeEvent& event );
    void on_help_button( wxCommandEvent& event );
    
    // An array of the showcase field names.
    wxArrayString               m_showcase_field_names;
        
    // Controls not in XML resources. The <unknown> class in XML resource is the
    // placeholder container that is used by the main_splitterwindow, and the others
    // being children of main_splitterwindow.
    showcase_listctrl*          m_showcase_listctrl;
    showcase_splitterwindow*    m_showcase_splitterwindow;

    wxHtmlWindow*               details_htmlwindow;
    wxHtmlWindow*               dummy_htmlwindow;   // A dummy htmlwindow to parse showcase file 

    DECLARE_EVENT_TABLE()    
};
                                        
//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif  // setupUSE_SHOWCASE

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_SHOWCASE_DIALOG_H_
