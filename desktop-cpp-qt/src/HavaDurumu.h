#ifndef HAVADURUMU_H
#define HAVADURUMU_H

#include <QWidget>
#include <QPushButton>
#include <QThread>
#include <QVector>
#include <QMap>
#include <QPair>
#include <QMetaType>

struct GunlukTahmin {
    QString gun;
    QString durum;
    QString tip;
    int tMax = 0;
    int tMin = 0;
    int yagis = 0;
};

struct HavaVerisi {
    int sicaklik = 0;
    int hissedilen = 0;
    int nem = 0;
    int ruzgar = 0;
    bool gunduzMu = true;
    QString durum;
    QString tip;
    QVector<GunlukTahmin> gunluk;
    QString guncellemeSaati;
    bool gecerli = false;
};
Q_DECLARE_METATYPE(HavaVerisi)

class HavaDurumuThread : public QThread {
    Q_OBJECT
public:
    HavaDurumuThread(const QString& ulke, const QString& sehir, const QString& ilce, QObject* parent = nullptr);

signals:
    void veriHazir(const HavaVerisi& veri);
    void hataOlustu(const QString& hata);

protected:
    void run() override;

private:
    QString m_ulke;
    QString m_sehir;
    QString m_ilce;

    QPair<double, double> koordinatBul() const;
};

class HavaDurumuWidget : public QWidget {
    Q_OBJECT
public:
    explicit HavaDurumuWidget(QWidget* parent = nullptr);
    ~HavaDurumuWidget();

    void konumAyarla(const QString& ulke, const QString& sehir, const QString& ilce, const QString& tema = "varsayilan", bool zorla = false);
    void temaAyarla(const QString& tema);
    void animasyonAdimi();

public slots:
    void havaDurumunuGuncelle(bool zorla = false);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onVeriHazir(const HavaVerisi& veri);
    void onHata(const QString& hata);

private:
    QPushButton* m_yenileBtn = nullptr;
    HavaDurumuThread* m_aktifThread = nullptr;
    HavaVerisi m_veri;
    bool m_yukleniyor = true;
    QString m_hataMesaji;
    QString m_ulke = "TÜRKİYE";
    QString m_sehir = "İSTANBUL";
    QString m_ilce = "ESENYURT";
    QString m_tema = "varsayilan";
    double m_animFaz = 0.0;
    QString m_sonSorguAnahtari;

    void bulutCiz(QPainter& p, double x, double y, const QColor& renk);
    void havaIkonuCiz(QPainter& p, double cx, double cy, double olcek, const QString& tip, bool gunduzMu);
};

#endif // HAVADURUMU_H
