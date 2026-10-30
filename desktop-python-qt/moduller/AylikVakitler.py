# -*- coding: utf-8 -*-

# Hazırlayan : Rahman Yazgan (rahmanyazgan@gmail.com)
# Lisans : GPL v.3

import sqlite3
from PyQt5 import QtCore, QtGui, QtWidgets
from moduller.Ui_Vakitler import Ui_Dialog

class AylikVakitler(QtWidgets.QDialog, Ui_Dialog):
    veritabaniVarmi = None
    
    def __init__(self, veritabaniDosyasi):
        QtWidgets.QDialog.__init__(self)
        self.setupUi(self)
        
        self.tableView.setCornerButtonEnabled(False)
        
        veritabaniDosyasi = QtCore.QFile(veritabaniDosyasi)
        self.indis = 0
        self.vakitleriGoster(veritabaniDosyasi)
        
    def vakitleriGoster(self, veritabaniDosyasi):
        if QtCore.QFile.exists(veritabaniDosyasi) and veritabaniDosyasi.size() > 0:
            if self.veritabaninaBaglan(veritabaniDosyasi.fileName()):
                try:
                    veriler = self.veritabanindakiTumBilgileriCek()
                    if not veriler:
                        raise ValueError("Tablo boş")
                    self.tabloModeliniOlustur(len(veriler))
                    self.tabloyuDoldur(veriler)
                    self.exec_()
                    AylikVakitler.veritabaniVarmi = True
                except Exception:
                    QtWidgets.QMessageBox.information(self, "Bilgilendirme",
                                                  "Aylık namaz vakitleri veritabanı boş. " + 
                                                  "\"Kapat\" butonuna bastığınızda veritabanınız güncellenecek.", 
                                                  QtWidgets.QMessageBox.Ok)
                    AylikVakitler.veritabaniVarmi = False
                finally:
                    if hasattr(self, "baglanti") and self.baglanti:
                        self.baglanti.close()
        else:
            AylikVakitler.veritabaniVarmi = False
            
    def tabloModeliniOlustur(self, satirSayisi=30):
        self.satirSayisi = satirSayisi
        self.sutunSayisi = 8
        self.model = QtGui.QStandardItemModel(self.satirSayisi, self.sutunSayisi)
        
        sutunAdlari = ["Lokasyon", "Tarih", "İmsak", "Güneş", "Öğle", "İkindi", "Akşam", "Yatsı"]
        
        self.indis = 0
        for sutunAdi in sutunAdlari:
            self.model.setHorizontalHeaderItem(self.indis, QtGui.QStandardItem(sutunAdi))
            self.indis = self.indis + 1
            
        self.tableView.setModel(self.model)
    
    def veritabaninaBaglan(self, veritabaniDosyasi):
        try:
            self.baglanti = sqlite3.connect(veritabaniDosyasi)
            self.imlec = self.baglanti.cursor()
            return True
        except sqlite3.OperationalError:
            print("Veritabanı yok.")
            AylikVakitler.veritabaniVarmi = False
            return False
    
    def veritabanindakiTumBilgileriCek(self):
        self.imlec.execute('SELECT * FROM namazvakitleri')
        veriler = self.imlec.fetchall()
        return veriler
    
    def tabloyuDoldur(self, veriler):
        for satir in range(0, len(veriler), 1):
            for sutun in range(0, min(self.sutunSayisi, len(veriler[satir])), 1):                            
                nesne = QtGui.QStandardItem(str(veriler[satir][sutun]))
                self.model.setItem(satir, sutun, nesne)
                
        self.imlec.close()
        self.baglanti.close()