#ifndef VAKITLER_H
#define VAKITLER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QDate>
#include <QSqlDatabase>

class Vakitler : public QObject {
    Q_OBJECT
public:
    explicit Vakitler(const QString& dbYolu, QObject* parent = nullptr);
    ~Vakitler();

    bool veritabaniHazirla();
    bool gunlukVakitleriYukle(const QString& seciliYer, const QDate& tarih = QDate::currentDate());
    bool diyanetAylikVakitleriKaydet(const QString& seciliYer, const QString& link);

    QString imsak = "--:--";
    QString gunes = "--:--";
    QString ogle = "--:--";
    QString ikindi = "--:--";
    QString aksam = "--:--";
    QString yatsi = "--:--";

    bool veritabanindakiYerBilgisi = true;
    bool tarihHatasi = false;

    QString veritabaniYolu() const { return m_dbYolu; }
    QMap<QString, QString> asMap() const;

signals:
    void vakitlerGuncellendi();

private:
    QString m_dbYolu;
    QSqlDatabase m_db;

    static QString miladiTarihiFormatla(const QString& tarihStr);
};

#endif // VAKITLER_H
