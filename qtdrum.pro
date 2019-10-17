#-------------------------------------------------
#
# Project created by QtCreator 2016-02-29T10:04:54
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtdrum
TEMPLATE = app
INCLUDEPATH += /usr/include/rtmidi /usr/include/alsa
LIBS += -lrtmidi -ljack -lpthread -lrt

SOURCES += main.cpp\
        CMainWindow.cpp \
    CDrumWidget.cpp \
    CDrumKit.cpp \
   	CSevenSeg.cpp \
	CTimeAffich.cpp \ 
    COptionsDialog.cpp


HEADERS  += CMainWindow.h \
    CDrumWidget.h \
    CDrumKit.h \
    CSevenSeg.h \
	CTimeAffich.h \
    COptionsDialog.h

FORMS    += CMainWindow.ui \
    COptionsDialog.ui

TRANSLATIONS += qtdrum_fr.ts

RESOURCES += \
    qtdrum.qrc
