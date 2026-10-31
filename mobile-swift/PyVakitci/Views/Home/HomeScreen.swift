import SwiftUI

public struct HomeScreen: View {
    @ObservedObject private var loc = LocalizationManager.shared
    public let prayerTimes: PrayerTimes
    public let activeInfo: ActivePrayerInfo
    public let location: SelectedLocation
    public let currentTimeStr: String
    public let interfaceMode: String
    public let colors: AppThemeColors

    public init(prayerTimes: PrayerTimes, activeInfo: ActivePrayerInfo, location: SelectedLocation, currentTimeStr: String, interfaceMode: String, colors: AppThemeColors) {
        self.prayerTimes = prayerTimes
        self.activeInfo = activeInfo
        self.location = location
        self.currentTimeStr = currentTimeStr
        self.interfaceMode = interfaceMode
        self.colors = colors
    }

    public var body: some View {
        ScrollView(showsIndicators: false) {
            VStack(spacing: 16) {
                // Top Location & Date Header
                VStack(spacing: 4) {
                    HStack {
                        VStack(alignment: .leading, spacing: 2) {
                            Text("\(location.countryName) • \(location.cityName)")
                                .font(.system(size: 12, weight: .medium))
                                .foregroundColor(colors.text.opacity(0.6))

                            Text(location.districtName)
                                .font(.system(size: 22, weight: .heavy))
                                .foregroundColor(colors.text)
                        }

                        Spacer()

                        VStack(alignment: .trailing, spacing: 2) {
                            Text(TimeUtil.getDisplayDate())
                                .font(.system(size: 13, weight: .semibold))
                                .foregroundColor(colors.text)

                            Text(prayerTimes.hijriDate)
                                .font(.system(size: 11))
                                .foregroundColor(colors.text.opacity(0.6))
                        }
                    }
                    .padding(16)
                    .background(colors.surface)
                    .cornerRadius(16)
                    .shadow(color: Color.black.opacity(0.04), radius: 3, y: 1)
                }

                // Center Mode Display
                if interfaceMode == "gokyuzu" {
                    // Dinamik Gökyüzü Modu
                    VStack(spacing: 6) {
                        Text("✦ \(activeInfo.currentPrayer.localizedName.uppercased(with: loc.currentLocale)) ✦")
                            .font(.system(size: 18, weight: .bold))
                            .foregroundColor(Color.yellow)

                        Text(currentTimeStr)
                            .font(.system(size: 42, weight: .black, design: .rounded))
                            .foregroundColor(.white)

                        Text(activeInfo.nextPrayer.remainingSuffix)
                            .font(.system(size: 14))
                            .foregroundColor(.white.opacity(0.8))

                        Text(activeInfo.remainingFormatted)
                            .font(.system(size: 22, weight: .bold, design: .monospaced))
                            .foregroundColor(Color(red: 1.0, green: 0.8, blue: 0.5))
                    }
                    .frame(maxWidth: .infinity)
                    .frame(height: 220)
                    .background(
                        LinearGradient(
                            colors: [Color(red: 0.1, green: 0.2, blue: 0.45), Color(red: 0.16, green: 0.32, blue: 0.6)],
                            startPoint: .top,
                            endPoint: .bottom
                        )
                    )
                    .cornerRadius(20)
                } else if interfaceMode == "kartlar" {
                    // Modern Kartlar Modu
                    VStack(spacing: 6) {
                        Text(loc.f("h_time_of", activeInfo.currentPrayer.localizedName.uppercased(with: loc.currentLocale)))
                            .font(.system(size: 16, weight: .bold))
                            .foregroundColor(colors.primary)

                        Text(currentTimeStr)
                            .font(.system(size: 38, weight: .heavy, design: .rounded))
                            .foregroundColor(colors.text)

                        Text("\(activeInfo.nextPrayer.remainingSuffix): \(activeInfo.remainingFormatted)")
                            .font(.system(size: 14, weight: .semibold))
                            .foregroundColor(Color.red)
                    }
                    .frame(maxWidth: .infinity)
                    .padding(.vertical, 24)
                    .background(colors.surface)
                    .cornerRadius(20)
                    .shadow(color: Color.black.opacity(0.04), radius: 4, y: 1)
                } else {
                    // Varsayılan: Kadranlı Vakit Saati
                    PrayerClockDial(
                        prayerTimes: prayerTimes,
                        activeInfo: activeInfo,
                        currentTimeStr: currentTimeStr,
                        textColor: colors.text
                    )
                    .padding(.vertical, 6)
                }

                // Prayer Grid
                PrayerGrid(
                    times: prayerTimes.asList,
                    activePrayer: activeInfo.currentPrayer,
                    surfaceColor: colors.surface,
                    textColor: colors.text,
                    borderColor: colors.cardBorder
                )

                // Source text
                Text(loc.tr("h_source"))
                    .font(.system(size: 11))
                    .foregroundColor(colors.text.opacity(0.45))
                    .padding(.top, 4)
            }
            .padding(.horizontal, 16)
            .padding(.top, 10)
            .padding(.bottom, 24)
        }
        .background(colors.background.ignoresSafeArea())
    }
}

