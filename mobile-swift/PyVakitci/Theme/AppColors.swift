import SwiftUI

public struct AppThemeColors {
    public let background: Color
    public let surface: Color
    public let primary: Color
    public let text: Color
    public let cardBorder: Color

    public static let arcImsak = Color(red: 0x4A / 255.0, green: 0x90 / 255.0, blue: 0xE2 / 255.0)
    public static let arcGunes = Color(red: 0xF5 / 255.0, green: 0xA6 / 255.0, blue: 0x23 / 255.0)
    public static let arcOgle = Color(red: 0x50 / 255.0, green: 0xE3 / 255.0, blue: 0xC2 / 255.0)
    public static let arcIkindi = Color(red: 0xF8 / 255.0, green: 0xE7 / 255.0, blue: 0x1C / 255.0)
    public static let arcAksam = Color(red: 0xBD / 255.0, green: 0x10 / 255.0, blue: 0xE0 / 255.0)
    public static let arcYatsi = Color(red: 0x90 / 255.0, green: 0x13 / 255.0, blue: 0xFE / 255.0)
    public static let needleRed = Color(red: 0xE9 / 255.0, green: 0x4B / 255.0, blue: 0x3C / 255.0)

    public static func forTheme(_ themeKey: String) -> AppThemeColors {
        switch themeKey.lowercased() {
        case "koyu":
            return AppThemeColors(
                background: Color(red: 0x1E / 255.0, green: 0x1E / 255.0, blue: 0x2E / 255.0),
                surface: Color(red: 0x2A / 255.0, green: 0x2A / 255.0, blue: 0x3C / 255.0),
                primary: Color(red: 0x7A / 255.0, green: 0xA2 / 255.0, blue: 0xF7 / 255.0),
                text: Color(red: 0xEC / 255.0, green: 0xEF / 255.0, blue: 0xF4 / 255.0),
                cardBorder: Color(red: 0x3B / 255.0, green: 0x42 / 255.0, blue: 0x61 / 255.0)
            )
        case "gece_mavisi":
            return AppThemeColors(
                background: Color(red: 0x0D / 255.0, green: 0x1B / 255.0, blue: 0x2A / 255.0),
                surface: Color(red: 0x1B / 255.0, green: 0x26 / 255.0, blue: 0x3B / 255.0),
                primary: Color(red: 0x41 / 255.0, green: 0x5A / 255.0, blue: 0x77 / 255.0),
                text: Color(red: 0xE0 / 255.0, green: 0xE1 / 255.0, blue: 0xDD / 255.0),
                cardBorder: Color(red: 0x41 / 255.0, green: 0x5A / 255.0, blue: 0x77 / 255.0)
            )
        case "zumrut":
            return AppThemeColors(
                background: Color(red: 0x0F / 255.0, green: 0x20 / 255.0, blue: 0x27 / 255.0),
                surface: Color(red: 0x20 / 255.0, green: 0x3A / 255.0, blue: 0x43 / 255.0),
                primary: Color(red: 0x2C / 255.0, green: 0x53 / 255.0, blue: 0x64 / 255.0),
                text: Color(red: 0xE0 / 255.0, green: 0xF2 / 255.0, blue: 0xF1 / 255.0),
                cardBorder: Color(red: 0x2C / 255.0, green: 0x53 / 255.0, blue: 0x64 / 255.0)
            )
        case "kehribar":
            return AppThemeColors(
                background: Color(red: 0x2C / 255.0, green: 0x1E / 255.0, blue: 0x14 / 255.0),
                surface: Color(red: 0x3D / 255.0, green: 0x2C / 255.0, blue: 0x1E / 255.0),
                primary: Color(red: 0xD3 / 255.0, green: 0x54 / 255.0, blue: 0x00 / 255.0),
                text: Color(red: 0xFD / 255.0, green: 0xEB / 255.0, blue: 0xD0 / 255.0),
                cardBorder: Color(red: 0xE6 / 255.0, green: 0x7E / 255.0, blue: 0x22 / 255.0)
            )
        default:
            // "varsayilan" (Açık)
            return AppThemeColors(
                background: Color(red: 0xF6 / 255.0, green: 0xF8 / 255.0, blue: 0xFA / 255.0),
                surface: Color.white,
                primary: Color(red: 0x00 / 255.0, green: 0x7A / 255.0, blue: 0xCC / 255.0),
                text: Color(red: 0x24 / 255.0, green: 0x29 / 255.0, blue: 0x2E / 255.0),
                cardBorder: Color(red: 0xE1 / 255.0, green: 0xE4 / 255.0, blue: 0xE8 / 255.0)
            )
        }
    }
}

