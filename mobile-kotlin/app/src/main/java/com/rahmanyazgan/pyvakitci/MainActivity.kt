package com.rahmanyazgan.pyvakitci

import android.Manifest
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableDoubleStateOf
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import com.rahmanyazgan.pyvakitci.data.local.AppDatabaseHelper
import com.rahmanyazgan.pyvakitci.data.local.PreferencesManager
import com.rahmanyazgan.pyvakitci.data.model.City
import com.rahmanyazgan.pyvakitci.data.model.Country
import com.rahmanyazgan.pyvakitci.data.model.CurrentWeather
import com.rahmanyazgan.pyvakitci.data.model.District
import com.rahmanyazgan.pyvakitci.data.model.PrayerTimes
import com.rahmanyazgan.pyvakitci.data.model.WeatherData
import com.rahmanyazgan.pyvakitci.data.remote.DiyanetApiService
import com.rahmanyazgan.pyvakitci.data.remote.WeatherApiService
import com.rahmanyazgan.pyvakitci.sensor.QiblaCompassManager
import com.rahmanyazgan.pyvakitci.service.AudioPlayerManager
import com.rahmanyazgan.pyvakitci.service.BootReceiver
import com.rahmanyazgan.pyvakitci.service.PrayerNotificationHelper
import com.rahmanyazgan.pyvakitci.widget.PrayerWidgetProvider
import com.rahmanyazgan.pyvakitci.ui.components.BottomNavBar
import com.rahmanyazgan.pyvakitci.ui.screens.AudioSettingsScreen
import com.rahmanyazgan.pyvakitci.ui.screens.HomeScreen
import com.rahmanyazgan.pyvakitci.ui.screens.QiblaScreen
import com.rahmanyazgan.pyvakitci.ui.screens.SettingsScreen
import com.rahmanyazgan.pyvakitci.ui.screens.WeatherScreen
import com.rahmanyazgan.pyvakitci.ui.theme.PyVakitciTheme
import com.rahmanyazgan.pyvakitci.ui.theme.getAppColors
import com.rahmanyazgan.pyvakitci.util.TimeUtil
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import java.util.Calendar
import java.util.Locale

class MainActivity : ComponentActivity() {

    private lateinit var prefs: PreferencesManager
    private lateinit var dbHelper: AppDatabaseHelper
    private lateinit var diyanetApi: DiyanetApiService
    private lateinit var weatherApi: WeatherApiService
    private lateinit var audioPlayer: AudioPlayerManager
    private var compassManager: QiblaCompassManager? = null
    private var currentSelectedTab: Int = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        prefs = PreferencesManager(this)
        dbHelper = AppDatabaseHelper(this)
        diyanetApi = DiyanetApiService()
        weatherApi = WeatherApiService()
        audioPlayer = AudioPlayerManager(this)

        try {
            BootReceiver.schedulePrayerAlarms(this)
        } catch (_: Exception) {}

        setContent {
            var selectedTheme by remember { mutableStateOf(prefs.theme) }
            var selectedMode by remember { mutableStateOf(prefs.interfaceMode) }
            var selectedTab by remember { mutableIntStateOf(0) }
            currentSelectedTab = selectedTab

            val appColors = getAppColors(selectedTheme)

            // Current Time & Remaining
            var currentTimeStr by remember { mutableStateOf(TimeUtil.getCurrentTimeString()) }
            var prayerTimes by remember {
                val today = TimeUtil.getTodayString()
                val cached = dbHelper.getPrayerTimesForDate(prefs.location.districtId, today)
                mutableStateOf(cached ?: PrayerTimes.defaultSample())
            }

            var activePrayerInfo by remember {
                mutableStateOf(TimeUtil.calculateActivePrayer(prayerTimes))
            }

            // Weather state
            var weatherData by remember {
                mutableStateOf(
                    WeatherData(
                        current = CurrentWeather(17.0, 16.0, "Parçalı Bulutlu", 2, 65, 14.0),
                        daily = emptyList()
                    )
                )
            }

            // Compass State (Sadece Kıble sekmesinde aktif)
            var azimuth by remember { mutableFloatStateOf(0f) }
            var qiblaAngle by remember { mutableFloatStateOf(0f) }
            var isAligned by remember { mutableStateOf(false) }
            var hasGps by remember { mutableStateOf(false) }
            var currentLat by remember { mutableDoubleStateOf(prefs.location.latitude) }
            var currentLon by remember { mutableDoubleStateOf(prefs.location.longitude) }

            // Permissions launcher (Location & Notification)
            val permissionLauncher = rememberLauncherForActivityResult(
                ActivityResultContracts.RequestMultiplePermissions()
            ) { perms ->
                val granted = perms[Manifest.permission.ACCESS_FINE_LOCATION] == true ||
                              perms[Manifest.permission.ACCESS_COARSE_LOCATION] == true
                if (granted && selectedTab == 2) {
                    compassManager?.requestLocationUpdates()
                }
            }

            LaunchedEffect(Unit) {
                val perms = mutableListOf(
                    Manifest.permission.ACCESS_FINE_LOCATION,
                    Manifest.permission.ACCESS_COARSE_LOCATION
                )
                if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
                    perms.add(Manifest.permission.POST_NOTIFICATIONS)
                }
                permissionLauncher.launch(perms.toTypedArray())
            }

            // Location lists
            var countries by remember { mutableStateOf<List<Country>>(emptyList()) }
            var cities by remember { mutableStateOf<List<City>>(emptyList()) }
            var districts by remember { mutableStateOf<List<District>>(emptyList()) }

            val scope = rememberCoroutineScope()

            // Açılışta veya dakika içinde 1 defa anlık uyarı / ezan tetikleyici
            var lastTriggeredPrayerMinute by remember { mutableStateOf<String?>(null) }
            var lastTriggeredWarningMinute by remember { mutableStateOf<String?>(null) }

            // 1-Saniyelik Saat Döngüsü ve Anlık Vakit / Uyarı Kontrolü
            LaunchedEffect(prayerTimes, prefs.isWarningSoundEnabled, prefs.warningMinutes) {
                while (true) {
                    val now = Calendar.getInstance()
                    currentTimeStr = TimeUtil.getCurrentTimeString()
                    activePrayerInfo = TimeUtil.calculateActivePrayer(prayerTimes, now)

                    val currentHourMinute = String.format(Locale.US, "%02d:%02d", now.get(Calendar.HOUR_OF_DAY), now.get(Calendar.MINUTE))
                    val currentMinuteTotal = now.get(Calendar.HOUR_OF_DAY) * 60 + now.get(Calendar.MINUTE)

                    val prayers = listOf(
                        "İmsak" to prayerTimes.imsak,
                        "Öğle" to prayerTimes.ogle,
                        "İkindi" to prayerTimes.ikindi,
                        "Akşam" to prayerTimes.aksam,
                        "Yatsı" to prayerTimes.yatsi
                    )

                    // 1) Ezan / Vakit Giriş Kontrolü (O dakika içinde en az 1 kere)
                    for ((name, timeStr) in prayers) {
                        if (timeStr == currentHourMinute) {
                            val triggerKey = "${TimeUtil.getTodayString()}_${currentHourMinute}_$name"
                            val prayerKey = prefs.normalizePrayerKey(name)
                            val prayerTag = "ezan_$prayerKey"

                            if (AudioPlayerManager.isAnyAudioPlaying() &&
                                (AudioPlayerManager.currentPlayingTag == prayerTag || AudioPlayerManager.currentPlayingTag == "ezan_duasi")) {
                                lastTriggeredPrayerMinute = triggerKey
                            } else if (lastTriggeredPrayerMinute != triggerKey &&
                                       AudioPlayerManager.lastTriggeredKey != triggerKey &&
                                       !AudioPlayerManager.isAnyAudioPlaying()) {
                                lastTriggeredPrayerMinute = triggerKey
                                AudioPlayerManager.lastTriggeredKey = triggerKey
                                PrayerWidgetProvider.updateAllWidgets(this@MainActivity)
                                if (prefs.isPrayerSoundEnabled(prayerKey)) {
                                    val loc = prefs.location
                                    PrayerNotificationHelper(this@MainActivity)
                                        .showPrayerNotification(name, timeStr, "${loc.cityName} / ${loc.districtName}")

                                    val sound = prefs.getPrayerSound(prayerKey)
                                    audioPlayer.onCompletionListener = {
                                        if (prefs.isPrayerSoundEnabled("dua")) {
                                            audioPlayer.playAudio(prefs.getPrayerSound("dua"), prefs.volume, tag = "ezan_duasi")
                                        }
                                    }
                                    audioPlayer.playAudio(sound, prefs.volume, tag = prayerTag)
                                }
                            }
                        }
                    }

                    // 2) Erken Uyarı Kontrolü (15 dk önce o dakika içinde 1 kere)
                    if (prefs.isWarningSoundEnabled && prefs.warningMinutes > 0) {
                        for ((name, timeStr) in prayers) {
                            val pMinutes = TimeUtil.timeToMinutes(timeStr)
                            val targetWarnMinute = pMinutes - prefs.warningMinutes
                            if (targetWarnMinute == currentMinuteTotal) {
                                val warnTriggerKey = "${TimeUtil.getTodayString()}_${currentHourMinute}_warn_$name"
                                val warnTag = "uyari_$name"

                                if (AudioPlayerManager.isAnyAudioPlaying() && AudioPlayerManager.currentPlayingTag == warnTag) {
                                    lastTriggeredWarningMinute = warnTriggerKey
                                } else if (lastTriggeredWarningMinute != warnTriggerKey &&
                                           AudioPlayerManager.lastTriggeredKey != warnTriggerKey &&
                                           !AudioPlayerManager.isAnyAudioPlaying()) {
                                    lastTriggeredWarningMinute = warnTriggerKey
                                    AudioPlayerManager.lastTriggeredKey = warnTriggerKey
                                    PrayerNotificationHelper(this@MainActivity)
                                        .showWarningNotification(name, prefs.warningMinutes)

                                    val warnSound = prefs.getPrayerSound("uyari")
                                    audioPlayer.playAudio(warnSound, prefs.volume, tag = warnTag)
                                }
                            }
                        }
                    }

                    delay(1000)
                }
            }

            // Load initial data (countries, weather, prayer times)
            LaunchedEffect(prefs.location.districtId) {
                scope.launch {
                    countries = diyanetApi.getCountries()
                    cities = diyanetApi.getCities(prefs.location.countryId)
                    districts = diyanetApi.getDistricts(prefs.location.countryId, prefs.location.cityId, prefs.location.cityName)

                    val fetched = diyanetApi.fetchMonthlyTimes(prefs.location.districtId)
                    if (fetched.isNotEmpty()) {
                        dbHelper.savePrayerTimes(prefs.location.districtId, fetched)
                    }
                    val today = TimeUtil.getTodayString()
                    val todayTimes = dbHelper.getPrayerTimesForDate(prefs.location.districtId, today)
                    if (todayTimes != null) {
                        prayerTimes = todayTimes
                        activePrayerInfo = TimeUtil.calculateActivePrayer(todayTimes)
                    }
                    BootReceiver.schedulePrayerAlarms(this@MainActivity)
                    PrayerWidgetProvider.updateAllWidgets(this@MainActivity)

                    weatherData = weatherApi.fetchWeather(prefs.location.latitude, prefs.location.longitude)
                }
            }

            // SADECE Kıble sekmesine (sekme 2) girildiğinde Pusulayı ve GPS'i çalıştır
            LaunchedEffect(selectedTab, prefs.location) {
                if (selectedTab == 2) {
                    compassManager?.stop()
                    compassManager = QiblaCompassManager(this@MainActivity) { az, qibla, aligned, gps ->
                        azimuth = az
                        qiblaAngle = qibla
                        isAligned = aligned
                        hasGps = gps
                        compassManager?.let {
                            currentLat = it.currentLatitude
                            currentLon = it.currentLongitude
                        }
                    }.apply {
                        currentLatitude = prefs.location.latitude
                        currentLongitude = prefs.location.longitude
                        start()
                    }
                } else {
                    compassManager?.stop()
                    compassManager = null
                }
            }

            PyVakitciTheme(themeKey = selectedTheme) {
                Scaffold(
                    bottomBar = {
                        BottomNavBar(
                            selectedTab = selectedTab,
                            onTabSelected = { selectedTab = it },
                            containerColor = appColors.surface,
                            selectedColor = appColors.primary,
                            unselectedColor = appColors.text
                        )
                    }
                ) { innerPadding ->
                    when (selectedTab) {
                        0 -> HomeScreen(
                            prayerTimes = prayerTimes,
                            activeInfo = activePrayerInfo,
                            location = prefs.location,
                            currentTimeStr = currentTimeStr,
                            interfaceMode = selectedMode,
                            colors = appColors,
                            modifier = Modifier.padding(innerPadding)
                        )
                        1 -> WeatherScreen(
                            locationName = "${prefs.location.cityName} / ${prefs.location.districtName}",
                            weatherData = weatherData,
                            colors = appColors,
                            modifier = Modifier.padding(innerPadding)
                        )
                        2 -> QiblaScreen(
                            azimuth = azimuth,
                            qiblaAngle = qiblaAngle,
                            isAligned = isAligned,
                            hasGps = hasGps,
                            latitude = currentLat,
                            longitude = currentLon,
                            locationName = if (hasGps) "Canlı GPS Konumu" else "${prefs.location.cityName} / ${prefs.location.districtName}",
                            colors = appColors,
                            onRefreshLocation = {
                                compassManager?.requestLocationUpdates()
                            },
                            modifier = Modifier.padding(innerPadding)
                        )
                        3 -> AudioSettingsScreen(
                            prefs = prefs,
                            audioPlayer = audioPlayer,
                            colors = appColors,
                            modifier = Modifier.padding(innerPadding)
                        )
                        4 -> SettingsScreen(
                            prefs = prefs,
                            countries = countries,
                            cities = cities,
                            districts = districts,
                            colors = appColors,
                            diyanetApi = diyanetApi,
                            onSaveLocation = { newLoc ->
                                prefs.location = newLoc
                                scope.launch {
                                    val fetched = diyanetApi.fetchMonthlyTimes(newLoc.districtId)
                                    dbHelper.savePrayerTimes(newLoc.districtId, fetched)
                                    val today = TimeUtil.getTodayString()
                                    val todayTimes = dbHelper.getPrayerTimesForDate(newLoc.districtId, today)
                                    if (todayTimes != null) {
                                        prayerTimes = todayTimes
                                        activePrayerInfo = TimeUtil.calculateActivePrayer(todayTimes)
                                    }
                                    weatherData = weatherApi.fetchWeather(newLoc.latitude, newLoc.longitude)
                                    BootReceiver.schedulePrayerAlarms(this@MainActivity)
                                    PrayerWidgetProvider.updateAllWidgets(this@MainActivity)
                                }
                            },
                            onThemeChanged = { newTheme ->
                                selectedTheme = newTheme
                            },
                            onInterfaceModeChanged = { newMode ->
                                selectedMode = newMode
                            },
                            modifier = Modifier.padding(innerPadding)
                        )
                    }
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        if (currentSelectedTab == 2) {
            compassManager?.start()
        }
    }

    override fun onPause() {
        super.onPause()
        compassManager?.stop()
        if (AudioPlayerManager.currentPlayingTag?.startsWith("preview_") == true) {
            audioPlayer.stopSound()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        compassManager?.stop()
        if (AudioPlayerManager.currentPlayingTag?.startsWith("preview_") == true) {
            audioPlayer.stopSound()
        }
    }
}
