#
# $Id: Makefile.in,v 1.9 2004/03/28 04:07:48 prussar Exp $
#

SHELL           = /bin/sh

top_builddir    = ..
subdir          = xlit
fonts_dir	= $(top_builddir)/fonts
srcdir          = .


TOPRC           = $(fonts_dir)/toprc

PERL		= /usr/bin/perl

BINS            = Latin1.bin NonASCIIToPeriod.bin PBGreekKeys.bin CP1250.bin Latin2.bin

DOCS		= Latin1.html NonASCIIToPeriod.html PBGreekKeys.html CP1250.html Latin2.html

all: plucker-xlit.pdb

docs: $(DOCS)

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

clean:
	rm -f *.bin *.pdb $(TOPRC).exe $(TOPRC)

distclean: clean
	rm -f Makefile

plucker-xlit.pdb: $(BINS) $(TOPRC)
	$(TOPRC) -d plucker-xlit.pdb "Plkr-Xlit-default" Xlit Plkr $(BINS)

%.html: %.xlit xlit2bin.pl
	$(PERL) xlit2bin.pl html < $*.xlit > $*.html
	
%.bin: %.xlit xlit2bin.pl
	$(PERL) xlit2bin.pl < $*.xlit > $*.bin
