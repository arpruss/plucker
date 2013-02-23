//----------------------------------------------------------------------------------------
// Name:        exclusion_dialog.cpp
// Purpose:     Exclusion lists dialog
// Author:      Robert O'Connor
// Modified by:
// Created:     2001/10/20
// Copyright:   (c) Robert O'Connor ( www.oconnor.md )
// Licence:     GPL
// RCS-ID:      $Id: exclusion_dialog.cpp,v 1.32 2007/06/29 00:58:52 robertoconnor Exp $
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// GCC implementation
//----------------------------------------------------------------------------------------

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "exclusion_dialog.h"
#endif

//----------------------------------------------------------------------------------------
// Setup information
//----------------------------------------------------------------------------------------

#include "setup.h"

//----------------------------------------------------------------------------------------
// Begin feature removal condition
//----------------------------------------------------------------------------------------

#if ( setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR )

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

#include "exclusion_dialog.h"

//----------------------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//----------------------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"              // XRC XML resouces

//----------------------------------------------------------------------------------------

#include "blocked_dialog.h"
#include "configuration.h"
#include "utils_controls.h"
#include "help_controller.h"

//----------------------------------------------------------------------------------------
// Internal constants
//----------------------------------------------------------------------------------------

// Notebook pages indexes for context help and which listctrl to add/update.
enum {
    ID_EXCLUSION_EXTENSIONS_LISTCTRL = wxID_HIGHEST + 100,
    ID_EXCLUSION_URLS_LISTCTRL    
};

//----------------------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(exclusion_dialog, wxDialog)
    EVT_BUTTON( XRCID( "exclusion_dialog_extensions_tab_add_button" ), exclusion_dialog::on_extensions_tab_add_button )
    EVT_BUTTON( XRCID( "exclusion_dialog_extensions_tab_edit_button" ), exclusion_dialog::on_extensions_tab_edit_button )
    EVT_BUTTON( XRCID( "exclusion_dialog_extensions_tab_delete_button" ), exclusion_dialog::on_extensions_tab_delete_button )

    EVT_BUTTON( XRCID( "exclusion_dialog_urls_tab_add_button" ), exclusion_dialog::on_urls_tab_add_button )
    EVT_BUTTON( XRCID( "exclusion_dialog_urls_tab_edit_button" ), exclusion_dialog::on_urls_tab_edit_button )
    EVT_BUTTON( XRCID( "exclusion_dialog_urls_tab_delete_button" ), exclusion_dialog::on_urls_tab_delete_button )
                                                                
// For some reason BCC won't ever catch the event (on builds with everything switched on in setup.h)!
#if ( setupUSE_PARENT_WINDOW_CATCH_UPDATE_UI_EVENT_FROM_DERIVED_LISTCTRL_HACK )
    EVT_UPDATE_UI( -1, exclusion_dialog::on_update_ui_all_listctrls )
#else
    EVT_UPDATE_UI( ID_EXCLUSION_EXTENSIONS_LISTCTRL, exclusion_dialog::on_update_ui_extensions_tab_listctrl )
    EVT_UPDATE_UI( ID_EXCLUSION_URLS_LISTCTRL, exclusion_dialog::on_update_ui_urls_tab_listctrl )
#endif

    EVT_BUTTON( wxID_OK, exclusion_dialog::OnOK )
    EVT_BUTTON( wxID_HELP_CONTEXT, exclusion_dialog::on_help_button )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
// Public members
//----------------------------------------------------------------------------------------

// Constructor, including setting the dialog's m_configuration_section member to the
// incoming configuration_section string.
exclusion_dialog::exclusion_dialog( wxWindow* parent, wxString configuration_section )
                        : m_configuration_section( configuration_section )
{
	wxXmlResource::Get()->LoadDialog( this, parent, wxT( "exclusion_dialog") );
    
    // If configuration_section is DEFAULT, then load the shared exclusion list,
    // otherwise load the one from the channel (or NEW_CHANNEL_TEMPLATE, which is a channel).
    if ( m_configuration_section == wxT( "DEFAULT") ) 
    {
        m_exclusion_filename = get_plucker_directory( PLUCKERHOME ) + wxT( "/" ) 
                               + wxT( "exclusionlist.txt");
    } 
    else
    {
        m_exclusion_filename = get_plucker_directory( CHANNELS ) + wxT( "/" ) 
                               + m_configuration_section + wxT( "/" ) + wxT( "exclusionlist.txt");
    }
    wxLogDebug( wxT( "Name of exclusionlist filename is") + m_exclusion_filename );
    
    m_extensions_exclusion_listctrl = new exclusion_listctrl( this, 
                                        ID_EXCLUSION_EXTENSIONS_LISTCTRL, 
                                        wxDefaultPosition, wxDefaultSize,
                                        wxLC_REPORT, wxDefaultValidator,
                                        wxT( "extensions_exclusion_listctrl"),
                                        _( "File Extension" ) );
                                                            
    wxXmlResource::Get()->AttachUnknownControl( wxT( "exclusion_dialog_extensions_tab_exclusion_listctrl"),
                                           m_extensions_exclusion_listctrl );
   
    m_urls_exclusion_listctrl = new exclusion_listctrl( this,
                                        ID_EXCLUSION_URLS_LISTCTRL, 
                                        wxDefaultPosition, wxDefaultSize,
                                        wxLC_REPORT, wxDefaultValidator,
                                        wxT( "urls_exclusion_listctrl"),
                                        _( "URL" ) );
                                                            
    wxXmlResource::Get()->AttachUnknownControl( wxT( "exclusion_dialog_urls_tab_exclusion_listctrl" ),
                                           m_urls_exclusion_listctrl );
                                                                                                                                 
    wxLogDebug( wxT( "m_extensions_exclusion_listctrl successfully attached as unknown control" ) );     
    
    
    // Set up the listctrls
    listctrl_init(); 
    
#if ( setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS )	
	wxLogDebug( wxT( "Running a setupUSE_FORCE_DIALOG_RELAYOUT_IF_DEFAULT_SIZE_AND_HAS_ATTACHED_UNKNOWN_XRC_CONTROLS" ) );
	
	wxSize	default_size = GetSize();
	wxSize  new_size;
	new_size.x     = default_size.x + 1;
    new_size.y     = default_size.y + 150;
	SetSize( new_size );
#endif

    // Set the size, position of the frame using our common function for this dialog
    // [utils_controls.cpp]
    utils_controls::read_dialog_position_and_size( this, wxT( "exclusion_dialog" ) );
}


exclusion_dialog::~exclusion_dialog()
{
    // Save the size and position of the dialog for next time
    // [utils_controls.cpp]
    utils_controls::write_dialog_position_and_size( this, wxT( "exclusion_dialog" ) );
}

//----------------------------------------------------------------------------------------
// Protected members: non-event handlers
//----------------------------------------------------------------------------------------

void exclusion_dialog::listctrl_init()
{ 
    // To speed up inserting we hide the control temporarily
    m_extensions_exclusion_listctrl->Hide();
    m_urls_exclusion_listctrl->Hide();

    // Load the exclusions into the rows of the listctrl
    listctrl_load_rows();
    wxLogDebug( wxT( "Finished loading list control rows"));
    
    // Finished inserting. Show the control
    m_extensions_exclusion_listctrl->Show();
    m_urls_exclusion_listctrl->Show();
}


void exclusion_dialog::listctrl_load_rows()
{    
    wxString string_to_parse;        // The line of text from TextFile to parse
    wxTextFile textfile;             // The textfile to open
    bool successful = false;         // Whether opening the file was successful.
    bool textfile_is_empty = false;  // Whether textfile is empty.
    
    // Check to see if file exists before try to open this file (saves a warning dialog)
    if ( wxFileExists( m_exclusion_filename ) ) 
    {
        successful = textfile.Open( m_exclusion_filename );
        if ( successful ) 
        {
            // Now check to make sure that there is at least some characters in it, else
            // Loading rows will crash plucker desktop.
            if ( (int)textfile.GetLineCount() == 0 ) 
            {
                textfile_is_empty = true;
            }
        }
    } 
    else
    { 
        successful = textfile.Create( m_exclusion_filename );
        if ( successful ) 
        {            
            textfile_is_empty = true;
        }
    }
    // If an empty text file, then need to add some default entries.
    // Will cause an error if try to read the strings from an empty textfile.
    // Adding two demonstration strings (an extension and url) into the file.
    if ( textfile_is_empty ) 
    {
        textfile.AddLine( wxT( "0:-:.*\\.mp3$" ) );
        textfile.AddLine( wxT( "0:-:http://.*ad\\.doubleclick\\.net/.*" ) );
        // Write is like wxConfigBase::Flush(). It saves it to file.
        textfile.Write();
    }
    wxLogDebug( wxT( "Ability to open or create file is %d"), successful );
    
    // Can't open or create the file, must be a bad filename or something else wrong with
    // it. TODO: Add some error handling for that.

    // Loop through all the lines of the text file. Note that this loop only does something
    // with all but the last lines. String gotten from last line has to be dealt with after
    // loop is over. 
    for ( string_to_parse = textfile.GetFirstLine();
          ! textfile.Eof();
          string_to_parse = textfile.GetNextLine() ) 
    {
        // Parse this string that was read from the textfile. 
        listctrl_parse_single_loaded_row( string_to_parse );
    }
    // Now do the string that was read in the last line
    listctrl_parse_single_loaded_row( string_to_parse );
    // Closes the file and frees the memory.
    textfile.Close();
}


void exclusion_dialog::listctrl_parse_single_loaded_row( wxString string_to_parse ) 
{
    long extensions_row_number = 0;  // Row number of extensions listctrl to insert the info
    long urls_row_number = 0;        // Row number of urls listctrl to insert the info

    wxString exclusion;              // The string_to_parse to exlude
    wxString action;                 // The action of excluded item (+ or -)
    wxString priority;               // The priority of the excluded item

    // Ignore lines that are empty or start with a '#' as they are comments
    if ( ! ( string_to_parse.StartsWith( wxT( "#"), NULL ) || ( string_to_parse == wxT( "" ) ) ) ) 
    {        
        // Entries are of the format
        // 0:-:.*\.mp3$
        // Priority : Include(+) or Exclude(-) : Item to exclude (can contain a ':')
        priority = string_to_parse.BeforeFirst(':');
        // Since exclusion can contain a ':' (for example http:), can use AfterLast
        // yet. Therefore, first reduce the string_to_parse to what is after the first ':'
        string_to_parse = string_to_parse.AfterFirst(':');
        // Now the first ':' of remaining string safely divides the action and exclusion
        action = string_to_parse.BeforeFirst(':');        
        exclusion = string_to_parse.AfterFirst(':');   
        // More descriptive to have "Include" / "Exclude" that just a +/-, so convert them
        if ( action == '-') 
        {
            action = _( "Exclude" );
        } 
        else
        {
            action = _( "Include" );
        }
        
        // If there is no "http" in the string, treat it as a file extension.
        // and stick it into the file extension listctrl. TODO-OPTIONAL: If people are putting
        // non-http URLS in there, then need to have a commented section divider in the 
        // file, like #--------BEGIN URLS----------.
        if ( ! exclusion.Contains( wxT( "http") ) ) 
        {
            m_extensions_exclusion_listctrl->InsertItem( extensions_row_number, exclusion, 0 );
            m_extensions_exclusion_listctrl->SetItem( extensions_row_number, ACTION_COLUMN, action );
            m_extensions_exclusion_listctrl->SetItem( extensions_row_number, PRIORITY_COLUMN, priority );              
            extensions_row_number++; 
        // Else put in in the url tab.
        } 
        else 
        {
            m_urls_exclusion_listctrl->InsertItem( urls_row_number, exclusion, 0 );
            m_urls_exclusion_listctrl->SetItem( urls_row_number, ACTION_COLUMN, action );
            m_urls_exclusion_listctrl->SetItem( urls_row_number, PRIORITY_COLUMN, priority );
            urls_row_number++;
        }        
    }
}


void exclusion_dialog::listctrl_save_rows( exclusion_listctrl* listctrl )
{
    wxListItem     info;             // To look up the info for the item.
    long           item = -1;        // Item numbers of listctrl grid cells. Getting next
                                     // from row '-1' allows inclusion of first selected row.
    wxString       exclusion_string; // String extracted from exclusion column
    wxString       action_string;    // String extracted from action column
    wxString       priority_string;  // String extracted from priority column
    wxString       final_string;     // The final output string to write to the textfile.
                                    
    wxLogDebug( wxT( "Entering save rows function") );   
    
    // Recommended loop for listctrl.                                 
    for ( ;; ) 
    {
        item = listctrl->GetNextItem( item, wxLIST_NEXT_ALL );
        
        wxLogDebug( wxT( "Value of item is %ld"), item );  
        
        // No more rows in the item, so break loop.
        if ( item == -1 ) 
        {
            break; 
        }

        // Look up the properties of wxListItem--first thing is to set the id number
        // (m_itemId) to our current item so we know what item we are talking about
        // in the listctrl grid.
        info.m_itemId = item;
        // Set text mask (don't know why that is here, but seems to be always used).
        info.m_mask = wxLIST_MASK_TEXT;

        // Set the column of the cell to look for information, to the exclsuion column
        info.m_col = EXCLUSION_COLUMN;
        // Get the properties of that cell
        listctrl->GetItem( info );
        exclusion_string = info.m_text;
        // Abort this interation of the loop if this exclusion is blank
        if ( exclusion_string == wxT( "") ) 
        {
            continue;
        }
        
        // Same strategy for the action column
        info.m_col = ACTION_COLUMN;
        listctrl->GetItem( info );        
        if ( info.m_text == _( "Include" ) )
        {
            action_string = wxT( "+");
        } 
        else 
        {
            action_string = wxT( "-");
        }

        // And same strategy for the priority column
        info.m_col = PRIORITY_COLUMN;
        listctrl->GetItem( info );
        priority_string = info.m_text;
        
        // Assemble the bits into the final string into the ':' format used
        // as separators in the exclusion list.
        final_string = priority_string + wxT( ":" ) + action_string + wxT( ":" ) + exclusion_string;
        wxLogDebug( wxT( "Final_string to append to exclusion file: %s"), final_string.c_str() );  
        // Add the final string as a line in the file.
        m_textfile.AddLine( final_string );        
     }
}


void exclusion_dialog::save_exclusion_file()
{
    bool successful = wxRemoveFile( m_exclusion_filename );
    wxLogDebug( wxT( "Ability to delete file is %d"), successful );    
    
    successful = m_textfile.Create( m_exclusion_filename );
    wxLogDebug( wxT( "Ability to create file is %d"), successful );  
    
    m_textfile.AddLine( wxT( "##") );    
    m_textfile.AddLine( wxT( "## Leading and trailing white space is ignored.") );
    m_textfile.AddLine( wxT( "## emty lines or lines starting with a \'#\' are considered comments") );
    m_textfile.AddLine( wxT( "## everything else should be of the form:") );
    m_textfile.AddLine( wxT( "##  <prio>:<action>:<regexp>") );
    m_textfile.AddLine( wxT( "## where:") );
    m_textfile.AddLine( wxT( "##  <prio>: is an integer (negative numbers are also valid) specifing") );
    m_textfile.AddLine( wxT( "##          the priority.  Rules with higher priorities are considered") );
    m_textfile.AddLine( wxT( "##          before rules with lower priorities.  Rules of equal priority") );
    m_textfile.AddLine( wxT( "##          are considered in the sequence that they appear in the file.") );
    m_textfile.AddLine( wxT( "##  <action>: is either a plus or a minus sign.  Plus means \'include this") );
    m_textfile.AddLine( wxT( "##            document\' while minus means \'do not include this document\'.") );
    m_textfile.AddLine( wxT( "##  <regexp>: a valid regular expression (as known, e.g. from perl).") );
    m_textfile.AddLine( wxT( "##") );
    m_textfile.AddLine( wxT( "") );
    m_textfile.AddLine( wxT( "# all rules in this file have priority 0, so you can use easily") );
    m_textfile.AddLine( wxT( "# override things by using higher priorities.") );
    m_textfile.AddLine( wxT( "") );
    m_textfile.AddLine( wxT( "## ----------------------------------------------------------------------") );
    m_textfile.AddLine( wxT( "## Ignore files that we know we cannot handle:") );
    m_textfile.AddLine( wxT( "##") );    
    m_textfile.AddLine( wxT( "") );    
    
    // Write the blocked extensions
    listctrl_save_rows( m_extensions_exclusion_listctrl );

    m_textfile.AddLine( wxT( "") );   
    m_textfile.AddLine( wxT( "## ----------------------------------------------------------------------") );   
    m_textfile.AddLine( wxT( "## Ignore known advertisement sites:") );
    m_textfile.AddLine( wxT( "##") );
    m_textfile.AddLine( wxT( "") );   
    
    // Write the blocked URLs
    listctrl_save_rows( m_urls_exclusion_listctrl );
    
    m_textfile.AddLine( wxT( "## ----------------------------------------------------------------------") );   
   
    // Write the changes. NOTE: Similar to the way the_configuration->Write() works, 
    // m_textfile.AddLine(...) doesn't actually add the lines to the file on disk,
    // until there is a specific flushing called m_textfile.Write(), which flushes
    // all the AddLines to the disk.
    m_textfile.Write();
    
    // Closes the file and frees the memory.
    m_textfile.Close();
}

//----------------------------------------------------------------------------------------
// Protected members: event handlers
//----------------------------------------------------------------------------------------

void exclusion_dialog::on_extensions_tab_add_button( wxCommandEvent &event )
{
    m_extensions_exclusion_listctrl->on_add_command(event);
}


void exclusion_dialog::on_extensions_tab_edit_button( wxCommandEvent &event )
{
     m_extensions_exclusion_listctrl->on_edit_command(event);
}


void exclusion_dialog::on_extensions_tab_delete_button( wxCommandEvent &event )
{
    m_extensions_exclusion_listctrl->on_delete_command(event);
}


void exclusion_dialog::on_urls_tab_add_button( wxCommandEvent &event )
{
    m_urls_exclusion_listctrl->on_add_command(event);
}


void exclusion_dialog::on_urls_tab_edit_button( wxCommandEvent &event )
{
    m_urls_exclusion_listctrl->on_edit_command(event);
}


void exclusion_dialog::on_urls_tab_delete_button( wxCommandEvent &event )
{
    m_urls_exclusion_listctrl->on_delete_command(event);
}


// Update the enabled/disabled state of the edit/delete buttons depending on 
// whether a row (item) is selected in the listctrl
void exclusion_dialog::on_update_ui_extensions_tab_listctrl( wxUpdateUIEvent &event )
{
    bool enabled = (bool)m_extensions_exclusion_listctrl->GetSelectedItemCount();

    XRCCTRL( *this, "exclusion_dialog_extensions_tab_edit_button", wxButton )
        ->Enable( enabled );

    XRCCTRL( *this, "exclusion_dialog_extensions_tab_delete_button", wxButton )
        ->Enable( enabled );
}


// Update the enabled/disabled state of the edit/delete buttons depending on
// whether a row (item) is selected in the listctrl
void exclusion_dialog::on_update_ui_urls_tab_listctrl( wxUpdateUIEvent &event )
{
    bool enabled = (bool)m_urls_exclusion_listctrl->GetSelectedItemCount();

    XRCCTRL( *this, "exclusion_dialog_urls_tab_edit_button", wxButton )
        ->Enable( enabled );

    XRCCTRL( *this, "exclusion_dialog_urls_tab_delete_button", wxButton )
        ->Enable( enabled );    
}


#if ( setupUSE_PARENT_WINDOW_CATCH_UPDATE_UI_EVENT_FROM_DERIVED_LISTCTRL_HACK )
void exclusion_dialog::on_update_ui_all_listctrls( wxUpdateUIEvent& event )
{
    on_update_ui_extensions_tab_listctrl( event );
    on_update_ui_urls_tab_listctrl( event );      
}
#endif


void exclusion_dialog::OnOK( wxCommandEvent& event )
{
    // Save the file. Only save if either of the listctrls is dirty (they set it when 
    // added/edited/deleted an entry.
    if ( m_extensions_exclusion_listctrl->get_is_listctrl_dirty() ||
         m_urls_exclusion_listctrl->get_is_listctrl_dirty() )  
    {
        save_exclusion_file();
    }
    // Get rid of the modal dialog. Not transferring any info from this modal's control
    // to a parent dialog, so don't have to bother with wxWindow::Validate or 
    // wxWindow::TransferDataFromWindow. TODO: could return whether or not the list control
    // needs to be redrawn (since changed channel name or update time), or use a global.
    EndModal( wxID_OK );
}


void exclusion_dialog::on_help_button( wxCommandEvent &event )
{
#if ( setupUSE_ONLINE_HELP )
    help_controller::get()->show_help_topic( plkrHELP_ID_EXCLUSION_DIALOG );
#endif 
}

//----------------------------------------------------------------------------------------
// End feature removal condition
//----------------------------------------------------------------------------------------

#endif // setupUSE_INTEGRATED_EXCLUSION_LIST_EDITOR
