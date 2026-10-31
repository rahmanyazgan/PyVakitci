package com.rahmanyazgan.pyvakitci

import android.app.Application
import com.rahmanyazgan.pyvakitci.service.BootReceiver
import com.rahmanyazgan.pyvakitci.service.PrayerNotificationHelper
import com.rahmanyazgan.pyvakitci.util.L10n

class PyVakitciApp : Application() {
    override fun onCreate() {
        super.onCreate()
        L10n.init(this)
        PrayerNotificationHelper(this)
        try {
            BootReceiver.schedulePrayerAlarms(this)
        } catch (_: Exception) {}
    }
}
