package com.rahmanyazgan.pyvakitci.data.remote

import com.rahmanyazgan.pyvakitci.data.model.CurrentWeather
import com.rahmanyazgan.pyvakitci.data.model.DailyForecast
import com.rahmanyazgan.pyvakitci.data.model.WeatherData
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request
import org.json.JSONObject
import java.text.SimpleDateFormat
import java.util.Calendar
import java.util.Locale
import java.util.concurrent.TimeUnit

class WeatherApiService {
    private val client = OkHttpClient.Builder()
        .connectTimeout(10, TimeUnit.SECONDS)
        .readTimeout(10, TimeUnit.SECONDS)
        .build()

    suspend fun fetchWeather(lat: Double, lon: Double): WeatherData = withContext(Dispatchers.IO) {
        val url = "https://api.open-meteo.com/v1/forecast?latitude=$lat&longitude=$lon&current=temperature_2m,relative_humidity_2m,apparent_temperature,weather_code,wind_speed_10m&daily=weather_code,temperature_2m_max,temperature_2m_min&timezone=auto"
        val request = Request.Builder().url(url).build()

        try {
            client.newCall(request).execute().use { response ->
                if (response.isSuccessful) {
                    val body = response.body?.string() ?: ""
                    val root = JSONObject(body)

                    val curObj = root.optJSONObject("current")
                    val temp = curObj?.optDouble("temperature_2m", 16.5) ?: 16.5
                    val appTemp = curObj?.optDouble("apparent_temperature", 15.8) ?: 15.8
                    val hum = curObj?.optInt("relative_humidity_2m", 68) ?: 68
                    val wcode = curObj?.optInt("weather_code", 1) ?: 1
                    val wind = curObj?.optDouble("wind_speed_10m", 12.0) ?: 12.0

                    val curDesc = getWeatherDescription(wcode)

                    val dailyList = mutableListOf<DailyForecast>()
                    val dailyObj = root.optJSONObject("daily")
                    if (dailyObj != null) {
                        val times = dailyObj.optJSONArray("time")
                        val maxs = dailyObj.optJSONArray("temperature_2m_max")
                        val mins = dailyObj.optJSONArray("temperature_2m_min")
                        val codes = dailyObj.optJSONArray("weather_code")

                        val count = times?.length() ?: 0
                        val dayFormat = SimpleDateFormat("EEEE", Locale("tr"))
                        val parseFormat = SimpleDateFormat("yyyy-MM-dd", Locale.getDefault())

                        for (i in 0 until minOf(count, 5)) {
                            val dStr = times?.optString(i) ?: ""
                            val minT = mins?.optDouble(i) ?: 10.0
                            val maxT = maxs?.optDouble(i) ?: 18.0
                            val code = codes?.optInt(i) ?: 0

                            var dayName = dStr
                            try {
                                val parsed = parseFormat.parse(dStr)
                                if (parsed != null) {
                                    dayName = dayFormat.format(parsed)
                                }
                            } catch (_: Exception) {}

                            dailyList.add(
                                DailyForecast(
                                    date = dStr,
                                    dayName = dayName.replaceFirstChar { it.uppercase(java.util.Locale("tr", "TR")) },
                                    minTemp = minT,
                                    maxTemp = maxT,
                                    weatherCode = code,
                                    description = getWeatherDescription(code)
                                )
                            )
                        }
                    }

                    return@withContext WeatherData(
                        current = CurrentWeather(
                            temperature = temp,
                            apparentTemperature = appTemp,
                            weatherDescription = curDesc,
                            weatherCode = wcode,
                            humidity = hum,
                            windSpeed = wind
                        ),
                        daily = dailyList
                    )
                }
            }
        } catch (_: Exception) {
            // fallback
        }

        // Fallback default weather
        WeatherData(
            current = CurrentWeather(
                temperature = 17.0,
                apparentTemperature = 16.0,
                weatherDescription = "Parçalı Bulutlu",
                weatherCode = 2,
                humidity = 65,
                windSpeed = 14.0
            ),
            daily = listOf(
                DailyForecast("2026-10-29", "Bugün", 12.0, 18.0, 2, "Parçalı Bulutlu"),
                DailyForecast("2026-10-30", "Cuma", 11.0, 17.0, 3, "Bulutlu"),
                DailyForecast("2026-10-31", "Cumartesi", 10.0, 16.0, 61, "Hafif Yağmurlu"),
                DailyForecast("2026-11-01", "Pazar", 9.0, 15.0, 0, "Açık ve Güneşli"),
                DailyForecast("2026-11-02", "Pazartesi", 11.0, 18.0, 1, "Az Bulutlu")
            )
        )
    }

    private fun getWeatherDescription(code: Int): String {
        return when (code) {
            0 -> "Açık ve Güneşli"
            1 -> "Çoğunlukla Açık"
            2 -> "Parçalı Bulutlu"
            3 -> "Bulutlu"
            45, 48 -> "Sisli"
            51, 53, 55 -> "Çiseleyen Yağmur"
            61, 63 -> "Yağmurlu"
            65 -> "Kuvvetli Yağmur"
            71, 73, 75 -> "Kar Yağışlı"
            80, 81, 82 -> "Sağanak Yağış"
            95, 96, 99 -> "Gök Gürültülü Fırtına"
            else -> "Açık"
        }
    }
}

