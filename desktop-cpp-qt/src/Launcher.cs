using System;
using System.IO;
using System.IO.Compression;
using System.Diagnostics;
using System.Reflection;
using System.Security.Cryptography;

class Program {
    [STAThread]
    static int Main(string[] args) {
        try {
            string localApp = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
            string appDir = Path.Combine(localApp, "PyVakitci", "app");
            string marker = Path.Combine(appDir, ".installed_hash");
            string exePath = Path.Combine(appDir, "PyVakitci.exe");

            Assembly asm = Assembly.GetExecutingAssembly();
            using (Stream stream = asm.GetManifestResourceStream("payload.zip")) {
                if (stream == null) {
                    System.Windows.Forms.MessageBox.Show("Gömülü uygulama paketi bulunamadı (payload.zip).", "PyVakitci");
                    return 1;
                }

                // Compute SHA-256 hash of embedded payload.zip
                string payloadHash;
                using (var sha = SHA256.Create()) {
                    byte[] hash = sha.ComputeHash(stream);
                    payloadHash = BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
                }

                bool needExtract = !Directory.Exists(appDir) ||
                                   !File.Exists(marker) ||
                                   !File.Exists(exePath) ||
                                   File.ReadAllText(marker).Trim() != payloadHash;

                if (needExtract) {
                    // Terminate any running PyVakitci from appDir
                    try {
                        foreach (var p in Process.GetProcessesByName("PyVakitci")) {
                            try {
                                if (p.MainModule != null && p.MainModule.FileName.StartsWith(appDir, StringComparison.OrdinalIgnoreCase)) {
                                    p.Kill();
                                    p.WaitForExit(3000);
                                }
                            } catch {}
                        }
                    } catch {}

                    if (Directory.Exists(appDir)) {
                        try {
                            Directory.Delete(appDir, true);
                        } catch {
                            try {
                                foreach (var file in Directory.GetFiles(appDir, "*", SearchOption.AllDirectories)) {
                                    try { File.Delete(file); } catch {}
                                }
                            } catch {}
                        }
                    }
                    Directory.CreateDirectory(appDir);

                    stream.Position = 0;
                    using (ZipArchive archive = new ZipArchive(stream)) {
                        foreach (var entry in archive.Entries) {
                            if (string.IsNullOrEmpty(entry.Name)) {
                                string dir = Path.Combine(appDir, entry.FullName);
                                Directory.CreateDirectory(dir);
                                continue;
                            }
                            string destFile = Path.Combine(appDir, entry.FullName);
                            string destDir = Path.GetDirectoryName(destFile);
                            if (!string.IsNullOrEmpty(destDir)) {
                                Directory.CreateDirectory(destDir);
                            }
                            entry.ExtractToFile(destFile, true);
                        }
                    }
                    File.WriteAllText(marker, payloadHash);
                }
            }

            if (!File.Exists(exePath)) {
                System.Windows.Forms.MessageBox.Show("PyVakitci.exe bulunamadı: " + exePath, "PyVakitci");
                return 1;
            }

            ProcessStartInfo psi = new ProcessStartInfo(exePath);
            psi.WorkingDirectory = appDir;
            psi.Arguments = string.Join(" ", args);
            psi.UseShellExecute = false;

            Process proc = Process.Start(psi);
            proc.WaitForExit();
            return proc.ExitCode;
        } catch (Exception ex) {
            System.Windows.Forms.MessageBox.Show("Hata: " + ex.Message, "PyVakitci");
            return 1;
        }
    }
}
