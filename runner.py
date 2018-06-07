# Runner.py
import os

class module(object):
    pass # dummy class to add members to

class Runner():
    def __init__(self,watcher):
        self.watcher = watcher
    def mod(self,name):
        lenv = self.run(name+".py")
        robj = module()
        robj.__dict__ = lenv
        return robj
    def run(self,filename):
        fn = os.path.abspath(filename)
        self.watcher.watchFile(fn)
        genv = {'__builtins__': __builtins__,'use':self.mod}
        lenv = {}
        execfile(fn,genv,lenv)
        return lenv