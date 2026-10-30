package com.rahmanyazgan.pyvakitci.ui.screens

import android.os.Build
import android.os.PowerManager
import android.content.Intent
import android.provider.Settings
import androidx.compose.material.icons.filled.Check
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults

import android.net.Uri
import android.provider.OpenableColumns
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
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
import androidx.compose.material.icons.filled.FolderOpen
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Refresh
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
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rahmanyazgan.pyvakitci.data.local.PreferencesManager
import com.rahmanyazgan.pyvakitci.service.AudioPlayerManager
import com.rahmanyazgan.pyvakitci.service.BootReceiver
import com.rahmanyazgan.pyvakitci.ui.theme.AppColors
import java.io.File

@Composable
fun AudioSettingsScreen(
    prefs: PreferencesManager,
    audioPlayer: AudioPlayerManager,
    colors: AppColors,
    modifier: Modifier = Modifier
) {
    val context = LocalContext.current
    val scrollState = rememberScrollState()
    var currentPlayingSound by remember { mutableStateOf<String?>(null) }
    var soundRefreshTrigger by remember { mutableIntStateOf(0) }

    var isWarningEnabled by remember { mutableStateOf(prefs.isWarningSoundEnabled) }
    var isVibrationEnabled by remember { mutableStateOf(prefs.isVibrationEnabled) }
    var volumeLevel by remember { mutableStateOf(prefs.volume.toFloat()) }

    var activePickerTarget by remember { mutableStateOf<String?>(null) }

    val audioPickerLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.GetContent()
    ) { uri: Uri? ->
        val target = activePickerTarget
        if (uri != null && target != null) {
            try {
                var displayName = "ozel_ses_${target}.mp3"
                context.contentResolver.query(uri, null, null, null, null)?.use { cursor ->
                    val nameIndex = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME)
                    if (nameIndex != -1 && cursor.moveToFirst()) {
                        displayName = cursor.getString(nameIndex) ?: displayName
                    }
                }

                val audioDir = File(context.filesDir, "custom_audio").apply { mkdirs() }
                val targetFile = File(audioDir, "custom_${prefs.normalizePrayerKey(target)}.mp3")
                context.contentResolver.openInputStream(uri)?.use { input ->
                    targetFile.outputStream().use { output ->
                        input.copyTo(output)
                    }
                }

                prefs.setPrayerSoundCustom(target, targetFile.absolutePath, displayName)
                soundRefreshTrigger++
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
        activePickerTarget = null
    }

    fun playOrStopKey(soundKey: String) {
        if (currentPlayingSound == soundKey) {
            audioPlayer.stopSound()
            currentPlayingSound = null
        } else {
            val source = prefs.getPrayerSound(soundKey)
            audioPlayer.playAudio(source, volumeLevel.toInt(), tag = "preview_$soundKey")
            currentPlayingSound = soundKey
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
            Triple("Sabah", "sabah_saba", "Sabah Ezanı"),
            Triple("Öğle", "ogle_rast", "Öğle Ezanı"),
            Triple("İkindi", "ikindi_hicaz", "İkindi Ezanı"),
            Triple("Akşam", "aksam_segah", "Akşam Ezanı"),
            Triple("Yatsı", "yatsi_ussak", "Yatsı Ezanı"),
            Triple("Dua", "ezan_duasi", "Ezan Duası"),
            Triple("Sela", "sela", "Cuma Selası")
        )

        prayers.forEach { (name, _, defaultDesc) ->
            val prayerKey = prefs.normalizePrayerKey(name)
            var enabled by remember { mutableStateOf(prefs.isPrayerSoundEnabled(prayerKey)) }
            val currentDisplayName = remember(soundRefreshTrigger) {
                prefs.getPrayerSoundDisplayName(prayerKey)
            }
            val isCustom = remember(soundRefreshTrigger) {
                prefs.isCustomSound(prayerKey)
            }

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
                    Column(
                        modifier = Modifier
                            .weight(1f)
                            .clickable {
                                activePickerTarget = prayerKey
                                audioPickerLauncher.launch("audio/*")
                            }
                    ) {
                        Text(
                            text = name,
                            fontSize = 15.sp,
                            fontWeight = FontWeight.Bold,
                            color = colors.text
                        )
                        Text(
                            text = if (isCustom) "$currentDisplayName (Özel Ses)" else "$defaultDesc: $currentDisplayName",
                            fontSize = 11.sp,
                            color = if (isCustom) colors.primary else colors.text.copy(alpha = 0.6f)
                        )
                    }

                    Row(verticalAlignment = Alignment.CenterVertically) {
                        // Dosya Seçme Butonu
                        IconButton(
                            onClick = {
                                activePickerTarget = prayerKey
                                audioPickerLauncher.launch("audio/*")
                            },
                            modifier = Modifier.size(34.dp)
                        ) {
                            Icon(
                                imageVector = Icons.Default.FolderOpen,
                                contentDescription = "Ses Dosyası Seç",
                                tint = colors.text.copy(alpha = 0.7f),
                                modifier = Modifier.size(20.dp)
                            )
                        }

                        // Özel ses seçilmişse varsayılana dönme butonu
                        if (isCustom) {
                            IconButton(
                                onClick = {
                                    prefs.resetPrayerSoundToDefault(prayerKey)
                                    soundRefreshTrigger++
                                },
                                modifier = Modifier.size(34.dp)
                            ) {
                                Icon(
                                    imageVector = Icons.Default.Refresh,
                                    contentDescription = "Varsayılana Dön",
                                    tint = colors.text.copy(alpha = 0.7f),
                                    modifier = Modifier.size(18.dp)
                                )
                            }
                        }

                        // Oynat / Durdur
                        IconButton(
                            onClick = { playOrStopKey(prayerKey) },
                            modifier = Modifier.size(34.dp)
                        ) {
                            Icon(
                                imageVector = if (currentPlayingSound == prayerKey) Icons.Default.Stop else Icons.Default.PlayArrow,
                                contentDescription = "Oynat",
                                tint = colors.primary,
                                modifier = Modifier.size(22.dp)
                            )
                        }

                        Spacer(modifier = Modifier.width(4.dp))

                        Checkbox(
                            checked = enabled,
                            onCheckedChange = {
                                enabled = it
                                prefs.setPrayerSoundEnabled(prayerKey, it)
                            }
                        )
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Warning Sound Card
        val warningKey = "uyari"
        val warningDisplayName = remember(soundRefreshTrigger) {
            prefs.getPrayerSoundDisplayName(warningKey)
        }
        val isWarningCustom = remember(soundRefreshTrigger) {
            prefs.isCustomSound(warningKey)
        }

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
                Column(
                    modifier = Modifier
                        .weight(1f)
                        .clickable {
                            activePickerTarget = warningKey
                            audioPickerLauncher.launch("audio/*")
                        }
                ) {
                    Text(
                        text = "Vakit Öncesi Uyarı Sesi",
                        fontSize = 15.sp,
                        fontWeight = FontWeight.Bold,
                        color = colors.text
                    )
                    Text(
                        text = if (isWarningCustom) "$warningDisplayName (Özel Ses)" else "Vakitten 15 dk önce: $warningDisplayName",
                        fontSize = 11.sp,
                        color = if (isWarningCustom) colors.primary else colors.text.copy(alpha = 0.6f)
                    )
                }

                Row(verticalAlignment = Alignment.CenterVertically) {
                    IconButton(
                        onClick = {
                            activePickerTarget = warningKey
                            audioPickerLauncher.launch("audio/*")
                        },
                        modifier = Modifier.size(34.dp)
                    ) {
                        Icon(
                            imageVector = Icons.Default.FolderOpen,
                            contentDescription = "Uyarı Sesi Dosyası Seç",
                            tint = colors.text.copy(alpha = 0.7f),
                            modifier = Modifier.size(20.dp)
                        )
                    }

                    if (isWarningCustom) {
                        IconButton(
                            onClick = {
                                prefs.resetPrayerSoundToDefault(warningKey)
                                soundRefreshTrigger++
                            },
                            modifier = Modifier.size(34.dp)
                        ) {
                            Icon(
                                imageVector = Icons.Default.Refresh,
                                contentDescription = "Varsayılana Dön",
                                tint = colors.text.copy(alpha = 0.7f),
                                modifier = Modifier.size(18.dp)
                            )
                        }
                    }

                    IconButton(
                        onClick = { playOrStopKey(warningKey) },
                        modifier = Modifier.size(34.dp)
                    ) {
                        Icon(
                            imageVector = if (currentPlayingSound == warningKey) Icons.Default.Stop else Icons.Default.PlayArrow,
                            contentDescription = "Oynat",
                            tint = colors.primary,
                            modifier = Modifier.size(22.dp)
                        )
                    }

                    Spacer(modifier = Modifier.width(4.dp))

                    Checkbox(
                        checked = isWarningEnabled,
                        onCheckedChange = {
                            isWarningEnabled = it
                            prefs.isWarningSoundEnabled = it
                            BootReceiver.schedulePrayerAlarms(context)
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
        Spacer(modifier = Modifier.height(16.dp))

        // Pil Optimizasyonu (Kesintisiz Ezan İzni) Kartı
        val powerManager = remember { context.getSystemService(android.content.Context.POWER_SERVICE) as? PowerManager }
        var isIgnoringBattery by remember {
            mutableStateOf(
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    powerManager?.isIgnoringBatteryOptimizations(context.packageName) == true
                } else true
            )
        }

        Card(
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(containerColor = colors.surface),
            elevation = CardDefaults.cardElevation(1.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = "Kesintisiz Ezan İzni (Pil Optimizasyonu)",
                            fontSize = 15.sp,
                            fontWeight = FontWeight.Bold,
                            color = colors.text
                        )
                        Spacer(modifier = Modifier.height(4.dp))
                        Text(
                            text = if (isIgnoringBattery)
                                "Pil kısıtlaması yok. Alarmlar ekran kapalıyken kesintisiz çalacaktır."
                            else
                                "Telefonunuz arka planda ezan ve bildirimleri geciktirmemesi için pil kısıtlamasını kaldırın.",
                            fontSize = 12.sp,
                            color = if (isIgnoringBattery) colors.primary else colors.text.copy(alpha = 0.6f)
                        )
                    }

                    if (!isIgnoringBattery) {
                        Spacer(modifier = Modifier.width(8.dp))
                        Button(
                            onClick = {
                                try {
                                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                                        val intent = Intent(Settings.ACTION_REQUEST_IGNORE_BATTERY_OPTIMIZATIONS).apply {
                                            data = Uri.parse("package:${context.packageName}")
                                        }
                                        context.startActivity(intent)
                                    }
                                } catch (_: Exception) {
                                    try {
                                        val intent = Intent(Settings.ACTION_IGNORE_BATTERY_OPTIMIZATION_SETTINGS)
                                        context.startActivity(intent)
                                    } catch (_: Exception) {}
                                }
                            },
                            colors = ButtonDefaults.buttonColors(containerColor = colors.primary),
                            shape = RoundedCornerShape(8.dp)
                        ) {
                            Text("İzin Ver", fontSize = 12.sp)
                        }
                    } else {
                        Spacer(modifier = Modifier.width(8.dp))
                        Icon(
                            imageVector = Icons.Default.Check,
                            contentDescription = "Etkin",
                            tint = colors.primary,
                            modifier = Modifier.size(24.dp)
                        )
                    }
                }
            }
        }
    }
}

