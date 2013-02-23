
A general purpose conduit installer for Windows
Bill Nalen (bill@nalens.com)
Copyright 2003
Licensed under the GPL license (see copy of enclosed COPYING file for details)

This program will install a conduit for the Palm.  See the Hotsync developer
documentation for more details.

The command line arguments are all one letter long, to maximize the amount
of arguments that can be fit into the anemic Win32 Commandline space:

-i install:        Install the conduit
                  requires the creator and conduit options

                  or

-u uninstall:      Uninstall the conduit
                  requires only the creator option


-c conduit:            path & filename of conduit
-e creator:            creator id for the application to sync with
-p cp:                 classpath for Java conduits
-l class:              main class for Java conduits
-u user:               user entry
-i info:               info entry
-n name:               name of conduit
-f file:               local file name
-t title:              title of conduit
-d dir:                local directory
-b remoteDB:           remote file name
-r restarthotsync:     restart hotsync manager upon exit
-s silentifnoerror:    don't display messages if no errors


Example usage to install a conduit:
condinst.exe -i -r Plkr -c "C:\Program Files\Plucker\conduit_install\pluckercond.dll" -t "Plucker" -r -s 

Example usage to uninstall
condinst.exe -u -r Plkr 
