import Foundation

public class LocalStorageService {
    public static let shared = LocalStorageService()
    private let userDefaults = UserDefaults.standard

    private let keyLocation = "pyvakitci_saved_location"
    private let keyPrayers = "pyvakitci_cached_prayers"
    private let keyWarningSound = "pyvakitci_warning_sound"
    private let keyWarningMinutes = "pyvakitci_warning_minutes"
    private let keyVolume = "pyvakitci_volume"
    private let keyVibration = "pyvakitci_vibration"

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

    public func isPrayerSoundEnabled(prayerName: String) -> Bool {
        let key = "azan_enabled_\(prayerName.lowercased())"
        if userDefaults.object(forKey: key) == nil {
            return true
        }
        return userDefaults.bool(forKey: key)
    }

    public func setPrayerSoundEnabled(prayerName: String, enabled: Bool) {
        let key = "azan_enabled_\(prayerName.lowercased())"
        userDefaults.set(enabled, forKey: key)
    }
}

