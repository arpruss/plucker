#
# $id$
#

SHELL           = /bin/sh

top_builddir    = ../..
subdir          = tools/pluck-comics
srcdir          =   .


VERSION         = 2005-02-07-cvs

DESTDIR         =

BINDIR          = $(DESTDIR)/usr/local/bin
COMICSLIST_DIR  = $(DESTDIR)${prefix}/share/plucker/comics
MODULE_DIR      = $(DESTDIR)/usr/lib/python2.5/site-packages/PyPlucker
PYTHONDIR       = /usr/lib/python2.5

PYTHON          = /usr/bin/python
MKINSTALLDIRS   = $(top_builddir)/mkinstalldirs


all:
	@echo "all done"

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

install:
	$(MKINSTALLDIRS) $(MODULE_DIR)
	$(MKINSTALLDIRS) $(COMICSLIST_DIR)
	cp pluck-comics.py $(MODULE_DIR)
	chmod 755 $(MODULE_DIR)/pluck-comics.py
	cp *.list $(COMICSLIST_DIR)

	$(PYTHON) $(PYTHONDIR)/compileall.py $(MODULE_DIR)
	$(PYTHON) -O $(PYTHONDIR)/compileall.py $(MODULE_DIR)

clean:
	find . -name '*~' -exec rm -f {} \;
	find . -name '*.pyc' -exec rm -f {} \;
	find . -name '*.pyo' -exec rm -f {} \;

distclean: clean
	rm -f Makefile pluck-comics.py

maintainer-clean: distclean
	@echo "------------------------------------------------------------"
	@echo "This command is intended for maintainers to use..."
	@echo "it deletes files that may require special tools to rebuild."
	@echo "------------------------------------------------------------"

