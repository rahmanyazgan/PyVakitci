package com.rahmanyazgan.pyvakitci.util

import java.util.Calendar

object Calverter {
    private val islamicMonths = arrayOf(
        "Muharrem", "Safer", "Rebiülevvel", "Rebiülahir",
        "Cemaziyelevvel", "Cemaziyelahir", "Recep", "Şaban",
        "Ramazan", "Şevval", "Zilkade", "Zilhicce"
    )

    fun getHijriDate(calendar: Calendar = Calendar.getInstance()): String {
        val y = calendar.get(Calendar.YEAR)
        val m = calendar.get(Calendar.MONTH) + 1
        val d = calendar.get(Calendar.DAY_OF_MONTH)
        val jd = gregorianToJd(y, m, d)
        val (hYear, hMonth, hDay) = jdToIslamic(jd)
        val monthName = islamicMonths.getOrElse(hMonth - 1) { "" }
        return "$hDay $monthName $hYear"
    }

    private fun gregorianToJd(year: Int, month: Int, day: Int): Double {
        var y = year
        var m = month
        if (m <= 2) {
            y -= 1
            m += 12
        }
        val a = y / 100
        val b = 2 - a + (a / 4)
        return (Math.floor(365.25 * (y + 4716)) +
                Math.floor(30.6001 * (m + 1)) +
                day + b - 1524.5)
    }

    private fun jdToIslamic(jd: Double): Triple<Int, Int, Int> {
        val jdFloor = Math.floor(jd) + 0.5
        val year = Math.floor((30.0 * (jdFloor - 1948440.0) + 10646.0) / 10631.0).toInt()
        val month = Math.min(
            12.0,
            Math.ceil((jdFloor - (29.0 + islamicToJd(year, 1, 1))) / 29.5) + 1.0
        ).toInt()
        val day = (jdFloor - islamicToJd(year, month, 1) + 1.0).toInt()
        return Triple(year, month, day)
    }

    private fun islamicToJd(year: Int, month: Int, day: Int): Double {
        return (day +
                Math.ceil(29.5 * (month - 1)) +
                (year - 1) * 354 +
                Math.floor((3 + (11 * year)) / 30.0) +
                1948440 - 1)
    }
}

