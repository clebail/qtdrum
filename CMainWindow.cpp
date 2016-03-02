#include <QtDebug>
#include <QFontDatabase>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
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
    font.setBold(true);

    lbTimer->setFont(font);
    lbRealTime->setFont(font);

    midiout = new RtMidiOut();

    midiPort = getMidiOutputPort();

    for(int i=0;i<midiPort.length();i++) {
        cbMidiPort->addItem(midiPort[i]);
    }

    drumWidget->addPads(&pads);

    playing = false;
    curTemps = 1;
    realTime = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    startTimer = new QTimer(this);
    connect(startTimer, SIGNAL(timeout()), this, SLOT(onStartTimer()));

    realTimeTimer = new QTimer(this);
    realTimeTimer->setInterval(1000);
    connect(realTimeTimer, SIGNAL(timeout()), this, SLOT(onRealTimeTimer()));

    isOpenFileUnsaved = false;
    setOpenFileName("unamed.qdr", "unamed.qdr");

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
        realTime = 0;
        startTimer->setInterval(60000 / spTempo->value() / 4);
        startTimer->start();
    }else {
        startTimer->stop();
        timer->stop();
        realTimeTimer->stop();

        lbTimer->setText("0");
        lbRealTime->setText("00:00");
    }
}

void CMainWindow::on_cbMidiPort_currentIndexChanged(int) {
    initDrumKit();
}

void CMainWindow::onStartTimer(void) {
    if(startTimerValue % 4 == 0) {
        if(startTimerValue == 16) {
            realTimeTimer->start();
            startTimer->stop();

            return;
        }
        playNote(42);
    } else {
        if(startTimerValue == 15) {
            timer->setInterval(60000 / spTempo->value() / 4);
            timer->start();
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

void CMainWindow::onRealTimeTimer(void) {
    int sec, min;

    realTime++;

    sec = realTime % 60;
    min = realTime / 60;

    lbRealTime->setText(QString("%1").arg(min, 2, 10, QChar('0'))+":"+QString("%1").arg(sec, 2, 10, QChar('0')));
}

void CMainWindow::on_actNewFile_triggered(bool) {
    if(!isOpenFileUnsaved || QMessageBox::question(this, tr("Confirmation"), tr("Current file not save, create new ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        isOpenFileUnsaved = false;
        setOpenFileName("unamed.qdr", "unamed.qdr");

        drumWidget->clear();
    }
}

void CMainWindow::on_actOpenFile_triggered(bool) {
    if(!isOpenFileUnsaved || QMessageBox::question(this, tr("Confirmation"), tr("Current file not save, open other ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open qtdrum file"), QString(), tr("qtdum file (*.qdr)"));

        if(!fileName.isEmpty()) {
            QFile file(fileName);

            if(file.open(QIODevice::ReadOnly)) {
                QTextStream stream(&file);
                QString fileContent;

                fileContent = stream.readAll();

                if(loadFile(fileContent)) {
                    QFileInfo fileInfo(file);

                    isOpenFileUnsaved = false;
                    setOpenFileName(fileInfo.fileName(), fullOpenFileName);
                }

                 file.close();
            }else {
                QMessageBox::critical(this, tr("Error"), tr("Unable to load file"));
                return;
            }
        }
    }
}

void CMainWindow::on_actSave_triggered(bool) {
    if(saveFile(fullOpenFileName)) {
        isOpenFileUnsaved = false;
    }
}

void CMainWindow::on_actSaveAs_triggered(bool) {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open qtdrum file"), fullOpenFileName, tr("qtdum file (*.qdr)"), 0, QFileDialog::DontConfirmOverwrite);

    if(!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);

        if(fileInfo.completeSuffix().isEmpty()) {
            fileName += ".qdr";
            fileInfo = QFileInfo(fileName);
        }

        if(saveFile(fileName)) {
            isOpenFileUnsaved = false;
            setOpenFileName(fileInfo.fileName(), fileName);
        }
    }
}

void CMainWindow::on_actQuit_triggered(bool) {
    this->close();
}

void CMainWindow::on_drumWidget_edit(const SPad&, const QByteArray&, int) {
    isOpenFileUnsaved = true;
}

void CMainWindow::on_spTempo_valueChanged(int) {
    isOpenFileUnsaved = true;
}

void CMainWindow::closeEvent(QCloseEvent *event) {
    if(!isOpenFileUnsaved || QMessageBox::question(this, tr("Confirmation"), tr("Current file not save, quit ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        event->accept();
    }else {
        event->ignore();
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
    message.push_back(127);
    midiout->sendMessage(&message);
}

void CMainWindow::stopNote(char note) {
    std::vector<unsigned char> message;

    message.push_back(138);
    message.push_back(note);
    message.push_back(0);
    midiout->sendMessage(&message);
}

void CMainWindow::setOpenFileName(QString openFileName, QString fullOpenFileName) {
    this->openFileName = openFileName;
    this->fullOpenFileName = fullOpenFileName;

    actSave->setText(tr("Save ")+openFileName);
}

QString CMainWindow::createFileContent(void) {
    QString fileContent = "";
    QList<QByteArray> matrice = drumWidget->getMatrices();

    fileContent += "Tempo: "+QString::number(spTempo->value())+"\n";
    for(int i=0;i<pads.size();i++) {
        fileContent += QString::number(pads[i].note)+": "+QString(matrice[i])+"\n";
    }

    return fileContent;
}

bool CMainWindow::loadFile(QString fileContent) {
    int tempo;

    if(fileContent.indexOf("Tempo: ") == 0) {
        int i;
        QList<SPad> newPads;

        tempo = fileContent.mid(7, i = fileContent.indexOf("\n")-7).toInt();

        while(i<fileContent.size()) {
            int posCR = fileContent.indexOf("\n", i);
            QString line = (posCR == -1 ? fileContent.mid(i) : fileContent.mid(i, posCR-i));
            QStringList elts = line.split(": ");

            if(elts.size() == 2) {
                newPads.append(SPad("", elts[0].toInt(), elts[1].toAscii()));
            }else {
                return false;
            }

            i = (posCR == -1 ? fileContent.size() : posCR + 1);
        }

        drumWidget->clear();
        spTempo->setValue(tempo);

        for(i=0;i<newPads.size();i++) {
            drumWidget->setMatriceRow(newPads[i].note, newPads[i].map);
        }

        drumWidget->repaint();

        return true;
    }

    return false;
}

bool CMainWindow::saveFile(QString fileName) {
    QFile file(fileName);

    if(file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);

        stream << createFileContent();

        file.close();

        return true;
    }

    QMessageBox::critical(this, tr("Error"), tr("Unable to save file"));

    return false;
}
