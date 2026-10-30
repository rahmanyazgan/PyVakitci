package com.rahmanyazgan.pyvakitci.data.model

data class PrayerTimes(
    val date: String,          // YYYY-MM-DD
    val hijriDate: String,     // e.g. "17 Cemaziyelevvel 1448"
    val imsak: String,         // "05:58"
    val gunes: String,         // "07:24"
    val ogle: String,          // "12:53"
    val ikindi: String,        // "15:44"
    val aksam: String,         // "18:12"
    val yatsi: String          // "19:32"
) {
    fun toList(): List<Pair<String, String>> = listOf(
        "İmsak" to imsak,
        "Güneş" to gunes,
        "Öğle" to ogle,
        "İkindi" to ikindi,
        "Akşam" to aksam,
        "Yatsı" to yatsi
    )

    companion object {
        fun defaultSample(): PrayerTimes {
            return PrayerTimes(
                date = java.text.SimpleDateFormat("yyyy-MM-dd", java.util.Locale.getDefault()).format(java.util.Calendar.getInstance().time),
                hijriDate = "17 Cemaziyelevvel 1448",
                imsak = "05:58",
                gunes = "07:24",
                ogle = "12:53",
                ikindi = "15:44",
                aksam = "18:12",
                yatsi = "19:32"
            )
        }
    }
}

val TR_LOCALE = java.util.Locale("tr", "TR")
fun String.toTrUpper(): String = this.uppercase(TR_LOCALE)
fun String.toTrLower(): String = this.lowercase(TR_LOCALE)

enum class PrayerKind(val title: String, val uppercaseTitle: String) {
    IMSAK("Sabah", "SABAH"),
    GUNES("Güneş", "GÜNEŞ"),
    OGLE("Öğle", "ÖĞLE"),
    IKINDI("İkindi", "İKİNDİ"),
    AKSAM("Akşam", "AKŞAM"),
    YATSI("Yatsı", "YATSI");

    fun getRemainingSuffix(): String = when (this) {
        IMSAK -> "Sabah'a Kalan"
        GUNES -> "Güneş'e Kalan"
        OGLE -> "Öğle'ye Kalan"
        IKINDI -> "İkindi'ye Kalan"
        AKSAM -> "Akşam'a Kalan"
        YATSI -> "Yatsı'ya Kalan"
    }
}

data class ActivePrayerInfo(
    val currentPrayer: PrayerKind,
    val nextPrayer: PrayerKind,
    val remainingSeconds: Long,
    val remainingFormatted: String
)

