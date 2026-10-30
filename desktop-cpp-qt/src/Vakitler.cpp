#include "Vakitler.h"
#include "HttpClient.h"
#include "DiyanetUlkeler.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QRegularExpression>

Vakitler::Vakitler(const QString& dbYolu, QObject* parent)
    : QObject(parent), m_dbYolu(dbYolu) {
    veritabaniHazirla();
}

Vakitler::~Vakitler() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool Vakitler::veritabaniHazirla() {
    if (QSqlDatabase::contains("PyVakitciConnection")) {
        m_db = QSqlDatabase::database("PyVakitciConnection");
        if (m_db.databaseName() != m_dbYolu) {
            m_db.close();
            m_db.setDatabaseName(m_dbYolu);
        }
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "PyVakitciConnection");
        m_db.setDatabaseName(m_dbYolu);
    }

    if (!m_db.isOpen()) {
        if (!m_db.open()) {
            return false;
        }
    }

    QSqlQuery q(m_db);
    q.exec("CREATE TABLE IF NOT EXISTS namazvakitleri ("
           "id INTEGER PRIMARY KEY, sehir VARCHAR(30), tarih VARCHAR(10), "
           "imsak VARCHAR(5), gunes VARCHAR(5), ogle VARCHAR(5), "
           "ikindi VARCHAR(5), aksam VARCHAR(5), yatsi VARCHAR(5))");
    return true;
}

QMap<QString, QString> Vakitler::asMap() const {
    return {
        {"imsak", imsak},
        {"gunes", gunes},
        {"ogle", ogle},
        {"ikindi", ikindi},
        {"aksam", aksam},
        {"yatsi", yatsi}
    };
}

QString Vakitler::miladiTarihiFormatla(const QString& tarihStr) {
    static const QMap<QString, QString> aylar = {
        {"Ocak", "01"}, {"Şubat", "02"}, {"Mart", "03"}, {"Nisan", "04"},
        {"Mayıs", "05"}, {"Haziran", "06"}, {"Temmuz", "07"}, {"Ağustos", "08"},
        {"Eylül", "09"}, {"Ekim", "10"}, {"Kasım", "11"}, {"Aralık", "12"}
    };
    QStringList veri = tarihStr.trimmed().split(QRegularExpression("\\s+"));
    if (veri.size() >= 3 && aylar.contains(veri[1])) {
        int gunInt = veri[0].toInt();
        QString gun = QString("%1").arg(gunInt, 2, 10, QChar('0'));
        QString ay = aylar[veri[1]];
        QString yil = veri[2];
        return QString("%1.%2.%3").arg(gun, ay, yil);
    }
    QDate parsed = QDate::fromString(tarihStr.trimmed(), "dd.MM.yyyy");
    if (parsed.isValid()) {
        return parsed.toString("dd.MM.yyyy");
    }
    return QString();
}

bool Vakitler::gunlukVakitleriYukle(const QString& seciliYer, const QDate& tarih) {
    if (!m_db.isOpen() && !veritabaniHazirla()) return false;

    veritabanindakiYerBilgisi = true;
    tarihHatasi = false;

    QSqlQuery qYer(m_db);
    if (qYer.exec("SELECT sehir FROM namazvakitleri LIMIT 1") && qYer.next()) {
        QString kayitliSehir = qYer.value(0).toString();
        if (!seciliYer.isEmpty() && kayitliSehir != seciliYer) {
            veritabanindakiYerBilgisi = false;
            return false;
        }
    } else {
        veritabanindakiYerBilgisi = false;
        return false;
    }

    QString bugunStr = tarih.toString("dd.MM.yyyy");
    QSqlQuery q(m_db);
    q.prepare("SELECT imsak, gunes, ogle, ikindi, aksam, yatsi FROM namazvakitleri WHERE tarih = :tarih LIMIT 1");
    q.bindValue(":tarih", bugunStr);

    if (q.exec() && q.next()) {
        imsak = q.value(0).toString();
        gunes = q.value(1).toString();
        ogle = q.value(2).toString();
        ikindi = q.value(3).toString();
        aksam = q.value(4).toString();
        yatsi = q.value(5).toString();
        emit vakitlerGuncellendi();
        return true;
    }

    tarihHatasi = true;
    return false;
}

bool Vakitler::diyanetAylikVakitleriKaydet(const QString& seciliYer, const QString& link) {
    if (link.isEmpty() || seciliYer.isEmpty()) {
        return false;
    }

    QString url = DiyanetUlkeler::BASE_URL + link;
    bool ok = false;
    QByteArray raw = HttpClient::get(url, 10000, &ok);
    if (!ok || raw.isEmpty()) {
        return false;
    }

    QString html = QString::fromUtf8(raw);

    // <table class="table vakit-table"> tablosunu ayıkla
    int tblIdx = html.indexOf("vakit-table");
    if (tblIdx >= 0) {
        int endTbl = html.indexOf("</table>", tblIdx);
        if (endTbl > tblIdx) {
            html = html.mid(tblIdx, endTbl - tblIdx);
        }
    }

    static const QRegularExpression trRe("<tr[^>]*>(.*?)</tr>", QRegularExpression::DotMatchesEverythingOption);
    static const QRegularExpression tdRe("<td[^>]*>(.*?)</td>", QRegularExpression::DotMatchesEverythingOption);

    auto matchIt = trRe.globalMatch(html);

    if (!m_db.isOpen() && !veritabaniHazirla()) return false;

    m_db.transaction();
    QSqlQuery dropQ(m_db);
    dropQ.exec("DROP TABLE IF EXISTS namazvakitleri");
    dropQ.exec("CREATE TABLE IF NOT EXISTS namazvakitleri ("
               "id INTEGER PRIMARY KEY, sehir VARCHAR(30), tarih VARCHAR(10), "
               "imsak VARCHAR(5), gunes VARCHAR(5), ogle VARCHAR(5), "
               "ikindi VARCHAR(5), aksam VARCHAR(5), yatsi VARCHAR(5))");

    int idCounter = 1;
    while (matchIt.hasNext()) {
        auto m = matchIt.next();
        QString trContent = m.captured(1);
        auto tdIt = tdRe.globalMatch(trContent);
        QStringList hucreler;
        while (tdIt.hasNext()) {
            QString c = tdIt.next().captured(1);
            c.remove(QRegularExpression("<[^>]*>"));
            hucreler.append(c.trimmed());
        }

        // Diyanet tablosu: [0] Miladi Tarih, [1] Hicri Tarih, [2] İmsak, [3] Güneş, [4] Öğle, [5] İkindi, [6] Akşam, [7] Yatsı
        if (hucreler.size() >= 8) {
            QString formatliTarih = miladiTarihiFormatla(hucreler[0]);
            if (!formatliTarih.isEmpty()) {
                QSqlQuery insQ(m_db);
                insQ.prepare("INSERT INTO namazvakitleri VALUES (:id, :sehir, :tarih, :imsak, :gunes, :ogle, :ikindi, :aksam, :yatsi)");
                insQ.bindValue(":id", idCounter++);
                insQ.bindValue(":sehir", seciliYer);
                insQ.bindValue(":tarih", formatliTarih);
                insQ.bindValue(":imsak", hucreler[2]);
                insQ.bindValue(":gunes", hucreler[3]);
                insQ.bindValue(":ogle", hucreler[4]);
                insQ.bindValue(":ikindi", hucreler[5]);
                insQ.bindValue(":aksam", hucreler[6]);
                insQ.bindValue(":yatsi", hucreler[7]);
                insQ.exec();
            }
        } else if (hucreler.size() == 7) {
            QString formatliTarih = miladiTarihiFormatla(hucreler[0]);
            if (!formatliTarih.isEmpty()) {
                QSqlQuery insQ(m_db);
                insQ.prepare("INSERT INTO namazvakitleri VALUES (:id, :sehir, :tarih, :imsak, :gunes, :ogle, :ikindi, :aksam, :yatsi)");
                insQ.bindValue(":id", idCounter++);
                insQ.bindValue(":sehir", seciliYer);
                insQ.bindValue(":tarih", formatliTarih);
                insQ.bindValue(":imsak", hucreler[1]);
                insQ.bindValue(":gunes", hucreler[2]);
                insQ.bindValue(":ogle", hucreler[3]);
                insQ.bindValue(":ikindi", hucreler[4]);
                insQ.bindValue(":aksam", hucreler[5]);
                insQ.bindValue(":yatsi", hucreler[6]);
                insQ.exec();
            }
        }
    }
    m_db.commit();

    if (idCounter > 1) {
        veritabanindakiYerBilgisi = true;
        gunlukVakitleriYukle(seciliYer);
        return true;
    }
    return false;
}
