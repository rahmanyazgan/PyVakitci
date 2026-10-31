import Foundation
import SwiftUI

public class LocalizationManager: ObservableObject {
    public static let shared = LocalizationManager()

    public static let supportedLanguages: [(code: String, name: String)] = [
        ("tr", "Türkçe"),
        ("en", "English"),
        ("de", "Deutsch"),
        ("zh", "中文"),
        ("ja", "日本語"),
        ("ru", "Русский")
    ]

    private let keyLanguage = "pyvakitci_language"

    @Published public var currentLanguage: String {
        didSet {
            UserDefaults.standard.set(currentLanguage, forKey: keyLanguage)
        }
    }

    private init() {
        if let saved = UserDefaults.standard.string(forKey: "pyvakitci_language") {
            self.currentLanguage = saved
        } else {
            let sys = Locale.current.languageCode?.lowercased() ?? "tr"
            let valid = LocalizationManager.supportedLanguages.map { $0.code }
            self.currentLanguage = valid.contains(sys) ? sys : "tr"
        }
    }

    public func setLanguage(_ lang: String) {
        let valid = LocalizationManager.supportedLanguages.map { $0.code }
        let target = valid.contains(lang) ? lang : "tr"
        if self.currentLanguage != target {
            self.currentLanguage = target
        }
    }

    /// Lisans dosyası: Türkçe -> GPLv3_TR.html, diğer tüm diller -> GPLv3_EN.html
    public func getLicenseFileName() -> String {
        return currentLanguage == "tr" ? "GPLv3_TR.html" : "GPLv3_EN.html"
    }

    public func tr(_ key: String) -> String {
        if let row = translations[key] {
            return row[currentLanguage] ?? row["tr"] ?? key
        }
        return key
    }

    public func f(_ key: String, _ args: CVarArg...) -> String {
        let format = tr(key)
        return String(format: format, locale: currentLocale, arguments: args)
    }

    public var currentLocale: Locale {
        switch currentLanguage {
        case "tr": return Locale(identifier: "tr_TR")
        case "en": return Locale(identifier: "en_US")
        case "de": return Locale(identifier: "de_DE")
        case "zh": return Locale(identifier: "zh_CN")
        case "ja": return Locale(identifier: "ja_JP")
        case "ru": return Locale(identifier: "ru_RU")
        default: return Locale(identifier: "tr_TR")
        }
    }

    public func prayerFromTr(_ trName: String) -> String {
        let clean = trName.trimmingCharacters(in: .whitespacesAndNewlines).lowercased()
        switch clean {
        case "imsak", "sabah": return tr("p_imsak")
        case "güneş", "gunes": return tr("p_gunes")
        case "öğle", "ogle": return tr("p_ogle")
        case "ikindi": return tr("p_ikindi")
        case "akşam", "aksam": return tr("p_aksam")
        case "yatsı", "yatsi": return tr("p_yatsi")
        case "dua": return tr("p_dua")
        case "sela", "cuma": return tr("p_sela")
        default: return trName
        }
    }

    public func weatherDescription(code: Int) -> String {
        switch code {
        case 0: return tr("wc_0")
        case 1: return tr("wc_1")
        case 2: return tr("wc_2")
        case 3: return tr("wc_3")
        case 45, 48: return tr("wc_fog")
        case 51, 53, 55: return tr("wc_drizzle")
        case 61, 63: return tr("wc_rain")
        case 65: return tr("wc_heavy_rain")
        case 71, 73, 75: return tr("wc_snow")
        case 80, 81, 82: return tr("wc_showers")
        case 95, 96, 99: return tr("wc_storm")
        default: return tr("wc_clear")
        }
    }

    private let translations: [String: [String: String]] = [
        // Tab Bar
        "nav_vakit": ["tr": "Vakit", "en": "Times", "de": "Zeiten", "zh": "时间", "ja": "時刻", "ru": "Время"],
        "nav_hava": ["tr": "Hava", "en": "Weather", "de": "Wetter", "zh": "天气", "ja": "天気", "ru": "Погода"],
        "nav_kible": ["tr": "Kıble", "en": "Qibla", "de": "Qibla", "zh": "朝向", "ja": "キブラ", "ru": "Кибла"],
        "nav_sesler": ["tr": "Sesler", "en": "Sounds", "de": "Töne", "zh": "声音", "ja": "サウンド", "ru": "Звуки"],
        "nav_ayarlar": ["tr": "Ayarlar", "en": "Settings", "de": "Einstellungen", "zh": "设置", "ja": "設定", "ru": "Настройки"],

        // Vakit İsimleri
        "p_imsak": ["tr": "İmsak", "en": "Fajr", "de": "Fadschr", "zh": "晨礼", "ja": "ファジュル", "ru": "Фаджр"],
        "p_gunes": ["tr": "Güneş", "en": "Sunrise", "de": "Sonnenaufgang", "zh": "日出", "ja": "日の出", "ru": "Восход"],
        "p_ogle": ["tr": "Öğle", "en": "Dhuhr", "de": "Dhuhr", "zh": "晌礼", "ja": "ズフル", "ru": "Зухр"],
        "p_ikindi": ["tr": "İkindi", "en": "Asr", "de": "Asr", "zh": "晡礼", "ja": "アスル", "ru": "Аср"],
        "p_aksam": ["tr": "Akşam", "en": "Maghrib", "de": "Maghrib", "zh": "昏礼", "ja": "マグリブ", "ru": "Магриб"],
        "p_yatsi": ["tr": "Yatsı", "en": "Isha", "de": "Ischa", "zh": "宵礼", "ja": "イシャー", "ru": "Иша"],
        "p_dua": ["tr": "Ezan Duası", "en": "Adhan Supplication", "de": "Adhan-Bittgebet", "zh": "宣礼后祈祷", "ja": "アザーン後のドゥアー", "ru": "Дуа после азана"],
        "p_sela": ["tr": "Cuma Selası", "en": "Friday Sala", "de": "Freitags-Sala", "zh": "主麻赞礼", "ja": "金曜日のサラー", "ru": "Пятничная сала"],

        // Kalan Süre
        "rem_imsak": ["tr": "İmsak'a Kalan", "en": "Until Fajr", "de": "Bis Fadschr", "zh": "距晨礼还有", "ja": "ファジュルまで", "ru": "До Фаджра"],
        "rem_gunes": ["tr": "Güneş'e Kalan", "en": "Until Sunrise", "de": "Bis Sonnenaufgang", "zh": "距日出还有", "ja": "日の出まで", "ru": "До восхода"],
        "rem_ogle": ["tr": "Öğle'ye Kalan", "en": "Until Dhuhr", "de": "Bis Dhuhr", "zh": "距晌礼还有", "ja": "ズフルまで", "ru": "До Зухра"],
        "rem_ikindi": ["tr": "İkindi'ye Kalan", "en": "Until Asr", "de": "Bis Asr", "zh": "距晡礼还有", "ja": "アスルまで", "ru": "До Асра"],
        "rem_aksam": ["tr": "Akşam'a Kalan", "en": "Until Maghrib", "de": "Bis Maghrib", "zh": "距昏礼还有", "ja": "マグリブまで", "ru": "До Магриба"],
        "rem_yatsi": ["tr": "Yatsı'ya Kalan", "en": "Until Isha", "de": "Bis Ischa", "zh": "距宵礼还有", "ja": "イシャーまで", "ru": "До Иши"],

        // Ana Ekran
        "h_time_of": ["tr": "%@ VAKTİ", "en": "%@ TIME", "de": "%@-ZEIT", "zh": "%@时间", "ja": "%@の時間", "ru": "ВРЕМЯ: %@"],
        "h_source": ["tr": "Kaynak: diyanet.gov.tr", "en": "Source: diyanet.gov.tr", "de": "Quelle: diyanet.gov.tr", "zh": "来源：diyanet.gov.tr", "ja": "出典: diyanet.gov.tr", "ru": "Источник: diyanet.gov.tr"],

        // Hava Durumu
        "w_feels": ["tr": "Hissedilen", "en": "Feels like", "de": "Gefühlt", "zh": "体感", "ja": "体感", "ru": "Ощущается"],
        "w_humidity": ["tr": "Nem", "en": "Humidity", "de": "Luftfeuchte", "zh": "湿度", "ja": "湿度", "ru": "Влажность"],
        "w_wind": ["tr": "Rüzgar", "en": "Wind", "de": "Wind", "zh": "风速", "ja": "風速", "ru": "Ветер"],
        "w_wind_unit": ["tr": "km/s", "en": "km/h", "de": "km/h", "zh": "公里/时", "ja": "km/h", "ru": "км/ч"],
        "w_forecast": ["tr": "5 Günlük Hava Tahmini", "en": "5-Day Forecast", "de": "5-Tage-Vorhersage", "zh": "5天天气预报", "ja": "5日間の天気予報", "ru": "Прогноз на 5 дней"],
        "wc_0": ["tr": "Açık ve Güneşli", "en": "Clear and Sunny", "de": "Klar und sonnig", "zh": "晴朗", "ja": "快晴", "ru": "Ясно и солнечно"],
        "wc_1": ["tr": "Çoğunlukla Açık", "en": "Mostly Clear", "de": "Überwiegend klar", "zh": "大部晴朗", "ja": "おおむね晴れ", "ru": "Преимущественно ясно"],
        "wc_2": ["tr": "Parçalı Bulutlu", "en": "Partly Cloudy", "de": "Teilweise bewölkt", "zh": "局部多云", "ja": "晴れ時々曇り", "ru": "Переменная облачность"],
        "wc_3": ["tr": "Bulutlu", "en": "Cloudy", "de": "Bewölkt", "zh": "多云", "ja": "曇り", "ru": "Облачно"],
        "wc_fog": ["tr": "Sisli", "en": "Foggy", "de": "Neblig", "zh": "有雾", "ja": "霧", "ru": "Туман"],
        "wc_drizzle": ["tr": "Çiseleyen Yağmur", "en": "Drizzle", "de": "Nieselregen", "zh": "毛毛雨", "ja": "霧雨", "ru": "Морось"],
        "wc_rain": ["tr": "Yağmurlu", "en": "Rainy", "de": "Regnerisch", "zh": "有雨", "ja": "雨", "ru": "Дождь"],
        "wc_heavy_rain": ["tr": "Kuvvetli Yağmur", "en": "Heavy Rain", "de": "Starkregen", "zh": "大雨", "ja": "大雨", "ru": "Сильный дождь"],
        "wc_snow": ["tr": "Kar Yağışlı", "en": "Snowy", "de": "Schneefall", "zh": "有雪", "ja": "雪", "ru": "Снег"],
        "wc_showers": ["tr": "Sağanak Yağış", "en": "Showers", "de": "Regenschauer", "zh": "阵雨", "ja": "にわか雨", "ru": "Ливень"],
        "wc_storm": ["tr": "Gök Gürültülü Fırtına", "en": "Thunderstorm", "de": "Gewitter", "zh": "雷暴", "ja": "雷雨", "ru": "Гроза"],
        "wc_clear": ["tr": "Açık", "en": "Clear", "de": "Klar", "zh": "晴", "ja": "晴れ", "ru": "Ясно"],

        // Kıble
        "q_title": ["tr": "Kıble Pusulası", "en": "Qibla Compass", "de": "Qibla-Kompass", "zh": "基卜拉罗盘", "ja": "キブラコンパス", "ru": "Компас Киблы"],
        "q_coord": ["tr": "Koordinat: %.4f° K, %.4f° D", "en": "Coordinates: %.4f° N, %.4f° E", "de": "Koordinaten: %.4f° N, %.4f° O", "zh": "坐标：北纬 %.4f°，东经 %.4f°", "ja": "座標: 北緯 %.4f°, 東経 %.4f°", "ru": "Координаты: %.4f° с. ш., %.4f° в. д."],
        "q_banner": ["tr": "✦ KIBLEYE YÖNELDİNİZ ✦", "en": "✦ YOU ARE FACING THE QIBLA ✦", "de": "✦ SIE SIND ZUR QIBLA AUSGERICHTET ✦", "zh": "✦ 您已面向基卜拉 ✦", "ja": "✦ キブラの方向を向いています ✦", "ru": "✦ ВЫ ОБРАЩЕНЫ К КИБЛЕ ✦"],
        "q_on_dir": ["tr": "Kıble Yönündesiniz", "en": "You Are Facing the Qibla", "de": "Sie sind zur Qibla ausgerichtet", "zh": "您正面向基卜拉", "ja": "キブラの方向です", "ru": "Вы смотрите на Киблу"],
        "q_angle": ["tr": "Kıble Açısı: %d°", "en": "Qibla Angle: %d°", "de": "Qibla-Winkel: %d°", "zh": "基卜拉角度：%d°", "ja": "キブラ角度: %d°", "ru": "Угол Киблы: %d°"],
        "q_desc_aligned": ["tr": "Cihaz şu an tam Kâbe istikametindedir.", "en": "Your device is pointing exactly toward the Kaaba.", "de": "Ihr Gerät zeigt jetzt genau in Richtung Kaaba.", "zh": "设备现在正对克尔白方向。", "ja": "端末は現在カアバの方向を正確に向いています。", "ru": "Устройство направлено точно на Каабу."],
        "q_desc_turn": ["tr": "Cihazı yatay tutup altın ibre tepeye gelene kadar dönünüz.", "en": "Hold your device flat and turn until the golden needle points up.", "de": "Halten Sie das Gerät waagerecht und drehen Sie es, bis die goldene Nadel nach oben zeigt.", "zh": "请将设备水平放置并转动，直到金色指针指向上方。", "ja": "端末を水平に持ち、金色の針が上を向くまで回してください。", "ru": "Держите устройство горизонтально и поворачивайте, пока золотая стрелка не укажет вверх."],
        "q_heading": ["tr": "Pusula Açısı: %d°", "en": "Compass Heading: %d°", "de": "Kompasskurs: %d°", "zh": "罗盘方位：%d°", "ja": "コンパス方位: %d°", "ru": "Направление компаса: %d°"],
        "q_accuracy": ["tr": "Doğruluk: ±%d°", "en": "Accuracy: ±%d°", "de": "Genauigkeit: ±%d°", "zh": "精度：±%d°", "ja": "精度: ±%d°", "ru": "Точность: ±%d°"],
        "q_need_calibration": ["tr": "Pusula kalibrasyonu gerekli. Telefonunuzu 8 şeklinde hareket ettiriniz.", "en": "Compass calibration required. Move your phone in a figure 8.", "de": "Kompasskalibrierung erforderlich. Bewegen Sie das Telefon in einer 8.", "zh": "需要校准罗盘。请画8字移动手机。", "ja": "コンパスの調整が必要です。端末を8の字に動かしてください。", "ru": "Требуется калибровка компаса. Опишите телефоном восьмёрку."],
        "q_live_gps": ["tr": "Canlı GPS", "en": "Live GPS", "de": "Live-GPS", "zh": "实时GPS", "ja": "ライブGPS", "ru": "GPS онлайн"],
        "q_saved_loc": ["tr": "Kayıtlı Konum", "en": "Saved Location", "de": "Gespeicherter Ort", "zh": "已保存位置", "ja": "保存済みの位置", "ru": "Сохранённое место"],
        "q_verify_gps": ["tr": "Konumumu GPS ile Doğrula", "en": "Verify My Location via GPS", "de": "Standort per GPS bestätigen", "zh": "通过GPS验证我的位置", "ja": "GPSで現在地を確認", "ru": "Уточнить местоположение по GPS"],

        // Bildirimler
        "n_warn_title": ["tr": "Vakit Yaklaşıyor", "en": "Prayer Time Approaching", "de": "Gebetszeit naht", "zh": "礼拜时间即将到来", "ja": "礼拝時刻が近づいています", "ru": "Приближается время намаза"],
        "n_warn_text": ["tr": "%@ vaktine %d dakika kaldı.", "en": "%2$d minutes left until %1$@.", "de": "Noch %2$d Minuten bis %1$@.", "zh": "距%1$@还有 %2$d 分钟。", "ja": "%1$@まであと%2$d分です。", "ru": "До времени «%1$@» осталось %2$d мин."],
        "n_prayer_title": ["tr": "%@ Vakti Girdi", "en": "It's %@ time", "de": "Es ist Zeit für %@", "zh": "%@时间到了", "ja": "%@の時間になりました", "ru": "Наступило время: %@"],
        "n_prayer_text": ["tr": "%@ | Saat: %@", "en": "%@ | Time: %@", "de": "%@ | Uhrzeit: %@", "zh": "%@ | 时间：%@", "ja": "%@ | 時刻: %@", "ru": "%@ | Время: %@"],

        // Ses Ayarları
        "a_title": ["tr": "Ezan ve Vakit Sesleri", "en": "Adhan & Prayer Sounds", "de": "Adhan- und Gebetstöne", "zh": "宣礼与礼拜声音", "ja": "アザーンと礼拝の音", "ru": "Азан и звуки намаза"],
        "a_volume": ["tr": "Ses Seviyesi", "en": "Volume", "de": "Lautstärke", "zh": "音量", "ja": "音量", "ru": "Громкость"],
        "a_vibration": ["tr": "Vakitlerde Titreşim", "en": "Vibrate at Prayer Times", "de": "Vibration zu Gebetszeiten", "zh": "礼拜时振动", "ja": "礼拝時刻に振動", "ru": "Вибрация во время намаза"],
        "a_ezan_duasi": ["tr": "Ezandan Sonra Ezan Duası Çal", "en": "Play Adhan Supplication After", "de": "Nach Adhan Bittgebet abspielen", "zh": "宣礼后播放祈祷词", "ja": "アザーン後にドゥアーを再生", "ru": "Читать дуа после азана"],
        "a_sound_mode": ["tr": "Ses Modu", "en": "Sound Mode", "de": "Tonmodus", "zh": "声音模式", "ja": "サウンドモード", "ru": "Режим звука"],
        "mode_ezan": ["tr": "Ezan", "en": "Adhan", "de": "Adhan", "zh": "宣礼", "ja": "アザーン", "ru": "Азан"],
        "mode_bip": ["tr": "Bip Sesi", "en": "Beep", "de": "Signalton", "zh": "提示音", "ja": "ビープ音", "ru": "Сигнал"],
        "mode_sessiz": ["tr": "Sessiz", "en": "Silent", "de": "Stumm", "zh": "静音", "ja": "消音", "ru": "Без звука"],
        "a_warn_title": ["tr": "Vakit Öncesi Uyarı Sesi", "en": "Pre-Prayer Warning Sound", "de": "Vorwarnton vor dem Gebet", "zh": "礼拜前提醒音", "ja": "礼拝前の警告音", "ru": "Предупреждающий звук"],
        "a_warn_desc": ["tr": "Vakitten %d dk önce", "en": "%d min before prayer", "de": "%d Min. vor dem Gebet", "zh": "礼拜前 %d 分钟", "ja": "礼拝の %d 分前", "ru": "За %d мин до намаза"],

        // Ayarlar
        "s_title": ["tr": "Bölgesel ve Genel Ayarlar", "en": "Regional & General Settings", "de": "Regionale und allgemeine Einstellungen", "zh": "地区与常规设置", "ja": "地域と一般設定", "ru": "Региональные и общие настройки"],
        "s_location": ["tr": "Konum Seçimi", "en": "Location", "de": "Standortauswahl", "zh": "位置选择", "ja": "位置の選択", "ru": "Выбор местоположения"],
        "s_loading": ["tr": "Yükleniyor...", "en": "Loading...", "de": "Wird geladen...", "zh": "加载中...", "ja": "読み込み中...", "ru": "Загрузка..."],
        "s_country": ["tr": "Ülke", "en": "Country", "de": "Land", "zh": "国家", "ja": "国", "ru": "Страна"],
        "s_city": ["tr": "Şehir", "en": "City", "de": "Stadt", "zh": "城市", "ja": "都市", "ru": "Город"],
        "s_district": ["tr": "İlçe", "en": "District", "de": "Bezirk", "zh": "区县", "ja": "地区", "ru": "Район"],
        "s_save_loc": ["tr": "Konumu Kaydet ve Vakitleri Güncelle", "en": "Save Location & Update Times", "de": "Standort speichern und Zeiten aktualisieren", "zh": "保存位置并更新时间", "ja": "位置を保存して時刻を更新", "ru": "Сохранить и обновить время"],
        "s_saved": ["tr": "Konum başarıyla kaydedildi!", "en": "Location saved successfully!", "de": "Standort erfolgreich gespeichert!", "zh": "位置保存成功！", "ja": "位置を保存しました！", "ru": "Местоположение сохранено!"],
        "s_ui_mode": ["tr": "Arayüz Modu", "en": "Interface Mode", "de": "Oberflächenmodus", "zh": "界面模式", "ja": "表示モード", "ru": "Режим интерфейса"],
        "mode_saat": ["tr": "Kadranlı Vakit Saati (Varsayılan)", "en": "Dial Prayer Clock (Default)", "de": "Zifferblatt-Gebetsuhr (Standard)", "zh": "表盘礼拜时钟（默认）", "ja": "ダイヤル式礼拝時計（デフォルト）", "ru": "Циферблат намаза (по умолчанию)"],
        "mode_gokyuzu": ["tr": "Dinamik Gökyüzü Modu", "en": "Dynamic Sky Mode", "de": "Dynamischer Himmelsmodus", "zh": "动态天空模式", "ja": "ダイナミックスカイモード", "ru": "Динамическое небо"],
        "mode_kartlar": ["tr": "Modern Kartlar Modu", "en": "Modern Cards Mode", "de": "Moderner Kartenmodus", "zh": "现代卡片模式", "ja": "モダンカードモード", "ru": "Современные карточки"],
        "s_theme": ["tr": "Renk Teması", "en": "Color Theme", "de": "Farbthema", "zh": "颜色主题", "ja": "カラーテーマ", "ru": "Цветовая тема"],
        "th_varsayilan": ["tr": "Açık Tema (Varsayılan)", "en": "Light Theme (Default)", "de": "Helles Thema (Standard)", "zh": "浅色主题（默认）", "ja": "ライトテーマ（デフォルト）", "ru": "Светлая тема (по умолчанию)"],
        "th_koyu": ["tr": "Modern Koyu Tema", "en": "Modern Dark Theme", "de": "Modernes dunkles Thema", "zh": "现代深色主题", "ja": "モダンダークテーマ", "ru": "Современная тёмная тема"],
        "th_gece_mavisi": ["tr": "Gece Mavisi", "en": "Midnight Blue", "de": "Mitternachtsblau", "zh": "午夜蓝", "ja": "ミッドナイトブルー", "ru": "Полночный синий"],
        "th_zumrut": ["tr": "Zümrüt Yeşili", "en": "Emerald Green", "de": "Smaragdgrün", "zh": "翡翠绿", "ja": "エメラルドグリーン", "ru": "Изумрудный"],
        "th_kehribar": ["tr": "Sıcak Kehribar", "en": "Warm Amber", "de": "Warmes Bernstein", "zh": "暖琥珀色", "ja": "ウォームアンバー", "ru": "Тёплый янтарь"],
        "s_language": ["tr": "Dil / Language", "en": "Language", "de": "Sprache", "zh": "语言", "ja": "言語", "ru": "Язык"],
        "s_license": ["tr": "Lisans (GPL v3)", "en": "License (GPL v3)", "de": "Lizenz (GPL v3)", "zh": "许可证 (GPL v3)", "ja": "ライセンス (GPL v3)", "ru": "Лицензия (GPL v3)"],
        "s_close": ["tr": "Kapat", "en": "Close", "de": "Schließen", "zh": "关闭", "ja": "閉じる", "ru": "Закрыть"],
        "s_footer": ["tr": "PyVakitci iOS v1.8 • Diyanet İşleri Başkanlığı", "en": "PyVakitci iOS v1.8 • Presidency of Religious Affairs (Diyanet)", "de": "PyVakitci iOS v1.8 • Diyanet", "zh": "PyVakitci iOS v1.8 • 土耳其宗教事务局 (Diyanet)", "ja": "PyVakitci iOS v1.8 • トルコ宗務庁 (Diyanet)", "ru": "PyVakitci iOS v1.8 • Diyanet"]
    ]
}
