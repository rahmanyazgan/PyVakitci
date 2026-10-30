QT       += core gui widgets network sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = PyVakitci
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/VakitGorselleri.cpp \
    src/HavaDurumu.cpp \
    src/Vakitler.cpp \
    src/Calverter.cpp \
    src/DiyanetUlkeler.cpp \
    src/Temalar.cpp \
    src/Clever.cpp

HEADERS += \
    src/MainWindow.h \
    src/VakitGorselleri.h \
    src/HavaDurumu.h \
    src/Vakitler.h \
    src/Calverter.h \
    src/DiyanetUlkeler.h \
    src/Temalar.h \
    src/Clever.h

RESOURCES += \
    resources/resources.qrc

RC_ICONS = resources/cami.ico

win32: LIBS += -luser32

