import SwiftUI

public struct PrayerGrid: View {
    public let times: [(name: String, time: String)]
    public let activePrayer: PrayerKind
    public let surfaceColor: Color
    public let textColor: Color
    public let borderColor: Color

    public init(times: [(name: String, time: String)], activePrayer: PrayerKind, surfaceColor: Color, textColor: Color, borderColor: Color) {
        self.times = times
        self.activePrayer = activePrayer
        self.surfaceColor = surfaceColor
        self.textColor = textColor
        self.borderColor = borderColor
    }

    public var body: some View {
        VStack(spacing: 8) {
            // Row 1: İmsak, Güneş, Öğle
            HStack(spacing: 8) {
                ForEach(times.prefix(3), id: \.name) { item in
                    PrayerCard(
                        title: item.name,
                        time: item.time,
                        isActive: item.name == activePrayer.rawValue,
                        surfaceColor: surfaceColor,
                        textColor: textColor,
                        borderColor: borderColor
                    )
                }
            }
            // Row 2: İkindi, Akşam, Yatsı
            HStack(spacing: 8) {
                ForEach(times.suffix(3), id: \.name) { item in
                    PrayerCard(
                        title: item.name,
                        time: item.time,
                        isActive: item.name == activePrayer.rawValue,
                        surfaceColor: surfaceColor,
                        textColor: textColor,
                        borderColor: borderColor
                    )
                }
            }
        }
    }
}

public struct PrayerCard: View {
    public let title: String
    public let time: String
    public let isActive: Bool
    public let surfaceColor: Color
    public let textColor: Color
    public let borderColor: Color

    private let activeBlue = Color(red: 0x19 / 255.0, green: 0x76 / 255.0, blue: 0xD2 / 255.0)

    public var body: some View {
        VStack(spacing: 4) {
            HStack(spacing: 4) {
                Text(LocalizationManager.shared.prayerFromTr(title))
                    .font(.system(size: 12, weight: isActive ? .bold : .medium))
                    .foregroundColor(isActive ? activeBlue : textColor.opacity(0.7))

                if isActive {
                    Circle()
                        .fill(activeBlue)
                        .frame(width: 6, height: 6)
                }
            }

            Text(time)
                .font(.system(size: 15, weight: .bold))
                .foregroundColor(isActive ? Color.red : textColor)
        }
        .frame(maxWidth: .infinity)
        .padding(.vertical, 10)
        .padding(.horizontal, 4)
        .background(surfaceColor)
        .cornerRadius(12)
        .overlay(
            RoundedRectangle(cornerRadius: 12)
                .stroke(isActive ? activeBlue : borderColor, lineWidth: isActive ? 2 : 1)
        )
        .shadow(color: Color.black.opacity(isActive ? 0.08 : 0.03), radius: isActive ? 4 : 1, y: 1)
    }
}

