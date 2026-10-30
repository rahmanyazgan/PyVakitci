import Foundation

public class WeatherApiService {
    public static let shared = WeatherApiService()
    private let session: URLSession

    public init() {
        let config = URLSessionConfiguration.default
        config.timeoutIntervalForRequest = 10
        self.session = URLSession(configuration: config)
    }

    public func fetchWeather(latitude: Double, longitude: Double) async -> WeatherData {
        let urlStr = "https://api.open-meteo.com/v1/forecast?latitude=\(latitude)&longitude=\(longitude)&current=temperature_2m,relative_humidity_2m,apparent_temperature,weather_code,wind_speed_10m&daily=weather_code,temperature_2m_max,temperature_2m_min&timezone=auto"
        guard let url = URL(string: urlStr) else {
            return WeatherData.defaultSample()
        }

        do {
            let (data, response) = try await session.data(from: url)
            if let httpResp = response as? HTTPURLResponse, httpResp.statusCode == 200,
               let json = try? JSONSerialization.jsonObject(with: data) as? [String: Any] {

                let currentObj = json["current"] as? [String: Any]
                let temp = currentObj?["temperature_2m"] as? Double ?? 16.5
                let appTemp = currentObj?["apparent_temperature"] as? Double ?? 15.8
                let humidity = currentObj?["relative_humidity_2m"] as? Int ?? 68
                let wCode = currentObj?["weather_code"] as? Int ?? 1
                let wind = currentObj?["wind_speed_10m"] as? Double ?? 12.0
                let desc = getWeatherDescription(wCode)

                var dailyList: [DailyForecast] = []
                if let dailyObj = json["daily"] as? [String: Any],
                   let times = dailyObj["time"] as? [String],
                   let maxs = dailyObj["temperature_2m_max"] as? [Double],
                   let mins = dailyObj["temperature_2m_min"] as? [Double],
                   let codes = dailyObj["weather_code"] as? [Int] {

                    let count = min(times.count, min(maxs.count, min(mins.count, codes.count)))
                    let df = DateFormatter()
                    df.dateFormat = "yyyy-MM-dd"
                    let dayFormat = DateFormatter()
                    dayFormat.locale = Locale(identifier: "tr_TR")
                    dayFormat.dateFormat = "EEEE"

                    for i in 0..<min(count, 5) {
                        let dStr = times[i]
                        let minT = mins[i]
                        let maxT = maxs[i]
                        let code = codes[i]

                        var dayName = dStr
                        if let d = df.date(from: dStr) {
                            dayName = dayFormat.string(from: d).capitalized
                        }

                        dailyList.append(DailyForecast(
                            date: dStr,
                            dayName: dayName,
                            minTemp: minT,
                            maxTemp: maxT,
                            weatherCode: code,
                            description: getWeatherDescription(code)
                        ))
                    }
                }

                return WeatherData(
                    current: CurrentWeather(
                        temperature: temp,
                        apparentTemperature: appTemp,
                        weatherDescription: desc,
                        weatherCode: wCode,
                        humidity: humidity,
                        windSpeed: wind
                    ),
                    daily: dailyList
                )
            }
        } catch {
            // fallback
        }

        return WeatherData.defaultSample()
    }

    private func getWeatherDescription(_ code: Int) -> String {
        switch code {
        case 0: return "Açık ve Güneşli"
        case 1: return "Çoğunlukla Açık"
        case 2: return "Parçalı Bulutlu"
        case 3: return "Bulutlu"
        case 45, 48: return "Sisli"
        case 51, 53, 55: return "Çiseleyen Yağmur"
        case 61, 63: return "Yağmurlu"
        case 65: return "Kuvvetli Yağmur"
        case 71, 73, 75: return "Kar Yağışlı"
        case 80, 81, 82: return "Sağanak Yağış"
        case 95, 96, 99: return "Gök Gürültülü Fırtına"
        default: return "Açık"
        }
    }
}

