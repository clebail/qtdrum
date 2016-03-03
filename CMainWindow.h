#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include "ui_CMainWindow.h"

typedef struct _STimerParams {
    int nbBeat, nbDiv, nbTemps;
    int curTemps;
    int timerValue;
    QList<SPad> *pads;
    CDrumWidget *drumWidget;
    QLabel *lbTimer;
}STimerParams;

typedef void (*FPHandler) (union sigval);

class CMainWindow : public QMainWindow, private Ui::CMainWindow {
    Q_OBJECT
public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();
private slots:
    void on_pbPlayPause_clicked(bool);
    void on_cbMidiPort_currentIndexChanged(int);
    void onRealTimeTimer(void);
    void on_actNewFile_triggered(bool);
    void on_actOpenFile_triggered(bool);
    void on_actSave_triggered(bool);
    void on_actSaveAs_triggered(bool);
    void on_actQuit_triggered(bool);
    void on_drumWidget_edit(const SPad&, const QByteArray&, int);
    void on_spTempo_valueChanged(int);
    void on_spNbBeat_valueChanged(int);
    void on_spNbDiv_valueChanged(int);
protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    bool playing;
    QTimer *realTimeTimer;
    STimerParams timerParams;
    int realTime;
    QList<SPad> pads;
    bool isOpenFileUnsaved;
    QString openFileName;
    QString fullOpenFileName;
    timer_t posixTimer;

    void initDrumKit(void);
    void setOpenFileName(QString openFileName, QString fullOpenFileName);
    QString createFileContent(void);
    bool loadFile(QString fileContent);
    bool saveFile(QString fileName);
    QFont getFont(QString resourceName);
    bool startPOSIXTimer(int intervalMS);
    void stopPOSIXTimer(void);
};

#endif // CMAINWINDOW_H
