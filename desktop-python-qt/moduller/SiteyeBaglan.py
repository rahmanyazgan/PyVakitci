# -*- coding: utf-8 -*-

# Hazırlayan : Rahman Yazgan (rahmanyazgan@gmail.com)
# Lisans : GPL v.3

import threading
import time
import requests
from requests.adapters import HTTPAdapter
from urllib3.util.retry import Retry

class SiteyeBaglan(threading.Thread):
    baglantiVarmi = False
    _sonBasariliKontrol = {}

    session = requests.Session()
    session.headers.update({
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36",
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,application/json,*/*;q=0.8",
        "Accept-Language": "tr-TR,tr;q=0.9,en-US;q=0.8,en;q=0.7",
    })
    _retry = Retry(total=3, backoff_factor=0.3, status_forcelist=[500, 502, 503, 504])
    _adapter = HTTPAdapter(max_retries=_retry, pool_connections=4, pool_maxsize=8)
    session.mount("https://", _adapter)
    session.mount("http://", _adapter)

    def __init__(self, url):
        threading.Thread.__init__(self)
        self.url = url

    def run(self):
        simdi = time.time()
        sonZaman = SiteyeBaglan._sonBasariliKontrol.get(self.url, 0)
        if simdi - sonZaman < 10:
            SiteyeBaglan.baglantiVarmi = True
            return

        try:
            resp = SiteyeBaglan.session.get(self.url, timeout=5, stream=True)
            resp.close()
            if resp.status_code < 500:
                SiteyeBaglan.baglantiVarmi = True
                SiteyeBaglan._sonBasariliKontrol[self.url] = simdi
            else:
                SiteyeBaglan.baglantiVarmi = False
        except Exception:
            print(self.url + " sitesine bağlantı yok.")
            SiteyeBaglan.baglantiVarmi = False