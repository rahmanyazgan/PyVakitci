#include "MainWindow.h"
#include "Temalar.h"
#include "DiyanetUlkeler.h"
#include "HttpClient.h"
#include "Dialogs.h"
#include "Clever.h"
#include "Localization.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QStyle>
#include <QTabBar>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QPainterPath>
#include <QDir>
#include <QFile>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setObjectName("MainWindow");
    setWindowTitle("PyVakitci");
    setWindowIcon(QIcon(":/resimler/cami.ico"));
    resize(351, 511);
    setMinimumSize(351, 511);
    setMaximumSize(351, 511);

    QFont mainFont("Arial");
    setFont(mainFont);
    m_windowFlags = windowFlags();

    // Uygulama dizini ve ses dosyaları konumu
    m_programinKonumu = QCoreApplication::applicationDirPath().replace("\\", "/");
    if (!QFile::exists(m_programinKonumu + "/ses_dosyalari")) {
        QDir upDir(m_programinKonumu);
        if (upDir.cdUp() && QFile::exists(upDir.absolutePath() + "/desktop-python-qt/ses_dosyalari")) {
            m_programinKonumu = upDir.absolutePath().replace("\\", "/") + "/desktop-python-qt";
        } else if (QFile::exists("C:/Users/Rahman/source/apps/PyVakitci/desktop-python-qt/ses_dosyalari")) {
            m_programinKonumu = "C:/Users/Rahman/source/apps/PyVakitci/desktop-python-qt";
        }
    }
    m_sesDosyalariKonum = m_programinKonumu + "/ses_dosyalari/";

    m_ayarlarKonum = QDir::homePath().replace("\\", "/") + "/.PyVakitci-1.8";
    QDir().mkpath(m_ayarlarKonum);

    m_ayarDosyasi = m_ayarlarKonum + "/ayarlar-cpp.ini";
    if (!QFile::exists(m_ayarDosyasi) && QFile::exists(m_ayarlarKonum + "/ayarlar.ini")) {
        QFile::copy(m_ayarlarKonum + "/ayarlar.ini", m_ayarDosyasi);
    }
    m_settings = new QSettings(m_ayarDosyasi, QSettings::IniFormat, this);
    Localization::init();
    QString kayitliDil = m_settings->value("Ayarlar/dil", Localization::detectSystemLanguage()).toString();
    Localization::setLanguage(kayitliDil);

    m_veritabaniDosyasi = m_ayarlarKonum + "/diyanet.db";
    m_vakitler = new Vakitler(m_veritabaniDosyasi, this);

    m_player = new QMediaPlayer(this);
    m_onizlemePlayer = new QMediaPlayer(this);
    connect(m_onizlemePlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onSesDurumDegisti);

    setupUI();
    setupMenuBar();
    setupTray();
    pencereStiliniUygula("Fusion", false);
    bolgeselAyarlariHazirla();
    ekrandaOrtala();
    sesVarsayilanAyarlar();

    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, &MainWindow::onAnimasyonTick);
    m_animTimer->start(33); // 30 FPS

    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &MainWindow::onSaatGuncelle);
    m_clockTimer->start(600);

    ayarlariUygula();
    retranslateUi();
    trayIconGoster();
    hicriTarihiGuncelle();
    onSaatGuncelle();

    if (m_chkBesmele->isChecked()) {
        sesDosyasiniAc(m_sesDosyalariKonum + "besmele.mp3");
    }
}

MainWindow::~MainWindow() {
}

int MainWindow::ayarInt(const QVariant& val, int def) {
    if (val.isNull() || !val.isValid()) return def;
    QString s = val.toString().trimmed().toLower();
    if (s == "true") return 2;
    if (s == "false") return 0;
    bool ok = false;
    int v = static_cast<int>(s.toDouble(&ok));
    return ok ? v : def;
}

void MainWindow::ekrandaOrtala() {
    QRect scr = QApplication::desktop()->screenGeometry();
    move((scr.width() - geometry().width()) / 2, (scr.height() - geometry().height()) / 2);
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralwidget");
    setCentralWidget(centralWidget);

    m_tabWidget = new QTabWidget(centralWidget);
    m_tabWidget->setObjectName("tabWidget");
    m_tabWidget->setGeometry(10, 10, 330, 441);
    m_tabWidget->setMinimumSize(310, 441);
    QFont tabFont("Arial", 11);
    m_tabWidget->setFont(tabFont);
    m_tabWidget->setTabPosition(QTabWidget::South);
    m_tabWidget->setIconSize(QSize(38, 38));
    m_tabWidget->setUsesScrollButtons(false);
    m_tabWidget->tabBar()->setExpanding(false);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onSekmeDegisti);

    setupVakitTab();
    setupHavaDurumuTab();
    setupAyarlarTab();
    setupSesTab();
    setupLinklerTab();

    QStatusBar* sb = new QStatusBar(this);
    sb->setObjectName("statusbar");
    sb->setFont(QFont("Arial", 10));
    setStatusBar(sb);
    sb->showMessage(Localization::tr("kaynak"));
}

QIcon MainWindow::havaDurumuIkonuOlustur() {
    QPixmap pix(64, 64);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    QLinearGradient bgGrad(0, 0, 0, 64);
    bgGrad.setColorAt(0.0, QColor("#38bdf8"));
    bgGrad.setColorAt(1.0, QColor("#0284c7"));
    p.setPen(Qt::NoPen);
    p.setBrush(bgGrad);
    p.drawRoundedRect(QRectF(4, 4, 56, 56), 14, 14);

    QRadialGradient sunGrad(24, 24, 14);
    sunGrad.setColorAt(0.0, QColor("#fef08a"));
    sunGrad.setColorAt(0.7, QColor("#facc15"));
    sunGrad.setColorAt(1.0, QColor("#eab308"));
    p.setBrush(sunGrad);
    p.drawEllipse(QPointF(24, 24), 11, 11);

    p.setBrush(QColor("#ffffff"));
    QPainterPath bulut;
    bulut.setFillRule(Qt::WindingFill);
    bulut.addRoundedRect(QRectF(16, 31, 34, 15), 7.5, 7.5);
    bulut.addEllipse(QPointF(26, 29), 9, 9);
    bulut.addEllipse(QPointF(37, 27), 10, 10);
    p.drawPath(bulut);
    p.end();

    QIcon ikon;
    ikon.addPixmap(pix, QIcon::Normal, QIcon::Off);
    return ikon;
}

void MainWindow::setupVakitTab() {
    m_vakitTab = new QWidget();
    m_vakitTab->setObjectName("pyvakitci_tab");

    // 1. Klasik Görünüm (Ui_PyVakitci.py ile birebir aynı)
    m_bilgilerGroupBox = new QGroupBox(m_vakitTab);
    m_bilgilerGroupBox->setGeometry(13, 12, 301, 64);
    m_bilgilerGroupBox->setFont(QFont("Arial", 10));
    m_bilgilerGroupBox->setTitle("");
    m_bilgilerGroupBox->setObjectName("bilgiler_groupBox");
    m_bilgilerGroupBox->hide();

    m_klasikUlkeLabel = new QLabel("...", m_bilgilerGroupBox);
    m_klasikUlkeLabel->setGeometry(10, 10, 150, 16);
    m_klasikUlkeLabel->setFont(QFont("Arial", 8));
    m_klasikUlkeLabel->setObjectName("ulke_label");

    m_klasikSehirLabel = new QLabel("...", m_bilgilerGroupBox);
    m_klasikSehirLabel->setGeometry(10, 40, 160, 16);
    m_klasikSehirLabel->setFont(QFont("Arial", 10));
    m_klasikSehirLabel->setObjectName("sehir_label");

    m_klasikMiladiLabel = new QLabel("...", m_bilgilerGroupBox);
    m_klasikMiladiLabel->setGeometry(198, 10, 98, 16);
    m_klasikMiladiLabel->setFont(QFont("Arial", 8));
    m_klasikMiladiLabel->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
    m_klasikMiladiLabel->setObjectName("miladi_tarih");

    m_klasikHicriLabel = new QLabel("...", m_bilgilerGroupBox);
    m_klasikHicriLabel->setGeometry(156, 25, 140, 16);
    m_klasikHicriLabel->setFont(QFont("Arial", 8));
    m_klasikHicriLabel->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
    m_klasikHicriLabel->setObjectName("hicri_tarih");

    m_klasikSaatLabel = new QLabel("...", m_bilgilerGroupBox);
    m_klasikSaatLabel->setGeometry(204, 40, 92, 16);
    m_klasikSaatLabel->setFont(QFont("Arial", 8));
    m_klasikSaatLabel->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
    m_klasikSaatLabel->setObjectName("saat_label");

    m_namazVakitleriGroupBox = new QGroupBox(m_vakitTab);
    m_namazVakitleriGroupBox->setGeometry(13, 84, 301, 301);
    m_namazVakitleriGroupBox->setFont(QFont("Arial", 10));
    m_namazVakitleriGroupBox->setTitle("");
    m_namazVakitleriGroupBox->setObjectName("namaz_vakitleri_groupBox");
    m_namazVakitleriGroupBox->hide();

    m_klasikDurumLabel = new QLabel("...", m_namazVakitleriGroupBox);
    m_klasikDurumLabel->setGeometry(10, 15, 281, 20);
    m_klasikDurumLabel->setFont(QFont("Arial", 11));
    m_klasikDurumLabel->setAlignment(Qt::AlignCenter);
    m_klasikDurumLabel->setObjectName("namaz_vakti_label");

    m_klasikLine = new QFrame(m_namazVakitleriGroupBox);
    m_klasikLine->setGeometry(40, 30, 221, 16);
    m_klasikLine->setFrameShape(QFrame::HLine);
    m_klasikLine->setFrameShadow(QFrame::Sunken);
    m_klasikLine->setObjectName("line");

    static const QStringList vakitKeys = {"imsak", "gunes", "ogle", "ikindi", "aksam", "yatsi"};
    static const QStringList vakitAdlari = {"İmsak", "Güneş", "Öğle", "İkindi", "Akşam", "Yatsı"};

    for (int i = 0; i < 6; ++i) {
        QLabel* ad = new QLabel(vakitAdlari[i], m_namazVakitleriGroupBox);
        ad->setGeometry(92, 55 + i * 30, 50, 20);
        ad->setFont(QFont("Arial", 11));
        ad->setObjectName(vakitKeys[i] + "_label");

        QLabel* nokta = new QLabel(":", m_namazVakitleriGroupBox);
        nokta->setGeometry(152, 57 + i * 30, 10, 16);
        nokta->setFont(QFont("Arial", 10));

        QLabel* val = new QLabel("...", m_namazVakitleriGroupBox);
        val->setGeometry(172, 58 + i * 30, 50, 16);
        val->setFont(QFont("Arial", 11));
        val->setObjectName(vakitKeys[i] + "_saat_label");
        m_klasikVakitLabels[vakitKeys[i]] = val;
        m_klasikVakitAdLabels[vakitKeys[i]] = ad;
    }

    m_klasikSonrakiVakitLabel = new QLabel("...", m_namazVakitleriGroupBox);
    m_klasikSonrakiVakitLabel->setGeometry(95, 235, 130, 16);
    m_klasikSonrakiVakitLabel->setFont(QFont("Arial", 9));
    m_klasikSonrakiVakitLabel->setObjectName("sonraki_vakit_label");

    m_klasikKalanSureLabel = new QLabel("...", m_namazVakitleriGroupBox);
    m_klasikKalanSureLabel->setGeometry(120, 255, 120, 16);
    m_klasikKalanSureLabel->setFont(QFont("Arial", 11));
    m_klasikKalanSureLabel->setObjectName("kalan_sure_label");

    m_klasikDiniGunLabel = new QLabel("", m_namazVakitleriGroupBox);
    m_klasikDiniGunLabel->setGeometry(10, 275, 281, 20);
    m_klasikDiniGunLabel->setFont(QFont("Arial", 11));
    m_klasikDiniGunLabel->setAlignment(Qt::AlignCenter);
    m_klasikDiniGunLabel->setObjectName("dini_gun_label");

    // 2. Animasyonlu Vakit Saati (Kadran) Widget'ı
    m_vakitSaatiWidget = new VakitSaatiWidget(m_vakitTab);
    m_vakitSaatiWidget->setGeometry(10, 8, 306, 378);
    m_vakitSaatiWidget->show();

    // 3. Dinamik Gökyüzü & Hilal Widget'ı
    m_gokyuzuWidget = new GokyuzuWidget(m_vakitTab);
    m_gokyuzuWidget->setGeometry(10, 8, 306, 378);
    m_gokyuzuWidget->hide();

    // 4. Modern Kart Paneli
    m_modernPanel = new QWidget(m_vakitTab);
    m_modernPanel->setGeometry(10, 8, 306, 378);
    m_modernPanel->setObjectName("modern_panel");
    m_modernPanel->hide();

    QFrame* ustKart = new QFrame(m_modernPanel);
    ustKart->setGeometry(2, 2, 302, 74);
    ustKart->setObjectName("modern_ust_kart");

    m_modernUlkeLabel = new QLabel(ustKart);
    m_modernUlkeLabel->setGeometry(14, 12, 145, 16);
    m_modernUlkeLabel->setObjectName("modern_ulke_label");

    m_modernSehirLabel = new QLabel(ustKart);
    m_modernSehirLabel->setGeometry(14, 29, 155, 24);
    m_modernSehirLabel->setObjectName("modern_sehir_label");

    m_modernDiniGunLabel = new QLabel(ustKart);
    m_modernDiniGunLabel->setGeometry(14, 53, 155, 16);
    m_modernDiniGunLabel->setObjectName("modern_dini_gun_label");

    m_modernSaatLabel = new QLabel(ustKart);
    m_modernSaatLabel->setGeometry(202, 8, 86, 22);
    m_modernSaatLabel->setAlignment(Qt::AlignCenter);
    m_modernSaatLabel->setObjectName("modern_saat_label");

    m_modernMiladiLabel = new QLabel(ustKart);
    m_modernMiladiLabel->setGeometry(140, 33, 148, 17);
    m_modernMiladiLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_modernMiladiLabel->setObjectName("modern_miladi_label");

    m_modernHicriLabel = new QLabel(ustKart);
    m_modernHicriLabel->setGeometry(120, 51, 168, 17);
    m_modernHicriLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_modernHicriLabel->setObjectName("modern_hicri_label");

    QFrame* sayacKart = new QFrame(m_modernPanel);
    sayacKart->setGeometry(2, 82, 302, 68);
    sayacKart->setObjectName("modern_sayac_kart");

    m_modernVakitDurumLabel = new QLabel(sayacKart);
    m_modernVakitDurumLabel->setGeometry(14, 12, 165, 20);
    m_modernVakitDurumLabel->setObjectName("modern_vakit_durum_label");

    m_modernKalanBaslikLabel = new QLabel(sayacKart);
    m_modernKalanBaslikLabel->setGeometry(14, 36, 165, 18);
    m_modernKalanBaslikLabel->setObjectName("modern_kalan_baslik_label");

    m_modernKalanSureLabel = new QLabel(sayacKart);
    m_modernKalanSureLabel->setGeometry(168, 12, 120, 44);
    m_modernKalanSureLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_modernKalanSureLabel->setObjectName("modern_kalan_sure_label");

    for (int i = 0; i < 6; ++i) {
        int y = 156 + i * 37;
        QFrame* row = new QFrame(m_modernPanel);
        row->setGeometry(2, y, 302, 34);
        row->setProperty("vakitSatiri", "true");
        row->setProperty("aktif", "false");

        QLabel* adLbl = new QLabel(vakitAdlari[i], row);
        adLbl->setGeometry(14, 0, 100, 34);
        adLbl->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        adLbl->setProperty("vakitAd", "true");
        adLbl->setProperty("aktif", "false");

        QLabel* badge = new QLabel("AKTİF", row);
        badge->setGeometry(122, 8, 56, 18);
        badge->setAlignment(Qt::AlignCenter);
        badge->setProperty("vakitBadge", "true");
        badge->hide();

        QLabel* stLbl = new QLabel("--:--", row);
        stLbl->setGeometry(195, 0, 93, 34);
        stLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        stLbl->setProperty("vakitSaat", "true");
        stLbl->setProperty("aktif", "false");

        m_modernSatirFrames[vakitKeys[i]] = row;
        m_modernAdLabels[vakitKeys[i]] = adLbl;
        m_modernBadgeLabels[vakitKeys[i]] = badge;
        m_modernSaatLabels[vakitKeys[i]] = stLbl;
    }

    m_tabWidget->addTab(m_vakitTab, QIcon(":/resimler/cami.png"), "");
    m_tabWidget->setTabToolTip(0, "Namaz Vakitleri");
}

void MainWindow::setupHavaDurumuTab() {
    m_havaTab = new QWidget();
    m_havaTab->setObjectName("bolgesel_ayarlar_tab");

    m_havaWidget = new HavaDurumuWidget(m_havaTab);
    m_havaWidget->setGeometry(10, 8, 306, 378);
    m_havaWidget->show();

    m_tabWidget->addTab(m_havaTab, havaDurumuIkonuOlustur(), "");
    m_tabWidget->setTabToolTip(1, "Hava Durumu");
}

void MainWindow::setupAyarlarTab() {
    m_ayarlarTab = new QWidget();
    m_ayarlarTab->setObjectName("ayarlar_tab");

    QButtonGroup* subTabGroup = new QButtonGroup(m_ayarlarTab);
    subTabGroup->setExclusive(true);

    m_btnGenelAyarlar = new QPushButton("⚙️ Genel Ayarlar", m_ayarlarTab);
    m_btnGenelAyarlar->setGeometry(13, 6, 148, 27);
    m_btnGenelAyarlar->setCheckable(true);
    m_btnGenelAyarlar->setChecked(true);
    m_btnGenelAyarlar->setCursor(Qt::PointingHandCursor);
    m_btnGenelAyarlar->setProperty("ayarAltSekme", "true");

    m_btnBolgeselAyarlar = new QPushButton("🌍 Bölgesel Ayarlar", m_ayarlarTab);
    m_btnBolgeselAyarlar->setGeometry(166, 6, 148, 27);
    m_btnBolgeselAyarlar->setCheckable(true);
    m_btnBolgeselAyarlar->setCursor(Qt::PointingHandCursor);
    m_btnBolgeselAyarlar->setProperty("ayarAltSekme", "true");

    subTabGroup->addButton(m_btnGenelAyarlar, 0);
    subTabGroup->addButton(m_btnBolgeselAyarlar, 1);
    connect(m_btnGenelAyarlar, &QPushButton::clicked, this, [this]() { onAyarAltSekmeDegisti(0); });
    connect(m_btnBolgeselAyarlar, &QPushButton::clicked, this, [this]() { onAyarAltSekmeDegisti(1); });

    QFont ayarFont("Arial", 11);

    // 1. Genel Ayarlar GroupBox
    m_genelAyarlarBox = new QGroupBox(m_ayarlarTab);
    m_genelAyarlarBox->setObjectName("ayarlar_groupBox");
    m_genelAyarlarBox->setTitle("");
    m_genelAyarlarBox->setMinimumSize(280, 348);
    m_genelAyarlarBox->setGeometry(13, 38, 301, 348);
    m_genelAyarlarBox->setFont(ayarFont);

    m_chkOtoCalistir = new QCheckBox("Açılışta programı otomatik çalıştır", m_genelAyarlarBox);
    m_chkOtoCalistir->setGeometry(10, 12, 285, 22);
    m_chkOtoCalistir->setFont(ayarFont);
    m_chkOtoCalistir->setObjectName("otomatik_calistir_checkBox");

    m_chkIkonGoster = new QCheckBox("Sistem tepsisinde ikon göster.", m_genelAyarlarBox);
    m_chkIkonGoster->setGeometry(10, 36, 285, 22);
    m_chkIkonGoster->setFont(ayarFont);
    m_chkIkonGoster->setToolTip("Aktif değilse program çarpıya tıklandığında kapanır.");
    m_chkIkonGoster->setObjectName("ikon_goster_checkBox");
    connect(m_chkIkonGoster, &QCheckBox::stateChanged, this, &MainWindow::trayIconGoster);

    m_chkSistemTepsisindeBaslat = new QCheckBox("Programı sistem tepsisinde başlat", m_genelAyarlarBox);
    m_chkSistemTepsisindeBaslat->setGeometry(10, 60, 285, 22);
    m_chkSistemTepsisindeBaslat->setFont(ayarFont);
    m_chkSistemTepsisindeBaslat->setObjectName("sistem_tepsisinde_baslat_checkBox");

    m_chkOtoGuncelle = new QCheckBox("Güncellemeleri kontrol et", m_genelAyarlarBox);
    m_chkOtoGuncelle->setGeometry(10, 84, 285, 22);
    m_chkOtoGuncelle->setFont(ayarFont);
    m_chkOtoGuncelle->setObjectName("otomatik_guncellestir_checkBox");

    m_chkHerZamanUstte = new QCheckBox("Her zaman üstte göster", m_genelAyarlarBox);
    m_chkHerZamanUstte->setGeometry(10, 108, 285, 22);
    m_chkHerZamanUstte->setFont(ayarFont);
    m_chkHerZamanUstte->setObjectName("her_zaman_ustte_checkBox");
    connect(m_chkHerZamanUstte, &QCheckBox::stateChanged, this, &MainWindow::herZamanUstte);

    m_chkBesmele = new QCheckBox("Besmele ile başla", m_genelAyarlarBox);
    m_chkBesmele->setGeometry(10, 132, 285, 22);
    m_chkBesmele->setFont(ayarFont);
    m_chkBesmele->setObjectName("besmele_ile_basla_checkBox");

    m_chkSpotify = new QCheckBox("Spotifyı duraklat", m_genelAyarlarBox);
    m_chkSpotify->setGeometry(10, 156, 137, 22);
    m_chkSpotify->setFont(ayarFont);
    m_chkSpotify->setToolTip("Spotify'da çalan şarkıyı duraklat.");
    m_chkSpotify->setObjectName("stopify_duraklat_checkBox");

    m_chkWinamp = new QCheckBox("Winampı duraklat", m_genelAyarlarBox);
    m_chkWinamp->setGeometry(153, 156, 141, 22);
    m_chkWinamp->setFont(ayarFont);
    m_chkWinamp->setToolTip("Winamp'ta çalan şarkıyı duraklat.");
    m_chkWinamp->setObjectName("winamp_duraklat_checkBox");
    connect(m_chkWinamp, &QCheckBox::stateChanged, this, &MainWindow::winampVarMi);

    m_chkErkenUyari = new QCheckBox("Vakit girmeden önce uyar (dk) :", m_genelAyarlarBox);
    m_chkErkenUyari->setGeometry(10, 180, 225, 22);
    m_chkErkenUyari->setFont(ayarFont);
    m_chkErkenUyari->setObjectName("erken_uyari_checkBox");
    connect(m_chkErkenUyari, &QCheckBox::stateChanged, this, &MainWindow::erkenUyariKontrolWidget);

    m_spnErkenUyari = new QSpinBox(m_genelAyarlarBox);
    m_spnErkenUyari->setGeometry(240, 181, 42, 21);
    m_spnErkenUyari->setRange(0, 99);
    m_spnErkenUyari->setValue(45);
    m_spnErkenUyari->setObjectName("erken_uyari_spinBox");

    m_chkErkenSela = new QCheckBox("Selayı vakit girmeden oku (dk) :", m_genelAyarlarBox);
    m_chkErkenSela->setGeometry(10, 204, 225, 22);
    m_chkErkenSela->setFont(ayarFont);
    m_chkErkenSela->setObjectName("erken_uyari_sela_checkBox");

    m_spnErkenSela = new QSpinBox(m_genelAyarlarBox);
    m_spnErkenSela->setGeometry(240, 205, 42, 21);
    m_spnErkenSela->setRange(0, 120);
    m_spnErkenSela->setValue(55);
    m_spnErkenSela->setObjectName("erken_uyari_sela_spinBox");

    m_chkHicriGun = new QCheckBox("Hicri tarihe gün ilave et :", m_genelAyarlarBox);
    m_chkHicriGun->setGeometry(10, 228, 225, 22);
    m_chkHicriGun->setFont(ayarFont);
    m_chkHicriGun->setToolTip("Hicri tarihteki gecikmeyi önlemek için kullanabilirsiniz. Ayda bir hicri tarihi başka kaynaktan kontrol etmek gerekebilir.");
    m_chkHicriGun->setObjectName("hicri_gun_ekle_checkBox");

    m_spnHicriGun = new QSpinBox(m_genelAyarlarBox);
    m_spnHicriGun->setGeometry(240, 228, 42, 21);
    m_spnHicriGun->setRange(-1, 1);
    m_spnHicriGun->setObjectName("hicri_tarih_gun_ekle_spinBox");

    m_btnGoster = new QPushButton(" Vakitleri aylık olarak göster", m_genelAyarlarBox);
    m_btnGoster->setGeometry(21, 254, 263, 25);
    m_btnGoster->setFont(ayarFont);
    m_btnGoster->setObjectName("vakitleri_goster_pushButton");
    connect(m_btnGoster, &QPushButton::clicked, this, &MainWindow::vakitleriTopluGoster);

    m_btnGuncelle = new QPushButton("Aylık vakitleri şimdi güncelle", m_genelAyarlarBox);
    m_btnGuncelle->setGeometry(21, 283, 263, 25);
    m_btnGuncelle->setFont(ayarFont);
    m_btnGuncelle->setObjectName("vakitleri_guncelle_pushButton");
    connect(m_btnGuncelle, &QPushButton::clicked, this, &MainWindow::diyanetAylikVakitleriKaydet);

    m_btnVarsayilan = new QPushButton("Varsayılan", m_genelAyarlarBox);
    m_btnVarsayilan->setGeometry(15, 313, 126, 27);
    m_btnVarsayilan->setFont(ayarFont);
    m_btnVarsayilan->setToolTip("Varsayılan ayarlara dönmek isterseniz buraya tıklayın.");
    m_btnVarsayilan->setObjectName("ayarlar_varsayilan_pushButton");
    connect(m_btnVarsayilan, &QPushButton::clicked, this, &MainWindow::varsayilanAyarlar);

    m_btnKaydet = new QPushButton("Kaydet", m_genelAyarlarBox);
    m_btnKaydet->setGeometry(155, 313, 126, 27);
    m_btnKaydet->setFont(ayarFont);
    m_btnKaydet->setToolTip("Program kapanıp açıldığında yaptığınız değişikliklerin\nhatırlanması için buraya tıklayın.");
    m_btnKaydet->setObjectName("ayarlar_kaydet_pushButton");
    connect(m_btnKaydet, &QPushButton::clicked, this, &MainWindow::ayarlariKaydet);

    // 2. Bölgesel Ayarlar GroupBox
    m_bolgeselAyarlarBox = new QGroupBox(m_ayarlarTab);
    m_bolgeselAyarlarBox->setObjectName("bolgesel_ayarlar_groupBox");
    m_bolgeselAyarlarBox->setTitle("");
    m_bolgeselAyarlarBox->setMinimumSize(280, 348);
    m_bolgeselAyarlarBox->setGeometry(13, 38, 301, 348);
    m_bolgeselAyarlarBox->setFont(ayarFont);
    m_bolgeselAyarlarBox->hide();

    m_lblBolgeselUlke = new QLabel("Ülke :", m_bolgeselAyarlarBox);
    m_lblBolgeselUlke->setGeometry(15, 28, 65, 23);
    m_lblBolgeselUlke->setFont(ayarFont);
    m_lblBolgeselUlke->setObjectName("bolgesel_ayarlar_ulke_label");

    m_cmbUlke = new QComboBox(m_bolgeselAyarlarBox);
    m_cmbUlke->setGeometry(80, 25, 205, 28);
    m_cmbUlke->setFont(ayarFont);
    m_cmbUlke->setObjectName("ulkeler_comboBox");

    m_lblBolgeselSehir = new QLabel("Şehir :", m_bolgeselAyarlarBox);
    m_lblBolgeselSehir->setGeometry(15, 73, 65, 23);
    m_lblBolgeselSehir->setFont(ayarFont);
    m_lblBolgeselSehir->setObjectName("bolgesel_ayarlar_sehir_label");

    m_cmbSehir = new QComboBox(m_bolgeselAyarlarBox);
    m_cmbSehir->setGeometry(80, 70, 205, 28);
    m_cmbSehir->setFont(ayarFont);
    m_cmbSehir->setObjectName("sehirler_comboBox");

    m_lblBolgeselIlce = new QLabel("İlçe :", m_bolgeselAyarlarBox);
    m_lblBolgeselIlce->setGeometry(15, 118, 65, 23);
    m_lblBolgeselIlce->setFont(ayarFont);
    m_lblBolgeselIlce->setObjectName("bolgesel_ayarlar_ilce_label");

    m_cmbIlce = new QComboBox(m_bolgeselAyarlarBox);
    m_cmbIlce->setGeometry(80, 115, 205, 28);
    m_cmbIlce->setFont(ayarFont);
    m_cmbIlce->setObjectName("ilceler_comboBox");

    m_lblBolgeselDil = new QLabel("Dil :", m_bolgeselAyarlarBox);
    m_lblBolgeselDil->setGeometry(15, 163, 65, 23);
    m_lblBolgeselDil->setFont(ayarFont);
    m_lblBolgeselDil->setObjectName("bolgesel_ayarlar_dil_label");

    m_cmbDil = new QComboBox(m_bolgeselAyarlarBox);
    m_cmbDil->setGeometry(80, 160, 205, 28);
    m_cmbDil->setFont(QFont("Arial", 9));
    m_cmbDil->setObjectName("dil_secimi_comboBox");
    for (const QString& lCode : Localization::supportedLanguages()) {
        m_cmbDil->addItem(Localization::languageDisplayName(lCode), lCode);
    }
    int langIdx = Localization::supportedLanguages().indexOf(Localization::currentLanguage());
    if (langIdx >= 0) m_cmbDil->setCurrentIndex(langIdx);
    connect(m_cmbDil, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onDilDegisti);

    m_btnBolgeVarsayilan = new QPushButton("Varsayılan", m_bolgeselAyarlarBox);
    m_btnBolgeVarsayilan->setGeometry(15, 313, 126, 27);
    m_btnBolgeVarsayilan->setFont(ayarFont);
    m_btnBolgeVarsayilan->setToolTip("Varsayılan ayarlara dönmek isterseniz buraya tıklayın.");
    m_btnBolgeVarsayilan->setObjectName("bolgesel_ayarlar_varsayilan_pushButton");
    connect(m_btnBolgeVarsayilan, &QPushButton::clicked, this, &MainWindow::varsayilanAyarlar);

    m_btnBolgeKaydet = new QPushButton("Kaydet", m_bolgeselAyarlarBox);
    m_btnBolgeKaydet->setGeometry(155, 313, 126, 27);
    m_btnBolgeKaydet->setFont(ayarFont);
    m_btnBolgeKaydet->setToolTip("Program kapanıp açıldığında yaptığınız değişikliklerin\nhatırlanması için buraya tıklayın.");
    m_btnBolgeKaydet->setObjectName("bolgesel_ayarlar_kaydet_pushButton");
    connect(m_btnBolgeKaydet, &QPushButton::clicked, this, &MainWindow::ayarlariKaydet);

    m_tabWidget->addTab(m_ayarlarTab, QIcon(":/resimler/ayarlar.png"), "");
    m_tabWidget->setTabToolTip(2, "Ayarlar (Genel & Bölgesel)");
}

void MainWindow::setupSesTab() {
    m_sesTab = new QWidget();
    m_sesTab->setObjectName("ses_dosyalari_tab");

    static const QStringList sesler = {"sabah", "ogle", "ikindi", "aksam", "yatsi", "dua", "sela", "uyari"};
    static const QStringList etiketler = {"Sabah :", "Öğle :", "İkindi :", "Akşam :", "Yatsı :", "Dua :", "Sela :", "Uyarı :"};
    static const int yKonumlar[8] = {18, 56, 94, 132, 170, 208, 248, 288};

    QFont sesFont("Arial", 11);
    QIcon playIco = style()->standardIcon(QStyle::SP_MediaPlay);

    for (int i = 0; i < 8; ++i) {
        int y = yKonumlar[i];
        QLabel* lbl = new QLabel(etiketler[i], m_sesTab);
        lbl->setGeometry(10, y + 2, 70, 17);
        lbl->setFont(sesFont);
        lbl->setObjectName(sesler[i] + "_ses_label");
        m_sesLabels[sesler[i]] = lbl;

        QLineEdit* le = new QLineEdit(m_sesTab);
        le->setGeometry(65, y - 1, 135, 25);
        le->setReadOnly(true);
        le->setObjectName(sesler[i] + "_lineEdit");
        m_sesEdits[sesler[i]] = le;

        QToolButton* btnEkle = new QToolButton(m_sesTab);
        btnEkle->setText("Ekle");
        btnEkle->setGeometry(205, y, 40, 23);
        btnEkle->setObjectName(sesler[i] + "_toolButton");
        connect(btnEkle, &QToolButton::clicked, this, [this, le]() {
            QString path = QFileDialog::getOpenFileName(this, "Aç", "", Localization::tr("ses_filtre"));
            if (!path.isEmpty()) le->setText(path);
        });
        m_sesEkleBtns[sesler[i]] = btnEkle;

        QToolButton* btnPlay = new QToolButton(m_sesTab);
        btnPlay->setGeometry(248, y, 35, 23);
        btnPlay->setIcon(playIco);
        btnPlay->setCursor(Qt::PointingHandCursor);
        btnPlay->setToolTip(Localization::tr("btn_oynat"));
        connect(btnPlay, &QToolButton::clicked, this, [this, le, btnPlay]() {
            onSesOynatDurdur(le, btnPlay);
        });
        m_sesPlayBtns[sesler[i]] = btnPlay;

        QCheckBox* chk = new QCheckBox("", m_sesTab);
        chk->setGeometry(292, y + 2, 21, 20);
        chk->setChecked(true);
        m_sesChecks[sesler[i]] = chk;
        if (i < 5) {
            chk->setObjectName(sesler[i] + "_ezani_oku_checkBox");
            connect(chk, &QCheckBox::stateChanged, this, &MainWindow::ezanOkuKontrol);
        } else if (sesler[i] == "dua") {
            chk->setObjectName("dua_oku_checkBox");
        } else if (sesler[i] == "sela") {
            chk->setObjectName("sela_oku_checkBox");
        } else {
            chk->setObjectName("uyari_sesi_checkBox");
        }
    }

    m_sesSlider = new QSlider(Qt::Horizontal, m_sesTab);
    m_sesSlider->setGeometry(189, 320, 121, 22);
    m_sesSlider->setRange(0, 100);
    m_sesSlider->setValue(30);
    m_sesSlider->setTickPosition(QSlider::TicksBelow);
    m_sesSlider->setObjectName("horizontalSlider");
    connect(m_sesSlider, &QSlider::valueChanged, this, [this](int val) {
        m_player->setVolume(val);
        m_onizlemePlayer->setVolume(val);
    });

    QFont ayarFont("Arial", 10);
    m_btnSesVarsayilan = new QPushButton("Varsayılan", m_sesTab);
    m_btnSesVarsayilan->setGeometry(15, 350, 126, 28);
    m_btnSesVarsayilan->setFont(ayarFont);
    m_btnSesVarsayilan->setToolTip("Varsayılan ayarlara dönmek isterseniz buraya tıklayın.");
    m_btnSesVarsayilan->setObjectName("ses_varsayilan_pushButton");
    connect(m_btnSesVarsayilan, &QPushButton::clicked, this, &MainWindow::sesVarsayilanAyarlar);

    m_btnSesKaydet = new QPushButton("Kaydet", m_sesTab);
    m_btnSesKaydet->setGeometry(155, 350, 126, 28);
    m_btnSesKaydet->setFont(ayarFont);
    m_btnSesKaydet->setToolTip("Program kapanıp açıldığında yaptığınız değişikliklerin\nhatırlanması için buraya tıklayın.");
    m_btnSesKaydet->setObjectName("ses_kaydet_pushButton");
    connect(m_btnSesKaydet, &QPushButton::clicked, this, &MainWindow::ayarlariKaydet);

    m_tabWidget->addTab(m_sesTab, QIcon(":/resimler/ses_dosyalari.png"), "");
    m_tabWidget->setTabToolTip(3, "Ses Ayarları");
}

void MainWindow::setupLinklerTab() {
    m_linkTab = new QWidget();
    m_linkTab->setObjectName("linkler_tab");

    m_linklerBox = new QGroupBox("Yararlı Siteler", m_linkTab);
    m_linklerBox->setObjectName("linkler_groupBox");
    m_linklerBox->setGeometry(13, 4, 301, 381);

    struct LinkInfo { QString objName; int y; int w; QString url; QString ad; };
    QVector<LinkInfo> linkler = {
        {"site_01_label", 25, 270, "http://www.sorularlaislamiyet.com/", "Sorularla İslamiyet"},
        {"site_04_label", 65, 270, "http://www.kurantv.net", "Kuran.tv"},
        {"site_05_label", 105, 270, "http://kuran.diyanet.gov.tr", "Diyanet Kur'an Eğitimi"},
        {"site_06_label", 145, 270, "https://kuran.diyanet.gov.tr/mushaf", "Diyanet (Kur'an-ı Kerim ve Meâli)"},
        {"site_07_label", 185, 270, "http://tanzil.net", "Tanzil (Kur'an-ı Kerim ve Meâli)"},
        {"site_11_label", 225, 270, "https://vakithesaplama.diyanet.gov.tr/icerik.php?icerik=153", "Dini Günler ve Geceler"},
        {"site_12_label", 265, 141, "http://www.erisale.com/", "Risale-i Nur Külliyatı"}
    };

    for (const auto& info : linkler) {
        QLabel* lbl = new QLabel(m_linklerBox);
        lbl->setObjectName(info.objName);
        lbl->setGeometry(15, info.y, info.w, 23);
        lbl->setFont(QFont("Arial", 11));
        lbl->setOpenExternalLinks(true);
        QString html = QString(
            "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
            "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
            "p, li { white-space: pre-wrap; }\n"
            "</style></head><body style=\" font-family:'Arial'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
            "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
            "<a href=\"%1\"><span style=\" text-decoration: underline; color:#0000ff;\">%2</span></a></p></body></html>"
        ).arg(info.url, info.ad);
        lbl->setText(html);
        m_linkLabels.append(lbl);
    }

    QPushButton* btnAndroid = new QPushButton(m_linklerBox);
    btnAndroid->setObjectName("pushButton_android");
    btnAndroid->setGeometry(170, 265, 21, 21);
    btnAndroid->setIcon(QIcon(":/resimler/android.png"));
    btnAndroid->setIconSize(QSize(24, 24));
    btnAndroid->setFlat(true);
    btnAndroid->setToolTip("Anroid için Risale-i Nur");
    connect(btnAndroid, &QPushButton::clicked, this, []() {
        QDesktopServices::openUrl(QUrl("https://play.google.com/store/apps/details?id=com.nesil.risalei_nur&hl=tr&gl=US"));
    });

    QPushButton* btnIos = new QPushButton(m_linklerBox);
    btnIos->setObjectName("pushButton_ios");
    btnIos->setGeometry(200, 265, 21, 21);
    btnIos->setIcon(QIcon(":/resimler/ios.png"));
    btnIos->setIconSize(QSize(24, 24));
    btnIos->setFlat(true);
    btnIos->setToolTip("iPhone için Risale-i Nur");
    connect(btnIos, &QPushButton::clicked, this, []() {
        QDesktopServices::openUrl(QUrl("https://apps.apple.com/az/app/risale-i-nur-k%C3%BClliyat%C4%B1-s%C3%B6z/id814437480"));
    });

    m_tabWidget->addTab(m_linkTab, QIcon(":/resimler/linkler.png"), "");
    m_tabWidget->setTabToolTip(4, "Yararlı Siteler");
}

void MainWindow::setupMenuBar() {
    QMenuBar* mb = menuBar();
    mb->setObjectName("menubar");
    mb->setGeometry(0, 0, 351, 23);
    mb->setFont(QFont("Arial", 11));

    QFont menuFont("Arial", 10);
    QFont actFont("Arial", 11);

    // 1. Dosya
    m_menuDosya = mb->addMenu("Dosya");
    m_menuDosya->setObjectName("menuDosya");
    m_menuDosya->setFont(menuFont);

    m_actCikis = m_menuDosya->addAction(QIcon(":/resimler/cikis.png"), "Çıkış");
    m_actCikis->setObjectName("actionCikis");
    m_actCikis->setFont(actFont);
    m_actCikis->setIconVisibleInMenu(true);
    m_actCikis->setShortcut(QKeySequence("Esc"));
    connect(m_actCikis, &QAction::triggered, this, &MainWindow::programiKapat);

    // 2. Görünüm
    m_menuGorunum = mb->addMenu("Görünüm");
    m_menuGorunum->setObjectName("menuGorunum");
    m_menuGorunum->setFont(menuFont);

    m_arayuzGroup = new QActionGroup(this);
    m_arayuzGroup->setExclusive(true);

    struct ArayuzSecenek { QString key; QString text; };
    QVector<ArayuzSecenek> modlar = {
        {"saat", "⏱️ Vakit Saati (Kadran & Canlı)"},
        {"gokyuzu", "🌙 Dinamik Gökyüzü & Hilal"},
        {"kartlar", "📋 Modern Kartlar"},
        {"klasik", "🏛️ Klasik Görünüm"}
    };

    for (const auto& m : modlar) {
        QAction* act = m_menuGorunum->addAction(m.text);
        act->setCheckable(true);
        if (m.key == "saat") act->setChecked(true);
        connect(act, &QAction::triggered, this, [this, k = m.key]() { arayuzModunuUygula(k, true); });
        m_arayuzGroup->addAction(act);
        m_arayuzActions[m.key] = act;
    }

    m_menuGorunum->addSeparator();

    // Renk Teması Alt Menüsü (Temalar.py sırasıyla birebir)
    m_menuTema = m_menuGorunum->addMenu("Renk Teması");
    m_temaGroup = new QActionGroup(this);
    m_temaGroup->setExclusive(true);

    const auto& temalarMap = Temalar::getTemalar();
    for (const QString& key : Temalar::getTemaAnahtarlari()) {
        if (!temalarMap.contains(key)) continue;
        QAction* tAct = m_menuTema->addAction(temalarMap[key].ad);
        tAct->setCheckable(true);
        if (key == "varsayilan") tAct->setChecked(true);
        connect(tAct, &QAction::triggered, this, [this, key]() { temayiUygula(key, true); });
        m_temaGroup->addAction(tAct);
        m_temaActions[key] = tAct;
    }

    // Pencere Stili Alt Menüsü
    m_menuStil = m_menuGorunum->addMenu("Pencere Stili");
    m_stilGroup = new QActionGroup(this);
    m_stilGroup->setExclusive(true);

    struct StilSecenek { QString key; QString text; };
    QVector<StilSecenek> stiller = {
        {"Fusion", "Fusion (Modern)"},
        {"windowsvista", "Windows Vista"},
        {"Windows", "Klasik Windows"}
    };
    for (const auto& st : stiller) {
        QAction* sAct = m_menuStil->addAction(st.text);
        sAct->setFont(actFont);
        sAct->setCheckable(true);
        if (st.key == "Fusion") sAct->setChecked(true);
        connect(sAct, &QAction::triggered, this, [this, k = st.key]() { pencereStiliniUygula(k, true); });
        m_stilGroup->addAction(sAct);
        m_stilActions[st.key] = sAct;
    }

    // 3. Yardım
    m_menuYardim = mb->addMenu("Yardım");
    m_menuYardim->setObjectName("menuYardim");
    m_menuYardim->setFont(menuFont);

    m_actLisans = m_menuYardim->addAction("Lisans");
    m_actLisans->setObjectName("actionLisans");
    m_actLisans->setFont(actFont);
    connect(m_actLisans, &QAction::triggered, this, &MainWindow::lisansAc);

    m_menuYardim->addSeparator();

    m_actHakkinda = m_menuYardim->addAction("Hakkında");
    m_actHakkinda->setObjectName("actionHakkinda");
    m_actHakkinda->setFont(actFont);
    connect(m_actHakkinda, &QAction::triggered, this, &MainWindow::hakkindaDialog);

    m_actQt = m_menuYardim->addAction("Qt Hakkında");
    m_actQt->setObjectName("actionQtHakkinda");
    m_actQt->setFont(actFont);
    connect(m_actQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::setupTray() {
    m_actVakitleriGoster = new QAction(QIcon(":/resimler/cami.png"), "Vakitleri Göster", this);
    m_actVakitleriGoster->setFont(QFont("Arial", 11));
    m_actVakitleriGoster->setIconVisibleInMenu(true);
    connect(m_actVakitleriGoster, &QAction::triggered, this, &MainWindow::vakitleriGoster);

    m_trayIcon = new QSystemTrayIcon(QIcon(":/resimler/cami.png"), this);
    QMenu* trayMenu = new QMenu(this);
    trayMenu->addAction(m_actVakitleriGoster);
    trayMenu->addAction(m_actCikis);
    m_trayIcon->setContextMenu(trayMenu);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);
}

void MainWindow::bolgeselAyarlariHazirla() {
    m_cmbUlke->blockSignals(true);
    m_cmbUlke->clear();
    m_cmbSehir->clear();
    m_cmbIlce->clear();

    for (const QString& ulke : DiyanetUlkeler::getUlkeler().keys()) {
        m_cmbUlke->addItem(ulke);
    }
    m_cmbUlke->blockSignals(false);

    connect(m_cmbUlke, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::sehirKontrolu);
    connect(m_cmbSehir, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::ilceKontrolu);
}

void MainWindow::labelAdiniDegistir(const QString& secilenUlke) {
    if (secilenUlke == "ABD" || secilenUlke == "KANADA" || secilenUlke == "ALMANYA" || secilenUlke == "AVUSTRALYA") {
        m_lblBolgeselSehir->setText("Eyalet :");
        m_lblBolgeselIlce->setText("Şehir :");
    } else if (secilenUlke == "TÜRKİYE") {
        m_lblBolgeselSehir->setText("Şehir :");
        m_lblBolgeselIlce->setText("İlçe :");
    } else {
        m_lblBolgeselSehir->setText("Bölge :");
        m_lblBolgeselIlce->setText("Şehir :");
    }
}

void MainWindow::sehirKontrolu() {
    QString secilenUlke = m_cmbUlke->currentText();
    labelAdiniDegistir(secilenUlke);

    m_cmbSehir->blockSignals(true);
    m_cmbSehir->clear();
    m_sehirlerMap = DiyanetUlkeler::sehirleriAl(secilenUlke, m_settings);
    for (const QString& s : m_sehirlerMap.keys()) {
        m_cmbSehir->addItem(s);
    }
    m_cmbSehir->blockSignals(false);

    ilceKontrolu();
}

void MainWindow::ilceKontrolu() {
    m_cmbIlce->clear();
    m_cmbIlce->setEnabled(true);

    QString secilenUlke = m_cmbUlke->currentText();
    QString secilenSehir = m_cmbSehir->currentText();
    if (m_cmbSehir->currentIndex() >= 0 && !m_sehirlerMap.isEmpty()) {
        auto paket = DiyanetUlkeler::ilceleriAl(m_sehirlerMap, secilenUlke, secilenSehir, m_settings);
        m_ilcelerMap = paket.ilceler;
        m_linklerMap = paket.linkler;
        for (const QString& ilce : m_ilcelerMap.keys()) {
            m_cmbIlce->addItem(ilce);
        }
    }
}

void MainWindow::ulkeSehirBilgisiniAl() {
    m_ulke = m_cmbUlke->currentText();
    m_sehir = m_cmbSehir->currentText();
    m_ilce = m_cmbIlce->currentText();

    m_klasikUlkeLabel->setText(m_ulke);
    if (!m_ilce.isEmpty()) {
        m_seciliYer = m_ilce;
        if (!m_sehir.isEmpty() && m_sehir != m_ulke && m_sehir != m_ilce) {
            m_klasikSehirLabel->setText(m_ilce);
            m_klasikSehirLabel->setToolTip(m_sehir + " / " + m_ilce);
            m_modernUlkeLabel->setText(m_ulke + " • " + m_sehir);
            m_modernSehirLabel->setText(m_ilce);
            m_modernSehirLabel->setToolTip(m_sehir + " / " + m_ilce);
        } else {
            m_klasikSehirLabel->setText(m_ilce);
            m_klasikSehirLabel->setToolTip(m_ilce);
            m_modernUlkeLabel->setText(m_ulke);
            m_modernSehirLabel->setText(m_ilce);
            m_modernSehirLabel->setToolTip(m_ilce);
        }
    } else {
        m_seciliYer = m_sehir;
        m_klasikSehirLabel->setText(m_sehir);
        m_klasikSehirLabel->setToolTip(m_sehir);
        m_modernUlkeLabel->setText(m_ulke);
        m_modernSehirLabel->setText(m_sehir);
        m_modernSehirLabel->setToolTip(m_sehir);
    }

    if (m_havaWidget) {
        m_havaWidget->konumAyarla(m_ulke, m_sehir, m_seciliYer, m_aktifTema);
    }
}

void MainWindow::vakitleriAl() {
    ulkeSehirBilgisiniAl();

    if (!m_ulke.isEmpty() && !m_sehir.isEmpty()) {
        if (!QFile::exists(m_veritabaniDosyasi) || QFile(m_veritabaniDosyasi).size() == 0) {
            diyanetAylikVakitleriKaydet();
        }

        if (!m_vakitler->gunlukVakitleriYukle(m_seciliYer)) {
            if (!m_vakitler->veritabanindakiYerBilgisi) {
                diyanetAylikVakitleriKaydet();
            }
        }

        if (!m_labelTimer) {
            m_labelTimer = new QTimer(this);
            connect(m_labelTimer, &QTimer::timeout, this, &MainWindow::onLabelGuncelle);
        }
        if (!m_labelTimer->isActive()) {
            m_labelTimer->start(500);
        }
        onLabelGuncelle();
    }
}

void MainWindow::diyanetAylikVakitleriKaydet() {
    QApplication::setOverrideCursor(Qt::BusyCursor);
    statusBar()->showMessage("Vakitler güncelleniyor...");
    ulkeSehirBilgisiniAl();

    QString link = m_linklerMap.value(m_seciliYer);
    bool basarili = m_vakitler->diyanetAylikVakitleriKaydet(m_seciliYer, link);

    QApplication::restoreOverrideCursor();
    if (basarili) {
        statusBar()->showMessage(Localization::tr("kaynak"));
        onLabelGuncelle();
    } else {
        statusBar()->clearMessage();
        QMessageBox::warning(this, "Bağlantı Hatası",
                             "Diyanetin sitesine bağlantı kurulamadığından dolayı \n"
                             "namaz vakitleri veritabanı güncellenemedi.",
                             "Tamam");
    }
}

void MainWindow::vakitleriTopluGoster() {
    if (!QFile::exists(m_veritabaniDosyasi) || QFile(m_veritabaniDosyasi).size() == 0) {
        diyanetAylikVakitleriKaydet();
    }
    AylikVakitlerDialog dlg(m_veritabaniDosyasi, this);
    if (!dlg.veritabaniVarmi) {
        diyanetAylikVakitleriKaydet();
        AylikVakitlerDialog dlg2(m_veritabaniDosyasi, this);
        if (dlg2.veritabaniVarmi) dlg2.exec();
    } else {
        dlg.exec();
    }
}

void MainWindow::ayarlariUygula() {
    m_gorunum = m_settings->value("Ayarlar/gorunum", "Fusion").toString();
    if (m_gorunum.isEmpty()) m_gorunum = "Fusion";
    pencereStiliniUygula(m_gorunum, false);

    QString kayitliTema = m_settings->value("Ayarlar/tema", "varsayilan").toString();
    if (!Temalar::getTemalar().contains(kayitliTema)) kayitliTema = "varsayilan";
    temayiUygula(kayitliTema, false);

    QString kayitliMod = m_settings->value("Ayarlar/arayuz_modu", "saat").toString();
    if (kayitliMod == "modern") kayitliMod = "kartlar";
    if (kayitliMod != "saat" && kayitliMod != "gokyuzu" && kayitliMod != "kartlar" && kayitliMod != "klasik") {
        kayitliMod = "saat";
    }
    arayuzModunuUygula(kayitliMod, false);

    int ulkeIndeks = ayarInt(m_settings->value("Ayarlar/ulke"), 190);
    int sehirIndeks = ayarInt(m_settings->value("Ayarlar/sehir"), 77);
    int ilceIndeks = ayarInt(m_settings->value("Ayarlar/ilce"), 17);

    if (ulkeIndeks == 186 && sehirIndeks < 0) {
        int trIdx = m_cmbUlke->findText("TÜRKİYE");
        if (trIdx >= 0) ulkeIndeks = trIdx;
    }

    if (m_cmbUlke->currentIndex() == ulkeIndeks) {
        sehirKontrolu();
    } else if (ulkeIndeks >= 0 && ulkeIndeks < m_cmbUlke->count()) {
        m_cmbUlke->setCurrentIndex(ulkeIndeks);
    }

    if (m_cmbSehir->count() > 0) {
        if (sehirIndeks < 0 || sehirIndeks >= m_cmbSehir->count()) {
            int istIdx = m_cmbSehir->findText("İSTANBUL");
            sehirIndeks = (istIdx >= 0) ? istIdx : 0;
        }
        if (m_cmbSehir->currentIndex() == sehirIndeks) {
            if (m_cmbIlce->count() == 0) ilceKontrolu();
        } else {
            m_cmbSehir->setCurrentIndex(sehirIndeks);
        }
    }

    if (m_cmbIlce->count() > 0) {
        if (ilceIndeks < 0 || ilceIndeks >= m_cmbIlce->count()) {
            int istIdx = m_cmbIlce->findText("İSTANBUL");
            ilceIndeks = (istIdx >= 0) ? istIdx : 0;
        }
        m_cmbIlce->setCurrentIndex(ilceIndeks);
    }

    m_chkOtoCalistir->setChecked(ayarInt(m_settings->value("Ayarlar/otomatik_calistir"), 2) == 2);
    m_chkIkonGoster->setChecked(ayarInt(m_settings->value("Ayarlar/ikon_goster"), 2) == 2);
    m_chkSistemTepsisindeBaslat->setChecked(ayarInt(m_settings->value("Ayarlar/sistem_tepsisinde_baslat"), 0) == 2);
    m_chkOtoGuncelle->setChecked(ayarInt(m_settings->value("Ayarlar/otomatik_guncelle"), 2) == 2);
    m_chkHerZamanUstte->setChecked(ayarInt(m_settings->value("Ayarlar/her_zaman_ustte"), 0) == 2);
    m_chkBesmele->setChecked(ayarInt(m_settings->value("Ayarlar/besmele_ile_basla"), 0) == 2);
    m_chkWinamp->setChecked(ayarInt(m_settings->value("Ayarlar/winamp_duraklat"), 0) == 2);
    m_chkSpotify->setChecked(ayarInt(m_settings->value("Ayarlar/stopify_duraklat"), 0) == 2);

    m_sesChecks["sabah"]->setChecked(ayarInt(m_settings->value("Ayarlar/sabah_ezani_oku"), 2) == 2);
    m_sesChecks["ogle"]->setChecked(ayarInt(m_settings->value("Ayarlar/ogle_ezani_oku"), 2) == 2);
    m_sesChecks["ikindi"]->setChecked(ayarInt(m_settings->value("Ayarlar/ikindi_ezani_oku"), 2) == 2);
    m_sesChecks["aksam"]->setChecked(ayarInt(m_settings->value("Ayarlar/aksam_ezani_oku"), 2) == 2);
    m_sesChecks["yatsi"]->setChecked(ayarInt(m_settings->value("Ayarlar/yatsi_ezani_oku"), 2) == 2);
    m_sesChecks["sela"]->setChecked(ayarInt(m_settings->value("Ayarlar/sela_oku"), 2) == 2);
    m_sesChecks["dua"]->setChecked(ayarInt(m_settings->value("Ayarlar/dua_oku"), 2) == 2);

    int erkenUyari = ayarInt(m_settings->value("Ayarlar/erken_uyari"), 2);
    m_chkErkenUyari->setChecked(erkenUyari == 2);
    m_spnErkenUyari->setValue(ayarInt(m_settings->value("Ayarlar/erken_uyari_suresi"), 45));

    m_chkErkenSela->setChecked(ayarInt(m_settings->value("Ayarlar/erken_uyari_sela"), 2) == 2);
    m_spnErkenSela->setValue(ayarInt(m_settings->value("Ayarlar/erken_uyari_suresi_sela"), 55));

    int uyariSesiCal = ayarInt(m_settings->value("Ayarlar/uyari_sesi_cal"), 2);
    m_sesChecks["uyari"]->setChecked(uyariSesiCal == 2);
    erkenUyariKontrolWidget();
    ezanOkuKontrol();

    m_chkHicriGun->setChecked(ayarInt(m_settings->value("Ayarlar/hicri_gun_ilave"), 2) == 2);
    m_spnHicriGun->setValue(ayarInt(m_settings->value("Ayarlar/ilave_gun_sayisi"), 0));

    int sesAyari = ayarInt(m_settings->value("Ses_Ayarlari/ses_ayari"), 30);
    m_sesSlider->setValue(sesAyari);

    static const QStringList sesKeys = {"sabah", "ogle", "ikindi", "aksam", "yatsi", "dua", "uyari", "sela"};
    for (const QString& k : sesKeys) {
        QString v = m_settings->value("Ses_Ayarlari/" + k).toString();
        if (!v.isEmpty()) m_sesEdits[k]->setText(v);
    }

    vakitleriAl();
}

void MainWindow::sesVarsayilanAyarlar() {
    m_sesEdits["sabah"]->setText(m_sesDosyalariKonum + "sabah_saba.mp3");
    m_sesEdits["ogle"]->setText(m_sesDosyalariKonum + "ogle_rast.mp3");
    m_sesEdits["ikindi"]->setText(m_sesDosyalariKonum + "ikindi_hicaz.mp3");
    m_sesEdits["aksam"]->setText(m_sesDosyalariKonum + "aksam_segah.mp3");
    m_sesEdits["yatsi"]->setText(m_sesDosyalariKonum + "yatsi_ussak.mp3");
    m_sesEdits["dua"]->setText(m_sesDosyalariKonum + "ezan_duasi.mp3");
    m_sesEdits["uyari"]->setText(m_sesDosyalariKonum + "uyari.mp3");
    m_sesEdits["sela"]->setText(m_sesDosyalariKonum + "sela.mp3");
}

void MainWindow::varsayilanAyarlar() {
    statusBar()->clearMessage();
    if (QFile::exists(m_ayarDosyasi)) QFile::remove(m_ayarDosyasi);
    if (QFile::exists(m_veritabaniDosyasi)) QFile::remove(m_veritabaniDosyasi);

    QString ontanimli = m_programinKonumu + "/ontanimli_ayarlar.ini";
    if (QFile::exists(ontanimli)) {
        QFile::copy(ontanimli, m_ayarDosyasi);
    }
    m_settings->sync();
    sesVarsayilanAyarlar();
    ayarlariUygula();
}

void MainWindow::ayarlariKaydet() {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_settings->setValue("Ayarlar/gorunum", m_gorunum);
    m_settings->setValue("Ayarlar/tema", m_aktifTema);
    m_settings->setValue("Ayarlar/arayuz_modu", m_arayuzModu);
    m_settings->setValue("Ayarlar/ulke", m_cmbUlke->currentIndex());
    m_settings->setValue("Ayarlar/sehir", m_cmbSehir->currentIndex());
    m_settings->setValue("Ayarlar/ilce", m_cmbIlce->currentIndex());
    m_settings->setValue("Ayarlar/otomatik_calistir", static_cast<int>(m_chkOtoCalistir->checkState()));
    m_settings->setValue("Ayarlar/ikon_goster", static_cast<int>(m_chkIkonGoster->checkState()));
    m_settings->setValue("Ayarlar/sistem_tepsisinde_baslat", static_cast<int>(m_chkSistemTepsisindeBaslat->checkState()));
    m_settings->setValue("Ayarlar/otomatik_guncelle", static_cast<int>(m_chkOtoGuncelle->checkState()));
    m_settings->setValue("Ayarlar/her_zaman_ustte", static_cast<int>(m_chkHerZamanUstte->checkState()));
    m_settings->setValue("Ayarlar/besmele_ile_basla", static_cast<int>(m_chkBesmele->checkState()));
    m_settings->setValue("Ayarlar/winamp_duraklat", static_cast<int>(m_chkWinamp->checkState()));
    m_settings->setValue("Ayarlar/stopify_duraklat", static_cast<int>(m_chkSpotify->checkState()));
    m_settings->setValue("Ayarlar/sabah_ezani_oku", static_cast<int>(m_sesChecks["sabah"]->checkState()));
    m_settings->setValue("Ayarlar/ogle_ezani_oku", static_cast<int>(m_sesChecks["ogle"]->checkState()));
    m_settings->setValue("Ayarlar/ikindi_ezani_oku", static_cast<int>(m_sesChecks["ikindi"]->checkState()));
    m_settings->setValue("Ayarlar/aksam_ezani_oku", static_cast<int>(m_sesChecks["aksam"]->checkState()));
    m_settings->setValue("Ayarlar/yatsi_ezani_oku", static_cast<int>(m_sesChecks["yatsi"]->checkState()));
    m_settings->setValue("Ayarlar/dua_oku", static_cast<int>(m_sesChecks["dua"]->checkState()));
    m_settings->setValue("Ayarlar/sela_oku", static_cast<int>(m_sesChecks["sela"]->checkState()));
    m_settings->setValue("Ayarlar/uyari_sesi_cal", static_cast<int>(m_sesChecks["uyari"]->checkState()));
    m_settings->setValue("Ayarlar/erken_uyari", static_cast<int>(m_chkErkenUyari->checkState()));
    m_settings->setValue("Ayarlar/erken_uyari_suresi", m_spnErkenUyari->text());
    m_settings->setValue("Ayarlar/erken_uyari_sela", static_cast<int>(m_chkErkenSela->checkState()));
    m_settings->setValue("Ayarlar/erken_uyari_suresi_sela", m_spnErkenSela->text());
    m_settings->setValue("Ayarlar/hicri_gun_ilave", static_cast<int>(m_chkHicriGun->checkState()));
    m_settings->setValue("Ayarlar/ilave_gun_sayisi", m_spnHicriGun->text());

    static const QStringList sesKeys = {"sabah", "ogle", "ikindi", "aksam", "yatsi", "dua", "uyari", "sela"};
    for (const QString& k : sesKeys) {
        m_settings->setValue("Ses_Ayarlari/" + k, m_sesEdits[k]->text());
    }
    m_settings->setValue("Ses_Ayarlari/ses_ayari", m_sesSlider->value());
    m_settings->sync();

    hicriTarihiGuncelle();
    QApplication::restoreOverrideCursor();
    vakitleriAl();
}

void MainWindow::trayIconGoster() {
    if (m_chkIkonGoster->isChecked()) {
        m_trayIcon->show();
        m_chkSistemTepsisindeBaslat->setEnabled(true);
        if (m_chkSistemTepsisindeBaslat->isChecked()) {
            hide();
        } else {
            show();
        }
    } else {
        m_chkSistemTepsisindeBaslat->setEnabled(false);
        m_chkSistemTepsisindeBaslat->setChecked(false);
        m_trayIcon->hide();
        show();
    }
}

void MainWindow::herZamanUstte() {
    if (m_chkHerZamanUstte->isChecked()) {
        setWindowFlags(m_windowFlags | Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(m_windowFlags & ~Qt::WindowStaysOnTopHint);
    }
    if (!isVisible()) show();
    ekrandaOrtala();
}

void MainWindow::ezanOkuKontrol() {
    bool anyChecked = m_sesChecks["sabah"]->isChecked() ||
                      m_sesChecks["ogle"]->isChecked() ||
                      m_sesChecks["ikindi"]->isChecked() ||
                      m_sesChecks["aksam"]->isChecked() ||
                      m_sesChecks["yatsi"]->isChecked();
    if (anyChecked) {
        m_sesChecks["dua"]->setEnabled(true);
    } else {
        m_sesChecks["dua"]->setEnabled(false);
        m_sesChecks["dua"]->setChecked(false);
    }
}

void MainWindow::erkenUyariKontrolWidget() {
    if (m_chkErkenUyari->isChecked()) {
        m_spnErkenUyari->setEnabled(true);
    } else {
        m_spnErkenUyari->setEnabled(false);
    }
    m_sesChecks["uyari"]->setEnabled(true);
}

void MainWindow::winampVarMi() {
    if (m_chkWinamp->isChecked()) {
        QString winampKonum = m_settings->value("Ayarlar/winamp_konum").toString();
        if (winampKonum.isEmpty() || winampKonum == "None") {
            QMessageBox::information(this, "Bilgilendirme",
                                     "Winamp programını seçmeniz gerekiyor. \nProgram seçerken aşağıdaki konumlara bakın:\n\n"
                                     "C:\\Program Files\\Winamp\n"
                                     "C:\\Program Files (x86)\\Winamp\n"
                                     "C:\\Program Dosyaları (x86)\\Winamp\n"
                                     "C:\\Program Dosyaları\\Winamp\n",
                                     QMessageBox::Ok);
            QString konum = QFileDialog::getOpenFileName(this, "Aç", "", "Exe Dosyaları (winamp.exe)");
            if (!konum.isEmpty()) {
                m_settings->setValue("Ayarlar/winamp_konum", konum);
                m_chkWinamp->setChecked(true);
            } else {
                m_chkWinamp->setChecked(false);
            }
        }
    }
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        if (isVisible()) {
            hide();
        } else {
            m_tabWidget->setCurrentIndex(0);
            show();
            activateWindow();
        }
    }
}

void MainWindow::onSekmeDegisti(int index) {
    if (m_tabWidget->widget(index) != m_sesTab) {
        if (m_calanSesBtn) {
            m_onizlemePlayer->stop();
            m_calanSesBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            m_calanSesBtn->setToolTip("Oynat");
            m_calanSesBtn = nullptr;
        }
    }
}

void MainWindow::onAyarAltSekmeDegisti(int index) {
    if (index == 0) {
        m_btnGenelAyarlar->setChecked(true);
        m_genelAyarlarBox->show();
        m_bolgeselAyarlarBox->hide();
    } else {
        m_btnBolgeselAyarlar->setChecked(true);
        m_genelAyarlarBox->hide();
        m_bolgeselAyarlarBox->show();
    }
}

void MainWindow::onSesOynatDurdur(QLineEdit* lineEdit, QToolButton* btn) {
    if (m_calanSesBtn == btn) {
        m_onizlemePlayer->stop();
        btn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        btn->setToolTip("Oynat");
        m_calanSesBtn = nullptr;
        return;
    }

    if (m_calanSesBtn) {
        m_calanSesBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        m_calanSesBtn->setToolTip("Oynat");
        m_onizlemePlayer->stop();
        m_calanSesBtn = nullptr;
    }

    QString path = lineEdit->text().trimmed();
    if (!QFile::exists(path)) {
        QMessageBox::warning(this, "Hata", "Ses dosyası bulunamadı:\n" + path, "Tamam");
        return;
    }

    m_onizlemePlayer->setMedia(QUrl::fromLocalFile(path));
    m_onizlemePlayer->setVolume(m_sesSlider->value());
    m_onizlemePlayer->play();

    btn->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    btn->setToolTip("Durdur");
    m_calanSesBtn = btn;
}

void MainWindow::onSesDurumDegisti(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::StoppedState) {
        if (m_calanSesBtn) {
            m_calanSesBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            m_calanSesBtn->setToolTip("Oynat");
            m_calanSesBtn = nullptr;
        }
    }
}

void MainWindow::onAnimasyonTick() {
    if (!isVisible()) return;
    int idx = m_tabWidget->currentIndex();
    if (idx == 0) {
        if (m_arayuzModu == "saat") m_vakitSaatiWidget->animasyonAdimi();
        else if (m_arayuzModu == "gokyuzu") m_gokyuzuWidget->animasyonAdimi();
    } else if (idx == 1) {
        m_havaWidget->animasyonAdimi();
    }
}

void MainWindow::hicriTarihiGuncelle() {
    int ilaveGun = m_chkHicriGun->isChecked() ? m_spnHicriGun->value() : 0;
    QDate hedef = QDate::currentDate().addDays(ilaveGun);
    double jd = m_calverter.gregorianToJd(hedef.year(), hedef.month(), hedef.day());
    auto hTarih = m_calverter.jdToIslamic(jd);

    static const QMap<QString, QStringList> hicriAylarMap = {
        {"tr", {"Muharrem", "Safer", "Rebiülevvel", "Rebiülahir", "Cemaziyelevvel", "Cemaziyelahir",
                "Receb", "Şaban", "Ramazan", "Şevval", "Zilkade", "Zilhicce"}},
        {"en", {"Muharram", "Safar", "Rabi' I", "Rabi' II", "Jumada I", "Jumada II",
                "Rajab", "Sha'ban", "Ramadan", "Shawwal", "Dhu al-Qi'dah", "Dhu al-Hijjah"}},
        {"de", {"Muharram", "Safar", "Rabi' I", "Rabi' II", "Dschumada I", "Dschumada II",
                "Radschab", "Scha'ban", "Ramadan", "Schawwal", "Dhu l-qa'da", "Dhu l-hiddscha"}},
        {"ru", {"Мухаррам", "Сафар", "Раби аль-авваль", "Раби ас-сани", "Джумада аль-уля", "Джумада ас-сания",
                "Раджаб", "Шабан", "Рамадан", "Шавваль", "Зу-ль-када", "Зу-ль-хиджа"}},
        {"zh", {"穆哈兰姆月", "色法尔月", "赖比尔·敖外鲁月", "赖比尔·阿色尼月", "主马达·敖外鲁月", "主马达·阿色尼月",
                "热哲卜月", "舍尔邦月", "赖买丹月", "闪瓦鲁月", "都尔喀尔德月", "都尔黑哲月"}},
        {"ja", {"ムハッラム", "サファル", "ラビー・アル＝アウワル", "ラビー・アッ＝サーニー", "ジュマーダー・アル＝ウーラー", "ジュマーダー・アッ＝サーニー",
                "ラジャブ", "シャアバーン", "ラマダーン", "シャウワール", "ズー・アル＝カアダ", "ズー・アル＝ヒッジャ"}}
    };
    QStringList hicriAylar = hicriAylarMap.value(Localization::currentLanguage(), hicriAylarMap.value("tr"));

    int hGun = std::get<2>(hTarih);
    int hAyIdx = std::get<1>(hTarih) - 1;
    int hYil = std::get<0>(hTarih);
    m_hicriGun = QString::number(hGun);
    m_hicriAy = (hAyIdx >= 0 && hAyIdx < 12) ? hicriAylar[hAyIdx] : "";
    m_hicriYil = QString::number(hYil);
    m_hicriTarih = QString("%1 %2 %3").arg(m_hicriGun, m_hicriAy, m_hicriYil);

    m_klasikHicriLabel->setText(m_hicriTarih);
    m_modernHicriLabel->setText(m_hicriTarih);
    diniGunleriHatirlat();
}

void MainWindow::diniGunleriHatirlat() {
    static const QStringList diniGunler = {
        "11 Rebiülevvel", "1 Recep", "6 Recep", "26 Recep", "14 Şaban",
        "1 Ramazan", "27 Ramazan", "30 Ramazan", "1 Şevval", "2 Şevval",
        "3 Şevval", "9 Zilhicce", "10 Zilhicce", "11 Zilhicce",
        "12 Zilhicce", "13 Zilhicce", "1 Muharrem", "10 Muharrem"
    };
    static const QStringList karsiliklar = {
        "Mevlid Kandili", "Üç Aylar'ın Başlangıcı", "Regaib Kandili",
        "Miraç Kandili", "Beraat Kandili", "Ramazan'ın Başlangıcı",
        "Kadir Gecesi", "Arefe", "Ramazan Bayramı (1. gün)", "Ramazan Bayramı (2. gün)",
        "Ramazan Bayramı (3. gün)", "Arefe", "Kurban Bayramı (1. gün)",
        "Kurban Bayramı (2. gün)", "Kurban Bayramı (3. gün)",
        "Kurban Bayramı (4. gün)", "Hicri Yılbaşı", "Aşure Günü"
    };

    QString hicriGunVeAy = m_hicriGun + " " + m_hicriAy;
    int idx = diniGunler.indexOf(hicriGunVeAy);
    if (idx >= 0) {
        QString diniGun = karsiliklar[idx];
        m_klasikDiniGunLabel->setText("<font color='#0174DF' size='3'>" + diniGun + "</font>");
        m_modernDiniGunLabel->setText("✦ " + diniGun);
        if (!m_diniGunMesajiGosterildi && m_trayIcon) {
            m_trayIcon->showMessage("Dini Günler ve Geceler", "\n" + hicriGunVeAy + " - " + diniGun, QSystemTrayIcon::Information, 20000);
            m_diniGunMesajiGosterildi = true;
        }
    } else {
        m_klasikDiniGunLabel->clear();
        m_modernDiniGunLabel->clear();
    }
}

void MainWindow::onSaatGuncelle() {
    m_saat = QTime::currentTime().toString("hh:mm:ss");
    m_klasikSaatLabel->setText("Saat: " + m_saat);
    m_modernSaatLabel->setText(m_saat);

    QDate curDate = QDate::currentDate();
    QLocale loc(Localization::currentLanguage());
    m_miladiTarih = loc.toString(curDate, "d MMMM yyyy");
    m_klasikMiladiLabel->setText(m_miladiTarih);
    m_modernMiladiLabel->setText(m_miladiTarih);

    gorselVerileriGuncelle();

    if (m_simdikiTarih != m_miladiTarih) {
        m_simdikiTarih = m_miladiTarih;
        hicriTarihiGuncelle();
        vakitleriAl();
    }
}

void MainWindow::onLabelGuncelle() {
    QString saatDakika = QTime::currentTime().toString("hh:mm");
    m_saat = QTime::currentTime().toString("hh:mm:ss");

    if (m_vakitler->imsak == "--:--") return;

    statusBar()->showMessage(Localization::tr("kaynak"));

    QString imsak = m_vakitler->imsak, gunes = m_vakitler->gunes, ogle = m_vakitler->ogle,
            ikindi = m_vakitler->ikindi, aksam = m_vakitler->aksam, yatsi = m_vakitler->yatsi;

    QString kImsak = imsak, kGunes = gunes, kOgle = ogle, kIkindi = ikindi, kAksam = aksam, kYatsi = yatsi;
    QString namazvakti, sonrakivakit, simdikivakit;

    if (saatDakika >= "00:00" && saatDakika < imsak) {
        kYatsi = "<font color='#FF0000'>" + yatsi + "</font>";
        namazvakti = Localization::formatCurrentPrayer("yatsi");
        sonrakivakit = Localization::formatRemainingTime("imsak");
        simdikivakit = "yatsı";
    } else if (saatDakika >= imsak && saatDakika < gunes) {
        kImsak = "<font color='#FF0000'>" + imsak + "</font>";
        namazvakti = Localization::formatCurrentPrayer("imsak");
        sonrakivakit = Localization::formatRemainingTime("gunes");
        simdikivakit = "sabah";
        diniGunleriHatirlat();
        vakitKontrol(Localization::tr("namaz_vakitleri_tray"), m_sesEdits["sabah"]->text(), kImsak, "sabah");
    } else if (saatDakika >= gunes && saatDakika < ogle) {
        kGunes = "<font color='#FF0000'>" + gunes + "</font>";
        namazvakti = Localization::formatCurrentPrayer("gunes");
        sonrakivakit = Localization::formatRemainingTime("ogle");
        simdikivakit = "güneş";
    } else if (saatDakika >= ogle && saatDakika < ikindi) {
        kOgle = "<font color='#FF0000'>" + ogle + "</font>";
        namazvakti = Localization::formatCurrentPrayer("ogle");
        sonrakivakit = Localization::formatRemainingTime("ikindi");
        simdikivakit = "öğle";
        diniGunleriHatirlat();
        vakitKontrol(Localization::tr("namaz_vakitleri_tray"), m_sesEdits["ogle"]->text(), kOgle, "öğle");
    } else if (saatDakika >= ikindi && saatDakika < aksam) {
        kIkindi = "<font color='#FF0000'>" + ikindi + "</font>";
        namazvakti = Localization::formatCurrentPrayer("ikindi");
        sonrakivakit = Localization::formatRemainingTime("aksam");
        simdikivakit = "ikindi";
        diniGunleriHatirlat();
        vakitKontrol(Localization::tr("namaz_vakitleri_tray"), m_sesEdits["ikindi"]->text(), kIkindi, "ikindi");
    } else if (saatDakika >= aksam && saatDakika < yatsi) {
        kAksam = "<font color='#FF0000'>" + aksam + "</font>";
        namazvakti = Localization::formatCurrentPrayer("aksam");
        sonrakivakit = Localization::formatRemainingTime("yatsi");
        simdikivakit = "akşam";
        diniGunleriHatirlat();
        vakitKontrol(Localization::tr("namaz_vakitleri_tray"), m_sesEdits["aksam"]->text(), kAksam, "akşam");
    } else {
        kYatsi = "<font color='#FF0000'>" + yatsi + "</font>";
        namazvakti = Localization::formatCurrentPrayer("yatsi");
        sonrakivakit = Localization::formatRemainingTime("imsak");
        simdikivakit = "yatsı";
        diniGunleriHatirlat();
        vakitKontrol(Localization::tr("namaz_vakitleri_tray"), m_sesEdits["yatsi"]->text(), kYatsi, "yatsı");
    }

    m_klasikVakitLabels["imsak"]->setText(kImsak);
    m_klasikVakitLabels["gunes"]->setText(kGunes);
    m_klasikVakitLabels["ogle"]->setText(kOgle);
    m_klasikVakitLabels["ikindi"]->setText(kIkindi);
    m_klasikVakitLabels["aksam"]->setText(kAksam);
    m_klasikVakitLabels["yatsi"]->setText(kYatsi);
    m_klasikDurumLabel->setText(namazvakti);
    m_namazVaktiStr = namazvakti;
    m_klasikSonrakiVakitLabel->setText(sonrakivakit);
    m_sonrakiVakit = sonrakivakit;

    m_modernVakitDurumLabel->setText(namazvakti);
    QString temizSonraki = sonrakivakit;
    temizSonraki.remove(" :");
    m_modernKalanBaslikLabel->setText(temizSonraki);

    static const QMap<QString, QString> vakitEsleme = {
        {"sabah", "imsak"}, {"güneş", "gunes"}, {"öğle", "ogle"},
        {"ikindi", "ikindi"}, {"akşam", "aksam"}, {"yatsı", "yatsi"}
    };
    kalanSureKontrol(simdikivakit);
    modernVakitleriGuncelle(vakitEsleme.value(simdikivakit, "imsak"));
}

void MainWindow::kalanSureKontrol(const QString& simdikivakit) {
    QTime simdi = QTime::currentTime();
    int simdikiSaat = simdi.hour();
    int simdikiDakika = simdi.minute();

    QString hedefStr;
    if (simdikivakit == "sabah") hedefStr = m_vakitler->gunes;
    else if (simdikivakit == "güneş") hedefStr = m_vakitler->ogle;
    else if (simdikivakit == "öğle") hedefStr = m_vakitler->ikindi;
    else if (simdikivakit == "ikindi") hedefStr = m_vakitler->aksam;
    else if (simdikivakit == "akşam") hedefStr = m_vakitler->yatsi;
    else hedefStr = m_vakitler->imsak;

    QStringList parts = hedefStr.split(":");
    if (parts.size() < 2) return;
    int saat = parts[0].toInt();
    int dakika = parts[1].toInt();

    int toplamDakika = 0;
    if (simdikivakit == "yatsı" && !(saat >= simdikiSaat && simdikiSaat >= 0)) {
        toplamDakika = (24 * 60) - (simdikiSaat * 60 + simdikiDakika) + (saat * 60 + dakika);
    } else {
        toplamDakika = (saat * 60 + dakika) - (simdikiSaat * 60 + simdikiDakika);
    }

    erkenUyariKontrol(toplamDakika);
    if (simdikivakit == "güneş" && m_sesChecks["sela"]->isChecked() && m_chkErkenSela->isChecked()) {
        selaOkuKontrol(toplamDakika);
    }

    QTime hedefZaman(saat, dakika, 0);
    int secs = simdi.secsTo(hedefZaman);
    if (secs < 0) secs += 86400;
    int h = secs / 3600;
    int m = (secs % 3600) / 60;
    int s = secs % 60;

    QString kalanKlasik = QString("%1:%2:%3").arg(h).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
    m_klasikKalanSureLabel->setText(kalanKlasik);

    m_kalanSure = QString("%1:%2:%3").arg(h, 2, 10, QChar('0')).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
    m_modernKalanSureLabel->setText(m_kalanSure);
}

void MainWindow::modernVakitleriGuncelle(const QString& aktifAnahtar) {
    m_aktifVakit = aktifAnahtar;
    gorselVerileriGuncelle();

    auto degerler = m_vakitler->asMap();
    for (const QString& anahtar : m_modernSatirFrames.keys()) {
        m_modernSaatLabels[anahtar]->setText(degerler.value(anahtar, "--:--"));
        bool aktifMi = (anahtar == aktifAnahtar);
        QString aktifStr = aktifMi ? "true" : "false";

        QFrame* frame = m_modernSatirFrames[anahtar];
        QLabel* adLbl = m_modernAdLabels[anahtar];
        QLabel* stLbl = m_modernSaatLabels[anahtar];
        QLabel* badge = m_modernBadgeLabels[anahtar];

        if (frame->property("aktif").toString() != aktifStr) {
            frame->setProperty("aktif", aktifStr);
            adLbl->setProperty("aktif", aktifStr);
            stLbl->setProperty("aktif", aktifStr);
            badge->setVisible(aktifMi);

            for (QWidget* w : {static_cast<QWidget*>(frame), static_cast<QWidget*>(adLbl), static_cast<QWidget*>(stLbl)}) {
                w->style()->unpolish(w);
                w->style()->polish(w);
                w->update();
            }
        }
    }
}

void MainWindow::gorselVerileriGuncelle() {
    VakitVeriPaketi veri;
    veri.ulke = m_ulke;
    veri.sehir = m_sehir;
    veri.seciliYer = m_seciliYer;
    veri.miladi = m_miladiTarih;
    veri.hicri = m_hicriTarih;
    veri.diniGun = m_klasikDiniGunLabel ? m_klasikDiniGunLabel->text() : "";
    veri.saat = QTime::currentTime().toString("hh:mm:ss");
    veri.aktifVakit = m_aktifVakit;
    veri.sonrakiVakit = m_sonrakiVakit;
    veri.kalanSure = m_kalanSure;
    veri.vakitler = m_vakitler->asMap();
    veri.tema = m_aktifTema;

    if (m_vakitSaatiWidget) m_vakitSaatiWidget->veriGuncelle(veri);
    if (m_gokyuzuWidget) m_gokyuzuWidget->veriGuncelle(veri);
    if (m_havaWidget) m_havaWidget->temaAyarla(m_aktifTema);
}

void MainWindow::erkenUyariKontrol(int toplamDakika) {
    if (toplamDakika == m_spnErkenUyari->value() && m_chkErkenUyari->isChecked() && !m_mesajGosterildi) {
        m_trayIcon->showMessage(Localization::tr("erken_uyari"),
                                m_klasikSonrakiVakitLabel->text() + " " + QString::number(toplamDakika) + " " + Localization::tr("dakika"),
                                QSystemTrayIcon::Information, 20000);
        m_mesajGosterildi = true;
        if (m_sesChecks["uyari"]->isChecked()) {
            sesDosyasiniAc(m_sesEdits["uyari"]->text());
        }
        if (!m_mesajTimer) {
            m_mesajTimer = new QTimer(this);
            m_mesajTimer->setSingleShot(true);
            connect(m_mesajTimer, &QTimer::timeout, this, [this]() { m_mesajGosterildi = false; });
        }
        m_mesajTimer->start(120000);
    }
}

void MainWindow::selaOkuKontrol(int toplamDakika) {
    QDate bugun = QDate::currentDate();
    if (bugun.dayOfWeek() == 5) {
        if (toplamDakika == m_spnErkenSela->value() && !m_mesajGosterildi) {
            m_trayIcon->showMessage("Hâyırlı Cumalar", "\nSela okunuyor.", QSystemTrayIcon::Information, 20000);
            m_mesajGosterildi = true;
            sesDosyasiniAc(m_sesEdits["sela"]->text());
            if (!m_mesajTimer) {
                m_mesajTimer = new QTimer(this);
                m_mesajTimer->setSingleShot(true);
                connect(m_mesajTimer, &QTimer::timeout, this, [this]() { m_mesajGosterildi = false; });
            }
            m_mesajTimer->start(120000);
        }
    }
}

void MainWindow::vakitKontrol(const QString& baslik, const QString& dosya, const QString& labelSaat, const QString& simdikivakit) {
    QRegularExpression re(">(.*)<");
    auto match = re.match(labelSaat);
    if (!match.hasMatch()) return;
    QString saat = match.captured(1);

    if (saat == m_saat.left(5) && !m_mesajGosterildi) {
        m_trayIcon->showMessage(baslik, vakitlerMesaj(), QSystemTrayIcon::Information, 20000);
        m_mesajGosterildi = true;

        static const QMap<QString, QString> eslesme = {
            {"sabah", "sabah"}, {"öğle", "ogle"}, {"ikindi", "ikindi"}, {"akşam", "aksam"}, {"yatsı", "yatsi"}
        };
        QString key = eslesme.value(simdikivakit);
        if (!key.isEmpty() && m_sesChecks.contains(key) && m_sesChecks[key]->isChecked()) {
            ezanOku(dosya);
        }

        if (!m_mesajTimer) {
            m_mesajTimer = new QTimer(this);
            m_mesajTimer->setSingleShot(true);
            connect(m_mesajTimer, &QTimer::timeout, this, [this]() { m_mesajGosterildi = false; });
        }
        m_mesajTimer->start(120000);
    }
}

void MainWindow::sesDosyasiniAc(const QString& dosya) {
    if (dosya.isEmpty() || !QFile::exists(dosya)) return;
    m_player->stop();
    m_player->setMedia(QUrl::fromLocalFile(dosya));
    m_player->setVolume(m_sesSlider->value());
    m_player->play();
}

void MainWindow::ezanOku(const QString& dosya) {
    sesDosyasiniAc(dosya);
}

QString MainWindow::vakitlerMesaj() const {
    QString yer = !m_cmbIlce->currentText().isEmpty() ? m_cmbIlce->currentText() : m_cmbSehir->currentText();
    QString durum = m_namazVaktiStr;
    QString header = Localization::tr("vakitler_icin_fmt").arg(yer, durum);
    return QString("%1\n%2: %3\t%4: %5\n%6: %7\t%8: %9\n%10: %11\t%12: %13")
        .arg(header,
             Localization::tr("imsak"), m_vakitler->imsak,
             Localization::tr("gunes"), m_vakitler->gunes,
             Localization::tr("ogle"), m_vakitler->ogle,
             Localization::tr("ikindi"), m_vakitler->ikindi,
             Localization::tr("aksam"), m_vakitler->aksam,
             Localization::tr("yatsi"), m_vakitler->yatsi);
}

void MainWindow::vakitleriGoster() {
    m_trayIcon->showMessage(Localization::tr("namaz_vakitleri_tray"), vakitlerMesaj(), QSystemTrayIcon::Information, 20000);
}

void MainWindow::lisansAc() {
    QString lisansDosyasi = Localization::getLicensePath(m_programinKonumu);
    if (!QFile::exists(lisansDosyasi)) {
        QString ustDizin = QFileInfo(m_programinKonumu).dir().absolutePath();
        QString altLisans = Localization::getLicensePath(ustDizin);
        if (QFile::exists(altLisans)) {
            lisansDosyasi = altLisans;
        }
    }
    if (QFile::exists(lisansDosyasi)) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(lisansDosyasi));
    }
}

void MainWindow::onDilDegisti(int index) {
    if (index < 0 || index >= Localization::supportedLanguages().size()) return;
    QString seciliDil = Localization::supportedLanguages().at(index);
    if (seciliDil != Localization::currentLanguage()) {
        Localization::setLanguage(seciliDil);
        m_settings->setValue("Ayarlar/dil", seciliDil);
        m_settings->sync();
        retranslateUi();
    }
}

void MainWindow::retranslateUi() {
    setWindowTitle(QString("PyVakitci 1.8 - %1").arg(Localization::tr("tab_saat")));

    if (m_tabWidget) {
        m_tabWidget->setTabText(0, "");
        m_tabWidget->setTabText(1, "");
        m_tabWidget->setTabText(2, "");
        m_tabWidget->setTabText(3, "");
        m_tabWidget->setTabText(4, "");

        m_tabWidget->setTabToolTip(0, Localization::tr("tab_saat"));
        m_tabWidget->setTabToolTip(1, Localization::tr("tab_hava"));
        m_tabWidget->setTabToolTip(2, Localization::tr("tab_ayarlar"));
        m_tabWidget->setTabToolTip(3, Localization::tr("tab_ses"));
        m_tabWidget->setTabToolTip(4, Localization::tr("tab_linkler"));
    }

    // Menüler
    if (m_menuDosya) m_menuDosya->setTitle(Localization::tr("menu_dosya"));
    if (m_menuGorunum) m_menuGorunum->setTitle(Localization::tr("menu_gorunum"));
    if (m_menuTema) m_menuTema->setTitle(Localization::tr("menu_renk_temasi"));
    if (m_menuStil) m_menuStil->setTitle(Localization::tr("menu_pencere_stili"));
    if (m_menuYardim) m_menuYardim->setTitle(Localization::tr("menu_yardim"));

    // Menü Actions
    if (m_actCikis) m_actCikis->setText(Localization::tr("cikis"));
    if (m_actLisans) m_actLisans->setText(Localization::tr("lisans"));
    if (m_actHakkinda) m_actHakkinda->setText(Localization::tr("hakkinda"));
    if (m_actQt) m_actQt->setText(Localization::tr("menu_qt_hakkinda"));

    // Arayüz Modları Actions
    if (m_arayuzActions.contains("saat") && m_arayuzActions["saat"]) m_arayuzActions["saat"]->setText(Localization::tr("mod_saat"));
    if (m_arayuzActions.contains("gokyuzu") && m_arayuzActions["gokyuzu"]) m_arayuzActions["gokyuzu"]->setText(Localization::tr("mod_gokyuzu"));
    if (m_arayuzActions.contains("kartlar") && m_arayuzActions["kartlar"]) m_arayuzActions["kartlar"]->setText(Localization::tr("mod_kartlar"));
    if (m_arayuzActions.contains("klasik") && m_arayuzActions["klasik"]) m_arayuzActions["klasik"]->setText(Localization::tr("mod_klasik"));

    // Renk Temaları Actions
    for (auto it = m_temaActions.begin(); it != m_temaActions.end(); ++it) {
        if (it.value()) {
            it.value()->setText(Localization::tr("tema_" + it.key()));
        }
    }

    // Pencere Stili Actions
    if (m_stilActions.contains("Fusion") && m_stilActions["Fusion"]) m_stilActions["Fusion"]->setText(Localization::tr("stil_fusion"));
    if (m_stilActions.contains("windowsvista") && m_stilActions["windowsvista"]) m_stilActions["windowsvista"]->setText(Localization::tr("stil_vista"));
    if (m_stilActions.contains("Windows") && m_stilActions["Windows"]) m_stilActions["Windows"]->setText(Localization::tr("stil_windows"));

    // Sistem Tepsisi
    if (m_actVakitleriGoster) m_actVakitleriGoster->setText(Localization::tr("vakitleri_goster"));

    // Ayarlar Sekmesi Butonları
    if (m_btnGenelAyarlar) m_btnGenelAyarlar->setText("⚙ " + Localization::tr("genel_ayarlar"));
    if (m_btnBolgeselAyarlar) m_btnBolgeselAyarlar->setText("📍 " + Localization::tr("bolgesel_ayarlar"));

    // Genel Ayarlar
    if (m_chkOtoCalistir) m_chkOtoCalistir->setText(Localization::tr("oto_calistir"));
    if (m_chkIkonGoster) m_chkIkonGoster->setText(Localization::tr("sistem_tepsisi_ikon"));
    if (m_chkSistemTepsisindeBaslat) m_chkSistemTepsisindeBaslat->setText(Localization::tr("sistem_tepsisi"));
    if (m_chkOtoGuncelle) m_chkOtoGuncelle->setText(Localization::tr("oto_guncelle"));
    if (m_chkHerZamanUstte) m_chkHerZamanUstte->setText(Localization::tr("her_zaman_ustte"));
    if (m_chkBesmele) m_chkBesmele->setText(Localization::tr("besmele_ile_basla"));
    if (m_chkSpotify) m_chkSpotify->setText(Localization::tr("spotify_duraklat"));
    if (m_chkWinamp) m_chkWinamp->setText(Localization::tr("winamp_duraklat"));

    if (m_chkErkenUyari) m_chkErkenUyari->setText(Localization::tr("erken_uyari_metin"));
    if (m_chkErkenSela) m_chkErkenSela->setText(Localization::tr("erken_sela_metin"));
    if (m_chkHicriGun) m_chkHicriGun->setText(Localization::tr("hicri_gun_metin"));

    if (m_btnGoster) m_btnGoster->setText(Localization::tr("vakitleri_aylik_goster"));
    if (m_btnGuncelle) m_btnGuncelle->setText(Localization::tr("vakitleri_guncelle"));
    if (m_btnVarsayilan) m_btnVarsayilan->setText(Localization::tr("btn_varsayilan"));
    if (m_btnKaydet) m_btnKaydet->setText(Localization::tr("btn_kaydet"));

    // Bölgesel Ayarlar
    if (m_lblBolgeselUlke) m_lblBolgeselUlke->setText(Localization::tr("ulke"));
    if (m_lblBolgeselSehir) m_lblBolgeselSehir->setText(Localization::tr("sehir"));
    if (m_lblBolgeselIlce) m_lblBolgeselIlce->setText(Localization::tr("ilce"));
    if (m_lblBolgeselDil) m_lblBolgeselDil->setText(Localization::tr("dil"));
    if (m_btnBolgeVarsayilan) m_btnBolgeVarsayilan->setText(Localization::tr("btn_varsayilan"));
    if (m_btnBolgeKaydet) m_btnBolgeKaydet->setText(Localization::tr("btn_kaydet"));

    // Ses Ayarları
    static const QStringList sesKeys = {"sabah", "ogle", "ikindi", "aksam", "yatsi", "dua", "sela", "uyari"};
    for (const QString& sk : sesKeys) {
        if (m_sesLabels.contains(sk) && m_sesLabels[sk]) {
            m_sesLabels[sk]->setText(Localization::tr(sk + "_etiket"));
        }
        if (m_sesEkleBtns.contains(sk) && m_sesEkleBtns[sk]) {
            m_sesEkleBtns[sk]->setText(Localization::tr("btn_ekle"));
        }
        if (m_sesPlayBtns.contains(sk) && m_sesPlayBtns[sk]) {
            m_sesPlayBtns[sk]->setToolTip(Localization::tr("btn_oynat"));
        }
    }
    if (m_btnSesVarsayilan) m_btnSesVarsayilan->setText(Localization::tr("btn_varsayilan"));
    if (m_btnSesKaydet) m_btnSesKaydet->setText(Localization::tr("btn_kaydet"));

    // Linkler Sekmesi
    if (m_linklerBox) m_linklerBox->setTitle(Localization::tr("yararli_siteler"));

    // Vakit İsimleri
    static const QStringList vakitKeys = {"imsak", "gunes", "ogle", "ikindi", "aksam", "yatsi"};
    for (const QString& k : vakitKeys) {
        if (m_modernAdLabels.contains(k) && m_modernAdLabels[k]) {
            m_modernAdLabels[k]->setText(Localization::tr(k));
        }
        if (m_modernBadgeLabels.contains(k) && m_modernBadgeLabels[k]) {
            m_modernBadgeLabels[k]->setText(Localization::tr("vakit_aktif"));
        }
        if (m_klasikVakitAdLabels.contains(k) && m_klasikVakitAdLabels[k]) {
            m_klasikVakitAdLabels[k]->setText(Localization::tr(k));
        }
    }

    // Statusbar
    if (statusBar()) statusBar()->showMessage(Localization::tr("kaynak"));

    // Hava durumu widget'ını tetikle
    if (m_havaWidget) {
        m_havaWidget->retranslateUi();
    }

    hicriTarihiGuncelle();
    onSaatGuncelle();
    onLabelGuncelle();
}

void MainWindow::hakkindaDialog() {
    HakkindaDialog dlg(this);
    dlg.exec();
}

void MainWindow::arayuzModunuUygula(const QString& mod, bool kaydet) {
    QString m = mod;
    if (m == "modern") m = "kartlar";
    if (m != "saat" && m != "gokyuzu" && m != "kartlar" && m != "klasik") m = "saat";
    m_arayuzModu = m;

    m_bilgilerGroupBox->hide();
    m_namazVakitleriGroupBox->hide();
    m_modernPanel->hide();
    m_vakitSaatiWidget->hide();
    m_gokyuzuWidget->hide();

    if (m == "saat") {
        m_vakitSaatiWidget->show();
    } else if (m == "gokyuzu") {
        m_gokyuzuWidget->show();
    } else if (m == "kartlar") {
        m_modernPanel->show();
    } else {
        m_bilgilerGroupBox->show();
        m_namazVakitleriGroupBox->show();
    }

    if (m_arayuzActions.contains(m)) {
        m_arayuzActions[m]->setChecked(true);
    }

    gorselVerileriGuncelle();
    if (kaydet && m_settings) {
        m_settings->setValue("Ayarlar/arayuz_modu", m_arayuzModu);
    }
}

void MainWindow::temayiUygula(const QString& temaKey, bool kaydet) {
    QString key = Temalar::getTemalar().contains(temaKey) ? temaKey : "varsayilan";
    m_aktifTema = key;
    QString qss = Temalar::getTemaQss(key);
    qApp->setStyleSheet(qss);

    if (key == "varsayilan") linkRenkleriniAyarla("#0000ff");
    else linkRenkleriniAyarla("#ffffff");

    if (m_temaActions.contains(key)) {
        m_temaActions[key]->setChecked(true);
    }

    if (kaydet && m_settings) {
        m_settings->setValue("Ayarlar/tema", m_aktifTema);
    }

    gorselVerileriGuncelle();
}

void MainWindow::pencereStiliniUygula(const QString& stilAdi, bool kaydet) {
    m_gorunum = stilAdi;
    QStyle* st = QStyleFactory::create(m_gorunum);
    if (st) {
        qApp->setStyle(st);
    }
    if (m_stilActions.contains(m_gorunum)) {
        m_stilActions[m_gorunum]->setChecked(true);
    }
    if (Temalar::getTemalar().contains(m_aktifTema)) {
        qApp->setStyleSheet(Temalar::getTemaQss(m_aktifTema));
    }
    if (kaydet && m_settings) {
        m_settings->setValue("Ayarlar/gorunum", m_gorunum);
    }
}

void MainWindow::linkRenkleriniAyarla(const QString& renk) {
    for (QLabel* lbl : m_linkLabels) {
        QString txt = lbl->text();
        txt.replace(QRegularExpression("color:\\s*#[0-9a-fA-F]+"), "color:" + renk);
        lbl->setText(txt);
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (m_chkIkonGoster->isChecked() && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        programiKapat();
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        programiKapat();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::programiKapat() {
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
    qApp->quit();
}

void MainWindow::testEkranGoruntuleriAl(const QString& outDir) {
    QDir().mkpath(outDir);
    QTimer::singleShot(2500, this, [this, outDir]() {
        // 1. Vakit Saati (Varsayılan - İstanbul / İstanbul)
        m_tabWidget->setCurrentIndex(0);
        arayuzModunuUygula("saat", false);
        temayiUygula("varsayilan", false);
        qApp->processEvents();
        this->grab().save(outDir + "/cpp_1_saat_varsayilan.png");

        // 2. Bölgesel Ayarlar Sekmesi (İstanbul / İstanbul)
        m_tabWidget->setCurrentIndex(2);
        onAyarAltSekmeDegisti(1);
        qApp->processEvents();
        this->grab().save(outDir + "/cpp_8_ayarlar_bolgesel.png");

        // 3. Ses Ayarları Sekmesi (Uyarı Sesi tiki kontrolü)
        m_tabWidget->setCurrentIndex(3);
        qApp->processEvents();
        this->grab().save(outDir + "/cpp_9_ses_ayarlari.png");

        // --- İNGİLİZCE TESTLERİ ---
        onDilDegisti(1);
        qApp->processEvents();

        // English Saat
        m_tabWidget->setCurrentIndex(0);
        arayuzModunuUygula("saat", false);
        temayiUygula("varsayilan", false);
        qApp->processEvents();
        this->grab().save(outDir + "/cpp_en_saat_varsayilan.png");

        // English Sade
        arayuzModunuUygula("klasik", false);
        qApp->processEvents();
        this->grab().save(outDir + "/cpp_en_sade_varsayilan.png");

        // English Modern Koyu
        arayuzModunuUygula("kartlar", false);
        temayiUygula("koyu", false);
        qApp->processEvents();
        this->grab().save(outDir + "/cpp_en_modern_koyu.png");

        // English Hava Durumu
        temayiUygula("varsayilan", false);
        m_tabWidget->setCurrentIndex(1);
        qApp->processEvents();
        this->grab().save(outDir + "/cpp_en_hava_varsayilan.png");

        // English Ses Ayarları
        m_tabWidget->setCurrentIndex(3);
        qApp->processEvents();
        this->grab().save(outDir + "/cpp_en_ses_varsayilan.png");

        // --- RUSÇA TESTİ ---
        onDilDegisti(5);
        arayuzModunuUygula("kartlar", false);
        temayiUygula("koyu", false);
        qApp->processEvents();
        this->grab().save(outDir + "/cpp_ru_modern_koyu.png");

        // Türkçe'ye dön ve kapat
        onDilDegisti(0);
        onAyarAltSekmeDegisti(0);
        arayuzModunuUygula("saat", false);
        temayiUygula("varsayilan", false);
        m_tabWidget->setCurrentIndex(0);
        programiKapat();
    });
}
