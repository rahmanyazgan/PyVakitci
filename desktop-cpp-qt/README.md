# PyVakitci - C++ Qt Sürümü (v1.7)

PyVakitci uygulamasının Python (PyQt5) sürümündeki tüm özellikleri, algoritmaları, görsel efektleri ve canlı hava durumu sistemi birebir C++ ve modern Qt (Qt 5 / Qt 6 uyumlu) mimarisine taşınmıştır.

## Özellikler

1. **⏱️ Vakit Saati (Kadran & Canlı):** 24 saatlik dairesel kadran, nefes alan aktif vakit ışık yayları, saniye hassasiyetinde canlı ibre, merkez HUD ve mini vakit kartları.
2. **🌙 Dinamik Gökyüzü & Hilal:** Namaz vaktine göre değişen gerçek gökyüzü renk gradyanları, parıldayan yıldızlar ve hilal efekti.
3. **📋 Modern Kartlar & 🏛️ Klasik Görünüm:** Alternatif arayüz tasarımları.
4. **🌤️ Canlı Hava Durumu (2. Sekme):** Open-Meteo REST API, 81 il dahili koordinat haritası, anlık sıcaklık, hissedilen, nem, rüzgâr ve 5 günlük detaylı tahmin tablosu.
5. **⚙️ 2 Sekmeli Ayarlar (3. Sekme):** `Genel Ayarlar` ve `Bölgesel Ayarlar` alt butonları, 11pt Arial yazı tipi ve başlıksız grup kutuları.
6. **🎵 Ses Yönetimi:** Ezan, sela, dua ve erken uyarı sesleri, ses seviyesi ayarı ve oynat/durdur önizleme butonları.
7. **🎨 5 Renk Teması:** Varsayılan (Açık), Modern Koyu, Zümrüt Yeşili, Derin Gece Mavisi ve Sıcak Kehribar.

---

## Derleme ve Çalıştırma

### Yöntem 1: Otomatik Tek Tıkla Derleme (Bat Dosyası)
`derle.bat` (veya `build.bat`) dosyasını çift tıklayarak ya da terminalden çalıştırabilirsiniz:
```cmd
derle.bat
```
*(Eğer Qt farklı bir dizinde kuruluysa yolu parametre olarak da verebilirsiniz: `derle.bat C:\Qt\6.5.3\msvc2019_64`)*

**Bu komut dosyası:**
1. Visual Studio MSVC derleyici ortamını otomatik yükler.
2. `build/` klasörünü açıp CMake ile Release modunda derler.
3. Çıktıyı `dist/` klasöründe toplar ve `windeployqt` ile gereksiz çeviri/opengl fazlalıklarını atarak **en az dosya boyutunda** hazırlar.
4. Windows `iexpress` aracılığıyla tüm bağımlılıkları tek bir çalıştırılabilir dosyaya paketleyerek **`dist/PyVakitci_TekDosya.exe`** üretir.

### Yöntem 2: Qt Creator ile
1. Qt Creator'ı açın.
2. `desktop-cpp-qt/PyVakitci.pro` veya `desktop-cpp-qt/CMakeLists.txt` dosyasını açın.
3. Kurulu Qt Kitinizi seçin ve **Ctrl + R** ile çalıştırın.

