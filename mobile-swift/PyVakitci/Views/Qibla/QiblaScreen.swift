import SwiftUI

public struct QiblaScreen: View {
    @ObservedObject private var loc = LocalizationManager.shared
    public let heading: Double
    public let relativeQiblaAngle: Double
    public let isAligned: Bool
    public let hasGps: Bool
    public let latitude: Double
    public let longitude: Double
    public let locationName: String
    public let colors: AppThemeColors
    public let onRefreshLocation: () -> Void

    public init(
        heading: Double,
        relativeQiblaAngle: Double,
        isAligned: Bool,
        hasGps: Bool,
        latitude: Double,
        longitude: Double,
        locationName: String,
        colors: AppThemeColors,
        onRefreshLocation: @escaping () -> Void
    ) {
        self.heading = heading
        self.relativeQiblaAngle = relativeQiblaAngle
        self.isAligned = isAligned
        self.hasGps = hasGps
        self.latitude = latitude
        self.longitude = longitude
        self.locationName = locationName
        self.colors = colors
        self.onRefreshLocation = onRefreshLocation
    }

    private let alignedGreen = Color(red: 0.18, green: 0.55, blue: 0.22)

    public var body: some View {
        VStack(spacing: 18) {
            // Header Location Card
            VStack(spacing: 6) {
                HStack {
                    VStack(alignment: .leading, spacing: 2) {
                        Text(loc.tr("q_title"))
                            .font(.system(size: 18, weight: .bold))
                            .foregroundColor(colors.text)

                        Text(locationName)
                            .font(.system(size: 13))
                            .foregroundColor(colors.text.opacity(0.65))
                    }

                    Spacer()

                    HStack(spacing: 4) {
                        Image(systemName: hasGps ? "location.fill" : "location")
                            .font(.system(size: 14))
                            .foregroundColor(hasGps ? alignedGreen : colors.primary)
                        Text(hasGps ? loc.tr("q_live_gps") : loc.tr("q_saved_loc"))
                            .font(.system(size: 11, weight: .semibold))
                            .foregroundColor(hasGps ? alignedGreen : colors.primary)
                    }
                }

                Text(loc.f("q_coord", latitude, longitude))
                    .font(.system(size: 11))
                    .foregroundColor(colors.text.opacity(0.45))
            }
            .frame(maxWidth: .infinity)
            .padding(14)
            .background(colors.surface)
            .cornerRadius(16)
            .shadow(color: Color.black.opacity(0.03), radius: 3, y: 1)

            // Aligned Badge
            if isAligned {
                HStack {
                    Text(loc.tr("q_banner"))
                        .font(.system(size: 14, weight: .bold))
                        .foregroundColor(.white)
                }
                .frame(maxWidth: .infinity)
                .padding(.vertical, 8)
                .background(alignedGreen)
                .cornerRadius(12)
                .transition(.scale)
            }

            Spacer()

            // Compass View
            ZStack {
                // Compass Dial
                Circle()
                    .stroke(isAligned ? alignedGreen : colors.cardBorder, lineWidth: isAligned ? 4 : 2.5)
                    .frame(width: 270, height: 270)
                    .background(Circle().fill(colors.surface))

                // Compass Rose Ticks (Rotates with device heading)
                ForEach(0..<12) { i in
                    Rectangle()
                        .fill(i == 0 ? Color.red : colors.text.opacity(0.35))
                        .frame(width: i % 3 == 0 ? 3 : 1.5, height: i % 3 == 0 ? 14 : 8)
                        .offset(y: -125)
                        .rotationEffect(.degrees(Double(i) * 30.0 - heading))
                }

                // Kaaba Needle (Always points to Kaaba in real world)
                VStack(spacing: 0) {
                    Circle()
                        .fill(isAligned ? alignedGreen : Color(red: 0.18, green: 0.55, blue: 0.22))
                        .frame(width: 20, height: 20)
                        .overlay(Circle().fill(Color.yellow).frame(width: 10, height: 10))

                    Rectangle()
                        .fill(isAligned ? alignedGreen : Color.yellow)
                        .frame(width: 4.5, height: 100)

                    Spacer().frame(height: 100)
                }
                .rotationEffect(.degrees(relativeQiblaAngle))
                .animation(.easeOut(duration: 0.15), value: relativeQiblaAngle)

                // Center Pin
                Circle()
                    .fill(isAligned ? alignedGreen : colors.primary)
                    .frame(width: 14, height: 14)
            }
            .frame(width: 270, height: 270)

            Spacer()

            // Direction Guidance Card
            VStack(spacing: 4) {
                Text(isAligned ? loc.tr("q_on_dir") : loc.f("q_angle", Int(relativeQiblaAngle)))
                    .font(.system(size: 20, weight: .black))
                    .foregroundColor(isAligned ? alignedGreen : colors.text)

                Text(isAligned ? loc.tr("q_desc_aligned") : loc.tr("q_desc_turn"))
                    .font(.system(size: 12))
                    .foregroundColor(colors.text.opacity(0.7))
            }
            .frame(maxWidth: .infinity)
            .padding(14)
            .background(isAligned ? Color(red: 0.93, green: 0.98, blue: 0.93) : colors.surface)
            .cornerRadius(16)
            .shadow(color: Color.black.opacity(0.03), radius: 3, y: 1)

            // Refresh GPS Button
            Button(action: onRefreshLocation) {
                HStack(spacing: 6) {
                    Image(systemName: "location.fill")
                    Text(loc.tr("q_verify_gps"))
                }
                .font(.system(size: 13, weight: .semibold))
                .foregroundColor(colors.primary)
                .frame(maxWidth: .infinity)
                .padding(.vertical, 12)
                .background(colors.surface)
                .cornerRadius(12)
                .overlay(RoundedRectangle(cornerRadius: 12).stroke(colors.cardBorder, lineWidth: 1))
            }
        }
        .padding(16)
        .background(colors.background.ignoresSafeArea())
    }
}
