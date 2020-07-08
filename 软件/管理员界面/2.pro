QT       += core gui serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    main.cpp \
    mywidget.cpp \
    carinfo.cpp \
    user_register.cpp

HEADERS += \
    mywidget.h \
    carinfo.h \
    user_register.h

FORMS += \
    mywidget.ui \
    carinfo.ui \
    user_register.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
