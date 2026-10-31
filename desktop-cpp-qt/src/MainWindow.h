#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QSettings>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QGroupBox>
#include <QActionGroup>
#include <QMediaPlayer>
#include <QCloseEvent>
#include <QKeyEvent>

#include "VakitGorselleri.h"
#include "HavaDurumu.h"
#include "Vakitler.h"
#include "Calverter.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void testEkranGoruntuleriAl(const QString& outDir);

private slots:
    void onAnimasyonTick();
    void onSaatGuncelle();
    void onLabelGuncelle();
    void onSekmeDegisti(int index);
    void onAyarAltSekmeDegisti(int index);
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onSesOynatDurdur(QLineEdit* lineEdit, QToolButton* btn);
    void onSesDurumDegisti(QMediaPlayer::MediaStatus status);
    void sehirKontrolu();
    void ilceKontrolu();
    void ayarlariKaydet();
    void varsayilanAyarlar();
    void sesVarsayilanAyarlar();
    void diyanetAylikVakitleriKaydet();
    void vakitleriTopluGoster();
    void vakitleriGoster();
    void lisansAc();
    void hakkindaDialog();
    void trayIconGoster();
    void herZamanUstte();
    void ezanOkuKontrol();
    void erkenUyariKontrolWidget();
    void winampVarMi();
    void arayuzModunuUygula(const QString& mod, bool kaydet = false);
    void temayiUygula(const QString& temaKey, bool kaydet = false);
    void pencereStiliniUygula(const QString& stilAdi, bool kaydet = true);

public slots:
    void programiKapat();

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setupUI();
    void setupMenuBar();
    void setupTray();
    void setupVakitTab();
    void setupHavaDurumuTab();
    void setupAyarlarTab();
    void setupSesTab();
    void setupLinklerTab();
    void ekrandaOrtala();
    void bolgeselAyarlariHazirla();
    void ayarlariUygula();
    void labelAdiniDegistir(const QString& secilenUlke);
    void ulkeSehirBilgisiniAl();
    void vakitleriAl();
    void hicriTarihiGuncelle();
    void diniGunleriHatirlat();
    void gorselVerileriGuncelle();
    void modernVakitleriGuncelle(const QString& aktifAnahtar);
    void kalanSureKontrol(const QString& simdikivakit);
    void erkenUyariKontrol(int toplamDakika);
    void selaOkuKontrol(int toplamDakika);
    void vakitKontrol(const QString& baslik, const QString& dosya, const QString& labelSaat, const QString& simdikivakit);
    void ezanOku(const QString& dosya);
    void sesDosyasiniAc(const QString& dosya);
    void linkRenkleriniAyarla(const QString& renk);
    QString vakitlerMesaj() const;
    QIcon havaDurumuIkonuOlustur();
    static int ayarInt(const QVariant& val, int def = 0);

    // Temel bileşenler
    QTabWidget* m_tabWidget = nullptr;
    QSystemTrayIcon* m_trayIcon = nullptr;
    QTimer* m_animTimer = nullptr;
    QTimer* m_clockTimer = nullptr;
    QTimer* m_labelTimer = nullptr;
    QTimer* m_mesajTimer = nullptr;
    QSettings* m_settings = nullptr;
    Calverter m_calverter;
    Vakitler* m_vakitler = nullptr;
    QMediaPlayer* m_player = nullptr;
    QMediaPlayer* m_onizlemePlayer = nullptr;
    QToolButton* m_calanSesBtn = nullptr;

    // Yollar ve durum değişkenleri
    QString m_programinKonumu;
    QString m_sesDosyalariKonum;
    QString m_ayarlarKonum;
    QString m_ayarDosyasi;
    QString m_veritabaniDosyasi;
    QString m_gorunum = "Fusion";
    QString m_aktifTema = "varsayilan";
    QString m_arayuzModu = "saat";
    QString m_ulke = "TÜRKİYE";
    QString m_sehir = "İSTANBUL";
    QString m_ilce = "ESENYURT";
    QString m_seciliYer = "ESENYURT";
    QString m_aktifVakit = "imsak";
    QString m_namazVaktiStr;
    QString m_sonrakiVakit;
    QString m_kalanSure = "00:00:00";
    QString m_miladiTarih;
    QString m_simdikiTarih;
    QString m_hicriTarih;
    QString m_hicriGun;
    QString m_hicriAy;
    QString m_hicriYil;
    QString m_saat;
    bool m_mesajGosterildi = false;
    bool m_diniGunMesajiGosterildi = false;
    Qt::WindowFlags m_windowFlags;

    // Diyanet bölge haritaları
    QMap<QString, int> m_sehirlerMap;
    QMap<QString, int> m_ilcelerMap;
    QMap<QString, QString> m_linklerMap;

    // Sekme 0: Vakitler
    QWidget* m_vakitTab = nullptr;
    VakitSaatiWidget* m_vakitSaatiWidget = nullptr;
    GokyuzuWidget* m_gokyuzuWidget = nullptr;
    QWidget* m_modernPanel = nullptr;
    QLabel* m_modernUlkeLabel = nullptr;
    QLabel* m_modernSehirLabel = nullptr;
    QLabel* m_modernSaatLabel = nullptr;
    QLabel* m_modernMiladiLabel = nullptr;
    QLabel* m_modernHicriLabel = nullptr;
    QLabel* m_modernDiniGunLabel = nullptr;
    QLabel* m_modernVakitDurumLabel = nullptr;
    QLabel* m_modernKalanBaslikLabel = nullptr;
    QLabel* m_modernKalanSureLabel = nullptr;
    QMap<QString, QFrame*> m_modernSatirFrames;
    QMap<QString, QLabel*> m_modernAdLabels;
    QMap<QString, QLabel*> m_modernSaatLabels;
    QMap<QString, QLabel*> m_modernBadgeLabels;

    // Klasik Panel (Ui_PyVakitci birebir)
    QGroupBox* m_bilgilerGroupBox = nullptr;
    QGroupBox* m_namazVakitleriGroupBox = nullptr;
    QLabel* m_klasikUlkeLabel = nullptr;
    QLabel* m_klasikSehirLabel = nullptr;
    QLabel* m_klasikSaatLabel = nullptr;
    QLabel* m_klasikMiladiLabel = nullptr;
    QLabel* m_klasikHicriLabel = nullptr;
    QLabel* m_klasikDurumLabel = nullptr;
    QFrame* m_klasikLine = nullptr;
    QLabel* m_klasikSonrakiVakitLabel = nullptr;
    QLabel* m_klasikKalanSureLabel = nullptr;
    QLabel* m_klasikDiniGunLabel = nullptr;
    QMap<QString, QLabel*> m_klasikVakitLabels;
    QMap<QString, QLabel*> m_klasikVakitAdLabels;

    // Sekme 1: Hava Durumu
    QWidget* m_havaTab = nullptr;
    HavaDurumuWidget* m_havaWidget = nullptr;

    // Sekme 2: Ayarlar
    QWidget* m_ayarlarTab = nullptr;
    QPushButton* m_btnGenelAyarlar = nullptr;
    QPushButton* m_btnBolgeselAyarlar = nullptr;
    QGroupBox* m_genelAyarlarBox = nullptr;
    QGroupBox* m_bolgeselAyarlarBox = nullptr;
    QLabel* m_lblBolgeselUlke = nullptr;
    QLabel* m_lblBolgeselSehir = nullptr;
    QLabel* m_lblBolgeselIlce = nullptr;
    QLabel* m_lblBolgeselDil = nullptr;
    QCheckBox* m_chkOtoCalistir = nullptr;
    QCheckBox* m_chkIkonGoster = nullptr;
    QCheckBox* m_chkSistemTepsisindeBaslat = nullptr;
    QCheckBox* m_chkOtoGuncelle = nullptr;
    QCheckBox* m_chkHerZamanUstte = nullptr;
    QCheckBox* m_chkBesmele = nullptr;
    QCheckBox* m_chkSpotify = nullptr;
    QCheckBox* m_chkWinamp = nullptr;
    QCheckBox* m_chkErkenUyari = nullptr;
    QSpinBox* m_spnErkenUyari = nullptr;
    QCheckBox* m_chkErkenSela = nullptr;
    QSpinBox* m_spnErkenSela = nullptr;
    QCheckBox* m_chkHicriGun = nullptr;
    QSpinBox* m_spnHicriGun = nullptr;
    QPushButton* m_btnGoster = nullptr;
    QPushButton* m_btnGuncelle = nullptr;
    QPushButton* m_btnVarsayilan = nullptr;
    QPushButton* m_btnKaydet = nullptr;
    QPushButton* m_btnBolgeVarsayilan = nullptr;
    QPushButton* m_btnBolgeKaydet = nullptr;
    QComboBox* m_cmbUlke = nullptr;
    QComboBox* m_cmbSehir = nullptr;
    QComboBox* m_cmbIlce = nullptr;
    QComboBox* m_cmbDil = nullptr;
    void onDilDegisti(int index);
    void retranslateUi();

    // Sekme 3: Ses Ayarları
    QWidget* m_sesTab = nullptr;
    QMap<QString, QLabel*> m_sesLabels;
    QMap<QString, QLineEdit*> m_sesEdits;
    QMap<QString, QToolButton*> m_sesEkleBtns;
    QMap<QString, QCheckBox*> m_sesChecks;
    QMap<QString, QToolButton*> m_sesPlayBtns;
    QPushButton* m_btnSesVarsayilan = nullptr;
    QPushButton* m_btnSesKaydet = nullptr;
    QSlider* m_sesSlider = nullptr;

    // Sekme 4: Linkler
    QWidget* m_linkTab = nullptr;
    QGroupBox* m_linklerBox = nullptr;
    QList<QLabel*> m_linkLabels;

    // Menü Actions
    QMenu* m_menuDosya = nullptr;
    QMenu* m_menuGorunum = nullptr;
    QMenu* m_menuTema = nullptr;
    QMenu* m_menuStil = nullptr;
    QMenu* m_menuYardim = nullptr;
    QAction* m_actLisans = nullptr;
    QAction* m_actHakkinda = nullptr;
    QAction* m_actQt = nullptr;
    QActionGroup* m_arayuzGroup = nullptr;
    QActionGroup* m_temaGroup = nullptr;
    QActionGroup* m_stilGroup = nullptr;
    QMap<QString, QAction*> m_arayuzActions;
    QMap<QString, QAction*> m_temaActions;
    QMap<QString, QAction*> m_stilActions;
    QAction* m_actVakitleriGoster = nullptr;
    QAction* m_actCikis = nullptr;
};

#endif // MAINWINDOW_H
