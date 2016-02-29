#include <QObject>
#include <QtDebug>
#include "CMainWindow.h"

static QList<SPad> emptyList(void) {
    QList<SPad> list;
    return list;
}

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent) {
    QStringList midiPort;
    QList<SPad> pads = emptyList()  << SPad(QString::fromUtf8("Bass Drum 2"), 35)
                                    << SPad(QString::fromUtf8("Bass Drum 1"), 36)
                                    << SPad(QString::fromUtf8("Side Stick/Rimshot"), 37)
                                    << SPad(QString::fromUtf8("Snare Drum 1"), 38)
                                    << SPad(QString::fromUtf8("Hand Clap"), 39)
                                    << SPad(QString::fromUtf8("Snare Drum 2"), 40)
                                    << SPad(QString::fromUtf8("Low Tom 2"), 41)
                                    << SPad(QString::fromUtf8("Closed Hi-hat"), 42)
                                    << SPad(QString::fromUtf8("Low Tom 1"), 43)
                                    << SPad(QString::fromUtf8("Pedal Hi-hat"), 44)
                                    << SPad(QString::fromUtf8("Mid Tom 2"), 45)
                                    << SPad(QString::fromUtf8("Open Hi-hat"), 46)
                                    << SPad(QString::fromUtf8("Mid Tom 1"), 47)
                                    << SPad(QString::fromUtf8("High Tom 2"), 48)
                                    << SPad(QString::fromUtf8("Crash Cymbal 1"), 49)
                                    << SPad(QString::fromUtf8("High Tom 1"), 50)
                                    << SPad(QString::fromUtf8("Ride Cymbal 1"), 51)
                                    << SPad(QString::fromUtf8("Splash Cymbal"), 55)
                                    << SPad(QString::fromUtf8("Crash Cymbal 2"), 57)
                                    << SPad(QString::fromUtf8("Ride Cymbal 2"), 59)
                                    ;

    setupUi(this);

    midiout = new RtMidiOut();

    midiPort = getMidiOutputPort();

    for(int i=0;i<midiPort.length();i++) {
        cbMidiPort->addItem(midiPort[i]);
    }

    drumWidget->addPads(&pads);

    playing = false;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

CMainWindow::~CMainWindow() {
    delete midiout;
}

QStringList CMainWindow::getMidiOutputPort(void) {
    QStringList result;
    unsigned nPorts = midiout->getPortCount();

    for(unsigned i=0;i<nPorts;i++) {
        result << QString::fromStdString(midiout->getPortName(i));
    }

    return result;
}

void CMainWindow::on_pbPlayPause_clicked(bool) {
    playing = !playing;

    if(playing) {
        timer->setInterval(60000 / spTempo->value());
        timer->start();
    }else {
        timer->stop();
    }
}

void CMainWindow::onTimer(void) {
    qDebug() << "tps";
}
