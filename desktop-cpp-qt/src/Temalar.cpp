#include "Temalar.h"

QStringList Temalar::getTemaAnahtarlari() {
    return {"varsayilan", "koyu", "zumrut", "gece_mavisi", "kehribar"};
}

QString Temalar::getOrtakQss() {
    return R"(
    QLabel#modern_ulke_label {
        font-size: 10px;
    }
    QLabel#modern_sehir_label {
        font-size: 15px;
        font-weight: bold;
    }
    QLabel#modern_saat_label {
        font-size: 12px;
        font-weight: bold;
    }
    QLabel#modern_miladi_label {
        font-size: 11px;
    }
    QLabel#modern_hicri_label {
        font-size: 11px;
    }
    QLabel#modern_dini_gun_label {
        font-size: 11px;
        font-weight: bold;
    }
    QLabel#modern_vakit_durum_label {
        font-size: 13px;
        font-weight: bold;
    }
    QLabel#modern_kalan_baslik_label {
        font-size: 11px;
    }
    QLabel#modern_kalan_sure_label {
        font-size: 20px;
        font-weight: bold;
    }
    QLabel[vakitAd="true"] {
        font-size: 13px;
    }
    QLabel[vakitSaat="true"] {
        font-size: 14px;
        font-weight: bold;
    }
    QLabel[vakitBadge="true"] {
        font-size: 10px;
        font-weight: bold;
    }
    QPushButton[ayarAltSekme="true"] {
        font-size: 11px;
        font-weight: bold;
        border-radius: 6px;
        padding: 4px 8px;
        border: 1px solid rgba(148, 163, 184, 90);
    }
    QPushButton[ayarAltSekme="true"]:checked {
        background-color: #2563eb;
        color: #ffffff;
        border: 1px solid #1d4ed8;
    }
    QGroupBox#ayarlar_groupBox, QGroupBox#bolgesel_ayarlar_groupBox {
        margin-top: 0px;
    }
)";
}

const QMap<QString, TemaVeri>& Temalar::getTemalar() {
    static QMap<QString, TemaVeri> temalar;
    if (!temalar.isEmpty()) return temalar;

    temalar["varsayilan"] = {
        "Varsayılan (Açık)",
        R"(
            QTabWidget::tab-bar {
                alignment: center;
            }
            QTabBar {
                alignment: center;
            }
            QTabBar QToolButton {
                width: 0px;
                height: 0px;
                border: none;
                background: transparent;
            }
            QTabBar::scroller {
                width: 0px;
                height: 0px;
            }
            QMenuBar {
                background-color: #f8fafc;
                color: #1e293b;
                border-bottom: 1px solid #e2e8f0;
                padding: 2px 4px;
            }
            QMenuBar::item {
                padding: 4px 10px;
                border-radius: 4px;
                background: transparent;
            }
            QMenuBar::item:selected {
                background-color: #e2e8f0;
                color: #0f172a;
            }
            QMenu {
                background-color: #ffffff;
                color: #1e293b;
                border: 1px solid #cbd5e1;
                padding: 4px;
            }
            QMenu::item {
                padding: 5px 24px 5px 24px;
                border-radius: 4px;
            }
            QMenu::item:selected {
                background-color: #2563eb;
                color: #ffffff;
            }
            QMenu::separator {
                height: 1px;
                background: #e2e8f0;
                margin: 4px 6px;
            }
            /* Modern Görünüm Kartları - Varsayılan Açık Tema */
            QFrame#modern_ust_kart {
                background-color: #ffffff;
                border: 1px solid #dbe3ee;
                border-radius: 10px;
            }
            QLabel#modern_sehir_label {
                color: #0f172a;
                font-weight: bold;
            }
            QLabel#modern_ulke_label {
                color: #64748b;
                font-weight: bold;
            }
            QLabel#modern_saat_label {
                background-color: #eff6ff;
                color: #1d4ed8;
                border: 1px solid #bfdbfe;
                border-radius: 5px;
                padding: 1px 6px;
                font-weight: bold;
            }
            QLabel#modern_miladi_label {
                color: #334155;
            }
            QLabel#modern_hicri_label {
                color: #64748b;
            }
            QFrame#modern_sayac_kart {
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #eff6ff, stop:1 #e0f2fe);
                border: 1px solid #bae6fd;
                border-radius: 10px;
            }
            QLabel#modern_vakit_durum_label {
                color: #0369a1;
                font-weight: bold;
            }
            QLabel#modern_kalan_baslik_label {
                color: #475569;
            }
            QLabel#modern_kalan_sure_label {
                color: #0f172a;
                font-weight: bold;
            }
            QLabel#modern_dini_gun_label {
                color: #0284c7;
                font-weight: bold;
            }
            QFrame[vakitSatiri="true"] {
                background-color: #ffffff;
                border: 1px solid #e2e8f0;
                border-radius: 7px;
            }
            QFrame[vakitSatiri="true"][aktif="true"] {
                background-color: #eff6ff;
                border: 1.5px solid #2563eb;
                border-radius: 7px;
            }
            QLabel[vakitAd="true"] {
                color: #334155;
                font-weight: 500;
            }
            QLabel[vakitAd="true"][aktif="true"] {
                color: #1d4ed8;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"] {
                color: #0f172a;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"][aktif="true"] {
                color: #dc2626;
                font-weight: bold;
            }
            QLabel[vakitBadge="true"] {
                background-color: #2563eb;
                color: #ffffff;
                border-radius: 4px;
                padding: 1px 5px;
                font-weight: bold;
            }
        )" + getOrtakQss()
    };

    temalar["koyu"] = {
        "Modern Koyu (Dark)",
        R"(
            QMainWindow, QWidget#centralwidget {
                background-color: #1e1e24;
                color: #f3f4f6;
            }
            QTabWidget::pane {
                border: 1px solid #374151;
                background-color: #1e1e24;
            }
            QTabWidget::tab-bar {
                alignment: center;
            }
            QTabBar {
                alignment: center;
            }
            QTabBar::tab {
                background-color: #282832;
                color: #9ca3af;
                border: 1px solid #374151;
                border-top: none;
                border-bottom-left-radius: 6px;
                border-bottom-right-radius: 6px;
                padding: 1px;
                margin: 0px 2px;
                width: 50px;
                height: 44px;
            }
            QTabBar::tab:selected {
                background-color: #32323f;
                color: #ffffff;
                border-top: 3px solid #3b82f6;
            }
            QTabBar::tab:hover:!selected {
                background-color: #2e2e3a;
                color: #e5e7eb;
            }
            QTabBar QToolButton {
                width: 0px;
                height: 0px;
                border: none;
                background: transparent;
            }
            QTabBar::scroller {
                width: 0px;
                height: 0px;
            }
            QGroupBox {
                background-color: #25252e;
                border: 1px solid #374151;
                border-radius: 8px;
                margin-top: 10px;
                font-weight: bold;
                color: #60a5fa;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0 6px;
                background-color: transparent;
            }
            QLabel {
                color: #e5e7eb;
            }
            QLabel#namaz_vakti_label {
                color: #60a5fa;
                font-weight: bold;
            }
            QLabel#sonraki_vakit_label, QLabel#kalan_sure_label {
                color: #93c5fd;
                font-weight: bold;
            }
            QLabel a, QGroupBox#linkler_groupBox QLabel, QGroupBox#linkler_groupBox QLabel a, QGroupBox#linkler_groupBox QLabel span {
                color: #ffffff;
            }
            QLineEdit {
                background-color: #2b2b36;
                color: #ffffff;
                border: 1px solid #4b5563;
                border-radius: 4px;
                padding: 3px 6px;
                selection-background-color: #3b82f6;
            }
            QLineEdit:focus {
                border: 1px solid #60a5fa;
            }
            QComboBox {
                background-color: #2b2b36;
                color: #ffffff;
                border: 1px solid #4b5563;
                border-radius: 4px;
                padding: 3px 6px;
            }
            QComboBox:focus {
                border: 1px solid #60a5fa;
            }
            QComboBox QAbstractItemView {
                background-color: #25252e;
                color: #ffffff;
                selection-background-color: #3b82f6;
                border: 1px solid #4b5563;
            }
            QPushButton, QToolButton {
                background-color: #32323f;
                color: #f9fafb;
                border: 1px solid #4b5563;
                border-radius: 5px;
                padding: 4px 10px;
                font-weight: 500;
            }
            QPushButton:hover, QToolButton:hover {
                background-color: #3f3f4f;
                border-color: #60a5fa;
            }
            QPushButton:pressed, QToolButton:pressed {
                background-color: #25252e;
            }
            QPushButton#pushButton_android, QPushButton#pushButton_ios {
                border: none;
                background-color: transparent;
                padding: 0px;
            }
            QCheckBox {
                color: #e5e7eb;
            }
            QCheckBox::indicator {
                width: 15px;
                height: 15px;
                border: 1px solid #4b5563;
                border-radius: 3px;
                background-color: #2b2b36;
            }
            QCheckBox::indicator:checked {
                background-color: #3b82f6;
                border-color: #60a5fa;
            }
            QSlider::groove:horizontal {
                height: 4px;
                background: #4b5563;
                border-radius: 2px;
            }
            QSlider::sub-page:horizontal {
                background: #3b82f6;
                border-radius: 2px;
            }
            QSlider::handle:horizontal {
                background: #93c5fd;
                border: 1px solid #3b82f6;
                width: 14px;
                margin-top: -5px;
                margin-bottom: -5px;
                border-radius: 7px;
            }
            QMenuBar {
                background-color: #18181f;
                color: #e5e7eb;
                border-bottom: 1px solid #2d2d3a;
                padding: 2px 4px;
            }
            QMenuBar::item {
                padding: 4px 10px;
                border-radius: 4px;
                background: transparent;
            }
            QMenuBar::item:selected {
                background-color: #2d2d3a;
            }
            QMenu {
                background-color: #25252e;
                color: #e5e7eb;
                border: 1px solid #3f3f4e;
                padding: 4px;
            }
            QMenu::item {
                padding: 5px 24px 5px 24px;
                border-radius: 4px;
            }
            QMenu::item:selected {
                background-color: #3b82f6;
                color: #ffffff;
            }
            QMenu::separator {
                height: 1px;
                background: #374151;
                margin: 4px 6px;
            }
            QStatusBar {
                background-color: #18181d;
                color: #9ca3af;
                border-top: 1px solid #282832;
            }
            QFrame#line {
                color: #374151;
            }
            /* Modern Görünüm Kartları - Koyu Tema */
            QFrame#modern_ust_kart {
                background-color: #262630;
                border: 1px solid #383846;
                border-radius: 10px;
            }
            QLabel#modern_sehir_label {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel#modern_ulke_label {
                color: #9ca3af;
                font-weight: bold;
            }
            QLabel#modern_saat_label {
                background-color: #1e293b;
                color: #60a5fa;
                border: 1px solid #3b82f6;
                border-radius: 5px;
                padding: 1px 6px;
                font-weight: bold;
            }
            QLabel#modern_miladi_label {
                color: #e5e7eb;
            }
            QLabel#modern_hicri_label {
                color: #9ca3af;
            }
            QFrame#modern_sayac_kart {
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1e293b, stop:1 #252538);
                border: 1px solid #3b82f6;
                border-radius: 10px;
            }
            QLabel#modern_vakit_durum_label {
                color: #60a5fa;
                font-weight: bold;
            }
            QLabel#modern_kalan_baslik_label {
                color: #9ca3af;
            }
            QLabel#modern_kalan_sure_label {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel#modern_dini_gun_label {
                color: #fbbf24;
                font-weight: bold;
            }
            QFrame[vakitSatiri="true"] {
                background-color: #262630;
                border: 1px solid #343442;
                border-radius: 7px;
            }
            QFrame[vakitSatiri="true"][aktif="true"] {
                background-color: #1e293b;
                border: 1.5px solid #3b82f6;
                border-radius: 7px;
            }
            QLabel[vakitAd="true"] {
                color: #d1d5db;
                font-weight: 500;
            }
            QLabel[vakitAd="true"][aktif="true"] {
                color: #60a5fa;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"] {
                color: #f3f4f6;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"][aktif="true"] {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel[vakitBadge="true"] {
                background-color: #3b82f6;
                color: #ffffff;
                border-radius: 4px;
                padding: 1px 5px;
                font-weight: bold;
            }
        )" + getOrtakQss()
    };

    temalar["zumrut"] = {
        "Zümrüt Yeşili (İslamî)",
        R"(
            QMainWindow, QWidget#centralwidget {
                background-color: #0b1d17;
                color: #e6f4ea;
            }
            QTabWidget::pane {
                border: 1px solid #164e3b;
                background-color: #0b1d17;
            }
            QTabWidget::tab-bar {
                alignment: center;
            }
            QTabBar {
                alignment: center;
            }
            QTabBar::tab {
                background-color: #102d24;
                color: #a7d9c5;
                border: 1px solid #164e3b;
                border-top: none;
                border-bottom-left-radius: 6px;
                border-bottom-right-radius: 6px;
                padding: 1px;
                margin: 0px 2px;
                width: 50px;
                height: 44px;
            }
            QTabBar::tab:selected {
                background-color: #174235;
                color: #ffffff;
                border-top: 3px solid #10b981;
            }
            QTabBar::tab:hover:!selected {
                background-color: #13382d;
                color: #d1fae5;
            }
            QTabBar QToolButton {
                width: 0px;
                height: 0px;
                border: none;
                background: transparent;
            }
            QTabBar::scroller {
                width: 0px;
                height: 0px;
            }
            QGroupBox {
                background-color: #0f271f;
                border: 1px solid #1a5642;
                border-radius: 8px;
                margin-top: 10px;
                font-weight: bold;
                color: #34d399;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0 6px;
                background-color: transparent;
            }
            QLabel {
                color: #ecfdf5;
            }
            QLabel#namaz_vakti_label {
                color: #34d399;
                font-weight: bold;
            }
            QLabel#sonraki_vakit_label, QLabel#kalan_sure_label {
                color: #6ee7b7;
                font-weight: bold;
            }
            QLabel a, QGroupBox#linkler_groupBox QLabel, QGroupBox#linkler_groupBox QLabel a, QGroupBox#linkler_groupBox QLabel span {
                color: #ffffff;
            }
            QLineEdit {
                background-color: #123328;
                color: #ffffff;
                border: 1px solid #1d614a;
                border-radius: 4px;
                padding: 3px 6px;
                selection-background-color: #10b981;
            }
            QLineEdit:focus {
                border: 1px solid #34d399;
            }
            QComboBox {
                background-color: #123328;
                color: #ffffff;
                border: 1px solid #1d614a;
                border-radius: 4px;
                padding: 3px 6px;
            }
            QComboBox:focus {
                border: 1px solid #34d399;
            }
            QComboBox QAbstractItemView {
                background-color: #0f271f;
                color: #ffffff;
                selection-background-color: #10b981;
                border: 1px solid #1d614a;
            }
            QPushButton, QToolButton {
                background-color: #174235;
                color: #f0fdf4;
                border: 1px solid #1d614a;
                border-radius: 5px;
                padding: 4px 10px;
                font-weight: 500;
            }
            QPushButton:hover, QToolButton:hover {
                background-color: #1e5343;
                border-color: #34d399;
            }
            QPushButton:pressed, QToolButton:pressed {
                background-color: #102d24;
            }
            QPushButton#pushButton_android, QPushButton#pushButton_ios {
                border: none;
                background-color: transparent;
                padding: 0px;
            }
            QCheckBox {
                color: #ecfdf5;
            }
            QCheckBox::indicator {
                width: 15px;
                height: 15px;
                border: 1px solid #1d614a;
                border-radius: 3px;
                background-color: #123328;
            }
            QCheckBox::indicator:checked {
                background-color: #10b981;
                border-color: #34d399;
            }
            QSlider::groove:horizontal {
                height: 4px;
                background: #1d614a;
                border-radius: 2px;
            }
            QSlider::sub-page:horizontal {
                background: #10b981;
                border-radius: 2px;
            }
            QSlider::handle:horizontal {
                background: #6ee7b7;
                border: 1px solid #10b981;
                width: 14px;
                margin-top: -5px;
                margin-bottom: -5px;
                border-radius: 7px;
            }
            QMenuBar {
                background-color: #071510;
                color: #ecfdf5;
                border-bottom: 1px solid #13352a;
                padding: 2px 4px;
            }
            QMenuBar::item {
                padding: 4px 10px;
                border-radius: 4px;
                background: transparent;
            }
            QMenuBar::item:selected {
                background-color: #13352a;
            }
            QMenu {
                background-color: #0f271f;
                color: #ecfdf5;
                border: 1px solid #1a5642;
                padding: 4px;
            }
            QMenu::item {
                padding: 5px 24px 5px 24px;
                border-radius: 4px;
            }
            QMenu::item:selected {
                background-color: #10b981;
                color: #ffffff;
            }
            QMenu::separator {
                height: 1px;
                background: #1a5642;
                margin: 4px 6px;
            }
            QStatusBar {
                background-color: #06110d;
                color: #a7d9c5;
                border-top: 1px solid #102d24;
            }
            QFrame#line {
                color: #1a5642;
            }
            /* Modern Görünüm Kartları - Zümrüt Tema */
            QFrame#modern_ust_kart {
                background-color: #102a21;
                border: 1px solid #1c5240;
                border-radius: 10px;
            }
            QLabel#modern_sehir_label {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel#modern_ulke_label {
                color: #6ee7b7;
                font-weight: bold;
            }
            QLabel#modern_saat_label {
                background-color: #153c2f;
                color: #34d399;
                border: 1px solid #10b981;
                border-radius: 5px;
                padding: 1px 6px;
                font-weight: bold;
            }
            QLabel#modern_miladi_label {
                color: #d1fae5;
            }
            QLabel#modern_hicri_label {
                color: #a7f3d0;
            }
            QFrame#modern_sayac_kart {
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #133b2e, stop:1 #0f2920);
                border: 1px solid #10b981;
                border-radius: 10px;
            }
            QLabel#modern_vakit_durum_label {
                color: #34d399;
                font-weight: bold;
            }
            QLabel#modern_kalan_baslik_label {
                color: #a7f3d0;
            }
            QLabel#modern_kalan_sure_label {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel#modern_dini_gun_label {
                color: #fbbf24;
                font-weight: bold;
            }
            QFrame[vakitSatiri="true"] {
                background-color: #102a21;
                border: 1px solid #194737;
                border-radius: 7px;
            }
            QFrame[vakitSatiri="true"][aktif="true"] {
                background-color: #164435;
                border: 1.5px solid #34d399;
                border-radius: 7px;
            }
            QLabel[vakitAd="true"] {
                color: #d1fae5;
                font-weight: 500;
            }
            QLabel[vakitAd="true"][aktif="true"] {
                color: #34d399;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"] {
                color: #ecfdf5;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"][aktif="true"] {
                color: #fbbf24;
                font-weight: bold;
            }
            QLabel[vakitBadge="true"] {
                background-color: #10b981;
                color: #ffffff;
                border-radius: 4px;
                padding: 1px 5px;
                font-weight: bold;
            }
        )" + getOrtakQss()
    };

    temalar["gece_mavisi"] = {
        "Gece Mavisi (Navy)",
        R"(
            QMainWindow, QWidget#centralwidget {
                background-color: #0d1527;
                color: #f0f4f8;
            }
            QTabWidget::pane {
                border: 1px solid #1e293b;
                background-color: #0d1527;
            }
            QTabWidget::tab-bar {
                alignment: center;
            }
            QTabBar {
                alignment: center;
            }
            QTabBar::tab {
                background-color: #131f37;
                color: #94a3b8;
                border: 1px solid #1e293b;
                border-top: none;
                border-bottom-left-radius: 6px;
                border-bottom-right-radius: 6px;
                padding: 1px;
                margin: 0px 2px;
                width: 50px;
                height: 44px;
            }
            QTabBar::tab:selected {
                background-color: #1c2c4e;
                color: #ffffff;
                border-top: 3px solid #38bdf8;
            }
            QTabBar::tab:hover:!selected {
                background-color: #172542;
                color: #e2e8f0;
            }
            QTabBar QToolButton {
                width: 0px;
                height: 0px;
                border: none;
                background: transparent;
            }
            QTabBar::scroller {
                width: 0px;
                height: 0px;
            }
            QGroupBox {
                background-color: #121c33;
                border: 1px solid #213254;
                border-radius: 8px;
                margin-top: 10px;
                font-weight: bold;
                color: #38bdf8;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0 6px;
                background-color: transparent;
            }
            QLabel {
                color: #f1f5f9;
            }
            QLabel#namaz_vakti_label {
                color: #38bdf8;
                font-weight: bold;
            }
            QLabel#sonraki_vakit_label, QLabel#kalan_sure_label {
                color: #7dd3fc;
                font-weight: bold;
            }
            QLabel a, QGroupBox#linkler_groupBox QLabel, QGroupBox#linkler_groupBox QLabel a, QGroupBox#linkler_groupBox QLabel span {
                color: #ffffff;
            }
            QLineEdit {
                background-color: #172440;
                color: #ffffff;
                border: 1px solid #283d66;
                border-radius: 4px;
                padding: 3px 6px;
                selection-background-color: #0284c7;
            }
            QLineEdit:focus {
                border: 1px solid #38bdf8;
            }
            QComboBox {
                background-color: #172440;
                color: #ffffff;
                border: 1px solid #283d66;
                border-radius: 4px;
                padding: 3px 6px;
            }
            QComboBox:focus {
                border: 1px solid #38bdf8;
            }
            QComboBox QAbstractItemView {
                background-color: #121c33;
                color: #ffffff;
                selection-background-color: #0284c7;
                border: 1px solid #283d66;
            }
            QPushButton, QToolButton {
                background-color: #1c2c4e;
                color: #f8fafc;
                border: 1px solid #283d66;
                border-radius: 5px;
                padding: 4px 10px;
                font-weight: 500;
            }
            QPushButton:hover, QToolButton:hover {
                background-color: #243863;
                border-color: #38bdf8;
            }
            QPushButton:pressed, QToolButton:pressed {
                background-color: #131f37;
            }
            QPushButton#pushButton_android, QPushButton#pushButton_ios {
                border: none;
                background-color: transparent;
                padding: 0px;
            }
            QCheckBox {
                color: #f1f5f9;
            }
            QCheckBox::indicator {
                width: 15px;
                height: 15px;
                border: 1px solid #283d66;
                border-radius: 3px;
                background-color: #172440;
            }
            QCheckBox::indicator:checked {
                background-color: #0284c7;
                border-color: #38bdf8;
            }
            QSlider::groove:horizontal {
                height: 4px;
                background: #283d66;
                border-radius: 2px;
            }
            QSlider::sub-page:horizontal {
                background: #0284c7;
                border-radius: 2px;
            }
            QSlider::handle:horizontal {
                background: #7dd3fc;
                border: 1px solid #0284c7;
                width: 14px;
                margin-top: -5px;
                margin-bottom: -5px;
                border-radius: 7px;
            }
            QMenuBar {
                background-color: #090e1b;
                color: #f1f5f9;
                border-bottom: 1px solid #172440;
                padding: 2px 4px;
            }
            QMenuBar::item {
                padding: 4px 10px;
                border-radius: 4px;
                background: transparent;
            }
            QMenuBar::item:selected {
                background-color: #172440;
            }
            QMenu {
                background-color: #121c33;
                color: #f1f5f9;
                border: 1px solid #213254;
                padding: 4px;
            }
            QMenu::item {
                padding: 5px 24px 5px 24px;
                border-radius: 4px;
            }
            QMenu::item:selected {
                background-color: #0284c7;
                color: #ffffff;
            }
            QMenu::separator {
                height: 1px;
                background: #213254;
                margin: 4px 6px;
            }
            QStatusBar {
                background-color: #070b16;
                color: #94a3b8;
                border-top: 1px solid #131f37;
            }
            QFrame#line {
                color: #213254;
            }
            /* Modern Görünüm Kartları - Gece Mavisi Tema */
            QFrame#modern_ust_kart {
                background-color: #131e36;
                border: 1px solid #23365c;
                border-radius: 10px;
            }
            QLabel#modern_sehir_label {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel#modern_ulke_label {
                color: #7dd3fc;
                font-weight: bold;
            }
            QLabel#modern_saat_label {
                background-color: #172746;
                color: #38bdf8;
                border: 1px solid #0284c7;
                border-radius: 5px;
                padding: 1px 6px;
                font-weight: bold;
            }
            QLabel#modern_miladi_label {
                color: #e2e8f0;
            }
            QLabel#modern_hicri_label {
                color: #94a3b8;
            }
            QFrame#modern_sayac_kart {
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #172a4a, stop:1 #111c33);
                border: 1px solid #0284c7;
                border-radius: 10px;
            }
            QLabel#modern_vakit_durum_label {
                color: #38bdf8;
                font-weight: bold;
            }
            QLabel#modern_kalan_baslik_label {
                color: #94a3b8;
            }
            QLabel#modern_kalan_sure_label {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel#modern_dini_gun_label {
                color: #fbbf24;
                font-weight: bold;
            }
            QFrame[vakitSatiri="true"] {
                background-color: #131e36;
                border: 1px solid #1f3052;
                border-radius: 7px;
            }
            QFrame[vakitSatiri="true"][aktif="true"] {
                background-color: #192b4d;
                border: 1.5px solid #38bdf8;
                border-radius: 7px;
            }
            QLabel[vakitAd="true"] {
                color: #cbd5e1;
                font-weight: 500;
            }
            QLabel[vakitAd="true"][aktif="true"] {
                color: #38bdf8;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"] {
                color: #f1f5f9;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"][aktif="true"] {
                color: #fbbf24;
                font-weight: bold;
            }
            QLabel[vakitBadge="true"] {
                background-color: #0284c7;
                color: #ffffff;
                border-radius: 4px;
                padding: 1px 5px;
                font-weight: bold;
            }
        )" + getOrtakQss()
    };

    temalar["kehribar"] = {
        "Sıcak Kehribar (Amber)",
        R"(
            QMainWindow, QWidget#centralwidget {
                background-color: #211a16;
                color: #fef3c7;
            }
            QTabWidget::pane {
                border: 1px solid #3d2e24;
                background-color: #211a16;
            }
            QTabWidget::tab-bar {
                alignment: center;
            }
            QTabBar {
                alignment: center;
            }
            QTabBar::tab {
                background-color: #2b221c;
                color: #d4a373;
                border: 1px solid #3d2e24;
                border-top: none;
                border-bottom-left-radius: 6px;
                border-bottom-right-radius: 6px;
                padding: 1px;
                margin: 0px 2px;
                width: 50px;
                height: 44px;
            }
            QTabBar::tab:selected {
                background-color: #3b2e25;
                color: #ffffff;
                border-top: 3px solid #f59e0b;
            }
            QTabBar::tab:hover:!selected {
                background-color: #332820;
                color: #fde68a;
            }
            QTabBar QToolButton {
                width: 0px;
                height: 0px;
                border: none;
                background: transparent;
            }
            QTabBar::scroller {
                width: 0px;
                height: 0px;
            }
            QGroupBox {
                background-color: #271f1a;
                border: 1px solid #4a382c;
                border-radius: 8px;
                margin-top: 10px;
                font-weight: bold;
                color: #fbbf24;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0 6px;
                background-color: transparent;
            }
            QLabel {
                color: #fef3c7;
            }
            QLabel#namaz_vakti_label {
                color: #fbbf24;
                font-weight: bold;
            }
            QLabel#sonraki_vakit_label, QLabel#kalan_sure_label {
                color: #fcd34d;
                font-weight: bold;
            }
            QLabel a, QGroupBox#linkler_groupBox QLabel, QGroupBox#linkler_groupBox QLabel a, QGroupBox#linkler_groupBox QLabel span {
                color: #ffffff;
            }
            QLineEdit {
                background-color: #2f251f;
                color: #ffffff;
                border: 1px solid #574133;
                border-radius: 4px;
                padding: 3px 6px;
                selection-background-color: #d97706;
            }
            QLineEdit:focus {
                border: 1px solid #fbbf24;
            }
            QComboBox {
                background-color: #2f251f;
                color: #ffffff;
                border: 1px solid #574133;
                border-radius: 4px;
                padding: 3px 6px;
            }
            QComboBox:focus {
                border: 1px solid #fbbf24;
            }
            QComboBox QAbstractItemView {
                background-color: #271f1a;
                color: #ffffff;
                selection-background-color: #d97706;
                border: 1px solid #574133;
            }
            QPushButton, QToolButton {
                background-color: #3b2e25;
                color: #fffbeb;
                border: 1px solid #574133;
                border-radius: 5px;
                padding: 4px 10px;
                font-weight: 500;
            }
            QPushButton:hover, QToolButton:hover {
                background-color: #4c3c30;
                border-color: #fbbf24;
            }
            QPushButton:pressed, QToolButton:pressed {
                background-color: #2b221c;
            }
            QPushButton#pushButton_android, QPushButton#pushButton_ios {
                border: none;
                background-color: transparent;
                padding: 0px;
            }
            QCheckBox {
                color: #fef3c7;
            }
            QCheckBox::indicator {
                width: 15px;
                height: 15px;
                border: 1px solid #574133;
                border-radius: 3px;
                background-color: #2f251f;
            }
            QCheckBox::indicator:checked {
                background-color: #d97706;
                border-color: #fbbf24;
            }
            QSlider::groove:horizontal {
                height: 4px;
                background: #574133;
                border-radius: 2px;
            }
            QSlider::sub-page:horizontal {
                background: #d97706;
                border-radius: 2px;
            }
            QSlider::handle:horizontal {
                background: #fcd34d;
                border: 1px solid #d97706;
                width: 14px;
                margin-top: -5px;
                margin-bottom: -5px;
                border-radius: 7px;
            }
            QMenuBar {
                background-color: #17120f;
                color: #fef3c7;
                border-bottom: 1px solid #2f251f;
                padding: 2px 4px;
            }
            QMenuBar::item {
                padding: 4px 10px;
                border-radius: 4px;
                background: transparent;
            }
            QMenuBar::item:selected {
                background-color: #2f251f;
            }
            QMenu {
                background-color: #271f1a;
                color: #fef3c7;
                border: 1px solid #4a382c;
                padding: 4px;
            }
            QMenu::item {
                padding: 5px 24px 5px 24px;
                border-radius: 4px;
            }
            QMenu::item:selected {
                background-color: #d97706;
                color: #ffffff;
            }
            QMenu::separator {
                height: 1px;
                background: #4a382c;
                margin: 4px 6px;
            }
            QStatusBar {
                background-color: #120e0c;
                color: #d4a373;
                border-top: 1px solid #2b221c;
            }
            QFrame#line {
                color: #4a382c;
            }
            /* Modern Görünüm Kartları - Sıcak Kehribar Tema */
            QFrame#modern_ust_kart {
                background-color: #2a211b;
                border: 1px solid #4d3a2e;
                border-radius: 10px;
            }
            QLabel#modern_sehir_label {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel#modern_ulke_label {
                color: #fcd34d;
                font-weight: bold;
            }
            QLabel#modern_saat_label {
                background-color: #382b22;
                color: #fbbf24;
                border: 1px solid #d97706;
                border-radius: 5px;
                padding: 1px 6px;
                font-weight: bold;
            }
            QLabel#modern_miladi_label {
                color: #fef3c7;
            }
            QLabel#modern_hicri_label {
                color: #d4a373;
            }
            QFrame#modern_sayac_kart {
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #3b2d23, stop:1 #281f19);
                border: 1px solid #d97706;
                border-radius: 10px;
            }
            QLabel#modern_vakit_durum_label {
                color: #fbbf24;
                font-weight: bold;
            }
            QLabel#modern_kalan_baslik_label {
                color: #fde68a;
            }
            QLabel#modern_kalan_sure_label {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel#modern_dini_gun_label {
                color: #34d399;
                font-weight: bold;
            }
            QFrame[vakitSatiri="true"] {
                background-color: #2a211b;
                border: 1px solid #433329;
                border-radius: 7px;
            }
            QFrame[vakitSatiri="true"][aktif="true"] {
                background-color: #3d2f24;
                border: 1.5px solid #f59e0b;
                border-radius: 7px;
            }
            QLabel[vakitAd="true"] {
                color: #fde68a;
                font-weight: 500;
            }
            QLabel[vakitAd="true"][aktif="true"] {
                color: #fbbf24;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"] {
                color: #fffbeb;
                font-weight: bold;
            }
            QLabel[vakitSaat="true"][aktif="true"] {
                color: #ffffff;
                font-weight: bold;
            }
            QLabel[vakitBadge="true"] {
                background-color: #d97706;
                color: #ffffff;
                border-radius: 4px;
                padding: 1px 5px;
                font-weight: bold;
            }
        )" + getOrtakQss()
    };

    return temalar;
}

QString Temalar::getTemaQss(const QString& temaKey) {
    const auto& t = getTemalar();
    if (t.contains(temaKey)) {
        return t[temaKey].qss;
    }
    return t["varsayilan"].qss;
}
