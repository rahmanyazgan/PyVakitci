#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from cx_Freeze import setup, Executable

includes = ["atexit","PyQt5"]

includefiles = ["GPLv3_EN.html",
                "GPLv3_TR.html",
                "ontanimli_ayarlar.ini",
                "ses_dosyalari"]

excludes = []
packages = []
path = []

base = None
if sys.platform == "win32":
    base = "Win32GUI"

exe = Executable(
    script = "Program.py",
    initScript = None,
    base = base,
    #targetDir = "dist",
    targetName = "PyVakitci.exe",
    #compress = True,
    #copyDependentFiles = True,
    #appendScriptToExe = True,
    #appendScriptToLibrary = True,
    icon = "C:/Users/Rahman/source/apps/PyVakitci/desktop-python-qt/resimler/cami.ico",
    )

setup(
    author = "Rahman Yazgan",
    author_email = 'rahmanyazgan@gmail.com',
    name = "PyVakitci",
    version = "1.7",
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
