package com.rahmanyazgan.pyvakitci

import android.app.Application
import com.rahmanyazgan.pyvakitci.service.PrayerNotificationHelper

class PyVakitciApp : Application() {
    override fun onCreate() {
        super.onCreate()
        PrayerNotificationHelper(this)
    }
}

