package com.rahmanyazgan.pyvakitci.widget

import android.app.PendingIntent
import android.appwidget.AppWidgetManager
import android.appwidget.AppWidgetProvider
import android.content.Context
import android.content.Intent
import android.widget.RemoteViews
import com.rahmanyazgan.pyvakitci.MainActivity
import com.rahmanyazgan.pyvakitci.R
import com.rahmanyazgan.pyvakitci.data.local.AppDatabaseHelper
import com.rahmanyazgan.pyvakitci.data.local.PreferencesManager
import com.rahmanyazgan.pyvakitci.util.TimeUtil

class PrayerWidgetProvider : AppWidgetProvider() {
    override fun onUpdate(context: Context, appWidgetManager: AppWidgetManager, appWidgetIds: IntArray) {
        val prefs = PreferencesManager(context)
        val db = AppDatabaseHelper(context)
        val today = TimeUtil.getTodayString()
        val times = db.getPrayerTimesForDate(prefs.location.districtId, today)

        for (widgetId in appWidgetIds) {
            val views = RemoteViews(context.packageName, R.layout.widget_prayer)

            val loc = prefs.location
            views.setTextViewText(R.id.widget_location, "${loc.countryName} • ${loc.cityName} / ${loc.districtName}")

            if (times != null) {
                val active = TimeUtil.calculateActivePrayer(times)
                views.setTextViewText(R.id.widget_current_prayer, active.currentPrayer.title.uppercase())
                views.setTextViewText(R.id.widget_countdown, active.remainingFormatted)
                views.setTextViewText(
                    R.id.widget_times_summary,
                    "İmsak: ${times.imsak} | Güneş: ${times.gunes} | Öğle: ${times.ogle}\nİkindi: ${times.ikindi} | Akşam: ${times.aksam} | Yatsı: ${times.yatsi}"
                )
            } else {
                views.setTextViewText(R.id.widget_current_prayer, "--")
                views.setTextViewText(R.id.widget_countdown, "--:--:--")
                views.setTextViewText(R.id.widget_times_summary, "Vakitleri yüklemek için dokunun")
            }

            val intent = Intent(context, MainActivity::class.java)
            val pendingIntent = PendingIntent.getActivity(
                context, 0, intent,
                PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
            )
            views.setOnClickPendingIntent(R.id.widget_location, pendingIntent)

            appWidgetManager.updateAppWidget(widgetId, views)
        }
    }
}

