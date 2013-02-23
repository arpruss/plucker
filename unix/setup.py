#!/usr/bin/env python

"""
$Id: setup.py.in,v 1.22 2004/11/27 11:23:38 nordstrom Exp $

Set up Plucker for the current user

Copyright 2000 Michael Nordstrom <micke@sslug.dk>

Distributable under the GNU General Public License Version 2 or newer.
"""

import string, sys, os, shutil

VERSION="2005-02-07-cvs"

CONFIGDIR=os.path.join("${prefix}/share/plucker","config")
PALMDIR=os.path.join("${prefix}/share/plucker", "palm")

if os.environ.has_key ('HOME'):
    USERDIR = os.environ["HOME"]
    PLUCKER = os.path.join(os.environ["HOME"], ".plucker")
else:
    # this shouldn't happen on a Unix/Linux system
    sys.exit(1)


def get_answer(question, default = 'n'):
    while 1:
        answer = raw_input (question + "? <y/n> [" + default + "] ")
        if answer:
            if answer == 'y' or answer == 'Y':
                ret = 1
                break
            elif answer == 'n' or answer == 'N':
                ret = 0
                break
            else:
                print "Please answer y or n"
        else:
            if default == 'y' or default == 'Y':
                ret = 1
            else:
                ret = 0
            break

    return ret


def install_config_files():
    print "Copying default config files..."
    PLUCKERRC="pluckerrc"
    if os.path.isfile(os.path.join(CONFIGDIR, PLUCKERRC + ".sample")):
        if os.path.isfile(os.path.join(USERDIR, "." + PLUCKERRC)):
            print "The configuration file .%s already exists in your home directory." % PLUCKERRC
            if get_answer("Do you wish to replace it"):
                shutil.copy(os.path.join(CONFIGDIR, PLUCKERRC + ".sample"), os.path.join(USERDIR, "." + PLUCKERRC))
        else:
            shutil.copy(os.path.join(CONFIGDIR, PLUCKERRC + ".sample"), os.path.join(USERDIR, "." + PLUCKERRC))

    for file in [ "home.html", "exclusionlist.txt" ]:
        if os.path.isfile(os.path.join(CONFIGDIR, file)):
            if os.path.isfile(os.path.join(PLUCKER, file)):
                print "The configuration file %s already exists in your Plucker directory." % file
                if get_answer("Do you wish to replace it"):
                    shutil.copy(os.path.join(CONFIGDIR, file), os.path.join(PLUCKER, file))
            else:
                shutil.copy(os.path.join(CONFIGDIR, file), os.path.join(PLUCKER, file))

def install_viewer():
    viewers = ( ( 'English', 'viewer_en.prc' ), ( 'German', 'viewer_de.prc' ), ( 'Czech', 'viewer_cs.prc' ), ( 'Italian', 'viewer_it.prc' ), ( 'French', 'viewer_fr.prc' ), ( 'Japanese', 'viewer_ja.prc' ), ( 'Faeroese', 'viewer_fo.prc' ), ( 'Danish', 'viewer_da.prc' ), ( 'Chinese (simplified)', 'viewer_zh_CN.prc' ), ( 'Polish', 'viewer_pl.prc' ), ( 'Russian', 'viewer_ru.prc' ), ( 'Spanish', 'viewer_es.prc' ), ( 'Turkish', 'viewer_tr.prc' ), ( 'Thai', 'viewer_th.prc' ), ( 'Catalan', 'viewer_ca.prc' ), ( 'Norwegian', 'viewer_no.prc' ) )

    if get_answer("\nInstall the Palm applications on your Palm OS handheld", 'y'):
        print "\n Select one of the following versions:\n"
        index = 1
        for viewer in viewers:
            print " %d. %s" % (index, viewer[0])
            index = index + 1
        print " q. Abort"

        while 1:
            answer = raw_input("\n Enter choice: [q] ")
            if answer:
                try:
                    n_answer = int(answer)
                    if 1 <= n_answer and n_answer <= len(viewers):
                        viewer = os.path.join(PALMDIR, viewers[int(answer) - 1][1])
                        break
                    else:
                        print " Please answer 1 to %d or q." % len(viewers)
                except ValueError or TypeError:
                    if answer == 'q':
                        return
                    else:
                        print " Please answer 1 to %d or q." % len(viewers)
            else:
                return

        zlib = ""
        if get_answer("\nInstall the ZLib library, too", 'y'):
            zlib = os.path.join(PALMDIR, "SysZLib.prc")

        guide = ""
        if get_answer("Install the User's Guide, too", 'y'):
            guide = os.path.join(PALMDIR, "PluckerUserGuide.pdb")

        os.system("pilot-xfer -i %s %s %s" % (viewer, zlib, guide))


#
# Main
#

if __name__ == '__main__':
    import getopt, os

    print "\nSetting up Plucker %s in %s...\n" % (VERSION, USERDIR)

if os.getuid() == 0: 
    print "You may wish to answer NO and run plucker-setup using your normal login."
    print "Plucker-setup will install files in your HOME directory."
    if not get_answer("Continue"):
        sys.exit(0)

if not os.path.exists(CONFIGDIR):
    print "Could not find config files"
    sys.exit(1)

if not os.path.exists(USERDIR):
    print "Could not find user directory %s" % USERDIR
    sys.exit(1)

if not os.path.exists(PLUCKER):
    print "Creating default Plucker directory (%s)" % PLUCKER
    os.makedirs(os.path.join(PLUCKER, "cache"))

install_config_files()

if not os.system('pilot-xfer -h > /dev/null 2>&1'):
    install_viewer()
else:
    print "\nThe program 'pilot-xfer' is missing so the Palm applications cannot be installed. You'll find them in %s." % PALMDIR

print "\nInstallation Finished\n"
