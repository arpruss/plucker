#
# $id$
#

SHELL		=	/bin/sh
RM		=	/usr/bin/rm
JAVAC		=	/usr/local/bin/javac
JAR		=	
JYTHONC		=	

top_builddir	=	../..
subdir		=	parser/python
srcdir          =	.


VERSION		=	2005-02-07-cvs

DESTDIR		=

BINDIR		=	$(DESTDIR)/usr/local/bin
MODULE_DIR	=	$(DESTDIR)/usr/lib/python2.5/site-packages/PyPlucker

JAVA_TARGET	=	
JAVA_HOME	=	
JAVA_CLASSPATH  =	.;C:\Program Files\Java\jre1.6.0_01\lib\ext\QTJava.zip

SUBDIRS		=	PyPlucker

MKINSTALLDIRS	=	$(top_builddir)/mkinstalldirs

all:	$(JAVA_TARGET)
	@echo ""all done

Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status

plucker-build.jar: InvokePluckerBuildFromJava.py Callback.java Invocation.java PyPlucker/*.py PyPlucker/helper/*.py
	# first, make the Java interface file
	mkdir -p ./javabuild/org/plkr/distiller/API
	$(RM) -f ./javabuild/org/plkr/distiller/API/*
	cp Invocation.java Callback.java ./javabuild/org/plkr/distiller/API
	$(JAVAC) -classpath ./javabuild/org/plkr/distiller/API ./javabuild/org/plkr/distiller/API/*.java
	$(RM) -f ./javabuild/PluckerBuildIfc.jar
	$(JAR) cf ./javabuild/PluckerBuildIfc.jar -C ./javabuild org/plkr/distiller/API/Invocation.class \
		-C ./javabuild org/plkr/distiller/API/Callback.class
	# next, run the jython compile
	CLASSPATH=./javabuild/PluckerBuildIfc.jar:.:$(JAVA_CLASSPATH) $(JYTHONC) \
		--addpackages net.sourceforge.jiu --compileropts '-deprecation -verbose' \
		--core --deep --package org.plkr.distiller --jar ./plucker-build.jar \
		InvokePluckerBuildFromJava.py
	# add the org.plkr.distiller.API interfaces
	$(JAR) uf ./plucker-build.jar -C ./javabuild org/plkr/distiller/API/Invocation.class \
		-C ./javabuild org/plkr/distiller/API/Callback.class
	# make InvokePluckerBuildFromJava be the main class
	echo "Main-Class: org.plkr.distiller.InvokePluckerBuildFromJava" > ./javabuild/pluckerbuildmainclass
	$(JAR) umf ./javabuild/pluckerbuildmainclass ./plucker-build.jar
	$(RM) -rf ./javabuild

install:
	if test "x$(JAVA_TARGET)" != x; then \
		$(INSTALL) -m 555 $(JAVA_TARGET) $(JAVA_HOME)/lib/ext; \
	fi
	$(MKINSTALLDIRS) $(MODULE_DIR)
	for i in $(SUBDIRS) ; do \
	    cd $$i; $(MAKE) install; cd ..; \
	done ;

clean:
	rm -rf javabuild
	rm -f plucker-build.jar
	find . -name '*~' -exec $(RM) -f {} \;
	find . -name '*.pyc' -exec $(RM) -f {} \;
	find . -name '*.pyo' -exec $(RM) -f {} \;
	find . -name '*.class' -exec $(RM) -f {} \;
	for dir in $(SUBDIRS); do \
		cd $$dir; $(MAKE) clean; cd ..; \
	done 

distclean: clean
	for dir in $(SUBDIRS); do \
		cd $$dir; $(MAKE) distclean; cd ..; \
	done 
	$(RM) -f Makefile
