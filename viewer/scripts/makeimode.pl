#!/usr/bin/perl
#
# $Id: makeimode.pl,v 1.1 2003/10/23 19:23:48 nordstrom Exp $
#
#  Dave Maddock
#
#  This script will take a listing of tbmp files as arguments, and output a pdb
#  to stdout.
#
#  It does no checking to make sure that the files are correct in any way.
#  Make sure that:
#    1. Your files are proper tbmp files, with the desired color depth.
#    2. You have 1 file for each number in the following ranges:
#        63647-63740    63808-63920 (these are the icon character codes)
#       If you miss one out, the icons won't match up to their appropriate value
#       in the viewer. 
#
#  ./makeimode *.bmp > imodeicons.pdb


sub monthtime
    {
    local($y, $l, $guess) = ($_[0], $_[1], time());
    local(@tm) = gmtime($guess);
    while ($diff = $y - ($tm[5] + 1900))
        { @tm = gmtime($guess += $diff * 363 * 86400); }
    while ($diff = $l - $tm[4])
        { @tm = gmtime($guess += $diff * 27 * 86400); }
    return $monthtime{pack('C2', $y, $l)} =
        $guess - $tm[3] * 86400 - $tm[2] * 3600 - $tm[1] * 60 - $tm[0];
    }

sub timegm
    {
    local($s, $m, $h, $d, $l, $y) = @_;
    $y += $y < 70 ? 2000 : 1900 if $y < 1900;
    $y = 1970 if $y < 1970;
    $l = $l < 0 ? 0 : $l > 11 ? 11 : $l;
    local($mt) = $monthtime{pack('C2', $y, $l)} || &monthtime($y, $l);
    return $mt + $d * 86400 + $h * 3600 + $m * 60 + $s;
    }

sub ptime
    { return ($_[5] ? &timegm(@_) : ($_[0] || time())) + 2082844800; }



$type = "Imod";
$creator = "Plkr";
$name = "Plkr-Imode";
$dbver = "";
$dbattr = 0;
$si = 0;
$offset = 0;



$crtime = &ptime(); $mdtime = &ptime(); $bktime = 0; $si = $offset = 0;


foreach $file (@ARGV) {
    open(FILE, $file) || &die("$file is unreadable");
    undef $/; $data[$si] = ""; $data[$si] .= <FILE>; close FILE;
    $len = length($data[$si]);
    $offset[++$si] = ($offset += $len);
}

$nsec = scalar(@data);
$hdrsize = 78 + (($dbattr & 1) ? 10 : 8) * $nsec + 2;
$aioff = $ainfo eq '' ? 0 : $hdrsize;
$sioff = $sinfo eq '' ? 0 : ($aioff ? $aioff + length($ainfo) : $hdrsize);

print pack('a32nnNNNNNNa4a4Nx4n', $name, $dbattr, $dbver, $crtime, $mdtime,
    $bktime, $mdcount, $aioff, $sioff, $type, $creator, $idseed, $nsec);
for ($si = 0; $si < $nsec; $si++)
    { print ($dbattr & 1 ? 
        pack('a4nN', $type[$si], $id[$si], $offset[$si] + $hdrsize) :
        pack('NN', $offset[$si] + $hdrsize, ($attr[$si]<<24) + $id[$si])); }
print "\x00\x00" . $ainfo . $sinfo . join('', @data);

