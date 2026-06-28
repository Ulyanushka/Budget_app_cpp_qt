lessThan(QT_MAJOR_VERSION, 6): error("Tests require Qt 6 (found Qt $$QT_VERSION)")

QT += core gui widgets testlib
CONFIG += c++17 testcase
TEMPLATE = app

SOURCES += \
    tests_mainwindow.cpp \
    ../mainwindow.cpp

HEADERS += \
    tests_mainwindow.h \
    ../mainwindow.h

FORMS += $$PWD/../mainwindow.ui

INCLUDEPATH += .. .
