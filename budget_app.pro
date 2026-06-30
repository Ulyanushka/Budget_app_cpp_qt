lessThan(QT_MAJOR_VERSION, 6): error("This project requires Qt 6 (found Qt $$QT_VERSION)")

QT += core gui widgets

CONFIG += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    $$PWD/*.cpp \
    $$PWD/db/*.cpp

HEADERS += \
    $$PWD/*.h \
    $$PWD/db/*.h

FORMS += \
    $$PWD/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
