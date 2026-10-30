import Foundation

public enum PrayerSoundMode: Int, Codable, CaseIterable, Identifiable {
    case ezan = 0
    case bip = 1
    case sessiz = 2

    public var id: Int { rawValue }

    public var title: String {
        switch self {
        case .ezan: return "Ezan"
        case .bip: return "Bip"
        case .sessiz: return "Sessiz"
        }
    }
}

public class LocalStorageService {
    public static let shared = LocalStorageService()
    private let userDefaults = UserDefaults.standard

    private let keyLocation = "pyvakitci_saved_location"
    private let keyPrayers = "pyvakitci_cached_prayers"
    private let keyWarningSound = "pyvakitci_warning_sound"
    private let keyWarningMinutes = "pyvakitci_warning_minutes"
    private let keyVolume = "pyvakitci_volume"
    private let keyVibration = "pyvakitci_vibration"
    private let keyEzanDuasi = "pyvakitci_ezan_duasi"

    public func saveLocation(_ location: SelectedLocation) {
        if let data = try? JSONEncoder().encode(location) {
            userDefaults.set(data, forKey: keyLocation)
        }
    }

    public func loadLocation() -> SelectedLocation {
        if let data = userDefaults.data(forKey: keyLocation),
           let loc = try? JSONDecoder().decode(SelectedLocation.self, from: data) {
            return loc
        }
        return SelectedLocation() // Default: TÜRKİYE / İSTANBUL / İSTANBUL
    }

    public func savePrayerTimes(districtId: String, times: [PrayerTimes]) {
        let key = "\(keyPrayers)_\(districtId)"
        if let data = try? JSONEncoder().encode(times) {
            userDefaults.set(data, forKey: key)
        }
    }

    public func loadPrayerTimes(districtId: String) -> [PrayerTimes] {
        let key = "\(keyPrayers)_\(districtId)"
        if let data = userDefaults.data(forKey: key),
           let list = try? JSONDecoder().decode([PrayerTimes].self, from: data) {
            return list
        }
        return []
    }

    public func isWarningSoundEnabled() -> Bool {
        if userDefaults.object(forKey: keyWarningSound) == nil {
            return true
        }
        return userDefaults.bool(forKey: keyWarningSound)
    }

    public func setWarningSoundEnabled(_ enabled: Bool) {
        userDefaults.set(enabled, forKey: keyWarningSound)
    }

    public func getWarningMinutes() -> Int {
        let val = userDefaults.integer(forKey: keyWarningMinutes)
        return val == 0 ? 15 : val
    }

    public func setWarningMinutes(_ minutes: Int) {
        userDefaults.set(minutes, forKey: keyWarningMinutes)
    }

    public func getVolume() -> Double {
        if userDefaults.object(forKey: keyVolume) == nil {
            return 1.0
        }
        return userDefaults.double(forKey: keyVolume)
    }

    public func setVolume(_ vol: Double) {
        userDefaults.set(vol, forKey: keyVolume)
    }

    public func isVibrationEnabled() -> Bool {
        if userDefaults.object(forKey: keyVibration) == nil {
            return true
        }
        return userDefaults.bool(forKey: keyVibration)
    }

    public func setVibrationEnabled(_ enabled: Bool) {
        userDefaults.set(enabled, forKey: keyVibration)
    }

    public func isEzanDuasiEnabled() -> Bool {
        if userDefaults.object(forKey: keyEzanDuasi) == nil {
            return true
        }
        return userDefaults.bool(forKey: keyEzanDuasi)
    }

    public func setEzanDuasiEnabled(_ enabled: Bool) {
        userDefaults.set(enabled, forKey: keyEzanDuasi)
    }

    public func getPrayerSoundMode(prayerName: String) -> PrayerSoundMode {
        let key = "prayer_sound_mode_\(normalizePrayerName(prayerName))"
        if let val = userDefaults.object(forKey: key) as? Int,
           let mode = PrayerSoundMode(rawValue: val) {
            return mode
        }
        // Geriye donuk uyumluluk: eski boolean degeri kontrol et
        let legacyKey = "azan_enabled_\(normalizePrayerName(prayerName))"
        if userDefaults.object(forKey: legacyKey) != nil {
            return userDefaults.bool(forKey: legacyKey) ? .ezan : .sessiz
        }
        return .ezan
    }

    public func setPrayerSoundMode(prayerName: String, mode: PrayerSoundMode) {
        let key = "prayer_sound_mode_\(normalizePrayerName(prayerName))"
        userDefaults.set(mode.rawValue, forKey: key)
        let legacyKey = "azan_enabled_\(normalizePrayerName(prayerName))"
        userDefaults.set(mode != .sessiz, forKey: legacyKey)
    }

    public func isPrayerSoundEnabled(prayerName: String) -> Bool {
        return getPrayerSoundMode(prayerName: prayerName) != .sessiz
    }

    public func setPrayerSoundEnabled(prayerName: String, enabled: Bool) {
        setPrayerSoundMode(prayerName: prayerName, mode: enabled ? .ezan : .sessiz)
    }

    private func normalizePrayerName(_ name: String) -> String {
        return name.lowercased()
            .replacingOccurrences(of: "ı", with: "i")
            .replacingOccurrences(of: "ö", with: "o")
            .replacingOccurrences(of: "ü", with: "u")
            .replacingOccurrences(of: "ş", with: "s")
            .replacingOccurrences(of: "ç", with: "c")
            .replacingOccurrences(of: "ğ", with: "g")
    }
}

