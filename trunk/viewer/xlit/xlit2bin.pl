#!/usr/bin/perl
# $Id: xlit2bin.pl,v 1.13 2004/03/01 03:33:26 prussar Exp $
#
# xlit2bin - a part of Plucker, the free off-line HTML viewer for PalmOS
# Copyright (c) 1998-2004, Mark Ian Lillywhite and Michael Nordstrom
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
#
#


sub ParseNumber {
    my $in = $_[ 0 ];
    if ( $in =~ /['"]\\x(.*)['"]/ ) {
        "0" . $in;
    }
    elsif ( $in =~ /['"]\\(.)['"]/   ||
         $in =~ /['"](.)['"]/  ) {
        ord( $1 );
    }
    elsif ( $in =~ /0[xX]([0-9a-fA-F]+)/ ) {
        hex( $1 );
    }
    else {
        $in;
    }
}

if ( $ARGV[0] eq "html" ) {
    $html = 1;
}

$top     = 256;
$bottom  = 0;

while(<STDIN>) {
    s/[\n\r]//g;
    s:#.*::;
    s://.*::;
    s/\t/ /g;
    s/^ +//g;
    s/ +$//g;
    s/ +/ /;
    s/^['"] ['"]/32/g;
    s/^['"]\\ ['"]/32/g;
    if ( / +/ ) {
        $from = $`;
        $to   = $';
        if ( $from eq "OTHER" ) {
            $other = ParseNumber( $to );
        }
        elsif ( $from eq "NAME" ) {
            $name = $to;
        }
        elsif ( $from eq "SYMMETRIC" ) {
            $symmetric = $to;
        }
        else {
            $fromNumber = ParseNumber( $from );
            if ( $fromNumber < $top ) {
                $top = $fromNumber;
            }
            if ( $bottom < $fromNumber ) {
                $bottom = $fromNumber;
            }
            $map[ ParseNumber( $from ) ] = ParseNumber( $to );
        }
    }
}

if ( $bottom < $top ) {
    $top    = 0;
    $bottom = 0;
    $map[ 0 ] = 0;
}
if ( $name eq undef ) {
    die "No NAME specified.";
}

$flags = 1;
if ( $other == undef ) {
    $flags |= 2; # TRANSLITERATION_FLAG_OUT_OF_RANGE_NO_CHANGE
}
if ( $symmetric ) {
    $flags |= 4;
}

for $i (0..255) {
    if ( $map[ $i ] == undef ) {
        if ( $other == undef ) {
            $map[ $i ] = $i;
        }
        else {
            $map[ $i ] = $other;
        }
    }
}

if ( $html ) {
    print "<html>\n<head><title>$name</title><body>\n";
    print "<h1>$name</h1>\n";
    print '<table border="1" cellpadding="0" cellspacing="0">', "\n";
    print "<tr><td width=25%>From</td><td width=25%>To</td><td width=25%>From</td><td width=25%>To</td>\n";
    for $i (0..127) {
        if ( $i == 0 ) {
            $c1 = "&nbsp;";
            $c2 = "&nbsp;";
        }
        else {
            $c1 = "&#" . $i . ";";
            $c2 = "&#" . $map[ $i ] . ";";
        }
        $j = $i + 128;
        $c3 = "&#" . ( $i + 128 ) . ";";
        $c4 = "&#" . $map[ $i + 128 ] . ";";
        print "<tr><td>$i $c1</td><td>$map[ $i ] $c2</td><td>$j $c3</td><td>$map[ $j ] $c4</td>\n";
    }
    print "</table></body></html>\n";
}
else {
    binmode STDOUT;
    $header = pack( "NNa60nnnn", 1, $flags, $name, $top, $bottom, $other, 4+4+60+4*2 );
    print $header;

    for $i ($top..$bottom) {
        print pack( "C", $map[ $i ] );
    }
}

