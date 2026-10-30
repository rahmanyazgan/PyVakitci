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
                date = "2026-10-29",
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

enum class PrayerKind(val title: String) {
    IMSAK("İmsak"),
    GUNES("Güneş"),
    OGLE("Öğle"),
    IKINDI("İkindi"),
    AKSAM("Akşam"),
    YATSI("Yatsı")
}

data class ActivePrayerInfo(
    val currentPrayer: PrayerKind,
    val nextPrayer: PrayerKind,
    val remainingSeconds: Long,
    val remainingFormatted: String
)

