#
# $Id: Makefile,v 1.2 2003/10/06 21:51:41 robertoconnor Exp $
#
# This Makefile makes an OSX application bundle

SHELL                   = /bin/sh

# Note that I used top_build_dir instead of top_builddir
top_build_dir            = ../../..
plucker_desktop_dir     = $(top_build_dir)/plucker_desktop
installer_osx_dir       = $(top_build_dir)/plucker_desktop/installer/osx

# Needed for the .rsrc file
wxwindows_lib_dir	= $(shell wx-config --prefix)/lib
WX_LIBRARY_BASENAME	= `wx-config --basename`
WX_RELEASE		= `wx-config --release`


# mkinstalldirs is some program in the main Plucker distribution root
# directorythat makes install directories.
MKINSTALLDIRS           = $(top_build_dir)/mkinstalldirs

TARGET                  = plucker-desktop
BUNDLE_NAME             = Plucker

BUNDLE = $(BUNDLE_NAME).app/Contents

# This will remove the old files when copying over
COPYFLAGS  = --force

# Note that the docs (including the online help) are different from the Linux/GTK version.
bundle: $(BUNDLE)/MacOS/$(TARGET) $(BUNDLE)/Info.plist $(BUNDLE)/PkgInfo $(BUNDLE)/Resources/application.icns
	@echo "------------------------------------------------------------"
	@echo "Copying Plucker Desktop docs..."
	@echo "------------------------------------------------------------"
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/doc
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/*.txt $(BUNDLE)/Resources/doc
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/docs/*.txt $(BUNDLE)/Resources/doc
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/docs/*.htb $(BUNDLE)/Resources/doc/
	@echo "------------------------------------------------------------"
	@echo "Copying Plucker Desktop resources..."
	@echo "------------------------------------------------------------"
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/graphics_dialog
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/graphics_dialog/*.* $(BUNDLE)/Resources/plucker_desktop/resource/graphics_dialog/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/graphics_misc
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/graphics_misc/*.* $(BUNDLE)/Resources/plucker_desktop/resource/graphics_misc/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/graphics_wizard
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/graphics_wizard/*.* $(BUNDLE)/Resources/plucker_desktop/resource/graphics_wizard/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/icons_application
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/icons_application/*.* $(BUNDLE)/Resources/plucker_desktop/resource/icons_application/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/icons_dialog
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/icons_dialog/*.* $(BUNDLE)/Resources/plucker_desktop/resource/icons_dialog/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/icons_image_list
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/icons_image_list/*.* $(BUNDLE)/Resources/plucker_desktop/resource/icons_image_list/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/icons_art_provider
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/icons_art_provider/*.* $(BUNDLE)/Resources/plucker_desktop/resource/icons_art_provider/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/launcher_icons_large
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/launcher_icons_large/*.* $(BUNDLE)/Resources/plucker_desktop/resource/launcher_icons_large/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/launcher_icons_small
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/launcher_icons_small/*.* $(BUNDLE)/Resources/plucker_desktop/resource/launcher_icons_small/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/plkrdata_installation
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/plkrdata_installation/*.plkrdata $(BUNDLE)/Resources/plucker_desktop/resource/plkrdata_installation/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/plkrdata_setup_wizard
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/plkrdata_setup_wizard/*.* $(BUNDLE)/Resources/plucker_desktop/resource/plkrdata_setup_wizard/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/plkrdata_showcase
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/plkrdata_showcase/*.* $(BUNDLE)/Resources/plucker_desktop/resource/plkrdata_showcase/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/strings_tips
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/strings_tips/*.txt $(BUNDLE)/Resources/plucker_desktop/resource/strings_tips/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/xrc
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/xrc/*.xrc $(BUNDLE)/Resources/plucker_desktop/resource/xrc/
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/resource/html_about
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/resource/html_about/*.htm $(BUNDLE)/Resources/plucker_desktop/resource/html_about/
	@echo "------------------------------------------------------------"
	@echo "Copying Plucker Desktop internationalization..."
	@echo "------------------------------------------------------------"
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/cs
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/da
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/de
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/es
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/fi
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/fo
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/fr
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/it
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/ja
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/nl
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/pl
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/ru
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/plucker_desktop/langs/sv
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/cs/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/cs/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/da/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/da/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/de/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/de/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/es/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/es/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/fi/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/fi/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/fo/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/fo/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/fr/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/fr/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/it/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/it/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/ja/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/ja/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/nl/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/nl/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/pl/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/pl/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/ru/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/ru/
	-cp $(COPYFLAGS) $(plucker_desktop_dir)/langs/sv/*.mo $(BUNDLE)/Resources/plucker_desktop/langs/sv/
	@echo "------------------------------------------------------------"
	@echo "Copying Python parser...."
	@echo "------------------------------------------------------------"
	$(MKINSTALLDIRS) $(BUNDLE)/Resources/parser/python/PyPlucker
	-cp $(COPYFLAGS) $(top_build_dir)/parser/python/PyPlucker/*.py $(BUNDLE)/Resources/parser/python/PyPlucker
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/parser/python/PyPlucker/helper
	-cp $(COPYFLAGS) $(top_build_dir)/parser/python/PyPlucker/helper/*.py $(BUNDLE)/Resources/parser/python/PyPlucker/helper
	@echo "------------------------------------------------------------"
	@echo "Copying PIL (Python Imaging Library) for OS X..."
	@echo "------------------------------------------------------------"
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/parser/python/vm/PIL
	-cp $(COPYFLAGS) ./application_bundle_files/Resources/parser/python/vm/PIL/*.*  $(BUNDLE)/Resources/parser/python/vm/PIL
	@echo "------------------------------------------------------------"
	@echo "Copying Plucker viewers...."
	@echo "Note: You will need to manually add to bundle, those parts"
	@echo "that haven't been built: viewers, zlib, or user's guide."
	@echo "------------------------------------------------------------"
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources/viewer/palmos
	-cp $(COPYFLAGS) $(top_build_dir)/viewer/*.prc $(BUNDLE)/Resources/viewer/palmos
	-cp $(COPYFLAGS) $(top_build_dir)/viewer/ZLib/*.prc $(BUNDLE)/Resources/viewer/palmos
	-cp $(COPYFLAGS) $(top_build_dir)/viewer/*.pdb $(BUNDLE)/Resources/viewer/palmos
	@echo "------------------------------------------------------------"
	@echo "Copying the .rsrc file from the wxWindows installed /lib directory..."
	@echo "------------------------------------------------------------"
	@$(MKINSTALLDIRS) $(BUNDLE)/Resources
	-cp $(COPYFLAGS) $(wxwindows_lib_dir)/lib$(WX_LIBRARY_BASENAME)-$(WX_RELEASE).rsrc $(BUNDLE)/Resources/$(TARGET).rsrc
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "A $(BUNDLE_NAME) OSX app bundle (called $(BUNDLE_NAME).app)"
	@echo "has been generated."
	@echo " "
	@echo "To launch the GUI version, double click the Plucker icon"
	@echo "located in this directory."
	@echo " "
	@echo "To use the commandline version, enter this command in"
	@echo "in this current directory:"
	@echo "<fullpathtocurrentdirectory>/$(BUNDLE_NAME).app/Contents/MacOS/$(TARGET)"
	@echo "followed by some commandline arguments, for example:"
	@echo "<fullpathtocurrentdirectory>/$(BUNDLE_NAME).app/Contents/MacOS/$(TARGET) --update-all"
	@echo "------------------------------------------------------------"
	@echo " "

$(BUNDLE)/Info.plist: ./application_bundle_files/Info.plist
	@$(MKINSTALLDIRS) `dirname $@`
	cp $(COPYFLAGS) $< $@

$(BUNDLE)/PkgInfo: ./application_bundle_files/PkgInfo
	@$(MKINSTALLDIRS) `dirname $@`
	cp $(COPYFLAGS) $< $@

# It should depend on the library also, but we can't get that from wx-config yet
# Note that we strip the executable
$(BUNDLE)/MacOS/$(TARGET): $(plucker_desktop_dir)/$(TARGET)
	@$(MKINSTALLDIRS) `dirname $@`
	cp $(COPYFLAGS) $< $@
	@echo "Stripping the plucker-desktop executable inside Bundle..."
	strip $@

$(BUNDLE)/Resources/application.icns: $(plucker_desktop_dir)/resource/icons_application/application.icns
	@$(MKINSTALLDIRS) `dirname $@`
	cp $(COPYFLAGS) $< $@
