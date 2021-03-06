#include <QtDebug>
#include <QFontDatabase>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QKeyEvent>
#include <QSound>
#include "CMainWindow.h"
#include "CDrumKit.h"
#include "COptionsDialog.h"

#define ONE_MINUTE              (static_cast<int>(60000))
#define ONE_SECOND              (static_cast<int>(1000))

static QList<SPad*> emptyList(void) {
    QList<SPad*> list;
    return list;
}

static void timerHandler(union sigval sigval) {
    CTimerParams *timerParams = static_cast<CTimerParams *>(sigval.sival_ptr);

    if(timerParams->timerValue < timerParams->nbTemps) {
        if(timerParams->timerValue % timerParams->nbDiv == 0) {
            CDrumKit::getInstance()->playNote(42);
        }
    }else {
        bool mute = false;
        QList<QByteArray> matrices = timerParams->drumWidget->getMatrices();

        timerParams->curMeasure+=(timerParams->curTemps+1 > timerParams->nbTemps ? 1 : 0);
        timerParams->emitTempsUpdate(((timerParams->curTemps - 1) / timerParams->nbDiv) + 1, timerParams->curTemps - 1);

        if(timerParams->nbMute != 0 && timerParams->nbMuteOver != 0) {
            int curMInBcl = timerParams->curMeasure % (timerParams->nbMuteOver + 1);

            if(curMInBcl > timerParams->nbMuteOver - timerParams->nbMute) {
                mute = true;
            }
        }

        if(!mute) {
            for(int i=0;i<matrices.length();i++) {
                int tps = timerParams->curTemps - 1;
                QByteArray map = matrices.at(i);

                if(map.at(tps) == '1' && !timerParams->pads->at(i)->mute) {
                    CDrumKit::getInstance()->playNote(static_cast<char>(timerParams->pads->at(i)->note));
                }
            }
        }

        timerParams->curTemps++;
        if(timerParams->curTemps > timerParams->nbTemps) {
            timerParams->curTemps = 1;
        }
    }

    timerParams->timerValue++;
}

void CTimerParams::emitTempsUpdate(int value, int temps) {
    emit(tempsUpdate(value, temps));
}

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    pads = emptyList()  << new SPad(QString::fromUtf8("Bass Drum 2"), 35, QByteArray("1000000110000000"))
                        << new SPad(QString::fromUtf8("Bass Drum 1"), 36, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Side Stick/Rimshot"), 37, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Snare Drum 1"), 38, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Hand Clap"), 39, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Snare Drum 2"), 40, QByteArray("0000100000001000"))
                        << new SPad(QString::fromUtf8("Low Tom 2"), 41, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Closed Hi-hat"), 42, QByteArray("1010100010101000"))
                        << new SPad(QString::fromUtf8("Low Tom 1"), 43, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Pedal Hi-hat"), 44, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Mid Tom 2"), 45, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Open Hi-hat"), 46, QByteArray("0000001000000010"))
                        << new SPad(QString::fromUtf8("Mid Tom 1"), 47, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("High Tom 2"), 48, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Crash Cymbal 1"), 49, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("High Tom 1"), 50, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Ride Cymbal 1"), 51, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Splash Cymbal"), 55, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Crash Cymbal 2"), 57, QByteArray("0000000000000000"))
                        << new SPad(QString::fromUtf8("Ride Cymbal 2"), 59, QByteArray("0000000000000000"))
                        ;

    setStyleSheet("QMainWindow { background-color: #efefef; }");

    settings = new QSettings("clebail", "qtdrum");

    spNbBeat->setMinimum(MIN_BEAT);
    spNbBeat->setMaximum(MAX_BEAT);
    spNbDiv->setMinimum(MIN_DIV);
    spNbDiv->setMaximum(MAX_DIV);

    ssTemps->setSegmentColor(QColor(0x93, 0x65, 0xb8));
    taTimer->setSegmentColor(QColor(0x1f, 0xb5, 0xac));

    cbMidiPort->addItems(CDrumKit::getInstance()->getMidiPorts());

    drumWidget->addPads(&pads);

    playing = false;
    timerParams.drumWidget = drumWidget;

    realTimeTimer = new QTimer(this);
    realTimeTimer->setInterval(ONE_SECOND);
    connect(realTimeTimer, SIGNAL(timeout()), this, SLOT(onRealTimeTimer()));

    isOpenFileUnsaved = false;
    setOpenFileName("unamed.qdr", "unamed.qdr");

    CDrumKit::getInstance()->init(cbMidiPort->currentIndex());

    connect(&timerParams, SIGNAL(tempsUpdate(int, int)), this, SLOT(onTempsUpdate(int, int)));

    actMute = new QAction("Mute", this);
    connect(actMute, SIGNAL(triggered()), this, SLOT(onMute()));
    popupMenu = new QMenu(drumWidget);
    popupMenu->addAction(actMute);
    currentPad2Mute = nullptr;

    qApp->installEventFilter(this);
}

CMainWindow::~CMainWindow() {
    settings->sync();

    for(int i=0;i<pads.length();i++) {
        delete pads.takeLast();
    }

    delete settings;
}

bool CMainWindow::eventFilter(QObject *object, QEvent *event) {
    int keyPlayStop = settings->value("playStopButton", Qt::Key_B).toInt();
    int keyUpTempo = settings->value("upTempoButton", Qt::Key_Plus).toInt();
    int keyDownTempo = settings->value("downTempoButton", Qt::Key_Minus).toInt();
    int keyReset = settings->value("resetButton", Qt::Key_0).toInt();

    if(event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);


        if(keyEvent->key() == keyPlayStop) {
            on_pbPlayPause_clicked();

            return true;
        }

        if(keyEvent->key() == keyUpTempo) {
            if(spTempo->value() <= spTempo->maximum() - 10) {
                spTempo->setValue(spTempo->value() + 10);
                return true;
            }
        }

        if(keyEvent->key() == keyDownTempo) {
            if(spTempo->value() > 10) {
                spTempo->setValue(spTempo->value() - 10);
                return true;
            }
        }

        if(keyEvent->key() == keyReset) {
            if(playing) {
                pause();
                play();
                return true;
            }
        }
    }

    return QObject::eventFilter(object, event);
}

void CMainWindow::on_pbPlayPause_clicked(bool) {
    playing = !playing;

    if(playing) {
        play();
    }else {
        pause();
    }
}

void CMainWindow::on_cbMidiPort_currentIndexChanged(int value) {
    CDrumKit::getInstance()->init(value);
}

void CMainWindow::onRealTimeTimer(void) {
    int sec, min;
    QString language = settings->value("speechLanguage", "fr").toString();

    realTime++;

    sec = realTime % 60;
    min = realTime / 60;

    if(sec == 0 && min != 0 && cbSoundEMinute->isChecked()) {
        CDrumKit::getInstance()->playNote(39);
    }

    taTimer->setValues(min, sec);
}

void CMainWindow::on_actNewFile_triggered(bool) {
    if(!isOpenFileUnsaved || QMessageBox::question(this, tr("Confirmation"), tr("Current file not save, create new ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        isOpenFileUnsaved = false;
        setOpenFileName("unamed.qdr", "unamed.qdr");

        drumWidget->clear();

        for(int i=0;i<pads.length();i++) {
           pads.at(i)->mute = false;
        }
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

                 for(int i=0;i<pads.length();i++) {
                    pads.at(i)->mute = false;
                 }
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
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open qtdrum file"), fullOpenFileName, tr("qtdum file (*.qdr)"), nullptr, QFileDialog::DontConfirmOverwrite);

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
    pause();
    this->close();
}

void CMainWindow::on_actOptions_triggered(bool) {
    COptionsDialog *optionsDialog = new COptionsDialog(this);

    optionsDialog->setPlayStopButton(settings->value("playStopButton", Qt::Key_B).toInt());
    optionsDialog->setSpeechLanguage(settings->value("speechLanguage", "fr").toString());
    optionsDialog->setUpTempoButton(settings->value("upTempoButton", Qt::Key_Plus).toInt());
    optionsDialog->setDownTempoButton(settings->value("downTempoButton", Qt::Key_Minus).toInt());
    optionsDialog->setResetButton(settings->value("resetButton", Qt::Key_0).toInt());

    if(optionsDialog->exec() == QDialog::Accepted) {
        settings->setValue("playStopButton", optionsDialog->getPlayStopButton());
        settings->setValue("speechLanguage", optionsDialog->getSpeechLanguage());
        settings->setValue("upTempoButton", optionsDialog->getUpTempoButton());
        settings->setValue("downTempoButton", optionsDialog->getDownTempoButton());
        settings->setValue("resetButton", optionsDialog->getResetButton());
    }

    delete optionsDialog;
}

void CMainWindow::on_drumWidget_edit(const SPad&, const QByteArray&, int) {
    isOpenFileUnsaved = true;
}

void CMainWindow::on_spTempo_valueChanged(int value) {
    // isOpenFileUnsaved = true;
    if(playing) {
        setPOSIXTimerTime(ONE_MINUTE / value / timerParams.nbDiv);
    }
}

void CMainWindow::on_spNbBeat_valueChanged(int value) {
    drumWidget->setNbBeat(value);
    isOpenFileUnsaved = true;
}

void CMainWindow::on_spNbDiv_valueChanged(int value) {
    drumWidget->setNbDivPerBeat(value);
    isOpenFileUnsaved = true;
}

void CMainWindow::onTempsUpdate(int value, int temps) {
    ssTemps->setValue(value);
    drumWidget->setCurTemps(temps);
}

void CMainWindow::on_drumWidget_mute(SPad *pad, const QPoint &mnuPos) {
    currentPad2Mute = pad;
    popupMenu->exec(mnuPos);
}

void CMainWindow::onMute(void) {
    if(currentPad2Mute != nullptr) {
        currentPad2Mute->mute = !currentPad2Mute->mute;
        drumWidget->repaint();
        currentPad2Mute = nullptr;
    }
}

void CMainWindow::closeEvent(QCloseEvent *event) {
    if(!isOpenFileUnsaved || QMessageBox::question(this, tr("Confirmation"), tr("Current file not save, quit ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        event->accept();
    }else {
        event->ignore();
    }
}

void CMainWindow::setOpenFileName(QString openFileName, QString fullOpenFileName) {
    this->openFileName = openFileName;
    this->fullOpenFileName = fullOpenFileName;

    actSave->setText(tr("&Save")+" "+openFileName);
}

QString CMainWindow::createFileContent(void) {
    QString fileContent = "";
    QList<QByteArray> matrice = drumWidget->getMatrices();

    fileContent += "Params: "+QString::number(spTempo->value())+","+QString::number(spNbBeat->value())+","+QString::number(spNbDiv->value())+"\n";
    for(int i=0;i<pads.size();i++) {
        fileContent += QString::number(pads[i]->note)+": "+QString(matrice[i])+"\n";
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
                newPads.append(SPad("", static_cast<unsigned char>(elts[0].toInt()), elts[1].toLatin1()));
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
    long intervalNS = (static_cast<long>(intervalMS)) * 1000000L;
    struct itimerspec itimer = { { 0,  intervalNS }, { 0, intervalNS } };
    struct sigevent sigev;

    memset (&sigev, 0, sizeof (struct sigevent));
    sigev.sigev_value.sival_ptr = static_cast<void *>(&timerParams);
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_notify_attributes = nullptr;
    sigev.sigev_notify_function = timerHandler;

    if(timer_create (CLOCK_REALTIME, &sigev, &posixTimer) == 0) {
        if(timer_settime (posixTimer, 0, &itimer, nullptr) == 0) {
            return true;
        }
    }

    return false;
}

void CMainWindow::stopPOSIXTimer(void) {
    timer_delete(posixTimer);
}

bool CMainWindow::setPOSIXTimerTime(int intervalMS) {
    long intervalNS = (static_cast<long>(intervalMS)) * 1000000L;
    struct itimerspec itimer = { { 0,  intervalNS }, { 0, intervalNS } };

    if(timer_settime (posixTimer, 0, &itimer, nullptr) == 0) {
        return true;
    }

    return false;
}

void CMainWindow::enableControls(bool enable) {
    cbMidiPort->setEnabled(enable);
    // spTempo->setEnabled(enable);
    spNbBeat->setEnabled(enable);
    spNbDiv->setEnabled(enable);
    spMute->setEnabled(enable);
    spMuteOver->setEnabled(enable);
    actOptions->setEnabled(enable);
}

void CMainWindow::pause(void) {
    realTimeTimer->stop();

    ssTemps->setValue(0);
    taTimer->setValues(0, 0);

    enableControls(true);

    pbPlayPause->setIcon(QIcon(":/qtdrum/resources/images/play.png"));

    stopPOSIXTimer();

    drumWidget->setCurTemps(-1);
}

void CMainWindow::play(void) {
    timerParams.curTemps = 1;
    timerParams.curMeasure = 1;
    timerParams.nbMute = spMute->value();
    timerParams.nbMuteOver = spMuteOver->value();
    timerParams.timerValue = 0;
    realTime = 0;
    timerParams.nbBeat = spNbBeat->value();
    timerParams.nbDiv = spNbDiv->value();
    timerParams.nbTemps = timerParams.nbBeat * timerParams.nbDiv;
    timerParams.pads = &pads;

    enableControls(false);

    pbPlayPause->setIcon(QIcon(":/qtdrum/resources/images/stop.png"));

    startPOSIXTimer(ONE_MINUTE / spTempo->value() / timerParams.nbDiv);

    realTimeTimer->start();
}


