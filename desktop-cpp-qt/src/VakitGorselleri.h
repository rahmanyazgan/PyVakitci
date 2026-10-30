#ifndef VAKITGORSELLERI_H
#define VAKITGORSELLERI_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <QVector>
#include <QPointF>

struct VakitVeriPaketi {
    QString ulke = "TÜRKİYE";
    QString sehir = "İSTANBUL";
    QString seciliYer = "ESENYURT";
    QString miladi;
    QString hicri;
    QString diniGun;
    QString saat = "00:00:00";
    QString aktifVakit = "imsak";
    QString sonrakiVakit;
    QString kalanSure = "00:00:00";
    QMap<QString, QString> vakitler;
    QString tema = "varsayilan";
};

struct YildizNoktasi {
    double x;
    double y;
    double r;
    double phase;
};

class VakitSaatiWidget : public QWidget {
    Q_OBJECT
public:
    explicit VakitSaatiWidget(QWidget* parent = nullptr);

    void veriGuncelle(const VakitVeriPaketi& veri);
    void animasyonAdimi();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    VakitVeriPaketi m_veri;
    double m_animFaz = 0.0;
};

class GokyuzuWidget : public QWidget {
    Q_OBJECT
public:
    explicit GokyuzuWidget(QWidget* parent = nullptr);

    void veriGuncelle(const VakitVeriPaketi& veri);
    void animasyonAdimi();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    VakitVeriPaketi m_veri;
    double m_animFaz = 0.0;
    QVector<YildizNoktasi> m_yildizlar;

    void yildizlariOlustur();
};

#endif // VAKITGORSELLERI_H
