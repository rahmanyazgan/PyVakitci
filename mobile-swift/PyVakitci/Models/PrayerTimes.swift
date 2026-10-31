import Foundation

public enum PrayerKind: String, CaseIterable, Identifiable, Codable {
    case imsak = "İmsak"
    case gunes = "Güneş"
    case ogle = "Öğle"
    case ikindi = "İkindi"
    case aksam = "Akşam"
    case yatsi = "Yatsı"

    public var id: String { rawValue }

    public var localizedName: String {
        switch self {
        case .imsak: return LocalizationManager.shared.tr("p_imsak")
        case .gunes: return LocalizationManager.shared.tr("p_gunes")
        case .ogle: return LocalizationManager.shared.tr("p_ogle")
        case .ikindi: return LocalizationManager.shared.tr("p_ikindi")
        case .aksam: return LocalizationManager.shared.tr("p_aksam")
        case .yatsi: return LocalizationManager.shared.tr("p_yatsi")
        }
    }

    public var remainingSuffix: String {
        switch self {
        case .imsak: return LocalizationManager.shared.tr("rem_imsak")
        case .gunes: return LocalizationManager.shared.tr("rem_gunes")
        case .ogle: return LocalizationManager.shared.tr("rem_ogle")
        case .ikindi: return LocalizationManager.shared.tr("rem_ikindi")
        case .aksam: return LocalizationManager.shared.tr("rem_aksam")
        case .yatsi: return LocalizationManager.shared.tr("rem_yatsi")
        }
    }
}

public struct PrayerTimes: Codable, Identifiable, Equatable {
    public var id: String { date }
    public let date: String         // YYYY-MM-DD
    public let hijriDate: String    // "17 Cemaziyelevvel 1448"
    public let imsak: String
    public let gunes: String
    public let ogle: String
    public let ikindi: String
    public let aksam: String
    public let yatsi: String

    public init(date: String, hijriDate: String, imsak: String, gunes: String, ogle: String, ikindi: String, aksam: String, yatsi: String) {
        self.date = date
        self.hijriDate = hijriDate
        self.imsak = imsak
        self.gunes = gunes
        self.ogle = ogle
        self.ikindi = ikindi
        self.aksam = aksam
        self.yatsi = yatsi
    }

    public var asList: [(name: String, time: String)] {
        [
            ("İmsak", imsak),
            ("Güneş", gunes),
            ("Öğle", ogle),
            ("İkindi", ikindi),
            ("Akşam", aksam),
            ("Yatsı", yatsi)
        ]
    }

    public static func defaultSample() -> PrayerTimes {
        PrayerTimes(
            date: "2026-10-29",
            hijriDate: "17 Cemaziyelevvel 1448",
            imsak: "05:58",
            gunes: "07:24",
            ogle: "12:53",
            ikindi: "15:44",
            aksam: "18:12",
            yatsi: "19:32"
        )
    }
}

public struct ActivePrayerInfo: Equatable {
    public let currentPrayer: PrayerKind
    public let nextPrayer: PrayerKind
    public let remainingSeconds: Int
    public let remainingFormatted: String

    public init(currentPrayer: PrayerKind, nextPrayer: PrayerKind, remainingSeconds: Int, remainingFormatted: String) {
        self.currentPrayer = currentPrayer
        self.nextPrayer = nextPrayer
        self.remainingSeconds = remainingSeconds
        self.remainingFormatted = remainingFormatted
    }
}

