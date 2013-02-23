#
# $Id: Makefile.in,v 1.4 2003/08/03 14:07:01 nordstrom Exp $
#

SHELL           =   /bin/sh

top_builddir    =   ..
subdir          =   parser
srcdir          =   .


DESTDIR         =

PYTHONDIR       =   python
DEFAULTSDIR     =   defaults

DATADIR         =   $(DESTDIR)${prefix}/share/plucker

MKINSTALLDIRS   =   $(top_builddir)/mkinstalldirs

all: pyplucker

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

pyplucker:
	cd $(PYTHONDIR) ; $(MAKE)

install: install-data install-pyplucker

install-data:
	$(MKINSTALLDIRS) $(DATADIR)/config
	cp $(DEFAULTSDIR)/home.html $(DATADIR)/config
	cp $(DEFAULTSDIR)/exclusionlist.txt $(DATADIR)/config
	cp $(DEFAULTSDIR)/pluckerrc.sample $(DATADIR)/config

install-pyplucker: pyplucker
	cd $(PYTHONDIR) ; $(MAKE) install

clean-recursive:
	cd $(PYTHONDIR) ; $(MAKE) clean

distclean-recursive:
	cd $(PYTHONDIR) ; $(MAKE) distclean

maintainer-clean-recursive:
	cd $(PYTHONDIR) ; $(MAKE) maintainer-clean

clean: clean-recursive

distclean: clean distclean-recursive
	rm -f Makefile

maintainer-clean: distclean maintainer-clean-recursive
	@echo "------------------------------------------------------------"
	@echo "This command is intended for maintainers to use..."
	@echo "it deletes files that may require special tools to rebuild."
	@echo "------------------------------------------------------------"

