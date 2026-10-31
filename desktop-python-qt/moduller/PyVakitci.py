#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Hazırlayan : Rahman Yazgan (rahmanyazgan@gmail.com)
# Lisans : GPL v.3
# Sürüm : 1.8
# Platform : Windows

import datetime, os, platform, re, shutil, sys, threading, urllib.request, winreg, win32api
from time import sleep
from win32con import VK_MEDIA_PLAY_PAUSE, KEYEVENTF_EXTENDEDKEY

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5 import QtMultimedia, QtNetwork
   
from moduller.DiyanetUlkeler import DiyanetUlkeler
from moduller.SehirleriAl import SehirleriAl
from moduller.IlceleriAl import IlceleriAl

from moduller.SiteyeBaglan import SiteyeBaglan
from moduller.Vakitler import Vakitler

from moduller.Calverter import Calverter
from moduller.Clever import Clever

from moduller.AylikVakitler import AylikVakitler
from moduller.HavaDurumu import HavaDurumuWidget
from moduller.Temalar import TEMALAR
from moduller.Ui_PyVakitci import Ui_MainWindow
from moduller.VakitGorselleri import VakitSaatiWidget, GokyuzuWidget
from moduller.Localization import (
    set_language, get_language, tr, get_license_file, detect_system_language,
    LANGUAGES, format_current_prayer, format_remaining_time, format_remaining_short
)

class PyVakitci(QtWidgets.QMainWindow, Ui_MainWindow):
    calismaDizini = ""
    diyanetUlkeler = None
    ilceler = None
    sehirler = None
    rlock = None
    veritabaniDosyasi = None
    version = 1.8

    def __init__(self):
        super(PyVakitci, self).__init__()
        self.setupUi(self)

        self.tabWidget.setUsesScrollButtons(False)
        self.tabWidget.tabBar().setExpanding(False)

        self.player = QtMultimedia.QMediaPlayer()
        
        self.degiskenleriHazirla()
        self.modernArayuzuHazirla()
        self.ayarlarVeHavaDurumuSekmeleriniHazirla()
        self.sinyalleriEkle()
        self.fusionGorunumu()
        self.calismaDizininiHazirla()
        self.bolgeselAyarlar()
        self.ekrandaOrtala()
        self.sesVarsayilanAyarlar()
        self.ayarlariUygula()
        self.retranslateUiDynamic()
        self.trayIconGoster()
        self.hicriTarihiGuncelle()
        self.otomatikGuncellestir()
        self.saatGuncelleyiciyiCalistir()
        self.initMultimedia()
        self.sesOynatmaButonlariniHazirla()
       
    def initMultimedia(self, filePath = ""):
        if filePath:
            url = QtCore.QUrl.fromLocalFile(filePath)
            content = QtMultimedia.QMediaContent(url)
        else:
            content = QtMultimedia.QMediaContent()
        self.player.setMedia(content)
           
    def degiskenleriHazirla(self):        
        self.volume = 0
        self.ulke = "TÜRKİYE"
        self.sehir = ""
        self.ilce = ""

        if getattr(sys, "frozen", False):
            self.programinKonumu = os.path.dirname(sys.executable).replace(os.sep, "/")
        else:
            self.programinKonumu = os.path.dirname(os.path.dirname(os.path.abspath(__file__))).replace(os.sep, "/")
        self.sesDosyalariKonum = self.programinKonumu + "/ses_dosyalari/"
        self.tempPath = QtCore.QDir.tempPath()
        self.ayarlarKonum = QtCore.QDir.homePath() + "/.PyVakitci-" + str(self.version)
        self.ayarDosyasi = self.ayarlarKonum + "/ayarlar.ini"
       
        self.diyanetURL = DiyanetUlkeler.BASE_URL
       
        SehirleriAl.ayarDosyasi = self.ayarDosyasi
        IlceleriAl.ayarDosyasi = self.ayarDosyasi
       
        self.ontanimliAyarDosyasi = self.programinKonumu + "/ontanimli_ayarlar.ini"
       
        self.settings = QtCore.QSettings(self.ayarDosyasi, QtCore.QSettings.IniFormat)
        self.settingsVarsayilan = QtCore.QSettings(self.ontanimliAyarDosyasi, QtCore.QSettings.IniFormat)
       
        saved_lang = self.settings.value("Ayarlar/dil", detect_system_language())
        set_language(saved_lang)
        self.dil = saved_lang

        PyVakitci.diyanetUlkeler = DiyanetUlkeler()
        self.winampKontrol = Clever(self.programinKonumu)
       
        self.lisansDosyasi = get_license_file(self.programinKonumu)
        self.besmeleDosyasi = self.sesDosyalariKonum + "besmele.mp3"

        self.simdikiTarih = QtCore.QDate().currentDate().toString("dd MMMM yyyy")
        Vakitler.simdikiTarih = QtCore.QDate().currentDate().toString("dd.MM.yyyy")

        self.diniGunlerVeGeceler = ["11 Rebiülevvel", "1 Recep", "6 Recep", "26 Recep", "14 Şaban",
                                    "1 Ramazan", "27 Ramazan", "30 Ramazan", "1 Şevval", "2 Şevval",
                                    "3 Şevval", "9 Zilhicce", "10 Zilhicce", "11 Zilhicce",
                                    "12 Zilhicce", "13 Zilhicce", "1 Muharrem", "10 Muharrem"]

        self.diniGunlerVeGecelerKarsiligi = ["Mevlid Kandili", "Üç Aylar'ın Başlangıcı", "Regaib Kandili",
                                             "Miraç Kandili", "Beraat Kandili", "Ramazan'ın Başlangıcı",
                                             "Kadir Gecesi", "Arefe","Ramazan Bayramı (1. gün)", "Ramazan Bayramı (2. gün)",
                                             "Ramazan Bayramı (3. gün)", "Arefe", "Kurban Bayramı (1. gün)",
                                             "Kurban Bayramı (2. gün)", "Kurban Bayramı (3. gün)",
                                             "Kurban Bayramı (4. gün)", "Hicri Yılbaşı", "Aşure Günü"]

        self.mesajGosterildi = False
        self.diniGunMesajiGosterildi = False

        self.windowFlag = self.windowFlags()
        self.mesajTimer = None
        self.linkler = {}
        self.seciliYer = ""

        self.onizlemePlayer = QtMultimedia.QMediaPlayer()
        self.playIcon = QtWidgets.QApplication.style().standardIcon(QtWidgets.QStyle.SP_MediaPlay)
        self.stopIcon = QtWidgets.QApplication.style().standardIcon(QtWidgets.QStyle.SP_MediaStop)
        self.suAnCalanSesButon = None
        self.onizlemePlayer.mediaStatusChanged.connect(self._onizlemeDurumDegisti)

        self.aktifTema = "varsayilan"
        self.temaActions = {}
        self.arayuzModu = "saat"
        self._sonAktifModernVakit = "imsak"

        self.rlock = threading.RLock()

    def sinyalleriEkle(self):
        # Görünüm menüsünü derli toplu ve düzenli şekilde yeniden yapılandır
        self.menuGorunum.clear()

        # 1. Arayüz Tasarımı (4 Farklı Görünüm Seçeneği)
        self.arayuzModuGroup = QtWidgets.QActionGroup(self)
        self.arayuzModuGroup.setExclusive(True)

        self.actionVakitSaati = QtWidgets.QAction("⏱️ Vakit Saati (Kadran & Canlı)", self)
        self.actionVakitSaati.setCheckable(True)
        self.actionVakitSaati.triggered.connect(lambda: self.arayuzModunuUygula("saat", kaydet=True))

        self.actionGokyuzu = QtWidgets.QAction("🌙 Dinamik Gökyüzü & Hilal", self)
        self.actionGokyuzu.setCheckable(True)
        self.actionGokyuzu.triggered.connect(lambda: self.arayuzModunuUygula("gokyuzu", kaydet=True))

        self.actionModernKartlar = QtWidgets.QAction("📋 Modern Kartlar", self)
        self.actionModernKartlar.setCheckable(True)
        self.actionModernKartlar.triggered.connect(lambda: self.arayuzModunuUygula("kartlar", kaydet=True))

        self.actionKlasikGorunum = QtWidgets.QAction("🏛️ Klasik Görünüm", self)
        self.actionKlasikGorunum.setCheckable(True)
        self.actionKlasikGorunum.triggered.connect(lambda: self.arayuzModunuUygula("klasik", kaydet=True))

        for act in (self.actionVakitSaati, self.actionGokyuzu, self.actionModernKartlar, self.actionKlasikGorunum):
            self.arayuzModuGroup.addAction(act)
            self.menuGorunum.addAction(act)

        self.menuGorunum.addSeparator()

        # 2. Renk Teması Alt Menüsü
        self.menuTema = QtWidgets.QMenu("Renk Teması", self.menuGorunum)
        self.temaActionGroup = QtWidgets.QActionGroup(self)
        self.temaActionGroup.setExclusive(True)

        for temaKey, temaVeri in TEMALAR.items():
            action = QtWidgets.QAction(temaVeri["ad"], self)
            action.setCheckable(True)
            if temaKey == "varsayilan":
                action.setChecked(True)
            action.triggered.connect(lambda checked, k=temaKey: self.temayiUygula(k, kaydet=True))
            self.temaActionGroup.addAction(action)
            self.menuTema.addAction(action)
            self.temaActions[temaKey] = action

        self.menuGorunum.addMenu(self.menuTema)

        # 3. Pencere Stili Alt Menüsü
        self.menuPencereStili = QtWidgets.QMenu("Pencere Stili", self.menuGorunum)
        self.stilActionGroup = QtWidgets.QActionGroup(self)
        self.stilActionGroup.setExclusive(True)

        self.actionFusion.setText("Fusion (Modern)")
        self.actionFusion.setCheckable(True)
        self.actionFusion.setChecked(True)
        self.actionFusion.triggered.connect(self.fusionGorunumu)

        self.actionWindowsVista.setText("Windows Vista")
        self.actionWindowsVista.setCheckable(True)
        self.actionWindowsVista.triggered.connect(self.windowsVistaGorunumu)

        self.actionWindowsXP.setText("Klasik Windows")
        self.actionWindowsXP.setCheckable(True)
        self.actionWindowsXP.triggered.connect(self.windowsXPGorunumu)

        for stilAction in (self.actionFusion, self.actionWindowsVista, self.actionWindowsXP):
            self.stilActionGroup.addAction(stilAction)
            self.menuPencereStili.addAction(stilAction)

        self.menuGorunum.addMenu(self.menuPencereStili)
        self.actionCikis.triggered.connect(self.programiKapat)
        self.actionVakitleriGoster.triggered.connect(self.vakitleriGoster)
        self.actionLisans.triggered.connect(self.lisans)
        self.actionHakkinda.triggered.connect(self.hakkindaDialog)
        self.actionQtHakkinda.triggered.connect(QtWidgets.QApplication.aboutQt)
        self.ikon_goster_checkBox.stateChanged.connect(self.trayIconGoster)
        self.her_zaman_ustte_checkBox.stateChanged.connect(self.herZamanUstte)
        self.winamp_duraklat_checkBox.stateChanged.connect(self.winampVarMi)
        self.sabah_ezani_oku_checkBox.stateChanged.connect(self.ezanOkuKontrol)
        self.ogle_ezani_oku_checkBox.stateChanged.connect(self.ezanOkuKontrol)
        self.ikindi_ezani_oku_checkBox.stateChanged.connect(self.ezanOkuKontrol)
        self.aksam_ezani_oku_checkBox.stateChanged.connect(self.ezanOkuKontrol)
        self.yatsi_ezani_oku_checkBox.stateChanged.connect(self.ezanOkuKontrol)
        self.erken_uyari_checkBox.stateChanged.connect(self.erkenUyari)
        self.vakitleri_guncelle_pushButton.clicked.connect(self.diyanetAylikVakitleriKaydet)
        self.vakitleri_goster_pushButton.clicked.connect(self.vakitleriTopluGoster)
        self.bolgesel_ayarlar_kaydet_pushButton.clicked.connect(self.ayarlariKaydet)
        self.bolgesel_ayarlar_varsayilan_pushButton.clicked.connect(self.varsayilanAyarlar)
        self.ayarlar_kaydet_pushButton.clicked.connect(self.ayarlariKaydet)
        self.ayarlar_varsayilan_pushButton.clicked.connect(self.varsayilanAyarlar)
        self.sabah_toolButton.clicked.connect(self.sesDosyasiEkleSabah)
        self.ogle_toolButton.clicked.connect(self.sesDosyasiEkleOgle)
        self.ikindi_toolButton.clicked.connect(self.sesDosyasiEkleIkindi)
        self.aksam_toolButton.clicked.connect(self.sesDosyasiEkleAksam)
        self.yatsi_toolButton.clicked.connect(self.sesDosyasiEkleYatsi)
        self.dua_toolButton.clicked.connect(self.sesDosyasiEkleDua)
        self.sela_toolButton.clicked.connect(self.sesDosyasiEkleSela)
        self.uyari_toolButton.clicked.connect(self.sesDosyasiEkleUyari)
        self.ses_kaydet_pushButton.clicked.connect(self.ayarlariKaydet)
        self.ses_varsayilan_pushButton.clicked.connect(self.sesVarsayilanAyarlar)
        self.trayIcon.activated.connect(self.iconActivated)
        self.pushButton_android.clicked.connect(self.risaleAppForAndroid)
        self.pushButton_ios.clicked.connect(self.risaleAppForIOS)
        self.horizontalSlider.valueChanged.connect(self.sesSeviyesiDegisti)
        self.tabWidget.currentChanged.connect(self._sekmeDegisti)
        
    def risaleAppForAndroid(self):
        url = "https://play.google.com/store/apps/details?id=com.nesil.risalei_nur&hl=tr&gl=US"
        self.siteAc(url)
        
    def risaleAppForIOS(self):
        url = "https://apps.apple.com/az/app/risale-i-nur-k%C3%BClliyat%C4%B1-s%C3%B6z/id814437480"
        self.siteAc(url)
        
    def trayIconGoster(self):
        if self.ikon_goster_checkBox.isChecked():
            self.trayIcon.show()
            self.sistem_tepsisinde_baslat_checkBox.setEnabled(True)
            if self.sistem_tepsisinde_baslat_checkBox.isChecked():
                self.hide()
            else:
                self.show()
        else:
            self.sistem_tepsisinde_baslat_checkBox.setEnabled(False)
            self.sistem_tepsisinde_baslat_checkBox.setChecked(False)
            self.trayIcon.hide()
            self.show()

    def ekrandaOrtala(self):
        self.move((QtWidgets.QDesktopWidget().screenGeometry().width() - self.geometry().width()) // 2,
                  (QtWidgets.QDesktopWidget().screenGeometry().height() - self.geometry().height()) // 2)

    def calismaDizininiHazirla(self):
        if QtCore.QDir.exists(QtCore.QDir(self.ayarlarKonum)) == False:
            QtCore.QDir(QtCore.QDir.homePath()).mkpath(".PyVakitci-" + str(self.version))

        self.calismaDizini = self.ayarlarKonum
        PyVakitci.veritabaniDosyasi = str(self.calismaDizini + "/diyanet.db")
       
    def varsayilanAyarlariKopyala(self):
        kaynakAyarDosyasi = self.programinKonumu + "/ontanimli_ayarlar.ini"
        self.ontanimliAyarDosyasiYeni = self.tempPath + "/ontanimli_ayarlar.ini"
       
        if QtCore.QFile.exists(QtCore.QFile(kaynakAyarDosyasi)):
            shutil.copy(kaynakAyarDosyasi, self.ontanimliAyarDosyasiYeni)
           
        self.ontanimliAyarDosyasi = self.ontanimliAyarDosyasiYeni
        
    def mesajKutusunuGoster(self, surumNotlari, version):
        messageBox = QtWidgets.QMessageBox()
        ikon = QtGui.QIcon()
        ikon.addPixmap(QtGui.QPixmap(":resimler/cami.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        messageBox.setWindowIcon(ikon)
        messageBox.setIcon(QtWidgets.QMessageBox.Question)
        messageBox.setWindowTitle("Güncelleme")
        
        yesButton = QtWidgets.QPushButton('Yeni Kurulum (23 mb)')
        noButton = QtWidgets.QPushButton('Güncelleme (2 mb)')
        cancelCheckBox = QtWidgets.QCheckBox("Bir Daha Gösterme")
        
        messageBox.addButton(yesButton, QtWidgets.QMessageBox.YesRole)        
        messageBox.addButton(noButton, QtWidgets.QMessageBox.NoRole)
        messageBox.addButton(cancelCheckBox, QtWidgets.QMessageBox.RejectRole)
        messageBox.setDefaultButton(noButton)
        
        version = str(version)
        surumNotlari = str(surumNotlari)
        
        messageBox.setText("PyVakitci programının yeni sürümü mevcut.\n\n" +
                           "PyVakitci " + version + " sürümündeki yenilikler:\n\n" +
                           surumNotlari + "\n\n" +
                           "PyVakitci " + version + " sürümünü indirmek ister misiniz?")

        messageBox.exec_()
        
        if messageBox.clickedButton() == yesButton:
            url = "https://goo.gl/cPxj3s"
            self.siteAc(url)
        
        if messageBox.clickedButton() == noButton:
            url = "https://goo.gl/QBySgt"
            self.siteAc(url)
            
        if messageBox.clickedButton() == cancelCheckBox:
            self.otomatik_guncellestir_checkBox.setChecked(False)
            self.ayarlariKaydet()

    def otomatikGuncellestir(self):
        if self.otomatik_guncellestir_checkBox.isChecked():
            url = "https://sourceforge.net/projects/pyvakitci2011/files/"
            file_url = "https://master.dl.sourceforge.net/project/pyvakitci2011"
            
            if self.siteyeBaglantiVarMi(url):
                try:
                    site = urllib.request.urlopen(file_url + "/Version", timeout=5)
                    version = float(site.read())
                    
                    if self.version < version:
                        site = urllib.request.urlopen(file_url + "/SurumNotlari", timeout=5)
                        surumNotlari = site.read().decode('utf-8')
                        self.mesajKutusunuGoster(surumNotlari, version)
                except Exception:
                    pass

    # trayIcon a tıklandığında program görünür/görünmez olur.
    def iconActivated(self, reason):
        if reason in (QtWidgets.QSystemTrayIcon.Trigger, QtWidgets.QSystemTrayIcon.DoubleClick):
            try:
                if self.isVisible():
                    self.hide()
                else:
                    self.tabWidget.setCurrentIndex(0)
                    self.show()
            except AttributeError:
                pass

    def fusionGorunumu(self):
        self.gorunumuDegistir("Fusion")

    def windowsXPGorunumu(self):
        self.gorunumuDegistir("Windows")

    def windowsVistaGorunumu(self):
        self.gorunumuDegistir("windowsvista")

    def gorunumuDegistir(self, lookAndFeel):
        self.gorunum = lookAndFeel
        QtWidgets.QApplication.setStyle(QtWidgets.QStyleFactory.create(self.gorunum))
        if hasattr(self, "actionFusion"):
            if lookAndFeel == "Fusion":
                self.actionFusion.setChecked(True)
            elif lookAndFeel == "windowsvista":
                self.actionWindowsVista.setChecked(True)
            elif lookAndFeel == "Windows":
                self.actionWindowsXP.setChecked(True)
        if hasattr(self, "aktifTema") and self.aktifTema in TEMALAR:
            QtWidgets.QApplication.instance().setStyleSheet(TEMALAR[self.aktifTema]["qss"])
        if hasattr(self, "settings") and self.settings is not None:
            self.settings.setValue("Ayarlar/gorunum", self.gorunum)

    def modernArayuzuHazirla(self):
        # 1. Animasyonlu Vakit Saati (Kadran) Widget'ı
        self.vakitSaatiWidget = VakitSaatiWidget(self.pyvakitci_tab)
        self.vakitSaatiWidget.setGeometry(QtCore.QRect(10, 8, 306, 378))
        self.vakitSaatiWidget.show()

        # 2. Dinamik Gökyüzü & Hilal Widget'ı
        self.gokyuzuWidget = GokyuzuWidget(self.pyvakitci_tab)
        self.gokyuzuWidget.setGeometry(QtCore.QRect(10, 8, 306, 378))
        self.gokyuzuWidget.hide()

        # 3. Modern Kart Paneli
        self.modern_panel = QtWidgets.QWidget(self.pyvakitci_tab)
        self.modern_panel.setGeometry(QtCore.QRect(10, 8, 306, 378))
        self.modern_panel.setObjectName("modern_panel")
        self.modern_panel.hide()

        # 30 FPS Akıcı Animasyon Zamanlayıcısı (Pürüzsüz İbre Hareketi & Parıltı Efekti)
        self.timerAnimasyon = QtCore.QTimer(self)
        self.timerAnimasyon.timeout.connect(self._animasyonTick)
        self.timerAnimasyon.start(33)

        # Üst Konum ve Tarih/Saat Kartı (Modern Kartlar için)
        self.modern_ust_kart = QtWidgets.QFrame(self.modern_panel)
        self.modern_ust_kart.setGeometry(QtCore.QRect(2, 2, 302, 74))
        self.modern_ust_kart.setObjectName("modern_ust_kart")

        self.modern_ulke_label = QtWidgets.QLabel(self.modern_ust_kart)
        self.modern_ulke_label.setGeometry(QtCore.QRect(14, 12, 145, 16))
        self.modern_ulke_label.setObjectName("modern_ulke_label")

        self.modern_sehir_label = QtWidgets.QLabel(self.modern_ust_kart)
        self.modern_sehir_label.setGeometry(QtCore.QRect(14, 29, 155, 24))
        self.modern_sehir_label.setObjectName("modern_sehir_label")

        self.modern_dini_gun_label = QtWidgets.QLabel(self.modern_ust_kart)
        self.modern_dini_gun_label.setGeometry(QtCore.QRect(14, 53, 155, 16))
        self.modern_dini_gun_label.setObjectName("modern_dini_gun_label")

        self.modern_saat_label = QtWidgets.QLabel(self.modern_ust_kart)
        self.modern_saat_label.setGeometry(QtCore.QRect(202, 8, 86, 22))
        self.modern_saat_label.setAlignment(QtCore.Qt.AlignCenter)
        self.modern_saat_label.setObjectName("modern_saat_label")

        self.modern_miladi_label = QtWidgets.QLabel(self.modern_ust_kart)
        self.modern_miladi_label.setGeometry(QtCore.QRect(140, 33, 148, 17))
        self.modern_miladi_label.setAlignment(QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter)
        self.modern_miladi_label.setObjectName("modern_miladi_label")

        self.modern_hicri_label = QtWidgets.QLabel(self.modern_ust_kart)
        self.modern_hicri_label.setGeometry(QtCore.QRect(120, 51, 168, 17))
        self.modern_hicri_label.setAlignment(QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter)
        self.modern_hicri_label.setObjectName("modern_hicri_label")

        # Aktif Vakit ve Geri Sayım Kartı
        self.modern_sayac_kart = QtWidgets.QFrame(self.modern_panel)
        self.modern_sayac_kart.setGeometry(QtCore.QRect(2, 82, 302, 68))
        self.modern_sayac_kart.setObjectName("modern_sayac_kart")

        self.modern_vakit_durum_label = QtWidgets.QLabel(self.modern_sayac_kart)
        self.modern_vakit_durum_label.setGeometry(QtCore.QRect(14, 12, 165, 20))
        self.modern_vakit_durum_label.setObjectName("modern_vakit_durum_label")

        self.modern_kalan_baslik_label = QtWidgets.QLabel(self.modern_sayac_kart)
        self.modern_kalan_baslik_label.setGeometry(QtCore.QRect(14, 36, 165, 18))
        self.modern_kalan_baslik_label.setObjectName("modern_kalan_baslik_label")

        self.modern_kalan_sure_label = QtWidgets.QLabel(self.modern_sayac_kart)
        self.modern_kalan_sure_label.setGeometry(QtCore.QRect(168, 12, 120, 44))
        self.modern_kalan_sure_label.setAlignment(QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter)
        self.modern_kalan_sure_label.setObjectName("modern_kalan_sure_label")

        # Altı Vakit Satır Kartları
        vakitTanimlari = [
            ("imsak", "İmsak"),
            ("gunes", "Güneş"),
            ("ogle", "Öğle"),
            ("ikindi", "İkindi"),
            ("aksam", "Akşam"),
            ("yatsi", "Yatsı"),
        ]
        self.modern_vakit_satirlari = {}
        baslangicY = 156
        satirYukseklik = 34
        satirAralik = 37

        for idx, (anahtar, baslik) in enumerate(vakitTanimlari):
            y = baslangicY + idx * satirAralik
            satirFrame = QtWidgets.QFrame(self.modern_panel)
            satirFrame.setGeometry(QtCore.QRect(2, y, 302, satirYukseklik))
            satirFrame.setProperty("vakitSatiri", "true")
            satirFrame.setProperty("aktif", "false")

            adLabel = QtWidgets.QLabel(baslik, satirFrame)
            adLabel.setGeometry(QtCore.QRect(14, 0, 100, satirYukseklik))
            adLabel.setAlignment(QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter)
            adLabel.setProperty("vakitAd", "true")
            adLabel.setProperty("aktif", "false")

            badgeLabel = QtWidgets.QLabel("AKTİF", satirFrame)
            badgeLabel.setGeometry(QtCore.QRect(122, 8, 56, 18))
            badgeLabel.setAlignment(QtCore.Qt.AlignCenter)
            badgeLabel.setProperty("vakitBadge", "true")
            badgeLabel.hide()

            saatLabel = QtWidgets.QLabel("--:--", satirFrame)
            saatLabel.setGeometry(QtCore.QRect(195, 0, 93, satirYukseklik))
            saatLabel.setAlignment(QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter)
            saatLabel.setProperty("vakitSaat", "true")
            saatLabel.setProperty("aktif", "false")

            self.modern_vakit_satirlari[anahtar] = {
                "frame": satirFrame,
                "ad": adLabel,
                "badge": badgeLabel,
                "saat": saatLabel,
            }

    def _havaDurumuIkonuOlustur(self):
        pix = QtGui.QPixmap(64, 64)
        pix.fill(QtCore.Qt.transparent)
        p = QtGui.QPainter(pix)
        p.setRenderHint(QtGui.QPainter.Antialiasing)
        p.setRenderHint(QtGui.QPainter.SmoothPixmapTransform)

        # Yuvarlatılmış kare arka plan (Diğer sekme ikonlarıyla birebir uyumlu)
        bgGrad = QtGui.QLinearGradient(0, 0, 0, 64)
        bgGrad.setColorAt(0.0, QtGui.QColor("#38bdf8"))
        bgGrad.setColorAt(1.0, QtGui.QColor("#0284c7"))
        p.setPen(QtCore.Qt.NoPen)
        p.setBrush(bgGrad)
        p.drawRoundedRect(QtCore.QRectF(4, 4, 56, 56), 14, 14)

        # Güneş
        sunGrad = QtGui.QRadialGradient(24, 24, 14)
        sunGrad.setColorAt(0.0, QtGui.QColor("#fef08a"))
        sunGrad.setColorAt(0.7, QtGui.QColor("#facc15"))
        sunGrad.setColorAt(1.0, QtGui.QColor("#eab308"))
        p.setBrush(sunGrad)
        p.drawEllipse(QtCore.QPointF(24, 24), 11, 11)

        # Beyaz Bulut
        p.setBrush(QtGui.QColor("#ffffff"))
        bulut = QtGui.QPainterPath()
        bulut.setFillRule(QtCore.Qt.WindingFill)
        bulut.addRoundedRect(QtCore.QRectF(16, 31, 34, 15), 7.5, 7.5)
        bulut.addEllipse(QtCore.QPointF(26, 29), 9, 9)
        bulut.addEllipse(QtCore.QPointF(37, 27), 10, 10)
        p.drawPath(bulut)
        p.end()

        ikon = QtGui.QIcon()
        ikon.addPixmap(pix, QtGui.QIcon.Normal, QtGui.QIcon.Off)
        return ikon

    def ayarlarVeHavaDurumuSekmeleriniHazirla(self):
        # 1) Bölgesel Ayarlar kutusunu 3. sekmedeki Ayarlar sekmesine taşı
        self.bolgesel_ayarlar_groupBox.setParent(self.ayarlar_tab)

        # Ayarlar sekmesinin üst kısmına 2'li alt sekme seçici buton grubu ekle
        self.ayarAltSekmeGrup = QtWidgets.QButtonGroup(self.ayarlar_tab)
        self.ayarAltSekmeGrup.setExclusive(True)

        self.btnGenelAyarlar = QtWidgets.QPushButton("⚙️ Genel Ayarlar", self.ayarlar_tab)
        self.btnGenelAyarlar.setGeometry(QtCore.QRect(13, 6, 148, 27))
        self.btnGenelAyarlar.setCheckable(True)
        self.btnGenelAyarlar.setChecked(True)
        self.btnGenelAyarlar.setCursor(QtCore.Qt.PointingHandCursor)
        self.btnGenelAyarlar.setProperty("ayarAltSekme", "true")

        self.btnBolgeselAyarlar = QtWidgets.QPushButton("🌍 Bölgesel Ayarlar", self.ayarlar_tab)
        self.btnBolgeselAyarlar.setGeometry(QtCore.QRect(166, 6, 148, 27))
        self.btnBolgeselAyarlar.setCheckable(True)
        self.btnBolgeselAyarlar.setCursor(QtCore.Qt.PointingHandCursor)
        self.btnBolgeselAyarlar.setProperty("ayarAltSekme", "true")

        self.ayarAltSekmeGrup.addButton(self.btnGenelAyarlar, 0)
        self.ayarAltSekmeGrup.addButton(self.btnBolgeselAyarlar, 1)
        self.btnGenelAyarlar.clicked.connect(lambda: self._ayarAltSekmeDegistir(0))
        self.btnBolgeselAyarlar.clicked.connect(lambda: self._ayarAltSekmeDegistir(1))

        # Her iki GroupBox'ın üst başlık yazılarını kaldır ve alt sekme butonlarının altına hizala
        self.ayarlar_groupBox.setTitle("")
        self.bolgesel_ayarlar_groupBox.setTitle("")

        # Bölgesel Ayarlar'daki tüm bileşenlerin yazı boyutunu Genel Ayarlar (Arial 11pt) ile birebir aynı yap
        ayarFont = QtGui.QFont()
        ayarFont.setFamily("Arial")
        ayarFont.setPointSize(11)
        self.bolgesel_ayarlar_groupBox.setFont(ayarFont)
        for w in (
            self.bolgesel_ayarlar_ulke_label,
            self.bolgesel_ayarlar_sehir_label,
            self.bolgesel_ayarlar_ilce_label,
            self.ulkeler_comboBox,
            self.sehirler_comboBox,
            self.ilceler_comboBox,
            self.bolgesel_ayarlar_varsayilan_pushButton,
            self.bolgesel_ayarlar_kaydet_pushButton,
        ):
            w.setFont(ayarFont)

        ortakRect = QtCore.QRect(13, 38, 301, 348)
        self.ayarlar_groupBox.setMinimumSize(QtCore.QSize(280, 348))
        self.ayarlar_groupBox.setGeometry(ortakRect)
        self.bolgesel_ayarlar_groupBox.setMinimumSize(QtCore.QSize(280, 348))
        self.bolgesel_ayarlar_groupBox.setGeometry(ortakRect)

        # Genel Ayarlar içindeki kontrollerin yerleşimi (başlık kalktığı için üstten dengeli boşluk)
        self.otomatik_calistir_checkBox.setGeometry(QtCore.QRect(10, 12, 285, 22))
        self.ikon_goster_checkBox.setGeometry(QtCore.QRect(10, 36, 285, 22))
        self.sistem_tepsisinde_baslat_checkBox.setGeometry(QtCore.QRect(10, 60, 285, 22))
        self.otomatik_guncellestir_checkBox.setGeometry(QtCore.QRect(10, 84, 285, 22))
        self.her_zaman_ustte_checkBox.setGeometry(QtCore.QRect(10, 108, 285, 22))
        self.besmele_ile_basla_checkBox.setGeometry(QtCore.QRect(10, 132, 285, 22))
        self.stopify_duraklat_checkBox.setGeometry(QtCore.QRect(10, 156, 137, 22))
        self.winamp_duraklat_checkBox.setGeometry(QtCore.QRect(153, 156, 141, 22))
        self.erken_uyari_checkBox.setGeometry(QtCore.QRect(10, 180, 225, 22))
        self.erken_uyari_spinBox.setGeometry(QtCore.QRect(240, 181, 42, 21))
        self.erken_uyari_sela_checkBox.setGeometry(QtCore.QRect(10, 204, 225, 22))
        self.erken_uyari_sela_spinBox.setGeometry(QtCore.QRect(240, 205, 42, 21))
        self.hicri_gun_ekle_checkBox.setGeometry(QtCore.QRect(10, 228, 225, 22))
        self.hicri_tarih_gun_ekle_spinBox.setGeometry(QtCore.QRect(240, 228, 42, 21))
        self.vakitleri_goster_pushButton.setGeometry(QtCore.QRect(21, 254, 263, 25))
        self.vakitleri_guncelle_pushButton.setGeometry(QtCore.QRect(21, 283, 263, 25))

        self.ayarlar_varsayilan_pushButton.setGeometry(QtCore.QRect(15, 313, 126, 27))
        self.ayarlar_kaydet_pushButton.setGeometry(QtCore.QRect(155, 313, 126, 27))

        # Bölgesel Ayarlar içindeki etiket, açılır kutu ve buton yerleşimi
        self.bolgesel_ayarlar_ulke_label.setGeometry(QtCore.QRect(15, 28, 65, 23))
        self.ulkeler_comboBox.setGeometry(QtCore.QRect(80, 25, 205, 28))
        self.bolgesel_ayarlar_sehir_label.setGeometry(QtCore.QRect(15, 73, 65, 23))
        self.sehirler_comboBox.setGeometry(QtCore.QRect(80, 70, 205, 28))
        self.bolgesel_ayarlar_ilce_label.setGeometry(QtCore.QRect(15, 118, 65, 23))
        self.ilceler_comboBox.setGeometry(QtCore.QRect(80, 115, 205, 28))

        self.bolgesel_ayarlar_dil_label = QtWidgets.QLabel(self.bolgesel_ayarlar_groupBox)
        self.bolgesel_ayarlar_dil_label.setGeometry(QtCore.QRect(15, 163, 65, 23))
        self.bolgesel_ayarlar_dil_label.setFont(QtGui.QFont("Arial", 10))
        self.bolgesel_ayarlar_dil_label.setObjectName("bolgesel_ayarlar_dil_label")
        self.bolgesel_ayarlar_dil_label.setText(tr("dil"))

        self.dil_comboBox = QtWidgets.QComboBox(self.bolgesel_ayarlar_groupBox)
        self.dil_comboBox.setGeometry(QtCore.QRect(80, 160, 205, 28))
        self.dil_comboBox.setFont(QtGui.QFont("Arial", 9))
        self.dil_comboBox.setObjectName("dil_comboBox")
        for code, name in LANGUAGES.items():
            self.dil_comboBox.addItem(name, code)
        idx = list(LANGUAGES.keys()).index(self.dil) if self.dil in LANGUAGES else 0
        self.dil_comboBox.setCurrentIndex(idx)
        self.dil_comboBox.currentIndexChanged.connect(self.dilDegisti)

        self.bolgesel_ayarlar_varsayilan_pushButton.setGeometry(QtCore.QRect(15, 313, 126, 27))
        self.bolgesel_ayarlar_kaydet_pushButton.setGeometry(QtCore.QRect(155, 313, 126, 27))

        self._ayarAltSekmeDegistir(0)

        # 2) 2. Sekmeyi Seçilen İlin Hava Durumu ekranına dönüştür
        self.havaDurumuWidget = HavaDurumuWidget(self.bolgesel_ayarlar_tab)
        self.havaDurumuWidget.setGeometry(QtCore.QRect(10, 8, 306, 378))
        self.havaDurumuWidget.show()

        self.tabWidget.setTabIcon(1, self._havaDurumuIkonuOlustur())
        self.tabWidget.setTabToolTip(0, tr("tab_saat"))
        self.tabWidget.setTabToolTip(1, tr("tab_hava"))
        self.tabWidget.setTabToolTip(2, tr("tab_ayarlar"))
        self.tabWidget.setTabToolTip(3, tr("tab_ses"))
        self.tabWidget.setTabToolTip(4, tr("tab_linkler"))

    def _ayarAltSekmeDegistir(self, indeks):
        if indeks == 0:
            self.btnGenelAyarlar.setChecked(True)
            self.ayarlar_groupBox.show()
            self.bolgesel_ayarlar_groupBox.hide()
        else:
            self.btnBolgeselAyarlar.setChecked(True)
            self.ayarlar_groupBox.hide()
            self.bolgesel_ayarlar_groupBox.show()

    def _animasyonTick(self):
        if not self.isVisible():
            return
        aktifSekme = self.tabWidget.currentIndex()
        if aktifSekme == 0:
            if self.arayuzModu == "saat" and hasattr(self, "vakitSaatiWidget"):
                self.vakitSaatiWidget.animasyonAdimi()
            elif self.arayuzModu == "gokyuzu" and hasattr(self, "gokyuzuWidget"):
                self.gokyuzuWidget.animasyonAdimi()
        elif aktifSekme == 1 and hasattr(self, "havaDurumuWidget"):
            self.havaDurumuWidget.animasyonAdimi()

    def arayuzModunuUygula(self, mod, kaydet=False):
        if mod not in ("saat", "gokyuzu", "kartlar", "modern", "klasik"):
            mod = "saat"
        if mod == "modern":
            mod = "kartlar"
        self.arayuzModu = mod

        # Tüm panelleri başlangıçta gizle
        self.bilgiler_groupBox.hide()
        self.namaz_vakitleri_groupBox.hide()
        self.modern_panel.hide()
        if hasattr(self, "vakitSaatiWidget"):
            self.vakitSaatiWidget.hide()
        if hasattr(self, "gokyuzuWidget"):
            self.gokyuzuWidget.hide()

        if mod == "saat":
            if hasattr(self, "vakitSaatiWidget"):
                self.vakitSaatiWidget.show()
            if hasattr(self, "actionVakitSaati"):
                self.actionVakitSaati.setChecked(True)
        elif mod == "gokyuzu":
            if hasattr(self, "gokyuzuWidget"):
                self.gokyuzuWidget.show()
            if hasattr(self, "actionGokyuzu"):
                self.actionGokyuzu.setChecked(True)
        elif mod == "kartlar":
            self.modern_panel.show()
            if hasattr(self, "actionModernKartlar"):
                self.actionModernKartlar.setChecked(True)
        else: # klasik
            self.bilgiler_groupBox.show()
            self.namaz_vakitleri_groupBox.show()
            if hasattr(self, "actionKlasikGorunum"):
                self.actionKlasikGorunum.setChecked(True)

        self._gorselVerileriGuncelle()

        if kaydet and hasattr(self, "settings") and self.settings is not None:
            self.settings.setValue("Ayarlar/arayuz_modu", self.arayuzModu)

    def _gorselVerileriGuncelle(self):
        vakitler = {
            "imsak": getattr(Vakitler, "imsak", "--:--"),
            "gunes": getattr(Vakitler, "gunes", "--:--"),
            "ogle": getattr(Vakitler, "ogle", "--:--"),
            "ikindi": getattr(Vakitler, "ikindi", "--:--"),
            "aksam": getattr(Vakitler, "aksam", "--:--"),
            "yatsi": getattr(Vakitler, "yatsi", "--:--"),
        }
        aktifAnahtar = getattr(self, "_sonAktifModernVakit", "imsak") or "imsak"
        sonrakiVakit = getattr(self, "sonraki_vakit_label", None)
        sonrakiStr = sonrakiVakit.text() if sonrakiVakit else ""
        kalanStr = self.kalan_sure_label.text() if hasattr(self, "kalan_sure_label") else "00:00:00"
        parcalar = kalanStr.split(":")
        if len(parcalar) == 3 and len(parcalar[0]) == 1:
            kalanStr = "0" + kalanStr

        saatStr = QtCore.QTime.currentTime().toString("hh:mm:ss")
        self.saat = saatStr
        miladi = getattr(self, "tarih", "") or QtCore.QDate.currentDate().toString("d MMMM yyyy")
        hicri = getattr(self, "hicriTarih", "")
        diniGun = getattr(self, "dini_gun_label", None)
        diniGunStr = diniGun.text() if diniGun else ""
        ulke = getattr(self, "ulke", "") or "TÜRKİYE"
        sehir = getattr(self, "sehir", "") or ""
        seciliYer = getattr(self, "seciliYer", "") or getattr(self, "ilce", "") or sehir

        if hasattr(self, "vakitSaatiWidget"):
            self.vakitSaatiWidget.veriGuncelle(
                ulke, sehir, seciliYer,
                miladi, hicri, diniGunStr, saatStr,
                aktifAnahtar, sonrakiStr, kalanStr, vakitler, getattr(self, "aktifTema", "varsayilan")
            )
        if hasattr(self, "gokyuzuWidget"):
            self.gokyuzuWidget.veriGuncelle(
                ulke, sehir, seciliYer,
                miladi, hicri, diniGunStr, saatStr,
                aktifAnahtar, sonrakiStr, kalanStr, vakitler, getattr(self, "aktifTema", "varsayilan")
            )
        if hasattr(self, "havaDurumuWidget"):
            self.havaDurumuWidget.temaAyarla(getattr(self, "aktifTema", "varsayilan"))

    def _modernVakitleriGuncelle(self, aktifAnahtar):
        self._sonAktifModernVakit = aktifAnahtar
        self._gorselVerileriGuncelle()

        if not hasattr(self, "modern_vakit_satirlari"):
            return

        degerler = {
            "imsak": Vakitler.imsak,
            "gunes": Vakitler.gunes,
            "ogle": Vakitler.ogle,
            "ikindi": Vakitler.ikindi,
            "aksam": Vakitler.aksam,
            "yatsi": Vakitler.yatsi,
        }

        for anahtar, bilesenler in self.modern_vakit_satirlari.items():
            bilesenler["saat"].setText(degerler.get(anahtar) or "--:--")
            aktifMi = (anahtar == aktifAnahtar)
            aktifStr = "true" if aktifMi else "false"

            if bilesenler["frame"].property("aktif") != aktifStr:
                bilesenler["frame"].setProperty("aktif", aktifStr)
                bilesenler["ad"].setProperty("aktif", aktifStr)
                bilesenler["saat"].setProperty("aktif", aktifStr)
                bilesenler["badge"].setVisible(aktifMi)

                for w in (bilesenler["frame"], bilesenler["ad"], bilesenler["saat"]):
                    w.style().unpolish(w)
                    w.style().polish(w)
                    w.update()

    def linkRenkleriniAyarla(self, renk):
        link_labelleri = [
            self.site_01_label, self.site_04_label, self.site_05_label,
            self.site_06_label, self.site_07_label, self.site_11_label,
            self.site_12_label
        ]
        for lbl in link_labelleri:
            txt = lbl.text()
            if "color:" in txt:
                yeni_txt = re.sub(r'color:\s*#[0-9a-fA-F]+', f'color:{renk}', txt)
                lbl.setText(yeni_txt)
            else:
                yeni_txt = re.sub(r'(<span style="[^"]*)"', rf'\1 color:{renk};"', txt)
                lbl.setText(yeni_txt)

    def temayiUygula(self, temaKey, kaydet=False):
        if temaKey not in TEMALAR:
            temaKey = "varsayilan"
        self.aktifTema = temaKey
        qss = TEMALAR[temaKey]["qss"]
        QtWidgets.QApplication.instance().setStyleSheet(qss)

        # Varsayılan temada linkler mavi (#0000ff), diğer tüm temalarda beyaz (#ffffff)
        if temaKey == "varsayilan":
            self.linkRenkleriniAyarla("#0000ff")
        else:
            self.linkRenkleriniAyarla("#ffffff")

        if temaKey in self.temaActions:
            self.temaActions[temaKey].setChecked(True)

        if kaydet and hasattr(self, "settings") and self.settings is not None:
            self.settings.setValue("Ayarlar/tema", self.aktifTema)

        self._gorselVerileriGuncelle()

    # Visual Basic Script koduyla kısayol dosyası oluşturularak başlangıç dizinine ekleniyor.
    def vbScriptKodu(self, durum):
        winKonum = self.programinKonumu.replace("/", "\\")
        if durum:
            kod = """
                    sub KisayolOlustur()
                      dim shell, baslangicDizini, kisayolDosyasi

                      set shell = CreateObject("WScript.Shell")
                      baslangicDizini = shell.SpecialFolders("Startup")
                      set kisayolDosyasi = shell.CreateShortcut(baslangicDizini + "\\PyVakitci.lnk")
                      kisayolDosyasi.TargetPath = "%s\\Program.py"
                      kisayolDosyasi.WindowStyle = 1
                      kisayolDosyasi.WorkingDirectory = "%s\\"
                      kisayolDosyasi.Save
                    end sub

                    call KisayolOlustur()
                    """ %(winKonum, winKonum)
        else:
            kod = """
                sub DosyaSil()
                    dim kisayolDosyasi, objFSO, shell

                    Set shell = CreateObject("WScript.Shell")
                    kisayolDosyasi = shell.SpecialFolders("Startup") + "\\PyVakitci.lnk"

                    Set objFSO = CreateObject("Scripting.FileSystemObject")
                    If objFSO.FileExists(kisayolDosyasi) Then
                        objFSO.DeleteFile(kisayolDosyasi)
                    End If

                end sub

                call DosyaSil()
                """
        return kod

    def otomatikCalistir(self):
        isletimSistemi = platform.system()
        try:
            windowsVersion = int(platform.version().split(".")[0])
        except Exception:
            windowsVersion = 10
       
        if isletimSistemi == "Windows":
            if windowsVersion >= 6:
                if self.otomatik_calistir_checkBox.isChecked():
                    veriler = self.vbScriptKodu(True)
           
                if self.otomatik_calistir_checkBox.isChecked() == False:
                    veriler = self.vbScriptKodu(False)
           
                self.visualBasicScriptCalistir(veriler)
               
            if windowsVersion < 6:
                programinKonumu = (self.programinKonumu + "/Program.py").replace("/", os.sep)
                regYolu = "Software/Microsoft/Windows/CurrentVersion/Run".replace("/", os.sep)
                keyEx = winreg.OpenKeyEx(winreg.HKEY_LOCAL_MACHINE, regYolu, 0, winreg.KEY_ALL_ACCESS)
           
                if self.otomatik_calistir_checkBox.isChecked():
                    winreg.SetValueEx(keyEx, "PyVakitci", 0, winreg.REG_SZ, programinKonumu)
           
                if self.otomatik_calistir_checkBox.isChecked() == False:
                    try:
                        winreg.DeleteValue(keyEx, "PyVakitci")
                    except:
                        pass
                   
                winreg.CloseKey(keyEx)

    def visualBasicScriptCalistir(self, veriler):
        dosya = self.tempPath + "/pyvakitci.vbs"
        
        try:
            with open(dosya, 'w') as dosyaIslemi:
                dosyaIslemi.write(veriler)
       
            if QtCore.QFile.exists(QtCore.QFile(dosya)):
                import subprocess
                subprocess.run(["wscript.exe", "//B", "//Nologo", dosya], timeout=5, check=False)
                QtCore.QFile(dosya).remove()
        except Exception:
            pass

    def besmeleyleBasla(self):
        if self.besmele_ile_basla_checkBox.isChecked():
            self.sesDosyasiniAc(self.besmeleDosyasi)
            
    def herZamanUstte(self):
        if self.her_zaman_ustte_checkBox.isChecked():
            if self.windowFlag is None:
                self.windowFlag = self.windowFlags()
            self.setWindowFlags(self.windowFlag | QtCore.Qt.WindowStaysOnTopHint)
        else:
            if self.windowFlag is not None:
                self.setWindowFlags(self.windowFlag & ~QtCore.Qt.WindowStaysOnTopHint)
        
        if not self.isVisible():
            self.show()

        self.ekrandaOrtala()

    def ezanOkuKontrol(self):
        if self.sabah_ezani_oku_checkBox.isChecked() or +\
            self.ogle_ezani_oku_checkBox.isChecked() or  +\
            self.ikindi_ezani_oku_checkBox.isChecked() or +\
            self.aksam_ezani_oku_checkBox.isChecked() or +\
            self.yatsi_ezani_oku_checkBox.isChecked():
           
            self.dua_oku_checkBox.setEnabled(True)
        else:
            self.dua_oku_checkBox.setEnabled(False)
            self.dua_oku_checkBox.setChecked(False)

    def erkenUyari(self):
        if self.erken_uyari_checkBox.isChecked():
            self.erken_uyari_spinBox.setEnabled(True)
        else:
            self.erken_uyari_spinBox.setEnabled(False)
        self.uyari_sesi_checkBox.setEnabled(True)

    def karakterKontrol(self):
        QtWidgets.QMessageBox.warning(self, "Hata", "0 ve 99 arasında bir sayı girin.", "Tamam")

    def dosyaEkle(self, lineEdit):
        konum, _ = QtWidgets.QFileDialog.getOpenFileName(self, "Aç", "", "Ses Dosyaları (*.mp3 *.ogg *.wma *.wav)")

        if konum and len(konum) > 0:
            lineEdit.setText(konum)
           
    def winampVarMi(self):
        if self.winamp_duraklat_checkBox.isChecked():
            winampKonum = self.settings.value("Ayarlar/winamp_konum")
            
            if winampKonum in (None, "None", ""):
                QtWidgets.QMessageBox.information(self, "Bilgilendirme",
                                              "Winamp programını seçmeniz gerekiyor. \nProgram seçerken aşağıdaki konumlara bakın:\n\n" +
                                              "C:\\Program Files\\Winamp\n" +
                                              "C:\\Program Files (x86)\\Winamp\n" +
                                              "C:\\Program Dosyaları (x86)\\Winamp\n" +
                                              "C:\\Program Dosyaları\\Winamp\n",
                                              QtWidgets.QMessageBox.Ok)
                konum, _ = QtWidgets.QFileDialog.getOpenFileName(self, "Aç", "", "Exe Dosyaları (winamp.exe)")
       
                if konum and len(konum) > 0:
                    self.settings.setValue("Ayarlar/winamp_konum", konum)
                    self.winamp_duraklat_checkBox.setChecked(True)
                    return True
                else:
                    self.winamp_duraklat_checkBox.setChecked(False)
                    return False

    def sesDosyasiEkleSabah(self):
        self.dosyaEkle(self.sabah_lineEdit)

    def sesDosyasiEkleOgle(self):
        self.dosyaEkle(self.ogle_lineEdit)

    def sesDosyasiEkleIkindi(self):
        self.dosyaEkle(self.ikindi_lineEdit)

    def sesDosyasiEkleAksam(self):
        self.dosyaEkle(self.aksam_lineEdit)

    def sesDosyasiEkleYatsi(self):
        self.dosyaEkle(self.yatsi_lineEdit)

    def sesDosyasiEkleDua(self):
        self.dosyaEkle(self.dua_lineEdit)
       
    def sesDosyasiEkleSela(self):
        self.dosyaEkle(self.sela_lineEdit)

    def sesDosyasiEkleUyari(self):
        self.dosyaEkle(self.uyari_lineEdit)

    def sesAyari(self):
        if self.volume == None:
            self.volume = self.horizontalSlider.value()
            self.horizontalSlider.setValue(0)
        else:
            self.horizontalSlider.setValue(self.volume)
            self.volume = None

    def sesSeviyesiDegisti(self):
        self.volume = self.horizontalSlider.value()
        self.player.setVolume(self.volume)
        self.onizlemePlayer.setVolume(self.volume)

    def sesOynatmaButonlariniHazirla(self):
        satirlar = [
            (self.sabah_lineEdit, self.sabah_toolButton, self.sabah_ezani_oku_checkBox, 18),
            (self.ogle_lineEdit, self.ogle_toolButton, self.ogle_ezani_oku_checkBox, 56),
            (self.ikindi_lineEdit, self.ikindi_toolButton, self.ikindi_ezani_oku_checkBox, 94),
            (self.aksam_lineEdit, self.aksam_toolButton, self.aksam_ezani_oku_checkBox, 132),
            (self.yatsi_lineEdit, self.yatsi_toolButton, self.yatsi_ezani_oku_checkBox, 170),
            (self.dua_lineEdit, self.dua_toolButton, self.dua_oku_checkBox, 208),
            (self.sela_lineEdit, self.sela_toolButton, self.sela_oku_checkBox, 248),
            (self.uyari_lineEdit, self.uyari_toolButton, self.uyari_sesi_checkBox, 288),
        ]

        self.sesOynatButonlari = []
        for lineEdit, ekleBtn, checkBox, y in satirlar:
            lineEdit.setGeometry(QtCore.QRect(65, y - 1, 135, 25))
            ekleBtn.setGeometry(QtCore.QRect(205, y, 40, 23))

            oynatBtn = QtWidgets.QToolButton(self.ses_dosyalari_tab)
            oynatBtn.setGeometry(QtCore.QRect(248, y, 35, 23))
            oynatBtn.setIcon(self.playIcon)
            oynatBtn.setToolTip("Oynat")
            oynatBtn.setCursor(QtCore.Qt.PointingHandCursor)
            oynatBtn.clicked.connect(lambda checked=False, le=lineEdit, btn=oynatBtn: self.sesiOynatVeyaDurdur(le, btn))
            oynatBtn.show()
            self.sesOynatButonlari.append(oynatBtn)

            checkBox.setGeometry(QtCore.QRect(292, y + 2, 21, 20))

    def sesiOynatVeyaDurdur(self, lineEdit, btn):
        if self.suAnCalanSesButon == btn:
            self.onizlemePlayer.stop()
            btn.setIcon(self.playIcon)
            btn.setToolTip("Oynat")
            self.suAnCalanSesButon = None
            return

        if self.suAnCalanSesButon is not None:
            self.suAnCalanSesButon.setIcon(self.playIcon)
            self.suAnCalanSesButon.setToolTip("Oynat")
            self.onizlemePlayer.stop()
            self.suAnCalanSesButon = None

        dosyaYolu = lineEdit.text().strip()
        if not dosyaYolu or not os.path.exists(dosyaYolu):
            QtWidgets.QMessageBox.warning(self, "Hata", "Ses dosyası bulunamadı:\n" + dosyaYolu, "Tamam")
            return

        url = QtCore.QUrl.fromLocalFile(dosyaYolu)
        self.onizlemePlayer.setMedia(QtMultimedia.QMediaContent(url))
        self.onizlemePlayer.setVolume(self.horizontalSlider.value())
        self.onizlemePlayer.play()
        btn.setIcon(self.stopIcon)
        btn.setToolTip("Durdur")
        self.suAnCalanSesButon = btn

    def _onizlemeDurumDegisti(self, status):
        if status in (QtMultimedia.QMediaPlayer.EndOfMedia, QtMultimedia.QMediaPlayer.StoppedState):
            if self.suAnCalanSesButon is not None:
                self.suAnCalanSesButon.setIcon(self.playIcon)
                self.suAnCalanSesButon.setToolTip("Oynat")
                self.suAnCalanSesButon = None

    def _sekmeDegisti(self, index):
        if self.tabWidget.widget(index) != self.ses_dosyalari_tab:
            if self.suAnCalanSesButon is not None:
                self.onizlemePlayer.stop()
                self.suAnCalanSesButon.setIcon(self.playIcon)
                self.suAnCalanSesButon.setToolTip("Oynat")
                self.suAnCalanSesButon = None
           
    def saatGuncelleyiciyiCalistir(self):
        self.timerSaat = QtCore.QTimer()
        self.timerSaat.timeout.connect(self.saatiGuncelle)
        self.timerSaat.start(600)

    def saatiGuncelle(self):
        self.saat = QtCore.QTime().currentTime().toString("hh:mm:ss")
        self.saat_label.setText("Saat: " + self.saat)
        self.modern_saat_label.setText(self.saat)

        loc = QtCore.QLocale(get_language())
        self.tarih = loc.toString(QtCore.QDate.currentDate(), "d MMMM yyyy")
        self.miladi_tarih.setText(self.tarih)
        self.modern_miladi_label.setText(self.tarih)
        self._gorselVerileriGuncelle()

        if self.simdikiTarih != self.tarih:
            self.miladi_tarih.setText(self.tarih)
            self.modern_miladi_label.setText(self.tarih)
            self.simdikiTarih = self.tarih
            self.hicriTarihiGuncelle()
            self.vakitleriAl()

    def hicriTarihiBul(self):
        if self.hicri_gun_ekle_checkBox.isChecked():
            ilaveGun = int(self.hicri_tarih_gun_ekle_spinBox.text())
        else:
            ilaveGun = 0

        hedefTarih = QtCore.QDate().currentDate().addDays(ilaveGun)
        yil = hedefTarih.year()
        ay = hedefTarih.month()
        gun = hedefTarih.day()

        hicri_aylar_map = {
            "tr": ["Muharrem", "Safer", "Rebiülevvel", "Rebiülahir", "Cemaziyelevvel", "Cemaziyelahir",
                   "Receb", "Şaban", "Ramazan", "Şevval", "Zilkade", "Zilhicce"],
            "en": ["Muharram", "Safar", "Rabi' I", "Rabi' II", "Jumada I", "Jumada II",
                   "Rajab", "Sha'ban", "Ramadan", "Shawwal", "Dhu al-Qi'dah", "Dhu al-Hijjah"],
            "de": ["Muharram", "Safar", "Rabi' I", "Rabi' II", "Dschumada I", "Dschumada II",
                   "Radschab", "Scha'ban", "Ramadan", "Schawwal", "Dhu l-qa'da", "Dhu l-hiddscha"],
            "ru": ["Мухаррам", "Сафар", "Раби аль-авваль", "Раби ас-сани", "Джумада аль-уля", "Джумада ас-сания",
                   "Раджаб", "Шабан", "Рамадан", "Шавваль", "Зу-ль-када", "Зу-ль-хиджа"],
            "zh": ["穆哈兰姆月", "色法尔月", "赖比尔·敖外鲁月", "赖比尔·阿色尼月", "主马达·敖外鲁月", "主马达·阿色尼月",
                   "热哲卜月", "舍尔邦月", "赖买丹月", "闪瓦鲁月", "都尔喀尔德月", "都尔黑哲月"],
            "ja": ["ムハッラム", "サファル", "ラビー・アル＝アウワル", "ラビー・アッ＝サーニー", "ジュマーダー・アル＝ウーラー", "ジュマーダー・アッ＝サーニー",
                   "ラジャブ", "シャアバーン", "ラマダーン", "シャウワール", "ズー・アル＝カアダ", "ズー・アル＝ヒッジャ"]
        }
        self.hicriAylar = hicri_aylar_map.get(get_language(), hicri_aylar_map["tr"])

        self.calverter = Calverter()
        jd = self.calverter.gregorian_to_jd(yil, ay, gun)
        hicri_tarih = self.calverter.jd_to_islamic(jd)

        self.hicriYil = str(hicri_tarih[0])
        self.hicriAy = str(self.hicriAylar[hicri_tarih[1] - 1])
        self.hicriGun = str(hicri_tarih[2])

        self.hicriTarih = self.hicriGun + " " + self.hicriAy + " " + self.hicriYil

    def hicriTarihiGuncelle(self):
        self.hicriTarihiBul()
        self.hicri_tarih.setText(self.hicriTarih)
        self.modern_hicri_label.setText(self.hicriTarih)
        self.diniGunleriHatirlat()

    def diniGunleriHatirlat(self):
        hicriGunVeAy = str(self.hicriGun + " " + self.hicriAy)

        if self.diniGunlerVeGeceler.count(hicriGunVeAy) == 1:
            diniGun = str(self.diniGunlerVeGecelerKarsiligi[self.diniGunlerVeGeceler.index(hicriGunVeAy)])

            self.dini_gun_label.setText("<font color='#0174DF' size='3'>" + diniGun  + "</font>")
            self.modern_dini_gun_label.setText("✦ " + diniGun)

            if self.diniGunMesajiGosterildi == False:
                mesaj = hicriGunVeAy + " - " + diniGun
                self.mesajGoster("Dini Günler ve Geceler", "\n" + mesaj)
                self.diniGunMesajiGosterildi = True
        else:
            self.dini_gun_label.clear()
            self.modern_dini_gun_label.clear()

    def labelTemizlik(self):
        self.ulke_label.clear()
        self.sehir_label.clear()
        Vakitler.saatler = ""
        self.namaz_vakti_label.clear()
        self.imsak_saat_label.clear()
        self.gunes_saat_label.clear()
        self.ogle_saat_label.clear()
        self.ikindi_saat_label.clear()
        self.aksam_saat_label.clear()
        self.yatsi_saat_label.clear()
        self.sonraki_vakit_label.clear()
        self.kalan_sure_label.clear()
        if hasattr(self, "modern_ulke_label"):
            self.modern_ulke_label.clear()
            self.modern_sehir_label.clear()
            self.modern_vakit_durum_label.clear()
            self.modern_kalan_baslik_label.clear()
            self.modern_kalan_sure_label.setText("--:--:--")
            for bilesenler in self.modern_vakit_satirlari.values():
                bilesenler["saat"].setText("--:--")
                bilesenler["badge"].hide()
                bilesenler["frame"].setProperty("aktif", "false")
                bilesenler["ad"].setProperty("aktif", "false")
                bilesenler["saat"].setProperty("aktif", "false")
            self._sonAktifModernVakit = None

    def bolgeselAyarlar(self):
        self.labelTemizlik()
       
        self.ulkeler_comboBox.blockSignals(True)
        self.ulkeler_comboBox.clear()
        self.sehirler_comboBox.clear()
       
        for ulke in sorted(self.diyanetUlkeler.ulkeler):
            self.ulkeler_comboBox.addItem(ulke)
        self.ulkeler_comboBox.blockSignals(False)
           
        self.ulkeler_comboBox.currentIndexChanged.connect(self.sehirKontrolu)
        self.sehirler_comboBox.currentIndexChanged.connect(self.ilceKontrolu)

    @staticmethod
    def _ayarInt(val, default=0):
        if val is None:
            return default
        if isinstance(val, bool):
            return 2 if val else 0
        if isinstance(val, str):
            v = val.strip().lower()
            if v in ("true", "1"):
                return 2
            elif v in ("false", "0"):
                return 0
        try:
            return int(float(val))
        except (ValueError, TypeError):
            return default

    def ayarlar(self):
        try:
            self.gorunum = self.settings.value("Ayarlar/gorunum")
           
            if self.gorunum != None:
                if len(self.gorunum) > 0:
                    self.gorunumuDegistir(self.gorunum)
                else:
                    self.fusionGorunumu()
            else:
                self.fusionGorunumu()

            kayitliTema = self.settings.value("Ayarlar/tema")
            if not kayitliTema or kayitliTema not in TEMALAR:
                kayitliTema = "varsayilan"
            self.temayiUygula(kayitliTema, kaydet=False)

            kayitliArayuzModu = self.settings.value("Ayarlar/arayuz_modu")
            if kayitliArayuzModu not in ("saat", "gokyuzu", "kartlar", "modern", "klasik"):
                kayitliArayuzModu = "saat"
            self.arayuzModunuUygula(kayitliArayuzModu, kaydet=False)
   
            ulkeIndeks = self._ayarInt(self.settings.value("Ayarlar/ulke"), 190)
            sehirIndeks = self._ayarInt(self.settings.value("Ayarlar/sehir"), 77)
            ilceIndeks = self._ayarInt(self.settings.value("Ayarlar/ilce"), 17)

            # Eski varsayılan ayarlardan (ulke=186, sehir=-1) geçiş uyumluluğu
            if ulkeIndeks == 186 and sehirIndeks < 0:
                trIndeks = self.ulkeler_comboBox.findText("TÜRKİYE")
                if trIndeks >= 0:
                    ulkeIndeks = trIndeks

            if self.ulkeler_comboBox.currentIndex() == ulkeIndeks:
                self.sehirKontrolu()
            else:
                self.ulkeler_comboBox.setCurrentIndex(ulkeIndeks)

            if self.sehirler_comboBox.count() > 0:
                if sehirIndeks < 0 or sehirIndeks >= self.sehirler_comboBox.count():
                    istIndeks = self.sehirler_comboBox.findText("İSTANBUL")
                    sehirIndeks = istIndeks if istIndeks >= 0 else 0

                if self.sehirler_comboBox.currentIndex() == sehirIndeks:
                    if self.ilceler_comboBox.count() == 0:
                        self.ilceKontrolu()
                else:
                    self.sehirler_comboBox.setCurrentIndex(sehirIndeks)

            if self.ilceler_comboBox.count() > 0:
                if ilceIndeks < 0 or ilceIndeks >= self.ilceler_comboBox.count():
                    istIlceIndeks = self.ilceler_comboBox.findText("İSTANBUL")
                    ilceIndeks = istIlceIndeks if istIlceIndeks >= 0 else 0
                self.ilceler_comboBox.setCurrentIndex(ilceIndeks)
   
            otomatikCalistir = self._ayarInt(self.settings.value("Ayarlar/otomatik_calistir"), 2)
            if otomatikCalistir == 0:
                self.otomatik_calistir_checkBox.setChecked(False)
            if otomatikCalistir == 2:
                self.otomatik_calistir_checkBox.setChecked(True)
               
            self.otomatikCalistir()
           
            ikonGoster = self._ayarInt(self.settings.value("Ayarlar/ikon_goster"), 2)
            if ikonGoster == 0:
                self.ikon_goster_checkBox.setChecked(False)
                self.sistem_tepsisinde_baslat_checkBox.setEnabled(False)
                self.sistem_tepsisinde_baslat_checkBox.setChecked(False)
                self.trayIcon.hide()
            if ikonGoster == 2:
                self.ikon_goster_checkBox.setChecked(True)
                self.sistem_tepsisinde_baslat_checkBox.setEnabled(True)
                self.trayIcon.show()
   
            self.sistemTepsisindeBaslat = self._ayarInt(self.settings.value("Ayarlar/sistem_tepsisinde_baslat"), 0)
            if self.sistemTepsisindeBaslat == 0:
                self.sistem_tepsisinde_baslat_checkBox.setChecked(False)
   
            if self.sistemTepsisindeBaslat == 2:
                self.sistem_tepsisinde_baslat_checkBox.setChecked(True)
   
            otomatikGuncelle = self._ayarInt(self.settings.value("Ayarlar/otomatik_guncelle"), 2)
            if otomatikGuncelle == 0:
                self.otomatik_guncellestir_checkBox.setChecked(False)
            if otomatikGuncelle == 2:
                self.otomatik_guncellestir_checkBox.setChecked(True)
   
            herZamanUstte = self._ayarInt(self.settings.value("Ayarlar/her_zaman_ustte"), 0)
            if herZamanUstte == 0:
                self.her_zaman_ustte_checkBox.setChecked(False)
            if herZamanUstte == 2:
                self.her_zaman_ustte_checkBox.setChecked(True)
   
            self.besmeleIleBasla = self._ayarInt(self.settings.value("Ayarlar/besmele_ile_basla"), 2)
            if self.besmeleIleBasla == 0:
                self.besmele_ile_basla_checkBox.setChecked(False)
            if self.besmeleIleBasla == 2:
                self.besmele_ile_basla_checkBox.setChecked(True)
               
            winampDuraklat = self._ayarInt(self.settings.value("Ayarlar/winamp_duraklat"), 0)
            if winampDuraklat == 0:
                self.winamp_duraklat_checkBox.setChecked(False)
            if winampDuraklat == 2:
                self.winamp_duraklat_checkBox.setChecked(True)
                
            stopifyDuraklat = self._ayarInt(self.settings.value("Ayarlar/stopify_duraklat"), 0)
            if stopifyDuraklat == 0:
                self.stopify_duraklat_checkBox.setChecked(False)
            if stopifyDuraklat == 2:
                self.stopify_duraklat_checkBox.setChecked(True)
   
            sabahEzaniOku = self._ayarInt(self.settings.value("Ayarlar/sabah_ezani_oku"), 2)
            if sabahEzaniOku == 0:
                self.sabah_ezani_oku_checkBox.setChecked(False)
            if sabahEzaniOku == 2:
                self.sabah_ezani_oku_checkBox.setChecked(True)
               
            ogleEzaniOku = self._ayarInt(self.settings.value("Ayarlar/ogle_ezani_oku"), 2)
            if ogleEzaniOku == 0:
                self.ogle_ezani_oku_checkBox.setChecked(False)
            if ogleEzaniOku == 2:
                self.ogle_ezani_oku_checkBox.setChecked(True)
               
            ikindiEzaniOku = self._ayarInt(self.settings.value("Ayarlar/ikindi_ezani_oku"), 2)
            if ikindiEzaniOku == 0:
                self.ikindi_ezani_oku_checkBox.setChecked(False)
            if ikindiEzaniOku == 2:
                self.ikindi_ezani_oku_checkBox.setChecked(True)
               
            aksamEzaniOku = self._ayarInt(self.settings.value("Ayarlar/aksam_ezani_oku"), 2)
            if aksamEzaniOku == 0:
                self.aksam_ezani_oku_checkBox.setChecked(False)
            if aksamEzaniOku == 2:
                self.aksam_ezani_oku_checkBox.setChecked(True)
               
            yatsiEzaniOku = self._ayarInt(self.settings.value("Ayarlar/yatsi_ezani_oku"), 2)
            if yatsiEzaniOku == 0:
                self.yatsi_ezani_oku_checkBox.setChecked(False)
            if yatsiEzaniOku == 2:
                self.yatsi_ezani_oku_checkBox.setChecked(True)
               
            selaOku = self._ayarInt(self.settings.value("Ayarlar/sela_oku"), 2)
            if selaOku == 0:
                self.sela_oku_checkBox.setChecked(False)
            if selaOku == 2:
                self.sela_oku_checkBox.setChecked(True)
   
            duaOku = self._ayarInt(self.settings.value("Ayarlar/dua_oku"), 2)
            if duaOku == 0:
                self.dua_oku_checkBox.setChecked(False)
            if duaOku == 2:
                self.dua_oku_checkBox.setChecked(True)
   
            erkenUyari = self._ayarInt(self.settings.value("Ayarlar/erken_uyari"), 2)
            if erkenUyari == 0:
                self.erken_uyari_checkBox.setChecked(False)
            if erkenUyari == 2:
                self.erken_uyari_checkBox.setChecked(True)
   
            erkenUyariSuresi = self.settings.value("Ayarlar/erken_uyari_suresi")
            self.erken_uyari_spinBox.setValue(self._ayarInt(erkenUyariSuresi, 45))
           
            erkenUyariSela = self._ayarInt(self.settings.value("Ayarlar/erken_uyari_sela"), 2)
            if erkenUyariSela == 0:
                self.erken_uyari_sela_checkBox.setChecked(False)
            if erkenUyariSela == 2:
                self.erken_uyari_sela_checkBox.setChecked(True)
           
            erkenUyariSuresiSela = self.settings.value("Ayarlar/erken_uyari_suresi_sela")
            self.erken_uyari_sela_spinBox.setValue(self._ayarInt(erkenUyariSuresiSela, 55))
           
            uyariSesiCal = self._ayarInt(self.settings.value("Ayarlar/uyari_sesi_cal"), 2)
            if uyariSesiCal == 0:
                self.uyari_sesi_checkBox.setChecked(False)
            if (erkenUyari == 2) and (uyariSesiCal == 2):
                self.uyari_sesi_checkBox.setChecked(True)
           
            hicriGunIlave = self._ayarInt(self.settings.value("Ayarlar/hicri_gun_ilave"), 2)
           
            if hicriGunIlave == 0:
                self.hicri_gun_ekle_checkBox.setChecked(False)
            if hicriGunIlave == 2:
                self.hicri_gun_ekle_checkBox.setChecked(True)
               
            ilaveGunSayisi = self.settings.value("Ayarlar/ilave_gun_sayisi")
            self.hicri_tarih_gun_ekle_spinBox.setValue(self._ayarInt(ilaveGunSayisi, 0))
   
            sesAyari = self._ayarInt(self.settings.value("Ses_Ayarlari/ses_ayari"), 30)
            self.horizontalSlider.setValue(sesAyari)
            
            self.volume = self.horizontalSlider.value()
   
            sesDosyasiSabah = self.settings.value("Ses_Ayarlari/sabah")
            self.sesDosyasiKontrol(self.sabah_lineEdit, sesDosyasiSabah)
   
            sesDosyasiOgle = self.settings.value("Ses_Ayarlari/ogle")
            self.sesDosyasiKontrol(self.ogle_lineEdit, sesDosyasiOgle)
   
            sesDosyasiIkindi = self.settings.value("Ses_Ayarlari/ikindi")
            self.sesDosyasiKontrol(self.ikindi_lineEdit, sesDosyasiIkindi)
   
            sesDosyasiAksam = self.settings.value("Ses_Ayarlari/aksam")
            self.sesDosyasiKontrol(self.aksam_lineEdit, sesDosyasiAksam)
   
            sesDosyasiYatsi = self.settings.value("Ses_Ayarlari/yatsi")
            self.sesDosyasiKontrol(self.yatsi_lineEdit, sesDosyasiYatsi)
   
            sesDosyasiDua = self.settings.value("Ses_Ayarlari/dua")
            self.sesDosyasiKontrol(self.dua_lineEdit, sesDosyasiDua)
   
            sesDosyasiUyari = self.settings.value("Ses_Ayarlari/uyari")
            self.sesDosyasiKontrol(self.uyari_lineEdit, sesDosyasiUyari)
   
            sesDosyasiSela = self.settings.value("Ses_Ayarlari/sela")
            self.sesDosyasiKontrol(self.sela_lineEdit, sesDosyasiSela)
       
        except TypeError:
            if QtCore.QFile.exists(self.ayarDosyasi):
                QtWidgets.QMessageBox.information(self, "Bilgilendirme", "Ayar dosyasında bozukluk tespit edildiğinden silindi. \n\nAyarlarınızı yaptıktan sonra kaydetmelisiniz.", QtWidgets.QMessageBox.Ok)
               
                QtCore.QFile(self.ayarDosyasi).remove()
                self.varsayilanAyarlar()

    def sesDosyasiKontrol(self, lineEdit, sesDosyasi):
        try:
            if len(sesDosyasi) > 0:
                lineEdit.setText(sesDosyasi)
        except TypeError:
            pass

    def varsayilanAyarlar(self):
        self.labelTemizlik()
        self.statusbar.clearMessage()
       
        self.varsayilanAyarlariKopyala()

        self.settings = QtCore.QSettings(self.ontanimliAyarDosyasi, QtCore.QSettings.IniFormat)
       
        self.settings.setValue("Ayarlar/winamp_konum", None)
       
        if QtCore.QFile.exists(QtCore.QFile(self.ontanimliAyarDosyasi)):
            if QtCore.QFile.exists(QtCore.QFile(self.ayarDosyasi)):
                QtCore.QFile(self.ayarDosyasi).remove()
               
            if QtCore.QFile.exists(QtCore.QFile(PyVakitci.veritabaniDosyasi)):
                QtCore.QFile(PyVakitci.veritabaniDosyasi).remove()

            self.ayarlar()
            self.vakitleriAl()

    def sesVarsayilanAyarlar(self):
        self.sabah_lineEdit.setText(self.sesDosyalariKonum + "sabah_saba.mp3")
        self.ogle_lineEdit.setText(self.sesDosyalariKonum + "ogle_rast.mp3")
        self.ikindi_lineEdit.setText(self.sesDosyalariKonum + "ikindi_hicaz.mp3")
        self.aksam_lineEdit.setText(self.sesDosyalariKonum + "aksam_segah.mp3")
        self.yatsi_lineEdit.setText(self.sesDosyalariKonum + "yatsi_ussak.mp3")
        self.dua_lineEdit.setText(self.sesDosyalariKonum + "ezan_duasi.mp3")
        self.uyari_lineEdit.setText(self.sesDosyalariKonum + "uyari.mp3")
        self.sela_lineEdit.setText(self.sesDosyalariKonum + "sela.mp3")

    def ayarlariUygula(self):
        if QtCore.QFile.exists(QtCore.QFile(self.ayarDosyasi)):
            self.settings = QtCore.QSettings(self.ayarDosyasi, QtCore.QSettings.IniFormat)
           
            self.ayarlar()
            self.vakitleriAl()
        else:
            self.settings = QtCore.QSettings(self.ontanimliAyarDosyasi, QtCore.QSettings.IniFormat)
            self.sesVarsayilanAyarlar()
            self.varsayilanAyarlar()

    def ayarlariKaydet(self):
        QtWidgets.QApplication.setOverrideCursor(QtCore.Qt.WaitCursor)

        self.calismaDizininiHazirla()
       
        self.settings = QtCore.QSettings(self.ayarDosyasi, QtCore.QSettings.IniFormat)

        self.settings.setValue("Ayarlar/gorunum", self.gorunum)
        self.settings.setValue("Ayarlar/tema", self.aktifTema)
        self.settings.setValue("Ayarlar/arayuz_modu", self.arayuzModu)
        self.settings.setValue("Ayarlar/ulke", self.ulkeler_comboBox.currentIndex())
        self.settings.setValue("Ayarlar/sehir", self.sehirler_comboBox.currentIndex())
        self.settings.setValue("Ayarlar/ilce", self.ilceler_comboBox.currentIndex())
        self.settings.setValue("Ayarlar/otomatik_calistir", self.otomatik_calistir_checkBox.checkState())
        self.otomatikCalistir()
        self.settings.setValue("Ayarlar/ikon_goster", self.ikon_goster_checkBox.checkState())
        self.settings.setValue("Ayarlar/sistem_tepsisinde_baslat", self.sistem_tepsisinde_baslat_checkBox.checkState())
        self.settings.setValue("Ayarlar/otomatik_guncelle", self.otomatik_guncellestir_checkBox.checkState())
        self.otomatikGuncellestir()
        self.settings.setValue("Ayarlar/her_zaman_ustte", self.her_zaman_ustte_checkBox.checkState())
        self.settings.setValue("Ayarlar/besmele_ile_basla", self.besmele_ile_basla_checkBox.checkState())
        self.settings.setValue("Ayarlar/winamp_duraklat", self.winamp_duraklat_checkBox.checkState())
        self.settings.setValue("Ayarlar/stopify_duraklat", self.stopify_duraklat_checkBox.checkState())
        self.settings.setValue("Ayarlar/sabah_ezani_oku", self.sabah_ezani_oku_checkBox.checkState())
        self.settings.setValue("Ayarlar/ogle_ezani_oku", self.ogle_ezani_oku_checkBox.checkState())
        self.settings.setValue("Ayarlar/ikindi_ezani_oku", self.ikindi_ezani_oku_checkBox.checkState())
        self.settings.setValue("Ayarlar/aksam_ezani_oku", self.aksam_ezani_oku_checkBox.checkState())
        self.settings.setValue("Ayarlar/yatsi_ezani_oku", self.yatsi_ezani_oku_checkBox.checkState())
        self.settings.setValue("Ayarlar/dua_oku", self.dua_oku_checkBox.checkState())
        self.settings.setValue("Ayarlar/sela_oku", self.sela_oku_checkBox.checkState())      
        self.settings.setValue("Ayarlar/uyari_sesi_cal", self.uyari_sesi_checkBox.checkState())
        self.settings.setValue("Ayarlar/erken_uyari", self.erken_uyari_checkBox.checkState())
        self.settings.setValue("Ayarlar/erken_uyari_suresi", self.erken_uyari_spinBox.text())
        self.settings.setValue("Ayarlar/erken_uyari_sela", self.erken_uyari_sela_checkBox.checkState())
        self.settings.setValue("Ayarlar/erken_uyari_suresi_sela", self.erken_uyari_sela_spinBox.text())
        self.settings.setValue("Ayarlar/hicri_gun_ilave", self.hicri_gun_ekle_checkBox.checkState())
        self.settings.setValue("Ayarlar/ilave_gun_sayisi", self.hicri_tarih_gun_ekle_spinBox.text())
        self.settings.setValue("Ayarlar/uyari_sesi_cal", self.uyari_sesi_checkBox.checkState())
        self.settings.setValue("Ses_Ayarlari/sabah", self.sabah_lineEdit.text())
        self.settings.setValue("Ses_Ayarlari/ogle", self.ogle_lineEdit.text())
        self.settings.setValue("Ses_Ayarlari/ikindi", self.ikindi_lineEdit.text())
        self.settings.setValue("Ses_Ayarlari/aksam", self.aksam_lineEdit.text())
        self.settings.setValue("Ses_Ayarlari/yatsi", self.yatsi_lineEdit.text())
        self.settings.setValue("Ses_Ayarlari/dua", self.dua_lineEdit.text())
        self.settings.setValue("Ses_Ayarlari/uyari", self.uyari_lineEdit.text())
        self.settings.setValue("Ses_Ayarlari/sela", self.sela_lineEdit.text())
        self.settings.setValue("Ses_Ayarlari/ses_ayari", self.horizontalSlider.value())
        self.volume = self.horizontalSlider.value()
       
        self.hicriTarihiGuncelle()
       
        QtWidgets.QApplication.setOverrideCursor(QtCore.Qt.ArrowCursor)

        self.vakitleriAl()
   
    def veritabaniniGuncelle(self):
        Vakitler.veritabanindakiYerBilgisi = True
        try:
            if self.timerLabel.isActive():
                self.timerLabel.stop()
        except AttributeError:
            pass
       
        self.diyanetAylikVakitleriKaydet()
       
    def yerKontrol(self, veri):
        if len(veri) > 0:
            return veri
        else:
            return None
       
    def ulkeSehirBilgisiniAl(self):
        self.ulke = self.ulkeler_comboBox.currentText()
        self.sehir = self.sehirler_comboBox.currentText()
        self.ilce = self.ilceler_comboBox.currentText()
       
        self.ulke_label.setText(self.ulke)
        if self.yerKontrol(self.ilce) is not None:
            self.seciliYer = self.ilce
            if self.sehir and self.sehir != self.ulke and self.sehir != self.ilce:
                self.sehir_label.setText(self.ilce)
                self.sehir_label.setToolTip(self.sehir + " / " + self.ilce)
                self.modern_ulke_label.setText(self.ulke + " • " + self.sehir)
                self.modern_sehir_label.setText(self.ilce)
                self.modern_sehir_label.setToolTip(self.sehir + " / " + self.ilce)
            else:
                self.sehir_label.setText(self.ilce)
                self.sehir_label.setToolTip(self.ilce)
                self.modern_ulke_label.setText(self.ulke)
                self.modern_sehir_label.setText(self.ilce)
                self.modern_sehir_label.setToolTip(self.ilce)
        else:
            self.seciliYer = self.sehir
            self.sehir_label.setText(self.sehir)
            self.sehir_label.setToolTip(self.sehir)
            self.modern_ulke_label.setText(self.ulke)
            self.modern_sehir_label.setText(self.sehir)
            self.modern_sehir_label.setToolTip(self.sehir)
       
        Vakitler.ulke = self.diyanetUlkeler.ulkeler.get(self.ulke)
        if self.yerKontrol(self.sehir) is not None and self.sehirler:
            Vakitler.sehir = self.sehirler.get(self.sehir)
        else:
            Vakitler.sehir = None
       
        if self.yerKontrol(self.ilce) is not None and self.ilceler and self.linkler:
            Vakitler.ilce = self.ilceler.get(self.ilce)
            Vakitler.link = self.linkler.get(self.ilce)
        else:
            Vakitler.ilce = None
            Vakitler.link = None
           
        Vakitler.seciliSehir = self.seciliYer
        if hasattr(self, "havaDurumuWidget"):
            self.havaDurumuWidget.konumAyarla(
                self.ulke, self.sehir, self.seciliYer, getattr(self, "aktifTema", "varsayilan")
            )
       
    def vakitleriAl(self):
        self.labelTemizlik()
        self.statusbar.clearMessage()
       
        self.ulkeSehirBilgisiniAl()
       
        if len(self.ulke) > 0 and len(self.sehir) > 0:

            if not QtCore.QFile.exists(self.veritabaniDosyasi) or QtCore.QFile(self.veritabaniDosyasi).size() == 0:
                self.diyanetAylikVakitleriKaydet()
           
            if self.ulkeler_comboBox.currentIndex() >= 0:
                if self.sehirler_comboBox.currentIndex() >= 0:
                    if self.rlock._is_owned() == False:
                        vakitler = Vakitler(self, self.seciliYer)
                        vakitler.start()
           
            if hasattr(self, "timerLabel") and self.timerLabel.isActive():
                self.timerLabel.stop()
            self.timerLabel = QtCore.QTimer()
            self.timerLabel.timeout.connect(self.labelGuncelle)
            self.timerLabel.start(500)
       
    def gunlukVakitleriKaydet(self):
        if len(Vakitler.saatler) != 0:
            self.settings.setValue("Vakitler/imsak", Vakitler.imsak)
            self.settings.setValue("Vakitler/gunes", Vakitler.gunes)
            self.settings.setValue("Vakitler/ogle", Vakitler.ogle)
            self.settings.setValue("Vakitler/ikindi", Vakitler.ikindi)
            self.settings.setValue("Vakitler/aksam", Vakitler.aksam)
            self.settings.setValue("Vakitler/yatsi", Vakitler.yatsi)
            self.settings.setValue("Vakitler/sela", self.erken_uyari_sela_spinBox.text())

    def labelGuncelle(self):
        saatDakika = QtCore.QTime().currentTime().toString("hh:mm")
        self.saat = QtCore.QTime().currentTime().toString("hh:mm:ss")
        self.saatDakika = saatDakika
       
        if len(Vakitler.saatler) != 0:
            self.statusbar.showMessage(tr("kaynak"))
           
            self.gunlukVakitleriKaydet()
           
            try:
                if "00:00" <= saatDakika < Vakitler.imsak:
                    self.labelVeriler(Vakitler.imsak, Vakitler.gunes, Vakitler.ogle, Vakitler.ikindi,
                                      Vakitler.aksam, "<font color='#FF0000'>" + Vakitler.yatsi + "</font>",
                                      format_current_prayer("yatsi"), format_remaining_time("imsak"), "yatsı")

                if Vakitler.imsak <= saatDakika < Vakitler.gunes:
                    self.labelVeriler("<font color='#FF0000'>" + Vakitler.imsak + "</font>", Vakitler.gunes,
                                      Vakitler.ogle, Vakitler.ikindi, Vakitler.aksam, Vakitler.yatsi,
                                      format_current_prayer("imsak"), format_remaining_time("gunes"), "sabah")

                    self.diniGunleriHatirlat()

                    self.vakitKontrol(tr("namaz_vakitleri_tray"), self.sabah_lineEdit.text(), self.imsak_saat_label.text(), "sabah")

                if Vakitler.gunes <= saatDakika < Vakitler.ogle:
                    self.labelVeriler(Vakitler.imsak, "<font color='#FF0000'>" + Vakitler.gunes + "</font>",
                                      Vakitler.ogle, Vakitler.ikindi, Vakitler.aksam, Vakitler.yatsi,
                                      format_current_prayer("gunes"), format_remaining_time("ogle"), "güneş")

                if Vakitler.ogle <= saatDakika < Vakitler.ikindi:
                    self.labelVeriler(Vakitler.imsak, Vakitler.gunes, "<font color='#FF0000'>" + Vakitler.ogle + "</font>",
                                      Vakitler.ikindi, Vakitler.aksam, Vakitler.yatsi,
                                      format_current_prayer("ogle"), format_remaining_time("ikindi"), "öğle")

                    self.diniGunleriHatirlat()

                    self.vakitKontrol(tr("namaz_vakitleri_tray"), self.ogle_lineEdit.text(), self.ogle_saat_label.text(), "öğle")

                if Vakitler.ikindi <= saatDakika < Vakitler.aksam:
                    self.labelVeriler(Vakitler.imsak, Vakitler.gunes, Vakitler.ogle,
                                      "<font color='#FF0000'>" + Vakitler.ikindi + "</font>", Vakitler.aksam,
                                      Vakitler.yatsi,
                                      format_current_prayer("ikindi"), format_remaining_time("aksam"), "ikindi")

                    self.diniGunleriHatirlat()

                    self.vakitKontrol(tr("namaz_vakitleri_tray"), self.ikindi_lineEdit.text(), self.ikindi_saat_label.text(), "ikindi")

                if Vakitler.aksam <= saatDakika < Vakitler.yatsi:
                    self.labelVeriler(Vakitler.imsak, Vakitler.gunes, Vakitler.ogle, Vakitler.ikindi,
                                      "<font color='#FF0000'>" + Vakitler.aksam + "</font>", Vakitler.yatsi,
                                      format_current_prayer("aksam"), format_remaining_time("yatsi"), "akşam")

                    self.diniGunleriHatirlat()

                    self.vakitKontrol(tr("namaz_vakitleri_tray"), self.aksam_lineEdit.text(), self.aksam_saat_label.text(), "akşam")

                if Vakitler.yatsi <= saatDakika <= "23:59":
                    self.labelVeriler(Vakitler.imsak, Vakitler.gunes, Vakitler.ogle, Vakitler.ikindi,
                                      Vakitler.aksam, "<font color='#FF0000'>" + Vakitler.yatsi + "</font>",
                                      format_current_prayer("yatsi"), format_remaining_time("imsak"), "yatsı")

                    self.diniGunleriHatirlat()

                    self.vakitKontrol(tr("namaz_vakitleri_tray"), self.yatsi_lineEdit.text(), self.yatsi_saat_label.text(), "yatsı")
            except ValueError:
                pass

        else:
            if Vakitler.tarihHatasi:
                self.timerLabel.stop()
                self.statusbar.clearMessage()
                QtWidgets.QMessageBox.warning(self, "Tarih Hatası",
                                                "Sisteminizin tarihi yanlış. Tarihi düzelttikten sonra veya \"Vakitleri aylık olarak al.\" " +
                                                "seçeneğinin işaretini kaldırdıktan sonra Ayarlar sekmesindeki Kaydet butonuna tıklayın.",
                                                "Tamam")
            else:
                Vakitler.tarihHatasi = False
               
        if Vakitler.veritabanindakiYerBilgisi == False:
            self.veritabaniniGuncelle()

    def labelVeriler(self, imsak, gunes, ogle, ikindi, aksam, yatsi, namazvakti, sonrakivakit, simdikivakit):
        self.imsak_saat_label.setText(imsak)
        self.gunes_saat_label.setText(gunes)
        self.ogle_saat_label.setText(ogle)
        self.ikindi_saat_label.setText(ikindi)
        self.aksam_saat_label.setText(aksam)
        self.yatsi_saat_label.setText(yatsi)
        self.namaz_vakti_label.setText(namazvakti)
        self.namazvakti = namazvakti
        self.sonraki_vakit_label.setText(sonrakivakit)

        self.modern_vakit_durum_label.setText(namazvakti)
        self.modern_kalan_baslik_label.setText(sonrakivakit.replace(" :", ""))

        vakitEsleme = {
            "sabah": "imsak",
            "güneş": "gunes",
            "öğle": "ogle",
            "ikindi": "ikindi",
            "akşam": "aksam",
            "yatsı": "yatsi",
        }
        self._modernVakitleriGuncelle(vakitEsleme.get(simdikivakit, ""))

        self.kalanSureKontrol(simdikivakit)

    def kalanSureKontrol(self, vakit):
        self.simdikiZaman = QtCore.QTime().currentTime()
        self.simdikiSaat = int(self.simdikiZaman.toString("hh"))
        self.simdikiDakika = int(self.simdikiZaman.toString("mm"))
        self.simdikiSaniye = int(self.simdikiZaman.toString("ss"))

        if vakit == "sabah":
            gunes = str(self.gunes_saat_label.text())
            saat = int(gunes[:gunes.find(":")].replace("<font color='#FF0000'>", ""))
            dakika = int(gunes[gunes.find(":") + 1:].replace("</font>", ""))

            self.toplamDakika = (saat * 60 + dakika) - (self.simdikiSaat * 60 + self.simdikiDakika)
            self.erkenUyariKontrol(self.toplamDakika)

        if vakit == "güneş":
            ogle = str(self.ogle_saat_label.text())
            saat = int(ogle[:ogle.find(":")].replace("<font color='#FF0000'>", ""))
            dakika = int(ogle[ogle.find(":") + 1:].replace("</font>", ""))

            self.toplamDakika = (saat * 60 + dakika) - (self.simdikiSaat * 60 + self.simdikiDakika)
            self.erkenUyariKontrol(self.toplamDakika)
            if self.sela_oku_checkBox.isChecked() and self.erken_uyari_sela_checkBox.isChecked():
                self.selaOkuKontrol(str(self.toplamDakika))

        if vakit == "öğle":
            ikindi = str(self.ikindi_saat_label.text())
            saat = int(ikindi[:ikindi.find(":")].replace("<font color='#FF0000'>", ""))
            dakika = int(ikindi[ikindi.find(":") + 1:].replace("</font>", ""))

            self.toplamDakika = (saat * 60 + dakika) - (self.simdikiSaat * 60 + self.simdikiDakika)
            self.erkenUyariKontrol(self.toplamDakika)

        if vakit == "ikindi":
            aksam = str(self.aksam_saat_label.text())
            saat = int(aksam[:aksam.find(":")].replace("<font color='#FF0000'>", ""))
            dakika = int(aksam[aksam.find(":") + 1:].replace("</font>", ""))

            self.toplamDakika = (saat * 60 + dakika) - (self.simdikiSaat * 60 + self.simdikiDakika)
            self.erkenUyariKontrol(self.toplamDakika)

        if vakit == "akşam":
            yatsi = str(self.yatsi_saat_label.text())
            saat = int(yatsi[:yatsi.find(":")].replace("<font color='#FF0000'>", ""))
            dakika = int(yatsi[yatsi.find(":") + 1:].replace("</font>", ""))

            self.toplamDakika = (saat * 60 + dakika) - (self.simdikiSaat * 60 + self.simdikiDakika)
            self.erkenUyariKontrol(self.toplamDakika)

        if vakit == "yatsı":
            imsak = str(self.imsak_saat_label.text())
            saat = int(imsak[:imsak.find(":")].replace("<font color='#FF0000'>", ""))
            dakika = int(imsak[imsak.find(":") + 1:].replace("</font>", ""))

            if saat >= self.simdikiSaat >= 0:
                self.toplamDakika = (saat * 60 + dakika) - (self.simdikiSaat * 60 + self.simdikiDakika)
                self.erkenUyariKontrol(self.toplamDakika)
            else:
                self.toplamDakika = (24 * 60) - (self.simdikiSaat * 60 + self.simdikiDakika) + (saat * 60 + dakika)
                self.erkenUyariKontrol(self.toplamDakika)

        self.kalanSureyiHesapla(saat, dakika)

    def kalanSureyiHesapla(self, saat, dakika):
        belirlenenSaat = int(saat)
        belirlenenDakika = int(dakika)

        belirlenenZaman = datetime.timedelta(0, 0, 0, 0, belirlenenDakika, belirlenenSaat, 0)
        simdi = datetime.timedelta(0, self.simdikiSaniye, 0, 0, self.simdikiDakika, self.simdikiSaat, 0)

        kalanSure = str(belirlenenZaman - simdi)

        # "-1 gün..." tarzında bir çıktı alma ihtimaline karşı önlem.
        kalanSure = kalanSure[kalanSure.rfind(" ") + 1:]

        self.kalan_sure_label.setText(kalanSure)
        parcalar = kalanSure.split(":")
        if len(parcalar) == 3 and len(parcalar[0]) == 1:
            modernSure = "0" + kalanSure
        else:
            modernSure = kalanSure
        self.modern_kalan_sure_label.setText(modernSure)


    def erkenUyariKontrol(self, toplamDakika):
        try:
            if toplamDakika == int(self.erken_uyari_spinBox.text()):
                if self.erken_uyari_checkBox.isChecked():
                    if self.mesajGosterildi == False:
                        self.trayIcon.showMessage(tr("erken_uyari"),
                                                  self.sonraki_vakit_label.text() + " " + str(toplamDakika) + " " + tr("dakika"),
                                                  self.trayIcon.Information, 20000)
                        self.mesajGosterildi = True
                        
                        if self.uyari_sesi_checkBox.isChecked():
                            self.sesDosyasiniAc(self.uyari_lineEdit.text())
   
                        self.timerMesaj()
        except Exception:
            pass

    def mesajGoster(self, baslik, mesaj):
        self.trayIcon.showMessage(baslik, mesaj, self.trayIcon.Information, 20000)

    def vakitleriGoster(self):
        self.mesajGoster(tr("namaz_vakitleri_tray"), self.vakitlerMesaj())

    def vakitlerMesaj(self):
        try:
            yer = self.ilceler_comboBox.currentText() or self.sehirler_comboBox.currentText()
            header = tr("vakitler_icin_fmt").replace("%1", yer).replace("%2", self.namazvakti)
            mesaj = f"{header}\n{tr('imsak')}: {Vakitler.imsak}\t{tr('gunes')}: {Vakitler.gunes}" \
                    f"\n{tr('ogle')}: {Vakitler.ogle}\t{tr('ikindi')}: {Vakitler.ikindi}" \
                    f"\n{tr('aksam')}: {Vakitler.aksam}\t{tr('yatsi')}: {Vakitler.yatsi}"
        except AttributeError:
            mesaj = tr("hava_hata")

        return mesaj

    def _vakitEzaniSeciliMi(self, simdikivakit):
        eslesme = {
            "sabah": self.sabah_ezani_oku_checkBox,
            "öğle": self.ogle_ezani_oku_checkBox,
            "ikindi": self.ikindi_ezani_oku_checkBox,
            "akşam": self.aksam_ezani_oku_checkBox,
            "yatsı": self.yatsi_ezani_oku_checkBox,
        }
        checkBox = eslesme.get(simdikivakit)
        return checkBox.isChecked() if checkBox is not None else False

    def vakitKontrol(self, baslik, dosya, labelsaat, simdikivakit):
        try:
            ara = re.search(">(.*)<", labelsaat)
            saat = ara.group(1)

            if saat == self.saat[:5]:
                if self.mesajGosterildi == False:
                    mesaj = self.vakitlerMesaj()
                    self.mesajGoster(baslik, mesaj)
                    self.mesajGosterildi = True
                   
                    if self._vakitEzaniSeciliMi(simdikivakit):
                        self.ezanOku(dosya)

                    self.timerMesaj()
        except Exception:
            pass

    def timerMesaj(self):
        if self.mesajTimer is not None and self.mesajTimer.isActive():
            self.mesajTimer.stop()
        self.mesajTimer = QtCore.QTimer()
        self.mesajTimer.timeout.connect(self.mesajGosterimKontrol)
        self.mesajTimer.start(120000)

    def mesajGosterimKontrol(self):
        self.mesajGosterildi = False
        if self.mesajTimer is not None:
            self.mesajTimer.stop()

    def sesDosyasiniAc(self, dosya):
        self.initMultimedia(dosya)

        try:
            self.player.stop()
        except Exception:
            pass

        self.player.play()
        
    def stopifyOynatDuraklat(self):
        win32api.keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_EXTENDEDKEY, 0)
        
    def mediaPlayersPlayOrPause(self):
        if self.stopify_duraklat_checkBox.isChecked():
            self.stopifyOynatDuraklat()
        
        sleep(1)
        
        if self.winamp_duraklat_checkBox.isChecked():
            self.winampKontrol.duraklat()

    def _medyaSinyalleriniTemizle(self):
        for slot in (self.ezanDuasiniOku, self.sesDosyasiOynatmaBitti):
            try:
                self.player.mediaStatusChanged.disconnect(slot)
            except TypeError:
                pass

    def ezanOku(self, dosya):
        self.mediaPlayersPlayOrPause()
        self._medyaSinyalleriniTemizle()
        
        self.sesDosyasiniAc(dosya)
        self.player.mediaStatusChanged.connect(self.ezanDuasiniOku)
            
    def ezanDuasiniOku(self, status):
        if status == self.player.EndOfMedia:
            self._medyaSinyalleriniTemizle()
            if self.dua_oku_checkBox.isChecked():
                self.initMultimedia(self.dua_lineEdit.text())    
                self.player.play()
                
                self.player.mediaStatusChanged.connect(self.sesDosyasiOynatmaBitti)
            else:
                self.sesDosyasiOynatmaBitti(self.player.EndOfMedia)
   
    def sesDosyasiOynatmaBitti(self, status=None):
        if status is None or status == self.player.EndOfMedia:
            self._medyaSinyalleriniTemizle()
            self.player.stop()
            self.initMultimedia("")
            
            self.mediaPlayersPlayOrPause()

    def selaOkuKontrol(self, dakika):
        bugun = QtCore.QDate().currentDate()
        gun = bugun.toString("dddd")
        
        try:
            if bugun.dayOfWeek() == 5 or gun == "Cuma":
                if self.namaz_vakti_label.text() == "Şimdi Güneş Vakti":
                    if str(dakika) == self.erken_uyari_sela_spinBox.text():
                        if self.mesajGosterildi == False:
                            self.mesajGoster("Hâyırlı Cumalar", "\nSela okunuyor.")
                            self.mesajGosterildi = True
    
                            self.selaOku()
                            self.timerMesaj()
        except Exception:
            pass

    def selaOku(self):
        if self.winamp_duraklat_checkBox.isChecked():
            self.winampKontrol.duraklat()
        self._medyaSinyalleriniTemizle()
           
        self.selaDosyasi = self.sela_lineEdit.text()
        self.sesDosyasiniAc(self.selaDosyasi)
        self.player.mediaStatusChanged.connect(self.sesDosyasiOynatmaBitti)

    def ulkeKontrolu(self):
        return True

    def sehirKontrolu(self):
        self.labelAdiniDegistir(self.ulkeler_comboBox.currentText())
        self.sehirler_comboBox.blockSignals(True)
        self.sehirler_comboBox.clear()
        self.sehirler_comboBox.blockSignals(False)
        self.sehirleriAl()
   
    def sehirleriAl(self):
        self.sehirler = None
       
        if self.ulkeler_comboBox.currentIndex() > -1:
            if self.siteyeBaglantiVarMi(self.diyanetURL):
                sehirleriAl = SehirleriAl(self.ulkeler_comboBox.currentText())
                self.sehirler = sehirleriAl.basla()
            else:
                self.sehirler = self.settings.value("Diyanet/sehirler")
               
            if self.sehirler and len(self.sehirler.keys()) > 0:
                self.sehirleriEkle()
       
    def sehirleriEkle(self):
        self.sehirler_comboBox.blockSignals(True)
        for sehir in sorted(self.sehirler):
            self.sehirler_comboBox.addItem(sehir)
        self.sehirler_comboBox.blockSignals(False)
        self.ilceKontrolu()
       
    def ilceKontrolu(self):
        self.ilceler_comboBox.clear()

        if self.ulkeKontrolu():
            self.ilceler_comboBox.setEnabled(True)
            self.ilceleriAl()
        else:
            self.ilceler_comboBox.setEnabled(False)
   
    def ilceleriAl(self):
        self.ilceler = None
       
        if self.sehirler_comboBox.currentIndex() > -1 and self.sehirler:
            if self.siteyeBaglantiVarMi(self.diyanetURL):
                ilceleriAl = IlceleriAl(self.sehirler, self.ulkeler_comboBox.currentText(), self.sehirler_comboBox.currentText())
                data = ilceleriAl.basla()
                
                self.ilceler = data[0]
                self.linkler = data[1]
            else:
                self.ilceler = self.settings.value("Diyanet/ilceler")
                self.linkler = self.settings.value("Diyanet/linkler")
               
            if self.ilceler and len(self.ilceler.keys()) > 0:
                self.ilceleriEkle()
           
    def ilceleriEkle(self):
        for ilce in sorted(self.ilceler):
            self.ilceler_comboBox.addItem(ilce)

    def labelAdiniDegistir(self, secilenUlke):
        if secilenUlke in ("ABD", "KANADA", "ALMANYA", "AVUSTRALYA"):
            self.bolgesel_ayarlar_sehir_label.setText("Eyalet :")
            self.bolgesel_ayarlar_ilce_label.setText("Şehir :")
        elif secilenUlke == "TÜRKİYE":
            self.bolgesel_ayarlar_sehir_label.setText("Şehir :")
            self.bolgesel_ayarlar_ilce_label.setText("İlçe :")
        else:
            self.bolgesel_ayarlar_sehir_label.setText("Bölge :")
            self.bolgesel_ayarlar_ilce_label.setText("Şehir :")

    def vakitleriTopluGoster(self):
        if QtCore.QFile.exists(self.veritabaniDosyasi) and QtCore.QFile(self.veritabaniDosyasi).size() > 0:
            AylikVakitler(self.veritabaniDosyasi)

            if Vakitler.tarihHatasi == True:
                self.timerLabel.stop()
                self.statusbar.clearMessage()
                QtWidgets.QMessageBox.warning(self, "Tarih Hatası",
                                          'Sisteminizin tarihi yanlış. Tarihi düzelttikten sonra veya "Vakitleri aylık olarak al." ' +
                                           "seçeneğinin işaretini kaldırdıktan sonra Ayarlar sekmesindeki Kaydet butonuna tıklayın " +
                                           "veya programı tekrar çalıştırın.", "Tamam")

            if AylikVakitler.veritabaniVarmi == False:
                self.diyanetAylikVakitleriKaydet()
                AylikVakitler(self.veritabaniDosyasi)

        else:
            self.diyanetAylikVakitleriKaydet()
            AylikVakitler(self.veritabaniDosyasi)

    def siteyeBaglantiVarMi(self, url):
        siteyeBaglan = SiteyeBaglan(url)
        siteyeBaglan.start()
        siteyeBaglan.join(3)
       
        if siteyeBaglan.baglantiVarmi:
            return True
        else:
            return False

    def diyanetAylikVakitleriKaydet(self):
        if self.siteyeBaglantiVarMi(self.diyanetURL):
            QtWidgets.QApplication.setOverrideCursor(QtCore.Qt.BusyCursor)

            self.statusbar.showMessage("Vakitler güncelleniyor...")
            self.ulkeSehirBilgisiniAl()
           
            vakitler = Vakitler(self, self.seciliYer)

            ulke = self.ulkeler_comboBox.currentText()

            if ulke == "TÜRKİYE":
                vakitler.diyanetAylikVakitleriKaydet("İlçe")
            else:
                vakitler.diyanetAylikVakitleriKaydet("Şehir")

            self.statusbar.clearMessage()

            QtWidgets.QApplication.setOverrideCursor(QtCore.Qt.ArrowCursor)

            self.vakitleriAl()

        else:
            self.statusbar.clearMessage()
            QtWidgets.QMessageBox.warning(self, "Bağlantı Hatası",
                                            "Diyanetin sitesine bağlantı kurulamadığından dolayı \n" +
                                            "namaz vakitleri veritabanı güncellenemedi.",
                                            "Tamam")
            
    def siteAc(self, url):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl(url))

    def lisans(self):
        self.lisansDosyasi = get_license_file(self.programinKonumu)
        if os.path.exists(self.lisansDosyasi):
            QtGui.QDesktopServices.openUrl(QtCore.QUrl.fromLocalFile(self.lisansDosyasi))
        else:
            self.siteAc(os.path.basename(self.lisansDosyasi))

    def dilDegisti(self, index):
        secili_kod = self.dil_comboBox.itemData(index)
        if secili_kod and secili_kod != self.dil:
            self.dil = secili_kod
            set_language(self.dil)
            self.settings.setValue("Ayarlar/dil", self.dil)
            self.lisansDosyasi = get_license_file(self.programinKonumu)
            self.retranslateUiDynamic()

    def retranslateUiDynamic(self):
        self.setWindowTitle(f"PyVakitci 1.8 - {tr('tab_saat')}")
        self.tabWidget.setTabText(0, "")
        self.tabWidget.setTabText(1, "")
        self.tabWidget.setTabText(2, "")
        self.tabWidget.setTabText(3, "")
        self.tabWidget.setTabText(4, "")
        self.tabWidget.setTabToolTip(0, tr("tab_saat"))
        self.tabWidget.setTabToolTip(1, tr("tab_hava"))
        self.tabWidget.setTabToolTip(2, tr("tab_ayarlar"))
        self.tabWidget.setTabToolTip(3, tr("tab_ses"))
        self.tabWidget.setTabToolTip(4, tr("tab_linkler"))

        if hasattr(self, "btnGenelAyarlar") and self.btnGenelAyarlar:
            self.btnGenelAyarlar.setText("⚙ " + tr("genel_ayarlar"))
        if hasattr(self, "btnBolgeselAyarlar") and self.btnBolgeselAyarlar:
            self.btnBolgeselAyarlar.setText("📍 " + tr("bolgesel_ayarlar"))

        self.otomatik_calistir_checkBox.setText(tr("oto_calistir"))
        if hasattr(self, "ikon_goster_checkBox"):
            self.ikon_goster_checkBox.setText(tr("sistem_tepsisi_ikon"))
        self.sistem_tepsisinde_baslat_checkBox.setText(tr("sistem_tepsisi"))
        self.otomatik_guncellestir_checkBox.setText(tr("oto_guncelle"))
        self.her_zaman_ustte_checkBox.setText(tr("her_zaman_ustte"))
        self.besmele_ile_basla_checkBox.setText(tr("besmele_ile_basla"))
        if hasattr(self, "stopify_duraklat_checkBox"):
            self.stopify_duraklat_checkBox.setText(tr("spotify_duraklat"))
        self.winamp_duraklat_checkBox.setText(tr("winamp_duraklat"))

        if hasattr(self, "menuDosya"):
            self.menuDosya.setTitle(tr("menu_dosya"))
        if hasattr(self, "menuGorunum"):
            self.menuGorunum.setTitle(tr("menu_gorunum"))
        if hasattr(self, "menuTema"):
            self.menuTema.setTitle(tr("menu_renk_temasi"))
        if hasattr(self, "menuPencereStili"):
            self.menuPencereStili.setTitle(tr("menu_pencere_stili"))
        if hasattr(self, "menuYardim"):
            self.menuYardim.setTitle(tr("menu_yardim"))

        # Arayüz Modları
        if hasattr(self, "actionVakitSaati"):
            self.actionVakitSaati.setText(tr("mod_saat"))
        if hasattr(self, "actionGokyuzu"):
            self.actionGokyuzu.setText(tr("mod_gokyuzu"))
        if hasattr(self, "actionModernKartlar"):
            self.actionModernKartlar.setText(tr("mod_kartlar"))
        if hasattr(self, "actionKlasikGorunum"):
            self.actionKlasikGorunum.setText(tr("mod_klasik"))

        # Tema Seçenekleri
        for t_key, act in getattr(self, "temaActions", {}).items():
            act.setText(tr(f"tema_{t_key}"))

        # Pencere Stilleri
        if hasattr(self, "actionFusion"):
            self.actionFusion.setText(tr("stil_fusion"))
        if hasattr(self, "actionWindowsVista"):
            self.actionWindowsVista.setText(tr("stil_vista"))
        if hasattr(self, "actionWindowsXP"):
            self.actionWindowsXP.setText(tr("stil_windows"))

        if hasattr(self, "actionCikis"):
            self.actionCikis.setText(tr("cikis"))
        if hasattr(self, "actionLisans"):
            self.actionLisans.setText(tr("lisans"))
        if hasattr(self, "actionHakkinda"):
            self.actionHakkinda.setText(tr("hakkinda"))
        if hasattr(self, "actionQtHakkinda"):
            self.actionQtHakkinda.setText(tr("menu_qt_hakkinda"))
        if hasattr(self, "actionVakitleriGoster"):
            self.actionVakitleriGoster.setText(tr("vakitleri_goster"))
        if hasattr(self, "linkler_groupBox"):
            self.linkler_groupBox.setTitle(tr("yararli_siteler"))

        self.bolgesel_ayarlar_ulke_label.setText(tr("ulke"))
        self.bolgesel_ayarlar_sehir_label.setText(tr("sehir"))
        self.bolgesel_ayarlar_ilce_label.setText(tr("ilce"))
        if hasattr(self, "bolgesel_ayarlar_dil_label"):
            self.bolgesel_ayarlar_dil_label.setText(tr("dil"))

        if hasattr(self, "erken_uyari_checkBox"):
            self.erken_uyari_checkBox.setText(tr("erken_uyari_metin"))
        if hasattr(self, "erken_uyari_sela_checkBox"):
            self.erken_uyari_sela_checkBox.setText(tr("erken_sela_metin"))
        if hasattr(self, "hicri_gun_ekle_checkBox"):
            self.hicri_gun_ekle_checkBox.setText(tr("hicri_gun_metin"))

        if hasattr(self, "vakitleri_goster_pushButton"):
            self.vakitleri_goster_pushButton.setText(tr("vakitleri_aylik_goster"))
        if hasattr(self, "vakitleri_guncelle_pushButton"):
            self.vakitleri_guncelle_pushButton.setText(tr("vakitleri_guncelle"))

        if hasattr(self, "ayarlar_varsayilan_pushButton"):
            self.ayarlar_varsayilan_pushButton.setText(tr("btn_varsayilan"))
        if hasattr(self, "ayarlar_kaydet_pushButton"):
            self.ayarlar_kaydet_pushButton.setText(tr("btn_kaydet"))
        if hasattr(self, "bolgesel_ayarlar_varsayilan_pushButton"):
            self.bolgesel_ayarlar_varsayilan_pushButton.setText(tr("btn_varsayilan"))
        if hasattr(self, "bolgesel_ayarlar_kaydet_pushButton"):
            self.bolgesel_ayarlar_kaydet_pushButton.setText(tr("btn_kaydet"))

        # Ses Ayarları Sekmesi
        if hasattr(self, "ses_seviyesi_label"):
            self.ses_seviyesi_label.setText(tr("ses_seviyesi"))
        sound_keys = ["sabah", "ogle", "ikindi", "aksam", "yatsi", "dua", "sela", "uyari"]
        for sk in sound_keys:
            lbl = getattr(self, f"{sk}_ses_label", None) or getattr(self, f"{sk}_label", None)
            if lbl:
                lbl.setText(tr(f"{sk}_etiket"))
            btn_ekle = getattr(self, f"{sk}_toolButton", None)
            if btn_ekle:
                btn_ekle.setText(tr("btn_ekle"))
            btn_play = getattr(self, f"btnPlay_{sk}", None)
            if btn_play:
                btn_play.setToolTip(tr("btn_oynat"))

        if hasattr(self, "ses_varsayilan_pushButton"):
            self.ses_varsayilan_pushButton.setText(tr("btn_varsayilan"))
        if hasattr(self, "ses_kaydet_pushButton"):
            self.ses_kaydet_pushButton.setText(tr("btn_kaydet"))

        # Vakit İsimleri
        vakit_map = {
            "imsak": tr("imsak"),
            "gunes": tr("gunes"),
            "ogle": tr("ogle"),
            "ikindi": tr("ikindi"),
            "aksam": tr("aksam"),
            "yatsi": tr("yatsi")
        }
        for vk, bilesenler in getattr(self, "modern_vakit_satirlari", {}).items():
            if vk in vakit_map:
                bilesenler["ad"].setText(vakit_map[vk])
            bilesenler["badge"].setText(tr("vakit_aktif"))

        self.imsak_label.setText(tr("imsak"))
        self.gunes_label.setText(tr("gunes"))
        self.ogle_label.setText(tr("ogle"))
        self.ikindi_label.setText(tr("ikindi"))
        self.aksam_label.setText(tr("aksam"))
        self.yatsi_label.setText(tr("yatsi"))

        # Statusbar
        if hasattr(self, "statusbar") and self.statusbar:
            self.statusbar.showMessage(tr("kaynak"))

        # Hava Durumu
        if hasattr(self, "havaDurumuWidget") and self.havaDurumuWidget:
            self.havaDurumuWidget.retranslateUi()

        if hasattr(self, "saatiGuncelle"):
            self.saatiGuncelle()

    def hakkindaDialog(self):
        from moduller.Hakkinda import Hakkinda
        dialog = Hakkinda()
        dialog.exec_()

    def closeEvent(self, event):
        if self.trayIcon.isVisible():
            event.ignore()
            self.setVisible(False)
        else:
            sys.exit(0)

    def programiKapat(self):
        self.trayIcon.hide()
        sys.exit(0)

if __name__ == "__main__":
    uygulama = QtWidgets.QApplication(sys.argv)
    uygulama.setApplicationName("PyVakitci")
