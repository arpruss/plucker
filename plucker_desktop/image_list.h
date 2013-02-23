//----------------------------------------------------------------------------------------
/*!
    \file       image_list.h        
    \modified                            
    \copyright  (c) Robert O'Connor    
    \licence    GPL
    \brief      Describes image_list class    
    \author     Robert O'Connor
    \date       2001/10/20    
 */  
// RCS-ID:      $Id: image_list.h,v 1.10 2003/03/17 17:43:20 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _IMAGE_LIST_H_
#define _IMAGE_LIST_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "image_list.h"
#endif

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/imaglist.h"

//----------------------------------------------------------------------------------------
// Class definition: image_list
//----------------------------------------------------------------------------------------

//! Class to manage a shared imagelist for the application controls that need imagelists.
/*! Maintains the shared image list of icons that can be used throughout the 
    program by controls that use imagelists instead of bitmaps (namely, 
    listcontrols and notebook tabs). Since so many icons will appear in multiple places
    just create one wxImageList at program and delete it at the program 
    termination. ListControls just need to have a line that says somthing
    like:
    SetImageList( image_list::get() );
    before inserting items into the rows. Then images can be inserted along
    with items just by calling the index number of the image in the image list.
    The numbers of the image list are given #defines in image_list.h for 
    easier maintenance. 
*/    
class image_list : public wxImageList
{

    // I guess one doesn't use DECLARE_DYNAMIC_CLASS on these.
    
public:

    //! Constructor.
    image_list();

    //! Destructor.
    ~image_list();

    //! Adds a file from the image list directory.
    /*!
        \param basename Basename of the file
     */
    void add_file_from_image_list_directory( const wxString& basename );

    //! Gets the global object or creates one if none exists.
    /*! 
        \param parent Parent of the image_list. May be NULL.
     */
    static image_list* get();
    
    //! Sets the global object and returns a pointer to previous one (may be NULL).
    /*!
        \param desired_image_list The new image_list
     */
    static image_list* set( image_list* desired_image_list );

private:
    
    // Singleton instance:
    static image_list* ms_instance;    
    
};

//----------------------------------------------------------------------------------------
// Image list index number defines
//----------------------------------------------------------------------------------------

// Currently the item order in the small icon list are as follows:
enum {
    plkrSATELLITE_DISH         = 0,
    plkrEDIT_PENCIL,
    plkrPREVIEW_PAGE,
    plkrRADIO_OPTIONS,
    plkrSYNC,
    plkrFOLDER,
    plkrSMALL_IMAGE_LIST_ID_CHECKBOX_CHECKED,
    plkrSMALL_IMAGE_LIST_ID_CHECKBOX_UNCHECKED
};

// Places at a glance where icons are being used/reused
#define SMALL_IMAGE_LIST_ID_MAIN_FRAME_LISTCTRL                                     plkrSATELLITE_DISH
#define SMALL_IMAGE_LIST_ID_SHOWCASE_DIALOG_LISTCTRL                                plkrSATELLITE_DISH
#define SMALL_IMAGE_LIST_ID_EDITOR_DIALOG_EDIT_TAB                                  plkrEDIT_PENCIL
#define SMALL_IMAGE_LIST_ID_EDITOR_DIALOG_PREVIEW_TAB                               plkrPREVIEW_PAGE
#define plkrSMALL_IMAGE_LIST_ID_CHANNEL_DIALOG_DESTINATION_TAB_USERS_LISTCTRL       plkrSYNC
#define plkrSMALL_IMAGE_LIST_ID_CHANNEL_DIALOG_DESTINATION_TAB_DIRECTORIES_LISTCTRL plkrFOLDER

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_IMAGE_LIST_H_
