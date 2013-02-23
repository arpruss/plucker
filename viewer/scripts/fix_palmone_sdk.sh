#!/bin/sh
# Script to fix case in palmOne SDK
#
#

renames="Common Common/System Common/Libraries Common/Libraries/Camera Common/Libraries/CameraMgr Common/Libraries/CodecPluginMgr Common/Libraries/ComChannelProvider Common/Libraries/Datebook Common/Libraries/DefaultHelperLibrary Common/Libraries/FavoritesDBLibrary Common/Libraries/HTTP Common/Libraries/HiresTimerLib Common/Libraries/HsSoundLib Common/Libraries/Imaging Common/Libraries/LcdOverlayLib Common/Libraries/Locs Common/Libraries/MMS Common/Libraries/NetMaster Common/Libraries/NetPref Common/Libraries/PmConnectorLib Common/Libraries/PmKeyLib Common/Libraries/PmSysGadgetLib Common/Libraries/PmUIUtilLib Common/Libraries/SmartTextEngine Common/Libraries/Telephony Common/Libraries/TonesLibrary Common/Libraries/TransparencyLibrary Common/Libraries/VMFont 68K 68K/System 68K/Libraries 68K/Libraries/Address 68K/Libraries/Address/2.0/Pub 68K/Libraries/Audio 68K/Libraries/Camera 68K/Libraries/CameraMgr 68K/Libraries/CarrierCustomLib 68K/Libraries/CodecPluginMgr 68K/Libraries/DefaultHelperLibrary 68K/Libraries/FavoritesDBLibrary 68K/Libraries/HTTP 68K/Libraries/HiresTimerLib 68K/Libraries/HsSoundLib 68K/Libraries/HtmlLib 68K/Libraries/Imaging 68K/Libraries/LcdOverlayLib 68K/Libraries/Locs 68K/Libraries/NetMaster 68K/Libraries/NetPref 68K/Libraries/Network 68K/Libraries/PmConnectorLib 68K/Libraries/PmKeyLib 68K/Libraries/PmSysGadgetLib 68K/Libraries/PmUIUtilLib 68K/Libraries/SmartTextEngine 68K/Libraries/SndFileStream 68K/Libraries/Telephony 68K/Libraries/TonesLibrary 68K/Libraries/TransparencyLibrary 68K/Libraries/VMFont"

downcase()
{
	file=$1
	downcase=`dirname $file`/`basename $file | tr '[:upper:]' '[:lower:]'`
	if [ "$file" != "$downcase" -a ! -d $file ]
		then
#		echo "rename $path/$downcase to $path/$file"
		mv $path/$downcase $path/$file
	fi
}

usage()
{
	echo "Script to fix directory case in palmOne SDK"
	echo "Make sure your line endings were correctly set"
	echo "(unzip SDK with unzip -a)."
	echo "Usage:"
	echo "   sh fix_palmone_sdk.sh /path/to/palmOneSDK/Incs"
	exit
}

if [ $# -ne 1 ]; then
	usage
fi

if [ ! -d $1 ]; then
	usage
fi

path=$1

for i in $renames
    do downcase $i
done

if [ -e "$1/68K/Libraries/filebrowser/FileBrowserLib68K.h" ]; then
sed -i '/^#define __FILE_BROWSER_LIB_68K_H__/a\
#include <Common/Libraries/filebrowser/FileBrowserLibCommon.h>' "$1/68K/Libraries/filebrowser/FileBrowserLib68K.h"
fi
