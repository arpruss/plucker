#
# $Id: Makefile.in,v 1.3 2003/08/03 14:07:03 nordstrom Exp $
#

SHELL           =   /bin/sh

top_builddir    =   ..
subdir          =   cleaner
srcdir          =   .


VERSION         =   1.9
APP             =   cleaner
APPID           =   PlkC
RCP             =   $(APP).rcp
ICONTEXT        =   "CleanPlkr"
LANG            =   en
ICONDIR         =   $(srcdir)/icons
LANGDIR         =   $(srcdir)/langs


TYPE            =   @TYPE@

CFLAGS          =   -Werror -Wall -O2 

GCC             =   /usr/bin/m68k-palmos-gcc
OBJ_RES         =   /usr/bin/m68k-palmos-obj-res
AS              =   /usr/bin/m68k-palmos-as -m68000 -mno-68881 -l
MULTISEGSTUBGEN =   @MULTISEGSTUBGEN@
PILRC           =   /usr/local/bin/pilrc
BUILD_PRC       =   /usr/bin/build-prc

PRC             =   $(foreach lang, $(LANG), $(APP)_$(lang).prc)

CC              =   $(GCC) $(DEBUG) $(DEFS)

SOURCES         =   cleaner.c form.c data.c

OBJS            =   ${SOURCES:.c=.o}


all: cleaner_app

cleaner_app: $(PRC)

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

clean:
	rm -f $(OBJS) $(APP) *.stamp *.bin *.prc core *~ *.grc

distclean: clean
	rm -fr Makefile *.bak *.orig .depend tags .sedscr $(RCP)

maintainer-clean: distclean

depend: $(SOURCES)
	here=`pwd`; cd $(srcdir) && $(GCC) $(DEFS) -I$$here -MM $(SOURCES) > $$here/.depend

.depend: $(SOURCES)
	here=`pwd`; cd $(srcdir) && $(GCC) $(DEFS) -I$$here -MM $(SOURCES) > $$here/.depend

ifeq ($(TYPE),palmos-coff)
$(APP)_%.prc: code.stamp %.stamp
	$(BUILD_PRC) $@ $(ICONTEXT) $(APPID) *.$(APP).grc *.bin
else
$(APP)_%.prc: $(APP) %.stamp
	$(BUILD_PRC) -o $@ -n $(ICONTEXT) -c $(APPID) $(APP) *.bin
endif

code.stamp: $(APP)
	$(OBJ_RES) $(APP) $(SEGMENTDEF)
	@touch $@

%.stamp: $(RCP)
	$(PILRC) -q -I . -I $(srcdir) -I $(ICONDIR) -I $(LANGDIR)/$* -L $* $(RCP)
	@touch $@

$(RCP): $(RCP).in #$(LANGDIR)/*.rcp
	@echo "s:@@VIEWER_VERSION@@:$(VERSION):" > .sedscr
	@for lang in $(LANG); do\
		echo "5r $(LANGDIR)/$$lang.rcp" >> .sedscr; \
	done
	@echo "// Generated automatically from $(RCP).in" > $@
	@sed -f .sedscr $(srcdir)/$(RCP).in >> $@

$(APP): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@

-include .depend

