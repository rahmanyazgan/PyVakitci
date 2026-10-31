#include "VakitGorselleri.h"
#include "Localization.h"
#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <cmath>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int dakikayaCevir(const QString &saatStr) {
  QString clean = saatStr;
  clean.remove("<font color='#FF0000'>");
  clean.remove("</font>");
  clean = clean.trimmed();
  QStringList parts = clean.split(":");
  if (parts.size() >= 2) {
    return parts[0].toInt() * 60 + parts[1].toInt();
  }
  return 0;
}

static double dkToAngle(int dk) {
  // 24 Saatlik döngü: 24:00 tepede (-90°), 12:00 altta (90°)
  return (static_cast<double>(dk) / 1440.0) * 360.0 - 90.0;
}

static QString localizeVakitUpper(const QString &v) {
  QString key = v.trimmed().toLower();
  if (key == "sabah")
    key = "imsak";
  return Localization::tr(key).toUpper();
}

static QString sonrakiVakitEtiketiAl(const QString &aktifVakit,
                                     const QString &sonrakiVakitStr) {
  Q_UNUSED(sonrakiVakitStr);
  static const QMap<QString, QString> sonrakiMap = {
      {"imsak", "gunes"}, {"sabah", "gunes"},  {"gunes", "ogle"},
      {"ogle", "ikindi"}, {"ikindi", "aksam"}, {"aksam", "yatsi"},
      {"yatsi", "imsak"}};
  QString k = sonrakiMap.value(aktifVakit.toLower(), "imsak");
  return Localization::formatRemainingTo(k);
}

// ----------------- VakitSaatiWidget -----------------

VakitSaatiWidget::VakitSaatiWidget(QWidget *parent) : QWidget(parent) {
  setGeometry(10, 8, 306, 376);
  setObjectName("vakit_saati_widget");
}

void VakitSaatiWidget::veriGuncelle(const VakitVeriPaketi &veri) {
  m_veri = veri;
  update();
}

void VakitSaatiWidget::animasyonAdimi() {
  m_animFaz += 0.06;
  if (m_animFaz > 6.28318)
    m_animFaz -= 6.28318;
  update();
}

void VakitSaatiWidget::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::TextAntialiasing);

  bool isLight = (m_veri.tema == "varsayilan");

  // 1. Arka Plan Kartı
  QRectF bgRect(2, 2, 302, 372);
  if (isLight) {
    p.setPen(QPen(QColor(219, 227, 238), 1.2));
    p.setBrush(QColor(255, 255, 255));
  } else if (m_veri.tema == "zumrut") {
    p.setPen(QPen(QColor(6, 78, 59), 1.2));
    p.setBrush(QColor(6, 32, 24));
  } else if (m_veri.tema == "gece_mavisi") {
    p.setPen(QPen(QColor(30, 58, 138), 1.2));
    p.setBrush(QColor(10, 22, 40));
  } else if (m_veri.tema == "kehribar") {
    p.setPen(QPen(QColor(120, 53, 15), 1.2));
    p.setBrush(QColor(36, 22, 14));
  } else { // koyu
    p.setPen(QPen(QColor(39, 39, 42), 1.2));
    p.setBrush(QColor(18, 18, 20));
  }
  p.drawRoundedRect(bgRect, 10, 10);

  // 2. Üst Başlık (Konum, Tarih ve Özel Gün)
  QString konumBaslik =
      (m_veri.sehir.isEmpty() || m_veri.sehir == m_veri.seciliYer)
          ? m_veri.ulke
          : QString("%1 • %2").arg(m_veri.ulke, m_veri.sehir);

  QFont fontKonum("Segoe UI", 8, QFont::Bold);
  QFont fontIlce("Segoe UI", 12, QFont::Bold);
  QFont fontTarih("Segoe UI", 8);

  if (isLight) {
    p.setPen(QColor(100, 116, 139));
    p.setFont(fontKonum);
    p.drawText(QRectF(14, 10, 160, 15), Qt::AlignLeft | Qt::AlignVCenter,
               konumBaslik);

    p.setPen(QColor(15, 23, 42));
    p.setFont(fontIlce);
    p.drawText(QRectF(14, 25, 160, 22), Qt::AlignLeft | Qt::AlignVCenter,
               m_veri.seciliYer);

    p.setPen(QColor(71, 85, 105));
    p.setFont(fontTarih);
    p.drawText(QRectF(140, 10, 150, 15), Qt::AlignRight | Qt::AlignVCenter,
               m_veri.miladi);
    p.drawText(QRectF(130, 25, 160, 20), Qt::AlignRight | Qt::AlignVCenter,
               m_veri.hicri);
  } else {
    p.setPen(QColor(148, 163, 184));
    p.setFont(fontKonum);
    p.drawText(QRectF(14, 10, 160, 15), Qt::AlignLeft | Qt::AlignVCenter,
               konumBaslik);

    p.setPen(QColor(248, 250, 252));
    p.setFont(fontIlce);
    p.drawText(QRectF(14, 25, 160, 22), Qt::AlignLeft | Qt::AlignVCenter,
               m_veri.seciliYer);

    p.setPen(QColor(148, 163, 184));
    p.setFont(fontTarih);
    p.drawText(QRectF(140, 10, 150, 15), Qt::AlignRight | Qt::AlignVCenter,
               m_veri.miladi);
    p.drawText(QRectF(130, 25, 160, 20), Qt::AlignRight | Qt::AlignVCenter,
               m_veri.hicri);
  }

  // 3. VAKİT SAATİ KADRANI
  double cx = 153.0;
  double cy = 154.0;
  double r_dis = 92.0;
  double r_ic = 76.0;

  int imsak_dk = dakikayaCevir(m_veri.vakitler.value("imsak", "06:00"));
  int gunes_dk = dakikayaCevir(m_veri.vakitler.value("gunes", "07:25"));
  int ogle_dk = dakikayaCevir(m_veri.vakitler.value("ogle", "12:54"));
  int ikindi_dk = dakikayaCevir(m_veri.vakitler.value("ikindi", "15:45"));
  int aksam_dk = dakikayaCevir(m_veri.vakitler.value("aksam", "18:13"));
  int yatsi_dk = dakikayaCevir(m_veri.vakitler.value("yatsi", "19:33"));

  struct DilimInfo {
    QString anahtar;
    int b_dk;
    int s_dk;
    QColor renk;
    QString ad;
  };

  QVector<DilimInfo> dilimler = {
      {"imsak", imsak_dk, gunes_dk, QColor(249, 115, 22), "Sabah"},
      {"gunes", gunes_dk, ogle_dk, QColor(14, 165, 233), "Güneş"},
      {"ogle", ogle_dk, ikindi_dk, QColor(234, 179, 8), "Öğle"},
      {"ikindi", ikindi_dk, aksam_dk, QColor(245, 158, 11), "İkindi"},
      {"aksam", aksam_dk, yatsi_dk, QColor(168, 85, 247), "Akşam"},
      {"yatsi", yatsi_dk, imsak_dk + 1440, QColor(59, 130, 246), "Yatsı"}};

  // Kadran Zemin Çemberi
  QColor ringColor = isLight ? QColor(226, 232, 240) : QColor(39, 39, 42, 160);
  p.setPen(QPen(ringColor, 14, Qt::SolidLine, Qt::FlatCap));
  p.setBrush(Qt::NoBrush);
  double rMid = (r_dis + r_ic) / 2.0;
  p.drawEllipse(QPointF(cx, cy), rMid, rMid);

  // Renkli Vakit Yayları
  for (const auto &d : dilimler) {
    int span_dk = d.s_dk - d.b_dk;
    if (span_dk < 0)
      span_dk += 1440;

    double start_deg = -(dkToAngle(d.b_dk));
    double span_deg = -(static_cast<double>(span_dk) / 1440.0 * 360.0);

    bool aktifMi = (d.anahtar == m_veri.aktifVakit);
    double pen_w = aktifMi ? 15.0 : 13.0;
    QColor c = d.renk;
    if (aktifMi) {
      double pulse = (std::sin(m_animFaz) + 1.0) * 0.5;
      c.setAlpha(static_cast<int>(210 + 45 * pulse));
    } else {
      c.setAlpha(isLight ? 110 : 130);
    }

    p.setPen(QPen(c, pen_w, Qt::SolidLine, Qt::FlatCap));
    QRectF arcRect(cx - rMid, cy - rMid, rMid * 2.0, rMid * 2.0);
    p.drawArc(arcRect, static_cast<int>(start_deg * 16.0),
              static_cast<int>(span_deg * 16.0));
  }

  // Saat İşaretçileri (06, 12, 18, 24)
  QFont fontSaat("Segoe UI", 7, QFont::Bold);
  p.setFont(fontSaat);
  struct SaatIsaret {
    int h;
    QString txt;
  };
  QVector<SaatIsaret> isaretler = {
      {0, "24"}, {6, "06"}, {12, "12"}, {18, "18"}};
  for (const auto &si : isaretler) {
    double ang_rad = dkToAngle(si.h * 60) * (M_PI / 180.0);
    double tx = cx + (r_dis + 10.0) * std::cos(ang_rad);
    double ty = cy + (r_dis + 10.0) * std::sin(ang_rad);
    p.setPen(isLight ? QColor(100, 116, 139) : QColor(148, 163, 184));
    p.drawText(QRectF(tx - 12, ty - 8, 24, 16), Qt::AlignCenter, si.txt);
  }

  // Vakit Pinleri (Noktalar)
  struct PinInfo {
    QString anahtar;
    int dk;
    QString ad;
    QColor renk;
  };
  QVector<PinInfo> pinler = {{"imsak", imsak_dk, Localization::tr("imsak"), QColor("#ea580c")},
                             {"gunes", gunes_dk, Localization::tr("gunes"), QColor("#0ea5e9")},
                             {"ogle", ogle_dk, Localization::tr("ogle"), QColor("#eab308")},
                             {"ikindi", ikindi_dk, Localization::tr("ikindi"), QColor("#f97316")},
                             {"aksam", aksam_dk, Localization::tr("aksam"), QColor("#a855f7")},
                             {"yatsi", yatsi_dk, Localization::tr("yatsi"), QColor("#3b82f6")}};

  for (const auto &pin : pinler) {
    double ang_rad = dkToAngle(pin.dk) * (M_PI / 180.0);
    double px = cx + rMid * std::cos(ang_rad);
    double py = cy + rMid * std::sin(ang_rad);
    bool aktifMi = (pin.anahtar == m_veri.aktifVakit);
    double dot_r = aktifMi ? 5.2 : 3.8;
    p.setPen(QPen(QColor(255, 255, 255), 1.5));
    p.setBrush(aktifMi ? QColor("#ffffff") : pin.renk);
    p.drawEllipse(QPointF(px, py), dot_r, dot_r);
  }

  // 4. Canlı Zaman İbresi (Şimdiki Saati Gösteren İbre)
  QStringList parts = m_veri.saat.split(":");
  double cur_dk = 0.0;
  if (parts.size() >= 2) {
    double s = (parts.size() >= 3) ? parts[2].toDouble() : 0.0;
    cur_dk = parts[0].toDouble() * 60.0 + parts[1].toDouble() + (s / 60.0);
  }
  double exact_angle_deg = (cur_dk / 1440.0) * 360.0 - 90.0;
  double exact_hand_rad = exact_angle_deg * (M_PI / 180.0);

  double hx = cx + (r_dis + 2.0) * std::cos(exact_hand_rad);
  double hy = cy + (r_dis + 2.0) * std::sin(exact_hand_rad);
  p.setPen(QPen(QColor(239, 68, 68), 2.5, Qt::SolidLine, Qt::RoundCap));
  p.drawLine(QPointF(cx, cy), QPointF(hx, hy));

  p.setPen(QPen(QColor(255, 255, 255), 2.0));
  p.setBrush(QColor(239, 68, 68));
  p.drawEllipse(QPointF(hx, hy), 4.5, 4.5);

  // 5. Merkez Gösterge Camı (Center HUD)
  double center_r = 53.0;
  QRadialGradient hudGrad(cx, cy, center_r);
  if (isLight) {
    p.setPen(QPen(QColor(186, 230, 253), 1.5));
    hudGrad.setColorAt(0.0, QColor(240, 249, 255));
    hudGrad.setColorAt(1.0, QColor(224, 242, 254));
  } else {
    p.setPen(QPen(QColor(71, 85, 105, 180), 1.5));
    hudGrad.setColorAt(0.0, QColor(30, 41, 59, 250));
    hudGrad.setColorAt(1.0, QColor(15, 23, 42, 255));
  }
  p.setBrush(hudGrad);
  p.drawEllipse(QPointF(cx, cy), center_r, center_r);

  // Merkez Metinleri
  QColor aktifRenk = isLight ? QColor(2, 132, 199) : QColor(56, 189, 248);
  p.setPen(aktifRenk);
  QFont fontRozet("Segoe UI", 8, QFont::Bold);
  p.setFont(fontRozet);
  QString aktifBaslik = localizeVakitUpper(m_veri.aktifVakit);
  p.drawText(QRectF(cx - 50, cy - 38, 100, 16), Qt::AlignCenter,
             QString("✦ %1 ✦").arg(aktifBaslik));

  QColor saatRenk = isLight ? QColor(15, 23, 42) : QColor(255, 255, 255);
  p.setPen(saatRenk);
  QFont fontHudSaat("Segoe UI", 13, QFont::Bold);
  p.setFont(fontHudSaat);
  QString saatGosterim = m_veri.saat;
  if (saatGosterim.split(":").size() < 3) {
    saatGosterim = QTime::currentTime().toString("hh:mm:ss");
  }
  p.drawText(QRectF(cx - 50, cy - 20, 100, 22), Qt::AlignCenter, saatGosterim);

  QColor kalanBaslikRenk =
      isLight ? QColor(100, 116, 139) : QColor(148, 163, 184);
  p.setPen(kalanBaslikRenk);
  QFont fontKalanBaslik("Segoe UI", 7);
  p.setFont(fontKalanBaslik);

  QString sonrakiEtiket =
      sonrakiVakitEtiketiAl(m_veri.aktifVakit, m_veri.sonrakiVakit);
  p.drawText(QRectF(cx - 50, cy + 2, 100, 14), Qt::AlignCenter, sonrakiEtiket);

  QColor kalanRenk = isLight ? QColor(217, 119, 6) : QColor(251, 191, 36);
  p.setPen(kalanRenk);
  QFont fontKalanSure("Segoe UI", 10, QFont::Bold);
  p.setFont(fontKalanSure);
  p.drawText(QRectF(cx - 50, cy + 16, 100, 20), Qt::AlignCenter,
             m_veri.kalanSure);

  // 6. Alt Vakit Mini Kartları (2 Satır x 3 Sütun)
  struct MiniKart {
    QString ad;
    QString saat;
    QString key;
  };
  QVector<MiniKart> kartlar = {
      {Localization::tr("imsak"), m_veri.vakitler.value("imsak", "--:--"), "imsak"},
      {Localization::tr("gunes"), m_veri.vakitler.value("gunes", "--:--"), "gunes"},
      {Localization::tr("ogle"), m_veri.vakitler.value("ogle", "--:--"), "ogle"},
      {Localization::tr("ikindi"), m_veri.vakitler.value("ikindi", "--:--"), "ikindi"},
      {Localization::tr("aksam"), m_veri.vakitler.value("aksam", "--:--"), "aksam"},
      {Localization::tr("yatsi"), m_veri.vakitler.value("yatsi", "--:--"), "yatsi"}};

  double startX = 12.0;
  double startY = 265.0;
  double cardW = 90.0;
  double cardH = 46.0;
  double gapX = 6.0;
  double gapY = 6.0;

  for (int idx = 0; idx < kartlar.size(); ++idx) {
    int row = idx / 3;
    int col = idx % 3;
    double kx = startX + col * (cardW + gapX);
    double ky = startY + row * (cardH + gapY);
    QRectF cardRect(kx, ky, cardW, cardH);

    bool aktifMi = (kartlar[idx].key == m_veri.aktifVakit);
    if (isLight) {
      if (aktifMi) {
        double pulse = (std::sin(m_animFaz) + 1.0) * 0.5;
        int borderAlpha = static_cast<int>(180 + 75 * pulse);
        p.setPen(QPen(QColor(37, 99, 235, borderAlpha), 1.6));
        p.setBrush(QColor(239, 246, 255));
      } else {
        p.setPen(QPen(QColor(226, 232, 240), 1.0));
        p.setBrush(QColor(248, 250, 252));
      }
    } else {
      if (aktifMi) {
        double pulse = (std::sin(m_animFaz) + 1.0) * 0.5;
        int borderAlpha = static_cast<int>(180 + 75 * pulse);
        p.setPen(QPen(QColor(56, 189, 248, borderAlpha), 1.6));
        p.setBrush(QColor(30, 58, 138, 180));
      } else {
        p.setPen(QPen(QColor(51, 65, 85, 120), 1.0));
        p.setBrush(QColor(30, 41, 59, 140));
      }
    }
    p.drawRoundedRect(cardRect, 7, 7);

    // İsim
    if (isLight) {
      p.setPen(aktifMi ? QColor(29, 78, 216) : QColor(100, 116, 139));
    } else {
      p.setPen(aktifMi ? QColor(56, 189, 248) : QColor(148, 163, 184));
    }
    QFont fontKartAd("Segoe UI", 8, aktifMi ? QFont::Bold : QFont::Normal);
    p.setFont(fontKartAd);
    p.drawText(QRectF(kx + 6, ky + 5, cardW - 12, 16),
               Qt::AlignLeft | Qt::AlignVCenter, kartlar[idx].ad);

    // Saat
    if (isLight) {
      p.setPen(aktifMi ? QColor(220, 38, 38) : QColor(15, 23, 42));
    } else {
      p.setPen(aktifMi ? QColor(251, 191, 36) : QColor(255, 255, 255));
    }
    QFont fontKartSaat("Segoe UI", 11, QFont::Bold);
    p.setFont(fontKartSaat);
    p.drawText(QRectF(kx + 6, ky + 21, cardW - 12, 20),
               Qt::AlignRight | Qt::AlignVCenter, kartlar[idx].saat);

    // Aktif Nokta
    if (aktifMi) {
      p.setPen(Qt::NoPen);
      p.setBrush(isLight ? QColor(37, 99, 235) : QColor(56, 189, 248));
      p.drawEllipse(QPointF(kx + cardW - 12, ky + 12), 3.0, 3.0);
    }
  }
}

// ----------------- GokyuzuWidget -----------------

GokyuzuWidget::GokyuzuWidget(QWidget *parent) : QWidget(parent) {
  setGeometry(10, 8, 306, 376);
  setObjectName("gokyuzu_widget");
  yildizlariOlustur();
}

void GokyuzuWidget::yildizlariOlustur() {
  m_yildizlar.clear();
  std::mt19937 gen(42);
  std::uniform_real_distribution<double> distX(10.0, 296.0);
  std::uniform_real_distribution<double> distY(10.0, 160.0);
  std::uniform_real_distribution<double> distR(0.8, 2.2);
  std::uniform_real_distribution<double> distP(0.0, 6.28);
  for (int i = 0; i < 40; ++i) {
    m_yildizlar.append({distX(gen), distY(gen), distR(gen), distP(gen)});
  }
}

void GokyuzuWidget::veriGuncelle(const VakitVeriPaketi &veri) {
  m_veri = veri;
  update();
}

void GokyuzuWidget::animasyonAdimi() {
  m_animFaz += 0.06;
  if (m_animFaz > 6.28318)
    m_animFaz -= 6.28318;
  update();
}

void GokyuzuWidget::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::TextAntialiasing);

  double w = 302.0, h = 372.0;
  QRectF bgRect(2, 2, w, h);

  // 1. Vakte Göre Dinamik Gökyüzü Gradyanı
  QLinearGradient skyGrad(0, 0, 0, h);
  bool isNight = false;
  if (m_veri.aktifVakit == "yatsi" || m_veri.aktifVakit == "sabah") {
    skyGrad.setColorAt(0.0, QColor(11, 15, 25));
    skyGrad.setColorAt(0.5, QColor(23, 37, 84));
    skyGrad.setColorAt(1.0, QColor(30, 27, 75));
    isNight = true;
  } else if (m_veri.aktifVakit == "imsak") {
    skyGrad.setColorAt(0.0, QColor(23, 23, 56));
    skyGrad.setColorAt(0.6, QColor(124, 45, 18));
    skyGrad.setColorAt(1.0, QColor(245, 158, 11));
    isNight = false;
  } else if (m_veri.aktifVakit == "gunes") {
    skyGrad.setColorAt(0.0, QColor(2, 132, 199));
    skyGrad.setColorAt(0.7, QColor(56, 189, 248));
    skyGrad.setColorAt(1.0, QColor(254, 240, 138));
    isNight = false;
  } else if (m_veri.aktifVakit == "ogle") {
    skyGrad.setColorAt(0.0, QColor(3, 105, 161));
    skyGrad.setColorAt(0.6, QColor(14, 165, 233));
    skyGrad.setColorAt(1.0, QColor(186, 230, 253));
    isNight = false;
  } else if (m_veri.aktifVakit == "ikindi") {
    skyGrad.setColorAt(0.0, QColor(30, 58, 138));
    skyGrad.setColorAt(0.5, QColor(194, 65, 12));
    skyGrad.setColorAt(1.0, QColor(251, 191, 36));
    isNight = false;
  } else { // aksam
    skyGrad.setColorAt(0.0, QColor(30, 27, 75));
    skyGrad.setColorAt(0.5, QColor(131, 24, 67));
    skyGrad.setColorAt(1.0, QColor(234, 88, 12));
    isNight = true;
  }

  p.setPen(QPen(QColor(255, 255, 255, 30), 1));
  p.setBrush(skyGrad);
  p.drawRoundedRect(bgRect, 10, 10);

  // 2. Gökyüzü Öğeleri (Yıldızlar veya Güneş Halesi)
  if (isNight) {
    for (const auto &y : m_yildizlar) {
      double twinkle = (std::sin(m_animFaz * 2.0 + y.phase) + 1.0) * 0.5;
      int alpha = static_cast<int>(90 + 165 * twinkle);
      p.setPen(Qt::NoPen);
      p.setBrush(QColor(255, 255, 255, alpha));
      p.drawEllipse(QPointF(y.x, y.y), y.r, y.r);
    }

    // Hilal (Crescent Moon)
    double mx = 245.0, my = 48.0;
    QPainterPath moonPath;
    moonPath.addEllipse(QPointF(mx, my), 14.0, 14.0);
    QPainterPath cutout;
    cutout.addEllipse(QPointF(mx - 6.0, my - 4.0), 13.0, 13.0);
    QPainterPath crescent = moonPath.subtracted(cutout);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(254, 240, 138, 240));
    p.drawPath(crescent);
  } else {
    double sx = 245.0, sy = 46.0;
    double pulse = (std::sin(m_animFaz) + 1.0) * 0.5;
    double halo_r = 22.0 + 3.0 * pulse;
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(254, 240, 138, 50));
    p.drawEllipse(QPointF(sx, sy), halo_r, halo_r);
    p.setBrush(QColor(253, 224, 71, 220));
    p.drawEllipse(QPointF(sx, sy), 13.0, 13.0);
  }

  // 3. Üst Buzlu Cam Kapsülü (Location & Date)
  QRectF topCapsule(12, 12, 210, 52);
  p.setPen(QPen(QColor(255, 255, 255, 60), 1));
  p.setBrush(QColor(15, 23, 42, 130));
  p.drawRoundedRect(topCapsule, 8, 8);

  p.setPen(QColor(226, 232, 240));
  p.setFont(QFont("Segoe UI", 8, QFont::Bold));
  p.drawText(QRectF(22, 18, 190, 15), Qt::AlignLeft | Qt::AlignVCenter,
             QString("%1 • %2").arg(m_veri.ulke, m_veri.sehir));

  p.setPen(QColor(255, 255, 255));
  p.setFont(QFont("Segoe UI", 12, QFont::Bold));
  p.drawText(QRectF(22, 34, 190, 22), Qt::AlignLeft | Qt::AlignVCenter,
             m_veri.seciliYer);

  // 4. Merkez Hero Kartı (Canlı Saat & Kalan Süre)
  QRectF heroRect(12, 72, 282, 88);
  p.setPen(QPen(QColor(255, 255, 255, 80), 1.2));
  p.setBrush(QColor(15, 23, 42, 160));
  p.drawRoundedRect(heroRect, 10, 10);

  // Canlı Saat ve Aktif Vakit Rozeti
  p.setPen(QColor(56, 189, 248));
  p.setFont(QFont("Segoe UI", 9, QFont::Bold));
  QString aktifBaslik = localizeVakitUpper(m_veri.aktifVakit);
  p.drawText(QRectF(24, 82, 150, 18), Qt::AlignLeft | Qt::AlignVCenter,
             QString("✦ %1 ✦").arg(aktifBaslik));

  p.setPen(QColor(255, 255, 255));
  p.setFont(QFont("Segoe UI", 18, QFont::Bold));
  QString saatGosterim = m_veri.saat;
  if (saatGosterim.split(":").size() < 3) {
    saatGosterim = QTime::currentTime().toString("hh:mm:ss");
  }
  p.drawText(QRectF(24, 102, 150, 30), Qt::AlignLeft | Qt::AlignVCenter,
             saatGosterim);

  // Sağ Taraf: Geri Sayım
  QString sonrakiEtiket =
      sonrakiVakitEtiketiAl(m_veri.aktifVakit, m_veri.sonrakiVakit);

  p.setPen(QColor(203, 213, 225));
  p.setFont(QFont("Segoe UI", 8));
  p.drawText(QRectF(140, 82, 142, 18), Qt::AlignRight | Qt::AlignVCenter,
             sonrakiEtiket);

  p.setPen(QColor(251, 191, 36));
  p.setFont(QFont("Segoe UI", 17, QFont::Bold));
  p.drawText(QRectF(140, 102, 142, 30), Qt::AlignRight | Qt::AlignVCenter,
             m_veri.kalanSure);

  p.setPen(QColor(226, 232, 240, 180));
  p.setFont(QFont("Segoe UI", 7));
  p.drawText(QRectF(24, 136, 258, 16), Qt::AlignCenter,
             QString("%1  •  %2").arg(m_veri.miladi, m_veri.hicri));

  // 5. Alt 6 Vakit Kartları (2x3 Izgara)
  struct MiniKart {
    QString ad;
    QString saat;
    QString key;
  };
  QVector<MiniKart> kartlar = {
      {Localization::tr("imsak"), m_veri.vakitler.value("imsak", "--:--"), "imsak"},
      {Localization::tr("gunes"), m_veri.vakitler.value("gunes", "--:--"), "gunes"},
      {Localization::tr("ogle"), m_veri.vakitler.value("ogle", "--:--"), "ogle"},
      {Localization::tr("ikindi"), m_veri.vakitler.value("ikindi", "--:--"), "ikindi"},
      {Localization::tr("aksam"), m_veri.vakitler.value("aksam", "--:--"), "aksam"},
      {Localization::tr("yatsi"), m_veri.vakitler.value("yatsi", "--:--"), "yatsi"}};

  double startX = 12.0;
  double startY = 176.0;
  double cardW = 89.0;
  double cardH = 82.0;
  double gapX = 7.0;
  double gapY = 10.0;

  for (int idx = 0; idx < kartlar.size(); ++idx) {
    int row = idx / 3;
    int col = idx % 3;
    double kx = startX + col * (cardW + gapX);
    double ky = startY + row * (cardH + gapY);
    QRectF cardRect(kx, ky, cardW, cardH);

    bool aktifMi = (kartlar[idx].key == m_veri.aktifVakit);
    if (aktifMi) {
      double pulse = (std::sin(m_animFaz) + 1.0) * 0.5;
      int borderAlpha = static_cast<int>(180 + 75 * pulse);
      p.setPen(QPen(QColor(251, 191, 36, borderAlpha), 2.0));
      p.setBrush(QColor(15, 23, 42, 220));
    } else {
      p.setPen(QPen(QColor(255, 255, 255, 45), 1.0));
      p.setBrush(QColor(15, 23, 42, 140));
    }

    p.drawRoundedRect(cardRect, 10, 10);

    // Vakit Adı
    p.setPen(aktifMi ? QColor(251, 191, 36) : QColor(203, 213, 225));
    p.setFont(QFont("Segoe UI", 9, aktifMi ? QFont::Bold : QFont::Normal));
    p.drawText(QRectF(kx + 4, ky + 12, cardW - 8, 20), Qt::AlignCenter,
               kartlar[idx].ad);

    // Saat
    p.setPen(QColor(255, 255, 255));
    p.setFont(QFont("Segoe UI", 14, QFont::Bold));
    p.drawText(QRectF(kx + 4, ky + 36, cardW - 8, 28), Qt::AlignCenter,
               kartlar[idx].saat);

    if (aktifMi) {
      p.setPen(Qt::NoPen);
      p.setBrush(QColor(251, 191, 36));
      p.drawEllipse(QPointF(kx + cardW / 2.0, ky + 70), 3.0, 3.0);
    }
  }
}
