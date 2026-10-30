package com.rahmanyazgan.pyvakitci.service

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import com.rahmanyazgan.pyvakitci.data.local.PreferencesManager

class AlarmReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        val prayerName = intent.getStringExtra("PRAYER_NAME") ?: "Vakit"
        val prayerTime = intent.getStringExtra("PRAYER_TIME") ?: ""
        val isWarning = intent.getBooleanExtra("IS_WARNING", false)

        val prefs = PreferencesManager(context)
        val notifHelper = PrayerNotificationHelper(context)
        val audioPlayer = AudioPlayerManager(context)

        if (isWarning) {
            if (prefs.isWarningSoundEnabled) {
                notifHelper.showWarningNotification(prayerName, prefs.warningMinutes)
                audioPlayer.playByName("uyari", prefs.volume)
            }
        } else {
            if (prefs.isPrayerSoundEnabled(prayerName)) {
                val loc = prefs.location
                notifHelper.showPrayerNotification(prayerName, prayerTime, "${loc.cityName} / ${loc.districtName}")

                val soundKey = when (prayerName.lowercase()) {
                    "sabah" -> "sabah_saba"
                    "öğle", "ogle" -> "ogle_rast"
                    "ikindi" -> "ikindi_hicaz"
                    "akşam", "aksam" -> "aksam_segah"
                    "yatsı", "yatsi" -> "yatsi_ussak"
                    else -> "uyari"
                }
                val selectedSound = prefs.getPrayerSound(prayerName, soundKey)
                audioPlayer.playByName(selectedSound, prefs.volume)
            }
        }
    }
}

