package com.rahmanyazgan.pyvakitci.service

import android.app.AlarmManager
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.os.Build
import com.rahmanyazgan.pyvakitci.MainActivity
import com.rahmanyazgan.pyvakitci.data.local.AppDatabaseHelper
import com.rahmanyazgan.pyvakitci.data.local.PreferencesManager
import com.rahmanyazgan.pyvakitci.data.model.PrayerTimes
import com.rahmanyazgan.pyvakitci.util.TimeUtil
import java.text.SimpleDateFormat
import java.util.Calendar
import java.util.Locale

class BootReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == Intent.ACTION_BOOT_COMPLETED ||
            intent.action == "android.intent.action.QUICKBOOT_POWERON" ||
            intent.action == Intent.ACTION_MY_PACKAGE_REPLACED ||
            intent.action == Intent.ACTION_TIME_CHANGED ||
            intent.action == Intent.ACTION_TIMEZONE_CHANGED) {
            schedulePrayerAlarms(context)
        }
    }

    companion object {
        fun schedulePrayerAlarms(context: Context) {
            try {
                val prefs = PreferencesManager(context)
                val db = AppDatabaseHelper(context)
                val alarmManager = context.getSystemService(Context.ALARM_SERVICE) as? AlarmManager ?: return

                val now = System.currentTimeMillis()
                val dateFormat = SimpleDateFormat("yyyy-MM-dd", Locale.getDefault())

                // 7 gün boyunca tüm vakit ve uyarıları planla
                val daysToSchedule = (0..6).map { dayOffset ->
                    val c = Calendar.getInstance().apply { add(Calendar.DAY_OF_YEAR, dayOffset) }
                    Triple(
                        dateFormat.format(c.time),
                        c.get(Calendar.YEAR),
                        c.get(Calendar.DAY_OF_YEAR)
                    )
                }

                for ((dateStr, year, dayOfYear) in daysToSchedule) {
                    val times = db.getPrayerTimesForDate(prefs.location.districtId, dateStr)
                        ?: if (dateStr == TimeUtil.getTodayString()) PrayerTimes.defaultSample() else continue

                    val prayerList = listOf(
                        "İmsak" to times.imsak,
                        "Öğle" to times.ogle,
                        "İkindi" to times.ikindi,
                        "Akşam" to times.aksam,
                        "Yatsı" to times.yatsi
                    )

                    for ((idx, pair) in prayerList.withIndex()) {
                        val (name, timeStr) = pair
                        val parts = timeStr.trim().split(":")
                        if (parts.size >= 2) {
                            val hour = parts[0].toIntOrNull() ?: continue
                            val minute = parts[1].toIntOrNull() ?: continue

                            val prayerCal = Calendar.getInstance().apply {
                                set(Calendar.YEAR, year)
                                set(Calendar.DAY_OF_YEAR, dayOfYear)
                                set(Calendar.HOUR_OF_DAY, hour)
                                set(Calendar.MINUTE, minute)
                                set(Calendar.SECOND, 0)
                                set(Calendar.MILLISECOND, 0)
                            }

                            val prayerTimeMillis = prayerCal.timeInMillis
                            val warnReqCode = (dayOfYear * 100) + idx
                            val alarmReqCode = (dayOfYear * 100) + 50 + idx

                            // 1) Erken Uyarı Alarmı (15 dakika önce)
                            if (prefs.isWarningSoundEnabled && prefs.warningMinutes > 0) {
                                val warningTimeMillis = prayerTimeMillis - (prefs.warningMinutes * 60 * 1000L)
                                if (warningTimeMillis > now) {
                                    val warnIntent = Intent(context, AlarmReceiver::class.java).apply {
                                        action = "com.rahmanyazgan.pyvakitci.ACTION_WARN_${year}_${dayOfYear}_$idx"
                                        putExtra("PRAYER_NAME", name)
                                        putExtra("PRAYER_TIME", timeStr)
                                        putExtra("IS_WARNING", true)
                                    }
                                    val warnPi = PendingIntent.getBroadcast(
                                        context, warnReqCode, warnIntent,
                                        PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
                                    )
                                    setExactAlarm(alarmManager, warningTimeMillis, warnPi, context)
                                }
                            }

                            // 2) Vakit Giriş Alarmı (Ezan / Tam vakit)
                            if (prayerTimeMillis > now) {
                                val alarmIntent = Intent(context, AlarmReceiver::class.java).apply {
                                    action = "com.rahmanyazgan.pyvakitci.ACTION_ALARM_${year}_${dayOfYear}_$idx"
                                    putExtra("PRAYER_NAME", name)
                                    putExtra("PRAYER_TIME", timeStr)
                                    putExtra("IS_WARNING", false)
                                }
                                val pi = PendingIntent.getBroadcast(
                                    context, alarmReqCode, alarmIntent,
                                    PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
                                )
                                setExactAlarm(alarmManager, prayerTimeMillis, pi, context)
                            }
                        }
                    }
                }
            } catch (_: Exception) {}
        }

        private fun setExactAlarm(
            alarmManager: AlarmManager,
            triggerMillis: Long,
            pi: PendingIntent,
            context: Context
        ) {
            try {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    val showIntent = Intent(context, MainActivity::class.java).apply {
                        flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TOP
                    }
                    val showPi = PendingIntent.getActivity(
                        context, 0, showIntent,
                        PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
                    )
                    alarmManager.setAlarmClock(
                        AlarmManager.AlarmClockInfo(triggerMillis, showPi),
                        pi
                    )
                } else {
                    alarmManager.setExact(AlarmManager.RTC_WAKEUP, triggerMillis, pi)
                }
            } catch (_: Exception) {
                try {
                    alarmManager.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, triggerMillis, pi)
                } catch (_: Exception) {
                    try {
                        alarmManager.set(AlarmManager.RTC_WAKEUP, triggerMillis, pi)
                    } catch (_: Exception) {}
                }
            }
        }
    }
}
