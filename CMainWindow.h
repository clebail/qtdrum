#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_CMainWindow.h"
#include "RtMidi.h"

class CMainWindow : public QMainWindow, private Ui::CMainWindow {
    Q_OBJECT
public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();
private slots:
    void on_pbPlayPause_clicked(bool);
    void on_cbMidiPort_currentIndexChanged(int);
    void onStartTimer(void);
    void onTimer(void);
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
    RtMidiOut *midiout;
    bool playing;
    QTimer *timer, *startTimer, *realTimeTimer;
    int curTemps, realTime;
    QList<SPad> pads;
    int startTimerValue;
    bool isOpenFileUnsaved;
    QString openFileName;
    QString fullOpenFileName;
    QFont getFont(QString resourceName);
    int nbBeat, nbDiv, nbTemps;

    QStringList getMidiOutputPort(void);
    void initDrumKit(void);
    void playNote(char note);
    void stopNote(char note);
    void setOpenFileName(QString openFileName, QString fullOpenFileName);
    QString createFileContent(void);
    bool loadFile(QString fileContent);
    bool saveFile(QString fileName);
};

#endif // CMAINWINDOW_H
