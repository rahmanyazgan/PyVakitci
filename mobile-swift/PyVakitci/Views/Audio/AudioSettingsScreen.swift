import SwiftUI

public struct AudioSettingsScreen: View {
    @ObservedObject public var audioPlayer = AudioPlayerService.shared
    public let colors: AppThemeColors

    @State private var volume: Double = LocalStorageService.shared.getVolume()
    @State private var isVibrationEnabled: Bool = LocalStorageService.shared.isVibrationEnabled()
    @State private var isWarningEnabled: Bool = LocalStorageService.shared.isWarningSoundEnabled()

    public init(colors: AppThemeColors) {
        self.colors = colors
    }

    private let prayers = [
        ("Sabah", "sabah_saba", "Sabah Ezanı (Saba Makamı)"),
        ("Öğle", "ogle_rast", "Öğle Ezanı (Rast Makamı)"),
        ("İkindi", "ikindi_hicaz", "İkindi Ezanı (Hicaz Makamı)"),
        ("Akşam", "aksam_segah", "Akşam Ezanı (Segah Makamı)"),
        ("Yatsı", "yatsi_ussak", "Yatsı Ezanı (Uşşak Makamı)"),
        ("Dua", "ezan_duasi", "Ezan Duası"),
        ("Sela", "sela", "Cuma Selası")
    ]

    public var body: some View {
        ScrollView(showsIndicators: false) {
            VStack(alignment: .leading, spacing: 16) {
                Text("Ezan ve Vakit Sesleri")
                    .font(.system(size: 22, weight: .bold))
                    .foregroundColor(colors.text)

                // Prayer sounds list
                ForEach(prayers, id: \.0) { item in
                    prayerSoundRow(name: item.0, soundFile: item.1, desc: item.2)
                }

                // Warning sound row
                HStack {
                    VStack(alignment: .leading, spacing: 2) {
                        Text("Vakit Öncesi Uyarı Sesi")
                            .font(.system(size: 15, weight: .bold))
                            .foregroundColor(colors.text)

                        Text("Vakitten 15 dakika önce çalar")
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
                        }
                }
                .padding(14)
                .background(colors.surface)
                .cornerRadius(14)
                .shadow(color: Color.black.opacity(0.02), radius: 2, y: 1)

                // Volume & Vibration
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
            .padding(16)
        }
        .background(colors.background.ignoresSafeArea())
    }

    private func prayerSoundRow(name: String, soundFile: String, desc: String) -> some View {
        PrayerSoundRowView(
            name: name,
            soundFile: soundFile,
            desc: desc,
            colors: colors,
            isPlaying: audioPlayer.currentlyPlaying == soundFile,
            onPlayToggle: {
                togglePlay(soundName: soundFile)
            }
        )
    }

    private func togglePlay(soundName: String) {
        if audioPlayer.currentlyPlaying == soundName {
            audioPlayer.stop()
        } else {
            audioPlayer.play(soundName: soundName, volume: Float(volume))
        }
    }
}

public struct PrayerSoundRowView: View {
    public let name: String
    public let soundFile: String
    public let desc: String
    public let colors: AppThemeColors
    public let isPlaying: Bool
    public let onPlayToggle: () -> Void

    @State private var isEnabled: Bool

    public init(name: String, soundFile: String, desc: String, colors: AppThemeColors, isPlaying: Bool, onPlayToggle: @escaping () -> Void) {
        self.name = name
        self.soundFile = soundFile
        self.desc = desc
        self.colors = colors
        self.isPlaying = isPlaying
        self.onPlayToggle = onPlayToggle
        _isEnabled = State(initialValue: LocalStorageService.shared.isPrayerSoundEnabled(prayerName: name))
    }

    public var body: some View {
        HStack {
            VStack(alignment: .leading, spacing: 2) {
                Text(name)
                    .font(.system(size: 15, weight: .bold))
                    .foregroundColor(colors.text)

                Text(desc)
                    .font(.system(size: 11))
                    .foregroundColor(colors.text.opacity(0.6))
            }

            Spacer()

            Button(action: onPlayToggle) {
                Image(systemName: isPlaying ? "stop.circle.fill" : "play.circle.fill")
                    .font(.system(size: 28))
                    .foregroundColor(colors.primary)
            }

            Toggle("", isOn: $isEnabled)
                .labelsHidden()
                .onChange(of: isEnabled) { val in
                    LocalStorageService.shared.setPrayerSoundEnabled(prayerName: name, enabled: val)
                }
        }
        .padding(14)
        .background(colors.surface)
        .cornerRadius(14)
        .shadow(color: Color.black.opacity(0.02), radius: 2, y: 1)
    }
}

