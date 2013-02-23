#!/usr/bin/perl
##########################################################################
# 
# comic-plucker.pl: A perl script to fetch comic strips from ucomics.com,
#                   kingfeatures.com, and other sites. It will fetch the
#                   master list of comics from the upstream page, then
#                   fetch each sub-comic page, and extract just the comic
#                   image itself, and write a separate HTML file for each
#                   comic.
#
#                   From there, you can spider the local content,
#                   starting with the file "index.html, to a depth of 1,
#                   with whatever color depth you wish.
#
# Copyright 2005 David A. Desrosiers. All Rights Reserved.
#                desrod@gnu-designs.com
#
# Permission to use, copy, modify, and distribute this script is hereby
# granted without any fee, provided that this copyright notice and
# permission notice are not removed from any derived copies.
#
# Please see the README file in this directory for more information
#
# This tool is a work in progress.
#
# -dad 3/6/2005
#
##########################################################################
use strict;
use Env qw(PATH HOME);		# (core) Check $HOME and other vars
use URI;			# Grab the URL fragments
use Date::Manip;		# Dates on the output files
use HTML::Entities;		# Decode entities in filenames
use CGI qw/:standard/;		# Output the final index page
use LWP::Simple qw/!head/;	# Fetch the page itself
use LWP::UserAgent;		# Create a proper User-Agent header
use HTML::TreeBuilder;		# (cpan) Find the attributes of the tag

use constant TITLE => 0;
use constant FILE  => 1;

my $cgi		= CGI->new();
my $ua		= LWP::UserAgent->new;
my $date	= UnixDate("today", "%b %e, %Y at %T");
my $filedate	= UnixDate("today", "%G%m%d");

# Store comics under user's home directory, where it is 
# writeable by that user, instead of /var/spool/netcomics
my $homedir	= $ENV{'HOME'};
my $pluckerhome	= "$homedir/.plucker";
my $comicdir	= "$pluckerhome/comics";

my @useragents	= (
        'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.1.4322)',
        'Opera/7.54 (Windows NT 5.0; U)  [de]',
        'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)',
	'Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en) AppleWebKit/125.5.6 (KHTML, like Gecko) Safari/125.12',
        'Mozilla/4.7 (Macintosh; I; PPC)',
        'Mozilla/5.0 (compatible; Konqueror/3.1; Linux)',
	'iSiloX/4.2 Windows/32',
	'Lynx/2.8.5rel.1 libwww-FM/2.14 SSL-MM/1.4.1 OpenSSL/0.9.7d',
	'SharpReader/0.9.5.1 (.NET CLR 1.1.4322.2032; WinNT 5.1.2600.0)',
);

my $page	= "http://www.ucomics.com/comics/";
print "Now fetching list of comics from $page...\n\n";
my $response	= $ua->request(HTTP::Request->new(GET => "$page"));
my $root	= HTML::TreeBuilder->new_from_content($response->content);
my (%strips, %stripname) = ();

# Open the master index file
mkdir "$comicdir";
open(INDEX, ">$comicdir/index.html") 
	or die "Cannot open $comicdir/ ($!)\n";
print INDEX start_html(-title => "Pluck Comics / Perl Edition"),
	p(strong("$date"));

my @Links;
foreach my $node ($root->find_by_tag_name('option')) {
	# Randomize the UserAgent strings, so we don't show up
	# on their radar as easily
	my $agent	= $useragents[int rand($#useragents)];

        my $link	= $node->attr('value');
        $strips{$link}++ if ($link);

        my $url		= URI->new($link);
        my $path	= $url->path;

        fetch_comic($link, $agent) if (length($path) > 1);
}

# Sort the array of comic titles
for (sort {uc($a->[TITLE]) cmp uc($b->[TITLE])} @Links) {
	$_ =~ s/([\x80-\xFF]+)/encode_entities($1)/ge;

        print INDEX 
		a({-href => $_->[FILE]}, 
        	$_->[TITLE]), br(), "\n";
}

# Clean up and close the master index file
print INDEX end_html;
close INDEX;

print "That's it! You're done... now spider $comicdir/index.html\n\n\n";

##########################################################################
# Fetch the actual comic images from each linked sub-page
sub fetch_comic {
        my ($strip,
	    $agent)	= @_;

        my $req		= HTTP::Request->new(GET => $strip);

	$req->referer("$page");
	$ua->agent($agent);

	my $response 	= $ua->request($req);
        my $content	= $response->content;

        my $root	= HTML::TreeBuilder->new_from_content($content);
        my %image_links	= ();

        my $title = $root->look_down('_tag', 'title')->as_text
          if $root->look_down('_tag', 'title');

        # Find comic strip image in the comic sub-pages
        foreach my $node ($root->find_by_tag_name('img')) {
                $image_links{ $node->attr('src') }++;
        }

        $title =~ s/([\x80-\xFF]+)/encode_entities($1)/ge;
        $title =~ s/^.*\bfeaturing\s+(.+)$/$1/i;
        $title =~ s/(.+) -- The Best Comic Site In The Universe!/$1/i;

        (my $filename = lc($title)) =~ s/\W+/_/g;
        $filename =~ s/\W//g;

        foreach my $comic_image (sort keys %image_links) {
                if ($comic_image =~ m|/comics/|) {
                        my $imgurl  = URI->new($comic_image);
                        my $imgpath = $imgurl->path;
                        my $image   = (split('/', $imgpath))[-1];

                        print "Retrieving...: \"$title\"\n";
			print "       URL...: $strip\n";
			print "     image...: ${filedate}-${image}\n\n";

                        getstore($comic_image, "$comicdir/${filedate}-${image}");

                        $stripname{$title} = {
                                title    => $title,
                                filename => lc($filename) . ".html",
                                url      => $strip,
                                image    => $image,
                        };

			write_file($stripname{$title});
                        push @Links,
                          [
                                $stripname{$title}->{title},
                                $stripname{$title}->{filename}
			  ];
                }
        }
        $root = $root->delete;
}

##########################################################################
# Output each comic to its own index file
sub write_file {
        my $comic_file = shift;

        open(STRIPFILE, ">$comicdir/$comic_file->{filename}")
          or die "Can't open $!\n";

        print STRIPFILE start_html(-title => "$comic_file->{title}"),
          p(strong("$comic_file->{title}"), "for $date"),
          p(a({-href   =>"index.html"}, "Back to index"), br(), 
		img({-src    => "${filedate}-$comic_file->{image}", 
			   -border =>'0'}),
	  ),
	  p(i("URL: ($comic_file->{url})"),),
          end_html;
        close STRIPFILE;
}
