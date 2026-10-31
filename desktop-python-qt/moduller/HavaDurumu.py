# -*- coding: utf-8 -*-
import datetime
import json
import math
import urllib.parse
import urllib.request
from PyQt5 import QtCore, QtGui, QtWidgets
from moduller.Localization import tr, day_name, weather_description

WMO_DURUMLAR = {
    0: ("Açık", "gunesli"),
    1: ("Az Bulutlu", "az_bulutlu"),
    2: ("Parçalı Bulutlu", "parcali_bulutlu"),
    3: ("Çok Bulutlu", "bulutlu"),
    45: ("Sisli", "sisli"),
    48: ("Kırağılı Sis", "sisli"),
    51: ("Hafif Çisenti", "yagmurlu"),
    53: ("Çisenti", "yagmurlu"),
    55: ("Yoğun Çisenti", "yagmurlu"),
    56: ("Dondurucu Çisenti", "yagmurlu"),
    57: ("Yoğun Dondurucu Çisenti", "yagmurlu"),
    61: ("Hafif Yağmurlu", "yagmurlu"),
    63: ("Yağmurlu", "yagmurlu"),
    65: ("Şiddetli Yağmurlu", "saganak"),
    66: ("Dondurucu Yağmur", "saganak"),
    67: ("Şiddetli Dondurucu Yağmur", "saganak"),
    71: ("Hafif Kar Yağışlı", "karli"),
    73: ("Kar Yağışlı", "karli"),
    75: ("Yoğun Kar Yağışlı", "karli"),
    77: ("Kar Taneleri", "karli"),
    80: ("Hafif Sağanak", "saganak"),
    81: ("Sağanak Yağışlı", "saganak"),
    82: ("Şiddetli Sağanak", "saganak"),
    85: ("Hafif Kar Sağanağı", "karli"),
    86: ("Yoğun Kar Sağanağı", "karli"),
    95: ("Gök Gürültülü Fırtına", "firtina"),
    96: ("Dolu ve Fırtına", "firtina"),
    99: ("Şiddetli Dolu Fırtınası", "firtina"),
}

GUN_ADLARI = ["Pazartesi", "Salı", "Çarşamba", "Perşembe", "Cuma", "Cumartesi", "Pazar"]

# Türkiye'nin 81 ili için hızlı yedek koordinat tablosu
TR_IL_KOORDINATLARI = {
    "ADANA": (37.0000, 35.3213), "ADIYAMAN": (37.7648, 38.2786), "AFYONKARAHİSAR": (38.7507, 30.5567),
    "AĞRI": (39.7191, 43.0503), "AMASYA": (40.6499, 35.8353), "ANKARA": (39.9208, 32.8541),
    "ANTALYA": (36.8841, 30.7056), "ARTVİN": (41.1828, 41.8183), "AYDIN": (37.8560, 27.8416),
    "BALIKESİR": (39.6484, 27.8826), "BİLECİK": (40.1451, 29.9799), "BİNGÖL": (38.8854, 40.4980),
    "BİTLİS": (38.4006, 42.1095), "BOLU": (40.7392, 31.6089), "BURDUR": (37.7203, 30.2908),
    "BURSA": (40.1885, 29.0610), "ÇANAKKALE": (40.1553, 26.4142), "ÇANKIRI": (40.6013, 33.6134),
    "ÇORUM": (40.5506, 34.9556), "DENİZLİ": (37.7765, 29.0864), "DİYARBAKIR": (37.9144, 40.2306),
    "EDİRNE": (41.6818, 26.5623), "ELAZIĞ": (38.6810, 39.2264), "ERZİNCAN": (39.7500, 39.5000),
    "ERZURUM": (39.9000, 41.2700), "ESKİŞEHİR": (39.7767, 30.5206), "GAZİANTEP": (37.0662, 37.3833),
    "GİRESUN": (40.9128, 38.3895), "GÜMÜŞHANE": (40.4386, 39.5086), "HAKKARİ": (37.5833, 43.7333),
    "HATAY": (36.2021, 36.1606), "ISPARTA": (37.7648, 30.5566), "MERSİN": (36.8121, 34.6415),
    "İSTANBUL": (41.0082, 28.9784), "İZMİR": (38.4237, 27.1428), "KARS": (40.6013, 43.0975),
    "KASTAMONU": (41.3887, 33.7827), "KAYSERİ": (38.7312, 35.4787), "KIRKLARELİ": (41.7333, 27.2167),
    "KIRŞEHİR": (39.1425, 34.1709), "KOCAELİ": (40.8533, 29.8815), "KONYA": (37.8667, 32.4833),
    "KÜTAHYA": (39.4167, 29.9833), "MALATYA": (38.3552, 38.3095), "MANİSA": (38.6191, 27.4289),
    "KAHRAMANMARAŞ": (37.5858, 36.9371), "MARDİN": (37.3212, 40.7245), "MUĞLA": (37.2153, 28.3636),
    "MUŞ": (38.9462, 41.7539), "NEVŞEHİR": (38.6939, 34.6857), "NİĞDE": (37.9667, 34.6833),
    "ORDU": (40.9839, 37.8764), "RİZE": (41.0201, 40.5234), "SAKARYA": (40.7569, 30.3783),
    "SAMSUN": (41.2867, 36.3300), "SİİRT": (37.9333, 41.9500), "SİNOP": (42.0231, 35.1531),
    "SİVAS": (39.7477, 37.0179), "TEKİRDAĞ": (40.9833, 27.5167), "TOKAT": (40.3167, 36.5500),
    "TRABZON": (41.0027, 39.7168), "TUNCELİ": (39.1079, 39.5401), "ŞANLIURFA": (37.1591, 38.7969),
    "UŞAK": (38.6823, 29.4082), "VAN": (38.4891, 43.4089), "YOZGAT": (39.8181, 34.8147),
    "ZONGULDAK": (41.4564, 31.7987), "AKSARAY": (38.3687, 34.0370), "BAYBURT": (40.2552, 40.2249),
    "KARAMAN": (37.1759, 33.2287), "KIRIKKALE": (39.8468, 33.5153), "BATMAN": (37.8812, 41.1351),
    "ŞIRNAK": (37.4187, 42.4918), "BARTIN": (41.6344, 32.3375), "ARDAHAN": (41.1105, 42.7022),
    "IĞDIR": (39.9167, 44.0333), "YALOVA": (40.6500, 29.2667), "KARABÜK": (41.2061, 32.6204),
    "KİLİS": (36.7184, 37.1212), "OSMANİYE": (37.0742, 36.2478), "DÜZCE": (40.8438, 31.1565),
}


class HavaDurumuThread(QtCore.QThread):
    veriHazir = QtCore.pyqtSignal(dict)
    hataOlustu = QtCore.pyqtSignal(str)

    def __init__(self, ulke, sehir, ilce, parent=None):
        super(HavaDurumuThread, self).__init__(parent)
        self.ulke = (ulke or "").strip()
        self.sehir = (sehir or "").strip()
        self.ilce = (ilce or "").strip()

    def _koordinatBul(self):
        aranacaklar = []
        if self.ilce and self.ilce != self.sehir:
            aranacaklar.append(self.ilce)
        if self.sehir:
            aranacaklar.append(self.sehir)

        for sorgu in aranacaklar:
            try:
                q = urllib.parse.quote(sorgu)
                url = f"https://geocoding-api.open-meteo.com/v1/search?name={q}&count=5&language=tr&format=json"
                req = urllib.request.Request(url, headers={"User-Agent": "PyVakitci/1.8"})
                with urllib.request.urlopen(req, timeout=6) as resp:
                    veri = json.loads(resp.read().decode("utf-8"))
                results = veri.get("results") or []
                if results:
                    if self.ulke in ("TÜRKİYE", "TURKIYE"):
                        for r in results:
                            if r.get("country_code") == "TR":
                                return float(r["latitude"]), float(r["longitude"])
                    return float(results[0]["latitude"]), float(results[0]["longitude"])
            except Exception:
                pass

        # Yedek Türkiye il koordinatları
        sehirUst = self.sehir.upper()
        if sehirUst in TR_IL_KOORDINATLARI:
            return TR_IL_KOORDINATLARI[sehirUst]
        ilceUst = self.ilce.upper()
        if ilceUst in TR_IL_KOORDINATLARI:
            return TR_IL_KOORDINATLARI[ilceUst]

        # Varsayılan İstanbul
        return 41.0082, 28.9784

    def run(self):
        try:
            lat, lon = self._koordinatBul()
            url = (
                f"https://api.open-meteo.com/v1/forecast?"
                f"latitude={lat:.4f}&longitude={lon:.4f}"
                f"&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,weather_code,wind_speed_10m"
                f"&daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_probability_max"
                f"&timezone=auto&forecast_days=5"
            )
            req = urllib.request.Request(url, headers={"User-Agent": "PyVakitci/1.8"})
            with urllib.request.urlopen(req, timeout=8) as resp:
                raw = json.loads(resp.read().decode("utf-8"))

            cur = raw.get("current", {})
            daily = raw.get("daily", {})

            wcode = int(cur.get("weather_code", 0))
            durumAd, ikonTip = WMO_DURUMLAR.get(wcode, ("Açık", "gunesli"))

            gunlukListe = []
            times = daily.get("time", [])
            codes = daily.get("weather_code", [])
            maxs = daily.get("temperature_2m_max", [])
            mins = daily.get("temperature_2m_min", [])
            probs = daily.get("precipitation_probability_max", [])

            for i in range(min(5, len(times))):
                tarihStr = times[i]
                wday = 1
                try:
                    dt = datetime.date.fromisoformat(tarihStr)
                    wday = dt.isoweekday()
                    gunAd = "Bugün" if i == 0 else GUN_ADLARI[dt.weekday()]
                except Exception:
                    gunAd = tarihStr
                dc = int(codes[i]) if i < len(codes) else 0
                dAd, dTip = WMO_DURUMLAR.get(dc, ("Açık", "gunesli"))
                tmax = round(maxs[i]) if i < len(maxs) and maxs[i] is not None else 0
                tmin = round(mins[i]) if i < len(mins) and mins[i] is not None else 0
                prob = int(probs[i]) if i < len(probs) and probs[i] is not None else 0
                gunlukListe.append({
                    "gun": gunAd,
                    "weekday": wday,
                    "wcode": dc,
                    "durum": dAd,
                    "tip": dTip,
                    "max": tmax,
                    "min": tmin,
                    "yagis": prob,
                })

            sonuc = {
                "sicaklik": round(cur.get("temperature_2m", 0)),
                "hissedilen": round(cur.get("apparent_temperature", 0)),
                "nem": int(cur.get("relative_humidity_2m", 0)),
                "ruzgar": round(cur.get("wind_speed_10m", 0)),
                "gunduzMu": bool(cur.get("is_day", 1)),
                "wcode": wcode,
                "durum": durumAd,
                "tip": ikonTip,
                "gunluk": gunlukListe,
                "guncellemeSaati": datetime.datetime.now().strftime("%H:%M"),
            }
            self.veriHazir.emit(sonuc)
        except Exception as e:
            self.hataOlustu.emit(str(e))


class HavaDurumuWidget(QtWidgets.QWidget):
    """
    Seçilen il/ilçe için canlı, animasyonlu Hava Durumu ve 5 Günlük Tahmin ekranı.
    """
    def __init__(self, parent=None):
        super(HavaDurumuWidget, self).__init__(parent)
        self.setGeometry(QtCore.QRect(10, 8, 306, 378))
        self.setObjectName("hava_durumu_widget")
        self.animFaz = 0.0
        self.tema = "varsayilan"

        self.ulke = "TÜRKİYE"
        self.sehir = "İSTANBUL"
        self.ilce = "ESENYURT"
        self.yukleniyor = True
        self.hataMesaji = ""
        self.veri = None
        self.aktifThread = None
        self._sonSorguAnahtari = None

        # Yenile butonu
        self.yenileBtn = QtWidgets.QPushButton("↻", self)
        self.yenileBtn.setGeometry(QtCore.QRect(228, 12, 64, 24))
        self.yenileBtn.setCursor(QtCore.Qt.PointingHandCursor)
        self.yenileBtn.clicked.connect(lambda: self.havaDurumunuGuncelle(zorla=True))
        self.retranslateUi()

    def retranslateUi(self):
        if hasattr(self, "yenileBtn") and self.yenileBtn:
            self.yenileBtn.setText("↻ " + tr("yenile"))
            self.yenileBtn.setToolTip(tr("hava_yenile_tooltip"))
        self.update()

    def temaAyarla(self, tema):
        self.tema = tema or "varsayilan"
        self.update()

    def konumAyarla(self, ulke, sehir, ilce, tema="varsayilan", zorla=False):
        self.ulke = ulke or "TÜRKİYE"
        self.sehir = sehir or "İSTANBUL"
        self.ilce = ilce or self.sehir
        self.tema = tema or "varsayilan"

        anahtar = f"{self.ulke}|{self.sehir}|{self.ilce}"
        if zorla or anahtar != self._sonSorguAnahtari or self.veri is None:
            if self.sehir:
                self._sonSorguAnahtari = anahtar
                self.havaDurumunuGuncelle(zorla=True)
        self.update()

    def havaDurumunuGuncelle(self, zorla=False):
        if self.aktifThread is not None and self.aktifThread.isRunning():
            return
        self.yukleniyor = True
        self.hataMesaji = ""
        self.update()

        self.aktifThread = HavaDurumuThread(self.ulke, self.sehir, self.ilce, self)
        self.aktifThread.veriHazir.connect(self._veriGeldi)
        self.aktifThread.hataOlustu.connect(self._hataGeldi)
        self.aktifThread.start()

    def _veriGeldi(self, veri):
        self.veri = veri
        self.yukleniyor = False
        self.hataMesaji = ""
        self.update()

    def _hataGeldi(self, hata):
        self.yukleniyor = False
        self.hataMesaji = "Hava durumu bilgisi alınamadı."
        self.update()

    def animasyonAdimi(self):
        self.animFaz += 0.06
        if self.animFaz > 6.28318:
            self.animFaz -= 6.28318
        self.update()

    def _havaIkonuCiz(self, p, cx, cy, olcek, tip, gunduzMu=True):
        p.save()
        p.translate(cx, cy)
        p.scale(olcek, olcek)

        pulse = (math.sin(self.animFaz) + 1.0) * 0.5
        drift = math.sin(self.animFaz) * 2.5

        if tip == "gunesli":
            if gunduzMu:
                # Dönen güneş ışınları
                p.setPen(QtGui.QPen(QtGui.QColor(251, 191, 36, 200), 2.2, QtCore.Qt.SolidLine, QtCore.Qt.RoundCap))
                for i in range(8):
                    ang = self.animFaz * 0.5 + i * (math.pi / 4.0)
                    r1, r2 = 14.0, 19.0 + pulse * 1.5
                    p.drawLine(
                        QtCore.QPointF(r1 * math.cos(ang), r1 * math.sin(ang)),
                        QtCore.QPointF(r2 * math.cos(ang), r2 * math.sin(ang)),
                    )
                p.setPen(QtCore.Qt.NoPen)
                p.setBrush(QtGui.QColor(251, 191, 36, 60))
                p.drawEllipse(QtCore.QPointF(0, 0), 15.0 + pulse * 2.0, 15.0 + pulse * 2.0)
                p.setBrush(QtGui.QColor(250, 204, 21))
                p.drawEllipse(QtCore.QPointF(0, 0), 11.0, 11.0)
            else:
                # Gece Hilal
                moon = QtGui.QPainterPath()
                moon.addEllipse(QtCore.QPointF(0, 0), 12.0, 12.0)
                cut = QtGui.QPainterPath()
                cut.addEllipse(QtCore.QPointF(-5.0, -3.0), 11.0, 11.0)
                p.setPen(QtCore.Qt.NoPen)
                p.setBrush(QtGui.QColor(254, 240, 138))
                p.drawPath(moon.subtracted(cut))

        elif tip in ("az_bulutlu", "parcali_bulutlu"):
            # Arkada güneş
            p.setPen(QtCore.Qt.NoPen)
            p.setBrush(QtGui.QColor(250, 204, 21))
            p.drawEllipse(QtCore.QPointF(-6.0, -6.0), 9.5, 9.5)
            # Önde kayan bulut
            self._bulutCiz(p, drift * 0.6, 4.0, QtGui.QColor(241, 245, 249))

        elif tip in ("bulutlu", "sisli"):
            self._bulutCiz(p, -3.0, -2.0, QtGui.QColor(148, 163, 184, 200))
            self._bulutCiz(p, drift * 0.6, 4.0, QtGui.QColor(226, 232, 240))

        elif tip in ("yagmurlu", "saganak"):
            self._bulutCiz(p, drift * 0.5, -2.0, QtGui.QColor(148, 163, 184))
            # Yağmur damlaları
            p.setPen(QtGui.QPen(QtGui.QColor(56, 189, 248), 2.0, QtCore.Qt.SolidLine, QtCore.Qt.RoundCap))
            for i, xOff in enumerate([-8.0, 0.0, 8.0]):
                yOff = ((self.animFaz * 6.0 + i * 3.0) % 10.0)
                p.drawLine(QtCore.QPointF(xOff, 8.0 + yOff), QtCore.QPointF(xOff - 2.0, 13.0 + yOff))

        elif tip == "karli":
            self._bulutCiz(p, drift * 0.5, -2.0, QtGui.QColor(203, 213, 225))
            p.setPen(QtCore.Qt.NoPen)
            p.setBrush(QtGui.QColor(255, 255, 255))
            for i, xOff in enumerate([-8.0, 0.0, 8.0]):
                yOff = ((self.animFaz * 4.0 + i * 3.0) % 10.0)
                p.drawEllipse(QtCore.QPointF(xOff, 9.0 + yOff), 2.0, 2.0)

        elif tip == "firtina":
            self._bulutCiz(p, drift * 0.5, -3.0, QtGui.QColor(100, 116, 139))
            # Şimşek
            bolt = QtGui.QPolygonF([
                QtCore.QPointF(1.0, 4.0),
                QtCore.QPointF(-4.0, 12.0),
                QtCore.QPointF(0.0, 12.0),
                QtCore.QPointF(-2.0, 19.0),
                QtCore.QPointF(5.0, 10.0),
                QtCore.QPointF(1.0, 10.0),
            ])
            p.setPen(QtCore.Qt.NoPen)
            p.setBrush(QtGui.QColor(250, 204, 21))
            p.drawPolygon(bolt)

        p.restore()

    def _bulutCiz(self, p, x, y, renk):
        p.setPen(QtCore.Qt.NoPen)
        p.setBrush(renk)
        path = QtGui.QPainterPath()
        path.setFillRule(QtCore.Qt.WindingFill)
        path.addRoundedRect(QtCore.QRectF(x - 13, y - 2, 26, 11), 5, 5)
        path.addEllipse(QtCore.QPointF(x - 5, y - 4), 7, 7)
        path.addEllipse(QtCore.QPointF(x + 3, y - 6), 8, 8)
        p.drawPath(path)

    def paintEvent(self, event):
        p = QtGui.QPainter(self)
        p.setRenderHint(QtGui.QPainter.Antialiasing)
        p.setRenderHint(QtGui.QPainter.TextAntialiasing)

        isLight = (self.tema == "varsayilan")

        # 1. Ana Kart Arka Planı
        bgRect = QtCore.QRectF(2, 2, 302, 372)
        if isLight:
            p.setPen(QtGui.QPen(QtGui.QColor(219, 227, 238), 1.2))
            p.setBrush(QtGui.QColor(255, 255, 255))
        elif self.tema == "zumrut":
            p.setPen(QtGui.QPen(QtGui.QColor(6, 78, 59), 1.2))
            p.setBrush(QtGui.QColor(6, 32, 24))
        elif self.tema == "gece_mavisi":
            p.setPen(QtGui.QPen(QtGui.QColor(30, 58, 138), 1.2))
            p.setBrush(QtGui.QColor(10, 22, 40))
        elif self.tema == "kehribar":
            p.setPen(QtGui.QPen(QtGui.QColor(120, 53, 15), 1.2))
            p.setBrush(QtGui.QColor(36, 22, 14))
        else:
            p.setPen(QtGui.QPen(QtGui.QColor(39, 39, 42), 1.2))
            p.setBrush(QtGui.QColor(18, 18, 20))
        p.drawRoundedRect(bgRect, 10, 10)

        # 2. Üst Konum Başlığı
        ustYaziRenk = QtGui.QColor(100, 116, 139) if isLight else QtGui.QColor(148, 163, 184)
        baslikRenk = QtGui.QColor(15, 23, 42) if isLight else QtGui.QColor(248, 250, 252)

        p.setPen(ustYaziRenk)
        p.setFont(QtGui.QFont("Segoe UI", 8, QtGui.QFont.Bold))
        konumUst = f"{self.ulke} • {self.sehir}" if (self.sehir and self.sehir != self.ilce) else self.ulke
        p.drawText(QtCore.QRectF(14, 10, 205, 15), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, konumUst)

        p.setPen(baslikRenk)
        p.setFont(QtGui.QFont("Segoe UI", 12, QtGui.QFont.Bold))
        p.drawText(QtCore.QRectF(14, 25, 205, 22), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, self.ilce)

        if self.yukleniyor and self.veri is None:
            p.setPen(ustYaziRenk)
            p.setFont(QtGui.QFont("Segoe UI", 10, QtGui.QFont.Bold))
            p.drawText(QtCore.QRectF(20, 150, 266, 40), QtCore.Qt.AlignCenter, tr("hava_yukleniyor"))
            return

        if self.hataMesaji and self.veri is None:
            p.setPen(QtGui.QColor(239, 68, 68))
            p.setFont(QtGui.QFont("Segoe UI", 9, QtGui.QFont.Bold))
            p.drawText(QtCore.QRectF(20, 150, 266, 40), QtCore.Qt.AlignCenter, tr("hava_hata"))
            return

        if not self.veri:
            return

        # 3. Hero Anlık Hava Durumu Kartı
        heroRect = QtCore.QRectF(12, 52, 282, 102)
        heroGrad = QtGui.QLinearGradient(12, 52, 294, 154)
        if isLight:
            heroGrad.setColorAt(0.0, QtGui.QColor(2, 132, 199))
            heroGrad.setColorAt(1.0, QtGui.QColor(37, 99, 235))
            p.setPen(QtGui.QPen(QtGui.QColor(147, 197, 253), 1))
        else:
            heroGrad.setColorAt(0.0, QtGui.QColor(15, 23, 42, 240))
            heroGrad.setColorAt(1.0, QtGui.QColor(30, 58, 138, 220))
            p.setPen(QtGui.QPen(QtGui.QColor(56, 189, 248, 120), 1.2))
        p.setBrush(heroGrad)
        p.drawRoundedRect(heroRect, 10, 10)

        # Sıcaklık
        p.setPen(QtGui.QColor(255, 255, 255))
        p.setFont(QtGui.QFont("Segoe UI", 26, QtGui.QFont.Bold))
        p.drawText(QtCore.QRectF(24, 58, 140, 44), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, f"{self.veri['sicaklik']}°C")

        # Durum Açıklaması
        durumAciklama = weather_description(self.veri["wcode"]) if "wcode" in self.veri else self.veri["durum"]
        p.setPen(QtGui.QColor(224, 242, 254))
        p.setFont(QtGui.QFont("Segoe UI", 10, QtGui.QFont.Bold))
        p.drawText(QtCore.QRectF(24, 104, 175, 20), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, durumAciklama)

        # Bugün En Yüksek / En Düşük
        if self.veri.get("gunluk"):
            bgn = self.veri["gunluk"][0]
            p.setPen(QtGui.QColor(186, 230, 253))
            p.setFont(QtGui.QFont("Segoe UI", 8))
            p.drawText(
                QtCore.QRectF(24, 126, 185, 18),
                QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter,
                f"{tr('en_yuksek')}: {bgn['max']}°   •   {tr('en_dusuk')}: {bgn['min']}°",
            )

        # Sağda Animasyonlu Hava İkonu
        self._havaIkonuCiz(p, 242.0, 98.0, 1.45, self.veri["tip"], self.veri.get("gunduzMu", True))

        # 4. Üçlü Detay Kutuları (Hissedilen, Nem, Rüzgâr)
        detaylar = [
            (tr("hava_hissedilen"), f"{self.veri['hissedilen']}°C"),
            (tr("hava_nem"), f"%{self.veri['nem']}"),
            (tr("hava_ruzgar"), f"{self.veri['ruzgar']} km/s"),
        ]
        for idx, (etiket, deger) in enumerate(detaylar):
            dx = 12.0 + idx * 96.0
            dy = 162.0
            dRect = QtCore.QRectF(dx, dy, 90.0, 46.0)
            if isLight:
                p.setPen(QtGui.QPen(QtGui.QColor(226, 232, 240), 1))
                p.setBrush(QtGui.QColor(248, 250, 252))
            else:
                p.setPen(QtGui.QPen(QtGui.QColor(51, 65, 85, 130), 1))
                p.setBrush(QtGui.QColor(30, 41, 59, 140))
            p.drawRoundedRect(dRect, 7, 7)

            p.setPen(ustYaziRenk)
            p.setFont(QtGui.QFont("Segoe UI", 7))
            p.drawText(QtCore.QRectF(dx + 4, dy + 5, 82, 14), QtCore.Qt.AlignCenter, etiket)

            p.setPen(baslikRenk)
            p.setFont(QtGui.QFont("Segoe UI", 10, QtGui.QFont.Bold))
            p.drawText(QtCore.QRectF(dx + 4, dy + 20, 82, 20), QtCore.Qt.AlignCenter, deger)

        # 5. 5 Günlük Hava Tahmini Satırları
        p.setPen(ustYaziRenk)
        p.setFont(QtGui.QFont("Segoe UI", 8, QtGui.QFont.Bold))
        p.drawText(QtCore.QRectF(14, 214, 180, 16), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, tr("hava_tahmin"))
        saatMetin = tr("hava_saat").replace("%1", self.veri.get('guncellemeSaati', '')) if "%1" in tr("hava_saat") else f"Saat: {self.veri.get('guncellemeSaati', '')}"
        p.drawText(
            QtCore.QRectF(180, 214, 112, 16),
            QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter,
            saatMetin,
        )

        gunluk = self.veri.get("gunluk", [])
        basY = 233.0
        satirH = 25.0
        aralikY = 27.0

        for idx, g in enumerate(gunluk[:5]):
            sy = basY + idx * aralikY
            sRect = QtCore.QRectF(12.0, sy, 282.0, satirH)
            if isLight:
                p.setPen(QtGui.QPen(QtGui.QColor(226, 232, 240), 1))
                p.setBrush(QtGui.QColor(248, 250, 252) if idx > 0 else QtGui.QColor(239, 246, 255))
            else:
                p.setPen(QtGui.QPen(QtGui.QColor(51, 65, 85, 110), 1))
                p.setBrush(QtGui.QColor(30, 41, 59, 120) if idx > 0 else QtGui.QColor(30, 58, 138, 140))
            p.drawRoundedRect(sRect, 6, 6)

            # Gün adı
            gunAd = tr("bugun") if idx == 0 else day_name(g.get("weekday", 1))
            p.setPen(baslikRenk)
            p.setFont(QtGui.QFont("Segoe UI", 8, QtGui.QFont.Bold if idx == 0 else QtGui.QFont.Normal))
            p.drawText(QtCore.QRectF(20, sy, 75, satirH), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, gunAd)

            # Küçük İkon
            self._havaIkonuCiz(p, 108.0, sy + satirH / 2.0, 0.52, g["tip"], True)

            # Durum veya Yağış İhtimali
            durumGun = weather_description(g["wcode"]) if "wcode" in g else g["durum"]
            if g.get("yagis", 0) >= 20:
                durumMetni = f"{durumGun} (%{g['yagis']})"
            else:
                durumMetni = durumGun
            p.setPen(ustYaziRenk)
            p.setFont(QtGui.QFont("Segoe UI", 7))
            p.drawText(QtCore.QRectF(124, sy, 105, satirH), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, durumMetni)

            # Max / Min Sıcaklık
            p.setPen(baslikRenk)
            p.setFont(QtGui.QFont("Segoe UI", 8, QtGui.QFont.Bold))
            p.drawText(
                QtCore.QRectF(225, sy, 62, satirH),
                QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter,
                f"{g['max']}° / {g['min']}°",
            )
