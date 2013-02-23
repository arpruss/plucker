//----------------------------------------------------------------------------------------
/*!
    \file       utils_controls.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Describes utils_controls class    
    \author     Robert O'Connor
    \date       2002/01/03    
 */  
// RCS-ID:      $Id: utils_controls.h,v 1.17 2007/06/29 00:58:57 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _UTILS_CONTROLS_H_
#define _UTILS_CONTROLS_H_

//----------------------------------------------------------------------------------------
// GCC interface
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "utils_controls.h"
#endif

//----------------------------------------------------------------------------------------
// Shared defines
//----------------------------------------------------------------------------------------

#include "plucker_defines.h"

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/splitter.h"            // wxSplitterWindow
#include "wx/html/htmlwin.h"        // wxHtmlWindow
#include "wx/settings.h"            // Enum of system colors
#include "wx/menu.h"				// wxMenu, wxMenuBar, wxMenuItem

//----------------------------------------------------------------------------------------
// Class option flags
//----------------------------------------------------------------------------------------

//! Option flags for set_html_window_text_in_gui_style()
enum
{
    //! Set the background of the HTML window to the color of the GUI's dialogs.    
    optionSET_HTML_BACKGROUND_TO_MATCH_GUI_PANEL = 1,
    //! Set the background of the HTML window to the color of the GUI's listbox.    
    optionSET_HTML_BACKGROUND_TO_MATCH_GUI_LISTBOX = 2,
    //! Set the font color of the HTML window to match the GUI's font.
    optionSET_HTML_FONT_COLOR_TO_MATCH_GUI = 4,
    //! Set the font face and pointsize of the HTML window to match the GUI's font.
    optionSET_HTML_FONT_FACE_AND_POINTSIZE_TO_MATCH_GUI = 8,
    //! Set the HTML windows padding from its edges to its content to be zero.
    optionSET_HTML_WINDOW_INTERNAL_PADDING_TO_ZERO = 16
};

//----------------------------------------------------------------------------------------
// Class definition: utils_controls (static only)
//----------------------------------------------------------------------------------------

//! A static-only class for commonly used functions of wxWindows controls.
class utils_controls 
{
public:

    //! Fixes a wxSplitterWindow that has its sash position out of range.
    /*!
       \param splitterwindow The splitterwindow to check to see if out of range. 
       \return TRUE if the splitterwindow needed a fix.
     */
    static bool splitterwindow_fix_sash_out_of_range( wxSplitterWindow* splitterwindow );    
    
    //! Show a color selector dialog and put the hex color value string in a textctrl.
    /*! 
        When a "Choose color..." button beside a hex color textctrl is tapped, show
        a color dialog, initialize to textctrl's previous value if possible, get the
        color, convert to hex, put it back in the list ctrl.
       \param target_color_textctrl Pointer to the textctrl to place the hex string
       \return FALSE if there was a problem.     
     */
    static bool textctrl_hex_color_selection ( wxTextCtrl* target_color_textctrl );    
    
    //! Show a file selector dialog and put the filename(s) into a textctrl.
    /*! 
       When a "Choose file..." button beside a filename textctrl is clicked, show
       a filename dialog, initialize to textctrl's previous value if possible, get the
       filename, put it back in the textctrl. Return false if there was a problem.
       \param target_filename_textctrl Pointer to the textctrl
       \param message The prompt message shown in the dialog.
       \param default_file The default filename to select (can be empty string).
       \param default_directory The default directory to open. If Empty will be 
            the current working directory.
       \param wildcard The wildcard filterpattern to use. One of the defines.
       \param allow_multiple TRUE means they can select more than one file.
       \param multiple_selection_delimiter If they select mulitple files, the 
            delimiter to place between each filename when the files are placed in 
            the textctrl.
       \param uses_file_protocal TRUE if the textctrl needs a file:// prefixed at 
            the start of the filename. Example: channel_dialog Starting Page tab.
        \return FALSE if there was a problem.
     */
    static bool textctrl_filename_selection ( wxTextCtrl* target_filename_textctrl,
                                              wxString message,                           
                                              wxString default_file,
                                              wxString default_directory, 
                                              wxString wildcard,
                                              bool allow_multiple, 
                                              wxChar multiple_selection_delimiter,
                                              bool uses_file_protocol 
                                            );
    
    //! Show a directory selector dialog and put the directory(ies) into a textctrl.                        
    /*! When a "Choose directory..." button beside a filename textctrl is clicked, show
        a directory dialog, initialize to textctrl's previous value if possible, get the
        directory, and put it back in the textctrl. 
       \param target_filename_textctrl Pointer to the textctrl
       \param message The prompt message shown in the dialog.
       \param default_directory The default directory to open. If Empty will be 
            the current working directory.
       \param allow_multiple TRUE means they can select more than one file.
       \param multiple_selection_delimiter If they select mulitple directories, the 
            delimiter to place between each directory when the directories are placed 
            in the textctrl.
       \param uses_file_protocal TRUE if the textctrl needs a file:// prefixed at 
            the start of the filename. Example: channel_dialog Starting Page tab.
       \return FALSE if there was a problem.
     */
    static bool textctrl_directory_selection( wxTextCtrl* target_filename_textctrl,        
                                               wxString message,
                                               wxString default_directory,
                                               bool allow_multiple,
                                               wxChar multiple_selection_delimiter
                                             );

    //! Sets a target HTML window to certain HTML content, in the style of part of a GUI's paenl.    
    /*!
        The html window will initially be set up to look like part of a panel: background
        will match the color of a panel, and fonts will be the same size as a panel. 
        The HTML source however can include a <font face=""> or a <body bgcolor=""> to 
        override this if so desired.
        \param target_html_window Pointer to the html window.
        \param source A string of HTML source code that should be put into the htmlwindow.
            This is not a filename, it is a string. See wxHtmlWindow::SetPage() in wx docs
            for more information. 
        \param Bitlist options of what things should be matched to the GUI, if you only 
            want to set just the fonts face/pointsize, font color or the background color.
     */
    static void set_html_window_text_in_gui_style( wxHtmlWindow* target_html_window,        
                                   const wxString& source = wxEmptyString,
                                   long options = optionSET_HTML_BACKGROUND_TO_MATCH_GUI_PANEL        |
                                                  optionSET_HTML_FONT_COLOR_TO_MATCH_GUI              |
                                                  optionSET_HTML_FONT_FACE_AND_POINTSIZE_TO_MATCH_GUI |
                                                  optionSET_HTML_WINDOW_INTERNAL_PADDING_TO_ZERO 
                                   );
    
    //! Sets a time choice to the time specified in a configuration key string
    /*!
        \param target_choice Pointer to the choice.
        \param time_configuration_key Key in configuration file holding the time.
        \param default_time_string Default time to fill into the textctrl.
     */
    static void set_choice_to_configuration_time_string( wxChoice* target_choice, 
                                    const wxString& time_configuration_key,
                                    const wxString& default_time_string = _( "12:00:00" )
                                    );
    
    //! Sets a config key to the time of a choice control
    /*!        
        \param time_configuration_key Key in configuration file holding the time.
        \param target_choice Pointer to the choice.
     */
    static void set_configuration_time_string_to_choice_index( const wxString& time_configuration_key,
                                    wxChoice* input_choice 
                                    );
    
    //! Reads a dialog position and size, 
    /*! 
        \param target_window The window whose size should be saved.
        \param configuration_key_basename The basename of the key in the config. 
            For example 'channel_dialog' (which will use keys like 
            'channel_dialog_position_x', etc.
        \param default_position Default position, if none in configuration file.
        \param default_size Default size, if none in cofiguration file.     
     */                                 
    static void read_dialog_position_and_size( wxWindow* target_window, 
                                    const wxString& configuration_key_basename,
                                    wxPoint default_position = wxDefaultPosition, 
                                    wxSize default_size = wxDefaultSize 
                                    );  
    
    //! Writes a dialog position and size to the configuraiton.
    /*! 
        \param target_window The window whose size should be saved.
        \param configuration_key_basename The basename of the key in the config. 
            For example 'channel_dialog' (which will use keys like 
            'channel_dialog_position_x', etc.
     */
    static void write_dialog_position_and_size( wxWindow* target_window, 
                                     const wxString& configuration_key_basename );
    
    //! Removes excess menu separators from a menubar.
    /*! 
        In the menus of the menubar, it is possible to end up with separators at
        the top of a menu, the bottom of a menu, or >1 separators in a row.
        This can occur as a result of the code which moves the About, Quit and
        Preferences menuitems into their special home on a Mac. It can also occur
        from us deleting menuitems of features that have been compiled out of 
        the Plucker Desktop application.
        \param target_menubar. The menubar that needs to be cleaned.
    */                               
    static void strip_excess_separators_from_menubar( wxMenuBar* target_menubar );    

                                             
};
                                   
// Defines to go with filename_selection wildcards when showing a file selection dialog
/*!
    \todo The FILE_DIALOG defines should be moved from utils_controls.h 
        to plucker_defines.h
 */
#define FILE_DIALOG_WILDCARDS_ALL _( "All files  (*)|*" )
#define FILE_DIALOG_WILDCARDS_IMAGES _( "Images (*.png, *.gif, *.jpg)|*.png;*.gif;*.jpg;*.jpeg|PNG files (*.png)|*.png|GIF files (*.gif)|*.gif|JPG files (*.jpg,*jpeg)|*.jpg;*.jpeg|All files  (*.*)|*.*" )
#define FILE_DIALOG_WILDCARDS_HANDHELD_LAUNCHER_ICONS _( "Images (*.bmp, *.png, *.gif)|*.bmp;*.png;*.gif|Bitmaps (*.bmp)|*.bmp|PNG files (*.png)|*.png|GIF files (*.gif)|*.gif" )

#ifdef __WXMSW__
    #define FILE_DIALOG_WILDCARDS_EXECUTABLES _( "Programs (*.exe)|*.exe" )
#else    
    #define FILE_DIALOG_WILDCARDS_EXECUTABLES _( "Programs |*" )
#endif

#define FILE_DIALOG_WILDCARDS_PLUCKABLES _( "All files |*|HTML, XML, Text files (*.html, *.htm, *.xml, *.txt)|*.html;*.htm;*.xml'*.txt|Images (*.png, *.gif, *.jpg)|*.png;*.gif;*.jpg;*.jpeg" )

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------
    
#endif  //_UTILS_CONTROLS_H_
