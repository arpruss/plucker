#
# $Id: Makefile.in,v 1.6 2003/08/03 14:07:02 nordstrom Exp $
#

SHELL           =   /bin/sh

top_builddir    =   ../..
subdir          =   tools/unpluck
srcdir          =   .


DESTDIR	        =

BINDIR          =   $(DESTDIR)/usr/local/bin
LIBDIR          =   $(DESTDIR)/usr/local/lib
DATADIR         =   $(DESTDIR)${prefix}/share/plucker

INSTALL         =   /usr/bin/install -c
INSTALL_PROGRAM =   ${INSTALL}
MKINSTALLDIRS   =   $(top_builddir)/mkinstalldirs

TARGET          =   libunpluck.a

RANLIB          =   ranlib

CC              =   gcc
CFLAGS          =   -g -O2 
CPPFLAGS        =    -DPLUCKER_VERSION="2005-02-07-cvs" -DPLUCKER_CONFIG_DIR="/usr/local/etc" -DSYS_CONFIG_FILE_NAME="pluckerrc" -DUSER_CONFIG_FILE_NAME=".pluckerrc" -DFILE_SEPARATOR_CHAR_S="/" -DOS_SECTION_NAME="posix"

SOURCES         =   unpluck.c config.c util.c

OBJECTS         =   ${SOURCES:.c=.o}

all: $(TARGET)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $(srcdir)/$*.c

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

$(TARGET): $(OBJECTS)
	$(AR) r $(TARGET) $(OBJECTS)
	$(RANLIB) $(TARGET)

depend: $(SOURCES)
	here=`pwd`; cd $(srcdir) && $(CC) -MM $(SOURCES) > $$here/.depend

.depend: $(SOURCES)
	here=`pwd`; cd $(srcdir) && $(CC) -MM $(SOURCES) > $$here/.depend

tags: $(SOURCES) *.h
	here=`pwd`; cd $(srcdir) && $(CTAGS) -o $$here/tags $(SOURCES) *.h


install:
	$(MKINSTALLDIRS) $(LIBDIR)
	-cp $(TARGET) $(LIBDIR)

clean:
	rm -f *.o core .depend $(TARGET)

distclean: clean
	rm -f Makefile

maintainer-clean: distclean
	@echo "------------------------------------------------------------"
	@echo "This command is intended for maintainers to use..."
	@echo "it deletes files that may require special tools to rebuild."
	@echo "------------------------------------------------------------"

