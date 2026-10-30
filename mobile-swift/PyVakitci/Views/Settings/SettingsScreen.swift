import SwiftUI

public struct SettingsScreen: View {
    @ObservedObject public var themeManager: ThemeManager
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
                Text("Bölgesel ve Genel Ayarlar")
                    .font(.system(size: 22, weight: .bold))
                    .foregroundColor(colors.text)

                // Bölgesel Konum Kartı
                VStack(alignment: .leading, spacing: 14) {
                    HStack {
                        Text("Konum Seçimi")
                            .font(.system(size: 16, weight: .bold))
                            .foregroundColor(colors.primary)
                        Spacer()
                        if isLoading {
                            Text("Yükleniyor...")
                                .font(.system(size: 11, weight: .medium))
                                .foregroundColor(colors.primary)
                        }
                    }

                    // Ülke
                    VStack(alignment: .leading, spacing: 4) {
                        Text("Ülke")
                            .font(.system(size: 11))
                            .foregroundColor(colors.text.opacity(0.6))

                        Picker("Ülke", selection: $selectedCountry) {
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
                        Text("Şehir")
                            .font(.system(size: 11))
                            .foregroundColor(colors.text.opacity(0.6))

                        Picker("Şehir", selection: $selectedCity) {
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
                        Text("İlçe")
                            .font(.system(size: 11))
                            .foregroundColor(colors.text.opacity(0.6))

                        Picker("İlçe", selection: $selectedDistrict) {
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
                            Text("Konumu Kaydet ve Vakitleri Güncelle")
                        }
                        .font(.system(size: 14, weight: .semibold))
                        .foregroundColor(.white)
                        .frame(maxWidth: .infinity)
                        .padding(.vertical, 12)
                        .background(colors.primary)
                        .cornerRadius(10)
                    }

                    if savedAlert {
                        Text("Konum başarıyla kaydedildi!")
                            .font(.system(size: 12, weight: .bold))
                            .foregroundColor(colors.primary)
                            .frame(maxWidth: .infinity, alignment: .center)
                    }
                }
                .padding(16)
                .background(colors.surface)
                .cornerRadius(16)
                .shadow(color: Color.black.opacity(0.03), radius: 3, y: 1)

                // Arayüz Modu Seçimi
                VStack(alignment: .leading, spacing: 12) {
                    Text("Arayüz Modu")
                        .font(.system(size: 16, weight: .bold))
                        .foregroundColor(colors.primary)

                    let modes = [
                        ("saat", "Kadranlı Vakit Saati (Varsayılan)"),
                        ("gokyuzu", "Dinamik Gökyüzü Modu"),
                        ("kartlar", "Modern Kartlar Modu")
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
                    Text("Renk Teması")
                        .font(.system(size: 16, weight: .bold))
                        .foregroundColor(colors.primary)

                    let themes = [
                        ("varsayilan", "Açık Tema (Varsayılan)"),
                        ("koyu", "Modern Koyu Tema"),
                        ("gece_mavisi", "Gece Mavisi"),
                        ("zumrut", "Zümrüt Yeşili"),
                        ("kehribar", "Sıcak Kehribar")
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
                Text("PyVakitci iOS v1.8 • Diyanet İşleri Başkanlığı")
                    .font(.system(size: 11))
                    .foregroundColor(colors.text.opacity(0.5))
                    .frame(maxWidth: .infinity, alignment: .center)
                    .padding(.top, 8)
            }
            .padding(16)
        }
        .background(colors.background.ignoresSafeArea())
    }
}

