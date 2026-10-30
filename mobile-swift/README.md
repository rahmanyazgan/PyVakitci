# PyVakitci iOS (Swift / SwiftUI)

Diyanet İşleri Başkanlığı namaz vakitlerini, ezan seslerini, 24 saatlik döner kadranı, hava durumunu ve kıble pusulasını sunan modern iOS uygulaması.

## Özellikler
* **24 Saatlik Renkli Kadran:** Güncel vakit yayları, ayraç noktaları ve canlı ibre.
* **4 Arayüz Modu & 5 Renk Teması:** Kadranlı Saat, Dinamik Gökyüzü ve Modern Kartlar modları; Açık, Koyu, Gece Mavisi, Zümrüt Yeşili ve Sıcak Kehribar temaları.
* **Kıble Pusulası:** `CoreLocation` manyetik pusula sensörleri ve Büyük Çember kerte hattı ile Kâbe yönü takibi.
* **Hava Durumu:** Open-Meteo ile anlık ve 5 günlük tahmin.
* **Ezan & Uyarı Sesleri:** Dahili makam ezan sesleri (`AVFoundation`) ve `UserNotifications` bildirimleri.
* **Çevrimdışı Çalışma:** Çekilen vakitlerin yerel önbelleklenmesi.

## Nasıl Çalıştırılır?
* **Xcode:** `PyVakitci.xcodeproj` dosyasını Xcode ile açıp doğrudan simülatörde veya gerçek cihazda çalıştırabilirsiniz.
* **Swift Package Manager:** `Package.swift` üzerinden SPM entegrasyonu desteklenmektedir.

