#ifndef TEMALAR_H
#define TEMALAR_H

#include <QString>
#include <QStringList>
#include <QMap>

struct TemaVeri {
    QString ad;
    QString qss;
};

class Temalar {
public:
    static QStringList getTemaAnahtarlari();
    static const QMap<QString, TemaVeri>& getTemalar();
    static QString getOrtakQss();
    static QString getTemaQss(const QString& temaKey);
};

#endif // TEMALAR_H
