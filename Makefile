#
# $Id: Makefile.in,v 1.38 2005/04/04 01:46:07 chrish Exp $
#

SHELL           =   /bin/sh

srcdir          =   .


DESTDIR         =

DOCS_DIR        =   docs
PARSER_DIR      =   parser
UNIX_DIR        =   unix
VIEWER_DIR      =   viewer
TOOLS_DIR       =   tools
DESKTOP_DIR     =   plucker_desktop
UNPLUCK_DIR     =   $(TOOLS_DIR)/unpluck
EXPLODE_DIR     =   $(TOOLS_DIR)/explode
GTKVIEWER_DIR   =   viewer-GTK+2-POSIX
CPLUCKER_DIR    =   $(PARSER_DIR)/c++

BINDIR          =   $(DESTDIR)/usr/local/bin
DOCDIR          =   $(DESTDIR)${prefix}/share/plucker/doc
DATADIR         =   $(DESTDIR)${prefix}/share/plucker

BUILD_PRC       =   yes
BUILD_MANUAL    =   no
BUILD_DESKTOP   =   yes
BUILD_UNPLUCK   =   yes
BUILD_EXPLODE   =   no
BUILD_GTKVIEWER =   no
BUILD_CPLUCKER  =   yes

MKINSTALLDIRS   =   ./mkinstalldirs
AUTOCONF        =   /usr/bin/autoconf


all: palmos_client pyplucker manual pluck_comics plucker-desktop unpluck explode gtkviewer cplucker

Makefile: Makefile.in config.status
	CONFIG_FILES=$@ $(SHELL) ./config.status

config.status: configure $(CONFIG_STATUS_DEPENDENCIES)
	$(SHELL) ./config.status --recheck

configure: configure.in $(CONFIGURE_DEPENDENCIES)
	$(AUTOCONF)

palmos_client:
	@if test "$(BUILD_PRC)" != "no"; then \
	    cd $(VIEWER_DIR) ; $(MAKE); \
	fi

pyplucker:
	cd $(PARSER_DIR) ; $(MAKE) $@;

manual:
	@if test "$(BUILD_MANUAL)" != "no"; then \
		cd $(DOCS_DIR) ; $(MAKE); \
	fi

pluck_comics:
	cd $(TOOLS_DIR) ; $(MAKE) $@

plucker-desktop:
	@if test "$(BUILD_DESKTOP)" != "no"; then \
		cd $(DESKTOP_DIR) ; $(MAKE) $@ ; \
	fi

unpluck:
	@if test "$(BUILD_UNPLUCK)" != "no"; then \
		cd $(UNPLUCK_DIR) ; $(MAKE) all ; \
	fi

explode:
	@if test "$(BUILD_EXPLODE)" != "no"; then \
		cd $(EXPLODE_DIR) ; $(MAKE) all ; \
	fi

gtkviewer:
	@if test "$(BUILD_GTKVIEWER)" != "no"; then \
		cd $(GTKVIEWER_DIR) ; $(MAKE) all ; \
	fi

cplucker:
	@if test "$(BUILD_CPLUCKER)" != "no"; then \
		cd $(CPLUCKER_DIR) ; $(MAKE) all ; \
	fi

install: install-palmos_client install-manual install-data install-pyplucker install-pluck_comics install-plucker-desktop install-unpluck install-explode install-gtkviewer install-cplucker

install-palmos_client: palmos_client
	$(MKINSTALLDIRS) $(DATADIR)/palm
	-cp $(VIEWER_DIR)/*.prc $(DATADIR)/palm

install-manual: manual
	cd $(DOCS_DIR) ; $(MAKE) install

install-data:
	$(MKINSTALLDIRS) $(BINDIR)
	cp $(UNIX_DIR)/setup.py $(BINDIR)/plucker-setup ; chmod 755 $(BINDIR)/plucker-setup
	$(MKINSTALLDIRS) $(DOCDIR)
	cp AUTHORS BUGREPORT COPYING CREDITS ChangeLog FAQ NEWS README REQUIREMENTS TODO $(DOCDIR)
	cd $(PARSER_DIR) ; $(MAKE) $@

install-pyplucker: pyplucker
	cd $(PARSER_DIR) ; $(MAKE) $@

install-pluck_comics: pluck_comics
	cd $(TOOLS_DIR) ; $(MAKE) install-pluck_comics

install-plucker-desktop: plucker-desktop
    @if test "$(BUILD_DESKTOP)" != "no"; then \
	    cd $(DESKTOP_DIR) ; $(MAKE) install \
    fi

install-unpluck: unpluck
	@if test "$(BUILD_UNPLUCK)" != "no"; then \
		cd $(UNPLUCK_DIR) ; $(MAKE) install ; \
	fi

install-explode: explode
	@if test "$(BUILD_EXPLODE)" != "no"; then \
		cd $(EXPLODE_DIR) ; $(MAKE) install ; \
	fi

install-gtkviewer: gtkviewer
	@if test "$(BUILD_GTKVIEWER)" != "no"; then \
		cd $(GTKVIEWER_DIR) ; $(MAKE) install ; \
	fi

install-cplucker: cplucker
	@if test "$(BUILD_CPLUCKER)" != "no"; then \
		cd $(CPLUCKER_DIR) ; $(MAKE) install ; \
	fi

clean-recursive:
	@if test "$(BUILD_MANUAL)" != "no"; then \
	    cd $(DOCS_DIR) ; $(MAKE) clean ; \
    fi
	cd $(PARSER_DIR) ; $(MAKE) clean
	@if test "$(BUILD_PRC)" != "no"; then \
	    cd $(VIEWER_DIR) ; $(MAKE) clean ; \
    fi
	cd $(TOOLS_DIR) ; $(MAKE) clean
	@if test "$(BUILD_DESKTOP)" != "no"; then \
	    cd $(DESKTOP_DIR) ; $(MAKE) clean ; \
    fi
	@if test "$(BUILD_GTKVIEWER)" != "no"; then \
	    cd $(GTKVIEWER_DIR) ; $(MAKE) clean ; \
    fi
	@if test "$(BUILD_CPLUCKER)" != "no"; then \
	    cd $(CPLUCKER_DIR) ; $(MAKE) clean ; \
    fi

distclean-recursive:
	@if test "$(BUILD_MANUAL)" != "no"; then \
	    cd $(DOCS_DIR) ; $(MAKE) distclean ; \
    fi
	cd $(PARSER_DIR) ; $(MAKE) distclean
	@if test "$(BUILD_PRC)" != "no"; then \
	    cd $(VIEWER_DIR) ; $(MAKE) distclean ; \
    fi
	cd $(TOOLS_DIR) ; $(MAKE) distclean
	@if test "$(BUILD_DESKTOP)" != "no"; then \
	    cd $(DESKTOP_DIR) ; $(MAKE) distclean ; \
    fi
	@if test "$(BUILD_GTKVIEWER)" != "no"; then \
	    cd $(GTKVIEWER_DIR) ; $(MAKE) distclean ; \
    fi
	@if test "$(BUILD_CPLUCKER)" != "no"; then \
	    cd $(CPLUCKER_DIR) ; $(MAKE) distclean ; \
    fi
	cd $(UNIX_DIR) ; rm -f setup.py

maintainer-clean-recursive:
	@if test "$(BUILD_MANUAL)" != "no"; then \
	    cd $(DOCS_DIR) ; $(MAKE) maintainer-clean ; \
    fi
	cd $(PARSER_DIR) ; $(MAKE) maintainer-clean
	@if test "$(BUILD_PRC)" != "no"; then \
	    cd $(VIEWER_DIR) ; $(MAKE) maintainer-clean ; \
    fi
	cd $(TOOLS_DIR) ; $(MAKE) maintainer-clean
	@if test "$(BUILD_DESKTOP)" != "no"; then \
	    cd $(DESKTOP_DIR) ; $(MAKE) maintainer-clean ; \
    fi
	@if test "$(BUILD_GTKVIEWER)" != "no"; then \
	    cd $(GTKVIEWER_DIR) ; $(MAKE) maintainer-clean ; \
    fi
	@if test "$(BUILD_CPLUCKER)" != "no"; then \
	    cd $(CPLUCKER_DIR) ; $(MAKE) maintainer-clean ; \
    fi
	cd $(UNIX_DIR) ; rm -f setup.py

clean: clean-recursive

distclean: clean distclean-recursive
	rm -f Makefile config.*
	rm -fr autom4te.cache

maintainer-clean: distclean maintainer-clean-recursive
	@echo "------------------------------------------------------------"
	@echo "This command is intended for maintainers to use..."
	@echo "it deletes files that may require special tools to rebuild."
	@echo "------------------------------------------------------------"
