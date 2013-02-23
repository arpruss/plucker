//----------------------------------------------------------------------------------------
/*!
    \file       hhictrl.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes wxHandheldInstallCtrl class    
    \author     Robert O'Connor
    \date       2001/10/27    
 */  
//  RCS-ID:     $Id: hhictrl.h,v 1.11 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _HANDHELDINSTALLCTRL_H_
#define _HANDHELDINSTALLCTRL_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "hhictrl.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/panel.h"
#include "wx/imaglist.h"
#include "wx/button.h"
#include "wx/listctrl.h"
#include "palm_installer.h"
#include "pda_listctrl.h"

//----------------------------------------------------------------------------------------
// Class definition: wxHandheldInstallCtrl
//----------------------------------------------------------------------------------------
   
//! A control to allow install to Palm handheld usernames or directories.
/*!
    There are two listctrls one, one for handheld destinations, one for directory 
    desinations. Buttons to the left of each listctrl allow entries to be added or
    deleted.
    
    The choices for handheld destinations are populated via a palm_installer .
 */
class wxHandheldInstallCtrl : public wxPanel
{

    DECLARE_DYNAMIC_CLASS( wxHandheldInstallCtrl )

public:

    //! Constructor.
    /*!
        \param parent The parent window. May be NULL.
        \param id The id of the control. 
        \param users_message Message that appears at the top of the m_usersListCtrl.
        \param directories_message Message that appears at the top of the
            m_directoriesListCtrl.
        \param pos The pixel position of the listctrl on its parent window
        \param size The pixel size of the listctrl
        \param name Windows name (rarely used).
     */
    wxHandheldInstallCtrl( wxWindow *parent, 
                           wxWindowID id,
                           const wxString& users_message = _( "Install files to these handhelds" ),
                           const wxString& directories_message = _( "Install files to these directories" ), 
                           const wxPoint& pos = wxDefaultPosition,                      
                           const wxSize& size = wxDefaultSize,
                           const wxString& name = wxT( "handheldInstallCtrl" )
                         );
    
    // Unused constructor: solely for usage of RTTI (DYNAMIC_CLASS) macros.
    wxHandheldInstallCtrl() {};
    
    //! Destructor.                     
    ~wxHandheldInstallCtrl();
    
    //! Optionally, set a wxImageList to be used by the 2 listctrls in the object.
    /*! 
        This will make icons appear in the listctrls, use the SetUsersImageListImageId()
        SetDirectoriesImageListImageId() to get the ones that you want.
        param imageList The wxImageList object to use
     */
    void            SetImageList( wxImageList* imageList );

    //! Set a specific icon index in the image list to use for the m_usersListCtrl.
    void            SetUsersImageListImageId( int imageId );

    //! Load the m_usersListCtrl with users.
    /*!
        \param handheld_dest_array An array of handheld user destinations to be loaded.
     */
    void            InsertUserDestinations( handheld_dest_array_type& handheld_dest_array );

    //! Get the array of handheld user destinations from the m_usersListCtrl.
    /*!
        \param handheld_dest_array An array to hold the handheld user destinations.
     */
    void            GetUserDestinations( handheld_dest_array_type& handheld_dest_array );

    //! Set a specific icon index in the image list to use for the m_usersListCtrl.
    void            SetDirectoriesImageListImageId( int imageId );    
    
    //! Load the m_directoriesListCtrl with users.
    /*!
        \param directories An array of directory destinations to be loaded.
     */    
    void            InsertDirectoryDestinations( wxArrayString& directories );
    
    //! Get the array of directory destinations from the m_directoriesListCtrl.
    /*!
        \param directoryDestinations An array to hold the directory destinations.
     */
    void            GetDirectoryDestinations( wxArrayString& directoryDestinations );
    
    //! Install a list of files to the user and directory destinations.
    /*!
        Uses a palm_installer for its work.
        \return TRUE if there was no problems with the installation.
     */    
    bool            InstallFilesToDestinations( wxArrayString& install_fullnames );
    
    //! Check to see if there are no destinations in either control.
    /*!
        This is a convenience function to do quickly check that there are no 
        destinations, and do some behaviour to say, remind the user to add one.
        For example, see destinations_wizardpage::on_wizardpage_changing() .
        \return TRUE if neither the m_usersListCtrl and m_directoriesListCtrl
             have an entry.
     */
    bool            IsAllDestinationsEmpty();
        
protected:

    void            OnRemoveButtons( wxCommandEvent& event );

    void            OnUpdateUiUsersListCtrl( wxUpdateUIEvent& event );
    void            OnUsersAddButton( wxCommandEvent& event );    
    
    void            OnUpdateUiDirectoriesListCtrl( wxUpdateUIEvent& event );
    void            OnDirectoriesAddButton( wxCommandEvent& event );
    
    palm_installer* m_palmInstaller;
    
    pda_listctrl*   m_usersListCtrl;
    int             m_usersListCtrlImageId;
    wxButton*       m_usersAddButton;
    wxButton*       m_usersRemoveButton;   
    
    wxListCtrl*     m_directoriesListCtrl;
    int             m_directoriesListCtrlImageId;
    wxButton*       m_directoriesAddButton;
    wxButton*       m_directoriesRemoveButton;      

    DECLARE_EVENT_TABLE()
    
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_HANDHELDINSTALLCTRL_H_

