#
# $Id: Makefile.in,v 1.4 2003/08/03 14:07:02 nordstrom Exp $
#

SHELL           =   /bin/sh

top_builddir    =   ../../..
subdir          =   parser/python/PyPlucker

DESTDIR         =

PYTHONDIR       =   /usr/lib/python2.5
MODULE_DIR      =   $(DESTDIR)/usr/lib/python2.5/site-packages/PyPlucker

PYTHON          =   /usr/bin/python
MKINSTALLDIRS   =   $(top_builddir)/mkinstalldirs

all:
	@echo "all done in PyPlucker"

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

install:
	$(MKINSTALLDIRS) $(MODULE_DIR)/helper
	cp *.py $(MODULE_DIR) ; chmod 755 $(MODULE_DIR)/*.py
	cp helper/*.py $(MODULE_DIR)/helper ; chmod 755 $(MODULE_DIR)/helper/*.py

	$(PYTHON) $(PYTHONDIR)/compileall.py $(MODULE_DIR)
	$(PYTHON) -O $(PYTHONDIR)/compileall.py $(MODULE_DIR)

clean:
	@echo "all clean in PyPlucker"

distclean: clean
	rm -f Makefile __init__.py
