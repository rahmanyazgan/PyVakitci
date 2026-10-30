package com.rahmanyazgan.pyvakitci.service

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.PowerManager
import android.os.VibrationEffect
import android.os.Vibrator
import android.os.VibratorManager
import com.rahmanyazgan.pyvakitci.data.local.PreferencesManager
import com.rahmanyazgan.pyvakitci.util.TimeUtil
import com.rahmanyazgan.pyvakitci.widget.PrayerWidgetProvider

class AlarmReceiver : BroadcastReceiver() {

    companion object {
        const val ACTION_STOP_AUDIO = "com.rahmanyazgan.pyvakitci.ACTION_STOP_AUDIO"

        fun triggerVibration(context: Context, isWarning: Boolean) {
            try {
                val pattern = if (isWarning) {
                    longArrayOf(0, 150, 100, 150)
                } else {
                    longArrayOf(0, 400, 200, 400, 200, 600)
                }

                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                    val vm = context.getSystemService(Context.VIBRATOR_MANAGER_SERVICE) as? VibratorManager
                    vm?.defaultVibrator?.vibrate(VibrationEffect.createWaveform(pattern, -1))
                } else {
                    @Suppress("DEPRECATION")
                    val v = context.getSystemService(Context.VIBRATOR_SERVICE) as? Vibrator
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                        v?.vibrate(VibrationEffect.createWaveform(pattern, -1))
                    } else {
                        @Suppress("DEPRECATION")
                        v?.vibrate(pattern, -1)
                    }
                }
            } catch (_: Exception) {}
        }
    }

    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == ACTION_STOP_AUDIO) {
            AudioPlayerManager.stopActiveSound()
            PrayerNotificationHelper(context).cancelAll()
            return
        }

        val prayerName = intent.getStringExtra("PRAYER_NAME") ?: "Vakit"
        val prayerTime = intent.getStringExtra("PRAYER_TIME") ?: ""
        val isWarning = intent.getBooleanExtra("IS_WARNING", false)

        val prefs = PreferencesManager(context)
        val notifHelper = PrayerNotificationHelper(context)

        // Widgetları anında güncelle
        PrayerWidgetProvider.updateAllWidgets(context)

        val pm = context.getSystemService(Context.POWER_SERVICE) as PowerManager
        val wakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "PyVakitci::AlarmReceiverWakeLock").apply {
            setReferenceCounted(false)
        }

        // Titreşim aktifse donanım seviyesinde titreşim ver
        if (prefs.isVibrationEnabled) {
            triggerVibration(context, isWarning)
        }

        if (isWarning) {
            if (prefs.isWarningSoundEnabled) {
                AudioPlayerManager.lastTriggeredKey = "${TimeUtil.getTodayString()}_${prayerTime}_warn_$prayerName"
                notifHelper.showWarningNotification(prayerName, prefs.warningMinutes)

                val pendingResult = goAsync()
                try {
                    wakeLock.acquire(2 * 60 * 1000L) // 2 dakika
                } catch (_: Exception) {}

                val audioPlayer = AudioPlayerManager(context)
                val warningSound = prefs.getPrayerSound("uyari")

                audioPlayer.onCompletionListener = {
                    try {
                        if (wakeLock.isHeld) wakeLock.release()
                    } catch (_: Exception) {}
                    try {
                        pendingResult.finish()
                    } catch (_: Exception) {}
                }
                audioPlayer.playAudio(warningSound, prefs.volume, tag = "uyari_$prayerName")
            }
        } else {
            val prayerKey = prefs.normalizePrayerKey(prayerName)
            if (prefs.isPrayerSoundEnabled(prayerKey)) {
                AudioPlayerManager.lastTriggeredKey = "${TimeUtil.getTodayString()}_${prayerTime}_$prayerName"
                val loc = prefs.location
                notifHelper.showPrayerNotification(prayerName, prayerTime, "${loc.cityName} / ${loc.districtName}")

                val pendingResult = goAsync()
                try {
                    wakeLock.acquire(10 * 60 * 1000L) // 10 dakika
                } catch (_: Exception) {}

                val audioPlayer = AudioPlayerManager(context)
                val selectedSound = prefs.getPrayerSound(prayerKey)

                audioPlayer.onCompletionListener = {
                    if (prefs.isPrayerSoundEnabled("dua")) {
                        val duaSound = prefs.getPrayerSound("dua")
                        audioPlayer.onCompletionListener = {
                            try {
                                if (wakeLock.isHeld) wakeLock.release()
                            } catch (_: Exception) {}
                            try {
                                pendingResult.finish()
                            } catch (_: Exception) {}
                        }
                        audioPlayer.playAudio(duaSound, prefs.volume, tag = "ezan_duasi")
                    } else {
                        try {
                            if (wakeLock.isHeld) wakeLock.release()
                        } catch (_: Exception) {}
                        try {
                            pendingResult.finish()
                        } catch (_: Exception) {}
                    }
                }
                audioPlayer.playAudio(selectedSound, prefs.volume, tag = "ezan_$prayerKey")
            }
        }

        // Gelecek alarmları ve vakitleri tazele
        try {
            BootReceiver.schedulePrayerAlarms(context)
        } catch (_: Exception) {}
    }
}
