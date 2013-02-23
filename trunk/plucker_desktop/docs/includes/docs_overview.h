/*! \page docs_overview One page summary overview of program
  This page contains the subsections
  <ol>
     <li>\ref purpose_of_the_program
     <li>\ref architecture
     <li>\ref program_flow
  </ol>
  
  <hr>

  \section purpose_of_the_program Purpose of the program
  
  Plucker Desktop is a visual management tool for Plucker. Plucker is a suite of 
  scripts, utilities, and viewer that allow an off-line webbrowsing for PDAs. 
  Plucker Desktop manages a set of channels, which get fed through the parser, and 
  ultimately HotSynced over to the PDA.
  <hr>
  
  \section architecture Architecture
  
  There is a single application object, called the_app.
  
  The channels are managed via a single text configuration file called plucker.ini or
  .pluckerrc, depending on the platform. The global the_configuration object does 
  the reading and writing to this text configuration file.
  
  When the_app is started, a few wxModule-managed classes automatically 
  start up: for example help_controller, commandline_parser, image_list and 
  plucker_controller. These module-managed classes clean themselves up automatically
  after program exit.
  
  There is a single, non-visual plucker_controller that handles all channel 
  operations, like adding, updating, and deleting channels. 
  
  The updating of channels is via a python script. The output of the python script 
  can be selected by the user to either be shown in a shell console window, or on 
  a build_progress_dialog.
  
  Almost all resources (dialogs, etc) are loaded at runtime from XML resouces 
  (these .xrc files can be found in the plucker_desktop/resource/xrc sourcecode 
  directory).  
    
  Translation of strings is during runtime, via gettext catalogs. The system locale
  is selected via the methods in the internationalization class, and the translated 
  strings are then read from the proper catalog (.mo file).  
  
  The HotSync behaviour is via the palm_installer class, which allows 
  cross-platform HotSyncs. On POSIX, it calls pilot-link, and on MSW/MAC, it 
  sends files over to /Palm/ThatUser/Install directory and marks the appropriate 
  system flag to notify the Palm that there are files to be HotSynced.
  
  <hr>
    
  \section program_flow Program Flow
  
  When the program is started without commandline arguments, the program starts in
  a normal GUI manner. A main_frame is built, and then the program then continues 
  by just responding to user events, such as the user clicking a button to open 
  a preferences_dialog,  etc. 
  When actions need to be done on channels, the application calls the relevant 
  plucker_controller method. The application ends when the main_frame is closed.  
  
  When the program is started with commandline arguments, a main_frame is not 
  created. A commandline_parser parser is initialized and actions are done 
  to the plucker_controller directly. 
  The program then terminates when the desired command is done. 
  (However if user has used a build_progress_dialog that they said not to autoclose on 
  completion, then the program will only terminate when they close that dialog.)     
  
*/
