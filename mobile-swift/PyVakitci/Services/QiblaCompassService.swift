import Foundation
import CoreLocation
import Combine
import UIKit

public class QiblaCompassService: NSObject, ObservableObject, CLLocationManagerDelegate {
    public static let shared = QiblaCompassService()

    private let locationManager = CLLocationManager()
    private var lastHapticTime: Date = Date.distantPast

    @Published public var heading: Double = 0.0
    @Published public var qiblaAngle: Double = 0.0
    @Published public var relativeQiblaAngle: Double = 0.0
    @Published public var isAligned: Bool = false
    @Published public var hasGpsFix: Bool = false

    public var currentLatitude: Double = 41.0082 // Istanbul
    public var currentLongitude: Double = 28.9784

    private let kaabaLat = 21.4225
    private let kaabaLon = 39.8262

    override public init() {
        super.init()
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        locationManager.headingFilter = 1.0
    }

    public func start() {
        locationManager.requestWhenInUseAuthorization()
        locationManager.startUpdatingLocation()
        if CLLocationManager.headingAvailable() {
            locationManager.startUpdatingHeading()
        }
    }

    public func stop() {
        locationManager.stopUpdatingHeading()
        locationManager.stopUpdatingLocation()
    }

    public func refreshLocation() {
        locationManager.requestLocation()
    }

    public func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let loc = locations.last else { return }
        self.currentLatitude = loc.coordinate.latitude
        self.currentLongitude = loc.coordinate.longitude
        self.hasGpsFix = true

        let bearing = calculateQiblaBearing(lat: currentLatitude, lon: currentLongitude)
        self.qiblaAngle = bearing
    }

    public func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        // Location fallback to saved district
    }

    public func locationManager(_ manager: CLLocationManager, didUpdateHeading newHeading: CLHeading) {
        // Prefer trueHeading if available, otherwise magneticHeading
        let currentHead = newHeading.trueHeading > 0 ? newHeading.trueHeading : newHeading.magneticHeading
        self.heading = currentHead

        let bearing = calculateQiblaBearing(lat: currentLatitude, lon: currentLongitude)
        self.qiblaAngle = bearing

        var rel = bearing - currentHead
        while rel < 0 { rel += 360 }
        while rel >= 360 { rel -= 360 }
        self.relativeQiblaAngle = rel

        let aligned = rel <= 3.5 || rel >= 356.5
        self.isAligned = aligned

        if aligned && Date().timeIntervalSince(lastHapticTime) > 2.5 {
            lastHapticTime = Date()
            triggerHaptic()
        }
    }

    private func triggerHaptic() {
        DispatchQueue.main.async {
            let generator = UIImpactFeedbackGenerator(style: .medium)
            generator.prepare()
            generator.impactOccurred()
        }
    }

    private func calculateQiblaBearing(lat: Double, lon: Double) -> Double {
        let latRad = lat * .pi / 180.0
        let kaabaLatRad = kaabaLat * .pi / 180.0
        let dLonRad = (kaabaLon - lon) * .pi / 180.0

        let y = sin(dLonRad) * cos(kaabaLatRad)
        let x = cos(latRad) * sin(kaabaLatRad) - sin(latRad) * cos(kaabaLatRad) * cos(dLonRad)

        var bearing = atan2(y, x) * 180.0 / .pi
        while bearing < 0 { bearing += 360.0 }
        return bearing
    }
}
