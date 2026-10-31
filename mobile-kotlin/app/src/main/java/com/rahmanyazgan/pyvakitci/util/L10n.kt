package com.rahmanyazgan.pyvakitci.util

import android.content.Context
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import java.util.Locale

/**
 * PyVakitci Mobil çoklu dil (i18n) yöneticisi.
 *
 * Desteklenen diller: tr (varsayılan), en, de, zh, ja, ru.
 * [lang] bir Compose state'idir; değiştiğinde onu okuyan tüm composable'lar
 * otomatik olarak yeniden çizilir (uygulamayı yeniden başlatmaya gerek yoktur).
 *
 * Lisans kuralı: Türkçe -> GPLv3_TR.html, diğer tüm diller -> GPLv3_EN.html
 */
object L10n {

    /** Dil kodu -> yerel dil adı (sıralama çeviri tablosundaki sütun sırasıyla aynıdır). */
    val SUPPORTED: LinkedHashMap<String, String> = linkedMapOf(
        "tr" to "Türkçe",
        "en" to "English",
        "de" to "Deutsch",
        "zh" to "中文",
        "ja" to "日本語",
        "ru" to "Русский"
    )

    private val CODES = SUPPORTED.keys.toList()

    private const val PREFS_NAME = "pyvakitci_mobile_prefs"
    const val KEY_LANGUAGE = "language"

    var lang: String by mutableStateOf("tr")
        private set

    @Volatile
    private var initialized = false

    /** Kayıtlı dili yükler; kayıt yoksa sistem dilini algılar. */
    fun init(context: Context) {
        if (initialized) return
        val sp = context.applicationContext.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
        lang = normalize(sp.getString(KEY_LANGUAGE, null) ?: detectSystemLanguage())
        initialized = true
    }

    /** Dili değiştirir ve kalıcı olarak kaydeder. */
    fun setLanguage(context: Context, code: String) {
        val c = normalize(code)
        context.applicationContext.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
            .edit().putString(KEY_LANGUAGE, c).apply()
        lang = c
        initialized = true
    }

    fun detectSystemLanguage(): String {
        val sys = Locale.getDefault().language.lowercase(Locale.ROOT)
        return if (sys in CODES) sys else "tr"
    }

    private fun normalize(code: String): String {
        val c = code.lowercase(Locale.ROOT).substringBefore('-').substringBefore('_')
        return if (c in CODES) c else "tr"
    }

    fun locale(): Locale = when (lang) {
        "tr" -> Locale("tr", "TR")
        "en" -> Locale.US
        "de" -> Locale.GERMANY
        "zh" -> Locale.SIMPLIFIED_CHINESE
        "ja" -> Locale.JAPAN
        "ru" -> Locale("ru", "RU")
        else -> Locale("tr", "TR")
    }

    /** Lisans dosyası: Türkçe için GPLv3_TR.html, diğer tüm dillerde GPLv3_EN.html */
    fun licenseAssetFile(): String = if (lang == "tr") "GPLv3_TR.html" else "GPLv3_EN.html"

    fun t(key: String): String {
        val row = TABLE[key] ?: return key
        val idx = CODES.indexOf(lang).coerceAtLeast(0)
        return row.getOrNull(idx) ?: row[0]
    }

    fun f(key: String, vararg args: Any?): String =
        try { String.format(locale(), t(key), *args) } catch (_: Exception) { t(key) }

    fun upper(s: String): String = s.uppercase(locale())

    private val TR_PRAYER_KEYS = mapOf(
        "imsak" to "p_imsak", "sabah" to "p_sabah", "güneş" to "p_gunes", "gunes" to "p_gunes",
        "öğle" to "p_ogle", "ogle" to "p_ogle", "ikindi" to "p_ikindi",
        "akşam" to "p_aksam", "aksam" to "p_aksam", "yatsı" to "p_yatsi", "yatsi" to "p_yatsi",
        "dua" to "p_dua", "sela" to "p_sela", "cuma" to "p_sela"
    )

    /** Dahili Türkçe vakit adını (İmsak, Öğle, ...) seçili dile çevirir. */
    fun prayerFromTr(name: String): String {
        val k = name.trim().lowercase(Locale("tr", "TR"))
        return TR_PRAYER_KEYS[k]?.let { t(it) } ?: name
    }

    /** Open-Meteo hava durumu kodunu seçili dile çevirir. */
    fun weather(code: Int): String = t(
        when (code) {
            0 -> "wc_0"
            1 -> "wc_1"
            2 -> "wc_2"
            3 -> "wc_3"
            45, 48 -> "wc_fog"
            51, 53, 55 -> "wc_drizzle"
            61, 63 -> "wc_rain"
            65 -> "wc_heavy_rain"
            71, 73, 75 -> "wc_snow"
            80, 81, 82 -> "wc_showers"
            95, 96, 99 -> "wc_storm"
            else -> "wc_clear"
        }
    )

    /** Yerleşik ses dosyası adını seçili dile çevirir; bilinmiyorsa null döner. */
    fun soundName(raw: String): String? {
        val key = "snd_$raw"
        return if (TABLE.containsKey(key)) t(key) else null
    }

    // Sütun sırası: tr, en, de, zh, ja, ru
    private val TABLE: Map<String, Array<String>> = mapOf(
        // Alt menü
        "nav_vakit" to arrayOf("Vakit", "Times", "Zeiten", "时间", "時刻", "Время"),
        "nav_hava" to arrayOf("Hava", "Weather", "Wetter", "天气", "天気", "Погода"),
        "nav_kible" to arrayOf("Kıble", "Qibla", "Qibla", "朝向", "キブラ", "Кибла"),
        "nav_sesler" to arrayOf("Sesler", "Sounds", "Töne", "声音", "サウンド", "Звуки"),
        "nav_ayarlar" to arrayOf("Ayarlar", "Settings", "Einstellungen", "设置", "設定", "Настройки"),

        // Vakit adları
        "p_imsak" to arrayOf("İmsak", "Fajr", "Fadschr", "晨礼", "ファジュル", "Фаджр"),
        "p_sabah" to arrayOf("Sabah", "Fajr", "Fadschr", "晨礼", "ファジュル", "Фаджр"),
        "p_gunes" to arrayOf("Güneş", "Sunrise", "Sonnenaufgang", "日出", "日の出", "Восход"),
        "p_ogle" to arrayOf("Öğle", "Dhuhr", "Dhuhr", "晌礼", "ズフル", "Зухр"),
        "p_ikindi" to arrayOf("İkindi", "Asr", "Asr", "晡礼", "アスル", "Аср"),
        "p_aksam" to arrayOf("Akşam", "Maghrib", "Maghrib", "昏礼", "マグリブ", "Магриб"),
        "p_yatsi" to arrayOf("Yatsı", "Isha", "Ischa", "宵礼", "イシャー", "Иша"),
        "p_dua" to arrayOf("Dua", "Dua", "Dua", "杜阿", "ドゥアー", "Дуа"),
        "p_sela" to arrayOf("Sela", "Sala", "Sala", "赞礼", "サラー", "Сала"),

        // Kalan süre
        "rem_imsak" to arrayOf("Sabah'a Kalan", "Until Fajr", "Bis Fadschr", "距晨礼还有", "ファジュルまで", "До Фаджра"),
        "rem_gunes" to arrayOf("Güneş'e Kalan", "Until Sunrise", "Bis Sonnenaufgang", "距日出还有", "日の出まで", "До восхода"),
        "rem_ogle" to arrayOf("Öğle'ye Kalan", "Until Dhuhr", "Bis Dhuhr", "距晌礼还有", "ズフルまで", "До Зухра"),
        "rem_ikindi" to arrayOf("İkindi'ye Kalan", "Until Asr", "Bis Asr", "距晡礼还有", "アスルまで", "До Асра"),
        "rem_aksam" to arrayOf("Akşam'a Kalan", "Until Maghrib", "Bis Maghrib", "距昏礼还有", "マグリブまで", "До Магриба"),
        "rem_yatsi" to arrayOf("Yatsı'ya Kalan", "Until Isha", "Bis Ischa", "距宵礼还有", "イシャーまで", "До Иши"),

        // Ana ekran
        "h_time_of" to arrayOf("%s VAKTİ", "%s TIME", "%s-ZEIT", "%s时间", "%sの時間", "ВРЕМЯ: %s"),
        "h_source" to arrayOf("Kaynak: diyanet.gov.tr", "Source: diyanet.gov.tr", "Quelle: diyanet.gov.tr", "来源：diyanet.gov.tr", "出典: diyanet.gov.tr", "Источник: diyanet.gov.tr"),

        // Hava durumu
        "w_feels" to arrayOf("Hissedilen", "Feels like", "Gefühlt", "体感", "体感", "Ощущается"),
        "w_humidity" to arrayOf("Nem", "Humidity", "Luftfeuchte", "湿度", "湿度", "Влажность"),
        "w_wind" to arrayOf("Rüzgar", "Wind", "Wind", "风速", "風速", "Ветер"),
        "w_wind_unit" to arrayOf("km/s", "km/h", "km/h", "公里/时", "km/h", "км/ч"),
        "w_forecast" to arrayOf("5 Günlük Hava Tahmini", "5-Day Forecast", "5-Tage-Vorhersage", "5天天气预报", "5日間の天気予報", "Прогноз на 5 дней"),
        "wc_0" to arrayOf("Açık ve Güneşli", "Clear and Sunny", "Klar und sonnig", "晴朗", "快晴", "Ясно и солнечно"),
        "wc_1" to arrayOf("Çoğunlukla Açık", "Mostly Clear", "Überwiegend klar", "大部晴朗", "おおむね晴れ", "Преимущественно ясно"),
        "wc_2" to arrayOf("Parçalı Bulutlu", "Partly Cloudy", "Teilweise bewölkt", "局部多云", "晴れ時々曇り", "Переменная облачность"),
        "wc_3" to arrayOf("Bulutlu", "Cloudy", "Bewölkt", "多云", "曇り", "Облачно"),
        "wc_fog" to arrayOf("Sisli", "Foggy", "Neblig", "有雾", "霧", "Туман"),
        "wc_drizzle" to arrayOf("Çiseleyen Yağmur", "Drizzle", "Nieselregen", "毛毛雨", "霧雨", "Морось"),
        "wc_rain" to arrayOf("Yağmurlu", "Rainy", "Regnerisch", "有雨", "雨", "Дождь"),
        "wc_heavy_rain" to arrayOf("Kuvvetli Yağmur", "Heavy Rain", "Starkregen", "大雨", "大雨", "Сильный дождь"),
        "wc_snow" to arrayOf("Kar Yağışlı", "Snowy", "Schneefall", "有雪", "雪", "Снег"),
        "wc_showers" to arrayOf("Sağanak Yağış", "Showers", "Regenschauer", "阵雨", "にわか雨", "Ливень"),
        "wc_storm" to arrayOf("Gök Gürültülü Fırtına", "Thunderstorm", "Gewitter", "雷暴", "雷雨", "Гроза"),
        "wc_clear" to arrayOf("Açık", "Clear", "Klar", "晴", "晴れ", "Ясно"),

        // Kıble
        "q_title" to arrayOf("Kıble Pusulası", "Qibla Compass", "Qibla-Kompass", "基卜拉罗盘", "キブラコンパス", "Компас Киблы"),
        "q_live_gps" to arrayOf("Canlı GPS", "Live GPS", "Live-GPS", "实时GPS", "ライブGPS", "GPS онлайн"),
        "q_saved_loc" to arrayOf("Kayıtlı Konum", "Saved Location", "Gespeicherter Ort", "已保存位置", "保存済みの位置", "Сохранённое место"),
        "q_live_gps_loc" to arrayOf("Canlı GPS Konumu", "Live GPS Location", "Live-GPS-Standort", "实时GPS位置", "ライブGPS位置", "Текущее GPS-местоположение"),
        "q_coord" to arrayOf("Koordinat: %.4f° K, %.4f° D", "Coordinates: %.4f° N, %.4f° E", "Koordinaten: %.4f° N, %.4f° O", "坐标：北纬 %.4f°，东经 %.4f°", "座標: 北緯 %.4f°, 東経 %.4f°", "Координаты: %.4f° с. ш., %.4f° в. д."),
        "q_banner" to arrayOf("✦ KIBLEYE YÖNELDİNİZ ✦", "✦ YOU ARE FACING THE QIBLA ✦", "✦ SIE SIND ZUR QIBLA AUSGERICHTET ✦", "✦ 您已面向基卜拉 ✦", "✦ キブラの方向を向いています ✦", "✦ ВЫ ОБРАЩЕНЫ К КИБЛЕ ✦"),
        "q_on_dir" to arrayOf("Kıble Yönündesiniz", "You Are Facing the Qibla", "Sie sind zur Qibla ausgerichtet", "您正面向基卜拉", "キブラの方向です", "Вы смотрите на Киблу"),
        "q_angle" to arrayOf("Kıble Açısı: %d°", "Qibla Angle: %d°", "Qibla-Winkel: %d°", "基卜拉角度：%d°", "キブラ角度: %d°", "Угол Киблы: %d°"),
        "q_desc_aligned" to arrayOf("Cihaz şu an tam Kâbe istikametindedir.", "Your device is pointing exactly toward the Kaaba.", "Ihr Gerät zeigt jetzt genau in Richtung Kaaba.", "设备现在正对克尔白方向。", "端末は現在カアバの方向を正確に向いています。", "Устройство направлено точно на Каабу."),
        "q_desc_turn" to arrayOf("Cihazı yatay tutup altın ibre tepeye gelene kadar dönünüz.", "Hold your device flat and turn until the golden needle points up.", "Halten Sie das Gerät waagerecht und drehen Sie es, bis die goldene Nadel nach oben zeigt.", "请将设备水平放置并转动，直到金色指针指向上方。", "端末を水平に持ち、金色の針が上を向くまで回してください。", "Держите устройство горизонтально и поворачивайте, пока золотая стрелка не укажет вверх."),
        "q_verify_gps" to arrayOf("Konumumu GPS ile Doğrula", "Verify My Location via GPS", "Standort per GPS bestätigen", "通过GPS验证我的位置", "GPSで現在地を確認", "Уточнить местоположение по GPS"),

        // Ses ayarları
        "a_title" to arrayOf("Ezan ve Vakit Sesleri", "Adhan & Prayer Sounds", "Adhan- und Gebetstöne", "宣礼与礼拜声音", "アザーンと礼拝の音", "Азан и звуки намаза"),
        "a_adhan_of" to arrayOf("%s Ezanı", "%s Adhan", "%s-Adhan", "%s宣礼", "%sのアザーン", "Азан (%s)"),
        "a_custom" to arrayOf("Özel Ses", "Custom Sound", "Eigener Ton", "自定义声音", "カスタム音", "Свой звук"),
        "a_warn_title" to arrayOf("Vakit Öncesi Uyarı Sesi", "Pre-Prayer Warning Sound", "Vorwarnton vor dem Gebet", "礼拜前提醒音", "礼拝前の警告音", "Предупреждающий звук"),
        "a_warn_desc" to arrayOf("Vakitten %d dk önce", "%d min before prayer", "%d Min. vor dem Gebet", "礼拜前 %d 分钟", "礼拝の %d 分前", "За %d мин до намаза"),
        "a_volume" to arrayOf("Ses Seviyesi", "Volume", "Lautstärke", "音量", "音量", "Громкость"),
        "a_vibration" to arrayOf("Vakitlerde Titreşim", "Vibrate at Prayer Times", "Vibration zu Gebetszeiten", "礼拜时振动", "礼拝時刻に振動", "Вибрация во время намаза"),
        "a_battery_title" to arrayOf("Kesintisiz Ezan İzni (Pil Optimizasyonu)", "Uninterrupted Adhan (Battery Optimization)", "Ununterbrochener Adhan (Akku-Optimierung)", "不间断宣礼（电池优化）", "途切れないアザーン（バッテリー最適化）", "Непрерывный азан (оптимизация батареи)"),
        "a_battery_ok" to arrayOf("Pil kısıtlaması yok. Alarmlar ekran kapalıyken kesintisiz çalacaktır.", "No battery restriction. Alarms will play uninterrupted even with the screen off.", "Keine Akku-Beschränkung. Alarme werden auch bei ausgeschaltetem Bildschirm zuverlässig abgespielt.", "无电池限制。即使屏幕关闭，闹钟也会不间断播放。", "バッテリー制限はありません。画面オフでもアラームは途切れず鳴ります。", "Ограничений батареи нет. Сигналы будут звучать даже при выключенном экране."),
        "a_battery_need" to arrayOf("Telefonunuz arka planda ezan ve bildirimleri geciktirmemesi için pil kısıtlamasını kaldırın.", "Remove the battery restriction so your phone does not delay the adhan and notifications in the background.", "Entfernen Sie die Akku-Beschränkung, damit Ihr Telefon Adhan und Benachrichtigungen im Hintergrund nicht verzögert.", "请取消电池限制，以免手机在后台延迟宣礼和通知。", "バックグラウンドでアザーンや通知が遅れないよう、バッテリー制限を解除してください。", "Снимите ограничение батареи, чтобы телефон не задерживал азан и уведомления в фоне."),
        "a_allow" to arrayOf("İzin Ver", "Allow", "Erlauben", "允许", "許可", "Разрешить"),
        "a_pick" to arrayOf("Ses Dosyası Seç", "Choose Sound File", "Tondatei wählen", "选择声音文件", "音声ファイルを選択", "Выбрать звуковой файл"),
        "a_reset" to arrayOf("Varsayılana Dön", "Reset to Default", "Auf Standard zurücksetzen", "恢复默认", "デフォルトに戻す", "Сбросить"),
        "a_play" to arrayOf("Oynat", "Play", "Abspielen", "播放", "再生", "Воспроизвести"),
        "a_enabled" to arrayOf("Etkin", "Enabled", "Aktiv", "已启用", "有効", "Включено"),

        // Yerleşik ses adları
        "snd_sabah_saba" to arrayOf("Sabah (Saba Makamı)", "Fajr (Saba Maqam)", "Fadschr (Maqam Saba)", "晨礼（萨巴调式）", "ファジュル（サバ旋法）", "Фаджр (макам Саба)"),
        "snd_ogle_rast" to arrayOf("Öğle (Rast Makamı)", "Dhuhr (Rast Maqam)", "Dhuhr (Maqam Rast)", "晌礼（拉斯特调式）", "ズフル（ラスト旋法）", "Зухр (макам Раст)"),
        "snd_ikindi_hicaz" to arrayOf("İkindi (Hicaz Makamı)", "Asr (Hijaz Maqam)", "Asr (Maqam Hidschas)", "晡礼（希贾兹调式）", "アスル（ヒジャーズ旋法）", "Аср (макам Хиджаз)"),
        "snd_aksam_segah" to arrayOf("Akşam (Segah Makamı)", "Maghrib (Segah Maqam)", "Maghrib (Maqam Segah)", "昏礼（塞加调式）", "マグリブ（セガー旋法）", "Магриб (макам Сегях)"),
        "snd_yatsi_ussak" to arrayOf("Yatsı (Uşşak Makamı)", "Isha (Ushshaq Maqam)", "Ischa (Maqam Uschschak)", "宵礼（乌沙克调式）", "イシャー（ウッシャーク旋法）", "Иша (макам Ушшак)"),
        "snd_ezan_duasi" to arrayOf("Ezan Duası", "Adhan Supplication", "Adhan-Bittgebet", "宣礼后祈祷", "アザーン後のドゥアー", "Дуа после азана"),
        "snd_sela" to arrayOf("Cuma Selası", "Friday Sala", "Freitags-Sala", "主麻赞礼", "金曜日のサラー", "Пятничная сала"),
        "snd_uyari" to arrayOf("Kısa Uyarı Sesi", "Short Alert Sound", "Kurzer Warnton", "短提示音", "短い警告音", "Короткий сигнал"),

        // Ayarlar
        "s_title" to arrayOf("Bölgesel ve Genel Ayarlar", "Regional & General Settings", "Regionale und allgemeine Einstellungen", "地区与常规设置", "地域と一般設定", "Региональные и общие настройки"),
        "s_location" to arrayOf("Konum Seçimi", "Location", "Standortauswahl", "位置选择", "位置の選択", "Выбор местоположения"),
        "s_loading" to arrayOf("Yükleniyor...", "Loading...", "Wird geladen...", "加载中...", "読み込み中...", "Загрузка..."),
        "s_country" to arrayOf("Ülke", "Country", "Land", "国家", "国", "Страна"),
        "s_city" to arrayOf("Şehir", "City", "Stadt", "城市", "都市", "Город"),
        "s_district" to arrayOf("İlçe", "District", "Bezirk", "区县", "地区", "Район"),
        "s_save_loc" to arrayOf("Konumu Kaydet ve Vakitleri Güncelle", "Save Location & Update Times", "Standort speichern und Zeiten aktualisieren", "保存位置并更新时间", "位置を保存して時刻を更新", "Сохранить и обновить время"),
        "s_saved" to arrayOf("Konum kaydedildi!", "Location saved!", "Standort gespeichert!", "位置已保存！", "位置を保存しました！", "Местоположение сохранено!"),
        "s_ui_mode" to arrayOf("Arayüz Modu", "Interface Mode", "Oberflächenmodus", "界面模式", "表示モード", "Режим интерфейса"),
        "mode_saat" to arrayOf("Kadranlı Vakit Saati (Varsayılan)", "Dial Prayer Clock (Default)", "Zifferblatt-Gebetsuhr (Standard)", "表盘礼拜时钟（默认）", "ダイヤル式礼拝時計（デフォルト）", "Циферблат намаза (по умолчанию)"),
        "mode_gokyuzu" to arrayOf("Dinamik Gökyüzü Modu", "Dynamic Sky Mode", "Dynamischer Himmelsmodus", "动态天空模式", "ダイナミックスカイモード", "Динамическое небо"),
        "mode_kartlar" to arrayOf("Modern Kartlar Modu", "Modern Cards Mode", "Moderner Kartenmodus", "现代卡片模式", "モダンカードモード", "Современные карточки"),
        "s_theme" to arrayOf("Renk Teması", "Color Theme", "Farbthema", "颜色主题", "カラーテーマ", "Цветовая тема"),
        "th_varsayilan" to arrayOf("Açık Tema (Varsayılan)", "Light Theme (Default)", "Helles Thema (Standard)", "浅色主题（默认）", "ライトテーマ（デフォルト）", "Светлая тема (по умолчанию)"),
        "th_koyu" to arrayOf("Modern Koyu Tema", "Modern Dark Theme", "Modernes dunkles Thema", "现代深色主题", "モダンダークテーマ", "Современная тёмная тема"),
        "th_gece_mavisi" to arrayOf("Gece Mavisi", "Midnight Blue", "Mitternachtsblau", "午夜蓝", "ミッドナイトブルー", "Полночный синий"),
        "th_zumrut" to arrayOf("Zümrüt Yeşili", "Emerald Green", "Smaragdgrün", "翡翠绿", "エメラルドグリーン", "Изумрудный"),
        "th_kehribar" to arrayOf("Sıcak Kehribar", "Warm Amber", "Warmes Bernstein", "暖琥珀色", "ウォームアンバー", "Тёплый янтарь"),
        "s_language" to arrayOf("Dil / Language", "Language", "Sprache", "语言", "言語", "Язык"),
        "s_license" to arrayOf("Lisans (GPL v3)", "License (GPL v3)", "Lizenz (GPL v3)", "许可证 (GPL v3)", "ライセンス (GPL v3)", "Лицензия (GPL v3)"),
        "s_close" to arrayOf("Kapat", "Close", "Schließen", "关闭", "閉じる", "Закрыть"),
        "s_footer" to arrayOf("PyVakitci Mobil v1.8 • Diyanet İşleri Başkanlığı", "PyVakitci Mobile v1.8 • Presidency of Religious Affairs (Diyanet)", "PyVakitci Mobil v1.8 • Präsidium für Religionsangelegenheiten (Diyanet)", "PyVakitci 移动版 v1.8 • 土耳其宗教事务局 (Diyanet)", "PyVakitci モバイル v1.8 • トルコ宗務庁 (Diyanet)", "PyVakitci Mobile v1.8 • Управление по делам религии (Diyanet)"),

        // Bildirimler
        "n_channel" to arrayOf("Ezan ve Vakit Bildirimleri", "Adhan & Prayer Notifications", "Adhan- und Gebetsbenachrichtigungen", "宣礼与礼拜通知", "アザーンと礼拝の通知", "Уведомления об азане и намазе"),
        "n_channel_desc" to arrayOf("Vakit girdiğinde ve vakit öncesi uyarı bildirimleri", "Notifications at and before prayer times", "Benachrichtigungen zu und vor den Gebetszeiten", "礼拜时间及礼拜前提醒通知", "礼拝時刻と礼拝前の通知", "Уведомления о наступлении и приближении времени намаза"),
        "n_prayer_title" to arrayOf("%s Vakti Girdi", "It's %s time", "Es ist Zeit für %s", "%s时间到了", "%sの時間になりました", "Наступило время: %s"),
        "n_prayer_text" to arrayOf("%s | Saat: %s", "%s | Time: %s", "%s | Uhrzeit: %s", "%s | 时间：%s", "%s | 時刻: %s", "%s | Время: %s"),
        "n_warn_title" to arrayOf("Vakit Yaklaşıyor", "Prayer Time Approaching", "Gebetszeit naht", "礼拜时间即将到来", "礼拝時刻が近づいています", "Приближается время намаза"),
        "n_warn_text" to arrayOf("%1\$s vaktine %2\$d dakika kaldı.", "%2\$d minutes left until %1\$s.", "Noch %2\$d Minuten bis %1\$s.", "距%1\$s还有 %2\$d 分钟。", "%1\$sまであと%2\$d分です。", "До времени «%1\$s» осталось %2\$d мин."),
        "n_stop" to arrayOf("Sesi Durdur", "Stop Sound", "Ton stoppen", "停止声音", "音を停止", "Остановить звук"),

        // Widget
        "wg_tap_load" to arrayOf("Vakitleri yüklemek için dokunun", "Tap to load prayer times", "Tippen, um Gebetszeiten zu laden", "点击加载礼拜时间", "タップして礼拝時刻を読み込む", "Нажмите, чтобы загрузить время намаза")
    )
}

