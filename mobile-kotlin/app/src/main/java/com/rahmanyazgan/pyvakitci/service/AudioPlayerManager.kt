package com.rahmanyazgan.pyvakitci.service

import android.content.Context
import android.media.AudioAttributes
import android.media.MediaPlayer
import com.rahmanyazgan.pyvakitci.R

class AudioPlayerManager(private val context: Context) {
    private var mediaPlayer: MediaPlayer? = null
    var onCompletionListener: (() -> Unit)? = null

    fun playSound(rawResId: Int, volumePercent: Int = 100) {
        stopSound()
        try {
            mediaPlayer = MediaPlayer.create(context, rawResId)?.apply {
                setAudioAttributes(
                    AudioAttributes.Builder()
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .setUsage(AudioAttributes.USAGE_ALARM)
                        .build()
                )
                val vol = (volumePercent.coerceIn(0, 100) / 100f)
                setVolume(vol, vol)
                setOnCompletionListener {
                    onCompletionListener?.invoke()
                    stopSound()
                }
                start()
            }
        } catch (_: Exception) {}
    }

    fun playByName(soundName: String, volumePercent: Int = 100) {
        val resId = getRawResourceId(soundName)
        if (resId != 0) {
            playSound(resId, volumePercent)
        }
    }

    fun stopSound() {
        try {
            mediaPlayer?.let {
                if (it.isPlaying) {
                    it.stop()
                }
                it.release()
            }
        } catch (_: Exception) {}
        mediaPlayer = null
    }

    fun isPlaying(): Boolean = mediaPlayer?.isPlaying == true

    fun getRawResourceId(name: String): Int {
        val clean = name.lowercase().removeSuffix(".mp3")
        return when (clean) {
            "sabah_saba" -> R.raw.sabah_saba
            "ogle_rast" -> R.raw.ogle_rast
            "ikindi_hicaz" -> R.raw.ikindi_hicaz
            "aksam_segah" -> R.raw.aksam_segah
            "yatsi_ussak" -> R.raw.yatsi_ussak
            "ezan_duasi" -> R.raw.ezan_duasi
            "ezan_duasi_ar_tr" -> R.raw.ezan_duasi_ar_tr
            "sela" -> R.raw.sela
            "uyari" -> R.raw.uyari
            "besmele" -> R.raw.besmele
            else -> R.raw.uyari
        }
    }
}

