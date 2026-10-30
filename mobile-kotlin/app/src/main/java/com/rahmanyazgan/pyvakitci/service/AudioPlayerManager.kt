package com.rahmanyazgan.pyvakitci.service

import android.content.Context
import android.media.AudioAttributes
import android.media.AudioFocusRequest
import android.media.AudioManager
import android.media.MediaPlayer
import android.net.Uri
import android.os.Build
import android.os.PowerManager
import com.rahmanyazgan.pyvakitci.R
import java.io.File

class AudioPlayerManager(private val context: Context) {
    private var mediaPlayer: MediaPlayer? = null
    var onCompletionListener: (() -> Unit)? = null

    private val audioManager = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
    private var audioFocusRequest: AudioFocusRequest? = null

    companion object {
        @Volatile
        var activeInstance: AudioPlayerManager? = null

        @Volatile
        var currentPlayingTag: String? = null

        @Volatile
        var lastTriggeredKey: String? = null

        fun isAnyAudioPlaying(): Boolean {
            return activeInstance?.isPlaying() == true
        }

        fun stopActiveSound() {
            try {
                activeInstance?.stopSound()
            } catch (_: Exception) {}
            activeInstance = null
            currentPlayingTag = null
        }
    }

    fun isInCall(): Boolean {
        return try {
            audioManager.mode == AudioManager.MODE_IN_CALL ||
            audioManager.mode == AudioManager.MODE_IN_COMMUNICATION ||
            audioManager.mode == AudioManager.MODE_RINGTONE
        } catch (_: Exception) {
            false
        }
    }

    private fun requestAudioFocus() {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                audioFocusRequest = AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK)
                    .setAudioAttributes(
                        AudioAttributes.Builder()
                            .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                            .setUsage(AudioAttributes.USAGE_ALARM)
                            .build()
                    )
                    .setOnAudioFocusChangeListener { /* Focus değişiminde susturma/devam */ }
                    .build()
                audioFocusRequest?.let { audioManager.requestAudioFocus(it) }
            } else {
                @Suppress("DEPRECATION")
                audioManager.requestAudioFocus(null, AudioManager.STREAM_ALARM, AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK)
            }
        } catch (_: Exception) {}
    }

    private fun abandonAudioFocus() {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                audioFocusRequest?.let { audioManager.abandonAudioFocusRequest(it) }
                audioFocusRequest = null
            } else {
                @Suppress("DEPRECATION")
                audioManager.abandonAudioFocus(null)
            }
        } catch (_: Exception) {}
    }

    fun playSound(rawResId: Int, volumePercent: Int = 100, tag: String = "") {
        if (isInCall()) {
            val cb = onCompletionListener
            onCompletionListener = null
            cb?.invoke()
            return
        }

        // Eğer bu etiketli ses zaten çalıyorsa mükerrer başlatma
        if (isAnyAudioPlaying() && tag.isNotEmpty() && tag == currentPlayingTag) {
            return
        }

        stopActiveSound()
        try {
            requestAudioFocus()
            activeInstance = this
            currentPlayingTag = tag
            mediaPlayer = MediaPlayer.create(context, rawResId)?.apply {
                setAudioAttributes(
                    AudioAttributes.Builder()
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .setUsage(AudioAttributes.USAGE_ALARM)
                        .build()
                )
                try {
                    setWakeMode(context.applicationContext, PowerManager.PARTIAL_WAKE_LOCK)
                } catch (_: Exception) {}
                val vol = (volumePercent.coerceIn(0, 100) / 100f)
                setVolume(vol, vol)
                setOnCompletionListener {
                    val cb = onCompletionListener
                    onCompletionListener = null
                    stopSound()
                    cb?.invoke()
                }
                start()
            }
        } catch (_: Exception) {
            abandonAudioFocus()
            if (activeInstance === this) {
                activeInstance = null
                currentPlayingTag = null
            }
            val cb = onCompletionListener
            onCompletionListener = null
            cb?.invoke()
        }
    }

    fun playAudio(soundSource: String, volumePercent: Int = 100, tag: String = "") {
        if (isInCall()) {
            val cb = onCompletionListener
            onCompletionListener = null
            cb?.invoke()
            return
        }

        // Eğer bu etiketli ses zaten çalıyorsa mükerrer başlatma
        if (isAnyAudioPlaying() && tag.isNotEmpty() && tag == currentPlayingTag) {
            return
        }

        stopActiveSound()
        try {
            requestAudioFocus()
            activeInstance = this
            currentPlayingTag = tag
            val isCustomFile = soundSource.startsWith("/") ||
                               soundSource.startsWith("content://") ||
                               File(soundSource).exists()

            if (isCustomFile) {
                mediaPlayer = MediaPlayer().apply {
                    setAudioAttributes(
                        AudioAttributes.Builder()
                            .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                            .setUsage(AudioAttributes.USAGE_ALARM)
                            .build()
                    )
                    try {
                        setWakeMode(context.applicationContext, PowerManager.PARTIAL_WAKE_LOCK)
                    } catch (_: Exception) {}
                    if (soundSource.startsWith("content://")) {
                        setDataSource(context, Uri.parse(soundSource))
                    } else {
                        setDataSource(soundSource)
                    }
                    val vol = (volumePercent.coerceIn(0, 100) / 100f)
                    setVolume(vol, vol)
                    setOnCompletionListener {
                        val cb = onCompletionListener
                        onCompletionListener = null
                        stopSound()
                        cb?.invoke()
                    }
                    prepare()
                    start()
                }
            } else {
                val resId = getRawResourceId(soundSource)
                if (resId != 0) {
                    playSound(resId, volumePercent, tag)
                } else {
                    abandonAudioFocus()
                    if (activeInstance === this) {
                        activeInstance = null
                        currentPlayingTag = null
                    }
                    val cb = onCompletionListener
                    onCompletionListener = null
                    cb?.invoke()
                }
            }
        } catch (_: Exception) {
            abandonAudioFocus()
            if (activeInstance === this) {
                activeInstance = null
                currentPlayingTag = null
            }
            val cb = onCompletionListener
            onCompletionListener = null
            cb?.invoke()
        }
    }

    fun playByName(soundName: String, volumePercent: Int = 100, tag: String = "") {
        playAudio(soundName, volumePercent, tag)
    }

    fun stopSound() {
        abandonAudioFocus()
        try {
            mediaPlayer?.let {
                if (it.isPlaying) {
                    it.stop()
                }
                it.release()
            }
        } catch (_: Exception) {}
        mediaPlayer = null
        if (activeInstance === this) {
            activeInstance = null
            currentPlayingTag = null
        }
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
