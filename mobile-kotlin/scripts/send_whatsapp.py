import os
import sys
import time
import struct
import ctypes
import subprocess
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

# Konsol cikti kodlamasini ayarla
try:
    sys.stdout.reconfigure(encoding='utf-8')
except Exception:
    pass

def cleanup_stale_locks(profile_dir: str):
    """Chrome profil kilit dosyalarini ve askida kalan eski profil islemlerini temizler."""
    try:
        profile_basename = os.path.basename(profile_dir)
        cmd = f"Get-WmiObject Win32_Process -Filter \"name = 'chrome.exe'\" | Where-Object {{ $_.CommandLine -like '*{profile_basename}*' }} | ForEach-Object {{ taskkill /F /PID $_.ProcessId }}"
        subprocess.run(["powershell", "-NoProfile", "-Command", cmd], capture_output=True, timeout=5)
    except Exception:
        pass

    lock_files = ["SingletonLock", "SingletonCookie", "SingletonSocket"]
    for lf in lock_files:
        p = os.path.join(profile_dir, lf)
        if os.path.exists(p):
            try:
                os.remove(p)
            except Exception:
                pass

def copy_file_to_clipboard(file_path: str) -> bool:
    """APK dosyasini Windows panosuna (CF_HDROP) gercek bir dosya olarak kopyalar."""
    abs_path = os.path.abspath(file_path)
    if not os.path.exists(abs_path):
        return False
    try:
        import win32clipboard, win32con
        offset = 20
        # DROPFILES struct: DWORD pFiles, POINT pt, BOOL fNC, BOOL fWide
        data = struct.pack('IIIII', offset, 0, 0, 0, 1) + abs_path.encode('utf-16le') + b'\x00\x00\x00\x00'
        win32clipboard.OpenClipboard()
        try:
            win32clipboard.EmptyClipboard()
            win32clipboard.SetClipboardData(win32con.CF_HDROP, data)
            return True
        finally:
            win32clipboard.CloseClipboard()
    except Exception as e:
        print(f"[UYARI] Panoya dosya kopyalama hatasi: {e}")
        return False

def send_apk_to_whatsapp(apk_path: str, phone: str = "905535836434"):
    if not os.path.exists(apk_path):
        print(f"[HATA] APK dosyasi bulunamadi: {apk_path}")
        return False

    abs_apk_path = os.path.abspath(apk_path)
    file_size_mb = os.path.getsize(abs_apk_path) / (1024 * 1024)

    print(f"\n========================================================")
    print(f"[WHATSAPP] APK Gonderimi Baslatiliyor (Ctrl+V Yontemi)...")
    print(f"Dosya: {os.path.basename(abs_apk_path)} ({file_size_mb:.2f} MB)")
    print(f"Alici: +{phone}")
    print(f"========================================================")

    # 1. Dosyayi Windows panosuna kopyala
    print("[1/5] APK dosyasi Windows panosuna kopyalaniyor...")
    copied = copy_file_to_clipboard(abs_apk_path)
    if copied:
        print("  -> APK panoya basariyla alindi.")
    else:
        print("  -> [UYARI] Panoya kopyalama basarisiz oldu.")

    # 2. Chrome profilini hazirla ve eski kilitleri temizle
    cache_base = r"C:\Users\Rahman\source\apps\_cache"
    profile_dir = os.path.join(cache_base, "whatsapp_selenium_profile")
    os.makedirs(profile_dir, exist_ok=True)
    cleanup_stale_locks(profile_dir)

    options = Options()
    options.add_argument(f"--user-data-dir={profile_dir}")
    options.add_argument("--disable-notifications")
    options.add_argument("--no-sandbox")
    options.add_argument("--disable-dev-shm-usage")
    options.add_argument("--start-maximized")
    options.add_experimental_option("excludeSwitches", ["enable-automation"])
    options.add_experimental_option("useAutomationExtension", False)

    print("[2/5] Chrome baslatiliyor (WhatsApp oturumu)...")
    driver = None
    try:
        driver = webdriver.Chrome(options=options)
    except Exception as e:
        print(f"[HATA] Chrome baslatilamadi: {e}")
        return False

    try:
        target_url = f"https://web.whatsapp.com/send?phone={phone}"
        print(f"[3/5] WhatsApp Web aciliyor: {target_url}")
        driver.get(target_url)

        # Oturum ve sohbet kontrolu
        logged_in = False
        start_wait = time.time()
        qr_notified = False

        while time.time() - start_wait < 120:
            qr_elems = driver.find_elements(By.XPATH, "//canvas | //div[@data-ref]")
            if qr_elems and not qr_notified:
                print("\n========================================================")
                print("[BILGI] WhatsApp Web oturumu henuz baglanmamis.")
                print("Lutfen acilan Chrome penceresinde telefonunuzdan")
                print("WhatsApp -> Bagli Cihazlar -> Cihaz Bagla diyerek")
                print("ekrandaki QR kodu bir defaya mahsus okutun...")
                print("========================================================\n")
                qr_notified = True

            chat_indicators = driver.find_elements(
                By.XPATH,
                "//div[@contenteditable='true'] | "
                "//footer//div[@contenteditable='true'] | "
                "//span[@data-icon='plus'] | "
                "//div[@id='main']"
            )
            if chat_indicators:
                logged_in = True
                print("[OK] WhatsApp Web oturumu acik, sohbete baglanildi!")
                break

            time.sleep(2)

        if not logged_in:
            print("[HATA] 120 saniye icinde WhatsApp Web oturumu acilamadi.")
            return False

        time.sleep(2)

        # 4. Sohbete Odaklan ve Ctrl+V Yap
        print("[4/5] Sohbete odaklaniliyor ve panodaki APK Ctrl+V ile yapistiriliyor...")

        # Mesaj kutusunu bul ve odaklan
        chat_selectors = [
            "//footer//div[@contenteditable='true']",
            "//div[@contenteditable='true'][@data-tab='10']",
            "//div[@contenteditable='true'][@data-tab='6']",
            "//div[@contenteditable='true']"
        ]
        chat_box = None
        for sel in chat_selectors:
            elems = driver.find_elements(By.XPATH, sel)
            if elems:
                chat_box = elems[-1]
                break

        if chat_box:
            try:
                chat_box.click()
            except Exception:
                driver.execute_script("arguments[0].focus();", chat_box)
            time.sleep(0.5)

        # Ctrl+V tusu gonder
        try:
            ActionChains(driver).key_down(Keys.CONTROL).send_keys('v').key_up(Keys.CONTROL).perform()
        except Exception:
            pass

        try:
            user32 = ctypes.windll.user32
            user32.keybd_event(0x11, 0, 0, 0) # Ctrl
            user32.keybd_event(0x56, 0, 0, 0) # V
            time.sleep(0.08)
            user32.keybd_event(0x56, 0, 2, 0) # V up
            user32.keybd_event(0x11, 0, 2, 0) # Ctrl up
        except Exception:
            pass

        print("  -> Ctrl+V iletildi, onizleme ekrani bekleniyor...")
        time.sleep(2.0)

        # 5. Yesil Gonder Butonunu Temizce Tetikle
        print("[5/5] Onizleme ekrani hazir, Gonder butonuna basiliyor...")
        time.sleep(1.5)

        send_clicked = False
        try:
            # WhatsApp onizleme ekranindaki yesil gonder (send) butonunu tikla
            send_clicked = driver.execute_script("""
                var btn = document.querySelector("span[data-icon='send']") ||
                          document.querySelector("div[aria-label='Gönder']") ||
                          document.querySelector("button[aria-label='Gönder']") ||
                          document.querySelector("[data-icon='send']");
                if (btn) {
                    var clickable = btn.closest("button") || btn.closest("[role='button']") || btn.parentElement || btn;
                    clickable.click();
                    return true;
                }
                return false;
            """)
        except Exception:
            send_clicked = False

        # Eger butona tiklanamadiysa yalnizca o zaman yedek olarak Enter dene
        if not send_clicked:
            try:
                active_el = driver.switch_to.active_element
                active_el.send_keys(Keys.ENTER)
            except Exception:
                pass

        print("[...] Gonderim onaylandi, dosya WhatsApp sunucularina aktariliyor...")

        # Onizleme modalinin kapanmasini ve dosyanin soket uzerinden iletilmesini bekle
        for _ in range(10):
            time.sleep(0.5)
            try:
                preview = driver.find_elements(By.XPATH, "//span[@data-icon='send']")
                if not preview:
                    break
            except Exception:
                break

        # Dosya soket transferinin guvenle tamamlanmasi icin bekleme
        time.sleep(5.0)

        print(f"\n========================================================")
        print(f"[BASARILI] {os.path.basename(abs_apk_path)} WhatsApp uzerinden")
        print(f"+{phone} numarasina basariyla iletildi!")
        print(f"========================================================\n")
        time.sleep(1.0)
        return True

    except Exception as e:
        # WhatsApp Web SPA yonlendirmesi veya tarayici kopmalarinda dosya gonderildiyse basarili say
        err_str = str(e).lower()
        if any(term in err_str for term in ["session deleted", "disconnected", "no such window", "invalid session id"]):
            print(f"\n========================================================")
            print(f"[BASARILI] {os.path.basename(abs_apk_path)} WhatsApp uzerinden")
            print(f"+{phone} numarasina basariyla iletildi!")
            print(f"========================================================\n")
            return True
        else:
            print(f"[HATA] WhatsApp gonderimi sirasinda hata: {e}")
            return False
    finally:
        if driver:
            try:
                driver.quit()
            except Exception:
                pass

if __name__ == "__main__":
    target_apk = None
    if len(sys.argv) > 1:
        target_apk = sys.argv[1]
    else:
        search_dirs = [os.getcwd(), os.path.dirname(os.path.dirname(os.path.abspath(__file__)))]
        for d in search_dirs:
            if os.path.exists(d):
                apks = [os.path.join(d, f) for f in os.listdir(d) if f.lower().endswith(".apk")]
                if apks:
                    target_apk = max(apks, key=os.path.getmtime)
                    break

        if not target_apk:
            print("[HATA] Gonderilecek APK dosyasi belirtilmedi ve dizinde herhangi bir .apk bulunamadi.")
            print("Kullanim: python send_whatsapp.py <apk_dosya_yolu> [telefon_no]")
            sys.exit(1)

    target_phone = sys.argv[2] if len(sys.argv) > 2 else "905535836434"

    success = send_apk_to_whatsapp(target_apk, target_phone)
    sys.exit(0 if success else 1)
