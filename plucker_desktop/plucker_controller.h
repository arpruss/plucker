//----------------------------------------------------------------------------------------
/*!
    \file       plucker_controller.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes plucker_controller class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: plucker_controller.h,v 1.21 2007/06/30 14:55:43 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _PLUCKER_CONTROLLER_H_
#define _PLUCKER_CONTROLLER_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "plucker_controller.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/datetime.h"               // wxDateTime
#include "palm_installer.h"              
#include "channel_type.h"

//----------------------------------------------------------------------------------------
// Class definition: plucker_wizard_base
//----------------------------------------------------------------------------------------

//! Class to controls API functions that are performed on plucker channels.
/*!
    A modular non-visual class, describing API functions for the actions that can be 
    performed on plucker channels: add, delete, update, etc. 
 */
class plucker_controller
{

public:

    //! Contructor.
    plucker_controller();
    
    //! Destructor.
    ~plucker_controller();

    //! Gets the global object or creates one if none exists.
    static plucker_controller* get();
    
    //! Sets the global object and returns a pointer to previous one (may be NULL).
    /*!
        \param desired_plucker_controller The new plucker_controller
     */
    static plucker_controller* set( plucker_controller* desired_plucker_controller );

    //! Add a channel.
    /*!
        \param channel A channel_type class to be added.
        \return The configuration_section of the created channel.
     */
    wxString        add_channel( channel_type& channel );
    
    //! Delete a channel.
    /*!
        \param channel_section The configuration file section of channel to be deleted.
        \return TRUE if deletion successful.
     */
    bool            delete_channel( const wxString& channel_section );
    
    //! Delete some channels.
    /*!
        \param channel_sections The configuration file sections of channels to be deleted.
        \return TRUE if deletions successful.
     */ 
    bool            delete_channels( wxArrayString& channel_sections );
    
    //! Update a channel
    /*!
        \param channel_section The configuration file section of channel to be updated.
        \param install TRUE if channels should also be installed, after update.
        \return TRUE if update successful.
     */
    bool            update_channel( const wxString& channel_section,
                                    bool install = TRUE
                                  );
    
    //! Update some channels.
    /*!
        \param channel_sections The configuration file sections of channels to be updated.
        \param install TRUE if channels should also be installed, after update.
        \return TRUE if updates successful.
     */     
    bool            update_channels( wxArrayString& channel_sections,
                                     bool install = TRUE
                                   );
    
    //! Update all channels.
    /*!
        \param install TRUE if channels should also be installed, after update.
        \return TRUE if updates successful.
     */    
    bool            update_all_channels( bool install = TRUE );
    
    //! Update due channels.
    /*!
        \param install TRUE if channels should also be installed, after update.
        \return TRUE if updates successful.
     */ 
    bool            update_due_channels( bool install = TRUE );
    
    //! Creates a new subdirectory for a channel.
    /*! Also makes a \e cache subdirectory inside the newly created directory for the
        channels directory.
        \param channel_section The configuration file section of channel which requires
            a new subdirectory. Subdirectory usally made in the \e channels directory,
            and will be the same name as the channel_section .
        \param copy_template_files TRUE means to copy over the home.html or exclusion.html
            and whatever other template files over from the NEW_CHANNEL_TEMPLATE 
            subdirectory over to the new directory.            
        \return TRUE if creation was successful.
     */ 
    bool            create_new_channel_subdirectory( const wxString& channel_section,
                                                     bool copy_template_files = TRUE 
                                                   );
                                                     
    //! Deletes the subdirectory of a channel.
    /*!    
        Deletes the subdirectory called "channel_section" directory from channels directory
        Note: wxWindows docs says RmDir() won't work under VMS. 
        
        A recursive delete on the whole directory won't work, so we just have to
        cd to cache, delete all the cache; then delete cache, then cd up and delete *.*
        from there (home.html and exclusion list), then finally delete the channel subdir.        
        \todo OPTIONAL: could fork whether or not to leave the directory if there is 
        something else in it, like their images, etc.
        \param channel_section The configuration file section of channel which requires
            its subdirectory to be deleted.   
        \return TRUE if deletion was successful.
     */                                                
    bool            delete_channel_subdirectory( const wxString& channel_section );
    
    //! Get a list of all channels.
    /*!
        \param channel_sections The array in which to store the configuration file sections.
     */     
    void            get_all_channels( wxArrayString& channel_sections );
    
    //! Get a list of all due channels.
    /*!
        \param channel_sections The array in which to store the configuration file sections.
     */ 
    void            get_due_channels( wxArrayString& channel_sections );
    
    //! Get an array of channel sections that match the array of doc_names.
    /*!
        Since doc_name is not unique, there can be multiple sections for a doc_name.
        Don't expect the channel sections to thus always have the same index, or number 
        of array entries.
        Used for example in commandline updating by channel name. 
        \param doc_names Array of input doc_names to look up.
        \param channel_sections Array to store the channel_sections.
     */ 
    void            get_channel_sections_by_doc_names( wxArrayString& doc_names,
                                                       wxArrayString& channel_sections
                                                     );
    
    //! Install the channels.
    /*!
        Different approaches for GTK users, since more convenient to 
        assemble a list and do them all at once, so only have to hit HotSync button once
        in pilot-xfer.
        \param channel_sections Channel sections in config file to be installed.
        \return TRUE if installation successful.
     */
    bool            install_channels( wxArrayString& channel_sections );
    
    //! Find out if this channel is currently due.
    /*!
     *  \param channel_section The configuration file section of channel to be examined.
     *  \return TRUE if this channel is due.
     */ 
    bool            is_channel_due( const wxString& channel_section );  
    
    //! Find out if there are currently any due channels.
    /*!
     *  \return TRUE if there is at least one due channel.
     */ 
    bool            is_any_channel_due();
    
    //! Queries whether or not a channel is update behaviour enabled.
    /*!
        \param channel_section The configuration file section of channel to be examined.
        \return TRUE if update successful.
     */
    bool            is_channel_update_enabled( const wxString& channel_section );
    
    //! Get the datetime at which a channel became due or will become due.
    /*!
        Properly accounts for a update_base datetime in the future.
        \param channel_section The configuration file section of channel to be updated.
        \param shall_increment TRUE if value should be an incremented one instead of raw value from ini.
        \return The due datetime.
     */
    wxDateTime      get_channel_due_datetime( const wxString& channel_section, const bool shall_increment = TRUE );

    //! Sets a new update_base key of a channel.
    /*!
        \param channel_section The configuration file section of channel to be examined.
     */    
    void            set_channel_update_base( const wxString& channel_section );
    
    //! Query whether we are allowing or forbidding a new execution of autoupdates.
    /*! 
        Gets the member value of whether we are allowing or forbidding a new 
        execution of updates (because the app hasn't finished its load initization yet, 
        it is already executing an update, or some GUI component like a 
        preferences/channel dialog is displayed) and should hold off until that is 
        done first. 
        \return TRUE if a new update execution is allowed.
     */
    bool            get_is_new_update_execution_allowed();
    
    //! Set whether we are allowing or forbidding a new execution of autoupdates.
    /*! 
        Gets the member value of whether we are allowing or forbidding a new 
        execution of updates (because the app hasn't finished its load initization yet, 
        it is already executing an update, or some GUI component like a 
        preferences/channel dialog is displayed) and should hold off until that is 
        done first. 
        
        Note that in plucker_controller::plucker_controller() it was initialized to 
        FALSE, and at the end of the_app::OnInit() it was set to TRUE.
        \param TRUE if a new update execution is allowed.
     */
    void            set_is_new_update_execution_allowed( bool is_allowed );

    //! Check the 2 criteria needed for an autoupdate event to start:
    /*! 
        There are 2 criteria required for an autoupdate to start:
        <ol>
        <li> Is it allowed, via plucker_controller::set_is_new_update_execution_allowed()
             status?
        <li> Are any channels due?
        </ol>
        \return TRUE if all criteria have been met.
     */
    bool            is_all_criteria_met_to_execute_an_autoupdate(); 
    

                                 
private:
    
    // Singleton instance of plucker_controller
    static plucker_controller* ms_instance;   
    
    //! Copies a file from one channel to the next
    bool            copy_file_between_channels( const wxString& from_section, 
                                                const wxString& to_section,
                                                const wxString& filename,
                                                bool overwrite = TRUE );
    
    //! Executes the command plucker-build
    bool            execute_plucker_build( const wxString& channel_section );
    
    //! Returns the datetime incremented by the period and frequency.
    wxDateTime      get_incremented_channel_due_datetime( const wxString& channel_section,
                                                          int increments=1 );
                                                          
    //! Creates the "/channels" directory in PLUCKERHOME dir, if it doesn't exist already.
    /*!
        \return TRUE if a channels directory needed to be, and was created.
     */
    bool            create_channels_directory();
    
    //! Creates a suitably named plucker directory in the users home directory if not exist.    
    /*!
        \return TRUE if a channels directory needed to be, and was created.
     */
    bool            create_pluckerhome_directory();    
    
    //! Copies the systemwide exclusionist into user's directory if one doesn't exist.    
    /*!
        \return TRUE if the exclusionlist.txt needed to be, and was created.
     */
    bool            create_default_exclusion_list();
    
    //! Creates the channel template section in configuration file, and its subdirectory.     
    void            create_channel_template_section_and_subdirectory();
    
    //! TRUE if allowed to initiate a new update exectution.
    /*! 
        Get and set with get_is_new_update_execution_allowed() and 
        set_is_new_update_execution_allowed() .
     */
    bool            m_is_new_update_execution_allowed;

    //! A palm_installer for installing channel output to handhelds or directories.
    palm_installer* m_palm_installer;
    
};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //PLUCKER_CONTROLLER_H
