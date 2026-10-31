#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <QString>
#include <QStringList>
#include <QMap>

class Localization {
public:
    static void init();
    static QString currentLanguage();
    static void setLanguage(const QString& lang);
    static QString tr(const QString& key);
    static QString getLicensePath(const QString& baseDir);
    static QString detectSystemLanguage();
    static QStringList supportedLanguages();
    static QString languageDisplayName(const QString& lang);
    static QString formatCurrentPrayer(const QString& prayerKey);
    static QString formatRemainingTime(const QString& prayerKey);
    static QString formatRemainingTo(const QString& prayerKey);
    static QString dayName(int dayOfWeek);
    static QString weatherDescription(int code);

private:
    static QString s_currentLanguage;
    static QMap<QString, QMap<QString, QString>> s_translations;
};

#endif // LOCALIZATION_H

