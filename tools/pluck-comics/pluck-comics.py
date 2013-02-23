#!/usr/bin/python

"""
pluck-comics.py  $Id: pluck-comics.py.in,v 1.6 2005/01/10 02:01:40 chrish Exp $

Gathers comics from selected websites for Plucker

Copyright (C) 2001, Christopher R. Hawks <chrish@syscon-intl.com>

Distributable under the GNU General Public License Version 2 or newer.
"""

import sys
import os
import re
import urllib
import getopt
import time
import string
import random


get_all = 0
make_home = 0
show_icons = 0
remove_all = 0
random_count = 0
spin_img = 0
nocache = 0
show_all = 0
verbose = 0

# Where to put the comics
location = '/var/spool/netcomics'
# Where libraries are stored
libdir = '${prefix}/share/plucker/comics'

# URL shortening utility
def shorten_url(line, length = 30):
	sline = line
	while len(sline) > length:
		parts = string.split(sline, '/')
		if len(parts) < 5:
			return sline
		if parts.count('...'):
			parts.remove('...')
		parts[-2:-1] = ['...']
#		parts[(len(parts) / 3) * 2] = '...'
		sline = string.join(parts, '/')
	return sline

# Comic dictionaries example 
#comic_list.append({
#	'name': 'name of comic for filename and selection',
#	'page': 'URL of the comic page',
#	'expr': "regex for the picture name",
#	'suff': 'suffix of image (.gif, .jpg, .etc)',
#	'base': 'URL where the pictures are stored _or_ cgi script (see popeye)',
#	'refr': 'URL of referring page (some King Features comics need this',
#	'title': 'Proper title of the comic'})

comic_list = []
random_imgs = []
for name in os.listdir(libdir):
	if name[-5:] == '.list':
		execfile(os.path.join(libdir,name))

now = time.localtime(time.time())
today = time.strftime("%Y-%m-%d", now)

(opts, args) = getopt.getopt(sys.argv[1:], "adhir:s:l:nSv")
for (opt, arg) in opts:
	if opt == "-a":
		get_all = 1
	if opt == "-d":
		remove_all = 1
	if opt == "-h":
		make_home = 1
	if opt == "-i":
		show_icons = 1
	if opt == "-r":
		random_count = int(arg) 
	if opt == "-s":
		spin_img = arg 
	if opt == "-l":
		location = arg
	if opt == "-n":
		nocache = 1
	if opt == "-S":
		show_all = 1
	if opt == "-v":
		verbose = 1

if remove_all:
	for name in os.listdir(location):
		os.remove(os.path.join(location,name))

if not args and not get_all and not make_home and not random_count:
	print "Usage: %s [-adhinSv] [-r x] [-s x] [-l location] comic [comic] ..." % sys.argv[0]
	print "\t-a Collect All known comics"
	print "\t-d Delete _all_ files in \'location\' (%s) first" % location
	print "\t-h Create a Home page (%s)" % os.path.join(location, 'index.html')
	print "\t-i Put Icons on home page"
	print "\t-l Location to store the comics"
	print "\t-n Bypass cache"
	print "\t-r Collect x number of Random comics"
	print "\t-s Spin the comic x degrees (uses imagemagick)"
	print "\t-S Show available comics"
	print "\t-v Verbose mode"

if random_count:
	for x in range(random_count):
		chosen = random.choice(comic_list)
		random_imgs.append(chosen['name'])

for get_me in comic_list:
	if get_all or get_me['name'] in args or get_me['name'] in random_imgs:
		try:
			if verbose:
				print "Fetching Page  %s" % shorten_url(get_me['page'], 60)
			path = urllib.URLopener()
			if nocache:
				path.addheader('pragma', 'no-cache')
			conn = path.open(get_me['page'])
			page = conn.read()
			conn.close()

			name = re.search(get_me['expr'], page)
			if name is not None:
				if verbose:
					print "Fetching Comic %s" \
							% shorten_url(get_me['base'] + name.group(), 60)
				if get_me.has_key('refr'):
					path.addheader('Referer', get_me['refr'])
				conn = path.open(get_me['base'] + name.group())
				if nocache:
					path.addheader('pragma', 'no-cache')
				comic = conn.read()
				conn.close()
				title = get_me['name'] + today
				output = open(os.path.join(location,title + get_me['suff']), 'wb')
				output.write(comic)
				output.close()
				if spin_img:
					filename = os.path.join(location,title + get_me['suff'])
					command = "convert -rotate %s %s %s" % (rotate_img, filename, filename)
					if verbose:
						print "Rotating Image %s degrees" % spin_img
					os.popen(command)

		except:
			if verbose:
				print "Comic %s failed!" % get_me['name']
			pass

if make_home:
	if verbose:
		print "Creating Home Page %s" % os.path.join(location, 'index.html')
	home = open(os.path.join(location, 'index.html'), 'w')
	home.write("<TITLE>Cartoons</TITLE>\n")
	home.write("<H1>Comics from the Web.</H1>\n")
	home.write("<P>")

	for name in os.listdir(location):
		if name != 'index.html':
			comic_date = name[-14:-4]
			comic_name = name[0:-14]
			for get_me in comic_list:
				if get_me['name'] == comic_name:
					comic_title = get_me['title']
			if show_icons:
				line = "<STRONG>%s</STRONG><BR />%s<BR /><IMG SRC=\"%s\"></P>\n<P>" % (comic_title, comic_date, name)
			else:
				line = "<A HREF=\"%s\"><STRONG>%s</STRONG></A><BR />%s</P>\n<P>" % (name, comic_title, comic_date)
			home.write(line)

	home.write('</P>\n<HR SIZE="1" /><P ALIGN="CENTER"><SMALL>Generated by pluck-comics<BR />\n%s</SMALL></P>' % today)
	home.close()

if show_all:
	print "\tAvailable comics are:"
	for list_me in comic_list:
		print '\t' + list_me['name']
