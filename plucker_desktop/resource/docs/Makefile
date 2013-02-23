#-----------------------------------------------------------------------------------------
# Plucker Desktop User Help
# Requirements: txt2rtf executable (source is in <wx root dir>/utils/tex2rtf) 
#               hhp2cached executable (source is in <wx root dir>/utils/hhp2cached) 
#               and a zip program available to be executed
#-----------------------------------------------------------------------------------------

TARGET=plucker-desktop

# Name of the zip utility, somewhere in the path
ZIP=zip
# NOTE: the zipping of the help file has to be done such that there is no subdir path
# iformation stored in it. Seems that must have BOTH a ZIPFLAGS of "-D" (strip paths) 
# AND cd to the docs directory first before zipping for the paths to be removed.
# hhp2cached returns non-zero, so stops make, so added a '-' command in front.
ZIPFLAGS= -D
TEX2RTF=tex2rtf
HHP2CACHED=hhp2cached
# Names of all the output as gotten from the TARGET variable way up top.
HELP_BASENAME=$(TARGET)

userhelp:
	@echo "------------------------------------------------------------"
	@echo "Removing old html files, and .htb file (since would just append)..."
	@echo "------------------------------------------------------------"     
	-rm *.html $(HELP_BASENAME).htb
	@echo "------------------------------------------------------------"
	@echo "Compiling help using Tex2RTF..."
	@echo "------------------------------------------------------------"
	$(TEX2RTF) PluckerDesktop.tex $(HELP_BASENAME).html -macros Tex2Rtf.ini
	@echo "------------------------------------------------------------"
	@echo "Generating a cached version for faster reading..."
	@echo "------------------------------------------------------------"
	-$(HHP2CACHED) $(HELP_BASENAME).hhp
	@echo "------------------------------------------------------------"
	@echo "Zipping output html into a .htb file (hypertext book)..."
	@echo "------------------------------------------------------------"
	$(ZIP) $(ZIPFLAGS) $(HELP_BASENAME).htb $(HELP_BASENAME).hh* $(HELP_BASENAME)*.html
	@echo "Done."

#-----------------------------------------------------------------------------------------
# Clean all (leave in the plucker-manual.htb though, currently hard to make)
#-----------------------------------------------------------------------------------------
clean:
	@echo "------------------------------------------------------------"
	@echo "Removing generated Plucker Desktop user help html files..."
	@echo "------------------------------------------------------------"     
	rm -f $(TARGET)*.h*
	rm -f *.con
	rm -f *.ref*
	rm -f *.cached

