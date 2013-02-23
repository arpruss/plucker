#!/usr/bin/perl
#
# $Id: fix_sdk.pl,v 1.2 2003/03/19 00:14:34 adamm Exp $
#
# Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
# Copyright (c) 1998-2002, Mark Ian Lillywhite and Michael Nordstrom
# 
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
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

my $n = undef;
my $data;
my $file = shift(@ARGV);
my %valid = ('sysLibTrapCustom', 1);
my $base;

open(INPUT, $file) || die "$!";
while(<INPUT>) {
  $data .= $_;
}
close(INPUT);

open(OUTPUT, ">" . $file);
foreach (split(/\r?\n/,$data)) {
  if (/(\w+)\s*=\s*(\w+)/) {
    if (defined($valid{$2})) {
      print OUTPUT "\t$1_use_defines_not_enumerators\n" if (!defined $n);
      $base = $2;
      $n = 0;
    }
  }
  $n = undef if /[{}]/;
  if (defined $n && /^\s*(\w+)/) {
    $valid{ $1 } = 1;
    print OUTPUT "#define $1 ($base+$n)\n";
    $n++;
  }
  elsif ( /^#define (vchrSony(Min|Max))/ ) {
    print OUTPUT "#ifndef $1\n";
    print OUTPUT "$_\n";
    print OUTPUT "#endif\n\n";
  }
  elsif ( /^\s*#\s*pragma mark/ ) {
    # just ignore these lines
  }
  else {
    print OUTPUT "$_\n";
  }
}
close(OUTPUT);
