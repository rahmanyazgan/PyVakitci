import SwiftUI
import WebKit

public struct SettingsScreen: View {
    @ObservedObject public var themeManager: ThemeManager
    @ObservedObject private var loc = LocalizationManager.shared
    public let countries: [Country]
    public let cities: [City]
    public let districts: [District]
    public let onLocationSaved: (SelectedLocation) -> Void

    @State private var currentCities: [City] = []
    @State private var currentDistricts: [District] = []
    @State private var selectedCountry: Country
    @State private var selectedCity: City
    @State private var selectedDistrict: District
    @State private var savedAlert = false
    @State private var isLoading = false
    @State private var showLicenseSheet = false

    public init(
        themeManager: ThemeManager,
        countries: [Country],
        cities: [City],
        districts: [District],
        onLocationSaved: @escaping (SelectedLocation) -> Void
    ) {
        self.themeManager = themeManager
        self.countries = countries
        self.cities = cities
        self.districts = districts
        self.onLocationSaved = onLocationSaved

        let currentLoc = LocalStorageService.shared.loadLocation()
        let initCountry = countries.first(where: { $0.id == currentLoc.countryId }) ?? Country(id: "2", name: "TÜRKİYE")
        let initCity = cities.first(where: { $0.id == currentLoc.cityId }) ?? (cities.first ?? City(id: "539", name: "İSTANBUL"))
        let initDistrict = districts.first(where: { $0.id == currentLoc.districtId }) ?? (districts.first ?? District(id: "9541", name: "İSTANBUL"))

        _selectedCountry = State(initialValue: initCountry)
        _selectedCity = State(initialValue: initCity)
        _selectedDistrict = State(initialValue: initDistrict)
        _currentCities = State(initialValue: cities)
        _currentDistricts = State(initialValue: districts)
    }

    public var body: some View {
        let colors = themeManager.colors

        ScrollView(showsIndicators: false) {
            VStack(alignment: .leading, spacing: 20) {
                Text(loc.tr("s_title"))
                    .font(.system(size: 22, weight: .bold))
                    .foregroundColor(colors.text)

                // Bölgesel Konum Kartı
                VStack(alignment: .leading, spacing: 14) {
                    HStack {
                        Text(loc.tr("s_location"))
                            .font(.system(size: 16, weight: .bold))
                            .foregroundColor(colors.primary)
                        Spacer()
                        if isLoading {
                            Text(loc.tr("s_loading"))
                                .font(.system(size: 11, weight: .medium))
                                .foregroundColor(colors.primary)
                        }
                    }

                    // Ülke
                    VStack(alignment: .leading, spacing: 4) {
                        Text(loc.tr("s_country"))
                            .font(.system(size: 11))
                            .foregroundColor(colors.text.opacity(0.6))

                        Picker(loc.tr("s_country"), selection: $selectedCountry) {
                            ForEach(countries) { c in
                                Text(c.name).tag(c)
                            }
                        }
                        .pickerStyle(.menu)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding(8)
                        .background(colors.background)
                        .cornerRadius(8)
                        .onChange(of: selectedCountry) { newCountry in
                            isLoading = true
                            Task {
                                let fetchedCities = await DiyanetApiService.shared.getCities(countryId: newCountry.id)
                                let firstCity = fetchedCities.first ?? City(id: "1", name: "MERKEZ")
                                let fetchedDistricts = await DiyanetApiService.shared.getDistricts(countryId: newCountry.id, cityId: firstCity.id, cityName: firstCity.name)
                                let firstDist = fetchedDistricts.first ?? District(id: firstCity.id, name: firstCity.name)
                                await MainActor.run {
                                    self.currentCities = fetchedCities
                                    self.selectedCity = firstCity
                                    self.currentDistricts = fetchedDistricts
                                    self.selectedDistrict = firstDist
                                    self.isLoading = false
                                }
                            }
                        }
                    }

                    // Şehir
                    VStack(alignment: .leading, spacing: 4) {
                        Text(loc.tr("s_city"))
                            .font(.system(size: 11))
                            .foregroundColor(colors.text.opacity(0.6))

                        Picker(loc.tr("s_city"), selection: $selectedCity) {
                            ForEach(currentCities) { ci in
                                Text(ci.name).tag(ci)
                            }
                        }
                        .pickerStyle(.menu)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding(8)
                        .background(colors.background)
                        .cornerRadius(8)
                        .onChange(of: selectedCity) { newCity in
                            isLoading = true
                            Task {
                                let fetchedDistricts = await DiyanetApiService.shared.getDistricts(countryId: selectedCountry.id, cityId: newCity.id, cityName: newCity.name)
                                let firstDist = fetchedDistricts.first ?? District(id: newCity.id, name: newCity.name)
                                await MainActor.run {
                                    self.currentDistricts = fetchedDistricts
                                    self.selectedDistrict = firstDist
                                    self.isLoading = false
                                }
                            }
                        }
                    }

                    // İlçe
                    VStack(alignment: .leading, spacing: 4) {
                        Text(loc.tr("s_district"))
                            .font(.system(size: 11))
                            .foregroundColor(colors.text.opacity(0.6))

                        Picker(loc.tr("s_district"), selection: $selectedDistrict) {
                            ForEach(currentDistricts) { d in
                                Text(d.name).tag(d)
                            }
                        }
                        .pickerStyle(.menu)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding(8)
                        .background(colors.background)
                        .cornerRadius(8)
                    }

                    Button(action: {
                        let newLoc = SelectedLocation(
                            countryId: selectedCountry.id,
                            countryName: selectedCountry.name,
                            cityId: selectedCity.id,
                            cityName: selectedCity.name,
                            districtId: selectedDistrict.id,
                            districtName: selectedDistrict.name
                        )
                        LocalStorageService.shared.saveLocation(newLoc)
                        onLocationSaved(newLoc)
                        savedAlert = true
                    }) {
                        HStack {
                            Image(systemName: "checkmark")
                            Text(loc.tr("s_save_loc"))
                        }
                        .font(.system(size: 14, weight: .semibold))
                        .foregroundColor(.white)
                        .frame(maxWidth: .infinity)
                        .padding(.vertical, 12)
                        .background(colors.primary)
                        .cornerRadius(10)
                    }

                    if savedAlert {
                        Text(loc.tr("s_saved"))
                            .font(.system(size: 12, weight: .bold))
                            .foregroundColor(colors.primary)
                            .frame(maxWidth: .infinity, alignment: .center)
                    }
                }
                .padding(16)
                .background(colors.surface)
                .cornerRadius(16)
                .shadow(color: Color.black.opacity(0.03), radius: 3, y: 1)

                // Dil Seçimi (Language)
                VStack(alignment: .leading, spacing: 12) {
                    Text(loc.tr("s_language"))
                        .font(.system(size: 16, weight: .bold))
                        .foregroundColor(colors.primary)

                    ForEach(LocalizationManager.supportedLanguages, id: \.code) { item in
                        Button(action: {
                            loc.setLanguage(item.code)
                        }) {
                            HStack {
                                Image(systemName: loc.currentLanguage == item.code ? "largecircle.fill.circle" : "circle")
                                    .foregroundColor(loc.currentLanguage == item.code ? colors.primary : colors.text.opacity(0.4))
                                Text(item.name)
                                    .font(.system(size: 14, weight: loc.currentLanguage == item.code ? .semibold : .regular))
                                    .foregroundColor(colors.text)
                                Spacer()
                            }
                            .padding(.vertical, 4)
                        }
                    }

                    Divider()
                        .padding(.vertical, 4)

                    // Lisans Butonu (Türkçe -> GPLv3_TR.html, diğer tüm diller -> GPLv3_EN.html)
                    Button(action: {
                        showLicenseSheet = true
                    }) {
                        HStack {
                            Image(systemName: "doc.text")
                            Text(loc.tr("s_license"))
                        }
                        .font(.system(size: 14, weight: .semibold))
                        .foregroundColor(.white)
                        .frame(maxWidth: .infinity)
                        .padding(.vertical, 10)
                        .background(colors.primary)
                        .cornerRadius(10)
                    }
                }
                .padding(16)
                .background(colors.surface)
                .cornerRadius(16)
                .shadow(color: Color.black.opacity(0.03), radius: 3, y: 1)

                // Arayüz Modu Seçimi
                VStack(alignment: .leading, spacing: 12) {
                    Text(loc.tr("s_ui_mode"))
                        .font(.system(size: 16, weight: .bold))
                        .foregroundColor(colors.primary)

                    let modes = [
                        ("saat", loc.tr("mode_saat")),
                        ("gokyuzu", loc.tr("mode_gokyuzu")),
                        ("kartlar", loc.tr("mode_kartlar"))
                    ]

                    ForEach(modes, id: \.0) { item in
                        Button(action: {
                            themeManager.interfaceMode = item.0
                        }) {
                            HStack {
                                Image(systemName: themeManager.interfaceMode == item.0 ? "largecircle.fill.circle" : "circle")
                                    .foregroundColor(themeManager.interfaceMode == item.0 ? colors.primary : colors.text.opacity(0.4))
                                Text(item.1)
                                    .font(.system(size: 14))
                                    .foregroundColor(colors.text)
                                Spacer()
                            }
                            .padding(.vertical, 4)
                        }
                    }
                }
                .padding(16)
                .background(colors.surface)
                .cornerRadius(16)
                .shadow(color: Color.black.opacity(0.03), radius: 3, y: 1)

                // Renk Teması Seçimi
                VStack(alignment: .leading, spacing: 12) {
                    Text(loc.tr("s_theme"))
                        .font(.system(size: 16, weight: .bold))
                        .foregroundColor(colors.primary)

                    let themes = [
                        ("varsayilan", loc.tr("th_varsayilan")),
                        ("koyu", loc.tr("th_koyu")),
                        ("gece_mavisi", loc.tr("th_gece_mavisi")),
                        ("zumrut", loc.tr("th_zumrut")),
                        ("kehribar", loc.tr("th_kehribar"))
                    ]

                    ForEach(themes, id: \.0) { item in
                        Button(action: {
                            themeManager.currentTheme = item.0
                        }) {
                            HStack {
                                Image(systemName: themeManager.currentTheme == item.0 ? "largecircle.fill.circle" : "circle")
                                    .foregroundColor(themeManager.currentTheme == item.0 ? colors.primary : colors.text.opacity(0.4))
                                Text(item.1)
                                    .font(.system(size: 14))
                                    .foregroundColor(colors.text)
                                Spacer()
                            }
                            .padding(.vertical, 4)
                        }
                    }
                }
                .padding(16)
                .background(colors.surface)
                .cornerRadius(16)
                .shadow(color: Color.black.opacity(0.03), radius: 3, y: 1)

                // Hakkında
                Text(loc.tr("s_footer"))
                    .font(.system(size: 11))
                    .foregroundColor(colors.text.opacity(0.5))
                    .frame(maxWidth: .infinity, alignment: .center)
                    .padding(.top, 8)
            }
            .padding(16)
        }
        .background(colors.background.ignoresSafeArea())
        .sheet(isPresented: $showLicenseSheet) {
            LicenseSheetView(colors: colors, licenseFileName: loc.getLicenseFileName())
        }
    }
}

/// Uygulama içi lisans görüntüleyici (Türkçe için GPLv3_TR.html, diğer tüm diller için GPLv3_EN.html).
struct LicenseSheetView: View {
    @Environment(\.presentationMode) var presentationMode
    public let colors: AppThemeColors
    public let licenseFileName: String

    var body: some View {
        NavigationView {
            LicenseWebView(fileName: licenseFileName)
                .navigationTitle(LocalizationManager.shared.tr("s_license"))
                .navigationBarTitleDisplayMode(.inline)
                .toolbar {
                    ToolbarItem(placement: .navigationBarTrailing) {
                        Button(LocalizationManager.shared.tr("s_close")) {
                            presentationMode.wrappedValue.dismiss()
                        }
                    }
                }
        }
    }
}

struct LicenseWebView: UIViewRepresentable {
    let fileName: String

    func makeUIView(context: Context) -> WKWebView {
        let webView = WKWebView()
        let name = (fileName as NSString).deletingPathExtension
        let ext = (fileName as NSString).pathExtension
        if let url = Bundle.main.url(forResource: name, withExtension: ext) {
            webView.loadFileURL(url, allowingReadAccessTo: url.deletingLastPathComponent())
        }
        return webView
    }

    func updateUIView(_ uiView: WKWebView, context: Context) {}
}
