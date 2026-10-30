package com.rahmanyazgan.pyvakitci.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rahmanyazgan.pyvakitci.data.model.PrayerKind

@Composable
fun PrayerGrid(
    times: List<Pair<String, String>>,
    activePrayer: PrayerKind,
    surfaceColor: Color,
    textColor: Color,
    borderColor: Color,
    modifier: Modifier = Modifier
) {
    fun isMatchingPrayer(cardName: String, prayer: PrayerKind): Boolean {
        if (cardName.equals(prayer.title, ignoreCase = true)) return true
        if (cardName.equals("İmsak", ignoreCase = true) && prayer == PrayerKind.IMSAK) return true
        if (cardName.equals("Sabah", ignoreCase = true) && prayer == PrayerKind.IMSAK) return true
        return false
    }

    Column(
        modifier = modifier.fillMaxWidth(),
        verticalArrangement = Arrangement.spacedBy(8.dp)
    ) {
        // Row 1: İmsak, Güneş, Öğle
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            times.take(3).forEach { (name, time) ->
                PrayerCard(
                    title = name,
                    time = time,
                    isActive = isMatchingPrayer(name, activePrayer),
                    surfaceColor = surfaceColor,
                    textColor = textColor,
                    borderColor = borderColor,
                    modifier = Modifier.weight(1f)
                )
            }
        }
        // Row 2: İkindi, Akşam, Yatsı
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            times.drop(3).forEach { (name, time) ->
                PrayerCard(
                    title = name,
                    time = time,
                    isActive = isMatchingPrayer(name, activePrayer),
                    surfaceColor = surfaceColor,
                    textColor = textColor,
                    borderColor = borderColor,
                    modifier = Modifier.weight(1f)
                )
            }
        }
    }
}

@Composable
fun PrayerCard(
    title: String,
    time: String,
    isActive: Boolean,
    surfaceColor: Color,
    textColor: Color,
    borderColor: Color,
    modifier: Modifier = Modifier
) {
    val activeBorderColor = Color(0xFF1976D2)
    val shape = RoundedCornerShape(12.dp)

    Card(
        shape = shape,
        colors = CardDefaults.cardColors(
            containerColor = if (isActive) surfaceColor else surfaceColor.copy(alpha = 0.85f)
        ),
        elevation = CardDefaults.cardElevation(defaultElevation = if (isActive) 4.dp else 1.dp),
        modifier = modifier
            .border(
                width = if (isActive) 2.dp else 1.dp,
                color = if (isActive) activeBorderColor else borderColor,
                shape = shape
            )
    ) {
        Column(
            modifier = Modifier
                .padding(vertical = 10.dp, horizontal = 8.dp)
                .fillMaxWidth(),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Row(
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = title,
                    fontSize = 12.sp,
                    color = if (isActive) activeBorderColor else textColor.copy(alpha = 0.7f),
                    fontWeight = if (isActive) FontWeight.Bold else FontWeight.Medium
                )
                if (isActive) {
                    Spacer(modifier = Modifier.width(4.dp))
                    Box(
                        modifier = Modifier
                            .size(6.dp)
                            .background(activeBorderColor, CircleShape)
                    )
                }
            }
            Text(
                text = time,
                fontSize = 15.sp,
                fontWeight = FontWeight.Bold,
                color = if (isActive) Color(0xFFD32F2F) else textColor
            )
        }
    }
}

