# -*- coding: utf-8 -*-

# Hazırlayan : Rahman Yazgan (rahmanyazgan@gmail.com)
# Lisans : GPL v.3

import json, requests
from moduller.SiteyeBaglan import SiteyeBaglan
from moduller.DiyanetUlkeler import DiyanetUlkeler

from PyQt5 import QtCore

class SehirleriAl:
    ulke = None
    ayarDosyasi = None
    
    def __init__(self, ulke):
        self.ulke = ulke
        
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
        
        sehirler = None
        
        if self.siteyeBaglantiVarMi(url):
            try:
                country_id = DiyanetUlkeler.ulkeler.get(self.ulke)
                response = SiteyeBaglan.session.get(
                    url=url,
                    params={"ChangeType": "country", "CountryId": country_id},
                    timeout=10
                )
                veriler = json.loads(response.text)
                
                sehirler = {}
                state_list = veriler.get("StateList") or []
                
                for item in state_list:
                    sehir_adi = item.get("SehirAdi")
                    sehir_id = item.get("SehirID")
                    if sehir_adi and sehir_id:
                        sehirler[sehir_adi] = sehir_id
                    
                if SehirleriAl.ayarDosyasi:
                    self.settings = QtCore.QSettings(SehirleriAl.ayarDosyasi, QtCore.QSettings.IniFormat)
                    self.settings.setValue("Diyanet/sehirler", sehirler)
            except Exception:
                sehirler = None
            
        return sehirler