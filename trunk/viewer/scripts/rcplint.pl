#!/usr/bin/env perl
#
# Search missing resources in the given *.rcp file comparing with sample.rcp
#
use strict;

my $base="sample.rcp";
my $target=shift;
my %rsrc;
#
# Get tag:label pair from sample.rcp
#
open( FILE, "< $base" );
while( <FILE> ){
    if ( $_ =~ /^\"/ ) {
	my ($tag,$value) =  split(/=/,$_,2);
	#eliminate trailing spaces
	$tag             =~ s/\s*$//;
	if( $value =~ /^\s*$/ || $value =~ /\\\s*$/ ){
	    do {
		$_ = <FILE>;
		$value .= $_;
	    } while ( /\\\s*$/ );
	}
	$rsrc{$tag} = $value;
    }
}
close(FILE);

#
# Get tag:label pair from given .rcp
#
my %rsrc2;
my $output;
my $modified=0;
open( FILE, "< $target" );
while( <FILE> ){
    if ( $_ =~ /^\"/ ) {
	my $org          = $_;
	my ($tag,$value) =  split(/=/,$_,2);
	#eliminate trailing spaces
	$tag             =~ s/\s*$//;
	if( $value =~ /^\s*$/ || $value =~ /\\\s*$/ ){
	    do {
		$_ = <FILE>;
		$org   .= $_;
		$value .= $_;
	    } while ( /\\\s*$/ );
	}
	$rsrc2{$tag} = $value;
	if( ! defined $rsrc{$tag} ){
	    #excessing tag
	    $output .= "/*unused*/".$org;
	    $modified++;
	} else {
	    $output .= $org;
	}
    } else {
	$output .= $_;
    }
}
close(FILE);

my $missing;
foreach my $key ( sort keys %rsrc ){
    if( ! defined $rsrc2{ $key } ){
	$missing .= "$key  =  ".$rsrc{$key};
	$modified++;
    }
}
if ( $missing ) {
    my $header;
    $header .= "/*************************************************************\n";
    $header .= " * resources missing in $target:\n";
    $header .= " *************************************************************/\n";
    $output =~ s/BEGIN/BEGIN\n\n\n$header$missing\n/;
}

if ( $modified ) {
    open(FILE,"> $target.lint");
    print FILE  $output;
    close(FILE);
}


0;
