import Foundation

public struct CurrentWeather: Codable, Equatable {
    public let temperature: Double
    public let apparentTemperature: Double
    public let weatherDescription: String
    public let weatherCode: Int
    public let humidity: Int
    public let windSpeed: Double

    public init(temperature: Double, apparentTemperature: Double, weatherDescription: String, weatherCode: Int, humidity: Int, windSpeed: Double) {
        self.temperature = temperature
        self.apparentTemperature = apparentTemperature
        self.weatherDescription = weatherDescription
        self.weatherCode = weatherCode
        self.humidity = humidity
        self.windSpeed = windSpeed
    }
}

public struct DailyForecast: Identifiable, Codable, Equatable {
    public var id: String { date }
    public let date: String
    public let dayName: String
    public let minTemp: Double
    public let maxTemp: Double
    public let weatherCode: Int
    public let description: String

    public init(date: String, dayName: String, minTemp: Double, maxTemp: Double, weatherCode: Int, description: String) {
        self.date = date
        self.dayName = dayName
        self.minTemp = minTemp
        self.maxTemp = maxTemp
        self.weatherCode = weatherCode
        self.description = description
    }
}

public struct WeatherData: Codable, Equatable {
    public let current: CurrentWeather
    public let daily: [DailyForecast]

    public init(current: CurrentWeather, daily: [DailyForecast]) {
        self.current = current
        self.daily = daily
    }

    public static func defaultSample() -> WeatherData {
        WeatherData(
            current: CurrentWeather(
                temperature: 17.0,
                apparentTemperature: 16.0,
                weatherDescription: "Parçalı Bulutlu",
                weatherCode: 2,
                humidity: 65,
                windSpeed: 14.0
            ),
            daily: [
                DailyForecast(date: "2026-10-29", dayName: "Bugün", minTemp: 12.0, maxTemp: 18.0, weatherCode: 2, description: "Parçalı Bulutlu"),
                DailyForecast(date: "2026-10-30", dayName: "Cuma", minTemp: 11.0, maxTemp: 17.0, weatherCode: 3, description: "Bulutlu"),
                DailyForecast(date: "2026-10-31", dayName: "Cumartesi", minTemp: 10.0, maxTemp: 16.0, weatherCode: 61, description: "Hafif Yağmurlu"),
                DailyForecast(date: "2026-11-01", dayName: "Pazar", minTemp: 9.0, maxTemp: 15.0, weatherCode: 0, description: "Açık ve Güneşli"),
                DailyForecast(date: "2026-11-02", dayName: "Pazartesi", minTemp: 11.0, maxTemp: 18.0, weatherCode: 1, description: "Az Bulutlu")
            ]
        )
    }
}

