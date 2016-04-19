#ifndef COPTIONSDIALOG_H
#define COPTIONSDIALOG_H

#include <QDialog>
#include "ui_COptionsDialog.h"

class COptionsDialog : public QDialog, private Ui::COptionsDialog {
    Q_OBJECT
public:
    explicit COptionsDialog(QWidget *parent = 0);
    ~COptionsDialog();

    void setPlayPauseButton(int key);
    void setSpeechLanguage(QString language);
    int getPlayPauseButton(void);
    QString getSpeechLanguage(void);
};

#endif // COPTIONSDIALOG_H
