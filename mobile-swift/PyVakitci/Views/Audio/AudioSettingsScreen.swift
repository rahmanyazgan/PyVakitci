import SwiftUI

public struct AudioSettingsScreen: View {
    @ObservedObject private var loc = LocalizationManager.shared
    @ObservedObject public var audioPlayer = AudioPlayerService.shared
    public let colors: AppThemeColors

    @State private var volume: Double = LocalStorageService.shared.getVolume()
    @State private var isVibrationEnabled: Bool = LocalStorageService.shared.isVibrationEnabled()
    @State private var isWarningEnabled: Bool = LocalStorageService.shared.isWarningSoundEnabled()
    @State private var warningMinutes: Int = LocalStorageService.shared.getWarningMinutes()
    @State private var isEzanDuasiEnabled: Bool = LocalStorageService.shared.isEzanDuasiEnabled()

    public init(colors: AppThemeColors) {
        self.colors = colors
    }

    private let mainPrayers = [
        ("Sabah", "sabah_saba", "Sabah Ezanı (Saba Makamı)"),
        ("Öğle", "ogle_rast", "Öğle Ezanı (Rast Makamı)"),
        ("İkindi", "ikindi_hicaz", "İkindi Ezanı (Hicaz Makamı)"),
        ("Akşam", "aksam_segah", "Akşam Ezanı (Segah Makamı)"),
        ("Yatsı", "yatsi_ussak", "Yatsı Ezanı (Uşşak Makamı)")
    ]

    private let minuteOptions = [5, 10, 15, 20, 30, 45, 60]

    public var body: some View {
        ScrollView(showsIndicators: false) {
            VStack(alignment: .leading, spacing: 18) {
                Text(loc.tr("a_title"))
                    .font(.system(size: 22, weight: .bold))
                    .foregroundColor(colors.text)

                // 1. Vakit Ses Modları (Ezan / Bip / Sessiz)
                VStack(alignment: .leading, spacing: 12) {
                    Text(loc.tr("a_sound_mode").uppercased(with: loc.currentLocale))
                        .font(.system(size: 12, weight: .bold))
                        .foregroundColor(colors.primary)

                    ForEach(mainPrayers, id: \.0) { item in
                        PrayerSoundModeCard(
                            name: item.0,
                            soundFile: item.1,
                            desc: item.2,
                            colors: colors,
                            isPlaying: audioPlayer.currentlyPlaying == item.1,
                            onPlayToggle: {
                                togglePlay(soundName: item.1)
                            },
                            onModeChanged: {
                                NotificationService.shared.rescheduleCurrentLocation()
                            }
                        )
                    }
                }

                // 2. Vakit Öncesi Uyarı Kartı
                VStack(alignment: .leading, spacing: 12) {
                    Text(loc.tr("a_warn_title").uppercased(with: loc.currentLocale))
                        .font(.system(size: 12, weight: .bold))
                        .foregroundColor(colors.primary)

                    VStack(spacing: 12) {
                        HStack {
                            VStack(alignment: .leading, spacing: 2) {
                                Text(loc.tr("a_warn_title"))
                                    .font(.system(size: 15, weight: .bold))
                                    .foregroundColor(colors.text)

                                Text(loc.f("a_warn_desc", warningMinutes))
                                    .font(.system(size: 11))
                                    .foregroundColor(colors.text.opacity(0.6))
                            }

                            Spacer()

                            Button(action: {
                                togglePlay(soundName: "uyari")
                            }) {
                                Image(systemName: audioPlayer.currentlyPlaying == "uyari" ? "stop.circle.fill" : "play.circle.fill")
                                    .font(.system(size: 28))
                                    .foregroundColor(colors.primary)
                            }

                            Toggle("", isOn: $isWarningEnabled)
                                .labelsHidden()
                                .onChange(of: isWarningEnabled) { val in
                                    LocalStorageService.shared.setWarningSoundEnabled(val)
                                    NotificationService.shared.rescheduleCurrentLocation()
                                }
                        }

                        if isWarningEnabled {
                            Divider()

                            HStack {
                                Text("Kaç Dakika Önce:")
                                    .font(.system(size: 13, weight: .medium))
                                    .foregroundColor(colors.text)

                                Spacer()

                                Picker("Dakika", selection: $warningMinutes) {
                                    ForEach(minuteOptions, id: \.self) { min in
                                        Text("\(min) dk önce").tag(min)
                                    }
                                }
                                .pickerStyle(.menu)
                                .padding(.horizontal, 10)
                                .padding(.vertical, 4)
                                .background(colors.background)
                                .cornerRadius(8)
                                .onChange(of: warningMinutes) { val in
                                    LocalStorageService.shared.setWarningMinutes(val)
                                    NotificationService.shared.rescheduleCurrentLocation()
                                }
                            }
                        }
                    }
                    .padding(14)
                    .background(colors.surface)
                    .cornerRadius(14)
                    .shadow(color: Color.black.opacity(0.02), radius: 2, y: 1)
                }

                // 3. Ezan Duası ve Cuma Selası
                VStack(alignment: .leading, spacing: 12) {
                    Text("ÖZEL SESLER VE DUA")
                        .font(.system(size: 12, weight: .bold))
                        .foregroundColor(colors.primary)

                    // Ezan Duasi
                    HStack {
                        VStack(alignment: .leading, spacing: 2) {
                            Text("Ezan Duası")
                                .font(.system(size: 15, weight: .bold))
                                .foregroundColor(colors.text)

                            Text("Ezan bittikten sonra otomatik olarak çalar")
                                .font(.system(size: 11))
                                .foregroundColor(colors.text.opacity(0.6))
                        }

                        Spacer()

                        Button(action: {
                            togglePlay(soundName: "ezan_duasi")
                        }) {
                            Image(systemName: audioPlayer.currentlyPlaying == "ezan_duasi" ? "stop.circle.fill" : "play.circle.fill")
                                .font(.system(size: 28))
                                .foregroundColor(colors.primary)
                        }

                        Toggle("", isOn: $isEzanDuasiEnabled)
                            .labelsHidden()
                            .onChange(of: isEzanDuasiEnabled) { val in
                                LocalStorageService.shared.setEzanDuasiEnabled(val)
                            }
                    }
                    .padding(14)
                    .background(colors.surface)
                    .cornerRadius(14)
                    .shadow(color: Color.black.opacity(0.02), radius: 2, y: 1)

                    // Cuma Selasi
                    HStack {
                        VStack(alignment: .leading, spacing: 2) {
                            Text("Cuma Selası")
                                .font(.system(size: 15, weight: .bold))
                                .foregroundColor(colors.text)

                            Text("Cuma namazı öncesi sala kaydı")
                                .font(.system(size: 11))
                                .foregroundColor(colors.text.opacity(0.6))
                        }

                        Spacer()

                        Button(action: {
                            togglePlay(soundName: "sela")
                        }) {
                            Image(systemName: audioPlayer.currentlyPlaying == "sela" ? "stop.circle.fill" : "play.circle.fill")
                                .font(.system(size: 28))
                                .foregroundColor(colors.primary)
                        }
                    }
                    .padding(14)
                    .background(colors.surface)
                    .cornerRadius(14)
                    .shadow(color: Color.black.opacity(0.02), radius: 2, y: 1)
                }

                // 4. Ses Seviyesi ve Titreşim
                VStack(alignment: .leading, spacing: 14) {
                    Text("GENEL SES AYARLARI")
                        .font(.system(size: 12, weight: .bold))
                        .foregroundColor(colors.primary)

                    VStack(alignment: .leading, spacing: 14) {
                        Text("Ses Seviyesi: %\(Int(volume * 100))")
                            .font(.system(size: 14, weight: .semibold))
                            .foregroundColor(colors.text)

                        Slider(value: $volume, in: 0...1) { _ in
                            LocalStorageService.shared.setVolume(volume)
                        }

                        Divider()

                        Toggle(isOn: $isVibrationEnabled) {
                            Text("Vakitlerde Titreşim")
                                .font(.system(size: 14))
                                .foregroundColor(colors.text)
                        }
                        .onChange(of: isVibrationEnabled) { val in
                            LocalStorageService.shared.setVibrationEnabled(val)
                        }
                    }
                    .padding(16)
                    .background(colors.surface)
                    .cornerRadius(16)
                    .shadow(color: Color.black.opacity(0.02), radius: 2, y: 1)
                }
            }
            .padding(16)
        }
        .background(colors.background.ignoresSafeArea())
    }

    private func togglePlay(soundName: String) {
        if audioPlayer.currentlyPlaying == soundName {
            audioPlayer.stop()
        } else {
            audioPlayer.play(soundName: soundName, volume: Float(volume))
        }
    }
}

public struct PrayerSoundModeCard: View {
    public let name: String
    public let soundFile: String
    public let desc: String
    public let colors: AppThemeColors
    public let isPlaying: Bool
    public let onPlayToggle: () -> Void
    public let onModeChanged: () -> Void

    @State private var currentMode: PrayerSoundMode

    public init(
        name: String,
        soundFile: String,
        desc: String,
        colors: AppThemeColors,
        isPlaying: Bool,
        onPlayToggle: @escaping () -> Void,
        onModeChanged: @escaping () -> Void
    ) {
        self.name = name
        self.soundFile = soundFile
        self.desc = desc
        self.colors = colors
        self.isPlaying = isPlaying
        self.onPlayToggle = onPlayToggle
        self.onModeChanged = onModeChanged
        _currentMode = State(initialValue: LocalStorageService.shared.getPrayerSoundMode(prayerName: name))
    }

    public var body: some View {
        VStack(spacing: 10) {
            HStack {
                VStack(alignment: .leading, spacing: 2) {
                    Text(LocalizationManager.shared.prayerFromTr(name))
                        .font(.system(size: 16, weight: .bold))
                        .foregroundColor(colors.text)

                    Text(desc)
                        .font(.system(size: 11))
                        .foregroundColor(colors.text.opacity(0.6))
                }

                Spacer()

                Button(action: onPlayToggle) {
                    Image(systemName: isPlaying ? "stop.circle.fill" : "play.circle.fill")
                        .font(.system(size: 26))
                        .foregroundColor(colors.primary)
                }
            }

            // 3'lu Mod Secici: Ezan / Bip / Sessiz
            HStack(spacing: 8) {
                ForEach(PrayerSoundMode.allCases) { mode in
                    Button(action: {
                        currentMode = mode
                        LocalStorageService.shared.setPrayerSoundMode(prayerName: name, mode: mode)
                        onModeChanged()
                    }) {
                        HStack(spacing: 4) {
                            Image(systemName: iconForMode(mode))
                                .font(.system(size: 11))
                            Text(mode.localizedTitle)
                                .font(.system(size: 12, weight: .semibold))
                        }
                        .foregroundColor(currentMode == mode ? .white : colors.text)
                        .frame(maxWidth: .infinity)
                        .padding(.vertical, 7)
                        .background(currentMode == mode ? colors.primary : colors.background)
                        .cornerRadius(8)
                    }
                }
            }
        }
        .padding(14)
        .background(colors.surface)
        .cornerRadius(14)
        .shadow(color: Color.black.opacity(0.02), radius: 2, y: 1)
    }

    private func iconForMode(_ mode: PrayerSoundMode) -> String {
        switch mode {
        case .ezan: return "speaker.wave.3.fill"
        case .bip: return "bell.fill"
        case .sessiz: return "speaker.slash.fill"
        }
    }
}

