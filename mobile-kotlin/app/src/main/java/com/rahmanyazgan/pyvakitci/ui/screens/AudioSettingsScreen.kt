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
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Stop
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Checkbox
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Slider
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rahmanyazgan.pyvakitci.data.local.PreferencesManager
import com.rahmanyazgan.pyvakitci.service.AudioPlayerManager
import com.rahmanyazgan.pyvakitci.ui.theme.AppColors

@Composable
fun AudioSettingsScreen(
    prefs: PreferencesManager,
    audioPlayer: AudioPlayerManager,
    colors: AppColors,
    modifier: Modifier = Modifier
) {
    val scrollState = rememberScrollState()
    var currentPlayingSound by remember { mutableStateOf<String?>(null) }

    var isWarningEnabled by remember { mutableStateOf(prefs.isWarningSoundEnabled) }
    var isVibrationEnabled by remember { mutableStateOf(prefs.isVibrationEnabled) }
    var volumeLevel by remember { mutableStateOf(prefs.volume.toFloat()) }

    fun playOrStop(soundName: String) {
        if (currentPlayingSound == soundName) {
            audioPlayer.stopSound()
            currentPlayingSound = null
        } else {
            audioPlayer.playSound(audioPlayer.getRawResourceId(soundName), volumeLevel.toInt())
            currentPlayingSound = soundName
            audioPlayer.onCompletionListener = {
                currentPlayingSound = null
            }
        }
    }

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(colors.background)
            .padding(16.dp)
            .verticalScroll(scrollState)
    ) {
        Text(
            text = "Ezan ve Vakit Sesleri",
            fontSize = 20.sp,
            fontWeight = FontWeight.Bold,
            color = colors.text
        )

        Spacer(modifier = Modifier.height(16.dp))

        // Prayers List
        val prayers = listOf(
            Triple("Sabah", "sabah_saba", "Sabah Ezanı (Saba Makamı)"),
            Triple("Öğle", "ogle_rast", "Öğle Ezanı (Rast Makamı)"),
            Triple("İkindi", "ikindi_hicaz", "İkindi Ezanı (Hicaz Makamı)"),
            Triple("Akşam", "aksam_segah", "Akşam Ezanı (Segah Makamı)"),
            Triple("Yatsı", "yatsi_ussak", "Yatsı Ezanı (Uşşak Makamı)"),
            Triple("Dua", "ezan_duasi", "Ezan Duası"),
            Triple("Sela", "sela", "Cuma Selası")
        )

        prayers.forEach { (name, soundName, description) ->
            var enabled by remember { mutableStateOf(prefs.isPrayerSoundEnabled(name)) }

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
                        .padding(horizontal = 12.dp, vertical = 8.dp)
                        .fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = name,
                            fontSize = 15.sp,
                            fontWeight = FontWeight.Bold,
                            color = colors.text
                        )
                        Text(
                            text = description,
                            fontSize = 11.sp,
                            color = colors.text.copy(alpha = 0.6f)
                        )
                    }

                    Row(verticalAlignment = Alignment.CenterVertically) {
                        IconButton(
                            onClick = { playOrStop(soundName) },
                            modifier = Modifier.size(36.dp)
                        ) {
                            Icon(
                                imageVector = if (currentPlayingSound == soundName) Icons.Default.Stop else Icons.Default.PlayArrow,
                                contentDescription = "Oynat",
                                tint = colors.primary
                            )
                        }

                        Spacer(modifier = Modifier.width(8.dp))

                        Checkbox(
                            checked = enabled,
                            onCheckedChange = {
                                enabled = it
                                prefs.setPrayerSoundEnabled(name, it)
                            }
                        )
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Warning Sound Card
        Card(
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(containerColor = colors.surface),
            elevation = CardDefaults.cardElevation(1.dp),
            modifier = Modifier.fillMaxWidth().padding(vertical = 4.dp)
        ) {
            Row(
                modifier = Modifier
                    .padding(horizontal = 12.dp, vertical = 8.dp)
                    .fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Column(modifier = Modifier.weight(1f)) {
                    Text(
                        text = "Vakit Öncesi Uyarı Sesi",
                        fontSize = 15.sp,
                        fontWeight = FontWeight.Bold,
                        color = colors.text
                    )
                    Text(
                        text = "Vakitten 15 dakika önce çalar",
                        fontSize = 11.sp,
                        color = colors.text.copy(alpha = 0.6f)
                    )
                }

                Row(verticalAlignment = Alignment.CenterVertically) {
                    IconButton(
                        onClick = { playOrStop("uyari") },
                        modifier = Modifier.size(36.dp)
                    ) {
                        Icon(
                            imageVector = if (currentPlayingSound == "uyari") Icons.Default.Stop else Icons.Default.PlayArrow,
                            contentDescription = "Oynat",
                            tint = colors.primary
                        )
                    }

                    Spacer(modifier = Modifier.width(8.dp))

                    Checkbox(
                        checked = isWarningEnabled,
                        onCheckedChange = {
                            isWarningEnabled = it
                            prefs.isWarningSoundEnabled = it
                        }
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Volume Level Slider
        Card(
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(containerColor = colors.surface),
            elevation = CardDefaults.cardElevation(1.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text(
                    text = "Ses Seviyesi: %${volumeLevel.toInt()}",
                    fontSize = 14.sp,
                    fontWeight = FontWeight.SemiBold,
                    color = colors.text
                )
                Slider(
                    value = volumeLevel,
                    onValueChange = {
                        volumeLevel = it
                        prefs.volume = it.toInt()
                    },
                    valueRange = 0f..100f
                )

                Spacer(modifier = Modifier.height(8.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "Vakitlerde Titreşim",
                        fontSize = 14.sp,
                        color = colors.text
                    )
                    Switch(
                        checked = isVibrationEnabled,
                        onCheckedChange = {
                            isVibrationEnabled = it
                            prefs.isVibrationEnabled = it
                        }
                    )
                }
            }
        }
    }
}

