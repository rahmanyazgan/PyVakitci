#include "HttpClient.h"

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#endif

QByteArray HttpClient::get(const QString& url, int timeoutMs, bool* ok) {
    if (ok) *ok = false;
    QByteArray result;

#ifdef _WIN32
    HINTERNET hInternet = InternetOpenW(
        L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL,
        0
    );
    if (!hInternet) {
        return result;
    }

    DWORD timeout = static_cast<DWORD>(timeoutMs);
    InternetSetOptionW(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_KEEP_CONNECTION;
    if (url.startsWith("https://", Qt::CaseInsensitive)) {
        flags |= INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    }

    std::wstring wUrl = url.toStdWString();
    HINTERNET hUrl = InternetOpenUrlW(
        hInternet,
        wUrl.c_str(),
        L"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,application/json,*/*;q=0.8\r\nAccept-Language: tr-TR,tr;q=0.9,en-US;q=0.8,en;q=0.7\r\n",
        static_cast<DWORD>(-1L),
        flags,
        0
    );

    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return result;
    }

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    if (HttpQueryInfoW(hUrl, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusSize, NULL)) {
        if (statusCode >= 400) {
            InternetCloseHandle(hUrl);
            InternetCloseHandle(hInternet);
            return result;
        }
    }

    char buffer[8192];
    DWORD bytesRead = 0;
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        result.append(buffer, static_cast<int>(bytesRead));
    }

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    if (ok) *ok = !result.isEmpty();
#endif

    return result;
}

bool HttpClient::checkConnection(const QString& url, int timeoutMs) {
    bool ok = false;
    QByteArray data = get(url, timeoutMs, &ok);
    Q_UNUSED(data);
    return ok;
}

