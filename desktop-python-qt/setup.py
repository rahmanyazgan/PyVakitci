#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from cx_Freeze import setup, Executable

includes = ["atexit","PyQt5"]

includefiles = ["GPLv3_EN.html",
                "GPLv3_TR.html",
                "ontanimli_ayarlar.ini",
                "ses_dosyalari"]

excludes = [
    "tkinter",
    "unittest",
    "pydoc",
    "OpenSSL",
    "cryptography",
    "brotlicffi",
    "html5lib",
    "lxml",
    "cchardet",
    "chardet",
    "simplejson",
    "pyodide",
    "js",
    "win_inet_pton",
]
packages = []
path = []

base = None
if sys.platform == "win32":
    base = "gui"

exe = Executable(
    script = "Program.py",
    base = base,
    target_name = "PyVakitci.exe",
    icon = "resimler/cami.ico",
    )

setup(
    author = "Rahman Yazgan",
    author_email = 'rahmanyazgan@gmail.com',
    name = "PyVakitci",
    version = "1.8",
    description = "Diyanet verilerine göre tüm ülkeler için namaz vakitlerini gösterir. Ezan ve ezan duasını okur.",
    options = {"build_exe": {"includes": includes,
                             "include_files": includefiles,
                             "excludes": excludes,
                             "packages": packages,
                             "path": path,
                             "zip_include_packages": "*", 
                             "zip_exclude_packages": ""
                             }
               },
    executables = [exe]
    )
