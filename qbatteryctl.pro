#-------------------------------------------------
#
# Project created by QtCreator 2017-12-10T17:50:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qbatteryctl
TEMPLATE = app

SOURCES += \
    batteryicon.cpp \
    main.cpp \
    battery.cpp \
    chargethreshold.cpp \
    storage.cpp \
    mainwindow.cpp \
    thinkpads_org_about.cpp

HEADERS  += \
    batteryicon.h \
    battery.h \
    chargethreshold.h \
    storage.h \
    mainwindow.h \
    thinkpads_org_about.h

FORMS    += \
    chargethreshold.ui \
    mainwindow.ui \
    thinkpads_org_about.ui

RESOURCES += \
    resources.qrc
