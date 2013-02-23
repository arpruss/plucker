#!/usr/bin/env python

"""
$Id: plucker.py,v 1.3 2002/05/18 10:28:24 nordstrom Exp $

Plucker Application for Pyrite

This Pyrite application fetches Web contents using PyPlucker and
writes the output to a PDB file, so that the Install conduit can
install it during next HotSync.

Copyright 2000 Ondrej Palkovsky <ondrap@penguin.cz>
Some parts copyright 1999 Holger Duerer <holly@starship.python.net>

Distributable under the GNU General Public License Version 2 or newer.
"""

__version__ = '$Id: plucker.py,v 1.3 2002/05/18 10:28:24 nordstrom Exp $'

__copyright__ = 'Copyright 2000 Ondrej Palkovsky <ondrap@penguin.cz>'

import Pyrite
from Pyrite import _
from Pyrite.Application import Application
from Sulfur import Options

from PyPlucker import Writer,PluckerDocs

import os

class PDBWriter(Writer.Writer):
    def __init__(self,collection,pdb,oldformat=0):
        Writer.Writer.__init__(self,collection,oldformat)
        self._pdb = pdb

    def save_data(self,data,url,id,verbose):
        rec = self._pdb.new_record()
        rec.id = id
        rec.raw = data
        self._pdb.append(rec)

class App(Application):
    name = 'Plucker Conduit for Pyrite'
    version = '$Revision: 1.3 $'[11:-1]
    author = 'Ondrej Palkovsky <ondrap@penguin.cz>'
    description= _("Fetch Web & install database in install dir")

    options = [
        Options.String('user', None, _("name of the Palm user"), None, None,
                       ['user','U']),
        Options.String('pluckerdir',os.path.expanduser("~/.plucker"),
                       _("Plucker directory"),None,None,['pluckerdir','H']),
        Options.Integer('verbosity',0,_("verbosity of PluckerDocs"),None, None,
                        ['verbose','v']),
        Options.String('dbname','PluckerDB',_("name of the target database"),
                       None,None,['dbname','D']),
        Options.Integer('dbversion',7,_("version of target database"),
                        None,None,['dbversion'])
        ]
    def __init__(self):
        Application.__init__(self)

        self.config_path = 'Pyrite::Plucker'

    def run(self,argv):
        from PyPlucker import Spider,Retriever,Parser
        
        olduid = None
        user = self.get_option('user')
        if user:
            olduid = self.uid
            i = self.user_lookup(user)
            if i:
                self.change_user(i)
            else:
                print _("user '%s' is unknown") % user
                return
            
        verbosity = self.get_option('verbosity')
        dbname = self.get_option('dbname')
        dbversion = self.get_option('dbversion')

        # Retreive the Web
        retriever = Retriever.SimpleRetriever(self.get_option('pluckerdir'))
        spider = Spider.Spider(retriever.retrieve,Parser.default_parser)
        spider.process_all(verbose=verbosity)
        
        # We will use install conduit to do the work
        store = self.user_directory('install')
        pdb = store.create(dbname,'Plkr','Data',version=dbversion)
        # Write it to the database
        writer = PDBWriter(spider.get_collected(),pdb)

        if verbosity:
            print "Writing to database: %s" % dbname
        writer.write(verbose=verbosity)
        pdb.close()
        
        # Change back, because Pyrite remembers last User
        if olduid:
            self.change_user(olduid)
        
if __name__ == '__main__':
    a = App()
    a()
