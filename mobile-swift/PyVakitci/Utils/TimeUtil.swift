import Foundation

public struct TimeUtil {
    private static let dateFormatter: DateFormatter = {
        let f = DateFormatter()
        f.dateFormat = "yyyy-MM-dd"
        return f
    }()

    private static let displayDateFormatter: DateFormatter = {
        let f = DateFormatter()
        f.locale = Locale(identifier: "tr_TR")
        f.dateFormat = "d MMMM yyyy"
        return f
    }()

    private static let timeFormatter: DateFormatter = {
        let f = DateFormatter()
        f.dateFormat = "HH:mm:ss"
        return f
    }()

    public static func getTodayString(date: Date = Date()) -> String {
        dateFormatter.string(from: date)
    }

    public static func getDisplayDate(date: Date = Date()) -> String {
        displayDateFormatter.string(from: date)
    }

    public static func getCurrentTimeString(date: Date = Date()) -> String {
        timeFormatter.string(from: date)
    }

    public static func timeToMinutes(_ timeStr: String) -> Int {
        let parts = timeStr.trimmingCharacters(in: .whitespaces).split(separator: ":")
        if parts.count >= 2 {
            let h = Int(parts[0]) ?? 0
            let m = Int(parts[1]) ?? 0
            return h * 60 + m
        }
        return 0
    }

    public static func timeToSeconds(_ timeStr: String) -> Int {
        let parts = timeStr.trimmingCharacters(in: .whitespaces).split(separator: ":")
        let h = parts.count > 0 ? (Int(parts[0]) ?? 0) : 0
        let m = parts.count > 1 ? (Int(parts[1]) ?? 0) : 0
        let s = parts.count > 2 ? (Int(parts[2]) ?? 0) : 0
        return h * 3600 + m * 60 + s
    }

    public static func calculateActivePrayer(prayerTimes: PrayerTimes, date: Date = Date()) -> ActivePrayerInfo {
        let cal = Calendar.current
        let nowSec = cal.component(.hour, from: date) * 3600 +
                     cal.component(.minute, from: date) * 60 +
                     cal.component(.second, from: date)

        let imsakSec = timeToSeconds(prayerTimes.imsak)
        let gunesSec = timeToSeconds(prayerTimes.gunes)
        let ogleSec = timeToSeconds(prayerTimes.ogle)
        let ikindiSec = timeToSeconds(prayerTimes.ikindi)
        let aksamSec = timeToSeconds(prayerTimes.aksam)
        let yatsiSec = timeToSeconds(prayerTimes.yatsi)

        let currentPrayer: PrayerKind
        let nextPrayer: PrayerKind
        let remainingSec: Int

        if nowSec < imsakSec {
            currentPrayer = .yatsi
            nextPrayer = .imsak
            remainingSec = imsakSec - nowSec
        } else if nowSec < gunesSec {
            currentPrayer = .imsak
            nextPrayer = .gunes
            remainingSec = gunesSec - nowSec
        } else if nowSec < ogleSec {
            currentPrayer = .gunes
            nextPrayer = .ogle
            remainingSec = ogleSec - nowSec
        } else if nowSec < ikindiSec {
            currentPrayer = .ogle
            nextPrayer = .ikindi
            remainingSec = ikindiSec - nowSec
        } else if nowSec < aksamSec {
            currentPrayer = .ikindi
            nextPrayer = .aksam
            remainingSec = aksamSec - nowSec
        } else if nowSec < yatsiSec {
            currentPrayer = .aksam
            nextPrayer = .yatsi
            remainingSec = yatsiSec - nowSec
        } else {
            currentPrayer = .yatsi
            nextPrayer = .imsak
            remainingSec = (86400 - nowSec) + imsakSec
        }

        let hours = remainingSec / 3600
        let mins = (remainingSec % 3600) / 60
        let secs = remainingSec % 60
        let formatted = String(format: "%02d:%02d:%02d", hours, mins, secs)

        return ActivePrayerInfo(
            currentPrayer: currentPrayer,
            nextPrayer: nextPrayer,
            remainingSeconds: remainingSec,
            remainingFormatted: formatted
        )
    }
}

