#include "HavaDurumu.h"
#include "HttpClient.h"
#include "Localization.h"
#include <QPainter>
#include <QPainterPath>
#include <QDateTime>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static QPair<QString, QString> wmoDurumGetir(int kod) {
    static const QMap<int, QString> wmoTipler = {
        {0, "gunesli"}, {1, "az_bulutlu"}, {2, "parcali_bulutlu"}, {3, "bulutlu"},
        {45, "sisli"}, {48, "sisli"}, {51, "yagmurlu"}, {53, "yagmurlu"}, {55, "yagmurlu"},
        {56, "yagmurlu"}, {57, "yagmurlu"}, {61, "yagmurlu"}, {63, "yagmurlu"}, {65, "saganak"},
        {66, "saganak"}, {67, "saganak"}, {71, "karli"}, {73, "karli"}, {75, "karli"},
        {77, "karli"}, {80, "saganak"}, {81, "saganak"}, {82, "saganak"}, {85, "karli"},
        {86, "karli"}, {95, "firtina"}, {96, "firtina"}, {99, "firtina"}
    };
    QString tip = wmoTipler.value(kod, "gunesli");
    QString durum = Localization::weatherDescription(kod);
    return {durum, tip};
}

static const QMap<QString, QPair<double, double>>& getTurkiyeKoordinatlari() {
    static const QMap<QString, QPair<double, double>> coords = {
        {"ADANA", {37.0000, 35.3213}}, {"ADIYAMAN", {37.7648, 38.2786}}, {"AFYONKARAHİSAR", {38.7507, 30.5567}},
        {"AĞRI", {39.7191, 43.0503}}, {"AMASYA", {40.6499, 35.8353}}, {"ANKARA", {39.9208, 32.8541}},
        {"ANTALYA", {36.8841, 30.7056}}, {"ARTVİN", {41.1828, 41.8183}}, {"AYDIN", {37.8560, 27.8416}},
        {"BALIKESİR", {39.6484, 27.8826}}, {"BİLECİK", {40.1451, 29.9799}}, {"BİNGÖL", {38.8854, 40.4980}},
        {"BİTLİS", {38.4006, 42.1095}}, {"BOLU", {40.7392, 31.6089}}, {"BURDUR", {37.7203, 30.2908}},
        {"BURSA", {40.1885, 29.0610}}, {"ÇANAKKALE", {40.1553, 26.4142}}, {"ÇANKIRI", {40.6013, 33.6134}},
        {"ÇORUM", {40.5506, 34.9556}}, {"DENİZLİ", {37.7765, 29.0864}}, {"DİYARBAKIR", {37.9144, 40.2306}},
        {"EDİRNE", {41.6818, 26.5623}}, {"ELAZIĞ", {38.6810, 39.2264}}, {"ERZİNCAN", {39.7500, 39.5000}},
        {"ERZURUM", {39.9000, 41.2700}}, {"ESKİŞEHİR", {39.7767, 30.5206}}, {"GAZİANTEP", {37.0662, 37.3833}},
        {"GİRESUN", {40.9128, 38.3895}}, {"GÜMÜŞHANE", {40.4386, 39.5086}}, {"HAKKARİ", {37.5833, 43.7333}},
        {"HATAY", {36.2021, 36.1606}}, {"ISPARTA", {37.7648, 30.5566}}, {"MERSİN", {36.8121, 34.6415}},
        {"İSTANBUL", {41.0082, 28.9784}}, {"İZMİR", {38.4237, 27.1428}}, {"KARS", {40.6013, 43.0975}},
        {"KASTAMONU", {41.3887, 33.7827}}, {"KAYSERİ", {38.7312, 35.4787}}, {"KIRKLARELİ", {41.7333, 27.2167}},
        {"KIRŞEHİR", {39.1425, 34.1709}}, {"KOCAELİ", {40.8533, 29.8815}}, {"KONYA", {37.8667, 32.4833}},
        {"KÜTAHYA", {39.4167, 29.9833}}, {"MALATYA", {38.3552, 38.3095}}, {"MANİSA", {38.6191, 27.4289}},
        {"KAHRAMANMARAŞ", {37.5858, 36.9371}}, {"MARDİN", {37.3212, 40.7245}}, {"MUĞLA", {37.2153, 28.3636}},
        {"MUŞ", {38.9462, 41.7539}}, {"NEVŞEHİR", {38.6939, 34.6857}}, {"NİĞDE", {37.9667, 34.6833}},
        {"ORDU", {40.9839, 37.8764}}, {"RİZE", {41.0201, 40.5234}}, {"SAKARYA", {40.7569, 30.3783}},
        {"SAMSUN", {41.2867, 36.3300}}, {"SİİRT", {37.9333, 41.9500}}, {"SİNOP", {42.0231, 35.1531}},
        {"SİVAS", {39.7477, 37.0179}}, {"TEKİRDAĞ", {40.9833, 27.5167}}, {"TOKAT", {40.3167, 36.5500}},
        {"TRABZON", {41.0027, 39.7168}}, {"TUNCELİ", {39.1079, 39.5401}}, {"ŞANLIURFA", {37.1591, 38.7969}},
        {"UŞAK", {38.6823, 29.4082}}, {"VAN", {38.4891, 43.4089}}, {"YOZGAT", {39.8181, 34.8147}},
        {"ZONGULDAK", {41.4564, 31.7987}}, {"AKSARAY", {38.3687, 34.0370}}, {"BAYBURT", {40.2552, 40.2249}},
        {"KARAMAN", {37.1759, 33.2287}}, {"KIRIKKALE", {39.8468, 33.5153}}, {"BATMAN", {37.8812, 41.1351}},
        {"ŞIRNAK", {37.4187, 42.4918}}, {"BARTIN", {41.6344, 32.3375}}, {"ARDAHAN", {41.1105, 42.7022}},
        {"IĞDIR", {39.9167, 44.0333}}, {"YALOVA", {40.6500, 29.2667}}, {"KARABÜK", {41.2061, 32.6204}},
        {"KİLİS", {36.7184, 37.1212}}, {"OSMANİYE", {37.0742, 36.2478}}, {"DÜZCE", {40.8438, 31.1565}}
    };
    return coords;
}

// ----------------- HavaDurumuThread -----------------

HavaDurumuThread::HavaDurumuThread(const QString& ulke, const QString& sehir, const QString& ilce, QObject* parent)
    : QThread(parent), m_ulke(ulke.trimmed()), m_sehir(sehir.trimmed()), m_ilce(ilce.trimmed()) {
    qRegisterMetaType<HavaVerisi>("HavaVerisi");
}

QPair<double, double> HavaDurumuThread::koordinatBul() const {
    QStringList aranacaklar;
    if (!m_ilce.isEmpty() && m_ilce != m_sehir) {
        aranacaklar.append(m_ilce);
    }
    if (!m_sehir.isEmpty()) {
        aranacaklar.append(m_sehir);
    }

    for (const QString& sorgu : aranacaklar) {
        QString q = QString::fromUtf8(QUrl::toPercentEncoding(sorgu));
        QString url = QString("https://geocoding-api.open-meteo.com/v1/search?name=%1&count=5&language=tr&format=json").arg(q);
        bool ok = false;
        QByteArray raw = HttpClient::get(url, 6000, &ok);
        if (ok && !raw.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(raw);
            if (doc.isObject()) {
                QJsonArray results = doc.object().value("results").toArray();
                if (!results.isEmpty()) {
                    if (m_ulke == "TÜRKİYE" || m_ulke == "TURKIYE") {
                        for (const QJsonValue& val : results) {
                            QJsonObject r = val.toObject();
                            if (r.value("country_code").toString() == "TR") {
                                return {r.value("latitude").toDouble(), r.value("longitude").toDouble()};
                            }
                        }
                    }
                    QJsonObject r0 = results[0].toObject();
                    return {r0.value("latitude").toDouble(), r0.value("longitude").toDouble()};
                }
            }
        }
    }

    const auto& trMap = getTurkiyeKoordinatlari();
    QString sehirUst = m_sehir.toUpper();
    if (trMap.contains(sehirUst)) {
        return trMap[sehirUst];
    }
    QString ilceUst = m_ilce.toUpper();
    if (trMap.contains(ilceUst)) {
        return trMap[ilceUst];
    }

    return {41.0082, 28.9784};
}

void HavaDurumuThread::run() {
    auto coords = koordinatBul();
    double lat = coords.first;
    double lon = coords.second;

    QString url = QString(
        "https://api.open-meteo.com/v1/forecast?"
        "latitude=%1&longitude=%2"
        "&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,weather_code,wind_speed_10m"
        "&daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_probability_max"
        "&timezone=auto&forecast_days=5"
    ).arg(QString::number(lat, 'f', 4), QString::number(lon, 'f', 4));

    bool ok = false;
    QByteArray rawBytes = HttpClient::get(url, 8000, &ok);
    if (!ok || rawBytes.isEmpty()) {
        emit hataOlustu("Bağlantı kurulamadı");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(rawBytes);
    if (!doc.isObject()) {
        emit hataOlustu("Veri okunamadı");
        return;
    }

    QJsonObject raw = doc.object();
    QJsonObject cur = raw.value("current").toObject();
    QJsonObject daily = raw.value("daily").toObject();

    int wcode = cur.value("weather_code").toInt(0);
    auto durumCift = wmoDurumGetir(wcode);

    QVector<GunlukTahmin> gunlukListe;
    QJsonArray times = daily.value("time").toArray();
    QJsonArray codes = daily.value("weather_code").toArray();
    QJsonArray maxs = daily.value("temperature_2m_max").toArray();
    QJsonArray mins = daily.value("temperature_2m_min").toArray();
    QJsonArray probs = daily.value("precipitation_probability_max").toArray();

    int limit = std::min(5, static_cast<int>(times.size()));
    for (int i = 0; i < limit; ++i) {
        QString tarihStr = times[i].toString();
        QDate dt = QDate::fromString(tarihStr, Qt::ISODate);
        int dayOfWeek = dt.isValid() ? dt.dayOfWeek() : 1;
        QString gunAd = (i == 0) ? Localization::tr("bugun") : (dt.isValid() ? Localization::dayName(dayOfWeek) : tarihStr);

        int dc = (i < codes.size()) ? codes[i].toInt(0) : 0;
        auto dCift = wmoDurumGetir(dc);
        int tmax = (i < maxs.size() && !maxs[i].isNull()) ? static_cast<int>(std::round(maxs[i].toDouble())) : 0;
        int tmin = (i < mins.size() && !mins[i].isNull()) ? static_cast<int>(std::round(mins[i].toDouble())) : 0;
        int prob = (i < probs.size() && !probs[i].isNull()) ? probs[i].toInt(0) : 0;

        GunlukTahmin gt;
        gt.gun = gunAd;
        gt.durum = dCift.first;
        gt.tip = dCift.second;
        gt.tMax = tmax;
        gt.tMin = tmin;
        gt.yagis = prob;
        gt.wmoKodu = dc;
        gt.dayOfWeek = dayOfWeek;
        gunlukListe.append(gt);
    }

    HavaVerisi sonuc;
    sonuc.sicaklik = static_cast<int>(std::round(cur.value("temperature_2m").toDouble(0)));
    sonuc.hissedilen = static_cast<int>(std::round(cur.value("apparent_temperature").toDouble(0)));
    sonuc.nem = cur.value("relative_humidity_2m").toInt(0);
    sonuc.ruzgar = static_cast<int>(std::round(cur.value("wind_speed_10m").toDouble(0)));
    sonuc.gunduzMu = (cur.value("is_day").toInt(1) != 0);
    sonuc.wmoKodu = wcode;
    sonuc.durum = durumCift.first;
    sonuc.tip = durumCift.second;
    sonuc.gunluk = gunlukListe;
    sonuc.guncellemeSaati = QTime::currentTime().toString("HH:mm");
    sonuc.gecerli = true;

    emit veriHazir(sonuc);
}

// ----------------- HavaDurumuWidget -----------------

HavaDurumuWidget::HavaDurumuWidget(QWidget* parent) : QWidget(parent) {
    setGeometry(10, 8, 306, 378);
    setObjectName("hava_durumu_widget");

    m_yenileBtn = new QPushButton("↻", this);
    m_yenileBtn->setGeometry(228, 12, 64, 24);
    m_yenileBtn->setCursor(Qt::PointingHandCursor);
    connect(m_yenileBtn, &QPushButton::clicked, this, [this]() {
        havaDurumunuGuncelle(true);
    });
    retranslateUi();
}

HavaDurumuWidget::~HavaDurumuWidget() {
    if (m_aktifThread && m_aktifThread->isRunning()) {
        m_aktifThread->wait(2000);
    }
}

void HavaDurumuWidget::retranslateUi() {
    if (m_yenileBtn) {
        m_yenileBtn->setText(QString("↻ %1").arg(Localization::tr("yenile")));
        m_yenileBtn->setToolTip(Localization::tr("hava_yenile_tooltip"));
    }
    if (m_veri.gecerli) {
        m_veri.durum = Localization::weatherDescription(m_veri.wmoKodu);
        for (int i = 0; i < m_veri.gunluk.size(); ++i) {
            m_veri.gunluk[i].durum = Localization::weatherDescription(m_veri.gunluk[i].wmoKodu);
            m_veri.gunluk[i].gun = (i == 0) ? Localization::tr("bugun") : Localization::dayName(m_veri.gunluk[i].dayOfWeek);
        }
    }
    update();
}

void HavaDurumuWidget::temaAyarla(const QString& tema) {
    m_tema = !tema.isEmpty() ? tema : "varsayilan";
    update();
}

void HavaDurumuWidget::konumAyarla(const QString& ulke, const QString& sehir, const QString& ilce, const QString& tema, bool zorla) {
    m_ulke = !ulke.isEmpty() ? ulke : "TÜRKİYE";
    m_sehir = !sehir.isEmpty() ? sehir : "İSTANBUL";
    m_ilce = !ilce.isEmpty() ? ilce : m_sehir;
    m_tema = !tema.isEmpty() ? tema : "varsayilan";

    QString anahtar = QString("%1|%2|%3").arg(m_ulke, m_sehir, m_ilce);
    if (zorla || anahtar != m_sonSorguAnahtari || !m_veri.gecerli) {
        if (!m_sehir.isEmpty()) {
            m_sonSorguAnahtari = anahtar;
            havaDurumunuGuncelle(true);
        }
    }
    update();
}

void HavaDurumuWidget::havaDurumunuGuncelle(bool zorla) {
    Q_UNUSED(zorla);
    if (m_aktifThread && m_aktifThread->isRunning()) {
        return;
    }
    m_yukleniyor = true;
    m_hataMesaji.clear();
    update();

    m_aktifThread = new HavaDurumuThread(m_ulke, m_sehir, m_ilce, this);
    connect(m_aktifThread, &HavaDurumuThread::veriHazir, this, &HavaDurumuWidget::onVeriHazir);
    connect(m_aktifThread, &HavaDurumuThread::hataOlustu, this, &HavaDurumuWidget::onHata);
    connect(m_aktifThread, &QThread::finished, m_aktifThread, &QObject::deleteLater);
    connect(m_aktifThread, &QThread::finished, this, [this]() { m_aktifThread = nullptr; });
    m_aktifThread->start();
}

void HavaDurumuWidget::onVeriHazir(const HavaVerisi& veri) {
    m_veri = veri;
    m_veri.durum = Localization::weatherDescription(m_veri.wmoKodu);
    for (int i = 0; i < m_veri.gunluk.size(); ++i) {
        m_veri.gunluk[i].durum = Localization::weatherDescription(m_veri.gunluk[i].wmoKodu);
        m_veri.gunluk[i].gun = (i == 0) ? Localization::tr("bugun") : Localization::dayName(m_veri.gunluk[i].dayOfWeek);
    }
    m_yukleniyor = false;
    m_hataMesaji.clear();
    update();
}

void HavaDurumuWidget::onHata(const QString& hata) {
    Q_UNUSED(hata);
    m_yukleniyor = false;
    m_hataMesaji = Localization::tr("hava_hata");
    update();
}

void HavaDurumuWidget::animasyonAdimi() {
    m_animFaz += 0.06;
    if (m_animFaz > 6.28318) {
        m_animFaz -= 6.28318;
    }
    update();
}

void HavaDurumuWidget::bulutCiz(QPainter& p, double x, double y, const QColor& renk) {
    p.setPen(Qt::NoPen);
    p.setBrush(renk);
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(QRectF(x - 13, y - 2, 26, 11), 5, 5);
    path.addEllipse(QPointF(x - 5, y - 4), 7, 7);
    path.addEllipse(QPointF(x + 3, y - 6), 8, 8);
    p.drawPath(path);
}

void HavaDurumuWidget::havaIkonuCiz(QPainter& p, double cx, double cy, double olcek, const QString& tip, bool gunduzMu) {
    p.save();
    p.translate(cx, cy);
    p.scale(olcek, olcek);

    double pulse = (std::sin(m_animFaz) + 1.0) * 0.5;
    double drift = std::sin(m_animFaz) * 2.5;

    if (tip == "gunesli") {
        if (gunduzMu) {
            p.setPen(QPen(QColor(251, 191, 36, 200), 2.2, Qt::SolidLine, Qt::RoundCap));
            for (int i = 0; i < 8; ++i) {
                double ang = m_animFaz * 0.5 + i * (M_PI / 4.0);
                double r1 = 14.0, r2 = 19.0 + pulse * 1.5;
                p.drawLine(QPointF(r1 * std::cos(ang), r1 * std::sin(ang)),
                           QPointF(r2 * std::cos(ang), r2 * std::sin(ang)));
            }
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(251, 191, 36, 60));
            p.drawEllipse(QPointF(0, 0), 15.0 + pulse * 2.0, 15.0 + pulse * 2.0);
            p.setBrush(QColor(250, 204, 21));
            p.drawEllipse(QPointF(0, 0), 11.0, 11.0);
        } else {
            QPainterPath moon;
            moon.addEllipse(QPointF(0, 0), 12.0, 12.0);
            QPainterPath cut;
            cut.addEllipse(QPointF(-5.0, -3.0), 11.0, 11.0);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(254, 240, 138));
            p.drawPath(moon.subtracted(cut));
        }
    } else if (tip == "az_bulutlu" || tip == "parcali_bulutlu") {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(250, 204, 21));
        p.drawEllipse(QPointF(-6.0, -6.0), 9.5, 9.5);
        bulutCiz(p, drift * 0.6, 4.0, QColor(241, 245, 249));
    } else if (tip == "bulutlu" || tip == "sisli") {
        bulutCiz(p, -3.0, -2.0, QColor(148, 163, 184, 200));
        bulutCiz(p, drift * 0.6, 4.0, QColor(226, 232, 240));
    } else if (tip == "yagmurlu" || tip == "saganak") {
        bulutCiz(p, drift * 0.5, -2.0, QColor(148, 163, 184));
        p.setPen(QPen(QColor(56, 189, 248), 2.0, Qt::SolidLine, Qt::RoundCap));
        const double xOffsets[3] = {-8.0, 0.0, 8.0};
        for (int i = 0; i < 3; ++i) {
            double yOff = std::fmod(m_animFaz * 6.0 + i * 3.0, 10.0);
            p.drawLine(QPointF(xOffsets[i], 8.0 + yOff), QPointF(xOffsets[i] - 2.0, 13.0 + yOff));
        }
    } else if (tip == "karli") {
        bulutCiz(p, drift * 0.5, -2.0, QColor(203, 213, 225));
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255));
        const double xOffsets[3] = {-8.0, 0.0, 8.0};
        for (int i = 0; i < 3; ++i) {
            double yOff = std::fmod(m_animFaz * 4.0 + i * 3.0, 10.0);
            p.drawEllipse(QPointF(xOffsets[i], 9.0 + yOff), 2.0, 2.0);
        }
    } else if (tip == "firtina") {
        bulutCiz(p, drift * 0.5, -3.0, QColor(100, 116, 139));
        static const QPolygonF bolt(QVector<QPointF>{
            QPointF(1.0, 4.0), QPointF(-4.0, 12.0), QPointF(0.0, 12.0),
            QPointF(-2.0, 19.0), QPointF(5.0, 10.0), QPointF(1.0, 10.0)
        });
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(250, 204, 21));
        p.drawPolygon(bolt);
    }

    p.restore();
}

void HavaDurumuWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    bool isLight = (m_tema == "varsayilan");

    // 1. Ana Kart Arka Planı
    QRectF bgRect(2, 2, 302, 372);
    if (isLight) {
        p.setPen(QPen(QColor(219, 227, 238), 1.2));
        p.setBrush(QColor(255, 255, 255));
    } else if (m_tema == "zumrut") {
        p.setPen(QPen(QColor(6, 78, 59), 1.2));
        p.setBrush(QColor(6, 32, 24));
    } else if (m_tema == "gece_mavisi") {
        p.setPen(QPen(QColor(30, 58, 138), 1.2));
        p.setBrush(QColor(10, 22, 40));
    } else if (m_tema == "kehribar") {
        p.setPen(QPen(QColor(120, 53, 15), 1.2));
        p.setBrush(QColor(36, 22, 14));
    } else {
        p.setPen(QPen(QColor(39, 39, 42), 1.2));
        p.setBrush(QColor(18, 18, 20));
    }
    p.drawRoundedRect(bgRect, 10, 10);

    // 2. Üst Konum Başlığı
    QColor ustYaziRenk = isLight ? QColor(100, 116, 139) : QColor(148, 163, 184);
    QColor baslikRenk = isLight ? QColor(15, 23, 42) : QColor(248, 250, 252);

    p.setPen(ustYaziRenk);
    p.setFont(QFont("Segoe UI", 8, QFont::Bold));
    QString konumUst = (!m_sehir.isEmpty() && m_sehir != m_ilce) ? QString("%1 • %2").arg(m_ulke, m_sehir) : m_ulke;
    p.drawText(QRectF(14, 10, 205, 15), Qt::AlignLeft | Qt::AlignVCenter, konumUst);

    p.setPen(baslikRenk);
    p.setFont(QFont("Segoe UI", 12, QFont::Bold));
    p.drawText(QRectF(14, 25, 205, 22), Qt::AlignLeft | Qt::AlignVCenter, m_ilce);

    if (m_yukleniyor && !m_veri.gecerli) {
        p.setPen(ustYaziRenk);
        p.setFont(QFont("Segoe UI", 10, QFont::Bold));
        p.drawText(QRectF(20, 150, 266, 40), Qt::AlignCenter, Localization::tr("hava_yukleniyor"));
        return;
    }

    if (!m_hataMesaji.isEmpty() && !m_veri.gecerli) {
        p.setPen(QColor(239, 68, 68));
        p.setFont(QFont("Segoe UI", 9, QFont::Bold));
        p.drawText(QRectF(20, 150, 266, 40), Qt::AlignCenter, Localization::tr("hava_hata"));
        return;
    }

    if (!m_veri.gecerli) {
        return;
    }

    // 3. Hero Anlık Hava Durumu Kartı
    QRectF heroRect(12, 52, 282, 102);
    QLinearGradient heroGrad(12, 52, 294, 154);
    if (isLight) {
        heroGrad.setColorAt(0.0, QColor(2, 132, 199));
        heroGrad.setColorAt(1.0, QColor(37, 99, 235));
        p.setPen(QPen(QColor(147, 197, 253), 1));
    } else {
        heroGrad.setColorAt(0.0, QColor(15, 23, 42, 240));
        heroGrad.setColorAt(1.0, QColor(30, 58, 138, 220));
        p.setPen(QPen(QColor(56, 189, 248, 120), 1.2));
    }
    p.setBrush(heroGrad);
    p.drawRoundedRect(heroRect, 10, 10);

    // Sıcaklık
    p.setPen(QColor(255, 255, 255));
    p.setFont(QFont("Segoe UI", 26, QFont::Bold));
    p.drawText(QRectF(24, 58, 140, 44), Qt::AlignLeft | Qt::AlignVCenter, QString("%1°C").arg(m_veri.sicaklik));

    // Durum Açıklaması
    QString durumAciklama = Localization::weatherDescription(m_veri.wmoKodu);
    if (durumAciklama.isEmpty()) {
        durumAciklama = m_veri.durum;
    }
    p.setPen(QColor(224, 242, 254));
    p.setFont(QFont("Segoe UI", 10, QFont::Bold));
    p.drawText(QRectF(24, 104, 175, 20), Qt::AlignLeft | Qt::AlignVCenter, durumAciklama);

    // Bugün En Yüksek / En Düşük
    if (!m_veri.gunluk.isEmpty()) {
        const auto& bgn = m_veri.gunluk[0];
        p.setPen(QColor(186, 230, 253));
        p.setFont(QFont("Segoe UI", 8));
        p.drawText(
            QRectF(24, 126, 185, 18),
            Qt::AlignLeft | Qt::AlignVCenter,
            QString("%1: %2°   •   %3: %4°").arg(Localization::tr("en_yuksek")).arg(bgn.tMax).arg(Localization::tr("en_dusuk")).arg(bgn.tMin)
        );
    }

    // Sağda Animasyonlu Hava İkonu
    havaIkonuCiz(p, 242.0, 98.0, 1.45, m_veri.tip, m_veri.gunduzMu);

    // 4. Üçlü Detay Kutuları (Hissedilen, Nem, Rüzgâr)
    struct DetayKutu { QString etiket; QString deger; };
    QString ruzgarBirimi = Localization::tr("hava_ruzgar_birim");
    if (ruzgarBirimi.isEmpty() || ruzgarBirimi == "hava_ruzgar_birim") {
        ruzgarBirimi = (Localization::currentLanguage() == "tr") ? "km/s" : "km/h";
    }
    QVector<DetayKutu> detaylar = {
        {Localization::tr("hava_hissedilen"), QString("%1°C").arg(m_veri.hissedilen)},
        {Localization::tr("hava_nem"), QString("%%1").arg(m_veri.nem)},
        {Localization::tr("hava_ruzgar"), QString("%1 %2").arg(m_veri.ruzgar).arg(ruzgarBirimi)}
    };
    for (int idx = 0; idx < detaylar.size(); ++idx) {
        double dx = 12.0 + idx * 96.0;
        double dy = 162.0;
        QRectF dRect(dx, dy, 90.0, 46.0);
        if (isLight) {
            p.setPen(QPen(QColor(226, 232, 240), 1));
            p.setBrush(QColor(248, 250, 252));
        } else {
            p.setPen(QPen(QColor(51, 65, 85, 130), 1));
            p.setBrush(QColor(30, 41, 59, 140));
        }
        p.drawRoundedRect(dRect, 7, 7);

        p.setPen(ustYaziRenk);
        p.setFont(QFont("Segoe UI", 7));
        p.drawText(QRectF(dx + 4, dy + 5, 82, 14), Qt::AlignCenter, detaylar[idx].etiket);

        p.setPen(baslikRenk);
        p.setFont(QFont("Segoe UI", 10, QFont::Bold));
        p.drawText(QRectF(dx + 4, dy + 20, 82, 20), Qt::AlignCenter, detaylar[idx].deger);
    }

    // 5. 5 Günlük Hava Tahmini Satırları
    p.setPen(ustYaziRenk);
    p.setFont(QFont("Segoe UI", 8, QFont::Bold));
    p.drawText(QRectF(14, 214, 180, 16), Qt::AlignLeft | Qt::AlignVCenter, Localization::tr("hava_tahmin"));
    p.drawText(
        QRectF(180, 214, 112, 16),
        Qt::AlignRight | Qt::AlignVCenter,
        Localization::tr("hava_saat").arg(m_veri.guncellemeSaati)
    );

    double basY = 233.0;
    double satirH = 25.0;
    double aralikY = 27.0;

    int limit = std::min(5, static_cast<int>(m_veri.gunluk.size()));
    for (int idx = 0; idx < limit; ++idx) {
        const auto& g = m_veri.gunluk[idx];
        double sy = basY + idx * aralikY;
        QRectF sRect(12.0, sy, 282.0, satirH);
        if (isLight) {
            p.setPen(QPen(QColor(226, 232, 240), 1));
            p.setBrush(idx > 0 ? QColor(248, 250, 252) : QColor(239, 246, 255));
        } else {
            p.setPen(QPen(QColor(51, 65, 85, 110), 1));
            p.setBrush(idx > 0 ? QColor(30, 41, 59, 120) : QColor(30, 58, 138, 140));
        }
        p.drawRoundedRect(sRect, 6, 6);

        // Gün adı
        QString gunAd = (idx == 0) ? Localization::tr("bugun") : Localization::dayName(g.dayOfWeek);
        p.setPen(baslikRenk);
        p.setFont(QFont("Segoe UI", 8, idx == 0 ? QFont::Bold : QFont::Normal));
        p.drawText(QRectF(20, sy, 75, satirH), Qt::AlignLeft | Qt::AlignVCenter, gunAd);

        // Küçük İkon
        havaIkonuCiz(p, 108.0, sy + satirH / 2.0, 0.52, g.tip, true);

        // Durum veya Yağış İhtimali
        QString durumGun = Localization::weatherDescription(g.wmoKodu);
        if (durumGun.isEmpty()) {
            durumGun = g.durum;
        }
        QString durumMetni = durumGun;
        if (g.yagis >= 20) {
            durumMetni = QString("%1 (%%2)").arg(durumGun).arg(g.yagis);
        }
        p.setPen(ustYaziRenk);
        p.setFont(QFont("Segoe UI", 7));
        p.drawText(QRectF(124, sy, 105, satirH), Qt::AlignLeft | Qt::AlignVCenter, durumMetni);

        // Max / Min Sıcaklık
        p.setPen(baslikRenk);
        p.setFont(QFont("Segoe UI", 8, QFont::Bold));
        p.drawText(
            QRectF(225, sy, 62, satirH),
            Qt::AlignRight | Qt::AlignVCenter,
            QString("%1° / %2°").arg(g.tMax).arg(g.tMin)
        );
    }
}
