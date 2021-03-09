#ifndef COPTIONSDIALOG_H
#define COPTIONSDIALOG_H

#include <QDialog>
#include "ui_COptionsDialog.h"

class COptionsDialog : public QDialog, private Ui::COptionsDialog {
    Q_OBJECT
public:
    explicit COptionsDialog(QWidget *parent = nullptr);
    ~COptionsDialog();

    void setPlayStopButton(int key);
    void setSpeechLanguage(QString language);
    void setUpTempoButton(int key);
    void setDownTempoButton(int key);
    void setResetButton(int key);
    int getPlayStopButton(void);
    QString getSpeechLanguage(void);
    int getUpTempoButton(void);
    int getDownTempoButton(void);
    int getResetButton(void);
};


#endif // COPTIONSDIALOG_H
