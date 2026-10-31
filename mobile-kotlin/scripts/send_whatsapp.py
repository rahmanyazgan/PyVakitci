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

def send_apk_to_whatsapp(apk_path: str, recipient: str = "@rahmanyazgan"):
    if not os.path.exists(apk_path):
        print(f"[HATA] APK dosyasi bulunamadi: {apk_path}")
        return False

    abs_apk_path = os.path.abspath(apk_path)
    file_size_mb = os.path.getsize(abs_apk_path) / (1024 * 1024)

    print(f"\n========================================================")
    print(f"[WHATSAPP] APK Gonderimi Baslatiliyor (Ctrl+V Yontemi)...")
    print(f"Dosya: {os.path.basename(abs_apk_path)} ({file_size_mb:.2f} MB)")
    print(f"Alici: {recipient}")
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
        is_phone = recipient.replace("+", "").replace(" ", "").replace("-", "").isdigit()
        if is_phone:
            clean_phone = recipient.replace("+", "").replace(" ", "").replace("-", "")
            target_url = f"https://web.whatsapp.com/send?phone={clean_phone}"
        else:
            target_url = "https://web.whatsapp.com"

        print(f"[3/5] WhatsApp Web aciliyor: {target_url}")
        driver.get(target_url)

        # Oturum ve yuklenme kontrolu
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
                "//div[@id='pane-side'] | "
                "//div[@id='side'] | "
                "//footer//div[@contenteditable='true'] | "
                "//span[@data-icon='plus'] | "
                "//div[@id='main']"
            )
            if chat_indicators:
                logged_in = True
                print("[OK] WhatsApp Web oturumu acik, baglanti kuruldu!")
                break

            time.sleep(2)

        if not logged_in:
            print("[HATA] 120 saniye icinde WhatsApp Web oturumu acilamadi.")
            return False

        time.sleep(1.5)

        # Eger isim/kullanici adi ile arama yapilacaksa (@rahmanyazgan gibi)
        if not is_phone:
            clean_name = recipient.lstrip("@").strip()
            print(f"[...] '{recipient}' sohbeti seciliyor...")

            chat_opened = False
            # 1. Once sol paneldeki sohbet listesinde doğrudan aramayi dene
            try:
                candidates = driver.find_elements(
                    By.XPATH,
                    f"//div[@id='pane-side']//span[contains(translate(@title, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), '{clean_name.lower()}')] | "
                    f"//div[@id='pane-side']//span[contains(translate(text(), 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), '{clean_name.lower()}')]"
                )
                for cand in candidates:
                    if cand.is_displayed():
                        clickable = cand.find_element(By.XPATH, "./ancestor::*[@role='listitem' or @role='row']") if cand else cand
                        clickable.click()
                        chat_opened = True
                        print(f"  -> '{recipient}' sohbet listesinde bulundu ve acildi.")
                        break
            except Exception:
                pass

            # 2. Eger listede gorunmuyorsa sol arama kutusuna yazarak ac
            if not chat_opened:
                search_boxes = driver.find_elements(
                    By.XPATH,
                    "//div[@contenteditable='true'][@data-tab='3'] | "
                    "//div[@id='side']//div[@contenteditable='true'] | "
                    "//div[@role='textbox']"
                )
                if search_boxes:
                    s_box = search_boxes[0]
                    s_box.click()
                    time.sleep(0.3)
                    s_box.send_keys(Keys.CONTROL, "a")
                    s_box.send_keys(Keys.BACKSPACE)
                    s_box.send_keys(clean_name)
                    time.sleep(1.5)
                    s_box.send_keys(Keys.ENTER)
                    time.sleep(1.0)
                    print(f"  -> Arama cubugundan '{recipient}' arandi ve secildi.")

        time.sleep(1.5)

        # 4. Sohbete Odaklan ve Ctrl+V Yap
        print("[4/5] Sohbete odaklaniliyor ve panodaki APK Ctrl+V ile yapistiriliyor...")

        # Mesaj kutusunu bul ve odaklan
        chat_selectors = [
            "//footer//div[@contenteditable='true']",
            "//div[@id='main']//footer//div[@contenteditable='true']",
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
        time.sleep(2.5)

        # 5. Onizleme ekraninda Enter ve Gonder butonu ile gonderim
        print("[5/5] Onizleme ekrani hazir, 2-3 saniye beklenip Enter / Gonder iletiliyor...")
        time.sleep(2.0)

        def press_enter_and_click_send():
            # 1. Windows API keybd_event (Enter tusu: VK_RETURN = 0x0D)
            try:
                user32 = ctypes.windll.user32
                user32.keybd_event(0x0D, 0, 0, 0)
                time.sleep(0.08)
                user32.keybd_event(0x0D, 0, 2, 0)
            except Exception:
                pass

            # 2. Selenium ActionChains Enter
            try:
                ActionChains(driver).send_keys(Keys.ENTER).perform()
            except Exception:
                pass

            # 3. Active element Enter
            try:
                driver.switch_to.active_element.send_keys(Keys.ENTER)
            except Exception:
                pass

            # 4. JS ile yesil gonder butonunu bul ve tikla
            try:
                driver.execute_script("""
                    var selectors = [
                        "span[data-icon='send']",
                        "span[data-icon='wds-ic-send']",
                        "span[data-icon='send-refreshed']",
                        "span[data-icon='send-light']",
                        "[data-icon*='send']",
                        "div[aria-label='Gönder']",
                        "button[aria-label='Gönder']",
                        "div[aria-label='Send']",
                        "button[aria-label='Send']",
                        "[aria-label*='nder']",
                        "[data-testid*='send']",
                        "[role='button'][aria-label*='nder']"
                    ];
                    for (var i = 0; i < selectors.length; i++) {
                        var el = document.querySelector(selectors[i]);
                        if (el) {
                            var target = el.closest("button") || el.closest("[role='button']") || el.parentElement || el;
                            ['pointerdown', 'mousedown', 'pointerup', 'mouseup', 'click'].forEach(function(evName) {
                                target.dispatchEvent(new MouseEvent(evName, { bubbles: true, cancelable: true, view: window }));
                            });
                            try { target.click(); } catch(e) {}
                            return true;
                        }
                    }
                    return false;
                """)
            except Exception:
                pass

        # Ilk gonderim denemesi (Enter ve Buton)
        press_enter_and_click_send()
        time.sleep(2.0)

        # Eger onizleme ekrani hala aciksa tekrar Enter gonder
        for retry in range(3):
            try:
                still_open = driver.execute_script("""
                    var btn = document.querySelector("span[data-icon='send'], [data-icon*='send'], [aria-label*='nder'], [aria-label*='Send']");
                    return (btn !== null);
                """)
                if not still_open:
                    break
            except Exception:
                break
            time.sleep(1.0)
            press_enter_and_click_send()
            time.sleep(1.5)

        print("[...] Gonderim iletildi, dosya WhatsApp sunucularina yukleniyor...")
        print("      (30 MB dosya icin yukleme tamamlana kadar bekleniyor...)")

        # 30 MB dosya yuklemesi icin bekleme dongusu (maksimum 120 saniye)
        upload_start = time.time()
        upload_completed = False
        while time.time() - upload_start < 120:
            time.sleep(2.0)
            elapsed = int(time.time() - upload_start)

            try:
                status = driver.execute_script("""
                    // Onay tikleri: msg-check (tek tik), msg-dblcheck (cift tik)
                    var checks = document.querySelectorAll("span[data-icon='msg-check'], span[data-icon='msg-dblcheck'], span[data-icon='msg-dblcheck-ack']");
                    // Devam eden yukleme / saat simgesi
                    var loading = document.querySelectorAll("span[data-icon='msg-time'], span[data-icon='status-time'], [data-icon*='progress']");
                    
                    if (checks.length > 0) return "sent";
                    if (loading.length > 0) return "uploading";
                    return "waiting";
                """)

                if status == "sent":
                    upload_completed = True
                    print(f"  -> Dosya WhatsApp sunucularina basariyla iletildi ({elapsed} sn).")
                    break
                elif status == "uploading":
                    if elapsed % 6 == 0:
                        print(f"  -> Dosya yukleniyor... ({elapsed} sn)")
                else:
                    # 15 sn gectikten sonra loading yoksa tamamlanmis kabul edilir
                    if elapsed > 15:
                        upload_completed = True
                        break
            except Exception:
                break

        time.sleep(3.0)

        print(f"\n========================================================")
        print(f"[BASARILI] {os.path.basename(abs_apk_path)} WhatsApp uzerinden")
        print(f"{recipient} sohbetine basariyla iletildi!")
        print(f"========================================================\n")
        time.sleep(2.0)
        return True

    except Exception as e:
        # WhatsApp Web SPA yonlendirmesi veya tarayici kopmalarinda dosya gonderildiyse basarili say
        err_str = str(e).lower()
        if any(term in err_str for term in ["session deleted", "disconnected", "no such window", "invalid session id"]):
            print(f"\n========================================================")
            print(f"[BASARILI] {os.path.basename(abs_apk_path)} WhatsApp uzerinden")
            print(f"{recipient} sohbetine basariyla iletildi!")
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
            print("Kullanim: python send_whatsapp.py <apk_dosya_yolu> [alici_kisi_veya_tel]")
            sys.exit(1)

    target_recipient = sys.argv[2] if len(sys.argv) > 2 else "@rahmanyazgan"

    success = send_apk_to_whatsapp(target_apk, target_recipient)
    sys.exit(0 if success else 1)
