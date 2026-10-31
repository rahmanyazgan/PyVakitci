package com.rahmanyazgan.pyvakitci.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rahmanyazgan.pyvakitci.data.model.WeatherData
import com.rahmanyazgan.pyvakitci.ui.theme.AppColors
import com.rahmanyazgan.pyvakitci.util.L10n
import java.text.SimpleDateFormat
import java.util.Locale

/** "yyyy-MM-dd" tarihinden seçili dilde gün adı üretir (Pazartesi / Monday / 星期一 ...). */
private fun localizedDayName(date: String, fallback: String): String {
    return try {
        val parsed = SimpleDateFormat("yyyy-MM-dd", Locale.US).parse(date) ?: return fallback
        val loc = L10n.locale()
        SimpleDateFormat("EEEE", loc).format(parsed).replaceFirstChar { it.uppercase(loc) }
    } catch (_: Exception) {
        fallback
    }
}

@Composable
fun WeatherScreen(
    locationName: String,
    weatherData: WeatherData,
    colors: AppColors,
    modifier: Modifier = Modifier
) {
    val scrollState = rememberScrollState()

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(colors.background)
            .padding(16.dp)
            .verticalScroll(scrollState),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        // Main Current Weather Card with Gradient
        Card(
            shape = RoundedCornerShape(24.dp),
            modifier = Modifier.fillMaxWidth(),
            elevation = CardDefaults.cardElevation(4.dp)
        ) {
            Column(
                modifier = Modifier
                    .background(
                        brush = Brush.verticalGradient(
                            colors = listOf(Color(0xFF2980B9), Color(0xFF6DD5FA))
                        )
                    )
                    .padding(24.dp)
                    .fillMaxWidth(),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text(
                    text = locationName,
                    fontSize = 18.sp,
                    fontWeight = FontWeight.SemiBold,
                    color = Color.White
                )
                Spacer(modifier = Modifier.height(10.dp))
                Text(
                    text = "${weatherData.current.temperature.toInt()}°C",
                    fontSize = 52.sp,
                    fontWeight = FontWeight.Black,
                    color = Color.White
                )
                Text(
                    text = L10n.weather(weatherData.current.weatherCode),
                    fontSize = 16.sp,
                    fontWeight = FontWeight.Medium,
                    color = Color.White.copy(alpha = 0.9f)
                )
                Spacer(modifier = Modifier.height(16.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceEvenly
                ) {
                    WeatherDetailItem(label = L10n.t("w_feels"), value = "${weatherData.current.apparentTemperature.toInt()}°C")
                    WeatherDetailItem(label = L10n.t("w_humidity"), value = "%${weatherData.current.humidity}")
                    WeatherDetailItem(label = L10n.t("w_wind"), value = "${weatherData.current.windSpeed.toInt()} ${L10n.t("w_wind_unit")}")
                }
            }
        }

        Spacer(modifier = Modifier.height(20.dp))

        // 5-Day Forecast Header
        Text(
            text = L10n.t("w_forecast"),
            fontSize = 16.sp,
            fontWeight = FontWeight.Bold,
            color = colors.text,
            modifier = Modifier.fillMaxWidth()
        )

        Spacer(modifier = Modifier.height(12.dp))

        // Daily Forecast List
        weatherData.daily.forEach { forecast ->
            Card(
                shape = RoundedCornerShape(14.dp),
                colors = CardDefaults.cardColors(containerColor = colors.surface),
                elevation = CardDefaults.cardElevation(1.dp),
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 4.dp)
            ) {
                Row(
                    modifier = Modifier
                        .padding(horizontal = 16.dp, vertical = 14.dp)
                        .fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = localizedDayName(forecast.date, forecast.dayName),
                            fontSize = 14.sp,
                            fontWeight = FontWeight.Bold,
                            color = colors.text
                        )
                        Text(
                            text = L10n.weather(forecast.weatherCode),
                            fontSize = 12.sp,
                            color = colors.text.copy(alpha = 0.6f)
                        )
                    }

                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Text(
                            text = "${forecast.maxTemp.toInt()}°",
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Bold,
                            color = colors.text
                        )
                        Text(
                            text = " / ${forecast.minTemp.toInt()}°",
                            fontSize = 14.sp,
                            color = colors.text.copy(alpha = 0.6f)
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun WeatherDetailItem(label: String, value: String) {
    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Text(text = label, fontSize = 11.sp, color = Color.White.copy(alpha = 0.75f))
        Text(text = value, fontSize = 14.sp, fontWeight = FontWeight.Bold, color = Color.White)
    }
}

