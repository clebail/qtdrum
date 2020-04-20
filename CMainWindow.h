#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSettings>
#include <QMediaPlayer>
#include <QMenu>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include "ui_CMainWindow.h"

typedef void (*FPHandler) (union sigval);

class CTimerParams : public QObject {
    Q_OBJECT
public:
    int nbBeat, nbDiv, nbTemps, nbMute, nbMuteOver;
    int curTemps;
    int curMeasure;
    int timerValue;
    QList<SPad *> *pads;
    CDrumWidget *drumWidget;

    void emitTempsUpdate(int value, int temps);
signals:
    void tempsUpdate(int value, int temps);
};

class CMainWindow : public QMainWindow, private Ui::CMainWindow {
    Q_OBJECT
public:
    explicit CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();
    virtual bool eventFilter(QObject *object, QEvent *event);
private slots:
    void on_pbPlayPause_clicked(bool checked = false);
    void on_cbMidiPort_currentIndexChanged(int);
    void onRealTimeTimer(void);
    void on_actNewFile_triggered(bool);
    void on_actOpenFile_triggered(bool);
    void on_actSave_triggered(bool);
    void on_actSaveAs_triggered(bool);
    void on_actQuit_triggered(bool);
    void on_actOptions_triggered(bool);
    void on_drumWidget_edit(const SPad&, const QByteArray&, int);
    void on_spTempo_valueChanged(int);
    void on_spNbBeat_valueChanged(int);
    void on_spNbDiv_valueChanged(int);
    void onTempsUpdate(int, int);
    void on_drumWidget_mute(SPad *, const QPoint &);
    void onMute(void);
protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    bool playing;
    QTimer *realTimeTimer;
    CTimerParams timerParams;
    int realTime;
    QList<SPad *> pads;
    bool isOpenFileUnsaved;
    QString openFileName;
    QString fullOpenFileName;
    timer_t posixTimer;
    QSettings *settings;
    QMediaPlayer *mediaPlayer;
    QAction *actMute;
    QMenu *popupMenu;
    SPad *currentPad2Mute;

    void initDrumKit(void);
    void setOpenFileName(QString openFileName, QString fullOpenFileName);
    QString createFileContent(void);
    bool loadFile(QString fileContent);
    bool saveFile(QString fileName);
    QFont getFont(QString resourceName);
    bool startPOSIXTimer(int intervalMS);
    void stopPOSIXTimer(void);
    void enableControls(bool enable);
    void pause(void);
};

#endif // CMAINWINDOW_H
