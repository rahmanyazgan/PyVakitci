package com.rahmanyazgan.pyvakitci.ui.components

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rahmanyazgan.pyvakitci.data.model.ActivePrayerInfo
import com.rahmanyazgan.pyvakitci.data.model.PrayerTimes
import com.rahmanyazgan.pyvakitci.ui.theme.ArcAksam
import com.rahmanyazgan.pyvakitci.ui.theme.ArcGunes
import com.rahmanyazgan.pyvakitci.ui.theme.ArcIkindi
import com.rahmanyazgan.pyvakitci.ui.theme.ArcImsak
import com.rahmanyazgan.pyvakitci.ui.theme.ArcOgle
import com.rahmanyazgan.pyvakitci.ui.theme.ArcYatsi
import com.rahmanyazgan.pyvakitci.ui.theme.NeedleRed
import com.rahmanyazgan.pyvakitci.util.TimeUtil
import java.util.Calendar
import kotlin.math.cos
import kotlin.math.sin

@Composable
fun PrayerClockDial(
    prayerTimes: PrayerTimes,
    activeInfo: ActivePrayerInfo,
    currentTimeStr: String,
    textColor: Color,
    modifier: Modifier = Modifier
) {
    Box(
        modifier = modifier.size(280.dp),
        contentAlignment = Alignment.Center
    ) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            val center = Offset(size.width / 2f, size.height / 2f)
            val radius = (size.minDimension / 2f) - 24.dp.toPx()
            val strokeWidth = 18.dp.toPx()

            // Outer subtle background track
            drawCircle(
                color = Color.LightGray.copy(alpha = 0.2f),
                radius = radius,
                center = center,
                style = Stroke(width = strokeWidth)
            )

            // Prayer angles on 24h clock: 0h = top (-90 degrees), 12h = bottom (+90 degrees)
            fun timeToAngle(tStr: String): Float {
                val mins = TimeUtil.timeToMinutes(tStr)
                return (mins / 1440f) * 360f - 90f
            }

            val aImsak = timeToAngle(prayerTimes.imsak)
            val aGunes = timeToAngle(prayerTimes.gunes)
            val aOgle = timeToAngle(prayerTimes.ogle)
            val aIkindi = timeToAngle(prayerTimes.ikindi)
            val aAksam = timeToAngle(prayerTimes.aksam)
            val aYatsi = timeToAngle(prayerTimes.yatsi)

            fun sweep(from: Float, to: Float): Float {
                var s = to - from
                while (s < 0) s += 360f
                return s
            }

            val arcs = listOf(
                Triple(aImsak, sweep(aImsak, aGunes), ArcImsak),
                Triple(aGunes, sweep(aGunes, aOgle), ArcGunes),
                Triple(aOgle, sweep(aOgle, aIkindi), ArcOgle),
                Triple(aIkindi, sweep(aIkindi, aAksam), ArcIkindi),
                Triple(aAksam, sweep(aAksam, aYatsi), ArcAksam),
                Triple(aYatsi, sweep(aYatsi, aImsak), ArcYatsi)
            )

            val arcRect = Size(radius * 2f, radius * 2f)
            val arcTopLeft = Offset(center.x - radius, center.y - radius)

            for ((startAngle, sweepAngle, color) in arcs) {
                drawArc(
                    color = color,
                    startAngle = startAngle,
                    sweepAngle = sweepAngle,
                    useCenter = false,
                    topLeft = arcTopLeft,
                    size = arcRect,
                    style = Stroke(width = strokeWidth)
                )
            }

            // Divider white dots
            val angles = listOf(aImsak, aGunes, aOgle, aIkindi, aAksam, aYatsi)
            for (ang in angles) {
                val rad = Math.toRadians(ang.toDouble())
                val dotX = center.x + radius * cos(rad).toFloat()
                val dotY = center.y + radius * sin(rad).toFloat()
                drawCircle(
                    color = Color.White,
                    radius = 4.dp.toPx(),
                    center = Offset(dotX, dotY)
                )
            }

            // Red needle pointing to current time
            val cal = Calendar.getInstance()
            val nowMinutes = cal.get(Calendar.HOUR_OF_DAY) * 60f +
                    cal.get(Calendar.MINUTE) +
                    cal.get(Calendar.SECOND) / 60f
            val nowAngle = (nowMinutes / 1440f) * 360f - 90f
            val nowRad = Math.toRadians(nowAngle.toDouble())

            val innerR = radius - (strokeWidth / 2f) - 15.dp.toPx()
            val outerR = radius + (strokeWidth / 2f) + 6.dp.toPx()

            val needleStartX = center.x + innerR * cos(nowRad).toFloat()
            val needleStartY = center.y + innerR * sin(nowRad).toFloat()
            val needleEndX = center.x + outerR * cos(nowRad).toFloat()
            val needleEndY = center.y + outerR * sin(nowRad).toFloat()

            drawLine(
                color = NeedleRed,
                start = Offset(needleStartX, needleStartY),
                end = Offset(needleEndX, needleEndY),
                strokeWidth = 3.dp.toPx(),
                cap = StrokeCap.Round
            )
            drawCircle(
                color = NeedleRed,
                radius = 5.dp.toPx(),
                center = Offset(needleEndX, needleEndY)
            )
        }

        // Center Digital Display
        Column(
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                text = "✦ ${activeInfo.currentPrayer.uppercaseTitle} ✦",
                fontSize = 13.sp,
                fontWeight = FontWeight.Bold,
                color = ArcImsak
            )
            Spacer(modifier = Modifier.height(2.dp))
            Text(
                text = currentTimeStr,
                fontSize = 24.sp,
                fontWeight = FontWeight.ExtraBold,
                color = textColor
            )
            Spacer(modifier = Modifier.height(2.dp))
            Text(
                text = activeInfo.nextPrayer.getRemainingSuffix(),
                fontSize = 11.sp,
                color = textColor.copy(alpha = 0.7f)
            )
            Text(
                text = activeInfo.remainingFormatted,
                fontSize = 16.sp,
                fontWeight = FontWeight.Bold,
                color = Color(0xFFE65100)
            )
        }
    }
}

