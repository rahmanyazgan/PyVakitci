package com.rahmanyazgan.pyvakitci.util

import com.rahmanyazgan.pyvakitci.data.model.ActivePrayerInfo
import com.rahmanyazgan.pyvakitci.data.model.PrayerKind
import com.rahmanyazgan.pyvakitci.data.model.PrayerTimes
import java.text.SimpleDateFormat
import java.util.Calendar
import java.util.Locale

object TimeUtil {
    private val dateFormat = SimpleDateFormat("yyyy-MM-dd", Locale.getDefault())
    private val displayDateFormat = SimpleDateFormat("d MMMM yyyy", Locale("tr"))
    private val timeFormat = SimpleDateFormat("HH:mm:ss", Locale.getDefault())

    fun getTodayString(): String = dateFormat.format(Calendar.getInstance().time)
    fun getDisplayDate(calendar: Calendar = Calendar.getInstance()): String =
        displayDateFormat.format(calendar.time)

    fun getCurrentTimeString(): String = timeFormat.format(Calendar.getInstance().time)

    fun timeToMinutes(timeStr: String): Int {
        val parts = timeStr.trim().split(":")
        if (parts.size >= 2) {
            val h = parts[0].toIntOrNull() ?: 0
            val m = parts[1].toIntOrNull() ?: 0
            return h * 60 + m
        }
        return 0
    }

    fun timeToSeconds(timeStr: String): Long {
        val parts = timeStr.trim().split(":")
        val h = parts.getOrNull(0)?.toLongOrNull() ?: 0L
        val m = parts.getOrNull(1)?.toLongOrNull() ?: 0L
        val s = parts.getOrNull(2)?.toLongOrNull() ?: 0L
        return h * 3600L + m * 60L + s
    }

    /**
     * Converts time to angle on a 24-hour dial.
     * Top (24:00/00:00) is -90 degrees in standard Canvas or 0 degrees at top.
     * Angle = (minutes / 1440.0) * 360.0
     */
    fun timeToDialAngle(timeStr: String): Float {
        val minutes = timeToMinutes(timeStr)
        return (minutes / 1440f) * 360f
    }

    fun calculateActivePrayer(prayerTimes: PrayerTimes, calendar: Calendar = Calendar.getInstance()): ActivePrayerInfo {
        val nowSec = calendar.get(Calendar.HOUR_OF_DAY) * 3600L +
                calendar.get(Calendar.MINUTE) * 60L +
                calendar.get(Calendar.SECOND)

        val imsakSec = timeToSeconds(prayerTimes.imsak)
        val gunesSec = timeToSeconds(prayerTimes.gunes)
        val ogleSec = timeToSeconds(prayerTimes.ogle)
        val ikindiSec = timeToSeconds(prayerTimes.ikindi)
        val aksamSec = timeToSeconds(prayerTimes.aksam)
        val yatsiSec = timeToSeconds(prayerTimes.yatsi)

        val currentPrayer: PrayerKind
        val nextPrayer: PrayerKind
        val remainingSec: Long

        when {
            nowSec < imsakSec -> {
                currentPrayer = PrayerKind.YATSI
                nextPrayer = PrayerKind.IMSAK
                remainingSec = imsakSec - nowSec
            }
            nowSec < gunesSec -> {
                currentPrayer = PrayerKind.IMSAK
                nextPrayer = PrayerKind.GUNES
                remainingSec = gunesSec - nowSec
            }
            nowSec < ogleSec -> {
                currentPrayer = PrayerKind.GUNES
                nextPrayer = PrayerKind.OGLE
                remainingSec = ogleSec - nowSec
            }
            nowSec < ikindiSec -> {
                currentPrayer = PrayerKind.OGLE
                nextPrayer = PrayerKind.IKINDI
                remainingSec = ikindiSec - nowSec
            }
            nowSec < aksamSec -> {
                currentPrayer = PrayerKind.IKINDI
                nextPrayer = PrayerKind.AKSAM
                remainingSec = aksamSec - nowSec
            }
            nowSec < yatsiSec -> {
                currentPrayer = PrayerKind.AKSAM
                nextPrayer = PrayerKind.YATSI
                remainingSec = yatsiSec - nowSec
            }
            else -> {
                currentPrayer = PrayerKind.YATSI
                nextPrayer = PrayerKind.IMSAK
                // remaining until next day's imsak
                remainingSec = (86400L - nowSec) + imsakSec
            }
        }

        val hours = remainingSec / 3600
        val mins = (remainingSec % 3600) / 60
        val secs = remainingSec % 60
        val formatted = String.format(Locale.getDefault(), "%02d:%02d:%02d", hours, mins, secs)

        return ActivePrayerInfo(
            currentPrayer = currentPrayer,
            nextPrayer = nextPrayer,
            remainingSeconds = remainingSec,
            remainingFormatted = formatted
        )
    }
}

