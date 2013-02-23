#!/usr/bin/perl
#
# THIS IS NON-STABLE BETA CODE
#
#######################################
# Note: certain changes will be needed for win32/other compatibility, like binmode i
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
# Pat Trainor <ptrainor@title14.com>
#
# output:
#
# plucking [405P/861p/611c] n32 (3d/3m) http://home.global.co.za/~kitiara/tsasa/
#            1.   2.   3.    4.  5. 6.                   7.
# where:
#
# 1. parent of the page currently being parsed
# 2. current value of $page (which decides the record number of a document)
# 3. current number of @CHECK (the number of pages left to pluck)
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
# Cannot fetch http://lisa.title14.com/random/Weekly%20Status%204b.doc (status 200 [application/msword] OK)
#                                    1.                                    2.                3.         4.
# where:
#
# 1. link that was not a) successfully returned from server OR b) not a valid 'type' (see &parse_type) 
# 2. status returned from server 
# 3. MIME type
# 4. response message
#
##############
#
# the usual suspects:
#
use HTML::LinkExtor;
use LWP::UserAgent;
use HTML::Parser;
use HTML::TokeParser;
use URI::URL;
#
# -----------------------------------------------------------------------
# Initial setup
# -----------------------------------------------------------------------

my $verbose             = 0;
my $pluckerhome         = "$ENV{HOME}/.plucker";
my $debug_dir           = "$pluckerhome/.debug";

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

$| = 1; # flush STDOUT

&logit("---------------- START of logfile on $localdate");
&setup;

# -----------------------------------------------------------------------
MAIN:
while ($parent = shift @CHECK) {
	$parent = &fix_url($parent); 				# fix up the URL itself as necessary
	############# these tests should be consolidated later ############
	if ($did{$parent}++) {     				# builds visited urls keys, # times=value
		&logit("\n\tDuplicate Content:\n\tIgnored [$did{$parent}] $parent\n\n");
		###warn "DUPE url: $parent\n";
		next MAIN; # Add more here to direct parent to original (sole) content
	}
	if ($bad{$parent}) { # don't bother, its bad(tm)...
                &logit("BAD $parent");
		###warn "BAD url: $parent\n";
                next MAIN; 					# restart loop, its BAD(tm)!
	}
	if ($parent ne "$root_url" && ($$parent{depth} >= ($$parent{max_depth} + 1))) {
		&logit("MAX DEPTH [$$parent{depth}] $parent");
		###warn "MAX DEPTH [$$parent{depth}\/$$parent{max_depth}] url: $parent\n";
		next MAIN;
	} 
	# the above tests should be in this order: dupe, bad, max_depth
	###################################################################
	# this is where you start to work with a valid parent..
	push @docs, $parent;			# needed to know hash names when writing DB
	$num_docs 		= @docs;	# how many docs are in the array?
	$check_count 		= @CHECK;	# how many docs have yet to be checked?
	$$parent{url} 		= $parent; 	# set url
	$$parent{paras} 	= 1;		# all docs have at least 1 paragraph
	if ($page ==  $start_id) { 		# $parent is home.html, or ...
		$$parent{depth}		= "0";
		$$parent{title} 	= "Plucker Home Page";
		$$parent{parent} 	= $page; 
		$$parent{id}		= $page; 
		$$parent{at_max}	= "N"; 
	} 
	warn "plucking [", $$parent{parent}, "\P\/$page\p/", $check_count,"c] doc [$num_docs] ($$parent{depth}d\/$$parent{max_depth}m) $parent\n";
	# legend: P = parent, p = pageID, c = # of @CHECK elements, 
	# n = number of docs in @docs, d = parent depth, m = parent max depth
	$request = new HTTP::Request(GET,$parent); # build request
	$response = $ua->request($request); 	# Pluckit!
	if ($response->is_success && ($parent_type = &parse_type($response->content_type))) {
		$$parent{type} 			= $parent_type; 	# get/set type 
		$$parent{plucked}		= "Y"; 			# get/set type 
		my $everything 			= $response->as_string;	# COMPLETE response 
		($headers = $everything) 	=~ s#(.*?)\n\n.*#$1#s; 	# pull headers
		$$parent{headers}		= $headers; 		# store headers
		($body = $everything) 		=~ s#.*?\n\n(.*)#$1#s; 	# pull body
		$base 				= $response->base; 	# base url for $parent
		$$parent{ok}			= "Y";			# success!
		} else {
		warn "Cannot fetch $parent (status ", $response->code, " [", $response->content_type, "] ", $response->message,")\n";
		$$parent{ok}			= "N";			# may need status later..
		$bad{$parent}++; 					# its bad(tm)...
		next MAIN;						# no use staying here..
	} 
	# so, you have an image.. 1 paragraph will do..
	if ($parent_type =~ /image/) {
		$$parent{content} = $body;
		($$parent{length} = $headers) =~ s#.*?Content-Length: (.*?)\n.*#$1#s;
		&write_debug($parent); 					# for debug, write to the $crawler_dir
		next MAIN;						# move along... no need to parse
	}
	# you are in a real document now... ready to parse it.. (look for links and rewrite tags)
	my $p = HTML::TokeParser->new(\$body); 		# load $body up!
	# a word about HTML::Tokeparser...
	#   0     1      2         3         4 	 	$token->[?]
	# ["S", $tag, %$attr, @$attrseq, $origtext]	# Start
	# ["E", $tag, $origtext]			# End
	# ["T", $text]					# Text
	# ["C", $text]					# Comment
	# ["D", $text]					# Declaration
	# put any tags you want converted to text here in this hash (lc) 
	# and they will be output (say, applet) as: [APPLET] 
	%$p->{textify} 				= {applet => "alt"};
	my ($text, $parsed_body);
	$para_count = 0;
	while (my $token = $p->get_token) { 			# traverse body
		my $rel_url;					# used with $base to make an absolute url
		my $tag_id   		= $token->[0]; 		# [S,E,T,C,D] see HTML::TokeParser
		my $tag_text 		= $token->[1]; 		# href, img, ...
		if ($tag_id eq "S"){ 				# Start tag
###			print "S: $tag_text\n";
			if (grep { /\A$tag_text\Z/ } @tags_links) { # hyperlinks 
				my $key;
				foreach $key ( keys %{$token->[2]} ) {
###					print "doing \'$key\'\n";
					if ( $key =~ /href|src/ ) {
						$rel_url = %$token->[2]{$key};
###						print "\tKEY: \'$key\' VALUE: \'$rel_url\' ";
						} elsif ( $key =~ /maxdepth/i ) {
                        				my $child_url = URI->new_abs($rel_url, $base);
							$child_max_depth = %$token->[2]{$key};
						} else {
						# add more here...
###						print "I don't know how to handle: \'$key \= ", %$token->[2]{$key}, "\'\n";
					}
				}	
				my $child               = fix_url(URI->new_abs($rel_url, $base));
                        	$child			= "$child";
				if ($did{$child}){
					foreach $d (keys %{$child} ) {
						next unless $d eq "id";
						print "$d is: ", ${$child}{$d}, "\n";
					}
					print "Dupe: ($$parent{id}p\->$$child{id}\o) $child\n";
					# p = parent o = original(only) content
					# point this page to the existing link ($$child{id})
					# when we write the binaries here...
					# -this is really nice, as it will even point to content that
					# hasn't been plucked yet, but is in the @CHECK queue!!!
					next MAIN; # no use in overwriting %$child, it exists already
				}
                        	$$child{url}	= $child;
				$page++;				# make a new file/record id
                        	$$child{id}	= $page;		# set new id
                        	$$child{parent}	= $$parent{id};		# who loves ya..
                        	if (!$$parent{depth} && $parent ne "$root_url") {
                        	        &logit("NO PARENT LINK_DEPTH: [$$parent{id}] $parent");
                        	        # should NEVER be true... remove when code calms down..
                        	}
				$$child{depth}	= $$parent{depth} + 1; 	# max + 1
				if ( $child_max_depth ) {
					$$child{max_depth} = $child_max_depth;
					} else {
                        		$$child{max_depth}	= $$parent{max_depth}; # clone max_depth
				}
				$$child{at_max} = "Y" if $$child{depth} >= $$child{max_depth};
                        	push @CHECK, $child;
				# if not a hypertext link, then what to do with it?
				} elsif (grep { /\A$tag_text\Z/ } @tags_crs ) { # carriage returns
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } @tags_para ) { # make paragraphs
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } @tags_ignore ) {	# ignore these..
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } @tags_1 ) { 		# size=1
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } @tags_2 ) { 		# size=2
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } @tags_3 ) { 		# size=3
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } @tags_4 ) { 		# size=4
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } @tags_5 ) { 		# size=5
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } @tags_6 ) { 		# size=6
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } @tags_7 ) { 		# size=7
					$text .= "<\_$tag_text\_>"; # well, for debugging..
				} elsif (grep { /\A$tag_text\Z/ } "title" ) { 		# page title 
					$text .= "<\_$tag_text\_>"; # well, for debugging..
					unless ($$parent{title}){
						#$parent_title = $p->get_trimmed_text; 	# use page's
						#$$parent{title} = $parent_title; 	# title if none
						#$p->unget_token("title");		#
					}
				} else {
###				print ">->->-> I don't know how to handle: $tag_text\n";
			}	
			} elsif ($tag_id eq "E") {		# end tag
				my $origtext = $token->[2];
				$text .= "<\_\/$tag_text\_>"; # well, for debugging..
###				print "E: \'$tag_text\' \'$origtext\'\n";
				#$text .= $origtext; # adds END tokens

			} elsif ($tag_id eq "T") {		# text
				$text .= "_\_$tag_text\__" unless !$tag_text; # well, for debugging..
				my $origtext = $token->[2];
###				print "T: \'$tag_text\' \'$origtext\'\n";

			} elsif ($tag_id eq "C") { 		# comment
				$text .= "<\_$tag_text\_>"; # well, for debugging..
				my $origtext = $token->[2];
###				print "C: \'$tag_text\' \'$origtext\'\n";

			} elsif ($tag_id eq "D") {		# declaration
				$text .= "<\_$tag_text\_>"; # well, for debugging..
				my $origtext = $token->[2];
###				print "D: \'$tag_text\' \'$origtext\'\n";

			} else { 				# shouldn't get here...
###			print "W A R N I N G ! LEFTOVER TAGID: $tag_id TEXT: $text\n";
		} 						# end of if (tag_id eq "S"...
		$text 		.= $token->[4]; 		# add ORIGINAL TAGS to $text
		#$text 		.= $p->get_trimmed_text; 	# text n/associated with tokens
	} # end of while (my $token...
	$text 				=~ s#[\n\r]##sg; 	# remove source \n\r's
	my $parsed_text 		.= "$text "; 		# adds ONLY text between tags!!!
	$$parent{length}		= length($parsed_text);	# will need later..
	$$parent{content}		= cleanup($parsed_text);# load content
	&write_debug($parent); 					# for debug, write to the $crawler_dir
} 								# end of while($parent = shift...
#
#########
#
# end it..
#
&output_data; 		# take it all into whatever format is desired
#
###########
&logit("Summary:");
&logit("Plucked $page pages"); 
$did_count = keys %did;
&logit("$did_count links");
$bad_count = keys %bad;
&logit("$bad_count bad links");
&logit("------------- END."); 
#
#--------------------------------------------------------------whew!---------------#
#
######################
#
# now that all parsing is complete, dump from memory (to whatever format...)
# all urls are in @documents, and each hash (doc) is named in @documents
#
sub output_data {		# currently to cache directory.. but can be anywhere..
#forearch $doc (@documents) { 	# step through each document


printf OUTPUT '%8c',
	$blength, $blengthm,	# body length/256 and /256 modulus
	0, $offset_data,	# padding lsbs: (needed) (1byte) (2byte)
	0, 0,			# height (used by viewer, not parser)
	$pplength, $pplengthm;	# prev para length/256, /256 modulus
printf OUTPUT '%s', $body;
my @html_types = ("text/html", "text/plain"); 	# what makes an html?
my @image_types = ("image/gif");		# what makes an image?
$$out{title} = $$parent{title}; 		# 
my $output = "$cache_dir/$file";
open OUTPUT, ">$output";
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


###} # end of foreach $doc ...
} # end of sub output_data
#
############
#
# parse the raw url before it is plucked..
#
sub fix_url {
	my $rawurl = shift;
	$rawurl =~ s/%7e/~/ig;     	# convert ascii tildes to real ones
	$rawurl =~ s/(.*?)\#\w*/$1/ig;	# strips fragments YUK!!!
	return $rawurl;
}
#
# per RFC 2396 (and perldoc URI):
#
# <scheme>:<scheme-specific-part>#<fragment>
# <scheme>://<authority><path>?<query>#<fragment>
# <path>?<query>#<fragment>
#
##########
#
# write files (html)
#
sub write_debug {
my $out = shift;
$$out{title} = $$parent{title}; # I have no idea why this works... :(
my $output = "$crawler_dir/$$out{id}";
open DEBUG, ">$output";
print DEBUG "URL: ", 		$$out{url},"\n";
print DEBUG "OK: ", 		$$out{ok},"\n";
print DEBUG "TITLE: ", 		$$out{title},"\n";
print DEBUG "ID: ", 		$$out{id},"\n";
print DEBUG "PARENT: ", 	$$out{parent},"\n";
print DEBUG "DEPTH: ", 		$$out{depth},"\n";
print DEBUG "MAX_DEPTH: ", 	$$out{max_depth},"\n";
print DEBUG "AT MAX DEPTH: ", 	$$out{at_max},"\n";
print DEBUG "HEADERS: ", 	$$out{headers},"\n";
print DEBUG "TYPE: ", 		$$out{type},"\n";
print DEBUG "LENGTH: ", 	$$out{length},"\n";
print DEBUG "PLUCKED: ", 	$$out{plucked},"\n";
print DEBUG "PARAGRAPHS: ",	$$out{paras},"\n";
print DEBUG "CONTENT: ", 	$$out{content},"\n";
close DEBUG;
}
#
##########
#
# initial setup controlled from here
#
sub setup {
&get_roots;
if ( -e $crawler_dir ) {
	unlink <$crawler_dir/*>;
	print " I just removed all files from $crawler_dir";
	} else {
	mkdir($crawler_dir, 666);
	print " I just created $crawler_dir";
}
if ( -e $cache_dir ) {
	unlink <$cache_dir/*>;
	print " and I just removed all files from $cache_dir\n";
	} else {
	mkdir($cache_dir, 666);
	print " and I just created $cache_dir\n";
}
@tags_links 	= qw(a img);
@tags_crs 	= qw(ul li table th p br tr h1 h2 h3 h4 h5 h6 hr);
@tags_para 	= qw(ul p h1 h2 h3 h4 h5 h6 hr);
@tags_ignore	= qw(b i ul head body html frame frameset center style meta);
@tags_1		= qw(h1);
@tags_2		= qw(h2);
@tags_3		= qw(h3);
@tags_4		= qw(h4);
@tags_5		= qw(h5);
@tags_6		= qw(h6);
@tags_7		= qw(b);
}
#
##########
#
# determine root urls, and depth. replace with whatever will parse a config
# file like home.html...
# usage: &get_roots
#
sub get_roots {
if ($ARGV[0]) {
        $root_url = $ARGV[0];
        } else {
        $root_url = "$default_root"; 	# default.. home.html
}
push @CHECK, $root_url;			# loads fist url (home.html)?
if ( $ARGV[1] ) {
	$$root_url{max_depth}	= $ARGV[1];  	# how deep to drill (fix later..)
	} else {
	$$root_url{max_depth}	= 3;  	# how deep to drill (fix later..)
}
} # end-o-sub
#
###########
#
# mostly useful for logging, this returns a nice human date
# $now = getdate();
#
sub get_date {
	my $date_now = localtime();
	return $date_now;
}
#
###########
#
# used to create log entries
# logit("this is an entry");
#
sub logit {
my $entry = shift;
open LOG, ">>$logfile";
print LOG &get_date, " $entry\n";
close LOG;
}
#
###########
#
# designed to only return a value if the mime type in the link is good(tm)
# ..that is, a match for elements in @good_mime_types
#
sub parse_type {
my $type = shift;
@good_mime_types = ( 
"text/html",
"text/plain",
"image/gif"
);
if (grep {/$type/ } @good_mime_types) {
	return $type;
	} else {
	return undef;
}
} # end of sub parse_type
#
###########
#
# print out everything about the entry at that point
# degug($parent), i.e.
#
sub debug {
my $debug = shift;
print "=" x 70, "\n";
print "URL: ",           $$debug{url},"\n";
print "OK: ",            $$debug{ok},"\n";
print "TITLE: ",         $$debug{title},"\n";
print "ID: ",            $$debug{id},"\n";
print "PARENT: ",        $$debug{parent},"\n";
print "DEPTH: ",         $$debug{depth},"\n";
print "MAX_DEPTH: ",     $$debug{max_depth},"\n";
print "HEADERS: ",       $$debug{headers},"\n";
print "TYPE: ",          $$debug{type},"\n";
print "LENGTH: ",        $$debug{length},"\n";
print "PLUCKED: ",       $$debug{plucked},"\n";
print "CONTENT: ",       $$debug{content},"\n";
print "=" x 70, "\n";
}
#
############
#
# clean out weird characters -ugh..
#
sub cleanup {
my $rawtext = shift;
my %oddballs = (
'&nbsp;' => 1,
'&copy;' => 1,
'&uuml;' => 1,
'&Uuml;' => 1,
'&ouml;' => 1,
'&Ouml;' => 1,
'&auml;' => 1,
'&Auml;' => 1,
'&szlig;' => 1,
'&^;' => 1,
'&;;' => 1
);
return $rawtext;
}
#
############
