import SwiftUI

@main
struct PyVakitciApp: App {
    @StateObject private var themeManager = ThemeManager()
    @StateObject private var compassService = QiblaCompassService.shared
    @StateObject private var audioPlayer = AudioPlayerService.shared

    @State private var selectedTab = 0
    @State private var location: SelectedLocation = LocalStorageService.shared.loadLocation()

    @State private var prayerTimes: PrayerTimes = PrayerTimes.defaultSample()
    @State private var activePrayerInfo: ActivePrayerInfo = TimeUtil.calculateActivePrayer(prayerTimes: PrayerTimes.defaultSample())
    @State private var currentTimeStr: String = TimeUtil.getCurrentTimeString()
    @State private var currentDayString: String = TimeUtil.getTodayString()

    @State private var weatherData: WeatherData = WeatherData.defaultSample()

    @State private var countries: [Country] = []
    @State private var cities: [City] = []
    @State private var districts: [District] = []

    private let timer = Timer.publish(every: 1.0, on: .main, in: .common).autoconnect()

    var body: some Scene {
        WindowGroup {
            let colors = themeManager.colors

            ZStack(alignment: .bottom) {
                Group {
                    switch selectedTab {
                    case 0:
                        HomeScreen(
                            prayerTimes: prayerTimes,
                            activeInfo: activePrayerInfo,
                            location: location,
                            currentTimeStr: currentTimeStr,
                            interfaceMode: themeManager.interfaceMode,
                            colors: colors
                        )
                    case 1:
                        WeatherScreen(
                            locationName: "\(location.cityName) / \(location.districtName)",
                            weatherData: weatherData,
                            colors: colors
                        )
                    case 2:
                        QiblaScreen(
                            heading: compassService.heading,
                            relativeQiblaAngle: compassService.relativeQiblaAngle,
                            isAligned: compassService.isAligned,
                            hasGps: compassService.hasGpsFix,
                            latitude: compassService.currentLatitude,
                            longitude: compassService.currentLongitude,
                            locationName: compassService.hasGpsFix ? "Canlı GPS Konumu" : "\(location.cityName) / \(location.districtName)",
                            colors: colors,
                            onRefreshLocation: {
                                compassService.refreshLocation()
                            }
                        )
                    case 3:
                        AudioSettingsScreen(colors: colors)
                    case 4:
                        SettingsScreen(
                            themeManager: themeManager,
                            countries: countries,
                            cities: cities,
                            districts: districts,
                            onLocationSaved: { newLoc in
                                self.location = newLoc
                                self.loadData()
                            }
                        )
                    default:
                        EmptyView()
                    }
                }
                .frame(maxWidth: .infinity, maxHeight: .infinity)
                .padding(.bottom, 64)

                CustomTabBar(
                    selectedTab: $selectedTab,
                    surfaceColor: colors.surface,
                    primaryColor: colors.primary,
                    textColor: colors.text
                )
                .padding(.horizontal, 16)
                .padding(.bottom, 8)
            }
            .background(colors.background.ignoresSafeArea())
            .onReceive(timer) { _ in
                let now = Date()
                self.currentTimeStr = TimeUtil.getCurrentTimeString(date: now)
                self.activePrayerInfo = TimeUtil.calculateActivePrayer(prayerTimes: prayerTimes, date: now)

                // Gece yarisi tarih degisimi kontrolu
                let today = TimeUtil.getTodayString(date: now)
                if today != currentDayString {
                    self.currentDayString = today
                    let cached = LocalStorageService.shared.loadPrayerTimes(districtId: location.districtId)
                    if let todayCached = cached.first(where: { $0.date == today }) {
                        self.prayerTimes = todayCached
                        self.activePrayerInfo = TimeUtil.calculateActivePrayer(prayerTimes: todayCached, date: now)
                    }
                    // Yeni gun baslayinca 6 gunluk bildirim kuyrugunu guncelle
                    NotificationService.shared.schedulePrayerNotifications(
                        timesList: cached,
                        locationText: "\(location.cityName) / \(location.districtName)"
                    )
                }
            }
            .onAppear {
                NotificationService.shared.requestAuthorization()
                compassService.currentLatitude = location.latitude
                compassService.currentLongitude = location.longitude
                compassService.start()
                loadData()
            }
        }
    }

    private func loadData() {
        compassService.currentLatitude = location.latitude
        compassService.currentLongitude = location.longitude

        // Once onbellekteki vakitleri yukle ve bildirimleri hemen kur
        let cached = LocalStorageService.shared.loadPrayerTimes(districtId: location.districtId)
        let today = TimeUtil.getTodayString()
        if let todayCached = cached.first(where: { $0.date == today }) {
            self.prayerTimes = todayCached
            self.activePrayerInfo = TimeUtil.calculateActivePrayer(prayerTimes: todayCached)
        }
        if !cached.isEmpty {
            NotificationService.shared.schedulePrayerNotifications(
                timesList: cached,
                locationText: "\(location.cityName) / \(location.districtName)"
            )
        }

        Task {
            // Bolgesel hiyerarsiyi yukle
            let cList = await DiyanetApiService.shared.getCountries()
            let ciList = await DiyanetApiService.shared.getCities(countryId: location.countryId)
            let dList = await DiyanetApiService.shared.getDistricts(countryId: location.countryId, cityId: location.cityId, cityName: location.cityName)

            await MainActor.run {
                self.countries = cList
                self.cities = ciList
                self.districts = dList
            }

            // Guncel aylik vakitleri cek
            let times = await DiyanetApiService.shared.fetchMonthlyPrayerTimes(districtId: location.districtId)
            if !times.isEmpty {
                LocalStorageService.shared.savePrayerTimes(districtId: location.districtId, times: times)
                if let todayTime = times.first(where: { $0.date == today }) {
                    await MainActor.run {
                        self.prayerTimes = todayTime
                        self.activePrayerInfo = TimeUtil.calculateActivePrayer(prayerTimes: todayTime)
                        // Cok gunluk bildirimleri kur
                        NotificationService.shared.schedulePrayerNotifications(
                            timesList: times,
                            locationText: "\(location.cityName) / \(location.districtName)"
                        )
                    }
                }
            }

            // Hava durumunu cek
            let w = await WeatherApiService.shared.fetchWeather(latitude: location.latitude, longitude: location.longitude)
            await MainActor.run {
                self.weatherData = w
            }
        }
    }
}

