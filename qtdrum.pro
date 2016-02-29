#-------------------------------------------------
#
# Project created by QtCreator 2016-02-29T10:04:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtdrum
TEMPLATE = app
LIBS += -lrtmidi -ljack -lpthread

SOURCES += main.cpp\
        CMainWindow.cpp \
    CDrumWidget.cpp

HEADERS  += CMainWindow.h \
    CDrumWidget.h

FORMS    += CMainWindow.ui

RESOURCES += \
    qtdrum.qrc