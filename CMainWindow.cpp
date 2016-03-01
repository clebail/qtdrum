#include <QtDebug>
#include <QFontDatabase>
#include "CMainWindow.h"

static QList<SPad> emptyList(void) {
    QList<SPad> list;
    return list;
}

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent) {
    QStringList midiPort;
    pads = emptyList()  << SPad(QString::fromUtf8("Bass Drum 2"), 35, QByteArray("1010000010000000"))
                        << SPad(QString::fromUtf8("Bass Drum 1"), 36, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Side Stick/Rimshot"), 37, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Snare Drum 1"), 38, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Hand Clap"), 39, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Snare Drum 2"), 40, QByteArray("0000100000001000"))
                        << SPad(QString::fromUtf8("Low Tom 2"), 41, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Closed Hi-hat"), 42, QByteArray("1010101010101000"))
                        << SPad(QString::fromUtf8("Low Tom 1"), 43, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Pedal Hi-hat"), 44, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Mid Tom 2"), 45, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Open Hi-hat"), 46, QByteArray("0000000000000010"))
                        << SPad(QString::fromUtf8("Mid Tom 1"), 47, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("High Tom 2"), 48, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Crash Cymbal 1"), 49, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("High Tom 1"), 50, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Ride Cymbal 1"), 51, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Splash Cymbal"), 55, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Crash Cymbal 2"), 57, QByteArray("0000000000000000"))
                        << SPad(QString::fromUtf8("Ride Cymbal 2"), 59, QByteArray("0000000000000000"))
                        ;
    setupUi(this);

    int id = QFontDatabase::addApplicationFont(":/qtdrum/resources/fonts/DS-DIGI.TTF");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont font(family);

    font.setPixelSize(30);
    font.setItalic(true);
    font.setBold(true);

    lbTimer->setFont(font);

    midiout = new RtMidiOut();

    midiPort = getMidiOutputPort();

    for(int i=0;i<midiPort.length();i++) {
        cbMidiPort->addItem(midiPort[i]);
    }

    drumWidget->addPads(&pads);

    playing = false;
    curTemps = 1;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    startTimer = new QTimer(this);
    connect(startTimer, SIGNAL(timeout()), this, SLOT(onStartTimer()));

    initDrumKit();
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
        curTemps = 1;
        startTimerValue = 0;
        startTimer->setInterval(60000 / spTempo->value() / 4);
        startTimer->start();
    }else {
        startTimer->stop();
        timer->stop();

        lbTimer->setText("0");
    }
}

void CMainWindow::on_cbMidiPort_currentIndexChanged(int) {
    initDrumKit();
}

void CMainWindow::onStartTimer(void) {
    if(startTimerValue % 4 == 0) {
        playNote(42);
    } else {
        if(startTimerValue == 15) {
            timer->setInterval(60000 / spTempo->value() / 4);
            timer->start();

            startTimer->stop();
        }
    }

    startTimerValue++;
}

void CMainWindow::onTimer(void) {
    QList<QByteArray> matrices = drumWidget->getMatrices();

    lbTimer->setText(QString::number(((curTemps - 1) / 4) + 1));

    for(int i=0;i<matrices.length();i++) {
        int tps = curTemps - 1;
        QByteArray map = matrices.at(i);

        if(map.at(tps) == '1') {
            playNote(pads.at(i).note);
        }

    }

    curTemps++;
    if(curTemps > drumWidget->getNbTemps()) {
        curTemps = 1;
    }
}

void CMainWindow::initDrumKit(void) {
    std::vector<unsigned char> message;

    if(midiout->isPortOpen()) {
        midiout->closePort();
    }
    midiout->openPort(cbMidiPort->currentIndex());

    message.push_back(192);
    message.push_back(5);
    midiout->sendMessage(&message);

    message[0] = 176;
	message[1] = 7;
    message.push_back(127);
    midiout->sendMessage(&message);
}

void CMainWindow::playNote(char note) {
    std::vector<unsigned char> message;

    message.push_back(153);
    message.push_back(note);
    message.push_back(64);
    midiout->sendMessage(&message);
}

void CMainWindow::stopNote(char note) {
    std::vector<unsigned char> message;

    message.push_back(138);
    message.push_back(note);
    message.push_back(0);
    midiout->sendMessage(&message);
}
