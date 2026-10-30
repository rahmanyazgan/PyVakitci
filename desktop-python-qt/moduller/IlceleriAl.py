# -*- coding: utf-8 -*-

# Hazırlayan : Rahman Yazgan (rahmanyazgan@gmail.com)
# Lisans : GPL v.3

import json, requests
from PyQt5 import QtCore
from moduller.SiteyeBaglan import SiteyeBaglan
from moduller.DiyanetUlkeler import DiyanetUlkeler

class IlceleriAl:
    ayarDosyasi = None
    
    def __init__(self, sehirlerListesi, ulke, sehir):
        self.sehirler = sehirlerListesi
        self.ulke = ulke
        self.sehir = sehir
        
    def siteyeBaglantiVarMi(self, url):
        siteyeBaglan = SiteyeBaglan(url)
        siteyeBaglan.start()
        siteyeBaglan.join(3)
        
        if siteyeBaglan.baglantiVarmi:
            return True
        else:
            return False

    def basla(self):
        url = DiyanetUlkeler.REQUEST_URL
        
        ilceler = None
        linkler = None
        
        if self.siteyeBaglantiVarMi(url):       
            try:
                response = SiteyeBaglan.session.get(
                    url=url,
                    params={
                        "ChangeType": "state",
                        "CountryId": DiyanetUlkeler.ulkeler.get(self.ulke),
                        "StateId": self.sehirler.get(self.sehir) if self.sehirler else None
                    },
                    timeout=10
                )
                veriler = json.loads(response.text)
                
                ilceler = {}
                linkler = {}
                region_list = veriler.get("StateRegionList") or []
                
                for item in region_list:
                    ilce_adi = item.get("IlceAdi")
                    ilce_id = item.get("IlceID")
                    ilce_url = item.get("IlceUrl")
                    if ilce_adi:
                        ilceler[ilce_adi] = ilce_id
                        linkler[ilce_adi] = ilce_url
                
                if IlceleriAl.ayarDosyasi:
                    self.settings = QtCore.QSettings(IlceleriAl.ayarDosyasi, QtCore.QSettings.IniFormat)
                    self.settings.setValue("Diyanet/ilceler", ilceler)
                    self.settings.setValue("Diyanet/linkler", linkler)
        
            except Exception:
                ilceler = None
                linkler = None
        
        return ilceler, linkler