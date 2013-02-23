#
# $Id: Makefile.in,v 1.10 2006/12/18 00:13:10 chrish Exp $
#

SHELL           =   /bin/sh

top_builddir    =   ../..
subdir          =   parser/c++/cplucker
srcdir          =   .


DESTDIR	        =

BINDIR          =   $(DESTDIR)/usr/local/bin
LIBDIR          =   $(DESTDIR)/usr/local/lib
DATADIR         =   $(DESTDIR)${prefix}/share/plucker

INSTALL         =   /usr/bin/install -c
INSTALL_PROGRAM =   ${INSTALL}
MKINSTALLDIRS   =   $(top_builddir)/mkinstalldirs

REGEX_DIR       =   $(top_builddir)/tools/regexp
REGEX_LIB       =   $(REGEX_DIR)/libcppre.a

TARGET          =   cpluck

RANLIB          =   ranlib

CC              =   gcc
CFLAGS          =   -g -O2
CPPFLAGS        =   -I$(REGEX_DIR) $(CFLAGS)
LDFLAGS         =   -lz -lhttp `Magick++-config --ldflags --libs`

SOURCES         =   \
		Config.cpp \
		FileRetriever.cpp \
		ExclusionList.cpp \
		GlobalFunctions.cpp \
		HtmlParser.cpp \
		HttpRetriever.cpp \
		ImageParser.cpp \
		Message.cpp \
		PDBWriter.cpp \
		Parser.cpp \
		PluckerCategoryDocument.cpp \
		PluckerDocument.cpp \
		PluckerImageDocument.cpp \
		PluckerIndexDocument.cpp \
		PluckerLinkIndexDocument.cpp \
		PluckerLinksDocument.cpp \
		PluckerMailtoDocument.cpp \
		PluckerMetaDocument.cpp \
		PluckerMetaSubRecord.cpp \
		PluckerMultiImageDocument.cpp \
		PluckerTableDocument.cpp \
		PluckerTextDocument.cpp \
		PluckerTextParagraph.cpp \
		Resolver.cpp \
		Retriever.cpp \
		Spider.cpp \
		TextParser.cpp \
		Url.cpp \
		Writer.cpp \
		pluckerbuild.cpp

OBJECTS         =   ${SOURCES:.cpp=.o}

all: $(TARGET)

$(TARGET): $(OBJECTS) $(REGEX_LIB)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^

$(REGEX_LIB):
	cd $(REGEX_DIR); $(MAKE)

install:
	$(MKINSTALLDIRS) $(BINDIR)
	-cp $(TARGET) $(BINDIR)


distclean: clean
	rm -f Makefile *.bak *.orig
	cd $(REGEX_DIR); $(MAKE) distclean

clean:
	rm -f *.o core .depend $(TARGET)
	cd $(REGEX_DIR); $(MAKE) clean

