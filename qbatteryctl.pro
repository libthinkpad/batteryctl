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
    main.cpp \
    core/storage.cpp \
    core/battery.cpp \
    ui/batteryicon.cpp \
    ui/chargethreshold.cpp \
    ui/mainwindow.cpp \
    ui/thinkpads_org_about.cpp

HEADERS  += \
    core/storage.h \
    core/battery.h \
    ui/chargethreshold.h \
    ui/mainwindow.h \
    ui/batteryicon.h \
    ui/thinkpads_org_about.h

FORMS    += \
    ui/chargethreshold.ui \
    ui/mainwindow.ui \
    ui/thinkpads_org_about.ui

RESOURCES += \
    resources.qrc \
