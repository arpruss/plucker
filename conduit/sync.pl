#!/usr/bin/perl -w
#
# Perl-Conduit for Plucker
#
#------------------------------------------------------------------
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to:
#     Free Software Foundation, Inc.
#     59 Temple Place - Suite 330
#     Boston, MA  02111-1307, USA.
#------------------------------------------------------------------

use diagnostics; 

use PDA::Pilot;
use Data::Dumper; 
use Env qw(HOME);
use Getopt::Long; 

# Determine the OS of the unit. $device for OS/2
# was changed from 'COM1' to '/dev/com1', because
# OS/2 supports devices just like linux/unix do
# From the docs: 
#
# $^O     The name of the operating system under
#         which this copy of Perl was built, as
#         determined during the configuration
#         process.  The value is identical to
#         $Config{'osname'}.
#
# Remember to set the link under linux and Solaris
# machines in /dev, or pilot-link will fail. USB users
# will need to make sure that /dev/ttyUSB{#} is assigned
# for each port they use. 

$device = ($^O eq 'os2' && '/dev/com1') 
           || ($^O eq 'MSWin32' && 'COM1') 
           || '/dev/pilot';

#####################################################
#
# Assign some basic global parameters here
#
#####################################################
$pluckerhome     = $HOME . '/.plucker';   # where Plucker stores data
$ViewerAppID     = 'Plkr';                # Creator ID
$ViewerDBType    = 'Data';                # Data type
$ViewerDBVersion = 7;                     # PluckerDB internal version
$database        = 'PluckerDB';           # Name of the output DB

#####################################################
#
# use 'db' for to-disk PluckerDB writes or keep the quotes 
# blank for output directly to the serial port
#
#####################################################
$output          = 0;                     # 0: serial, 1: disk

@databases       = ('PluckerDB');
@cachedirs       = ('.plucker');

&GetOptions("name=s"   => \$database,
            "cache=s"  => \$pluckerhome,
            "database" => \$output,
	    "port=s"   => \$device);

@databases = split(/\s/, $database);
@cachedirs = split(/\s/, $pluckerhome);

#####################################################
#
# Print the application splash using a standard 
# 'here-print' syntax.
#
#####################################################
print <<_EOF_;
 
   @{[ "="x55 ]}
              Plucker conduit written in Perl
   @{[ "="x55 ]}

   ---o-o---  (c) 2000 by the Plucker Development Team
   ---o--o--      based on the original conduit written by
   ---o-o---      Mark Lillywhite. This version written by
   ---o-----      Alexander Wagner and David Desrosiers.
   ---o-----      Special thanks to: Holger Duerer, and
                  Alan Harder & Kenneth Albanowski

	email: plucker-team\@rubberchicken.org 

   Currently running OS       : $^O
   Communicating via          : $device
   source -> database created : 
_EOF_

if (($#databases != $#cachedirs)) {
   die "\n--> ERROR: You did not specify the same number of cachedirs and databases!\n";
}
 
for $i (0 .. $#databases) {
   print "   $i: $cachedirs[$i] -> $databases[$i]\n";
}

if ($output == 1) {
   my %dbinfo = (
      flagReadOnly          => 0,  flagResource            => 0,
      flagBackup            => 0,  flagOpen                => 0,
      flagAppInfoDirty      => 0,  flagNewer               => 0,
      flagReset             => 0,  flagExcludeFromSync     => 0,
      modnum                => 0,  index                   => 0,
      createDate            => 0,  modifyDate              => 0,
      backupDate            => 0,  
      type                  => $ViewerDBType,
      creator               => $ViewerAppID,
      version 	            => $ViewerDBVersion,
      name                  => $database
   );

      for $i (0 .. $#databases) {
         $pluckerhome       = $cachedirs[$i];
         $database          = $databases[$i];
         $filename       = $pluckerhome . "/" . $database . ".pdb";
         $db             = PDA::Pilot::File::create($filename, \%dbinfo);   
         $type           = "$pluckerhome/$database";

         print "\n   ", "-"x50, "\n";
         print "   pdb file '$filename' created... \n";
         print "   ", "-"x50, "\n\n";
         &read_dir;
      }
} else {
   $type     = "handheld";

   # Open the device and get the socket with DLP
   $socket   = PDA::Pilot::openPort($device);
   print "\n           ", "-"x30, "\n";
   print "             Now press HotSync button...   \n";
   print "           ", "-"x30, "\n\n";
   $dlp      = PDA::Pilot::accept($socket);

   #####################################################
   #
   # Extract the username from the device after opening
   #
   #####################################################
   $ui = $dlp->getUserInfo;
   print '='x15, "[ Syncing: $ui->{name} ]", '='x15, "\n  \n";
   print "  Connection established!\n";
   $dlp->getStatus();
   #####################################################
   #
   # Delete the plucker database on-palm by destroying
   # the creator ID on the device (another DLP function)
   #
   #####################################################
   $syncsize    = 0;
   for $i (0 .. $#databases) {
      print "\n\n  Current database: $databases[$i] \n";
      $pluckerhome = $cachedirs[$i];
      $database    = $databases[$i];
      print "$pluckerhome  ---> $database\n";
      print "  Removing old Plucker-database";
      $dlp->delete($database);     
      print "   --> done.\n";      

      #####################################################
      #
      # Create a new database on-palm, using the reverse
      # of the above
      #
      #####################################################
      print "  Creating new Plucker-database";
      $db = $dlp->create($database, $ViewerAppID, $ViewerDBType, 
                        0, $ViewerDBVersion);
      print "   --> done.\n";
      &read_dir;
      $db->close();
   }
}
      $syncsize >>= 10;
      $i = $#databases + 1;
      printf "\n   %i kB transfered to $type\n", $syncsize;
      print "   using $i databases\n";
      print "   Closing Plucker cache directory ...\n";
      closedir(PDir);

#####################################################
#
# Close and clean up a bit, dumping some debug 
# information to STDOUT
#
#####################################################
$db->close();
print "   Plucker database created and closed in $pluckerhome\n\n" if ($output == 1);
print "   Plucker is updated. Closing conduit...\n\n";
print "="x13, "[ Syncing: $ui->{name} done]", "="x13, "\n" if ($output != 1);

sub read_dir { 
#####################################################
#
# Read in the cache directory and parse it's files
# for output to the PluckerDB structures or to the
# serial port, in the case of a direct serial link
#
#####################################################
   $pcache   = $pluckerhome . "/cache";  
   print "  \nUsing $pcache as input.\n";   
   opendir PDir, "$pcache" or die "Can't open directory $pcache: $!\n";
   print "  $pcache directory opened, now processing files...\n";
   $i           = 0;
   @files = grep !/^\./, readdir(PDir);
   for $file (sort  {$a <=> $b} @files) {
      open(PFile, "< $pcache/$file") or die "Can't open $pcache/$file: $!";
      $size = (stat("$pcache/$file"))[7];
      $recordID  = $file;        # Filenames are record ID's
      $syncsize += $size;
      print "  |\n";
      print "  [ $recordID ] processing...\n";
      print "  |--> Reading file  ";  

      #####################################################
      #
      # Read the complete file to $data and process it
      # there. This was changed from a previous method
      # which proved to be substantially slower. 
      #
      #####################################################
      sysread PFile, $data, $size;              
      print "  --> $size Bytes done.\n";
      print "  |--> Adding data... ";   
      if ($output == 1) {
         $db->addRecordRaw($data, $recordID, 0, 0);
      } else {
         $rec               = $db->newRecord();
         $rec->{'category'} = 0;
         $rec->{'deleted'}  = 0;
         $rec->{'archived'} = 0;
         $rec->{'secret'}   = 0;
         $rec->{'modified'} = 1;
         $rec->{'raw'}      = $data;
         $rec->{'id'}       = $recordID;
         $db->setRecord($rec);
      }
   print " --> done.\n";
   close(PFile);
   }
}
# "The End(tm)" - David A. Desrosiers
