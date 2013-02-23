OUTPUT_DIR=Plucker
COPYFLAGS=--force

step1:
	-mkdir $(OUTPUT_DIR)
	-cp $(COPYFLAGS) ReadMe.rtf $(OUTPUT_DIR)/ReadMe.rtf
	-cp -R ../Plucker.app $(OUTPUT_DIR) 
# 	-cp $(COPYFLAGS) background.png $(OUTPUT_DIR)/background.png

# I am cutting out the background.png. Too much hassle.

# at this point, set the background image to background.png and change
# the icons and settings.

step2:
	-mv $(OUTPUT_DIR)/background.png $(OUTPUT_DIR)/.background.png

#steps:
# Create a background image. Make it big enough tallwise and widthwise, as
# I can't seem to control the size of the end result popup window.

# Make a folder and put your image in the folder, for example background.png
# Call the folder your desired volumes name, for example: Plucker

# Open up disk copy and drag the folder to the drag-and-drop window, 
# saving it as "empty_image" and being sure that it is read/write, not 
# compressed.

# Double click the outputted .dmg file to mount it on your desktop.

# In the window of the mounted volume, you should now see just your 
# background.png

# Choose View>Show View Options, and use the following settings:
# -This window only
# -Icon size=128x128
# -Snap To Grid
# -Background=Picture, and browse to the background.png on your volume (note
# that the volumes will appear as a top level item in the Finder, so it would
# be for example /Plucker/background.png
# Close the View Options popup
# In the finder window for the volume, now choose View>Hide Toolbar and 
# View>Hide Statusbar

# Open up the terminal, and cd to the volume, for example:
# cd /Volumes/Plucker
# Then in the terminal, move the background.png to .background.png. This will
# hide the background file. Do this moving by typing:
# mv background.png .background.png
# Then cd out of there:
# cd /

# Now, on the desktop, drag your volume to the DiskCopy drag and drop window,
# and make another disk image, again being sure that it is a read/write, not
# a compressed.

# THIS STILL DOESNT WORK!! HOW CRAPOLA!
