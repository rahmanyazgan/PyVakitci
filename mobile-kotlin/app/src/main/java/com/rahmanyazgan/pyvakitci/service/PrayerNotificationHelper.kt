package com.rahmanyazgan.pyvakitci.service

import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.os.Build
import androidx.core.app.NotificationCompat
import com.rahmanyazgan.pyvakitci.MainActivity
import com.rahmanyazgan.pyvakitci.R

class PrayerNotificationHelper(private val context: Context) {
    private val notificationManager =
        context.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager

    companion object {
        const val CHANNEL_ID_PRAYER = "channel_prayer_times"
        const val CHANNEL_NAME_PRAYER = "Ezan ve Vakit Bildirimleri"
        const val NOTIFICATION_ID_PRAYER = 1001
        const val NOTIFICATION_ID_WARNING = 1002
    }

    init {
        createNotificationChannels()
    }

    private fun createNotificationChannels() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                CHANNEL_ID_PRAYER,
                CHANNEL_NAME_PRAYER,
                NotificationManager.IMPORTANCE_HIGH
            ).apply {
                description = "Vakit girdiğinde ve vakit öncesi uyarı bildirimleri"
                enableVibration(true)
            }
            notificationManager.createNotificationChannel(channel)
        }
    }

    fun showPrayerNotification(prayerName: String, time: String, locationText: String) {
        val intent = Intent(context, MainActivity::class.java).apply {
            flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TOP
        }
        val pendingIntent = PendingIntent.getActivity(
            context, 0, intent,
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        )

        val notification = NotificationCompat.Builder(context, CHANNEL_ID_PRAYER)
            .setSmallIcon(R.mipmap.ic_launcher)
            .setContentTitle("$prayerName Vakti Girdi")
            .setContentText("$locationText | Saat: $time")
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setAutoCancel(true)
            .setContentIntent(pendingIntent)
            .build()

        notificationManager.notify(NOTIFICATION_ID_PRAYER, notification)
    }

    fun showWarningNotification(prayerName: String, minutesLeft: Int) {
        val intent = Intent(context, MainActivity::class.java)
        val pendingIntent = PendingIntent.getActivity(
            context, 1, intent,
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        )

        val notification = NotificationCompat.Builder(context, CHANNEL_ID_PRAYER)
            .setSmallIcon(R.mipmap.ic_launcher)
            .setContentTitle("Vakit Yaklaşıyor")
            .setContentText("$prayerName vaktine $minutesLeft dakika kaldı.")
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setAutoCancel(true)
            .setContentIntent(pendingIntent)
            .build()

        notificationManager.notify(NOTIFICATION_ID_WARNING, notification)
    }
}

