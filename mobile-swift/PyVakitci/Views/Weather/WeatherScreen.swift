import SwiftUI

public struct WeatherScreen: View {
    public let locationName: String
    public let weatherData: WeatherData
    public let colors: AppThemeColors

    public init(locationName: String, weatherData: WeatherData, colors: AppThemeColors) {
        self.locationName = locationName
        self.weatherData = weatherData
        self.colors = colors
    }

    public var body: some View {
        ScrollView(showsIndicators: false) {
            VStack(spacing: 20) {
                // Main Current Weather Card
                VStack(spacing: 12) {
                    Text(locationName)
                        .font(.system(size: 18, weight: .semibold))
                        .foregroundColor(.white)

                    Text("\(Int(weatherData.current.temperature))°C")
                        .font(.system(size: 56, weight: .black, design: .rounded))
                        .foregroundColor(.white)

                    Text(weatherData.current.weatherDescription)
                        .font(.system(size: 16, weight: .medium))
                        .foregroundColor(.white.opacity(0.9))

                    HStack(spacing: 30) {
                        weatherItem(label: "Hissedilen", value: "\(Int(weatherData.current.apparentTemperature))°C")
                        weatherItem(label: "Nem", value: "%\(weatherData.current.humidity)")
                        weatherItem(label: "Rüzgar", value: "\(Int(weatherData.current.windSpeed)) km/s")
                    }
                    .padding(.top, 8)
                }
                .frame(maxWidth: .infinity)
                .padding(.vertical, 28)
                .background(
                    LinearGradient(
                        colors: [Color(red: 0.16, green: 0.5, blue: 0.73), Color(red: 0.43, green: 0.84, blue: 0.98)],
                        startPoint: .topLeading,
                        endPoint: .bottomTrailing
                    )
                )
                .cornerRadius(24)
                .shadow(color: Color.black.opacity(0.08), radius: 6, y: 2)

                // 5-Day Forecast
                VStack(alignment: .leading, spacing: 12) {
                    Text("5 Günlük Hava Tahmini")
                        .font(.system(size: 16, weight: .bold))
                        .foregroundColor(colors.text)

                    ForEach(weatherData.daily) { forecast in
                        HStack {
                            VStack(alignment: .leading, spacing: 2) {
                                Text(forecast.dayName)
                                    .font(.system(size: 14, weight: .bold))
                                    .foregroundColor(colors.text)

                                Text(forecast.description)
                                    .font(.system(size: 12))
                                    .foregroundColor(colors.text.opacity(0.6))
                            }

                            Spacer()

                            HStack(spacing: 4) {
                                Text("\(Int(forecast.maxTemp))°")
                                    .font(.system(size: 16, weight: .bold))
                                    .foregroundColor(colors.text)

                                Text("/ \(Int(forecast.minTemp))°")
                                    .font(.system(size: 14))
                                    .foregroundColor(colors.text.opacity(0.6))
                            }
                        }
                        .padding(14)
                        .background(colors.surface)
                        .cornerRadius(14)
                        .shadow(color: Color.black.opacity(0.02), radius: 2, y: 1)
                    }
                }
            }
            .padding(16)
        }
        .background(colors.background.ignoresSafeArea())
    }

    private func weatherItem(label: String, value: String) -> some View {
        VStack(spacing: 2) {
            Text(label)
                .font(.system(size: 11))
                .foregroundColor(.white.opacity(0.75))
            Text(value)
                .font(.system(size: 14, weight: .bold))
                .foregroundColor(.white)
        }
    }
}

