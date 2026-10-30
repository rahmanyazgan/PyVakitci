package com.rahmanyazgan.pyvakitci.data.model

data class CurrentWeather(
    val temperature: Double,
    val apparentTemperature: Double,
    val weatherDescription: String,
    val weatherCode: Int,
    val humidity: Int,
    val windSpeed: Double,
    val iconRes: String = "gunesli"
)

data class DailyForecast(
    val date: String,
    val dayName: String,
    val minTemp: Double,
    val maxTemp: Double,
    val weatherCode: Int,
    val description: String
)

data class WeatherData(
    val current: CurrentWeather,
    val daily: List<DailyForecast>
)

