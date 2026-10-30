import SwiftUI

public struct CustomTabBar: View {
    @Binding public var selectedTab: Int
    public let surfaceColor: Color
    public let primaryColor: Color
    public let textColor: Color

    public init(selectedTab: Binding<Int>, surfaceColor: Color, primaryColor: Color, textColor: Color) {
        self._selectedTab = selectedTab
        self.surfaceColor = surfaceColor
        self.primaryColor = primaryColor
        self.textColor = textColor
    }

    public var body: some View {
        HStack {
            tabItem(index: 0, title: "Vakit", iconName: "clock.fill")
            Spacer()
            tabItem(index: 1, title: "Hava", iconName: "sun.max.fill")
            Spacer()
            tabItem(index: 2, title: "Kıble", iconName: "safari.fill")
            Spacer()
            tabItem(index: 3, title: "Sesler", iconName: "music.note")
            Spacer()
            tabItem(index: 4, title: "Ayarlar", iconName: "gearshape.fill")
        }
        .padding(.horizontal, 24)
        .padding(.vertical, 10)
        .background(surfaceColor)
        .cornerRadius(20)
        .shadow(color: Color.black.opacity(0.06), radius: 6, y: -2)
    }

    private func tabItem(index: Int, title: String, iconName: String) -> some View {
        Button(action: {
            selectedTab = index
        }) {
            VStack(spacing: 4) {
                Image(systemName: iconName)
                    .font(.system(size: 20))
                Text(title)
                    .font(.system(size: 11, weight: selectedTab == index ? .bold : .regular))
            }
            .foregroundColor(selectedTab == index ? primaryColor : textColor.opacity(0.45))
        }
    }
}

