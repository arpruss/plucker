#
# This file is only used in the Java (Jython) version
# It serves as an entry point.
#

import sys
import PyPlucker.Spider
from java.lang import Runtime;

theRuntime = Runtime.getRuntime()
try:
    val = PyPlucker.Spider.realmain()
    theRuntime.exit(val)
except:
    theRuntime.exit(1)
