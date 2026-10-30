import Foundation
import AVFoundation

public class AudioPlayerService: NSObject, ObservableObject, AVAudioPlayerDelegate {
    public static let shared = AudioPlayerService()
    private var audioPlayer: AVAudioPlayer?

    @Published public var currentlyPlaying: String? = nil

    override public init() {
        super.init()
        setupAudioSession()
    }

    private func setupAudioSession() {
        do {
            try AVAudioSession.sharedInstance().setCategory(.playback, mode: .default, options: [.duckOthers])
            try AVAudioSession.sharedInstance().setActive(true)
        } catch {
            print("Audio session error: \(error)")
        }
    }

    public static func getSoundFileForPrayer(prayerName: String) -> String {
        let norm = prayerName.lowercased()
            .replacingOccurrences(of: "ı", with: "i")
            .replacingOccurrences(of: "ö", with: "o")
            .replacingOccurrences(of: "ü", with: "u")
            .replacingOccurrences(of: "ş", with: "s")
            .replacingOccurrences(of: "ç", with: "c")
            .replacingOccurrences(of: "ğ", with: "g")

        if norm.contains("sabah") || norm.contains("imsak") { return "sabah_saba" }
        if norm.contains("ogle") { return "ogle_rast" }
        if norm.contains("ikindi") { return "ikindi_hicaz" }
        if norm.contains("aksam") { return "aksam_segah" }
        if norm.contains("yatsi") { return "yatsi_ussak" }
        if norm.contains("sela") { return "sela" }
        if norm.contains("dua") { return "ezan_duasi" }
        if norm.contains("uyari") { return "uyari" }
        return "sabah_saba"
    }

    public func play(soundName: String, volume: Float = 1.0) {
        stop()

        let cleanName = soundName.replacingOccurrences(of: ".mp3", with: "").lowercased()
        guard let url = Bundle.main.url(forResource: cleanName, withExtension: "mp3") ??
                        Bundle.main.url(forResource: cleanName, withExtension: "mp3", subdirectory: "Audio") else {
            print("Sound file not found: \(soundName)")
            return
        }

        do {
            audioPlayer = try AVAudioPlayer(contentsOf: url)
            audioPlayer?.delegate = self
            audioPlayer?.volume = volume
            audioPlayer?.prepareToPlay()
            audioPlayer?.play()
            currentlyPlaying = cleanName
        } catch {
            print("Playback failed: \(error)")
        }
    }

    public func stop() {
        audioPlayer?.stop()
        audioPlayer = nil
        currentlyPlaying = nil
    }

    public func isPlaying() -> Bool {
        audioPlayer?.isPlaying == true
    }

    public func audioPlayerDidFinishPlaying(_ player: AVAudioPlayer, successfully flag: Bool) {
        let previous = currentlyPlaying
        DispatchQueue.main.async {
            self.currentlyPlaying = nil
            // Ezan bittiginde kullanici ayari aciksa ezan duasina gec
            if flag, let prev = previous,
               (prev.contains("saba") || prev.contains("rast") || prev.contains("hicaz") || prev.contains("segah") || prev.contains("ussak")) {
                if LocalStorageService.shared.isEzanDuasiEnabled() {
                    let vol = Float(LocalStorageService.shared.getVolume())
                    self.play(soundName: "ezan_duasi", volume: vol)
                }
            }
        }
    }
}

