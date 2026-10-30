package com.rahmanyazgan.pyvakitci.data.local

import android.content.Context
import android.content.SharedPreferences
import com.rahmanyazgan.pyvakitci.data.model.SelectedLocation

class PreferencesManager(context: Context) {
    private val prefs: SharedPreferences =
        context.getSharedPreferences("pyvakitci_mobile_prefs", Context.MODE_PRIVATE)

    companion object {
        private const val KEY_COUNTRY_ID = "country_id"
        private const val KEY_COUNTRY_NAME = "country_name"
        private const val KEY_CITY_ID = "city_id"
        private const val KEY_CITY_NAME = "city_name"
        private const val KEY_DISTRICT_ID = "district_id"
        private const val KEY_DISTRICT_NAME = "district_name"
        private const val KEY_LATITUDE = "latitude"
        private const val KEY_LONGITUDE = "longitude"

        private const val KEY_INTERFACE_MODE = "interface_mode" // "saat", "gokyuzu", "kartlar", "klasik"
        private const val KEY_THEME = "theme" // "varsayilan", "koyu", "gece_mavisi", "zumrut", "kehribar"

        // Audio & Notifications
        private const val KEY_WARNING_SOUND = "warning_sound"
        private const val KEY_WARNING_MINUTES = "warning_minutes"
        private const val KEY_VOLUME = "volume"
        private const val KEY_VIBRATION = "vibration"

        private const val PREFIX_AZAN_ENABLED = "azan_enabled_"
        private const val PREFIX_AZAN_SOUND = "azan_sound_"
        private const val PREFIX_AZAN_DISPLAY_NAME = "azan_display_name_"
    }

    var location: SelectedLocation
        get() = SelectedLocation(
            countryId = prefs.getString(KEY_COUNTRY_ID, "2") ?: "2",
            countryName = prefs.getString(KEY_COUNTRY_NAME, "TÜRKİYE") ?: "TÜRKİYE",
            cityId = prefs.getString(KEY_CITY_ID, "539") ?: "539",
            cityName = prefs.getString(KEY_CITY_NAME, "İSTANBUL") ?: "İSTANBUL",
            districtId = prefs.getString(KEY_DISTRICT_ID, "9541") ?: "9541",
            districtName = prefs.getString(KEY_DISTRICT_NAME, "İSTANBUL") ?: "İSTANBUL",
            latitude = prefs.getFloat(KEY_LATITUDE, 41.0082f).toDouble(),
            longitude = prefs.getFloat(KEY_LONGITUDE, 28.9784f).toDouble()
        )
        set(value) {
            prefs.edit()
                .putString(KEY_COUNTRY_ID, value.countryId)
                .putString(KEY_COUNTRY_NAME, value.countryName)
                .putString(KEY_CITY_ID, value.cityId)
                .putString(KEY_CITY_NAME, value.cityName)
                .putString(KEY_DISTRICT_ID, value.districtId)
                .putString(KEY_DISTRICT_NAME, value.districtName)
                .putFloat(KEY_LATITUDE, value.latitude.toFloat())
                .putFloat(KEY_LONGITUDE, value.longitude.toFloat())
                .apply()
        }

    var interfaceMode: String
        get() = prefs.getString(KEY_INTERFACE_MODE, "saat") ?: "saat"
        set(value) = prefs.edit().putString(KEY_INTERFACE_MODE, value).apply()

    var theme: String
        get() = prefs.getString(KEY_THEME, "varsayilan") ?: "varsayilan"
        set(value) = prefs.edit().putString(KEY_THEME, value).apply()

    var isWarningSoundEnabled: Boolean
        get() = prefs.getBoolean(KEY_WARNING_SOUND, true)
        set(value) = prefs.edit().putBoolean(KEY_WARNING_SOUND, value).apply()

    var warningMinutes: Int
        get() = prefs.getInt(KEY_WARNING_MINUTES, 15)
        set(value) = prefs.edit().putInt(KEY_WARNING_MINUTES, value).apply()

    var volume: Int
        get() = prefs.getInt(KEY_VOLUME, 100)
        set(value) = prefs.edit().putInt(KEY_VOLUME, value).apply()

    var isVibrationEnabled: Boolean
        get() = prefs.getBoolean(KEY_VIBRATION, true)
        set(value) = prefs.edit().putBoolean(KEY_VIBRATION, value).apply()

    fun normalizePrayerKey(name: String): String {
        val clean = name.trim().lowercase(java.util.Locale("tr", "TR"))
            .replace("ı", "i")
            .replace("ğ", "g")
            .replace("ü", "u")
            .replace("ş", "s")
            .replace("ö", "o")
            .replace("ç", "c")
            .replace("\u0307", "")

        return when (clean) {
            "imsak", "sabah" -> "sabah"
            "ogle" -> "ogle"
            "ikindi" -> "ikindi"
            "aksam" -> "aksam"
            "yatsi" -> "yatsi"
            "dua" -> "dua"
            "sela", "cuma" -> "sela"
            else -> clean
        }
    }

    fun getDefaultSoundForPrayer(prayerName: String): String {
        return when (normalizePrayerKey(prayerName)) {
            "sabah" -> "sabah_saba"
            "ogle" -> "ogle_rast"
            "ikindi" -> "ikindi_hicaz"
            "aksam" -> "aksam_segah"
            "yatsi" -> "yatsi_ussak"
            "dua" -> "ezan_duasi"
            "sela" -> "sela"
            else -> "uyari"
        }
    }

    fun isPrayerSoundEnabled(prayerName: String): Boolean {
        val key = normalizePrayerKey(prayerName)
        return prefs.getBoolean(PREFIX_AZAN_ENABLED + key, true)
    }

    fun setPrayerSoundEnabled(prayerName: String, enabled: Boolean) {
        val key = normalizePrayerKey(prayerName)
        prefs.edit().putBoolean(PREFIX_AZAN_ENABLED + key, enabled).apply()
    }

    fun getPrayerSound(prayerName: String, defaultRaw: String = ""): String {
        val key = normalizePrayerKey(prayerName)
        val fallback = if (defaultRaw.isNotEmpty()) defaultRaw else getDefaultSoundForPrayer(key)
        return prefs.getString(PREFIX_AZAN_SOUND + key, fallback) ?: fallback
    }

    fun setPrayerSound(prayerName: String, rawName: String) {
        val key = normalizePrayerKey(prayerName)
        prefs.edit().putString(PREFIX_AZAN_SOUND + key, rawName).apply()
    }

    fun getPrayerSoundDisplayName(prayerName: String): String {
        val key = normalizePrayerKey(prayerName)
        val customName = prefs.getString(PREFIX_AZAN_DISPLAY_NAME + key, null)
        if (!customName.isNullOrEmpty()) return customName
        val sound = getPrayerSound(key)
        return when (sound) {
            "sabah_saba" -> "Sabah (Saba Makamı)"
            "ogle_rast" -> "Öğle (Rast Makamı)"
            "ikindi_hicaz" -> "İkindi (Hicaz Makamı)"
            "aksam_segah" -> "Akşam (Segah Makamı)"
            "yatsi_ussak" -> "Yatsı (Uşşak Makamı)"
            "ezan_duasi" -> "Ezan Duası"
            "sela" -> "Cuma Selası"
            "uyari" -> "Kısa Uyarı Sesi"
            else -> sound.substringAfterLast("/")
        }
    }

    fun setPrayerSoundCustom(prayerName: String, filePath: String, displayName: String) {
        val key = normalizePrayerKey(prayerName)
        prefs.edit()
            .putString(PREFIX_AZAN_SOUND + key, filePath)
            .putString(PREFIX_AZAN_DISPLAY_NAME + key, displayName)
            .apply()
    }

    fun resetPrayerSoundToDefault(prayerName: String) {
        val key = normalizePrayerKey(prayerName)
        val defaultSound = getDefaultSoundForPrayer(key)
        prefs.edit()
            .putString(PREFIX_AZAN_SOUND + key, defaultSound)
            .remove(PREFIX_AZAN_DISPLAY_NAME + key)
            .apply()
    }

    fun isCustomSound(prayerName: String): Boolean {
        val key = normalizePrayerKey(prayerName)
        return prefs.contains(PREFIX_AZAN_DISPLAY_NAME + key)
    }
}

