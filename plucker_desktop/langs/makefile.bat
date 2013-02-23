#-----------------------------------------------------------------------------------------
# Make commands for the translations catalog.
# Requirements: xgettext.exe, msgmerge.exe, and msgfmt.exe (all in a poEdit distribution)
# and the the custom wxrc.exe (source code found in this directory). All 4 executables 
# must be available somewhere in computer's AUTOEXEC.BAT's path. 
# To compile: make -f makefile.b32 langs
#             make -f makefile.b32 langs2
#-----------------------------------------------------------------------------------------

# First, define the Macro definitions that we will be using

# XRC files.
XRC_DIR=resource\xrc
XRC_FILES = \
	about_dialog.xrc \
	about_simple_dialog.xrc \
	channel_dialog.xrc \
	details_progress_dialog.xrc \
	editor_dialog.xrc \
	exclusion_dialog.xrc \
	main_menubar.xrc \
	main_toolbar.xrc \
	preferences_dialog.xrc \
	showcase_dialog.xrc \
	main_frame.xrc

# File to hold output.          
XRC_STRINGS_OUTPUT_FILE=xrc_strings_output.h

# Collect the strings from the tip-of-the-day files.
TIPS_DIR=resource\strings_tips
TIPS_FILES = \
    $(TIPS_DIR)\tips.txt

# Assemble list of all files that can have translatable strings.
ALL_SOURCE_FILES_TO_SEARCH = \
    $(SOURCES) \
    $(SOURCES:.cpp=.h) \
    $(XRC_DIR)\$(XRC_STRINGS_OUTPUT_FILE) \
    $(TIPS_FILES)

LANGUAGES_DIR=langs
FINAL_PLUCKER_TEMPLATE_POT=sample_po.txt
TEMPORARY_PLUCKER_TEMPLATE_POT=sample_po_temp
PLUCKER_TRANSLATED_PO=plucker-desktop.po
PLUCKER_TRANSLATED_MO=plucker-desktop.mo

WXSTD_TEMPLATE_POT=sample_wxstd_po.txt
WXSTD_TRANSLATED_PO=wxstd.po
WXSTD_TRANSLATED_MO=wxstd.mo

# Now, next step is to extract all the user-visible strings from the XML resoucres. See
# appendix below if want to know the switches used for wxrc.exe

langs:
    cd $(XRC_DIR)
    -wxrc /v /g /o $(XRC_STRINGS_OUTPUT_FILE) $(XRC_FILES)

# Now, run xgettext to search all the source files for translatable strings, and put that into
# a temporary po file. Then merge that temporary file with the old template .po file,
# This strategy preserved the title header of the old template file, with charset, etc. 
# See Appendix for arguments used for xgettext.exe and msgmerge.exe

langs2:  
    xgettext --foreign-user -n -k_ -o $(TEMPORARY_PLUCKER_TEMPLATE_POT) -p $(LANGUAGES_DIR) $(ALL_SOURCE_FILES_TO_SEARCH)
    msgmerge $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) $(LANGUAGES_DIR)\$(TEMPORARY_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT)
    -erase $(LANGUAGES_DIR)\$(TEMPORARY_PLUCKER_TEMPLATE_POT)

# Merge the updates with the existing translations 
    msgmerge $(LANGUAGES_DIR)\ca\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\ca\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\cs\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\cs\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\da\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\da\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\de\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\de\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\es\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\es\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\fi\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\fi\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\fo\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\fo\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\fr\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\fr\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\it\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\it\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\ja\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\ja\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\nl\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\nl\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\pl\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\pl\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\ru\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\ru\$(PLUCKER_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\sv\$(PLUCKER_TRANSLATED_PO) $(LANGUAGES_DIR)\$(FINAL_PLUCKER_TEMPLATE_POT) -o $(LANGUAGES_DIR)\sv\$(PLUCKER_TRANSLATED_PO)

# Compile the text .po files into binary .mo files. 
    msgfmt $(LANGUAGES_DIR)\ca\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\ca\$(PLUCKER_TRANSLATED_MO)    
    msgfmt $(LANGUAGES_DIR)\cs\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\cs\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\da\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\da\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\de\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\de\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\es\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\es\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\fi\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\fi\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\fo\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\fo\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\fr\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\fr\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\it\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\it\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\ja\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\ja\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\nl\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\nl\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\pl\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\pl\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\ru\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\ru\$(PLUCKER_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\sv\$(PLUCKER_TRANSLATED_PO) -o $(LANGUAGES_DIR)\sv\$(PLUCKER_TRANSLATED_MO)

# Successfully finished. We can get rid of the XRC strings now.
    -erase $(XRC_DIR)\$(XRC_STRINGS_OUTPUT_FILE)

# Merge the wxstd updates with the existing translations 
    msgmerge $(LANGUAGES_DIR)\ca\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\ca\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\cs\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\cs\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\da\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\da\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\de\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\de\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\es\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\es\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\fi\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\fi\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\fo\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\fo\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\fr\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\fr\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\it\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\it\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\ja\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\ja\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\nl\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\nl\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\pl\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\pl\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\ru\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\ru\$(WXSTD_TRANSLATED_PO)
    msgmerge $(LANGUAGES_DIR)\sv\$(WXSTD_TRANSLATED_PO) $(LANGUAGES_DIR)\$(WXSTD_TEMPLATE_POT) -o $(LANGUAGES_DIR)\sv\$(WXSTD_TRANSLATED_PO)

# Compile the wxstd text .po files into binary .mo files.
    msgfmt $(LANGUAGES_DIR)\ca\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\ca\$(WXSTD_TRANSLATED_MO)     
    msgfmt $(LANGUAGES_DIR)\cs\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\cs\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\da\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\da\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\de\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\de\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\es\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\es\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\fi\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\fi\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\fo\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\fo\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\fr\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\fr\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\it\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\it\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\ja\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\ja\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\nl\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\nl\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\pl\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\pl\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\ru\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\ru\$(WXSTD_TRANSLATED_MO)
    msgfmt $(LANGUAGES_DIR)\sv\$(WXSTD_TRANSLATED_PO) -o $(LANGUAGES_DIR)\sv\$(WXSTD_TRANSLATED_MO)

    
# This is a generic rule for making mo files out of po files.
# .po is the input extension, .mo is the output extension.
# The macro "$@" gets replaced as "somepath\somefilename.po"
# The macro "$<" is outputted as "somepath\somefilename.mo"
#po.mo:
# msgfmt -v -o $@ $< 
%.mo: %.po
	 msgfmt -v -o $@ $<

#-----------------------------------------------------------------------------------------
# Plucker Desktop Online Help
# Make commands for the online help.
# Requirements: txt2rtf.exe, and a zip utility
# To compile: make -f makefile.b32 onlinehelp 
#             make -f makefile.b32 onlinehelp2
#-----------------------------------------------------------------------------------------

# Name of the DOS zip utility, somewhere in the path
ZIP=pkzip
# Path to the PluckerDesktop.tex file, ini and where .htb should end up. 
DOCS_DIR=resource\docs
# Names of all the output as gotten from the TARGET variable way up top.
HELP_FILENAME=$(DOCS_DIR)\$(TARGET)

onlinehelp:    
    -erase $(DOCS_DIR)\*.html
    -tex2rtf $(DOCS_DIR)\PluckerDesktop.tex $(HELP_FILENAME).html -macros $(DOCS_DIR)\Tex2Rtf.ini

onlinehelp2:
    $(ZIP) $(HELP_FILENAME).htb $(HELP_FILENAME).hh* $(HELP_FILENAME)*.html

#-----------------------------------------------------------------------------------------
# "clean:" Make clean target is already in the included file: $(WXDIR)\src\makeprog.b32 
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Appendices
#-----------------------------------------------------------------------------------------

# APPENDIX: wxrc.exe usage (resource compiler)
# Wxrc, customized with a /s switch to output translatable strings. The original
# wxrc is in your wx/contrib/utils. Overwrite the wxrc.cpp with the one included in /langs
# to build this tool.
# Example usage is 
#        wxrc /v /g /o out.h 1.xrc 2.xrc
# (Other platforms will use their own form of command line switches instead of '/')
# in which the arguments, and parameters are:
# /v          Verbose (doesn't seem to do much)
# /g          Specifies to output as a list of translatable strings
# /k          Optional uses a different macro prefix in the output. Default is to use _ so strings
#             get enclosed in a _(""). If typed /k _T  then strings get enclosed in a _T("").
# /o out.h    Specifies the output file as "out.h".
# 1.xrc 2.xrc The filenames of the XRCs in which to look for strings are "1.xrc" and "2.xrc".

# APPENDIX: xgettext.exe usage
# Extracts strings enclosed in a tranlation function, usually _(), and outputs them to an
# ouput file called a .po file which is a series of original strings and its translation.
# Example usage is: 
#        xgettext -C -n -k_ -o fr.po -p langs/fr file1.cpp file2.cpp
# in which the arguments, and parameters are:
# -C          tells that the language is C++
# -n          tells not to add comments that say what line the string was on #            
# -k_         marks _() as a keyword of a string to put into a translation.
#             If using _T() [which I think this is the same as _() ] need to add that
#             too in the key words.
# -o fr.po    tells what want the output file of the catalog of translated strings 
#             to be called.
# -p langs/fr sets the output directory to langs/fr 
#  file1.cpp file2.cpp The input filles in which to extract the strings.
#
# Another option (not shown) is a "-f filelist.txt" that tells xgettext to look in a 
# textfile called "filelist.txt" for a list of files in which to search for 
# translatable strings. That file has each filename on a single line. Each line has 
# only the filename, and ends with a return.    
            
# APPENDIX: msgmerge.exe usage
# Updates an existing .po file that already contains some translations already. 
# There is 2 .po files: the old one, containing some translations, and the newly one
# containing no translations yet, but is an up-to-date list of all possible 
# translatable strings in the source code. By updating I mean three things happen
# (1) Any strings no longer in the up-to-date source code are removed from the
#     old .po file.
# (2) Any translatable strings in the new .po file, but not in the old .po file are
#     added with an empty translation entry ready to be translated by the translator.
# (3) Any translatable strings that are both in the new and old .po file, retain thier
#     translations, even if the stings location in the source code was moved.
# The header at the top of old .po file (translator's email address, charset, etc.
# is also kept.
#
# Usage is simple:
#            msgmerge old_po_file.po new_po_file.po -o old_po_file.po
# in which the arguments, and parameters are:
# old_po_file.po     The old .po file is about to be updated.
# new_po_file.po     The new .po file, containing an up to date list of all strings that
#                    exist in the source code.
# -o old_po_file.po  Specifies that the outputs should be written to the filename
#                    called old_po_file.po
# After running the command, can now delete new_po_file.po if desired.

# APPENDIX: msgfmt.exe usage
# Compiles a text .po file to a binary .mo file
# Usage is simple: 
#          msgfmt -v -o mo_file.mo po_file.po
# in which the arguments, and parameters are:
# -v                 Verbosity on (so can see error messages)
# -o mo_file.mo      The .mo file to output to.
# po_file.po         The input .po file

