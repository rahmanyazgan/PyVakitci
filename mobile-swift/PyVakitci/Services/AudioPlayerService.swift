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
        DispatchQueue.main.async {
            self.currentlyPlaying = nil
        }
    }
}

