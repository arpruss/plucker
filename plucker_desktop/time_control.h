//----------------------------------------------------------------------------------------
/*!
    \file       time_control.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )
    \licence    GPL
    \brief      Describes time_control class    
    \author     Robert O'Connor
    \date       2002/09/26
 */
// RCS-ID:      $Id: time_control.h,v 1.5 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _TIME_CONTROL_H_
#define _TIME_CONTROL_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "time_control.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/panel.h"
#include "wx/datetime.h"
#include "wx/spinbutt.h"

//----------------------------------------------------------------------------------------
// Class definition: time_control
//----------------------------------------------------------------------------------------

//! A control for entering a time: has spinners for hours and minutes.
class time_control : public wxPanel
{

public:

    DECLARE_DYNAMIC_CLASS( time_control )

public:

    //! Constructor.
    /*!
        \param parent The parent window.
        \param id The id of the progress_listbox. Will usually be -1 unless multiple
            of them on the same dialog.
        \param pos The pixel position of the listctrl on its parent window
        \param size The pixel size of the listctrl
        \param style Style of the listbox. See wxWindows wxListBox docs for details.
        \param name Windows name (rarely used).
     */
    time_control( wxWindow *parent,
                  wxWindowID id = -1,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  const wxString &name = wxT( "the_time_control" ) );

    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    time_control() {};

    //! Destructor
    ~time_control();

    wxDateTime               get_time();
    wxDateTime::wxDateTime_t get_hour();
    wxDateTime::wxDateTime_t get_minute();

    void        set_time( const wxDateTime& input_time );
    void        set_hour( const wxDateTime::wxDateTime_t input_hour );
    void        set_minute( const wxDateTime::wxDateTime_t input_minute );

    void        enable ( bool do_enable );

private:

    void        on_hour_textctrl( wxCommandEvent& event );
    void        on_hour_spinbutton( wxSpinEvent& event );
    void        on_minute_textctrl( wxCommandEvent& event );
    void        on_minute_spinbutton( wxSpinEvent& event );

    wxTextCtrl*   m_hour_textctrl;
    wxSpinButton* m_hour_spinbutton;
    wxStaticText* m_colon_statictext;
    wxTextCtrl*   m_minute_textctrl;
    wxSpinButton* m_minute_spinbutton;

    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_TIME_CONTROL_H_
