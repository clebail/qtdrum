#include <QtDebug>
#include <QFontDatabase>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QKeyEvent>
#include "CMainWindow.h"
#include "CDrumKit.h"

#define ONE_MINUTE              ((int)60000)
#define ONE_SECOND              ((int)1000)

static QList<SPad> emptyList(void) {
    QList<SPad> list;
    return list;
}

static void timerHandler(union sigval sigval) {
    STimerParams *timerParams = (STimerParams *)sigval.sival_ptr;

    if(timerParams->timerValue < timerParams->nbTemps) {
        if(timerParams->timerValue % timerParams->nbDiv == 0) {
            CDrumKit::getInstance()->playNote(42);
        }
    }else {
        QList<QByteArray> matrices = timerParams->drumWidget->getMatrices();

        timerParams->lbTimer->setText(QString::number(((timerParams->curTemps - 1) / timerParams->nbDiv) + 1));

        for(int i=0;i<matrices.length();i++) {
            int tps = timerParams->curTemps - 1;
            QByteArray map = matrices.at(i);

            if(map.at(tps) == '1') {
                CDrumKit::getInstance()->playNote(timerParams->pads->at(i).note);
            }
        }

        timerParams->curTemps++;
        if(timerParams->curTemps > timerParams->nbTemps) {
            timerParams->curTemps = 1;
        }
    }

   timerParams->timerValue++;
}

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent) {
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

    QFont afficheurfont = getFont(":/qtdrum/resources/fonts/DS-DIGI.TTF");

    afficheurfont.setPixelSize(30);
    afficheurfont.setBold(true);

    lbTimer->setFont(afficheurfont);
    lbRealTime->setFont(afficheurfont);

    spNbBeat->setMinimum(MIN_BEAT);
    spNbBeat->setMaximum(MAX_BEAT);
    spNbDiv->setMinimum(MIN_DIV);
    spNbDiv->setMaximum(MAX_DIV);

    cbMidiPort->addItems(CDrumKit::getInstance()->getMidiPorts());

    drumWidget->addPads(&pads);

    playing = false;
    timerParams.curTemps = 1;
    timerParams.drumWidget = drumWidget;
    realTime = 0;

    realTimeTimer = new QTimer(this);
    realTimeTimer->setInterval(ONE_SECOND);
    connect(realTimeTimer, SIGNAL(timeout()), this, SLOT(onRealTimeTimer()));

    isOpenFileUnsaved = false;
    setOpenFileName("unamed.qdr", "unamed.qdr");

    CDrumKit::getInstance()->init(cbMidiPort->currentIndex());
}

CMainWindow::~CMainWindow() {
}

void CMainWindow::on_pbPlayPause_clicked(bool) {
    playing = !playing;

    if(playing) {
        timerParams.curTemps = 1;
        timerParams.timerValue = 0;
        realTime = 0;
        timerParams.nbBeat = spNbBeat->value();
        timerParams.nbDiv = spNbDiv->value();
        timerParams.nbTemps = timerParams.nbBeat * timerParams.nbDiv;
        timerParams.pads = &pads;
        timerParams.lbTimer = lbTimer;

        enableControls(false);

        pbPlayPause->setIcon(QIcon(":/qtdrum/resources/images/stop.png"));

        startPOSIXTimer(ONE_MINUTE / spTempo->value() / timerParams.nbDiv);

        realTimeTimer->start();
    }else {
        realTimeTimer->stop();

        lbTimer->setText("0");
        lbRealTime->setText("00:00");

        enableControls(true);

        pbPlayPause->setIcon(QIcon(":/qtdrum/resources/images/play.png"));

        stopPOSIXTimer();
    }
}

void CMainWindow::on_cbMidiPort_currentIndexChanged(int value) {
    CDrumKit::getInstance()->init(value);
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

void CMainWindow::on_spNbBeat_valueChanged(int value) {
    drumWidget->setNbBeat(value);
    isOpenFileUnsaved = true;
}

void CMainWindow::on_spNbDiv_valueChanged(int value) {
    drumWidget->setNbDivPerBeat(value);
    isOpenFileUnsaved = true;
}

void CMainWindow::closeEvent(QCloseEvent *event) {
    if(!isOpenFileUnsaved || QMessageBox::question(this, tr("Confirmation"), tr("Current file not save, quit ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        event->accept();
    }else {
        event->ignore();
    }
}

void CMainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Return) {
        on_pbPlayPause_clicked();
    }

    event->accept();
}

void CMainWindow::setOpenFileName(QString openFileName, QString fullOpenFileName) {
    this->openFileName = openFileName;
    this->fullOpenFileName = fullOpenFileName;

    actSave->setText(tr("Save ")+openFileName);
}

QString CMainWindow::createFileContent(void) {
    QString fileContent = "";
    QList<QByteArray> matrice = drumWidget->getMatrices();

    fileContent += "Params: "+QString::number(spTempo->value())+","+QString::number(spNbBeat->value())+","+QString::number(spNbDiv->value())+"\n";
    for(int i=0;i<pads.size();i++) {
        fileContent += QString::number(pads[i].note)+": "+QString(matrice[i])+"\n";
    }

    return fileContent;
}

bool CMainWindow::loadFile(QString fileContent) {
    int tempo, nbBeat, nbDiv;

    if(fileContent.indexOf("Params: ") == 0) {
        int paramsSize = QString("Params: ").size();
        int i = fileContent.indexOf("\n") +1;
        QList<SPad> newPads;

        QStringList params = fileContent.mid(paramsSize, i - paramsSize - 1).split(",");

        tempo = params[0].toInt();
        nbBeat = params[1].toInt();
        nbDiv = params[2].toInt();

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
        spNbBeat->setValue(nbBeat);
        spNbDiv->setValue(nbDiv);

        drumWidget->setNbBeat(nbBeat);
        drumWidget->setNbDivPerBeat(nbDiv);

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

QFont CMainWindow::getFont(QString resourceName) {
    int id = QFontDatabase::addApplicationFont(resourceName);
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    return QFont(family);
}

bool CMainWindow::startPOSIXTimer(int intervalMS) {
    long intervalNS = ((long)intervalMS) * 1000000L;
    struct itimerspec itimer = { { 0,  intervalNS }, { 0, intervalNS } };
    struct sigevent sigev;

    memset (&sigev, 0, sizeof (struct sigevent));
    sigev.sigev_value.sival_ptr = (void *)&timerParams;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_notify_attributes = NULL;
    sigev.sigev_notify_function = timerHandler;

    if(timer_create (CLOCK_REALTIME, &sigev, &posixTimer) == 0) {
        if(timer_settime (posixTimer, 0, &itimer, NULL) == 0) {
            return true;
        }
    }

    return false;
}

void CMainWindow::stopPOSIXTimer(void) {
    timer_delete(posixTimer);
}

void CMainWindow::enableControls(bool enable) {
    cbMidiPort->setEnabled(enable);
    spTempo->setEnabled(enable);
    spNbBeat->setEnabled(enable);
    spNbDiv->setEnabled(enable);
}
