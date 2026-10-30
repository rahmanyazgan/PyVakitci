#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QByteArray>
#include <QString>

class HttpClient {
public:
    static QByteArray get(const QString& url, int timeoutMs = 8000, bool* ok = nullptr);
    static bool checkConnection(const QString& url, int timeoutMs = 3000);
};

#endif // HTTPCLIENT_H

