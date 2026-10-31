import Foundation
import UserNotifications

public class NotificationService: NSObject, UNUserNotificationCenterDelegate {
    public static let shared = NotificationService()

    override private init() {
        super.init()
    }

    public func requestAuthorization() {
        let center = UNUserNotificationCenter.current()
        center.delegate = self
        center.requestAuthorization(options: [.alert, .badge, .sound]) { granted, error in
            if granted {
                print("[PyVakitci] Bildirim izni onaylandi.")
            } else if let error = error {
                print("[PyVakitci] Bildirim izni hatasi: \(error)")
            }
        }
    }

    // On plandayken (uygulama acikken) bildirimlerin ekranda gorunmesi
    public func userNotificationCenter(
        _ center: UNUserNotificationCenter,
        willPresent notification: UNNotification,
        withCompletionHandler completionHandler: @escaping (UNNotificationPresentationOptions) -> Void
    ) {
        if #available(iOS 14.0, *) {
            completionHandler([.banner, .sound, .badge])
        } else {
            completionHandler([.alert, .sound, .badge])
        }
    }

    public func schedulePrayerNotifications(times: PrayerTimes, locationText: String) {
        schedulePrayerNotifications(timesList: [times], locationText: locationText)
    }

    public func schedulePrayerNotifications(timesList: [PrayerTimes], locationText: String) {
        let center = UNUserNotificationCenter.current()
        center.removeAllPendingNotificationRequests()

        guard !timesList.isEmpty else { return }

        let warningEnabled = LocalStorageService.shared.isWarningSoundEnabled()
        let warningMinutes = LocalStorageService.shared.getWarningMinutes()

        let calendar = Calendar.current
        let now = Date()
        let todayStr = TimeUtil.getTodayString(date: now)

        // Bugun ve sonraki gunleri filtrele (iOS 64 bildirim limitine takilmamak icin en fazla 6 gun: 6 gun x 10 bildirim = 60)
        let upcomingDays = timesList.filter { $0.date >= todayStr }.prefix(6)

        var scheduledCount = 0

        for dayTimes in upcomingDays {
            let dateParts = dayTimes.date.split(separator: "-")
            guard dateParts.count == 3,
                  let year = Int(dateParts[0]),
                  let month = Int(dateParts[1]),
                  let day = Int(dateParts[2]) else { continue }

            let prayers: [(name: String, timeStr: String)] = [
                ("İmsak", dayTimes.imsak),
                ("Öğle", dayTimes.ogle),
                ("İkindi", dayTimes.ikindi),
                ("Akşam", dayTimes.aksam),
                ("Yatsı", dayTimes.yatsi)
            ]

            for (name, timeStr) in prayers {
                let parts = timeStr.split(separator: ":")
                guard parts.count >= 2,
                      let hour = Int(parts[0]),
                      let minute = Int(parts[1]) else { continue }

                var prayerComponents = DateComponents()
                prayerComponents.year = year
                prayerComponents.month = month
                prayerComponents.day = day
                prayerComponents.hour = hour
                prayerComponents.minute = minute
                prayerComponents.second = 0

                guard let prayerDate = calendar.date(from: prayerComponents) else { continue }

                // 1. VAKIT ONCESI UYARI BILDIRIMI (Eger vakit henuz gecmediyse)
                if warningEnabled && warningMinutes > 0 {
                    let warnDate = prayerDate.addingTimeInterval(-Double(warningMinutes * 60))
                    if warnDate > now {
                        let warnComponents = calendar.dateComponents([.year, .month, .day, .hour, .minute, .second], from: warnDate)
                        let warnContent = UNMutableNotificationContent()
                        let localizedPrayer = LocalizationManager.shared.prayerFromTr(name)
                        warnContent.title = LocalizationManager.shared.tr("n_warn_title")
                        warnContent.body = LocalizationManager.shared.f("n_warn_text", localizedPrayer, warningMinutes)
                        warnContent.sound = UNNotificationSound(named: UNNotificationSoundName("uyari.mp3"))

                        let warnTrigger = UNCalendarNotificationTrigger(dateMatching: warnComponents, repeats: false)
                        let warnReq = UNNotificationRequest(
                            identifier: "warn_\(dayTimes.date)_\(name)",
                            content: warnContent,
                            trigger: warnTrigger
                        )
                        center.add(warnReq)
                        scheduledCount += 1
                    }
                }

                // 2. VAKIT ANI BILDIRIMI (Eger vakit henuz gecmediyse)
                if prayerDate > now {
                    let soundMode = LocalStorageService.shared.getPrayerSoundMode(prayerName: name)
                    let content = UNMutableNotificationContent()
                    let localizedPrayer = LocalizationManager.shared.prayerFromTr(name)
                    content.title = LocalizationManager.shared.f("n_prayer_title", localizedPrayer)
                    content.body = LocalizationManager.shared.f("n_prayer_text", locationText, timeStr)

                    switch soundMode {
                    case .ezan:
                        let soundName = AudioPlayerService.getSoundFileForPrayer(prayerName: name) + ".mp3"
                        content.sound = UNNotificationSound(named: UNNotificationSoundName(soundName))
                    case .bip:
                        content.sound = UNNotificationSound(named: UNNotificationSoundName("uyari.mp3"))
                    case .sessiz:
                        content.sound = nil
                    }

                    let trigger = UNCalendarNotificationTrigger(dateMatching: prayerComponents, repeats: false)
                    let request = UNNotificationRequest(
                        identifier: "prayer_\(dayTimes.date)_\(name)",
                        content: content,
                        trigger: trigger
                    )
                    center.add(request)
                    scheduledCount += 1
                }
            }
        }

        print("[PyVakitci] Toplam \(scheduledCount) adet bildirim basariyla zamanlandi.")
    }

    public func rescheduleCurrentLocation() {
        let loc = LocalStorageService.shared.loadLocation()
        let cached = LocalStorageService.shared.loadPrayerTimes(districtId: loc.districtId)
        if !cached.isEmpty {
            schedulePrayerNotifications(timesList: cached, locationText: "\(loc.cityName) / \(loc.districtName)")
        }
    }
}

