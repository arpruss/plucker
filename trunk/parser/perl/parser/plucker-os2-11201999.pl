#!/usr/bin/perl
#
# THIS IS NON-STABLE BETA CODE
#
#######################################
# Note: certain changes will be needed for win32/other compatibility, like
# binmode i
# calls and tricks like:
# eval "exec /usr/local/bin/perl -S $0 $*" if 0;
# and the %0.bat stuff for the archaic OSes...
# ..when there is time...
#######################################
#
# filename: pluckit
# usage: ./pluckit [[root_url] maxdepth]
#
# i.e. ./pluckit http://www.cia.gov 2
#
# 	"Go Pluck yourself!"
#
# [debug] output:
#
# plucking [405P/861p/611c] n32 (3d/3m) 
# http://home.global.co.za/~kitiara/tsasa/
#            1.   2.   3.    4.  5. 6.                   7.
# where:
#
# 1. parent of the page currently being parsed
# 2. current value of $page (which decides the record number of a document)
# 3. current number of @PLUCKME (the number of pages left to pluck)
# 4. number of documents in @docs (good parses)
# 5. current depth of page in 6.
# 6. max depth of page in 6.
# 7. the (parent) URL being plucked
# 
# ..and..
#
# DUPE CONTENT: (21p->13o) http://lisa.title14.com/random/
#                1.   2.                 3.
# where:
#
# 1. the parent page that has the duplicate link on it
# 2. the original page where the content can be found
# 3. the link in question
#
# ..and..
#
# Cannot fetch http://lisa.title14.com/random/Weekly%20Status%204b.doc
# (status 200 [application/msword] OK)
#
# where:
#
# 1. link that was not a) successfully returned from server OR b) not a
#    valid 'type' (see &parse_type) 
# 2. status returned from server 
# 3. MIME type
# 4. response message
#
# -----------------------------------------------------------------------
# the usual suspects:

use HTML::LinkExtor;
use LWP::UserAgent;
use HTML::Parser;
use HTML::TokeParser;
use URI::URL;
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# Initial setup
# -----------------------------------------------------------------------

my $verbose		= 0; 
my $pluckerhome         = "$ENV{HOME}/.plucker";
my $debug_dir		= "$pluckerhome/.debug";

# -----------------------------------------------------------------------
# These two directories exist under ~/.plucker. cache holds the binary files for
# sync.pl to pick up and assemble, and ./html contains the raw html files that
# are brought down from the web. Both will probably be removed as this script
# evolves to the point where the writing to disk is no longer required.

  my $crawler_dir         = "$pluckerhome/html";
  my $cache_dir           = "$pluckerhome/cache";
# -----------------------------------------------------------------------


# -----------------------------------------------------------------------
# This is passed to the remote server as the connecting UserAgent string

  my $user_agent          = "Plucker/0.05 [en] PalmOS(tm)/EPOC";
# -----------------------------------------------------------------------

my $home_url            = "$pluckerhome/home.html";
my $error_url           = "$pluckerhome/error.html";
my $external_url        = "$pluckerhome/external.html";
my $mailto_url          = "$pluckerhome/mailto.html";
my $page                = 11;                                           # index number start.. first root_url will be this
my $start_id            = 11;                                           # starting ID for root
my $logfile             = "$pluckerhome/pluckit.log";                              # logs start, dupes and end/summary

# if not in ARGV[0]
my $default_root        = "file://$home_url";                           # if not in ARGV[0]

my $localdate           = localtime();
# -----------------------------------------------------------------------
# An array containing all the (good) URLs plucked (therefore all hash
# names) this will be used when writing the data out to .pdb/sync/whatever
# format

  my @documents;          
  my @PLUCKME;
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# Tempfile needed by TokeParser, but only on OS/2
#  my $plktmp = ($^O eq 'os2' && 'E:/temp/plucker.tmp');

  if ($^O eq 'os2') {
        my $plktmp = ("$ENV{TMP}/plucker.tmp" || "$ENV{TEMP}/plucker.tmp" || "$ENV{TMPDIR}/plucker.tmp") 
                       or die "Hey, set TMP= something, silly!\n";
        my $cache_dir         = "./cache";
  }
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# These set up the 'browser' environment that the servers we hit will see us as...
# (nice addition for proxy hits)

  $ua = new LWP::UserAgent;
  $ua->agent("$user_agent");
  $ua->env_proxy;
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# $p will hold content of the body

  my $p ="";
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# Determine the os from the perl version parsing this script.
#
# $^O     The name of the operating system under which this
#         copy of Perl was built, as determined during the
#         configuration process.  The value is identical to
#         $Config{'osname'}.

  my $os = $^O;
# -----------------------------------------------------------------------

#
######################
#
# Plucker API (well..)
#
# All content is held in hashes. each hashes name is the URL. all hash names
# are held in @docs. subs can access the plucked data once the MAIN loop is
# done. each hash has the following keys:
# -----------------------------------------------------------------------
# The actual URL that was plucked (and the name of this hash).
# ex: http://www.whitehouse.gov/pics/monica/intimate/cigar.gif
# url		[URL] 		
# -----------------------------------------------------------------------
# Was this page plucked ok?
# ok		[Y|N] 		was this page plucked ok?
# -----------------------------------------------------------------------
# The <TITLE> of the page (non-image pages(data), that is..)
# title		[title]
# -----------------------------------------------------------------------
# The parent URL that this page was linked from. Note: for root URL this
# is the same as the key url (so you can find it later!)
# parent	[URL]
# -----------------------------------------------------------------------
# The current depth of this document. 0=root page/file.
# depth		[depth]
# -----------------------------------------------------------------------
# The depth (from root) that this page and all links from it must not exceed.
# max_depth	[max.depth]
# -----------------------------------------------------------------------
# Is this page at maximum pluck depth? Essential when same pages plucked from
# different parents.
# at_max	[Y|N]
# -----------------------------------------------------------------------
# The _complete_ headers returned from the server.
# headers	[headers]
# -----------------------------------------------------------------------
# The MIME type of the content returned (gotten from headers).
# type		[MIME type]
# -----------------------------------------------------------------------
# The actual length (bytes) of content on the page (headers not included,
# just content).
# length	[length]
# -----------------------------------------------------------------------
# has this page been plucked? that is, have its links been plucked.
# plucked	[Y|N]
# -----------------------------------------------------------------------
# The actual content of the link. raw data, picture, html, whatever..
# content	[content]
#
# NOTE: everything to do with paragraphs and headers in various database
#       formats is taken care of AFTER the actual plucking is over. this
#       is why those things required are not listed here. do them in
#	the subs where that format is relevant.
#

######################
#
# it begins.. note: "parent" is the current URL, "child" links are derived from the parent..
#
# Flush STDOUT
  $| = 1;
  &logit(" START of logfile on $localdate");
# keep all initial setup controlled from here
  &setup;
#
#-----------------------------------------------------------------------------#
MAIN:
while ($parent = shift @PLUCKME) {

# print "A doing $parent\n";
	$parent = &fix_url($parent);                            # fix up the URL itself as necessary
	if ($did{$parent}++) {                                  # builds visited urls keys, # times=value
		&logit("\n\tDuplicate Content:\n\tIgnored [$did{$parent}] $parent\n\n");
		warn "DUPE url: $parent\n" if $verbose >= 3;
		next MAIN; # Add more here to direct parent to original (sole) content (hmm...)
	}
	if ($bad{$parent}) {                                    # don't bother, its bad(tm)...
                &logit("BAD $parent");
		warn "BAD url: $parent\n" if $verbose >= 3;
                next MAIN;                                      # restart loop, its BAD(tm)!
	}
	if ($parent ne "$root_url" && ($$parent{depth} >= ($$parent{max_depth} + 1))) {
		&logit("MAX DEPTH [$$parent{depth}] $parent");
		warn "MAX DEPTH [$$parent{depth}\/$$parent{max_depth}] url: $parent\n" if $verbose >= 3;
		next MAIN;
	} 
	# the above tests should be in this order: dupe, bad, max_depth
	# -----------------------------------------------------------------------
	# this is where you start to work with a valid parent..
	# needed to know hash names when writing DB

          push @docs, $parent;
	# -----------------------------------------------------------------------
	# how many docs are in the array?

          $num_docs             = @docs;
	# -----------------------------------------------------------------------
	# how many docs have yet to be checked?

          $check_count            = @PLUCKME;
	# -----------------------------------------------------------------------
	# This pass is the first pass of the script (root URL). In here, we set the
	# url for the root url, which identifies this as the parent. We could also
	# try to match $root_url later, if it makes sense at that time. 

	if ($num_docs == 1) {
		$$parent{url}           = $parent;
		$$parent{parent}        = $parent;
		$$Parent{depth}         = 0;
	}
        # -----------------------------------------------------------------------
        # warn "plucking [", $$parent{parent}, "\P\/$page\p/", $check_count,"c] 
        # n$num_docs ($$parent{depth}d\/$$parent{max_depth}m) $parent\n";
        # 
	# legend: P = parent, p = pageID, c = # of @PLUCKME elements, 
	# n = number of docs in @docs, d = parent depth, m = parent max depth
	#	
	# time to build a request oject.. use $parent to write first doc and find links
	#

          $request = new HTTP::Request(GET,$parent); # build request
          $response = $ua->request($request);        # Pluckit!
        # -----------------------------------------------------------------------
	# now, what to do with the request? might be good, might be bad..
        # -----------------------------------------------------------------------

	if ($response->is_success && ($$parent{type} = &parse_type($response->content_type))) {
		$$parent{plucked}      = "Y";                                # get/set type 
		my $everything         = $response->as_string;               # COMPLETE response 
		($headers              = $everything) =~ s#(.*?)\n\n.*#$1#s; # pull headers
		$$parent{headers}      = $headers;                           # store headers
		($body                 = $everything) =~ s#.*?\n\n(.*)#$1#s; # pull body
		$base                  = $response->base;                    # base url for $parent
		$$parent{ok} = "Y";                                          # success!
		} else {                                                     # Bad(tm) request!
		warn "Cannot fetch $parent (status ", $response->code, " [", $response->content_type, "] ", $response->message,")\n";
		$$parent{ok}           = "N";                                # may need status later..
		$bad{$parent}++;                                             # its bad(tm)...
		next MAIN;                                                   # no use staying here..
	} 
        # -----------------------------------------------------------------------
	# so, you have an image.. no use parsing for links, eh? 
        # -----------------------------------------------------------------------

	if ($parent_type =~ /image/) {
		$$parent{content} = $body;                                                 # put image into a scalar.. can mangle/convert later! :)
		($$parent{length} = $headers) =~ s#.*?Content-Length: (.*?)\n.*#$1#s;      # set size of image (bytes)
		&write_debug($parent);                                                     # for debug, write to the $crawler_dir
                                                                                           # need more here..
		next MAIN;                                                                 # move along... no need to parse
	}

        # -----------------------------------------------------------------------
	# you are in a REAL document now... ready to parse it.. (look for links and load PLUCKME)
        # -----------------------------------------------------------------------
       
        if ($os eq "os2") {
                open (TMP, ">$plktmp");                # OS/2 needs this tempfile, sorry.  
                print TMP $body;
                close TMP;
                $p = HTML::TokeParser->new($plktmp);   # load $body up!
        } else {
                $p = HTML::TokeParser->new(\$body);    # load $body up!
        }
#	print "B got body!\n";

        # -----------------------------------------------------------------------
	# a word about HTML::Tokeparser...
	#   0     1      2         3         4 	 	$token->[?]
	# ["S", $tag, %$attr, @$attrseq, $origtext]	# Start
        # -----------------------------------------------------------------------

	my ($text, $parsed_body);
	while (my $token = $p->get_token) { 			# traverse body
		my $rel_url;					# used with $base to make an absolute url
		my $tag_id   		= $token->[0]; 		# [S,E,T,C,D] see HTML::TokeParser
		my $tag_text 		= $token->[1]; 		# href, img, ...
		if ($tag_id eq "S"){ 				# Start tag
			if (grep { /\A$tag_text\Z/ } @tags_links) { # hyperlinks 
				my $key;
				foreach $key ( keys %{$token->[2]} ) {
#	print "C doing \'$key\' \n";
					if ( $key =~ /href|src/ ) {
						$rel_url = %$token->[2]{$key};
						### print "\tKEY: \'$key\' VALUE: \'$rel_url\' ";
						} elsif ( $key =~ /maxdepth/i ) {
							# got bored..
                        				my $child_url = URI->new_abs($rel_url, $base);
							$child_max_depth = %$token->[2]{$key};
						} else {
					}
				}	
				my $child               = fix_url(URI->new_abs($rel_url, $base));
#	print "D got child: $child \n";
                        	$child			= "$child";
				if ($did{$child}){	# make our mark that we've been there, done that..
					# -this is really nice, as it will even point to content that
					# hasn't been plucked yet, but is in the @PLUCKME queue
					next MAIN; # no use in overwriting %$child, it exists already
				}
                        	$$child{url}	= $child;		# not a dupe, so assign it..
                        	$$child{parent}	= $$parent{url};	# who loves ya..
				$$child{depth}	= $$parent{depth} + 1; 	# max + 1
				if ( $child_max_depth ) {
					$$child{max_depth} 	= $child_max_depth; 	# from a home.html, or..?
					} else {
                        		$$child{max_depth}	= $$parent{max_depth}; 	# clone max_depth
				}
				$$child{at_max} = "Y" if $$child{depth} >= $$child{max_depth}; # makes sense..
                        	push @PLUCKME, $child; 	# this $child needs plucking!  
				# if not a hypertext link, then what to do with it?
				} else {
				grep { /\A$tag_text\Z/ } "title"; 	# page title 
				unless ($$parent{title}){ 		# only add if page has no title yet
					$$parent{title} = $p->get_trimmed_text;
				}
			} # end of if (grep { /\A$tag_text\Z/ } @tags_links)..	
		} # end of if (tag_id eq "S"... (start tag)
	} # end of while (my $token...
	$$parent{length}		= length($parsed_text);	# will need later..
	$$parent{content}		= $text;
	&debug($$parent{url});					# write debug files
} 								# end of while($parent = shift...
# -----------------------------------------------------------------------
# end it.. take it all into whatever format is desired

  print "YOU ARE OUTPUTTING DATA!\n";
  &output_data;

# -----------------------------------------------------------------------
  &logit("Summary:");
  &logit("Plucked $page pages"); 
  $did_count = keys %did;
  &logit("$did_count links");
  $bad_count = keys %bad;
  &logit("$bad_count bad links");
  &logit(" END."); 
# -----------------------------------------------------------------------
# now that all parsing is complete, dump from memory (to whatever format...)
# all urls are in @documents, and each hash (doc) is named in @documents
#
# NOT IMPLEMENTED NOW...
#
# -----------------------------------------------------------------------

sub output_data {                     # currently to cache directory.. but can be anywhere.
my $output = "$cache_dir/$file";
open OUTPUT, ">$output";

printf OUTPUT '%8c',
	$blength, $blengthm,          # body length/256 and /256 modulus
	0, $offset_data,              # padding lsbs: (needed) (1byte) (2byte)
	0, 0,                         # height (used by viewer, not parser)
	$pplength, $pplengthm;        # prev para length/256, /256 modulus
printf OUTPUT '%s', $body;
my @html_types = ("text/html", "text/plain"); 	# what makes an html?
my @image_types = ("image/gif");		# what makes an image?
$$out{title} = $$parent{title}; 		# 
if (grep { /$type/ } @html_types) {
        $doc_type = 0;
        } elsif (grep { /$type/ } @image_types) {
        $doc_type = 1;
        } else {
        print "UNKNOWN TYPE: $type\n";
}
					# write the document header
printf OUTPUT '%20c',		# 20 bytes in the header
        0, 20,                  	# document header length
	$header_size1,$header_size2,	# doc hdr + (# of paras * 8) /256, %256
	$doc_text1, $doc_text2,		# uncomp size of all para text
        0, 0,				# Y-origin of document (i.e. top of document on screen)
        0, 20,				# Location of 1st para within document
        0, 0,				# Y-location of 1st visible para (on/off screen)
        0, 0,				# Location of last para in doc. (Calculated in viewer)
        0, 0,				# Y-location of last para (calculated in viewer)
        0, 0,				# Height of document (pixels - calculated in viewer)
        0, $doc_type;			# Content-type. 0=html, 1=image, 2=comp \
close OUTPUT;


}
# end of sub output_data

# -----------------------------------------------------------------------
# parse the raw url before it is plucked..
# convert characters, OS specific, ...
# -----------------------------------------------------------------------

sub fix_url {
	my $rawurl = shift;
	$rawurl =~ s/%7e/~/ig;     	# convert ascii tildes to real ones
	$rawurl =~ s/(.*?)\#\w*/$1/ig;	# strips fragments YUK!!!
	return $rawurl;
}
# -----------------------------------------------------------------------
# per RFC 2396 (and perldoc URI):
#
# <scheme>:<scheme-specific-part>#<fragment>
# <scheme>://<authority><path>?<query>#<fragment>
# <path>?<query>#<fragment>
# -----------------------------------------------------------------------
# debug output.. makes dirs in $debug_dir with the URL as the name of the file.. 
# i know its weird, but the only way to do it pre-parsing that is useful..
# -----------------------------------------------------------------------

sub debug {
my $out = shift;
$outgood = $out;
$outgood =~ s#/#_#g;
my $output = "$debug_dir/$outgood";
print "Debugging $out now!!! to ($output)\n";
open DEBUG, ">$output";
print DEBUG "URL: ", 		$$out{url},"\n";
print DEBUG "OK: ", 		$$out{ok},"\n";
print DEBUG "TITLE: ", 		$$out{title},"\n";
print DEBUG "PARENT: ", 	$$out{parent},"\n";
print DEBUG "DEPTH: ", 		$$out{depth},"\n";
print DEBUG "MAX_DEPTH: ", 	$$out{max_depth},"\n";
print DEBUG "AT MAX DEPTH: ", 	$$out{at_max},"\n";
print DEBUG "HEADERS: ", 	$$out{headers},"\n";
print DEBUG "TYPE: ", 		$$out{type},"\n";
print DEBUG "LENGTH: ", 	$$out{length},"\n";
print DEBUG "PLUCKED: ", 	$$out{plucked},"\n";
print DEBUG "CONTENT: ", 	$$out{content},"\n";
close DEBUG;
}

# -----------------------------------------------------------------------
# initial setup controlled from here
# -----------------------------------------------------------------------

sub setup {
if ($ARGV[0]) {
        $root_url = $ARGV[0];
        } else {
        $root_url = "$default_root";    # default.. home.html
}
push @PLUCKME, $root_url;                 # loads fist url (home.html)?
if ( $ARGV[1] ) {
        $$root_url{max_depth}   = $ARGV[1];     # how deep to drill (fix later..)
        } else {
        $$root_url{max_depth}   = 3;    # how deep to drill (fix later..)
}
if ( -e $debug_dir ) {
	unlink <$debug_dir/*>;
	print " I just removed all files from $debug_dir\n";
	} else {
	mkdir($debug_dir, 0755);
	print " I just created $debug_dir\n";
}
@tags_links 	= qw(a img); # what makes a valid link?
}

# -----------------------------------------------------------------------
# used to create log entries
# logit("this is an entry");
# -----------------------------------------------------------------------

sub logit {
my $entry = shift;
my $date_now = localtime();
open LOG, ">>$logfile";
print LOG $date_now, " $entry\n";
close LOG;
}

# -----------------------------------------------------------------------
# designed to only return a value if the mime type in the link is good(tm)
# ..that is, a match for elements in @good_mime_types
# -----------------------------------------------------------------------

sub parse_type {
my $type = shift;
@good_mime_types = ( 
	"text/html",    # good ole html
	"text/plain",   # plain text
	"image/gif",    # GIF
	"image/jpeg"    # JPEG
);
if (grep {/$type/ } @good_mime_types) {
	return $type;
	} else {
	return undef;
}
} # end of sub parse_type
# -----------------------------------------------------------------------
