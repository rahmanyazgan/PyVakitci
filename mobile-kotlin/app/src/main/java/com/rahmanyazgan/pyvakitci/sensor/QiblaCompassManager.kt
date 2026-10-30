package com.rahmanyazgan.pyvakitci.sensor

import android.annotation.SuppressLint
import android.content.Context
import android.hardware.GeomagneticField
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.location.Location
import android.location.LocationListener
import android.location.LocationManager
import android.os.Build
import android.os.Bundle
import android.os.VibrationEffect
import android.os.Vibrator
import kotlin.math.atan2
import kotlin.math.cos
import kotlin.math.sin

class QiblaCompassManager(
    private val context: Context,
    private val onCompassUpdate: (azimuth: Float, qiblaAngle: Float, isAligned: Boolean, hasGps: Boolean) -> Unit
) : SensorEventListener, LocationListener {

    private val sensorManager = context.getSystemService(Context.SENSOR_SERVICE) as SensorManager
    private val locationManager = context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
    private val vibrator = context.getSystemService(Context.VIBRATOR_SERVICE) as? Vibrator

    private val rotationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR)
    private val accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)
    private val magnetometer = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD)

    private val rotationMatrix = FloatArray(9)
    private val orientationAngles = FloatArray(3)

    private var gravityValues = FloatArray(3)
    private var geoMagneticValues = FloatArray(3)
    private var hasGravity = false
    private var hasGeomagnetic = false

    var currentLatitude: Double = 41.0082 // Default: Istanbul
    var currentLongitude: Double = 28.9784
    var hasGpsFix: Boolean = false

    private var smoothedAzimuth = 0f
    private var lastVibrateTime = 0L

    fun start() {
        if (rotationSensor != null) {
            sensorManager.registerListener(this, rotationSensor, SensorManager.SENSOR_DELAY_UI)
        } else {
            sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_UI)
            sensorManager.registerListener(this, magnetometer, SensorManager.SENSOR_DELAY_UI)
        }
        requestLocationUpdates()
    }

    fun stop() {
        sensorManager.unregisterListener(this)
        try {
            locationManager.removeUpdates(this)
        } catch (_: Exception) {}
    }

    @SuppressLint("MissingPermission")
    fun requestLocationUpdates() {
        try {
            val providers = listOf(LocationManager.GPS_PROVIDER, LocationManager.NETWORK_PROVIDER)
            for (p in providers) {
                if (locationManager.isProviderEnabled(p)) {
                    val lastLoc = locationManager.getLastKnownLocation(p)
                    if (lastLoc != null) {
                        currentLatitude = lastLoc.latitude
                        currentLongitude = lastLoc.longitude
                        hasGpsFix = true
                    }
                    locationManager.requestLocationUpdates(p, 5000L, 5f, this)
                }
            }
        } catch (_: Exception) {
            // Permission not granted or provider disabled
        }
    }

    override fun onLocationChanged(location: Location) {
        currentLatitude = location.latitude
        currentLongitude = location.longitude
        hasGpsFix = true
    }

    @Deprecated("Deprecated in Java")
    override fun onStatusChanged(provider: String?, status: Int, extras: Bundle?) {}
    override fun onProviderEnabled(provider: String) {}
    override fun onProviderDisabled(provider: String) {}

    override fun onSensorChanged(event: SensorEvent) {
        var rawAzimuthDeg = 0f

        if (event.sensor.type == Sensor.TYPE_ROTATION_VECTOR) {
            SensorManager.getRotationMatrixFromVector(rotationMatrix, event.values)
            SensorManager.getOrientation(rotationMatrix, orientationAngles)
            rawAzimuthDeg = (Math.toDegrees(orientationAngles[0].toDouble()).toFloat() + 360f) % 360f
        } else {
            if (event.sensor.type == Sensor.TYPE_ACCELEROMETER) {
                gravityValues = event.values.clone()
                hasGravity = true
            } else if (event.sensor.type == Sensor.TYPE_MAGNETIC_FIELD) {
                geoMagneticValues = event.values.clone()
                hasGeomagnetic = true
            }

            if (hasGravity && hasGeomagnetic) {
                if (SensorManager.getRotationMatrix(rotationMatrix, null, gravityValues, geoMagneticValues)) {
                    SensorManager.getOrientation(rotationMatrix, orientationAngles)
                    rawAzimuthDeg = (Math.toDegrees(orientationAngles[0].toDouble()).toFloat() + 360f) % 360f
                }
            }
        }

        // True North correction via GeomagneticField (declination)
        val declination = try {
            val geoField = GeomagneticField(
                currentLatitude.toFloat(),
                currentLongitude.toFloat(),
                50f,
                System.currentTimeMillis()
            )
            geoField.declination
        } catch (_: Exception) {
            0f
        }

        val trueAzimuth = (rawAzimuthDeg + declination + 360f) % 360f

        // Exponential smoothing filter to avoid jitter
        smoothedAzimuth = smoothAngle(smoothedAzimuth, trueAzimuth, 0.18f)

        // Calculate Great Circle bearing to Kaaba
        val qiblaBearing = calculateQiblaBearing(currentLatitude, currentLongitude)

        // Relative angle on screen (phone rotation vs Kaaba direction)
        var relativeQiblaAngle = (qiblaBearing - smoothedAzimuth + 360f) % 360f

        // Alignment check (within ±3.5 degrees of true Qibla)
        val isAligned = relativeQiblaAngle <= 3.5f || relativeQiblaAngle >= 356.5f

        // Vibrate gently once when entering alignment
        if (isAligned && System.currentTimeMillis() - lastVibrateTime > 2500) {
            lastVibrateTime = System.currentTimeMillis()
            vibrateFeedback()
        }

        onCompassUpdate(smoothedAzimuth, relativeQiblaAngle, isAligned, hasGpsFix)
    }

    private fun smoothAngle(current: Float, target: Float, alpha: Float): Float {
        var diff = target - current
        while (diff < -180f) diff += 360f
        while (diff > 180f) diff -= 360f
        return (current + alpha * diff + 360f) % 360f
    }

    private fun vibrateFeedback() {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                vibrator?.vibrate(VibrationEffect.createOneShot(70, VibrationEffect.DEFAULT_AMPLITUDE))
            } else {
                @Suppress("DEPRECATION")
                vibrator?.vibrate(70)
            }
        } catch (_: Exception) {}
    }

    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {}

    companion object {
        private const val KAABA_LAT = 21.4225
        private const val KAABA_LON = 39.8262

        fun calculateQiblaBearing(lat: Double, lon: Double): Float {
            val latRad = Math.toRadians(lat)
            val kaabaLatRad = Math.toRadians(KAABA_LAT)
            val dLonRad = Math.toRadians(KAABA_LON - lon)

            val y = sin(dLonRad) * cos(kaabaLatRad)
            val x = cos(latRad) * sin(kaabaLatRad) - sin(latRad) * cos(kaabaLatRad) * cos(dLonRad)

            val bearing = Math.toDegrees(atan2(y, x))
            return ((bearing + 360.0) % 360.0).toFloat()
        }
    }
}
