# -*- coding: utf-8 -*-

# Hazırlayan : Rahman Yazgan (rahmanyazgan@gmail.com)
# Lisans : GPL v.3
# Winamp Kontrol Modulu (http://www.etcwiki.org/wiki/Winamp_command_line)

from PyQt5 import QtCore
import os, shutil

class Clever:
    def __init__(self, konum):
        self.konum = konum
        self.tempPath = QtCore.QDir.tempPath()
        
        self.cleverModulunuHazirla()
        
    def cleverModulunuHazirla(self):
        self.clever = self.konum + "/clever.exe"
        self.cleverYeni = self.tempPath + "/clever.exe"
        
        if not (QtCore.QFile.exists(QtCore.QFile(self.cleverYeni))):
            shutil.copy(self.clever, self.cleverYeni)
        
        self.clever = self.cleverYeni

    def oynat(self):
        self.komutCalistir(" play")
    
    def duraklat(self):
        self.komutCalistir(" pause")
    
    def komutCalistir(self, komut):
        self.komut = self.clever + komut
        
        if QtCore.QFile.exists(QtCore.QFile(self.clever)):
            os.popen(self.komut)
        else:
            self.cleverModulunuHazirla()
            os.popen(self.komut)
