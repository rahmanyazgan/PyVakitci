# -*- coding: utf-8 -*-
import math
from PyQt5 import QtCore, QtGui, QtWidgets

TURKCE_BUYUK_VAKIT = {
    "imsak": "İMSAK",
    "gunes": "GÜNEŞ",
    "ogle": "ÖĞLE",
    "ikindi": "İKİNDİ",
    "aksam": "AKŞAM",
    "yatsi": "YATSI",
    "sabah": "İMSAK",
}

class VakitSaatiWidget(QtWidgets.QWidget):
    """
    24 Saatlik Dairesel Vakit Saati (Kadran).
    Günün 24 saatini döngüsel bir kadranda vakit renk dilimleri,
    hareketli zaman ibresi, parıltılı vakit pinleri, merkez dijital HUD
    ve alt vakit mini kartlarıyla gösterir.
    """
    def __init__(self, parent=None):
        super(VakitSaatiWidget, self).__init__(parent)
        self.setGeometry(QtCore.QRect(10, 8, 306, 376))
        self.setObjectName("vakit_saati_widget")
        self.animFaz = 0.0

        # Veriler
        self.ulke = "TÜRKİYE"
        self.sehir = "İSTANBUL"
        self.ilce = "ESENYURT"
        self.miladiTarih = ""
        self.hicriTarih = ""
        self.diniGun = ""
        self.simdikiSaatStr = "00:00:00"
        self.aktifVakit = "imsak"
        self.sonrakiVakitAd = "Güneş"
        self.kalanSureStr = "00:00:00"
        self.tema = "varsayilan"

        self.vakitler = {
            "imsak": "06:00",
            "gunes": "07:25",
            "ogle": "12:54",
            "ikindi": "15:45",
            "aksam": "18:13",
            "yatsi": "19:33",
        }

    def _dakikayaCevir(self, saatStr):
        try:
            parts = str(saatStr).replace("<font color='#FF0000'>", "").replace("</font>", "").split(":")
            return int(parts[0]) * 60 + int(parts[1])
        except Exception:
            return 0

    def veriGuncelle(self, ulke, sehir, ilce, miladi, hicri, diniGun, saatStr, aktifVakit, sonrakiVakitAd, kalanSureStr, vakitler, tema):
        self.ulke = ulke or "TÜRKİYE"
        self.sehir = sehir or ""
        self.ilce = ilce or sehir or ""
        self.miladiTarih = miladi or ""
        self.hicriTarih = hicri or ""
        self.diniGun = diniGun or ""
        self.simdikiSaatStr = saatStr or "00:00:00"
        self.aktifVakit = aktifVakit or "imsak"
        self.sonrakiVakitAd = sonrakiVakitAd or ""
        self.kalanSureStr = kalanSureStr or "00:00:00"
        self.vakitler = vakitler or self.vakitler
        self.tema = tema or "varsayilan"
        self.update()

    def animasyonAdimi(self):
        self.animFaz += 0.06
        if self.animFaz > 6.28318:
            self.animFaz -= 6.28318
        self.update()

    def paintEvent(self, event):
        p = QtGui.QPainter(self)
        p.setRenderHint(QtGui.QPainter.Antialiasing)
        p.setRenderHint(QtGui.QPainter.TextAntialiasing)

        isLight = (self.tema == "varsayilan")

        # 1. Arka Plan Kartı
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
        else: # koyu
            p.setPen(QtGui.QPen(QtGui.QColor(39, 39, 42), 1.2))
            p.setBrush(QtGui.QColor(18, 18, 20))
        p.drawRoundedRect(bgRect, 10, 10)

        # 2. Üst Başlık (Konum, Tarih ve Özel Gün)
        if isLight:
            p.setPen(QtGui.QColor(100, 116, 139))
            p.setFont(QtGui.QFont("Segoe UI", 8, QtGui.QFont.Bold))
            konumBaslik = f"{self.ulke} • {self.sehir}" if (self.sehir and self.sehir != self.ilce) else self.ulke
            p.drawText(QtCore.QRectF(14, 10, 160, 15), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, konumBaslik)

            p.setPen(QtGui.QColor(15, 23, 42))
            p.setFont(QtGui.QFont("Segoe UI", 12, QtGui.QFont.Bold))
            p.drawText(QtCore.QRectF(14, 25, 160, 22), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, self.ilce)

            p.setPen(QtGui.QColor(71, 85, 105))
            p.setFont(QtGui.QFont("Segoe UI", 8))
            p.drawText(QtCore.QRectF(140, 10, 150, 15), QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter, self.miladiTarih)
            p.drawText(QtCore.QRectF(130, 25, 160, 20), QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter, self.hicriTarih)
        else:
            p.setPen(QtGui.QColor(148, 163, 184))
            p.setFont(QtGui.QFont("Segoe UI", 8, QtGui.QFont.Bold))
            konumBaslik = f"{self.ulke} • {self.sehir}" if (self.sehir and self.sehir != self.ilce) else self.ulke
            p.drawText(QtCore.QRectF(14, 10, 160, 15), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, konumBaslik)

            p.setPen(QtGui.QColor(248, 250, 252))
            p.setFont(QtGui.QFont("Segoe UI", 12, QtGui.QFont.Bold))
            p.drawText(QtCore.QRectF(14, 25, 160, 22), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, self.ilce)

            p.setPen(QtGui.QColor(148, 163, 184))
            p.setFont(QtGui.QFont("Segoe UI", 8))
            p.drawText(QtCore.QRectF(140, 10, 150, 15), QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter, self.miladiTarih)
            p.drawText(QtCore.QRectF(130, 25, 160, 20), QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter, self.hicriTarih)

        # 3. VAKİT SAATİ KADRANI
        cx, cy = 153.0, 154.0
        r_dis = 92.0
        r_ic = 76.0

        def dkToAngle(dk):
            # 24 Saatlik döngü: 12:00 tepede (-90°), 00:00 altta (90°)
            return (dk / 1440.0) * 360.0 - 90.0

        imsak_dk = self._dakikayaCevir(self.vakitler.get("imsak", "06:00"))
        gunes_dk = self._dakikayaCevir(self.vakitler.get("gunes", "07:25"))
        ogle_dk = self._dakikayaCevir(self.vakitler.get("ogle", "12:54"))
        ikindi_dk = self._dakikayaCevir(self.vakitler.get("ikindi", "15:45"))
        aksam_dk = self._dakikayaCevir(self.vakitler.get("aksam", "18:13"))
        yatsi_dk = self._dakikayaCevir(self.vakitler.get("yatsi", "19:33"))

        dilimler = [
            ("imsak", imsak_dk, gunes_dk, QtGui.QColor(249, 115, 22), "Sabah"),
            ("gunes", gunes_dk, ogle_dk, QtGui.QColor(14, 165, 233), "Güneş"),
            ("ogle", ogle_dk, ikindi_dk, QtGui.QColor(234, 179, 8), "Öğle"),
            ("ikindi", ikindi_dk, aksam_dk, QtGui.QColor(245, 158, 11), "İkindi"),
            ("aksam", aksam_dk, yatsi_dk, QtGui.QColor(168, 85, 247), "Akşam"),
            ("yatsi", yatsi_dk, imsak_dk + 1440, QtGui.QColor(59, 130, 246), "Yatsı"),
        ]

        # Kadran Zemin Çemberi
        ringColor = QtGui.QColor(226, 232, 240) if isLight else QtGui.QColor(39, 39, 42, 160)
        p.setPen(QtGui.QPen(ringColor, 14, QtCore.Qt.SolidLine, QtCore.Qt.FlatCap))
        p.drawEllipse(QtCore.QPointF(cx, cy), (r_dis + r_ic) / 2.0, (r_dis + r_ic) / 2.0)

        # Renkli Vakit Yayları
        for anahtar, b_dk, s_dk, renk, ad in dilimler:
            span_dk = s_dk - b_dk
            if span_dk < 0:
                span_dk += 1440

            start_deg = - (dkToAngle(b_dk))
            span_deg = - (span_dk / 1440.0 * 360.0)

            aktifMi = (anahtar == self.aktifVakit)
            pen_w = 15 if aktifMi else 13
            c = QtGui.QColor(renk)
            if aktifMi:
                pulse = (math.sin(self.animFaz) + 1.0) * 0.5
                c.setAlpha(int(210 + 45 * pulse))
            else:
                c.setAlpha(110 if isLight else 130)

            p.setPen(QtGui.QPen(c, pen_w, QtCore.Qt.SolidLine, QtCore.Qt.FlatCap))
            arcRect = QtCore.QRectF(cx - (r_dis + r_ic)/2.0, cy - (r_dis + r_ic)/2.0, r_dis + r_ic, r_dis + r_ic)
            p.drawArc(arcRect, int(start_deg * 16), int(span_deg * 16))

        # Saat İşaretçileri (06, 12, 18, 24)
        p.setFont(QtGui.QFont("Segoe UI", 7, QtGui.QFont.Bold))
        for saat_h, etiket in [(0, "24"), (6, "06"), (12, "12"), (18, "18")]:
            ang_rad = math.radians(dkToAngle(saat_h * 60))
            tx = cx + (r_dis + 10) * math.cos(ang_rad)
            ty = cy + (r_dis + 10) * math.sin(ang_rad)
            p.setPen(QtGui.QColor(100, 116, 139) if isLight else QtGui.QColor(148, 163, 184))
            p.drawText(QtCore.QRectF(tx - 12, ty - 8, 24, 16), QtCore.Qt.AlignCenter, etiket)

        # Vakit Pinleri (Noktalar)
        for anahtar, dk_val, ad_str, renk_val in [
            ("imsak", imsak_dk, "İmsak", QtGui.QColor("#ea580c")),
            ("gunes", gunes_dk, "Güneş", QtGui.QColor("#0ea5e9")),
            ("ogle", ogle_dk, "Öğle", QtGui.QColor("#eab308")),
            ("ikindi", ikindi_dk, "İkindi", QtGui.QColor("#f97316")),
            ("aksam", aksam_dk, "Akşam", QtGui.QColor("#a855f7")),
            ("yatsi", yatsi_dk, "Yatsı", QtGui.QColor("#3b82f6")),
        ]:
            ang_rad = math.radians(dkToAngle(dk_val))
            px = cx + (r_dis + r_ic)/2.0 * math.cos(ang_rad)
            py = cy + (r_dis + r_ic)/2.0 * math.sin(ang_rad)
            aktifMi = (anahtar == self.aktifVakit)
            dot_r = 5.2 if aktifMi else 3.8
            p.setPen(QtGui.QPen(QtGui.QColor(255, 255, 255), 1.5))
            p.setBrush(renk_val if not aktifMi else QtGui.QColor("#ffffff"))
            p.drawEllipse(QtCore.QPointF(px, py), dot_r, dot_r)

        # 4. Canlı Zaman İbresi (Şimdiki Saati Gösteren İbre)
        parts = self.simdikiSaatStr.split(":")
        try:
            cur_dk = int(parts[0]) * 60 + int(parts[1]) + (int(parts[2])/60.0 if len(parts) > 2 else 0)
        except Exception:
            cur_dk = 0
        hand_ang_rad = math.radians(dkToAngle(cur_dk))

        # İbre Çizgisi
        hx = cx + (r_dis + 2) * math.cos(hand_ang_rad)
        hy = cy + (r_dis + 2) * math.sin(hand_ang_rad)
        p.setPen(QtGui.QPen(QtGui.QColor(239, 68, 68), 2.5, QtCore.Qt.SolidLine, QtCore.Qt.RoundCap))
        p.drawLine(QtCore.QPointF(cx, cy), QtCore.QPointF(hx, hy))

        # İbre Ucu Parıltılı Başlığı
        p.setPen(QtGui.QPen(QtGui.QColor(255, 255, 255), 2))
        p.setBrush(QtGui.QColor(239, 68, 68))
        p.drawEllipse(QtCore.QPointF(hx, hy), 4.5, 4.5)

        # 5. Merkez Gösterge Camı (Center HUD)
        center_r = 53.0
        if isLight:
            p.setPen(QtGui.QPen(QtGui.QColor(186, 230, 253), 1.5))
            hudGrad = QtGui.QRadialGradient(cx, cy, center_r)
            hudGrad.setColorAt(0, QtGui.QColor(240, 249, 255))
            hudGrad.setColorAt(1, QtGui.QColor(224, 242, 254))
            p.setBrush(hudGrad)
        else:
            p.setPen(QtGui.QPen(QtGui.QColor(71, 85, 105, 180), 1.5))
            hudGrad = QtGui.QRadialGradient(cx, cy, center_r)
            hudGrad.setColorAt(0, QtGui.QColor(30, 41, 59, 250))
            hudGrad.setColorAt(1, QtGui.QColor(15, 23, 42, 255))
            p.setBrush(hudGrad)
        p.drawEllipse(QtCore.QPointF(cx, cy), center_r, center_r)

        # Merkez Metinleri
        aktifRenk = QtGui.QColor(2, 132, 199) if isLight else QtGui.QColor(56, 189, 248)
        p.setPen(aktifRenk)
        p.setFont(QtGui.QFont("Segoe UI", 8, QtGui.QFont.Bold))
        aktifBaslik = TURKCE_BUYUK_VAKIT.get(str(self.aktifVakit).lower(), "İMSAK")
        p.drawText(QtCore.QRectF(cx - 50, cy - 38, 100, 16), QtCore.Qt.AlignCenter, f"✦ {aktifBaslik} ✦")

        saatRenk = QtGui.QColor(15, 23, 42) if isLight else QtGui.QColor(255, 255, 255)
        p.setPen(saatRenk)
        p.setFont(QtGui.QFont("Segoe UI", 13, QtGui.QFont.Bold))
        saatGosterim = self.simdikiSaatStr
        if len(saatGosterim.split(":")) < 3:
            saatGosterim = QtCore.QTime.currentTime().toString("hh:mm:ss")
        p.drawText(QtCore.QRectF(cx - 50, cy - 20, 100, 22), QtCore.Qt.AlignCenter, saatGosterim)

        kalanBaslikRenk = QtGui.QColor(100, 116, 139) if isLight else QtGui.QColor(148, 163, 184)
        p.setPen(kalanBaslikRenk)
        p.setFont(QtGui.QFont("Segoe UI", 7))
        sonrakiHam = (self.sonrakiVakitAd or "").replace(" :", "")
        for t in ["'e Kalan Süre", "’e Kalan Süre", "'a Kalan Süre", "’a Kalan Süre", " Kalan Süre", "'e Kalan", "’e Kalan", "'a Kalan", "’a Kalan"]:
            sonrakiHam = sonrakiHam.replace(t, "")
        sonrakiEtiket = f"{sonrakiHam.strip()}'e Kalan"
        p.drawText(QtCore.QRectF(cx - 50, cy + 2, 100, 14), QtCore.Qt.AlignCenter, sonrakiEtiket)

        kalanRenk = QtGui.QColor(217, 119, 6) if isLight else QtGui.QColor(251, 191, 36)
        p.setPen(kalanRenk)
        p.setFont(QtGui.QFont("Segoe UI", 10, QtGui.QFont.Bold))
        p.drawText(QtCore.QRectF(cx - 50, cy + 16, 100, 20), QtCore.Qt.AlignCenter, self.kalanSureStr)

        # 6. Alt Vakit Mini Kartları (2 Satır x 3 Sütun)
        kartlar = [
            ("İmsak", self.vakitler.get("imsak", "--:--"), "imsak"),
            ("Güneş", self.vakitler.get("gunes", "--:--"), "gunes"),
            ("Öğle", self.vakitler.get("ogle", "--:--"), "ogle"),
            ("İkindi", self.vakitler.get("ikindi", "--:--"), "ikindi"),
            ("Akşam", self.vakitler.get("aksam", "--:--"), "aksam"),
            ("Yatsı", self.vakitler.get("yatsi", "--:--"), "yatsi"),
        ]

        startX = 12.0
        startY = 265.0
        cardW = 90.0
        cardH = 46.0
        gapX = 6.0
        gapY = 6.0

        for idx, (ad, saat, key) in enumerate(kartlar):
            row = idx // 3
            col = idx % 3
            kx = startX + col * (cardW + gapX)
            ky = startY + row * (cardH + gapY)
            cardRect = QtCore.QRectF(kx, ky, cardW, cardH)

            aktifMi = (key == self.aktifVakit)
            if isLight:
                if aktifMi:
                    pulse = (math.sin(self.animFaz) + 1.0) * 0.5
                    borderAlpha = int(180 + 75 * pulse)
                    p.setPen(QtGui.QPen(QtGui.QColor(37, 99, 235, borderAlpha), 1.6))
                    p.setBrush(QtGui.QColor(239, 246, 255))
                else:
                    p.setPen(QtGui.QPen(QtGui.QColor(226, 232, 240), 1))
                    p.setBrush(QtGui.QColor(248, 250, 252))
            else:
                if aktifMi:
                    pulse = (math.sin(self.animFaz) + 1.0) * 0.5
                    borderAlpha = int(180 + 75 * pulse)
                    p.setPen(QtGui.QPen(QtGui.QColor(56, 189, 248, borderAlpha), 1.6))
                    p.setBrush(QtGui.QColor(30, 58, 138, 180))
                else:
                    p.setPen(QtGui.QPen(QtGui.QColor(51, 65, 85, 120), 1))
                    p.setBrush(QtGui.QColor(30, 41, 59, 140))

            p.drawRoundedRect(cardRect, 7, 7)

            # İsim
            if isLight:
                p.setPen(QtGui.QColor(29, 78, 216) if aktifMi else QtGui.QColor(100, 116, 139))
            else:
                p.setPen(QtGui.QColor(56, 189, 248) if aktifMi else QtGui.QColor(148, 163, 184))
            p.setFont(QtGui.QFont("Segoe UI", 8, QtGui.QFont.Bold if aktifMi else QtGui.QFont.Normal))
            p.drawText(QtCore.QRectF(kx + 6, ky + 5, cardW - 12, 16), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, ad)

            # Saat
            if isLight:
                p.setPen(QtGui.QColor(220, 38, 38) if aktifMi else QtGui.QColor(15, 23, 42))
            else:
                p.setPen(QtGui.QColor(251, 191, 36) if aktifMi else QtGui.QColor(255, 255, 255))
            p.setFont(QtGui.QFont("Segoe UI", 11, QtGui.QFont.Bold))
            p.drawText(QtCore.QRectF(kx + 6, ky + 21, cardW - 12, 20), QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter, saat)

            if aktifMi:
                p.setPen(QtCore.Qt.NoPen)
                p.setBrush(QtGui.QColor(37, 99, 235) if isLight else QtGui.QColor(56, 189, 248))
                p.drawEllipse(QtCore.QPointF(kx + cardW - 12, ky + 12), 3, 3)


class GokyuzuWidget(QtWidgets.QWidget):
    """
    Dinamik Gökyüzü ve Atmosferik Vakit Görünümü.
    Mevcut vakte göre değişen gerçekçi gökyüzü gradyanı, parıldayan yıldızlar,
    hilal ve güneş animasyonu, buzlu cam (glassmorphism) bilgi kapsülleri
    ve akıcı geri sayım paneli içerir.
    """
    def __init__(self, parent=None):
        super(GokyuzuWidget, self).__init__(parent)
        self.setGeometry(QtCore.QRect(10, 8, 306, 376))
        self.setObjectName("gokyuzu_widget")
        self.animFaz = 0.0

        self.ulke = "TÜRKİYE"
        self.sehir = "İSTANBUL"
        self.ilce = "ESENYURT"
        self.miladiTarih = ""
        self.hicriTarih = ""
        self.simdikiSaatStr = "00:00:00"
        self.aktifVakit = "imsak"
        self.sonrakiVakitAd = "Güneş"
        self.kalanSureStr = "00:00:00"
        self.tema = "varsayilan"

        self.vakitler = {
            "imsak": "06:00",
            "gunes": "07:25",
            "ogle": "12:54",
            "ikindi": "15:45",
            "aksam": "18:13",
            "yatsi": "19:33",
        }

        # Rastgele sabit yıldız koordinatları (gece vakti için)
        import random
        rnd = random.Random(42)
        self.yildizlar = [(rnd.uniform(10, 296), rnd.uniform(10, 160), rnd.uniform(0.8, 2.2), rnd.uniform(0, 6.28)) for _ in range(40)]

    def veriGuncelle(self, ulke, sehir, ilce, miladi, hicri, diniGun, saatStr, aktifVakit, sonrakiVakitAd, kalanSureStr, vakitler, tema):
        self.ulke = ulke or "TÜRKİYE"
        self.sehir = sehir or ""
        self.ilce = ilce or sehir or ""
        self.miladiTarih = miladi or ""
        self.hicriTarih = hicri or ""
        self.simdikiSaatStr = saatStr or "00:00:00"
        self.aktifVakit = aktifVakit or "imsak"
        self.sonrakiVakitAd = sonrakiVakitAd or ""
        self.kalanSureStr = kalanSureStr or "00:00:00"
        self.vakitler = vakitler or self.vakitler
        self.tema = tema or "varsayilan"
        self.update()

    def animasyonAdimi(self):
        self.animFaz += 0.06
        if self.animFaz > 6.28318:
            self.animFaz -= 6.28318
        self.update()

    def paintEvent(self, event):
        p = QtGui.QPainter(self)
        p.setRenderHint(QtGui.QPainter.Antialiasing)
        p.setRenderHint(QtGui.QPainter.TextAntialiasing)

        w, h = 302.0, 372.0
        bgRect = QtCore.QRectF(2, 2, w, h)

        # 1. Vakte Göre Dinamik Gökyüzü Gradyanı
        skyGrad = QtGui.QLinearGradient(0, 0, 0, h)
        if self.aktifVakit in ("yatsi", "sabah"): # Gece / İmsak öncesi
            skyGrad.setColorAt(0.0, QtGui.QColor(11, 15, 25))
            skyGrad.setColorAt(0.5, QtGui.QColor(23, 37, 84))
            skyGrad.setColorAt(1.0, QtGui.QColor(30, 27, 75))
            isNight = True
        elif self.aktifVakit == "imsak": # Sabah Alacakaranlığı
            skyGrad.setColorAt(0.0, QtGui.QColor(23, 23, 56))
            skyGrad.setColorAt(0.6, QtGui.QColor(124, 45, 18))
            skyGrad.setColorAt(1.0, QtGui.QColor(245, 158, 11))
            isNight = False
        elif self.aktifVakit == "gunes": # Güneş Doğuşu / Kuşluk
            skyGrad.setColorAt(0.0, QtGui.QColor(2, 132, 199))
            skyGrad.setColorAt(0.7, QtGui.QColor(56, 189, 248))
            skyGrad.setColorAt(1.0, QtGui.QColor(254, 240, 138))
            isNight = False
        elif self.aktifVakit == "ogle": # Öğle Vakti
            skyGrad.setColorAt(0.0, QtGui.QColor(3, 105, 161))
            skyGrad.setColorAt(0.6, QtGui.QColor(14, 165, 233))
            skyGrad.setColorAt(1.0, QtGui.QColor(186, 230, 253))
            isNight = False
        elif self.aktifVakit == "ikindi": # İkindi Vakti / Altın Saat
            skyGrad.setColorAt(0.0, QtGui.QColor(30, 58, 138))
            skyGrad.setColorAt(0.5, QtGui.QColor(194, 65, 12))
            skyGrad.setColorAt(1.0, QtGui.QColor(251, 191, 36))
            isNight = False
        else: # Akşam (Günbatımı Alacakaranlığı)
            skyGrad.setColorAt(0.0, QtGui.QColor(30, 27, 75))
            skyGrad.setColorAt(0.5, QtGui.QColor(131, 24, 67))
            skyGrad.setColorAt(1.0, QtGui.QColor(234, 88, 12))
            isNight = True

        p.setPen(QtGui.QPen(QtGui.QColor(255, 255, 255, 30), 1))
        p.setBrush(skyGrad)
        p.drawRoundedRect(bgRect, 10, 10)

        # 2. Gökyüzü Öğeleri (Yıldızlar veya Güneş Halesi)
        if isNight:
            # Yanıp sönen yıldızlar
            for x, y, r, phase in self.yildizlar:
                twinkle = (math.sin(self.animFaz * 2.0 + phase) + 1.0) * 0.5
                alpha = int(90 + 165 * twinkle)
                p.setPen(QtCore.Qt.NoPen)
                p.setBrush(QtGui.QColor(255, 255, 255, alpha))
                p.drawEllipse(QtCore.QPointF(x, y), r, r)

            # Hilal (Crescent Moon)
            mx, my = 245.0, 48.0
            moonPath = QtGui.QPainterPath()
            moonPath.addEllipse(QtCore.QPointF(mx, my), 14.0, 14.0)
            cutout = QtGui.QPainterPath()
            cutout.addEllipse(QtCore.QPointF(mx - 6.0, my - 4.0), 13.0, 13.0)
            crescent = moonPath.subtracted(cutout)
            p.setPen(QtCore.Qt.NoPen)
            p.setBrush(QtGui.QColor(254, 240, 138, 240))
            p.drawPath(crescent)
        else:
            # Parlayan Güneş Halesi
            sx, sy = 245.0, 46.0
            pulse = (math.sin(self.animFaz) + 1.0) * 0.5
            halo_r = 22.0 + 3.0 * pulse
            p.setPen(QtCore.Qt.NoPen)
            p.setBrush(QtGui.QColor(254, 240, 138, 50))
            p.drawEllipse(QtCore.QPointF(sx, sy), halo_r, halo_r)
            p.setBrush(QtGui.QColor(253, 224, 71, 220))
            p.drawEllipse(QtCore.QPointF(sx, sy), 13.0, 13.0)

        # 3. Üst Buzlu Cam Kapsülü (Location & Date)
        topCapsule = QtCore.QRectF(12, 12, 210, 52)
        p.setPen(QtGui.QPen(QtGui.QColor(255, 255, 255, 60), 1))
        p.setBrush(QtGui.QColor(15, 23, 42, 130))
        p.drawRoundedRect(topCapsule, 8, 8)

        p.setPen(QtGui.QColor(226, 232, 240))
        p.setFont(QtGui.QFont("Segoe UI", 8, QtGui.QFont.Bold))
        p.drawText(QtCore.QRectF(22, 18, 190, 15), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, f"{self.ulke} • {self.sehir}")

        p.setPen(QtGui.QColor(255, 255, 255))
        p.setFont(QtGui.QFont("Segoe UI", 12, QtGui.QFont.Bold))
        p.drawText(QtCore.QRectF(22, 34, 190, 22), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, self.ilce)

        # 4. Merkez Hero Kartı (Canlı Saat & Kalan Süre)
        heroRect = QtCore.QRectF(12, 72, 282, 88)
        p.setPen(QtGui.QPen(QtGui.QColor(255, 255, 255, 80), 1.2))
        p.setBrush(QtGui.QColor(15, 23, 42, 160))
        p.drawRoundedRect(heroRect, 10, 10)

        # Canlı Saat ve Aktif Vakit Rozeti
        p.setPen(QtGui.QColor(56, 189, 248))
        p.setFont(QtGui.QFont("Segoe UI", 9, QtGui.QFont.Bold))
        aktifBaslik = TURKCE_BUYUK_VAKIT.get(str(self.aktifVakit).lower(), "İMSAK")
        p.drawText(QtCore.QRectF(24, 82, 150, 18), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, f"✦ {aktifBaslik} VAKTİ ✦")

        p.setPen(QtGui.QColor(255, 255, 255))
        p.setFont(QtGui.QFont("Segoe UI", 18, QtGui.QFont.Bold))
        saatGosterim = self.simdikiSaatStr
        if len(saatGosterim.split(":")) < 3:
            saatGosterim = QtCore.QTime.currentTime().toString("hh:mm:ss")
        p.drawText(QtCore.QRectF(24, 102, 150, 30), QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter, saatGosterim)

        # Sağ Taraf: Geri Sayım
        sonrakiHam = (self.sonrakiVakitAd or "").replace(" :", "")
        for t in ["'e Kalan Süre", "’e Kalan Süre", "'a Kalan Süre", "’a Kalan Süre", " Kalan Süre", "'e Kalan", "’e Kalan", "'a Kalan", "’a Kalan"]:
            sonrakiHam = sonrakiHam.replace(t, "")
        sonrakiEtiket = f"{sonrakiHam.strip()}'e Kalan"
        p.setPen(QtGui.QColor(203, 213, 225))
        p.setFont(QtGui.QFont("Segoe UI", 8))
        p.drawText(QtCore.QRectF(140, 82, 142, 18), QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter, sonrakiEtiket)

        p.setPen(QtGui.QColor(251, 191, 36))
        p.setFont(QtGui.QFont("Segoe UI", 17, QtGui.QFont.Bold))
        p.drawText(QtCore.QRectF(140, 102, 142, 30), QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter, self.kalanSureStr)

        p.setPen(QtGui.QColor(226, 232, 240, 180))
        p.setFont(QtGui.QFont("Segoe UI", 7))
        p.drawText(QtCore.QRectF(24, 136, 258, 16), QtCore.Qt.AlignCenter, f"{self.miladiTarih}  •  {self.hicriTarih}")

        # 5. Alt 6 Vakit Kartları
        kartlar = [
            ("İmsak", self.vakitler.get("imsak", "--:--"), "imsak"),
            ("Güneş", self.vakitler.get("gunes", "--:--"), "gunes"),
            ("Öğle", self.vakitler.get("ogle", "--:--"), "ogle"),
            ("İkindi", self.vakitler.get("ikindi", "--:--"), "ikindi"),
            ("Akşam", self.vakitler.get("aksam", "--:--"), "aksam"),
            ("Yatsı", self.vakitler.get("yatsi", "--:--"), "yatsi"),
        ]

        startX = 12.0
        startY = 176.0
        cardW = 89.0
        cardH = 82.0
        gapX = 7.0
        gapY = 10.0

        for idx, (ad, saat, key) in enumerate(kartlar):
            row = idx // 3
            col = idx % 3
            kx = startX + col * (cardW + gapX)
            ky = startY + row * (cardH + gapY)
            cardRect = QtCore.QRectF(kx, ky, cardW, cardH)

            aktifMi = (key == self.aktifVakit)
            if aktifMi:
                pulse = (math.sin(self.animFaz) + 1.0) * 0.5
                borderAlpha = int(180 + 75 * pulse)
                p.setPen(QtGui.QPen(QtGui.QColor(251, 191, 36, borderAlpha), 2.0))
                p.setBrush(QtGui.QColor(15, 23, 42, 220))
            else:
                p.setPen(QtGui.QPen(QtGui.QColor(255, 255, 255, 45), 1))
                p.setBrush(QtGui.QColor(15, 23, 42, 140))

            p.drawRoundedRect(cardRect, 10, 10)

            # Vakit Adı
            p.setPen(QtGui.QColor(251, 191, 36) if aktifMi else QtGui.QColor(203, 213, 225))
            p.setFont(QtGui.QFont("Segoe UI", 9, QtGui.QFont.Bold if aktifMi else QtGui.QFont.Normal))
            p.drawText(QtCore.QRectF(kx + 4, ky + 12, cardW - 8, 20), QtCore.Qt.AlignCenter, ad)

            # Saat
            p.setPen(QtGui.QColor(255, 255, 255))
            p.setFont(QtGui.QFont("Segoe UI", 14, QtGui.QFont.Bold))
            p.drawText(QtCore.QRectF(kx + 4, ky + 36, cardW - 8, 28), QtCore.Qt.AlignCenter, saat)

            if aktifMi:
                p.setPen(QtCore.Qt.NoPen)
                p.setBrush(QtGui.QColor(251, 191, 36))
                p.drawEllipse(QtCore.QPointF(kx + cardW / 2.0, ky + 70), 3.0, 3.0)
