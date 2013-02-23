#!/usr/bin/perl
#
# $Id: doaddition.pl,v 1.3 2004/05/22 19:42:44 prussar Exp $
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


while(<>) {
    s/\@backslash\@$/\\/g;
    $inputLine = $_;
    if ( /^WORDLIST / ) {
        $inWordList = 1;
    }
    elsif ( /^BEGIN/ && $inWordList ) {
        $inListBody = 1;
    }
    elsif ( /^END/ && $inListBody ) {
        $inWordList = 0;
        $inListBody = 0;
    }
    elsif ( $inListBody ) {
        s/(0[xX][0-9a-fA-F]+)/hex($1)/eg;
        while ( s/([0-9]+)\s*\+\s*([0-9]+)/$1+$2/eg ) {};
        if ( /[^\f\t\n\r 0-9]/ ) {
            die "Error in: $_";
        }
    }
    print;
}

