cmake_minimum_required(VERSION 3.16)

project(PyVakitci VERSION 1.8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Sql Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Sql Multimedia)

set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/VakitGorselleri.h
    src/VakitGorselleri.cpp
    src/HavaDurumu.h
    src/HavaDurumu.cpp
    src/Vakitler.h
    src/Vakitler.cpp
    src/Calverter.h
    src/Calverter.cpp
    src/DiyanetUlkeler.h
    src/DiyanetUlkeler.cpp
    src/Temalar.h
    src/Temalar.cpp
    src/Clever.h
    src/Clever.cpp
    src/HttpClient.h
    src/HttpClient.cpp
    src/Dialogs.h
    src/Dialogs.cpp
    resources/resources.qrc
    resources/app.rc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PyVakitci
        WIN32
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(PyVakitci
        WIN32
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(PyVakitci PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Multimedia
)

if(WIN32)
    target_link_libraries(PyVakitci PRIVATE user32 wininet)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(PyVakitci)
endif()

