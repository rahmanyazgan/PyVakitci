import Foundation

public struct Calverter {
    private static let islamicMonths = [
        "Muharrem", "Safer", "Rebiülevvel", "Rebiülahir",
        "Cemaziyelevvel", "Cemaziyelahir", "Recep", "Şaban",
        "Ramazan", "Şevval", "Zilkade", "Zilhicce"
    ]

    public static func getHijriDate(date: Date = Date()) -> String {
        let cal = Calendar.current
        let y = cal.component(.year, from: date)
        let m = cal.component(.month, from: date)
        let d = cal.component(.day, from: date)

        let jd = gregorianToJd(year: y, month: m, day: d)
        let (hYear, hMonth, hDay) = jdToIslamic(jd: jd)

        let mIndex = max(0, min(11, hMonth - 1))
        let monthName = islamicMonths[mIndex]
        return "\(hDay) \(monthName) \(hYear)"
    }

    private static func gregorianToJd(year: Int, month: Int, day: Int) -> Double {
        var y = Double(year)
        var m = Double(month)
        let d = Double(day)

        if m <= 2 {
            y -= 1
            m += 12
        }
        let a = floor(y / 100.0)
        let b = 2.0 - a + floor(a / 4.0)
        return floor(365.25 * (y + 4716.0)) + floor(30.6001 * (m + 1.0)) + d + b - 1524.5
    }

    private static func jdToIslamic(jd: Double) -> (year: Int, month: Int, day: Int) {
        let jdFloor = floor(jd) + 0.5
        let year = Int(floor((30.0 * (jdFloor - 1948440.0) + 10646.0) / 10631.0))
        let monthVal = ceil((jdFloor - (29.0 + islamicToJd(year: year, month: 1, day: 1))) / 29.5) + 1.0
        let month = Int(min(12.0, monthVal))
        let day = Int(jdFloor - islamicToJd(year: year, month: month, day: 1) + 1.0)
        return (year, month, day)
    }

    private static func islamicToJd(year: Int, month: Int, day: Int) -> Double {
        return Double(day) +
            ceil(29.5 * Double(month - 1)) +
            Double(year - 1) * 354.0 +
            floor((3.0 + (11.0 * Double(year))) / 30.0) +
            1948440.0 - 1.0
    }
}

