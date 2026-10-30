import Foundation
import UserNotifications

public class NotificationService {
    public static let shared = NotificationService()

    public func requestAuthorization() {
        UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .badge, .sound]) { granted, error in
            if granted {
                print("Notification permission granted")
            }
        }
    }

    public func schedulePrayerNotifications(times: PrayerTimes, locationText: String) {
        UNUserNotificationCenter.current().removeAllPendingNotificationRequests()

        let prayers = [
            ("İmsak", times.imsak),
            ("Öğle", times.ogle),
            ("İkindi", times.ikindi),
            ("Akşam", times.aksam),
            ("Yatsı", times.yatsi)
        ]

        for (name, timeStr) in prayers {
            let parts = timeStr.split(separator: ":")
            guard parts.count >= 2,
                  let hour = Int(parts[0]),
                  let minute = Int(parts[1]) else { continue }

            var dateComponents = DateComponents()
            dateComponents.hour = hour
            dateComponents.minute = minute

            let content = UNMutableNotificationContent()
            content.title = "\(name) Vakti Girdi"
            content.body = "\(locationText) | Saat: \(timeStr)"
            content.sound = UNNotificationSound.default

            let trigger = UNCalendarNotificationTrigger(dateMatching: dateComponents, repeats: false)
            let request = UNNotificationRequest(identifier: "prayer_\(name)", content: content, trigger: trigger)

            UNUserNotificationCenter.current().add(request)
        }
    }
}

