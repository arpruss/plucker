#
# $Id: Makefile.in,v 1.5 2003/08/03 14:07:02 nordstrom Exp $
#

SHELL		=	/bin/sh

top_builddir	=	..
subdir		=	tools
srcdir          =   .


DESTDIR		=

PLUCK_COMICSDIR	=	pluck-comics
EXPLODEDIR      =   explode
UNPLUCKDIR      =   unpluck

MKINSTALLDIRS	=	$(top_builddir)/mkinstalldirs

all: pluck_comics unpluck_lib explode_app

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

pluck_comics:
	cd $(PLUCK_COMICSDIR) ; $(MAKE)

explode_app:
	cd $(EXPLODEDIR) ; $(MAKE)

unpluck_lib:
	cd $(UNPLUCKDIR) ; $(MAKE)

install: install-pluck_comics install-explode_app install-unpluck_lib

install-pluck_comics:
	cd $(PLUCK_COMICSDIR) ; $(MAKE) install

install-explode_app:
	cd $(EXPLODEDIR) ; $(MAKE) install

install-unpluck_lib:
	cd $(UNPLUCKDIR) ; $(MAKE) install

clean-recursive:
	cd $(PLUCK_COMICSDIR) ; $(MAKE) clean
	cd $(EXPLODEDIR) ; $(MAKE) clean
	cd $(UNPLUCKDIR) ; $(MAKE) clean

distclean-recursive:
	cd $(PLUCK_COMICSDIR) ; $(MAKE) distclean
	cd $(EXPLODEDIR) ; $(MAKE) distclean
	cd $(UNPLUCKDIR) ; $(MAKE) distclean

maintainer-clean-recursive:
	cd $(PLUCK_COMICSDIR) ; $(MAKE) maintainer-clean
	cd $(EXPLODEDIR) ; $(MAKE) maintainer-clean
	cd $(UNPLUCKDIR) ; $(MAKE) maintainer-clean

clean: clean-recursive

distclean: clean distclean-recursive
	rm -f Makefile

maintainer-clean: distclean maintainer-clean-recursive
	@echo "------------------------------------------------------------"
	@echo "This command is intended for maintainers to use..."
	@echo "it deletes files that may require special tools to rebuild."
	@echo "------------------------------------------------------------"

