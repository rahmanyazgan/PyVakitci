#ifndef DIYANETULKELER_H
#define DIYANETULKELER_H

#include <QString>
#include <QMap>
#include <QSettings>

struct IlceBilgiPaketi {
    QMap<QString, int> ilceler;
    QMap<QString, QString> linkler;
};

class DiyanetUlkeler {
public:
    static const QString BASE_URL;
    static const QString REQUEST_URL;

    static const QMap<QString, int>& getUlkeler();
    static const QMap<QString, int>& getTurkiyeSehirler();

    static QMap<QString, int> sehirleriAl(const QString& ulke, QSettings* settings = nullptr);
    static IlceBilgiPaketi ilceleriAl(const QMap<QString, int>& sehirler, const QString& ulke, const QString& sehir, QSettings* settings = nullptr);
};

#endif // DIYANETULKELER_H
