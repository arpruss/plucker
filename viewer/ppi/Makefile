#
# $Id: Makefile.in,v 1.3 2004/03/22 16:25:48 chrish Exp $
#

SHELL           =   /bin/sh

top_builddir    =   ..
subdir          =   ppi
srcdir          =   .


VERSION         =   1.9
APP             =   ppi
APPID           =   PlkP
RCP             =   $(APP).rcp
ICONTEXT        =   "PPI"
LANG            =   en de cs it fr ja fo da zh_CN pl ru es tr th ca no
ICONDIR         =   $(srcdir)/icons
LANGDIR         =   $(srcdir)/langs
RESOURCEFILE    =   $(APP).ro


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

SOURCES         =   ppi.c form.c

OBJS            =   ${SOURCES:.c=.o}


all: ppi_app

ppi_app: $(PRC)

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

clean:
	rm -f $(OBJS) $(APP) *.stamp *.prc core *~ *.ro

distclean: clean
	rm -fr Makefile *.bak *.orig .depend tags .sedscr $(RCP)

maintainer-clean: distclean

depend: $(SOURCES)
	here=`pwd`; cd $(srcdir) && $(GCC) $(DEFS) -I$$here -MM $(SOURCES) > $$here/.depend

.depend: $(SOURCES)
	here=`pwd`; cd $(srcdir) && $(GCC) $(DEFS) -I$$here -MM $(SOURCES) > $$here/.depend

$(APP)_%.prc: $(APP) %.stamp
	$(BUILD_PRC) -o $@ -n $(ICONTEXT) -c $(APPID) $(APP) $(RESOURCEFILE)

code.stamp: $(APP)
	$(OBJ_RES) $(APP) $(SEGMENTDEF)
	@touch $@

%.stamp: $(RCP)
	$(PILRC) -ro -q -I . -I $(srcdir) -I $(ICONDIR) -I $(LANGDIR)/$* -L $* $(RCP) $(RESOURCEFILE)
	@touch $@

$(RCP): $(RCP).in $(LANGDIR)/*.rcp
	@echo "s:@@VIEWER_VERSION@@:$(VERSION):" > .sedscr
	@for lang in $(LANG); do\
		echo "5r $(LANGDIR)/$$lang.rcp" >> .sedscr; \
	done
	@echo "// Generated automatically from $(RCP).in" > $@
	@sed -f .sedscr $(srcdir)/$(RCP).in >> $@

$(APP): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@

-include .depend

