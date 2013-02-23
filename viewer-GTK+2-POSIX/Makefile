#
# $Id: Makefile.in,v 1.2 2003/08/03 14:07:04 nordstrom Exp $
#

SHELL           = /bin/sh

top_builddir    = ..
subdir          = viewer-GTK+2-POSIX

DESTDIR	        =

BINDIR          = $(DESTDIR)/usr/local/bin
DATADIR         = $(DESTDIR)${prefix}/share/plucker

UNPLUCK_DIR     = $(top_builddir)/tools/unpluck

LIBS            =   -lz -lz
CPPFLAGS        = -I$(UNPLUCK_DIR)  
CFLAGS          = -g -O2

INSTALL         = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
MKINSTALLDIRS   = $(top_builddir)/mkinstalldirs

TARGET          = plucker

SOURCES         = viewer.c image.c library.c

OBJECTS         = ${SOURCES:.c=.o}

all: $(TARGET)

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

$(TARGET): $(OBJECTS) $(UNPLUCK_DIR)/libunpluck.a
	$(CC) -o $(TARGET) $(OBJECTS) $(UNPLUCK_DIR)/libunpluck.a $(LIBS)

depend: $(SOURCES)
	$(CC) -MM $(SOURCES) >.depend

.depend: $(SOURCES)
	$(CC) -MM $(SOURCES) >.depend

tags: $(SOURCES) *.h
	$(CTAGS) $(SOURCES) *.h


install:
	$(MKINSTALLDIRS) $(BINDIR)
	$(INSTALL_PROGRAM) $(TARGET) $(BINDIR)

clean:
	rm -f *.o core .depend $(TARGET)

distclean: clean
	rm -f Makefile

maintainer-clean: distclean
	@echo "------------------------------------------------------------"
	@echo "This command is intended for maintainers to use..."
	@echo "it deletes files that may require special tools to rebuild."
	@echo "------------------------------------------------------------"

