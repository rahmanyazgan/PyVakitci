@echo off
setlocal
chcp 65001 >nul

echo ========================================================
echo        PyVakitci C++ Qt Derleme ve Paketleme Araci
echo ========================================================
echo.

set "PROJE_DIZINI=%~dp0"
cd /d "%PROJE_DIZINI%"

:: 1. MSVC Derleyici Ortamini Kontrol Et
where cl >nul 2>&1
if errorlevel 1 (
    echo [*] MSVC derleyici ortami hazirlaniyor...
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
    ) else (
        echo [UYARI] Visual Studio vcvarsall.bat bulunamadi, mevcut PATH ile devam ediliyor.
    )
)

:: 2. Qt Yolunu Belirle (Parametre, CMAKE_PREFIX_PATH veya C:\Qt taramasi)
set "QT_CMAKE_ARG="
if not "%~1"=="" (
    set "CMAKE_PREFIX_PATH=%~1"
    set "QT_CMAKE_ARG=-DCMAKE_PREFIX_PATH=%~1"
    echo [*] Belirtilen Qt konumu: %~1
    goto :qt_bulundu
)

if not "%CMAKE_PREFIX_PATH%"=="" (
    set "QT_CMAKE_ARG=-DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%"
    echo [*] Ortam degiskeni Qt konumu: %CMAKE_PREFIX_PATH%
    goto :qt_bulundu
)

if not "%QTDIR%"=="" (
    set "CMAKE_PREFIX_PATH=%QTDIR%"
    set "QT_CMAKE_ARG=-DCMAKE_PREFIX_PATH=%QTDIR%"
    echo [*] QTDIR konumu: %QTDIR%
    goto :qt_bulundu
)

:: C:\Qt altindaki kurulu Qt surumlerini tara
if exist "C:\Qt\5.15.2\msvc2019_64\lib\cmake" (
    set "CMAKE_PREFIX_PATH=C:\Qt\5.15.2\msvc2019_64"
    set "QT_CMAKE_ARG=-DCMAKE_PREFIX_PATH=C:\Qt\5.15.2\msvc2019_64"
    goto :qt_bulundu
)

for /d %%D in ("C:\Qt\6.*.*" "C:\Qt\5.*.*") do (
    for /d %%M in ("%%D\msvc2019_64" "%%D\msvc2022_64" "%%D\msvc*") do (
        if exist "%%M\lib\cmake" (
            set "CMAKE_PREFIX_PATH=%%M"
            set "QT_CMAKE_ARG=-DCMAKE_PREFIX_PATH=%%M"
            goto :qt_bulundu
        )
    )
)

:qt_bulundu
if not "%CMAKE_PREFIX_PATH%"=="" (
    echo [+] Kullanilacak Qt SDK: %CMAKE_PREFIX_PATH%
    goto :derlemeye_basla
)

:: Qt bulunamadiysa indirme secenegi sun
echo.
echo [HATA] Sistemde C++ Qt SDK (Qt5Config.cmake veya Qt6Config.cmake) bulunamadi.
echo        Bilgisayarinizda Qt Creator mevcut fakat C++ derleme kutuphaneleri henuz yuklu degil.
echo.
echo [*] Sisteminizdeki aqtinstall araci ile resmi Qt SDK otomatik kurulabilir.
echo     Qt 5.15.2 (MSVC 2019 x64) simdi C:\Qt altina kurulsun mu? [E/H]
set /p KURULUM_ONAY=Seciminiz (E/H): 

if /i "%KURULUM_ONAY%"=="E" (
    echo [*] Qt 5.15.2 indiriliyor, lutfen bekleyin...
    python -m aqt install-qt windows desktop 5.15.2 win64_msvc2019_64 -O C:\Qt
    if exist "C:\Qt\5.15.2\msvc2019_64\lib\cmake" (
        set "CMAKE_PREFIX_PATH=C:\Qt\5.15.2\msvc2019_64"
        set "QT_CMAKE_ARG=-DCMAKE_PREFIX_PATH=C:\Qt\5.15.2\msvc2019_64"
        echo [+] Qt SDK basariyla kuruldu.
        goto :derlemeye_basla
    ) else (
        echo [HATA] Qt kurulumu tamamlanamadi.
        exit /b 1
    )
) else (
    echo.
    echo [*] Farkli bir dizinde Qt kuruluysa yolu parametre olarak verebilirsiniz:
    echo     derle.bat C:\Qt\5.15.2\msvc2019_64
    exit /b 1
)

:derlemeye_basla
:: 3. Build Klasorunu Olustur ve Gir
echo.
echo [*] Derleme dizini hazirlaniyor...
if not exist build (
    mkdir build
)
cd build

:: 4. CMake Yapilandirma
echo [*] CMake yapilandiriliyor (Release)...
cmake .. -DCMAKE_BUILD_TYPE=Release %QT_CMAKE_ARG%
if errorlevel 1 (
    echo [HATA] CMake yapilandirmasi basarisiz oldu.
    cd /d "%PROJE_DIZINI%"
    exit /b 1
)

:: 5. CMake Derleme
echo.
echo [*] Proje derleniyor (Release)...
cmake --build . --config Release
if errorlevel 1 (
    echo [HATA] Derleme basarisiz oldu.
    cd /d "%PROJE_DIZINI%"
    exit /b 1
)

:: 6. Minimal Dagitim ve Dosya Sayisini Azaltma (windeployqt ve Paketleme)
echo.
echo [*] Minimal paketleme hazirlaniyor...
cd /d "%PROJE_DIZINI%"

set "EXE_BULUNDU="
if exist "build\Release\PyVakitci.exe" (
    set "EXE_BULUNDU=build\Release\PyVakitci.exe"
) else if exist "build\PyVakitci.exe" (
    set "EXE_BULUNDU=build\PyVakitci.exe"
)

if "%EXE_BULUNDU%"=="" (
    echo [HATA] Derlenen PyVakitci.exe bulunamadi.
    exit /b 1
)

set "DIST_DIR=%PROJE_DIZINI%dist"
if exist "%DIST_DIR%" rd /s /q "%DIST_DIR%"
mkdir "%DIST_DIR%"

copy /y "%EXE_BULUNDU%" "%DIST_DIR%\" >nul
copy /y "%PROJE_DIZINI%ontanimli_ayarlar.ini" "%DIST_DIR%\" >nul
if exist "%PROJE_DIZINI%clever.exe" copy /y "%PROJE_DIZINI%clever.exe" "%DIST_DIR%\" >nul
if exist "%PROJE_DIZINI%ses_dosyalari" xcopy /e /i /y "%PROJE_DIZINI%ses_dosyalari" "%DIST_DIR%\ses_dosyalari" >nul

:: windeployqt ile sadece minimum gereksinimleri cek
set "WINDEPLOYQT="
where windeployqt >nul 2>&1
if not errorlevel 1 (
    set "WINDEPLOYQT=windeployqt"
) else if exist "%CMAKE_PREFIX_PATH%\bin\windeployqt.exe" (
    set "WINDEPLOYQT=%CMAKE_PREFIX_PATH%\bin\windeployqt.exe"
) else if exist "C:\Users\Rahman\AppData\Local\Programs\Python\Python311\Lib\site-packages\qt6_applications\Qt\bin\windeployqt.exe" (
    set "WINDEPLOYQT=C:\Users\Rahman\AppData\Local\Programs\Python\Python311\Lib\site-packages\qt6_applications\Qt\bin\windeployqt.exe"
)

if not "%WINDEPLOYQT%"=="" (
    echo [*] windeployqt ile en az dosyali bagimliliklar aliniyor...
    "%WINDEPLOYQT%" --release --no-translations --no-system-d3d-compiler --no-opengl-sw --no-compiler-runtime --dir "%DIST_DIR%" "%DIST_DIR%\PyVakitci.exe" >nul 2>&1

    if exist "%DIST_DIR%\translations" rd /s /q "%DIST_DIR%\translations"
    if exist "%DIST_DIR%\generic" rd /s /q "%DIST_DIR%\generic"
)

:: 7. Tek Dosya Standalone EXE Olusturma (Portable Tek EXE)
echo [*] Tek dosyali (Standalone Tek EXE) paketleniyor...
set "PAYLOAD_ZIP=%PROJE_DIZINI%build\payload.zip"
if exist "%PAYLOAD_ZIP%" del /f /q "%PAYLOAD_ZIP%" >nul 2>&1
if exist "%DIST_DIR%\PyVakitci_TekDosya.exe" del /f /q "%DIST_DIR%\PyVakitci_TekDosya.exe" >nul 2>&1

:: dist klasorunu sikistir
where tar >nul 2>&1
if not errorlevel 1 (
    tar -a -c -f "%PAYLOAD_ZIP%" -C "%DIST_DIR%" . >nul 2>&1
) else (
    powershell -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%DIST_DIR%\*' -DestinationPath '%PAYLOAD_ZIP%' -Force" >nul 2>&1
)

:: CSC ile tek basina calisabilen EXE derle (cami.ico ikonu ile)
set "CSC_EXE=C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe"
if exist "%CSC_EXE%" if exist "%PAYLOAD_ZIP%" (
    "%CSC_EXE%" /nologo /target:winexe /optimize+ /r:System.IO.Compression.dll /r:System.IO.Compression.FileSystem.dll /r:System.Windows.Forms.dll /win32icon:"%PROJE_DIZINI%resources\cami.ico" /resource:"%PAYLOAD_ZIP%",payload.zip /out:"%DIST_DIR%\PyVakitci_TekDosya.exe" "%PROJE_DIZINI%src\Launcher.cs"
    if exist "%DIST_DIR%\PyVakitci_TekDosya.exe" (
        echo [+] Tek Dosyali Portable EXE: %DIST_DIR%\PyVakitci_TekDosya.exe (cami.ico ikonu uygulandi)
    ) else (
        echo [UYARI] PyVakitci_TekDosya.exe olusturulamadi!
    )
)

echo.
echo ========================================================
echo [+] Derleme tamamlandi.
echo [+] Cikti Klasoru: %DIST_DIR%
echo ========================================================
