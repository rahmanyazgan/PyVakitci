package com.rahmanyazgan.pyvakitci.service

import android.app.AlarmManager
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import com.rahmanyazgan.pyvakitci.data.local.AppDatabaseHelper
import com.rahmanyazgan.pyvakitci.data.local.PreferencesManager
import com.rahmanyazgan.pyvakitci.util.TimeUtil
import java.util.Calendar

class BootReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == Intent.ACTION_BOOT_COMPLETED ||
            intent.action == "android.intent.action.QUICKBOOT_POWERON") {
            schedulePrayerAlarms(context)
        }
    }

    companion object {
        fun schedulePrayerAlarms(context: Context) {
            val prefs = PreferencesManager(context)
            val db = AppDatabaseHelper(context)
            val today = TimeUtil.getTodayString()
            val times = db.getPrayerTimesForDate(prefs.location.districtId, today) ?: return

            val alarmManager = context.getSystemService(Context.ALARM_SERVICE) as AlarmManager

            val prayerList = listOf(
                "İmsak" to times.imsak,
                "Öğle" to times.ogle,
                "İkindi" to times.ikindi,
                "Akşam" to times.aksam,
                "Yatsı" to times.yatsi
            )

            var reqCode = 100
            for ((name, timeStr) in prayerList) {
                val cal = Calendar.getInstance()
                val parts = timeStr.split(":")
                if (parts.size >= 2) {
                    cal.set(Calendar.HOUR_OF_DAY, parts[0].toInt())
                    cal.set(Calendar.MINUTE, parts[1].toInt())
                    cal.set(Calendar.SECOND, 0)

                    if (cal.timeInMillis > System.currentTimeMillis()) {
                        val alarmIntent = Intent(context, AlarmReceiver::class.java).apply {
                            action = "com.rahmanyazgan.pyvakitci.ACTION_PRAYER_ALARM"
                            putExtra("PRAYER_NAME", name)
                            putExtra("PRAYER_TIME", timeStr)
                            putExtra("IS_WARNING", false)
                        }
                        val pi = PendingIntent.getBroadcast(
                            context, reqCode++, alarmIntent,
                            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
                        )

                        try {
                            alarmManager.setExactAndAllowWhileIdle(
                                AlarmManager.RTC_WAKEUP,
                                cal.timeInMillis,
                                pi
                            )
                        } catch (_: Exception) {}
                    }
                }
            }
        }
    }
}

