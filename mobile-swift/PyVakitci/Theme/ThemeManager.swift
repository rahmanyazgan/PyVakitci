import SwiftUI
import Combine

public class ThemeManager: ObservableObject {
    @AppStorage("selected_theme") public var currentTheme: String = "varsayilan"
    @AppStorage("interface_mode") public var interfaceMode: String = "saat" // "saat", "gokyuzu", "kartlar"

    public var colors: AppThemeColors {
        AppThemeColors.forTheme(currentTheme)
    }

    public init() {}
}

