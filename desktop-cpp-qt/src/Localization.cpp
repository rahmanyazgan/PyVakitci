#include "Localization.h"
#include <QLocale>

QString Localization::s_currentLanguage = "tr";
QMap<QString, QMap<QString, QString>> Localization::s_translations;

QStringList Localization::supportedLanguages() {
    return {"tr", "en", "de", "zh", "ja", "ru"};
}

QString Localization::languageDisplayName(const QString& lang) {
    if (lang == "tr") return "Türkçe";
    if (lang == "en") return "English";
    if (lang == "de") return "Deutsch";
    if (lang == "zh") return "中文 (Chinese)";
    if (lang == "ja") return "日本語 (Japanese)";
    if (lang == "ru") return "Русский (Russian)";
    return lang;
}

QString Localization::detectSystemLanguage() {
    QLocale sys = QLocale::system();
    QString langCode = sys.name().left(2).toLower();
    if (supportedLanguages().contains(langCode)) {
        return langCode;
    }
    return "tr";
}

void Localization::init() {
    if (!s_translations.isEmpty()) return;

    // ==========================================
    // 1. TÜRKÇE (tr)
    // ==========================================
    QMap<QString, QString>& trMap = s_translations["tr"];
    trMap["imsak"] = "İmsak";
    trMap["gunes"] = "Güneş";
    trMap["ogle"] = "Öğle";
    trMap["ikindi"] = "İkindi";
    trMap["aksam"] = "Akşam";
    trMap["yatsi"] = "Yatsı";
    trMap["vakti"] = "Vakti";
    trMap["kalan"] = "'e Kalan";
    trMap["kalan_sure"] = "'e Kalan Süre";
    trMap["tab_saat"] = "Vakit Saati";
    trMap["tab_hava"] = "Hava Durumu";
    trMap["tab_ayarlar"] = "Ayarlar";
    trMap["tab_ses"] = "Ses Ayarları";
    trMap["tab_linkler"] = "Linkler";
    trMap["genel_ayarlar"] = "Genel Ayarlar";
    trMap["bolgesel_ayarlar"] = "Bölgesel Ayarlar";
    trMap["oto_calistir"] = "Açılışta otomatik çalıştır";
    trMap["sistem_tepsisi"] = "Sistem tepsisinde başlat";
    trMap["oto_guncelle"] = "Otomatik güncelleştir";
    trMap["her_zaman_ustte"] = "Her zaman üstte";
    trMap["besmele_ile_basla"] = "Besmele ile başla";
    trMap["winamp_duraklat"] = "Ezanda medyayı duraklat";
    trMap["erken_uyari"] = "Erken uyarı";
    trMap["erken_uyari_dk"] = "dk önce";
    trMap["hicri_gun_ilave"] = "Hicri güne ilave et";
    trMap["dil"] = "Dil:";
    trMap["ulke"] = "Ülke:";
    trMap["sehir"] = "Şehir:";
    trMap["ilce"] = "İlçe:";
    trMap["ayarlari_kaydet"] = "Ayarları Kaydet";
    trMap["vakitleri_indir"] = "Vakitleri İndir";
    trMap["ses_seviyesi"] = "Ses Seviyesi:";
    trMap["ezan_duasi"] = "Ezan Duası";
    trMap["cuma_selasi"] = "Cuma Selası";
    trMap["uyari_sesi"] = "Uyarı Sesi";
    trMap["hakkinda"] = "Hakkında";
    trMap["lisans"] = "Lisans (GPL v3)";
    trMap["aylik_vakitler"] = "Aylık Namaz Vakitleri";
    trMap["cikis"] = "Çıkış";
    trMap["goster_gizle"] = "Göster / Gizle";
    trMap["bilgilendirme"] = "Bilgilendirme";
    trMap["hata"] = "Hata";
    trMap["kaynak"] = "Kaynak: diyanet.gov.tr";

    // Yeni Anahtarlar (tr)
    trMap["btn_varsayilan"] = "Varsayılan";
    trMap["btn_kaydet"] = "Kaydet";
    trMap["vakitleri_aylik_goster"] = " Vakitleri aylık olarak göster";
    trMap["vakitleri_guncelle"] = "Aylık vakitleri şimdi güncelle";
    trMap["erken_uyari_metin"] = "Vakit girmeden önce uyar (dk) :";
    trMap["erken_sela_metin"] = "Selayı vakit girmeden oku (dk) :";
    trMap["hicri_gun_metin"] = "Hicri tarihe gün ilave et :";
    trMap["sistem_tepsisi_ikon"] = "Sistem tepsisinde ikon göster.";
    trMap["spotify_duraklat"] = "Spotifyı duraklat";
    trMap["menu_dosya"] = "Dosya";
    trMap["menu_gorunum"] = "Görünüm";
    trMap["menu_yardim"] = "Yardım";
    trMap["menu_renk_temasi"] = "Renk Teması";
    trMap["menu_pencere_stili"] = "Pencere Stili";
    trMap["menu_qt_hakkinda"] = "Qt Hakkında";
    trMap["mod_saat"] = "⏱️ Vakit Saati (Kadran & Canlı)";
    trMap["mod_gokyuzu"] = "🌙 Dinamik Gökyüzü & Hilal";
    trMap["mod_kartlar"] = "📋 Modern Kartlar";
    trMap["mod_klasik"] = "🏛️ Klasik Görünüm";
    trMap["stil_fusion"] = "Fusion (Modern)";
    trMap["stil_vista"] = "Windows Vista";
    trMap["stil_windows"] = "Klasik Windows";
    trMap["sabah_etiket"] = "Sabah :";
    trMap["ogle_etiket"] = "Öğle :";
    trMap["ikindi_etiket"] = "İkindi :";
    trMap["aksam_etiket"] = "Akşam :";
    trMap["yatsi_etiket"] = "Yatsı :";
    trMap["dua_etiket"] = "Dua :";
    trMap["sela_etiket"] = "Sela :";
    trMap["uyari_etiket"] = "Uyarı :";
    trMap["btn_ekle"] = "Ekle";
    trMap["btn_oynat"] = "Oynat";
    trMap["btn_durdur"] = "Durdur";
    trMap["ses_filtre"] = "Ses Dosyaları (*.mp3 *.ogg *.wma *.wav)";
    trMap["hava_hissedilen"] = "Hissedilen";
    trMap["hava_nem"] = "Nem Oranı";
    trMap["hava_ruzgar"] = "Rüzgâr";
    trMap["hava_tahmin"] = "5 GÜNLÜK TAHMİN";
    trMap["hava_saat"] = "Saat: %1";
    trMap["bugun"] = "Bugün";
    trMap["en_yuksek"] = "En Yüksek";
    trMap["en_dusuk"] = "En Düşük";
    trMap["hava_yukleniyor"] = "Hava durumu yükleniyor...";
    trMap["hava_hata"] = "Hava durumu bilgisi alınamadı.";
    trMap["simdi_vakti_fmt"] = "Şimdi %1 Vakti";
    trMap["kalan_sure_fmt"] = "%1'e Kalan Süre :";
    trMap["kalan_kisa_fmt"] = "%1'e Kalan";
    trMap["vakitler_icin_fmt"] = "%1 için %2";
    trMap["namaz_vakitleri_tray"] = "Namaz Vakitleri";
    trMap["yararli_siteler"] = "Yararlı Siteler";
    trMap["tarih"] = "Tarih";
    trMap["hakkinda_aciklama"] = "Diyanet İşleri Başkanlığı'nın sunmuş olduğu namaz vakti bilgilerini kullanarak namaz vakitlerini öğrenmenizi ve namaz vakitlerinde otomatik olarak ezan okunmasını sağlayan bir programdır.";
    trMap["oneri_sikayet"] = "Programda gördüğünüz hataları ve programın geliştirilmesine yönelik önerilerinizi aşağıdaki e-posta adresine bildirebilirsiniz.";
    trMap["gun_1"] = "Pazartesi";
    trMap["gun_2"] = "Salı";
    trMap["gun_3"] = "Çarşamba";
    trMap["gun_4"] = "Perşembe";
    trMap["gun_5"] = "Cuma";
    trMap["gun_6"] = "Cumartesi";
    trMap["gun_7"] = "Pazar";
    trMap["tema_varsayilan"] = "Varsayılan";
    trMap["tema_koyu"] = "Koyu Tema";
    trMap["tema_lacivert"] = "Lacivert";
    trMap["tema_yesil"] = "Yeşil";
    trMap["tema_turkuaz"] = "Turkuaz";
    trMap["tema_gri"] = "Gri";
    trMap["tema_mor"] = "Mor";
    trMap["tema_turuncu"] = "Turuncu";
    trMap["vakit_aktif"] = "AKTİF";
    trMap["dakika"] = "Dakika";
    trMap["yenile"] = "Yenile";
    trMap["hava_yenile_tooltip"] = "Hava durumunu şimdi güncelle";
    trMap["hava_ruzgar_birim"] = "km/s";

    // ==========================================
    // 2. İNGİLİZCE (en)
    // ==========================================
    QMap<QString, QString>& enMap = s_translations["en"];
    enMap["imsak"] = "Fajr";
    enMap["gunes"] = "Sunrise";
    enMap["ogle"] = "Dhuhr";
    enMap["ikindi"] = "Asr";
    enMap["aksam"] = "Maghrib";
    enMap["yatsi"] = "Isha";
    enMap["vakti"] = "Time";
    enMap["kalan"] = "to";
    enMap["kalan_sure"] = "Remaining to";
    enMap["tab_saat"] = "Prayer Clock";
    enMap["tab_hava"] = "Weather";
    enMap["tab_ayarlar"] = "Settings";
    enMap["tab_ses"] = "Sound Settings";
    enMap["tab_linkler"] = "Links";
    enMap["genel_ayarlar"] = "General Settings";
    enMap["bolgesel_ayarlar"] = "Regional Settings";
    enMap["oto_calistir"] = "Start on system startup";
    enMap["sistem_tepsisi"] = "Start in system tray";
    enMap["oto_guncelle"] = "Auto update prayer times";
    enMap["her_zaman_ustte"] = "Always on top";
    enMap["besmele_ile_basla"] = "Play Basmala on startup";
    enMap["winamp_duraklat"] = "Pause media during Adhan";
    enMap["erken_uyari"] = "Early warning alert";
    enMap["erken_uyari_dk"] = "mins before";
    enMap["hicri_gun_ilave"] = "Adjust Hijri calendar";
    enMap["dil"] = "Language:";
    enMap["ulke"] = "Country:";
    enMap["sehir"] = "City:";
    enMap["ilce"] = "District:";
    enMap["ayarlari_kaydet"] = "Save Settings";
    enMap["vakitleri_indir"] = "Download Times";
    enMap["ses_seviyesi"] = "Volume Level:";
    enMap["ezan_duasi"] = "Adhan Dua";
    enMap["cuma_selasi"] = "Friday Sala";
    enMap["uyari_sesi"] = "Warning Alert";
    enMap["hakkinda"] = "About";
    enMap["lisans"] = "License (GPL v3)";
    enMap["aylik_vakitler"] = "Monthly Prayer Times";
    enMap["cikis"] = "Exit";
    enMap["goster_gizle"] = "Show / Hide";
    enMap["bilgilendirme"] = "Information";
    enMap["hata"] = "Error";
    enMap["kaynak"] = "Source: diyanet.gov.tr";

    // Yeni Anahtarlar (en)
    enMap["btn_varsayilan"] = "Defaults";
    enMap["btn_kaydet"] = "Save";
    enMap["vakitleri_aylik_goster"] = "Show monthly prayer times";
    enMap["vakitleri_guncelle"] = "Update monthly times now";
    enMap["erken_uyari_metin"] = "Early alert (min) :";
    enMap["erken_sela_metin"] = "Early sela (min) :";
    enMap["hicri_gun_metin"] = "Adjust Hijri day (+/-) :";
    enMap["sistem_tepsisi_ikon"] = "Show icon in system tray";
    enMap["spotify_duraklat"] = "Pause Spotify";
    enMap["menu_dosya"] = "File";
    enMap["menu_gorunum"] = "View";
    enMap["menu_yardim"] = "Help";
    enMap["menu_renk_temasi"] = "Color Theme";
    enMap["menu_pencere_stili"] = "Window Style";
    enMap["menu_qt_hakkinda"] = "About Qt";
    enMap["mod_saat"] = "⏱️ Prayer Clock (Dial & Live)";
    enMap["mod_gokyuzu"] = "🌙 Dynamic Sky & Crescent";
    enMap["mod_kartlar"] = "📋 Modern Cards";
    enMap["mod_klasik"] = "🏛️ Classic View";
    enMap["stil_fusion"] = "Fusion (Modern)";
    enMap["stil_vista"] = "Windows Vista";
    enMap["stil_windows"] = "Classic Windows";
    enMap["sabah_etiket"] = "Fajr :";
    enMap["ogle_etiket"] = "Dhuhr :";
    enMap["ikindi_etiket"] = "Asr :";
    enMap["aksam_etiket"] = "Maghrib :";
    enMap["yatsi_etiket"] = "Isha :";
    enMap["dua_etiket"] = "Dua :";
    enMap["sela_etiket"] = "Sala :";
    enMap["uyari_etiket"] = "Alert :";
    enMap["btn_ekle"] = "Browse";
    enMap["btn_oynat"] = "Play";
    enMap["btn_durdur"] = "Stop";
    enMap["ses_filtre"] = "Audio Files (*.mp3 *.ogg *.wma *.wav)";
    enMap["hava_hissedilen"] = "Feels like";
    enMap["hava_nem"] = "Humidity";
    enMap["hava_ruzgar"] = "Wind";
    enMap["hava_tahmin"] = "5-DAY FORECAST";
    enMap["hava_saat"] = "Time: %1";
    enMap["bugun"] = "Today";
    enMap["en_yuksek"] = "High";
    enMap["en_dusuk"] = "Low";
    enMap["hava_yukleniyor"] = "Loading weather...";
    enMap["hava_hata"] = "Could not load weather data.";
    enMap["simdi_vakti_fmt"] = "Now %1 Time";
    enMap["kalan_sure_fmt"] = "Time until %1 :";
    enMap["kalan_kisa_fmt"] = "to %1";
    enMap["vakitler_icin_fmt"] = "%2 for %1";
    enMap["namaz_vakitleri_tray"] = "Prayer Times";
    enMap["yararli_siteler"] = "Useful Websites";
    enMap["tarih"] = "Date";
    enMap["hakkinda_aciklama"] = "A program that provides prayer times using data from the Presidency of Religious Affairs and plays the Adhan automatically at prayer times.";
    enMap["oneri_sikayet"] = "You can report errors or suggest improvements for the program to the following e-mail address.";
    enMap["gun_1"] = "Monday";
    enMap["gun_2"] = "Tuesday";
    enMap["gun_3"] = "Wednesday";
    enMap["gun_4"] = "Thursday";
    enMap["gun_5"] = "Friday";
    enMap["gun_6"] = "Saturday";
    enMap["gun_7"] = "Sunday";
    enMap["tema_varsayilan"] = "Default";
    enMap["tema_koyu"] = "Dark Theme";
    enMap["tema_lacivert"] = "Navy";
    enMap["tema_yesil"] = "Green";
    enMap["tema_turkuaz"] = "Turquoise";
    enMap["tema_gri"] = "Gray";
    enMap["tema_mor"] = "Purple";
    enMap["tema_turuncu"] = "Orange";
    enMap["vakit_aktif"] = "ACTIVE";
    enMap["dakika"] = "Minutes";
    enMap["yenile"] = "Refresh";
    enMap["hava_yenile_tooltip"] = "Update weather now";
    enMap["hava_ruzgar_birim"] = "km/h";

    // ==========================================
    // 3. ALMANCA (de)
    // ==========================================
    QMap<QString, QString>& deMap = s_translations["de"];
    deMap["imsak"] = "Fadschr";
    deMap["gunes"] = "Sonnenaufgang";
    deMap["ogle"] = "Zuhr";
    deMap["ikindi"] = "Asr";
    deMap["aksam"] = "Maghrib";
    deMap["yatsi"] = "Ischa";
    deMap["vakti"] = "Zeit";
    deMap["kalan"] = "bis";
    deMap["kalan_sure"] = "Verbleibend bis";
    deMap["tab_saat"] = "Gebetsuhr";
    deMap["tab_hava"] = "Wetter";
    deMap["tab_ayarlar"] = "Einstellungen";
    deMap["tab_ses"] = "Toneinstellungen";
    deMap["tab_linkler"] = "Links";
    deMap["genel_ayarlar"] = "Allgemein";
    deMap["bolgesel_ayarlar"] = "Regional";
    deMap["oto_calistir"] = "Beim Systemstart ausführen";
    deMap["sistem_tepsisi"] = "Im Infobereich starten";
    deMap["oto_guncelle"] = "Automatisch aktualisieren";
    deMap["her_zaman_ustte"] = "Immer im Vordergrund";
    deMap["besmele_ile_basla"] = "Mit Basmala starten";
    deMap["winamp_duraklat"] = "Medien beim Gebetsruf anhalten";
    deMap["erken_uyari"] = "Frühwarnung";
    deMap["erken_uyari_dk"] = "Minuten vorher";
    deMap["hicri_gun_ilave"] = "Hidschri-Tage anpassen";
    deMap["dil"] = "Sprache:";
    deMap["ulke"] = "Land:";
    deMap["sehir"] = "Stadt:";
    deMap["ilce"] = "Bezirk:";
    deMap["ayarlari_kaydet"] = "Einstellungen speichern";
    deMap["vakitleri_indir"] = "Zeiten herunterladen";
    deMap["ses_seviyesi"] = "Lautstärke:";
    deMap["ezan_duasi"] = "Adhan-Dua";
    deMap["cuma_selasi"] = "Freitags-Sela";
    deMap["uyari_sesi"] = "Warnton";
    deMap["hakkinda"] = "Über";
    deMap["lisans"] = "Lizenz (GPL v3)";
    deMap["aylik_vakitler"] = "Monatliche Gebetszeiten";
    deMap["cikis"] = "Beenden";
    deMap["goster_gizle"] = "Anzeigen / Verbergen";
    deMap["bilgilendirme"] = "Information";
    deMap["hata"] = "Fehler";
    deMap["kaynak"] = "Quelle: diyanet.gov.tr";

    // Yeni Anahtarlar (de)
    deMap["btn_varsayilan"] = "Standard";
    deMap["btn_kaydet"] = "Speichern";
    deMap["vakitleri_aylik_goster"] = "Monatliche Zeiten anzeigen";
    deMap["vakitleri_guncelle"] = "Monatszeiten jetzt aktualisieren";
    deMap["erken_uyari_metin"] = "Frühwarnung (Min) :";
    deMap["erken_sela_metin"] = "Früh-Sela (Min) :";
    deMap["hicri_gun_metin"] = "Hidschri-Tage (+/-) :";
    deMap["sistem_tepsisi_ikon"] = "Symbol im Infobereich anzeigen";
    deMap["spotify_duraklat"] = "Spotify anhalten";
    deMap["menu_dosya"] = "Datei";
    deMap["menu_gorunum"] = "Ansicht";
    deMap["menu_yardim"] = "Hilfe";
    deMap["menu_renk_temasi"] = "Farbthema";
    deMap["menu_pencere_stili"] = "Fensterstil";
    deMap["menu_qt_hakkinda"] = "Über Qt";
    deMap["mod_saat"] = "⏱️ Gebetsuhr (Zifferblatt & Live)";
    deMap["mod_gokyuzu"] = "🌙 Dynamischer Himmel & Mondsichel";
    deMap["mod_kartlar"] = "📋 Moderne Karten";
    deMap["mod_klasik"] = "🏛️ Klassische Ansicht";
    deMap["stil_fusion"] = "Fusion (Modern)";
    deMap["stil_vista"] = "Windows Vista";
    deMap["stil_windows"] = "Klassisches Windows";
    deMap["sabah_etiket"] = "Fadschr :";
    deMap["ogle_etiket"] = "Zuhr :";
    deMap["ikindi_etiket"] = "Asr :";
    deMap["aksam_etiket"] = "Maghrib :";
    deMap["yatsi_etiket"] = "Ischa :";
    deMap["dua_etiket"] = "Dua :";
    deMap["sela_etiket"] = "Sela :";
    deMap["uyari_etiket"] = "Warnung :";
    deMap["btn_ekle"] = "Wählen";
    deMap["btn_oynat"] = "Abspielen";
    deMap["btn_durdur"] = "Stopp";
    deMap["ses_filtre"] = "Audiodateien (*.mp3 *.ogg *.wma *.wav)";
    deMap["hava_hissedilen"] = "Gefühlt";
    deMap["hava_nem"] = "Feuchtigkeit";
    deMap["hava_ruzgar"] = "Wind";
    deMap["hava_tahmin"] = "5-TAGE-PROGNOSE";
    deMap["hava_saat"] = "Uhr: %1";
    deMap["bugun"] = "Heute";
    deMap["en_yuksek"] = "Max";
    deMap["en_dusuk"] = "Min";
    deMap["hava_yukleniyor"] = "Wetter wird geladen...";
    deMap["hava_hata"] = "Wetterdaten nicht verfügbar.";
    deMap["simdi_vakti_fmt"] = "Jetzt ist %1";
    deMap["kalan_sure_fmt"] = "Zeit bis %1 :";
    deMap["kalan_kisa_fmt"] = "bis %1";
    deMap["vakitler_icin_fmt"] = "%2 für %1";
    deMap["namaz_vakitleri_tray"] = "Gebetszeiten";
    deMap["yararli_siteler"] = "Nützliche Webseiten";
    deMap["tarih"] = "Datum";
    deMap["hakkinda_aciklama"] = "Ein Programm zur Anzeige von Gebetszeiten und automatischen Wiedergabe des Gebetsrufs (Adhan) basierend auf offiziellen Diyanet-Daten.";
    deMap["oneri_sikayet"] = "Fehler und Verbesserungsvorschläge für das Programm können Sie an folgende E-Mail-Adresse senden.";
    deMap["gun_1"] = "Montag";
    deMap["gun_2"] = "Dienstag";
    deMap["gun_3"] = "Mittwoch";
    deMap["gun_4"] = "Donnerstag";
    deMap["gun_5"] = "Freitag";
    deMap["gun_6"] = "Samstag";
    deMap["gun_7"] = "Sonntag";
    deMap["tema_varsayilan"] = "Standard";
    deMap["tema_koyu"] = "Dunkles Design";
    deMap["tema_lacivert"] = "Marineblau";
    deMap["tema_yesil"] = "Grün";
    deMap["tema_turkuaz"] = "Türkis";
    deMap["tema_gri"] = "Grau";
    deMap["tema_mor"] = "Lila";
    deMap["tema_turuncu"] = "Orange";
    deMap["vakit_aktif"] = "AKTIV";
    deMap["dakika"] = "Minuten";
    deMap["yenile"] = "Aktualisieren";
    deMap["hava_yenile_tooltip"] = "Wetter jetzt aktualisieren";
    deMap["hava_ruzgar_birim"] = "km/h";

    // ==========================================
    // 4. ÇİNCE (zh)
    // ==========================================
    QMap<QString, QString>& zhMap = s_translations["zh"];
    zhMap["imsak"] = "晨礼";
    zhMap["gunes"] = "日出";
    zhMap["ogle"] = "晌礼";
    zhMap["ikindi"] = "晡礼";
    zhMap["aksam"] = "昏礼";
    zhMap["yatsi"] = "宵礼";
    zhMap["vakti"] = "时间";
    zhMap["kalan"] = "距离";
    zhMap["kalan_sure"] = "剩余时间";
    zhMap["tab_saat"] = "礼拜时钟";
    zhMap["tab_hava"] = "天气";
    zhMap["tab_ayarlar"] = "设置";
    zhMap["tab_ses"] = "音频设置";
    zhMap["tab_linkler"] = "链接";
    zhMap["genel_ayarlar"] = "常规设置";
    zhMap["bolgesel_ayarlar"] = "地区设置";
    zhMap["oto_calistir"] = "开机自启";
    zhMap["sistem_tepsisi"] = "最小化到托盘启动";
    zhMap["oto_guncelle"] = "自动更新礼拜时间";
    zhMap["her_zaman_ustte"] = "窗口置顶";
    zhMap["besmele_ile_basla"] = "启动时播放开端词";
    zhMap["winamp_duraklat"] = "宣礼时暂停媒体";
    zhMap["erken_uyari"] = "提前提醒";
    zhMap["erken_uyari_dk"] = "分钟前";
    zhMap["hicri_gun_ilave"] = "调整伊斯兰历天数";
    zhMap["dil"] = "语言:";
    zhMap["ulke"] = "国家:";
    zhMap["sehir"] = "省份/城市:";
    zhMap["ilce"] = "区县:";
    zhMap["ayarlari_kaydet"] = "保存设置";
    zhMap["vakitleri_indir"] = "下载时间";
    zhMap["ses_seviyesi"] = "音量大小:";
    zhMap["ezan_duasi"] = "宣礼赞词";
    zhMap["cuma_selasi"] = "主麻宣赞";
    zhMap["uyari_sesi"] = "提醒铃声";
    zhMap["hakkinda"] = "关于";
    zhMap["lisans"] = "开源许可 (GPL v3)";
    zhMap["aylik_vakitler"] = "月度礼拜时刻表";
    zhMap["cikis"] = "退出";
    zhMap["goster_gizle"] = "显示 / 隐藏";
    zhMap["bilgilendirme"] = "信息提示";
    zhMap["hata"] = "错误";
    zhMap["kaynak"] = "数据来源: diyanet.gov.tr";

    // Yeni Anahtarlar (zh)
    zhMap["btn_varsayilan"] = "默认值";
    zhMap["btn_kaydet"] = "保存";
    zhMap["vakitleri_aylik_goster"] = "显示月度礼拜时间";
    zhMap["vakitleri_guncelle"] = "立即更新月度时间";
    zhMap["erken_uyari_metin"] = "提前提醒 (分) :";
    zhMap["erken_sela_metin"] = "提前宣礼 (分) :";
    zhMap["hicri_gun_metin"] = "调整伊斯兰历 (天) :";
    zhMap["sistem_tepsisi_ikon"] = "在系统托盘显示图标";
    zhMap["spotify_duraklat"] = "暂停 Spotify";
    zhMap["menu_dosya"] = "文件";
    zhMap["menu_gorunum"] = "视图";
    zhMap["menu_yardim"] = "帮助";
    zhMap["menu_renk_temasi"] = "颜色主题";
    zhMap["menu_pencere_stili"] = "窗口样式";
    zhMap["menu_qt_hakkinda"] = "关于 Qt";
    zhMap["mod_saat"] = "⏱️ 礼拜时钟 (表盘和实时)";
    zhMap["mod_gokyuzu"] = "🌙 动态天空与新月";
    zhMap["mod_kartlar"] = "📋 现代卡片";
    zhMap["mod_klasik"] = "🏛️ 经典视图";
    zhMap["stil_fusion"] = "Fusion (现代)";
    zhMap["stil_vista"] = "Windows Vista";
    zhMap["stil_windows"] = "经典 Windows";
    zhMap["sabah_etiket"] = "晨礼 :";
    zhMap["ogle_etiket"] = "晌礼 :";
    zhMap["ikindi_etiket"] = "晡礼 :";
    zhMap["aksam_etiket"] = "昏礼 :";
    zhMap["yatsi_etiket"] = "宵礼 :";
    zhMap["dua_etiket"] = "祈祷 :";
    zhMap["sela_etiket"] = "赞圣 :";
    zhMap["uyari_etiket"] = "提醒 :";
    zhMap["btn_ekle"] = "浏览";
    zhMap["btn_oynat"] = "播放";
    zhMap["btn_durdur"] = "停止";
    zhMap["ses_filtre"] = "音频文件 (*.mp3 *.ogg *.wma *.wav)";
    zhMap["hava_hissedilen"] = "体感温度";
    zhMap["hava_nem"] = "湿度";
    zhMap["hava_ruzgar"] = "风速";
    zhMap["hava_tahmin"] = "5 天天气预报";
    zhMap["hava_saat"] = "时间: %1";
    zhMap["bugun"] = "今天";
    zhMap["en_yuksek"] = "最高";
    zhMap["en_dusuk"] = "最低";
    zhMap["hava_yukleniyor"] = "正在加载天气...";
    zhMap["hava_hata"] = "无法获取天气信息。";
    zhMap["simdi_vakti_fmt"] = "现在是%1时间";
    zhMap["kalan_sure_fmt"] = "距离%1剩余时间 :";
    zhMap["kalan_kisa_fmt"] = "距离%1";
    zhMap["vakitler_icin_fmt"] = "%1的%2";
    zhMap["namaz_vakitleri_tray"] = "礼拜时间";
    zhMap["yararli_siteler"] = "实用网站";
    zhMap["tarih"] = "日期";
    zhMap["hakkinda_aciklama"] = "一款使用宗教事务局数据提供礼拜时间并在礼拜时间自动播放宣礼的程序。";
    zhMap["oneri_sikayet"] = "您可以将程序中的错误或改进建议发送到以下电子邮件地址。";
    zhMap["gun_1"] = "星期一";
    zhMap["gun_2"] = "星期二";
    zhMap["gun_3"] = "星期三";
    zhMap["gun_4"] = "星期四";
    zhMap["gun_5"] = "星期五";
    zhMap["gun_6"] = "星期六";
    zhMap["gun_7"] = "星期日";
    zhMap["tema_varsayilan"] = "默认主题";
    zhMap["tema_koyu"] = "深色主题";
    zhMap["tema_lacivert"] = "藏青色";
    zhMap["tema_yesil"] = "绿色";
    zhMap["tema_turkuaz"] = "绿松石色";
    zhMap["tema_gri"] = "灰色";
    zhMap["tema_mor"] = "紫色";
    zhMap["tema_turuncu"] = "橙色";
    zhMap["vakit_aktif"] = "活跃";
    zhMap["dakika"] = "分钟";
    zhMap["yenile"] = "刷新";
    zhMap["hava_yenile_tooltip"] = "立即更新天气";
    zhMap["hava_ruzgar_birim"] = "km/h";

    // ==========================================
    // 5. JAPONCA (ja)
    // ==========================================
    QMap<QString, QString>& jaMap = s_translations["ja"];
    jaMap["imsak"] = "ファジュル";
    jaMap["gunes"] = "日の出";
    jaMap["ogle"] = "ズフル";
    jaMap["ikindi"] = "アスル";
    jaMap["aksam"] = "マグリブ";
    jaMap["yatsi"] = "イシャー";
    jaMap["vakti"] = "時間";
    jaMap["kalan"] = "まで";
    jaMap["kalan_sure"] = "残り時間";
    jaMap["tab_saat"] = "礼拝時計";
    jaMap["tab_hava"] = "天気";
    jaMap["tab_ayarlar"] = "設定";
    jaMap["tab_ses"] = "音声設定";
    jaMap["tab_linkler"] = "リンク";
    jaMap["genel_ayarlar"] = "一般設定";
    jaMap["bolgesel_ayarlar"] = "地域設定";
    jaMap["oto_calistir"] = "システム起動時に実行";
    jaMap["sistem_tepsisi"] = "タスクトレイで開始";
    jaMap["oto_guncelle"] = "礼拝時間を自動更新";
    jaMap["her_zaman_ustte"] = "常に最前面に表示";
    jaMap["besmele_ile_basla"] = "起動時にバスマラを再生";
    jaMap["winamp_duraklat"] = "アザーン時にメディアを一時停止";
    jaMap["erken_uyari"] = "事前通知";
    jaMap["erken_uyari_dk"] = "分前";
    jaMap["hicri_gun_ilave"] = "ヒジュラ暦の日付調整";
    jaMap["dil"] = "言語:";
    jaMap["ulke"] = "国:";
    jaMap["sehir"] = "都市:";
    jaMap["ilce"] = "地区:";
    jaMap["ayarlari_kaydet"] = "設定を保存";
    jaMap["vakitleri_indir"] = "時間をダウンロード";
    jaMap["ses_seviyesi"] = "音量レベル:";
    jaMap["ezan_duasi"] = "アザーンの祈り";
    jaMap["cuma_selasi"] = "金曜セラ";
    jaMap["uyari_sesi"] = "通知アラート";
    jaMap["hakkinda"] = "アプリについて";
    jaMap["lisans"] = "ライセンス (GPL v3)";
    jaMap["aylik_vakitler"] = "月間礼拝時間表";
    jaMap["cikis"] = "終了";
    jaMap["goster_gizle"] = "表示 / 非表示";
    jaMap["bilgilendirme"] = "お知らせ";
    jaMap["hata"] = "エラー";
    jaMap["kaynak"] = "出典: diyanet.gov.tr";

    // Yeni Anahtarlar (ja)
    jaMap["btn_varsayilan"] = "デフォルト";
    jaMap["btn_kaydet"] = "保存";
    jaMap["vakitleri_aylik_goster"] = "月間礼拝時間を表示";
    jaMap["vakitleri_guncelle"] = "月間時間を今すぐ更新";
    jaMap["erken_uyari_metin"] = "事前通知 (分) :";
    jaMap["erken_sela_metin"] = "事前セラ (分) :";
    jaMap["hicri_gun_metin"] = "ヒジュラ暦調整 (日) :";
    jaMap["sistem_tepsisi_ikon"] = "タスクトレイにアイコンを表示";
    jaMap["spotify_duraklat"] = "Spotify を一時停止";
    jaMap["menu_dosya"] = "ファイル";
    jaMap["menu_gorunum"] = "表示";
    jaMap["menu_yardim"] = "ヘルプ";
    jaMap["menu_renk_temasi"] = "カラーテーマ";
    jaMap["menu_pencere_stili"] = "ウィンドウスタイル";
    jaMap["menu_qt_hakkinda"] = "Qtについて";
    jaMap["mod_saat"] = "⏱️ 礼拝時計 (文字盤とライブ)";
    jaMap["mod_gokyuzu"] = "🌙 ダイナミックスカイと三日月";
    jaMap["mod_kartlar"] = "📋 モダンカード";
    jaMap["mod_klasik"] = "🏛️ クラシック表示";
    jaMap["stil_fusion"] = "Fusion (モダン)";
    jaMap["stil_vista"] = "Windows Vista";
    jaMap["stil_windows"] = "クラシック Windows";
    jaMap["sabah_etiket"] = "ファジュル :";
    jaMap["ogle_etiket"] = "ズフル :";
    jaMap["ikindi_etiket"] = "アスル :";
    jaMap["aksam_etiket"] = "マグリブ :";
    jaMap["yatsi_etiket"] = "イシャー :";
    jaMap["dua_etiket"] = "ドゥアー :";
    jaMap["sela_etiket"] = "セラ :";
    jaMap["uyari_etiket"] = "警告 :";
    jaMap["btn_ekle"] = "参照";
    jaMap["btn_oynat"] = "再生";
    jaMap["btn_durdur"] = "停止";
    jaMap["ses_filtre"] = "音声ファイル (*.mp3 *.ogg *.wma *.wav)";
    jaMap["hava_hissedilen"] = "体感温度";
    jaMap["hava_nem"] = "湿度";
    jaMap["hava_ruzgar"] = "風速";
    jaMap["hava_tahmin"] = "5日間天気予報";
    jaMap["hava_saat"] = "時刻: %1";
    jaMap["bugun"] = "今日";
    jaMap["en_yuksek"] = "最高";
    jaMap["en_dusuk"] = "最低";
    jaMap["hava_yukleniyor"] = "天気を読み込み中...";
    jaMap["hava_hata"] = "天気情報を取得できませんでした。";
    jaMap["simdi_vakti_fmt"] = "現在: %1";
    jaMap["kalan_sure_fmt"] = "%1までの残り時間 :";
    jaMap["kalan_kisa_fmt"] = "%1まで";
    jaMap["vakitler_icin_fmt"] = "%1の%2";
    jaMap["namaz_vakitleri_tray"] = "礼拝時間";
    jaMap["yararli_siteler"] = "役立つサイト";
    jaMap["tarih"] = "日付";
    jaMap["hakkinda_aciklama"] = "宗教庁のデータを使用して礼拝時間を提供し、時間になると自動的にアザーンを再生するプログラムです。";
    jaMap["oneri_sikayet"] = "プログラムの不具合や改善提案は、以下のメールアドレスまでお知らせください。";
    jaMap["gun_1"] = "月曜日";
    jaMap["gun_2"] = "火曜日";
    jaMap["gun_3"] = "水曜日";
    jaMap["gun_4"] = "木曜日";
    jaMap["gun_5"] = "金曜日";
    jaMap["gun_6"] = "土曜日";
    jaMap["gun_7"] = "日曜日";
    jaMap["tema_varsayilan"] = "デフォルト";
    jaMap["tema_koyu"] = "ダークテーマ";
    jaMap["tema_lacivert"] = "ネイビー";
    jaMap["tema_yesil"] = "グリーン";
    jaMap["tema_turkuaz"] = "ターコイズ";
    jaMap["tema_gri"] = "グレー";
    jaMap["tema_mor"] = "パープル";
    jaMap["tema_turuncu"] = "オレンジ";
    jaMap["vakit_aktif"] = "アクティブ";
    jaMap["dakika"] = "分";
    jaMap["yenile"] = "更新";
    jaMap["hava_yenile_tooltip"] = "天気を今すぐ更新";
    jaMap["hava_ruzgar_birim"] = "km/h";

    // ==========================================
    // 6. RUSÇA (ru)
    // ==========================================
    QMap<QString, QString>& ruMap = s_translations["ru"];
    ruMap["imsak"] = "Фаджр";
    ruMap["gunes"] = "Восход";
    ruMap["ogle"] = "Зухр";
    ruMap["ikindi"] = "Аср";
    ruMap["aksam"] = "Магриб";
    ruMap["yatsi"] = "Иша";
    ruMap["vakti"] = "Время";
    ruMap["kalan"] = "до";
    ruMap["kalan_sure"] = "Осталось до";
    ruMap["tab_saat"] = "Часы намаза";
    ruMap["tab_hava"] = "Погода";
    ruMap["tab_ayarlar"] = "Настройки";
    ruMap["tab_ses"] = "Настройки звука";
    ruMap["tab_linkler"] = "Ссылки";
    ruMap["genel_ayarlar"] = "Общие настройки";
    ruMap["bolgesel_ayarlar"] = "Региональные";
    ruMap["oto_calistir"] = "Запуск при старте системы";
    ruMap["sistem_tepsisi"] = "Запуск в системном трее";
    ruMap["oto_guncelle"] = "Автообновление времени";
    ruMap["her_zaman_ustte"] = "Поверх всех окон";
    ruMap["besmele_ile_basla"] = "Воспроизводить Бисмиллях";
    ruMap["winamp_duraklat"] = "Пауза медиа во время азана";
    ruMap["erken_uyari"] = "Раннее предупреждение";
    ruMap["erken_uyari_dk"] = "мин. до";
    ruMap["hicri_gun_ilave"] = "Смещение календаря хиджры";
    ruMap["dil"] = "Язык:";
    ruMap["ulke"] = "Страна:";
    ruMap["sehir"] = "Город:";
    ruMap["ilce"] = "Район:";
    ruMap["ayarlari_kaydet"] = "Сохранить настройки";
    ruMap["vakitleri_indir"] = "Скачать расписание";
    ruMap["ses_seviyesi"] = "Громкость:";
    ruMap["ezan_duasi"] = "Дуа после азана";
    ruMap["cuma_selasi"] = "Пятничный селя";
    ruMap["uyari_sesi"] = "Звук оповещения";
    ruMap["hakkinda"] = "О программе";
    ruMap["lisans"] = "Лицензия (GPL v3)";
    ruMap["aylik_vakitler"] = "Месячное расписание намаза";
    ruMap["cikis"] = "Выход";
    ruMap["goster_gizle"] = "Показать / Скрыть";
    ruMap["bilgilendirme"] = "Информация";
    ruMap["hata"] = "Ошибка";
    ruMap["kaynak"] = "Источник: diyanet.gov.tr";

    // Yeni Anahtarlar (ru)
    ruMap["btn_varsayilan"] = "По умолчанию";
    ruMap["btn_kaydet"] = "Сохранить";
    ruMap["vakitleri_aylik_goster"] = "Показать время на месяц";
    ruMap["vakitleri_guncelle"] = "Обновить время на месяц";
    ruMap["erken_uyari_metin"] = "Раннее увед. (мин) :";
    ruMap["erken_sela_metin"] = "Ранний селя (мин) :";
    ruMap["hicri_gun_metin"] = "Смещение хиджры (дней) :";
    ruMap["sistem_tepsisi_ikon"] = "Показывать значок в трее";
    ruMap["spotify_duraklat"] = "Приостановить Spotify";
    ruMap["menu_dosya"] = "Файл";
    ruMap["menu_gorunum"] = "Вид";
    ruMap["menu_yardim"] = "Справка";
    ruMap["menu_renk_temasi"] = "Цветовая тема";
    ruMap["menu_pencere_stili"] = "Стиль окна";
    ruMap["menu_qt_hakkinda"] = "О Qt";
    ruMap["mod_saat"] = "⏱️ Часы намаза (циферблат)";
    ruMap["mod_gokyuzu"] = "🌙 Динамическое небо и полумесяц";
    ruMap["mod_kartlar"] = "📋 Современные карточки";
    ruMap["mod_klasik"] = "🏛️ Классический вид";
    ruMap["stil_fusion"] = "Fusion (Современный)";
    ruMap["stil_vista"] = "Windows Vista";
    ruMap["stil_windows"] = "Классический Windows";
    ruMap["sabah_etiket"] = "Фаджр :";
    ruMap["ogle_etiket"] = "Зухр :";
    ruMap["ikindi_etiket"] = "Аср :";
    ruMap["aksam_etiket"] = "Магриб :";
    ruMap["yatsi_etiket"] = "Иша :";
    ruMap["dua_etiket"] = "Дуа :";
    ruMap["sela_etiket"] = "Селя :";
    ruMap["uyari_etiket"] = "Сигнал :";
    ruMap["btn_ekle"] = "Обзор";
    ruMap["btn_oynat"] = "Воспроизвести";
    ruMap["btn_durdur"] = "Остановить";
    ruMap["ses_filtre"] = "Аудиофайлы (*.mp3 *.ogg *.wma *.wav)";
    ruMap["hava_hissedilen"] = "Ощущается";
    ruMap["hava_nem"] = "Влажность";
    ruMap["hava_ruzgar"] = "Ветер";
    ruMap["hava_tahmin"] = "ПРОГНОЗ НА 5 ДНЕЙ";
    ruMap["hava_saat"] = "Время: %1";
    ruMap["bugun"] = "Сегодня";
    ruMap["en_yuksek"] = "Макс";
    ruMap["en_dusuk"] = "Мин";
    ruMap["hava_yukleniyor"] = "Загрузка погоды...";
    ruMap["hava_hata"] = "Не удалось получить погоду.";
    ruMap["simdi_vakti_fmt"] = "Сейчас время %1";
    ruMap["kalan_sure_fmt"] = "До %1 осталось :";
    ruMap["kalan_kisa_fmt"] = "До %1";
    ruMap["vakitler_icin_fmt"] = "%2 для %1";
    ruMap["namaz_vakitleri_tray"] = "Время намаза";
    ruMap["yararli_siteler"] = "Полезные сайты";
    ruMap["tarih"] = "Дата";
    ruMap["hakkinda_aciklama"] = "Программа для отображения времени намаза по данным Управления по делам религий с автоматическим воспроизведением азана.";
    ruMap["oneri_sikayet"] = "Вы можете сообщить об ошибках или предложениях по улучшению программы на следующий адрес электронной почты.";
    ruMap["gun_1"] = "Понедельник";
    ruMap["gun_2"] = "Вторник";
    ruMap["gun_3"] = "Среда";
    ruMap["gun_4"] = "Четверг";
    ruMap["gun_5"] = "Пятница";
    ruMap["gun_6"] = "Суббота";
    ruMap["gun_7"] = "Воскресенье";
    ruMap["tema_varsayilan"] = "По умолчанию";
    ruMap["tema_koyu"] = "Тёмная тема";
    ruMap["tema_lacivert"] = "Тёмно-синий";
    ruMap["tema_yesil"] = "Зелёный";
    ruMap["tema_turkuaz"] = "Бирюзовый";
    ruMap["tema_gri"] = "Серый";
    ruMap["tema_mor"] = "Фиолетовый";
    ruMap["tema_turuncu"] = "Оранжевый";
    ruMap["vakit_aktif"] = "АКТИВНО";
    ruMap["dakika"] = "минут";
    ruMap["yenile"] = "Обновить";
    ruMap["hava_yenile_tooltip"] = "Обновить погоду сейчас";
    ruMap["hava_ruzgar_birim"] = "км/ч";
}

QString Localization::currentLanguage() {
    return s_currentLanguage;
}

void Localization::setLanguage(const QString& lang) {
    init();
    if (supportedLanguages().contains(lang)) {
        s_currentLanguage = lang;
    } else {
        s_currentLanguage = "tr";
    }
}

QString Localization::tr(const QString& key) {
    init();
    if (s_translations.contains(s_currentLanguage) && s_translations[s_currentLanguage].contains(key)) {
        return s_translations[s_currentLanguage][key];
    }
    if (s_translations.contains("tr") && s_translations["tr"].contains(key)) {
        return s_translations["tr"][key];
    }
    return key;
}

QString Localization::getLicensePath(const QString& baseDir) {
    // SADECE Turkce icin GPLv3_TR.html, Turkce disindaki TUM dillerde GPLv3_EN.html
    if (s_currentLanguage == "tr") {
        return baseDir + "/GPLv3_TR.html";
    }
    return baseDir + "/GPLv3_EN.html";
}

QString Localization::formatCurrentPrayer(const QString& prayerKey) {
    QString pName = tr(prayerKey);
    return tr("simdi_vakti_fmt").arg(pName);
}

QString Localization::formatRemainingTime(const QString& prayerKey) {
    QString pName = tr(prayerKey);
    return tr("kalan_sure_fmt").arg(pName);
}

QString Localization::formatRemainingTo(const QString& prayerKey) {
    QString pName = tr(prayerKey);
    return tr("kalan_kisa_fmt").arg(pName);
}

QString Localization::dayName(int dayOfWeek) {
    int d = ((dayOfWeek - 1) % 7) + 1;
    return tr("gun_" + QString::number(d));
}

QString Localization::weatherDescription(int code) {
    QString lang = s_currentLanguage;
    if (lang == "en") {
        switch (code) {
            case 0: return "Clear sky";
            case 1: return "Mainly clear";
            case 2: return "Partly cloudy";
            case 3: return "Overcast";
            case 45: return "Foggy";
            case 48: return "Depositing rime fog";
            case 51: return "Light drizzle";
            case 53: return "Moderate drizzle";
            case 55: return "Dense drizzle";
            case 56:
            case 57: return "Freezing drizzle";
            case 61: return "Slight rain";
            case 63: return "Moderate rain";
            case 65: return "Heavy rain";
            case 66:
            case 67: return "Freezing rain";
            case 71: return "Slight snow";
            case 73: return "Moderate snow";
            case 75: return "Heavy snow";
            case 77: return "Snow grains";
            case 80: return "Slight rain showers";
            case 81: return "Moderate rain showers";
            case 82: return "Violent rain showers";
            case 85: return "Slight snow showers";
            case 86: return "Heavy snow showers";
            case 95: return "Thunderstorm";
            case 96: return "Thunderstorm with hail";
            case 99: return "Heavy thunderstorm with hail";
            default: return "Clear sky";
        }
    } else if (lang == "de") {
        switch (code) {
            case 0: return "Klarer Himmel";
            case 1: return "Überwiegend klar";
            case 2: return "Teilweise bewölkt";
            case 3: return "Bedeckt";
            case 45: return "Nebel";
            case 48: return "Raureifnebel";
            case 51: return "Leichter Nieselregen";
            case 53: return "Mäßiger Nieselregen";
            case 55: return "Dichter Nieselregen";
            case 56:
            case 57: return "Gefrierender Nieselregen";
            case 61: return "Leichter Regen";
            case 63: return "Mäßiger Regen";
            case 65: return "Starker Regen";
            case 66:
            case 67: return "Gefrierender Regen";
            case 71: return "Leichter Schneefall";
            case 73: return "Mäßiger Schneefall";
            case 75: return "Starker Schneefall";
            case 77: return "Griesel";
            case 80: return "Leichter Schauer";
            case 81: return "Mäßiger Schauer";
            case 82: return "Heftiger Schauer";
            case 85: return "Leichter Schneeschauer";
            case 86: return "Starker Schneeschauer";
            case 95: return "Gewitter";
            case 96: return "Gewitter mit Hagel";
            case 99: return "Schweres Hagelgewitter";
            default: return "Klarer Himmel";
        }
    } else if (lang == "zh") {
        switch (code) {
            case 0: return "晴朗";
            case 1: return "大部晴朗";
            case 2: return "局部多云";
            case 3: return "阴天";
            case 45: return "有雾";
            case 48: return "白霜雾";
            case 51: return "微毛毛雨";
            case 53: return "毛毛雨";
            case 55: return "稠密毛毛雨";
            case 56:
            case 57: return "冻毛毛雨";
            case 61: return "小雨";
            case 63: return "中雨";
            case 65: return "大雨";
            case 66:
            case 67: return "冻雨";
            case 71: return "小雪";
            case 73: return "中雪";
            case 75: return "大雪";
            case 77: return "雪粒";
            case 80: return "微阵雨";
            case 81: return "阵雨";
            case 82: return "暴阵雨";
            case 85: return "小阵雪";
            case 86: return "大阵雪";
            case 95: return "雷阵雨";
            case 96: return "雷阵雨伴有冰雹";
            case 99: return "强雷阵雨伴有强冰雹";
            default: return "晴朗";
        }
    } else if (lang == "ja") {
        switch (code) {
            case 0: return "快晴";
            case 1: return "ほぼ快晴";
            case 2: return "晴れ時々曇り";
            case 3: return "曇り";
            case 45: return "霧";
            case 48: return "着氷性の霧";
            case 51: return "弱い霧雨";
            case 53: return "霧雨";
            case 55: return "強い霧雨";
            case 56:
            case 57: return "着氷性の霧雨";
            case 61: return "弱い雨";
            case 63: return "雨";
            case 65: return "強い雨";
            case 66:
            case 67: return "着氷性の雨";
            case 71: return "弱い雪";
            case 73: return "雪";
            case 75: return "強い雪";
            case 77: return "細氷";
            case 80: return "にわか雨";
            case 81: return "強いにわか雨";
            case 82: return "激しいにわか雨";
            case 85: return "にわか雪";
            case 86: return "激しいにわか雪";
            case 95: return "雷雨";
            case 96: return "雹を伴う雷雨";
            case 99: return "激しい雹を伴う雷雨";
            default: return "快晴";
        }
    } else if (lang == "ru") {
        switch (code) {
            case 0: return "Ясно";
            case 1: return "Преимущественно ясно";
            case 2: return "Переменная облачность";
            case 3: return "Пасмурно";
            case 45: return "Туман";
            case 48: return "Изморозь";
            case 51: return "Легкая морось";
            case 53: return "Умеренная морось";
            case 55: return "Густая морось";
            case 56:
            case 57: return "Замерзающая морось";
            case 61: return "Небольшой дождь";
            case 63: return "Дождь";
            case 65: return "Сильный дождь";
            case 66:
            case 67: return "Ледяной дождь";
            case 71: return "Небольшой снег";
            case 73: return "Снегопад";
            case 75: return "Сильный снегопад";
            case 77: return "Снежные зерна";
            case 80: return "Кратковременный дождь";
            case 81: return "Ливень";
            case 82: return "Сильный ливень";
            case 85: return "Снежный заряд";
            case 86: return "Сильный снежный заряд";
            case 95: return "Гроза";
            case 96: return "Гроза с градом";
            case 99: return "Сильная гроза с градом";
            default: return "Ясно";
        }
    }

    // Varsayılan Türkçe
    switch (code) {
        case 0: return "Açık";
        case 1: return "Az Bulutlu";
        case 2: return "Parçalı Bulutlu";
        case 3: return "Çok Bulutlu";
        case 45: return "Sisli";
        case 48: return "Kırağılı Sis";
        case 51: return "Hafif Çisenti";
        case 53: return "Orta Çisenti";
        case 55: return "Yoğun Çisenti";
        case 56:
        case 57: return "Dondurucu Çisenti";
        case 61: return "Hafif Yağmurlu";
        case 63: return "Yağmurlu";
        case 65: return "Kuvvetli Yağmur";
        case 66:
        case 67: return "Dondurucu Yağmur";
        case 71: return "Hafif Kar";
        case 73: return "Kar Yağışlı";
        case 75: return "Yoğun Kar";
        case 77: return "Kar Taneleri";
        case 80: return "Hafif Sağanak";
        case 81: return "Orta Sağanak";
        case 82: return "Şiddetli Sağanak";
        case 85: return "Hafif Kar Sağanağı";
        case 86: return "Yoğun Kar Sağanağı";
        case 95: return "Gök Gürültülü Fırtına";
        case 96: return "Dolu Fırtınası";
        case 99: return "Şiddetli Dolu Fırtınası";
        default: return "Açık";
    }
}
