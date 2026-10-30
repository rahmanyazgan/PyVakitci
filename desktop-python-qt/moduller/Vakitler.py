# -*- coding: utf-8 -*-

# Hazırlayan : Rahman Yazgan (rahmanyazgan@gmail.com)
# Lisans : GPL v.3

import datetime, re, requests, sqlite3, threading
from PyQt5 import QtGui, QtCore, QtWidgets
from bs4 import BeautifulSoup

from moduller.SiteyeBaglan import SiteyeBaglan
from moduller.DiyanetUlkeler import DiyanetUlkeler

class Vakitler(threading.Thread):
    baglantiSorunu = None
    simdikiTarih = None
    saatler = None
    imsak = None
    gunes = None
    ogle = None
    ikindi = None
    aksam = None
    yatsi = None
    ulke = None
    sehir = None
    seciliSehir = None
    ilce = None
    link = None
    tarihDogrumu = None
    tarihHatasi = None
    veritabanindakiYerBilgisi = None
    veritabanindakiYer = None

    AYLAR = {
        "Ocak": "01", "Şubat": "02", "Mart": "03", "Nisan": "04",
        "Mayıs": "05", "Haziran": "06", "Temmuz": "07", "Ağustos": "08",
        "Eylül": "09", "Ekim": "10", "Kasım": "11", "Aralık": "12"
    }

    def __init__(self, sinif, sehir):
        threading.Thread.__init__(self)
        
        self.pyvakitci = sinif
        Vakitler.seciliSehir = sehir
        Vakitler.veritabaniSehir = sehir
        self.diyanetUlkeler = DiyanetUlkeler()
        
    def siteyeBaglantiVarMi(self, url):
        siteyeBaglan = SiteyeBaglan(url)
        siteyeBaglan.start()
        siteyeBaglan.join(3)
        
        if siteyeBaglan.baglantiVarmi:
            return True
        else:
            return False

    def run(self):
        self.pyvakitci.rlock.acquire()
        try:
            Vakitler.saatler = ""
            Vakitler.imsak = ""
            Vakitler.gunes = ""
            Vakitler.ogle = ""
            Vakitler.ikindi = ""
            Vakitler.aksam = ""
            Vakitler.yatsi = ""

            self.diyanetAylikVakitleriAl(yenidenDene=True)
        finally:
            self.pyvakitci.rlock.release()

    def diyanetAylikVakitleriAl(self, yenidenDene=True):
        veritabaniYolu = self.pyvakitci.veritabaniDosyasi
        if not veritabaniYolu or not QtCore.QFile.exists(veritabaniYolu) or QtCore.QFile(veritabaniYolu).size() == 0:
            if yenidenDene:
                self.diyanetAylikVakitleriKaydet(self.ilceSehirKontrolu())
                self.diyanetAylikVakitleriAl(yenidenDene=False)
            return

        connect = None
        guncellemeGerekli = False
        try:
            connect = sqlite3.connect(veritabaniYolu)
            cursor = connect.cursor()

            cursor.execute("SELECT * FROM namazvakitleri LIMIT 1")
            ilkSatir = cursor.fetchone()

            if not ilkSatir:
                guncellemeGerekli = True
            else:
                yerBilgisi = str(ilkSatir[0])

                if yerBilgisi == Vakitler.seciliSehir:
                    cursor.execute("SELECT Tarih FROM namazvakitleri")
                    veriler = cursor.fetchall()

                    tarihler = [str(tarih[0]) for tarih in veriler]

                    if tarihler.count(self.simdikiTarih) >= 1:
                        cursor.execute(
                            "SELECT İmsak, Güneş, Öğle, İkindi, Akşam, Yatsı FROM namazvakitleri WHERE Tarih = ?",
                            (str(self.simdikiTarih),)
                        )
                        vakitSatiri = cursor.fetchone()

                        if vakitSatiri:
                            vakitler = [str(vakit) for vakit in vakitSatiri]

                            Vakitler.saatler = vakitler
                            Vakitler.imsak = vakitler[0]
                            Vakitler.gunes = vakitler[1]
                            Vakitler.ogle = vakitler[2]
                            Vakitler.ikindi = vakitler[3]
                            Vakitler.aksam = vakitler[4]
                            Vakitler.yatsi = vakitler[5]

                            Vakitler.tarihDogrumu = True
                            Vakitler.tarihHatasi = False
                    else:
                        Vakitler.tarihDogrumu = False
                        if yenidenDene:
                            guncellemeGerekli = True
                        else:
                            Vakitler.tarihHatasi = True

                    Vakitler.veritabanindakiYerBilgisi = True
                    Vakitler.veritabanindakiYer = yerBilgisi
                else:
                    Vakitler.veritabanindakiYerBilgisi = False
                    Vakitler.veritabanindakiYer = yerBilgisi
                    if yenidenDene:
                        guncellemeGerekli = True
        except sqlite3.OperationalError:
            guncellemeGerekli = True
        finally:
            if connect:
                connect.close()

        if guncellemeGerekli and yenidenDene:
            self.diyanetAylikVakitleriKaydet(self.ilceSehirKontrolu())
            self.diyanetAylikVakitleriAl(yenidenDene=False)

    def ilceSehirKontrolu(self):
        if Vakitler.ulke in ("TÜRKİYE", 2):
            return "İlçe"
        else:
            return "Şehir"

    def _miladiTarihiCozumle(self, metin, varsayilanTarih):
        try:
            parcalar = metin.strip().split()
            if len(parcalar) >= 3 and parcalar[1] in self.AYLAR:
                gun = parcalar[0].zfill(2)
                ay = self.AYLAR[parcalar[1]]
                yil = parcalar[2]
                return "{}.{}.{}".format(gun, ay, yil)
        except Exception:
            pass
        return varsayilanTarih.date().strftime('%d.%m.%Y')

    def diyanetAylikVakitleriKaydet(self, ilceSehir):
        url = DiyanetUlkeler.BASE_URL
        
        try:
            if (Vakitler.ulke is not None) and (Vakitler.sehir is not None) and Vakitler.link:
                if self.siteyeBaglantiVarMi(url):
                    Vakitler.baglantiSorunu = False
                    veritabaniYolu = self.pyvakitci.veritabaniDosyasi

                    tamUrl = url + Vakitler.link
                    response = SiteyeBaglan.session.get(tamUrl, timeout=15)
                    response.encoding = "utf-8"
                    soup = BeautifulSoup(response.text, "html.parser")
                    table = soup.find_all("table", class_="vakit-table")
                    if not table:
                        return

                    hedefTablo = table[1] if len(table) > 1 else table[0]
                    aylik_vakitler = hedefTablo.find_all("tr")

                    if len(aylik_vakitler) > 1:
                        aylik_vakitler = aylik_vakitler[1:]

                        if QtCore.QFile.exists(veritabaniYolu):
                            QtCore.QFile(veritabaniYolu).remove()

                        connect = sqlite3.connect(veritabaniYolu)
                        try:
                            cursor = connect.cursor()
                            cursor.execute("DROP TABLE IF EXISTS namazvakitleri")
                            sqlString = "CREATE TABLE namazvakitleri (" + str(ilceSehir) + ", Tarih, İmsak, Güneş, Öğle, İkindi, Akşam, Yatsı)"
                            cursor.execute(sqlString)

                            temp_date = datetime.datetime.now()

                            for item in aylik_vakitler:
                                hucreler = [td.get_text(strip=True) for td in item.find_all("td")]
                                if len(hucreler) >= 8:
                                    tarih = self._miladiTarihiCozumle(hucreler[0], temp_date)
                                    imsak, gunes, ogle, ikindi, aksam, yatsi = hucreler[2:8]
                                elif len(hucreler) == 7:
                                    tarih = self._miladiTarihiCozumle(hucreler[0], temp_date)
                                    imsak, gunes, ogle, ikindi, aksam, yatsi = hucreler[1:7]
                                else:
                                    continue

                                cursor.execute(
                                    "INSERT INTO namazvakitleri VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                                    (str(self.veritabaniSehir), tarih, imsak, gunes, ogle, ikindi, aksam, yatsi)
                                )
                                temp_date = temp_date + datetime.timedelta(days=1)

                            connect.commit()
                            Vakitler.tarihHatasi = False
                        finally:
                            connect.close()
                else:
                    Vakitler.baglantiSorunu = True
        except Exception as e:
            print("Vakitler kaydedilirken hata:", e)