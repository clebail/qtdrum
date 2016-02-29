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
    void onTimer(void);
private:
    RtMidiOut *midiout;
    bool playing;
    QTimer *timer;

    QStringList getMidiOutputPort(void);
};

#endif // CMAINWINDOW_H
