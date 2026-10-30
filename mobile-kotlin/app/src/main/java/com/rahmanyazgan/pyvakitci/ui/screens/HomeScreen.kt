package com.rahmanyazgan.pyvakitci.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
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
import com.rahmanyazgan.pyvakitci.data.model.ActivePrayerInfo
import com.rahmanyazgan.pyvakitci.data.model.PrayerTimes
import com.rahmanyazgan.pyvakitci.data.model.SelectedLocation
import com.rahmanyazgan.pyvakitci.ui.components.PrayerClockDial
import com.rahmanyazgan.pyvakitci.ui.components.PrayerGrid
import com.rahmanyazgan.pyvakitci.ui.theme.AppColors
import com.rahmanyazgan.pyvakitci.util.TimeUtil

@Composable
fun HomeScreen(
    prayerTimes: PrayerTimes,
    activeInfo: ActivePrayerInfo,
    location: SelectedLocation,
    currentTimeStr: String,
    interfaceMode: String,
    colors: AppColors,
    modifier: Modifier = Modifier
) {
    val scrollState = rememberScrollState()

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(colors.background)
            .padding(horizontal = 16.dp, vertical = 8.dp)
            .verticalScroll(scrollState),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        // Top Location & Date Header
        Card(
            shape = RoundedCornerShape(16.dp),
            colors = CardDefaults.cardColors(containerColor = colors.surface),
            elevation = CardDefaults.cardElevation(2.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "${location.countryName} • ${location.cityName}",
                            fontSize = 12.sp,
                            color = colors.text.copy(alpha = 0.6f),
                            fontWeight = FontWeight.Medium
                        )
                        Text(
                            text = location.districtName,
                            fontSize = 20.sp,
                            fontWeight = FontWeight.ExtraBold,
                            color = colors.text
                        )
                    }
                    Column(horizontalAlignment = Alignment.End) {
                        Text(
                            text = TimeUtil.getDisplayDate(),
                            fontSize = 12.sp,
                            fontWeight = FontWeight.SemiBold,
                            color = colors.text
                        )
                        Text(
                            text = prayerTimes.hijriDate,
                            fontSize = 11.sp,
                            color = colors.text.copy(alpha = 0.6f)
                        )
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Center Area depending on interfaceMode
        when (interfaceMode) {
            "gokyuzu" -> {
                // Dynamic Sky Mode
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(240.dp)
                        .background(
                            brush = Brush.verticalGradient(
                                colors = listOf(Color(0xFF1E3C72), Color(0xFF2A5298))
                            ),
                            shape = RoundedCornerShape(20.dp)
                        ),
                    contentAlignment = Alignment.Center
                ) {
                    Column(horizontalAlignment = Alignment.CenterHorizontally) {
                        Text(
                            text = "✦ ${activeInfo.currentPrayer.title.uppercase()} ✦",
                            fontSize = 18.sp,
                            fontWeight = FontWeight.Bold,
                            color = Color(0xFFFFD700)
                        )
                        Spacer(modifier = Modifier.height(8.dp))
                        Text(
                            text = currentTimeStr,
                            fontSize = 40.sp,
                            fontWeight = FontWeight.Black,
                            color = Color.White
                        )
                        Spacer(modifier = Modifier.height(6.dp))
                        Text(
                            text = "${activeInfo.nextPrayer.title}'a Kalan",
                            fontSize = 14.sp,
                            color = Color.White.copy(alpha = 0.8f)
                        )
                        Text(
                            text = activeInfo.remainingFormatted,
                            fontSize = 22.sp,
                            fontWeight = FontWeight.Bold,
                            color = Color(0xFFFFCC80)
                        )
                    }
                }
            }
            "kartlar" -> {
                // Modern Cards Mode
                Card(
                    shape = RoundedCornerShape(20.dp),
                    colors = CardDefaults.cardColors(containerColor = colors.surface),
                    elevation = CardDefaults.cardElevation(3.dp),
                    modifier = Modifier.fillMaxWidth().padding(vertical = 12.dp)
                ) {
                    Column(
                        modifier = Modifier.padding(20.dp).fillMaxWidth(),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        Text(
                            text = "${activeInfo.currentPrayer.title.uppercase()} VAKTİ",
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Bold,
                            color = colors.primary
                        )
                        Text(
                            text = currentTimeStr,
                            fontSize = 36.sp,
                            fontWeight = FontWeight.ExtraBold,
                            color = colors.text
                        )
                        Spacer(modifier = Modifier.height(4.dp))
                        Text(
                            text = "${activeInfo.nextPrayer.title} Vaktine Kalan Süre: ${activeInfo.remainingFormatted}",
                            fontSize = 14.sp,
                            fontWeight = FontWeight.SemiBold,
                            color = Color(0xFFD32F2F)
                        )
                    }
                }
            }
            else -> {
                // Default: Circular Dial Mode ("saat")
                PrayerClockDial(
                    prayerTimes = prayerTimes,
                    activeInfo = activeInfo,
                    currentTimeStr = currentTimeStr,
                    textColor = colors.text,
                    modifier = Modifier.padding(vertical = 8.dp)
                )
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Prayer Grid (Cards)
        PrayerGrid(
            times = prayerTimes.toList(),
            activePrayer = activeInfo.currentPrayer,
            surfaceColor = colors.surface,
            textColor = colors.text,
            borderColor = colors.cardBorder
        )

        Spacer(modifier = Modifier.height(14.dp))

        // Source Footer
        Text(
            text = "Kaynak: diyanet.gov.tr",
            fontSize = 11.sp,
            color = colors.text.copy(alpha = 0.45f)
        )

        Spacer(modifier = Modifier.height(8.dp))
    }
}

