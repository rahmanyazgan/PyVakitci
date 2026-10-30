#include "Clever.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

Clever::Clever() {
}

void Clever::pauseSpotifyOrMedia() {
#ifdef Q_OS_WIN
    keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
#endif
}

void Clever::pauseWinamp() {
#ifdef Q_OS_WIN
    HWND hwnd = FindWindowW(L"Winamp v1.x", nullptr);
    if (hwnd) {
        SendMessageW(hwnd, WM_COMMAND, 40046, 0); // 40046: Pause/Unpause
    }
#endif
}

