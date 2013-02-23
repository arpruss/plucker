#
# This file is only used in the Java (Jython) version
# It serves as an entry point.
#

import sys
import PyPlucker.Spider
from java.lang import Runtime
from java.util import Hashtable
import org.plkr.distiller.API

class InvokePluckerBuildFromJava (org.plkr.distiller.API.Invocation):

    def __init__(self):
        "@sig public InvokePluckerBuildFromJava()"
        pass

    def create_dict_from_hashtable (self, ht):
        dict = {}
        e = ht.keys()
        while e.hasMoreElements():
            key = e.nextElement()
            value = ht.get(key)
            dict[str(key)] = str(value)
        return dict


    def invoke(self, args, os, inputstring, config, callback):
        "@sig public int invoke(java.lang.String[] args, java.io.OutputStream os, java.lang.String inputString, java.util.Hashtable config, org.plkr.distiller.API.Callback status)"
        varargs = ['plucker-build']
        if args:
            for arg in args:
                varargs.append(str(arg))
        if os:
            outputstream = org.python.core.PyFile(os, "<stream>", "wb")
        else:
            outputstream = None
        if config:
            configdict = self.create_dict_from_hashtable(config)
        else:
            configdict = None
        
        val = PyPlucker.Spider.realmain(varargs, outputstream, inputstring, configdict, callback)

        return val


if __name__ == '__main__':
    theRuntime = Runtime.getRuntime()
    try:
        val = InvokePluckerBuildFromJava().invoke(sys.argv[1:], None, None, None, None)
        theRuntime.exit(val)
    except:
        import traceback
        traceback.print_exc(None, sys.stderr)
        theRuntime.exit(1)
