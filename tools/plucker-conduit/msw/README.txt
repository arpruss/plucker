A Windows conduit for Plucker (www.plkr.org)
Bill Nalen (bill@nalens.com)
Copyright 2003

This conduit is licensed under the GPL license, see enclosed COPYING file for details.

This conduit will sync channels from Plucker Desktop to your handheld.

At Hotsync time, it will run Plucker Desktop to update all due channels
and install the resulting databases onto your handheld.


Note: For Plucker Desktop 1.2.0 you must select to have the console
output option selected in the Options - Preferences - Spidering tab
under Show Spider Progress, otherwise the Hotsync will hang.

You also must pick any option except "Never Autoupdate" on the Options - 
Preferences - Autoupdate tab under Channel Autoupdate Mode.


Working with programs other than Plucker Desktop


JPluck

Open or create a plucker.ini file
Add the following entries

[PLUCKER_DAEMON]
autoupdate_mode=3
sync_commandline=java -jar path_to_jpluckc.jar\jpluckc.jar -settings Setting_to_use path_to_jxl\content_name.jxl

You will also need to create the following registry entry
HKey Current User\SOFTWARE\The Plucker Team\Plucker\Path as a string value where the value is the path to your plucker.ini file.  This will allow the conduit to find the plucker.ini file and then find the command to run.

