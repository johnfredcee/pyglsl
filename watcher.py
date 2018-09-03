# Watcher.py

import sys,os,time,Queue

from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

class Watcher(FileSystemEventHandler):
    def __init__(self,path):
        self.filesToWatch = {}
        self.events = Queue.Queue()
        self.observer = Observer()
        self.observer.schedule(self, path, recursive=True)
        self.observer.start()
    def watchFile(self,fileToWatch):
        if fileToWatch not in self.filesToWatch:
            self.filesToWatch[fileToWatch] = True
    def waitForEvent(self,block=True,timeout=None):
        try:
            return self.events.get(block,timeout)
        except Queue.Empty:
            return None
    def on_modified(self,ev):
        if ev.src_path in self.filesToWatch:
            self.events.put(ev.src_path)
    def stop(self):
        self.observer.stop()
        self.observer.join()