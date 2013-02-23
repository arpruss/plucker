#
# $Id: Makefile.in,v 1.119 2006/11/12 02:00:53 prussar Exp $
#

SHELL           =   /bin/sh

top_builddir    =   .
srcdir          =   .


ICONTEXT        =   "Plucker"
APPID           =   Plkr
SECTIONS        =   "sec1 sec2 sec3 sec4 sec5 sec6"

# setup test environment
ifneq (,)
ICONTEXT        =   "PluckerTest"
APPID           =   UntT
PALMCUNITRO     =   PalmCUnit.ro
PALMCUNITHDR    =   PalmCUnitRsc.h
UNITTESTDIR     =   test
MOCKOBJECTSDIR  =   $(UNITTESTDIR)/mockobjects/
PALMCUNITDIR    =   
PALMCUNITRCP    =   $(PALMCUNITDIR)/PalmCUnit.rcp
SECTIONS       +=   "unit mock framewrk"
endif

VERSION         =   1.9
APP             =   viewer
RCP             =   $(APP).rcp
RESOURCEFILE    =   $(APP).ro
CATEGORY        =   
LANG            =   en de cs it fr ja fo da zh_CN pl ru es tr th ca no
ARMLETDIR       =   $(srcdir)/armlets
FONTDIR         =   $(srcdir)/fonts
ICONDIR         =   $(srcdir)/icons
SCRIPTDIR       =   $(srcdir)/scripts
LANGDIR         =   $(srcdir)/langs
XLITDIR		=   $(srcdir)/xlit

IMODEICONSPDB   =   imodeicons.pdb
IMODEDIR        =   imode
PALMCOLOR8      =   palmcolor8.map

CLEANERDIR      =   cleaner
PPIDIR          =   ppi

WAIT_ICON       =   bubble

TYPE            =   @TYPE@
DEBUG_LOG       =   

SEGMENTDEF      =   Plucker.def
MULTISEG_LD     =   Plucker-sections.ld
MULTISEG_S      =   Plucker-sections.s

SDK_CFLAGS      =   
ifneq (,)
SDK_CFLAGS     +=   -I$(PALMCUNITDIR) -I$(UNITTESTDIR) -I$(MOCKOBJECTSDIR)
endif

DEBUG           =   -g 
DEFS            =   -DNEWGCC  -DERROR_CHECK_LEVEL=ERROR_CHECK_NONE -DHAVE_CONFIG_H $(DEBUG_LOG)
CFLAGS          =   -Werror -Wall -O2 -isystem .  $(SDK_CFLAGS)
ifneq (,)
CFLAGS         +=   -Wno-unused
endif
LIBS            =   -lPalmOSGlue

GCC             =   /usr/bin/m68k-palmos-gcc
OBJ_RES         =   /usr/bin/m68k-palmos-obj-res
AS              =   /usr/bin/m68k-palmos-as -m68000 -mno-68881 -l
MULTIGEN        =   /usr/bin/m68k-palmos-multigen
PILRC           =   /usr/local/bin/pilrc
BUILD_PRC       =   /usr/bin/build-prc
CTAGS           =   /usr/bin/ctags
AUTOCONF        =   /usr/bin/autoconf
LN_S            =   ln -s
PERL            =   /usr/bin/perl
CPP             =   /usr/bin/m68k-palmos-gcc -E -x c

PRC             =   $(foreach lang, $(LANG), $(APP)_$(lang).prc)

CC              =   $(GCC) $(DEBUG) $(DEFS)

SOURCES         =   viewer.c os.c screen.c metadocument.c doclist.c control.c \
                    bookmarkform.c resultform.c emailform.c libraryform.c \
                    timeout.c util.c externalform.c mainform.c statusform.c \
            	    prefsform.c prefsfont.c prefscolor.c \
                    prefsgeneral.c prefsbrowsing.c prefslibrary.c \
                    prefsautoscroll.c prefshardcopy.c prefsbutton.c \
                    prefscontrol.c prefsgesture.c loadbar.c \
                    ramfile.c genericfile.c vfsfile.c cache.c \
                    detailsform.c searchform.c categoryform.c \
                    bookmark.c session.c document.c image.c history.c \
                    search8.c search.c prefsdata.c anchor.c \
                    paragraph.c uncompress.c keyboard.c keyboardform.c \
                    list.c link.c renamedocform.c hardcopyform.c font.c \
                    table.c fullscreenform.c annotation.c annotationform.c \
                     hires.c rotate.c rotatebitmap.c grayfont.c prefslookup.c xlit.c DIA.c resize.c

PALMCUNITSRC    =   $(PALMCUNITDIR)/TestRun.c $(PALMCUNITDIR)/TestDB.c \
                    $(PALMCUNITDIR)/AssertImpl.c $(PALMCUNITDIR)/Util.c \
                    $(PALMCUNITDIR)/PalmCUnit.c

TESTSRC         =   testsuite.c \
                    $(UNITTESTDIR)/testutil.c \
                    $(UNITTESTDIR)/listtester.c \
                    $(UNITTESTDIR)/historytester.c \
                    $(UNITTESTDIR)/externalformtester.c \
                    $(UNITTESTDIR)/ostester.c \
                    $(UNITTESTDIR)/prefsdatatester.c \
                    $(UNITTESTDIR)/metadocumenttester.c \
                    $(UNITTESTDIR)/genericfiletester.c \
                    $(UNITTESTDIR)/uncompresstester.c \
                    $(UNITTESTDIR)/paragraphtester.c \
                    $(UNITTESTDIR)/fonttester.c \
                    $(UNITTESTDIR)/vfsfiletester.c

MOCKSRC         =   $(MOCKOBJECTSDIR)/mock_Form.c \
                    $(MOCKOBJECTSDIR)/mock_Feature.c \
                    $(MOCKOBJECTSDIR)/mock_Device.c \
                    $(MOCKOBJECTSDIR)/mock_Window.c \
                    $(MOCKOBJECTSDIR)/mock_VFS.c \
                    $(MOCKOBJECTSDIR)/mock_Database.c


OBJS            =   ${SOURCES:.c=.o} ${MULTISEG_S:.s=.o}
DEPENDSOURCES   =   $(SOURCES)
ifneq (,)
DEPENDSOURCES  +=   $(TESTSRC) $(MOCKSRC)
OBJS           +=   $(PALMCUNITSRC:.c=.o) ${TESTSRC:.c=.o} ${MOCKSRC:.c=.o}
endif

ifeq (no,yes)
ARM_BFDEXES     =   $(ARMLETDIR)/search 
endif
ALL_BFDEXES     =   $(APP) $(ARM_BFDEXES)

all: cleaner_app viewer_app ppi_app xlit_data

viewer_app: armletdata $(PALMCUNITHDR) $(PRC)

xlit_data:
	@cd $(XLITDIR) ; $(MAKE)

cleaner_app:
	@cd $(CLEANERDIR) ; $(MAKE)

ppi_app:
	@cd $(PPIDIR) ; $(MAKE)

install: all

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$@ $(SHELL) ./config.status

config.status: configure $(CONFIG_STATUS_DEPENDENCIES)
	$(SHELL) ./config.status --recheck

configure: configure.in $(CONFIGURE_DEPENDENCIES)
	here=`pwd`; cd $(srcdir) && $(AUTOCONF)

test:
	@cd $(LANGDIR) ; $(MAKE) test

clean:
	@cd $(PPIDIR) ; $(MAKE) clean
	@cd $(CLEANERDIR) ; $(MAKE) clean
	@cd $(LANGDIR) ; $(MAKE) clean
	@cd $(FONTDIR) ; $(MAKE) clean
	@cd $(ARMLETDIR) ; $(MAKE) clean
	@cd $(XLITDIR) ; $(MAKE) clean
	rm -f $(OBJS) $(APP) $(PALMCUNITHDR) *.stamp *.prc *.ro core *~ $(MULTISEG_LD) $(MULTISEG_S) $(MULTISEG_S:.s=.o) $(RCP) $(RCP).s $(RCP).s0

distclean: clean
	@cd $(PPIDIR) ; $(MAKE) distclean
	@cd $(CLEANERDIR) ; $(MAKE) distclean
	@cd $(LANGDIR) ; $(MAKE) distclean
	@cd $(FONTDIR) ; $(MAKE) distclean
	@cd $(XLITDIR) ; $(MAKE) distclean
	@cd $(ARMLETDIR) ; $(MAKE) distclean
	rm -f Makefile *.bak *.orig .depend tags .sedscr
	rm -f config.status config.cache config.log config.h 
	rm -rf sony_sdk
	rm -f $(SEGMENTDEF)
	rm -fr autom4te.cache
	rm -f $(IMODEDIR)/*.tbmp

maintainer-clean: distclean
	@echo "------------------------------------------------------------"
	@echo "This command is intended for maintainers to use..."
	@echo "it deletes files that may require special tools to rebuild."
	@echo "------------------------------------------------------------"
	@cd $(CLEANERDIR) ; $(MAKE) maintainer-clean
	@cd $(PPIDIR) ; $(MAKE) maintainer-clean
	rm -f config.h.in
	rm -f $(IMODEICONSPDB)

depend: $(DEPENDSOURCES) $(PALMCUNITHDR)
	here=`pwd`; cd $(srcdir) && $(GCC) $(DEFS) -I$$here $(SDK_CFLAGS) -MM $(DEPENDSOURCES) > $$here/.depend

.depend: $(DEPENDSOURCES) $(PALMCUNITHDR)
	here=`pwd`; cd $(srcdir) && $(GCC) $(DEFS) -I$$here $(SDK_CFLAGS) -MM $(DEPENDSOURCES) > $$here/.depend


tags: $(SOURCES) $(srcdir)/*.h
	here=`pwd`; cd $(srcdir) && $(CTAGS) -o $$here/tags $(SOURCES) *.h

armletdata:
	@( cd $(ARMLETDIR) ; $(MAKE) )

$(IMODEICONSPDB): $(IMODEDIR)/*.pnm
	@for pnmfile in $(IMODEDIR)/*.pnm ; do \
		tbmpfile=`echo $$pnmfile | sed 's/\.pnm/.tbmp/'` ; \
		@PPMQUANT@ -map $(PALMCOLOR8) $$pnmfile | @PNMTOPALM@ > $$tbmpfile ; \
	done
	$(PERL) $(SCRIPTDIR)/makeimode.pl $(IMODEDIR)/*.tbmp > $@

$(APP)_%.prc: $(ALL_BFDEXES) %.stamp $(PALMCUNITRO)
	$(BUILD_PRC) -o $@ -n $(ICONTEXT) -c $(APPID) $(SEGMENTDEF) $(ALL_BFDEXES) $(RESOURCEFILE) $(PALMCUNITRO)

code.stamp: $(APP)
	$(OBJ_RES) $(APP) $(SEGMENTDEF)
	@touch $@

%.stamp: $(RCP) resourceids.h const.h $(ICONDIR)/*.bmp $(LANGDIR)/%.h
	@rm -f lang.h
	@$(LN_S) $(LANGDIR)/$*.h lang.h
	@if test "$*" = "ja" ; then \
	    if test "yes" = "yes" ; then \
	        pilrc_flags='-ro -noEllipsis -Fj -q -I . -I $(srcdir) -I $(FONTDIR) -I $(ICONDIR) -L $* $(RCP) $(RESOURCEFILE)' ; \
	    else \
	        pilrc_flags='-ro -Fj -q -I . -I $(srcdir) -I $(FONTDIR) -I $(ICONDIR) -L $* $(RCP) $(RESOURCEFILE)' ; \
	    fi \
	else \
	    pilrc_flags='-ro -q -I . -I $(srcdir) -I $(FONTDIR) -I $(ICONDIR) -L $* $(RCP) $(RESOURCEFILE)' ; \
	fi ; \
    echo $(PILRC) $$pilrc_flags ; \
	$(PILRC) $$pilrc_flags
	@touch $@
	@rm -f lang.h

$(RCP): $(RCP).in $(LANGDIR)/*.rcp $(FONTDIR)/*.txt $(FONTDIR)/*.rcp $(SCRIPTDIR)/doaddition.pl
	@( cd $(FONTDIR) ; $(MAKE) )
	@$(CPP) -P -I. -I$(srcdir) $(DEFS) $(srcdir)/$(RCP).in > $@.s0
	@$(PERL) $(SCRIPTDIR)/doaddition.pl $@.s0 > $@.s
ifneq ($(strip $(CATEGORY)),)
	@echo "s:@@DEFAULT_CATEGORY@@:LAUNCHERCATEGORY ID 1000 \"$(CATEGORY)\":" > .sedscr
else
	@echo "s:@@DEFAULT_CATEGORY@@::" > .sedscr
endif
	@echo "s:@@VIEWER_VERSION@@:$(VERSION):" >> .sedscr
	@echo "s:@@WAIT@@:$(WAIT_ICON):g" >> .sedscr
	@for lang in $(LANG); do \
	    echo "7r $(LANGDIR)/$$lang.rcp" >> .sedscr ; \
	done
	@echo "// Generated automatically from $(RCP).in" > $@
	@echo "#include \"lang.h\"" >> $@
	@sed -f .sedscr $@.s >> $@

$(APP): $(OBJS) $(MULTISEG_LD)
	$(CC) $(OBJS) $(LIBS) -o $@ $(MULTISEG_LD)

$(SEGMENTDEF): Makefile
	echo "application { $(ICONTEXT) $(APPID) }" > $@ 
	echo "multiple code { $(SECTIONS) }" >> $@ 

$(MULTISEG_LD) $(MULTISEG_S): $(SEGMENTDEF)
	$(MULTIGEN) $(SEGMENTDEF)

ifneq (,)
$(PALMCUNITRCP):

$(PALMCUNITHDR) $(PALMCUNITRO): $(PALMCUNITRCP)
	$(PILRC) -ro -allowEditID -H $(PALMCUNITHDR) -q $(PALMCUNITRCP) $(PALMCUNITRO)
endif

-include .depend

