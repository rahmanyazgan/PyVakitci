package com.rahmanyazgan.pyvakitci.ui.screens

import androidx.compose.animation.animateColorAsState
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.core.tween
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.GpsFixed
import androidx.compose.material.icons.filled.MyLocation
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rahmanyazgan.pyvakitci.ui.theme.AppColors
import kotlin.math.cos
import kotlin.math.sin

@Composable
fun QiblaScreen(
    azimuth: Float,
    qiblaAngle: Float,
    isAligned: Boolean,
    hasGps: Boolean,
    latitude: Double,
    longitude: Double,
    locationName: String,
    colors: AppColors,
    onRefreshLocation: () -> Unit,
    modifier: Modifier = Modifier
) {
    val animatedQiblaAngle by animateFloatAsState(
        targetValue = qiblaAngle,
        animationSpec = tween(durationMillis = 150),
        label = "qiblaAngle"
    )

    val animatedAzimuth by animateFloatAsState(
        targetValue = azimuth,
        animationSpec = tween(durationMillis = 150),
        label = "azimuth"
    )

    val alignedGreen = Color(0xFF2E7D32)
    val cardBgColor by animateColorAsState(
        targetValue = if (isAligned) Color(0xFFE8F5E9) else colors.surface,
        label = "cardBgColor"
    )

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(colors.background)
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        // Location Info Card
        Card(
            shape = RoundedCornerShape(16.dp),
            colors = CardDefaults.cardColors(containerColor = colors.surface),
            elevation = CardDefaults.cardElevation(2.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(
                modifier = Modifier.padding(14.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Text(
                            text = "Kıble Pusulası",
                            fontSize = 17.sp,
                            fontWeight = FontWeight.Bold,
                            color = colors.text
                        )
                        Text(
                            text = locationName,
                            fontSize = 13.sp,
                            color = colors.text.copy(alpha = 0.65f)
                        )
                    }

                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Icon(
                            imageVector = if (hasGps) Icons.Default.GpsFixed else Icons.Default.MyLocation,
                            contentDescription = null,
                            tint = if (hasGps) alignedGreen else colors.primary,
                            modifier = Modifier.size(18.dp)
                        )
                        Spacer(modifier = Modifier.width(4.dp))
                        Text(
                            text = if (hasGps) "Canlı GPS" else "Kayıtlı Konum",
                            fontSize = 11.sp,
                            fontWeight = FontWeight.SemiBold,
                            color = if (hasGps) alignedGreen else colors.primary
                        )
                    }
                }

                Spacer(modifier = Modifier.height(4.dp))

                Text(
                    text = String.format("Koordinat: %.4f° K, %.4f° D", latitude, longitude),
                    fontSize = 11.sp,
                    color = colors.text.copy(alpha = 0.5f)
                )
            }
        }

        Spacer(modifier = Modifier.height(18.dp))

        // Aligned Status Banner
        if (isAligned) {
            Card(
                shape = RoundedCornerShape(12.dp),
                colors = CardDefaults.cardColors(containerColor = alignedGreen),
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 8.dp)
            ) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(vertical = 8.dp, horizontal = 12.dp),
                    horizontalArrangement = Arrangement.Center,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text(
                        text = "✦ KIBLEYE YÖNELDİNİZ ✦",
                        fontSize = 14.sp,
                        fontWeight = FontWeight.Bold,
                        color = Color.White
                    )
                }
            }
            Spacer(modifier = Modifier.height(12.dp))
        }

        // Compass Canvas
        Box(
            modifier = Modifier.size(280.dp),
            contentAlignment = Alignment.Center
        ) {
            Canvas(modifier = Modifier.fillMaxSize()) {
                val center = Offset(size.width / 2f, size.height / 2f)
                val radius = (size.minDimension / 2f) - 18.dp.toPx()

                // Compass Outer Circle
                drawCircle(
                    color = colors.surface,
                    radius = radius,
                    center = center
                )
                drawCircle(
                    color = if (isAligned) alignedGreen else colors.cardBorder,
                    radius = radius,
                    center = center,
                    style = Stroke(width = if (isAligned) 4.dp.toPx() else 2.5.dp.toPx())
                )

                // Draw N, E, S, W Ticks (Compass Rose rotates with device)
                for (i in 0 until 360 step 30) {
                    val angle = (i - animatedAzimuth - 90f)
                    val rad = Math.toRadians(angle.toDouble())
                    val tickLen = if (i % 90 == 0) 14.dp.toPx() else 8.dp.toPx()

                    val startX = center.x + (radius - tickLen) * cos(rad).toFloat()
                    val startY = center.y + (radius - tickLen) * sin(rad).toFloat()
                    val endX = center.x + radius * cos(rad).toFloat()
                    val endY = center.y + radius * sin(rad).toFloat()

                    drawLine(
                        color = if (i == 0) Color(0xFFE53935) else colors.text.copy(alpha = 0.35f),
                        start = Offset(startX, startY),
                        end = Offset(endX, endY),
                        strokeWidth = if (i == 0) 3.5.dp.toPx() else 1.5.dp.toPx(),
                        cap = StrokeCap.Round
                    )
                }

                // Kaaba Direction Needle (Points directly to Kaaba in real world)
                val qRad = Math.toRadians((animatedQiblaAngle - 90f).toDouble())
                val needleLen = radius - 16.dp.toPx()
                val qEndX = center.x + needleLen * cos(qRad).toFloat()
                val qEndY = center.y + needleLen * sin(qRad).toFloat()

                // Needle Line
                drawLine(
                    color = if (isAligned) alignedGreen else Color(0xFFFFB300),
                    start = center,
                    end = Offset(qEndX, qEndY),
                    strokeWidth = 4.5.dp.toPx(),
                    cap = StrokeCap.Round
                )

                // Kaaba Gold/Green Tip
                drawCircle(
                    color = if (isAligned) alignedGreen else Color(0xFF2E7D32),
                    radius = 11.dp.toPx(),
                    center = Offset(qEndX, qEndY)
                )
                drawCircle(
                    color = Color(0xFFFFD700),
                    radius = 6.dp.toPx(),
                    center = Offset(qEndX, qEndY)
                )

                // Center Pivot Pin
                drawCircle(
                    color = if (isAligned) alignedGreen else colors.primary,
                    radius = 7.dp.toPx(),
                    center = center
                )
            }
        }

        Spacer(modifier = Modifier.height(18.dp))

        // Angle & Guidance Card
        Card(
            shape = RoundedCornerShape(16.dp),
            colors = CardDefaults.cardColors(containerColor = cardBgColor),
            elevation = CardDefaults.cardElevation(2.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(
                modifier = Modifier.padding(14.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text(
                    text = if (isAligned) "Kıble Yönündesiniz" else "Kıble Açısı: ${qiblaAngle.toInt()}°",
                    fontSize = 18.sp,
                    fontWeight = FontWeight.ExtraBold,
                    color = if (isAligned) alignedGreen else colors.text
                )
                Spacer(modifier = Modifier.height(4.dp))
                Text(
                    text = if (isAligned)
                        "Cihaz şu an tam Kâbe istikametindedir."
                    else
                        "Cihazı yatay tutup altın ibre tepeye gelene kadar dönünüz.",
                    fontSize = 12.sp,
                    color = colors.text.copy(alpha = 0.7f)
                )
            }
        }

        Spacer(modifier = Modifier.height(14.dp))

        // Refresh GPS Location Button
        Button(
            onClick = onRefreshLocation,
            colors = ButtonDefaults.buttonColors(containerColor = colors.surface),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier
                .fillMaxWidth()
                .border(1.dp, colors.cardBorder, RoundedCornerShape(12.dp))
        ) {
            Icon(
                imageVector = Icons.Default.GpsFixed,
                contentDescription = null,
                tint = colors.primary,
                modifier = Modifier.size(18.dp)
            )
            Spacer(modifier = Modifier.width(8.dp))
            Text(
                text = "Konumumu GPS ile Doğrula",
                fontSize = 13.sp,
                fontWeight = FontWeight.SemiBold,
                color = colors.primary
            )
        }
    }
}
