#
# $Id: Makefile.in,v 1.4 2003/08/03 14:36:13 nordstrom Exp $
#

SHELL           = /bin/sh

top_builddir    = ..
subdir          = armlets
srcdir          = .


ARMLETS         = no
CFLAGS          = -Wall -Werror -nostartfiles -O2

GCC             = 

SOURCES         = search.c rotatebitmap.c
BINS            = ${SOURCES:.c=}

all: armlets

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

clean:
	rm -f $(BINS) code.stamp .depend

distclean: clean
	rm -f Makefile

armlets: code.stamp

ifeq ($(ARMLETS),no)
code.stamp:
else
-include .depend
code.stamp: $(BINS)
endif
	@touch $@

.depend: $(SOURCES)
	here=`pwd`; cd $(srcdir) && $(GCC) $(DEFS) -I$$here -MM $(SOURCES) | sed s/.o:/:/ > $$here/.depend

$(BINS):
	$(GCC) $(CFLAGS) -o $@ $@.c

