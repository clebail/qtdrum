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
private:
    RtMidiOut *midiout;
    bool playing;
    QTimer *timer, *startTimer;
    int curTemps;
    QList<SPad> pads;
    int startTimerValue;

    QStringList getMidiOutputPort(void);
    void initDrumKit(void);
    void playNote(char note);
    void stopNote(char note);
};

#endif // CMAINWINDOW_H
