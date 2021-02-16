#include <QMetaObject>
#include <QMetaEnum>
#include <QtDebug>
#include "COptionsDialog.h"

COptionsDialog::COptionsDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);

    QMetaObject mo = QObject::staticQtMetaObject;
    int index = mo.indexOfEnumerator("Key");
    if(index != -1) {
        QMetaEnum me = mo.enumerator(index);

        cbButtonPlayStop->clear();
        cbButtonUpTempo->clear();
        cbButtonDownTempo->clear();
        for(int i=0;i<me.keyCount();i++) {
            cbButtonPlayStop->addItem(QString(me.key(i)), QVariant(me.value(i)));
            cbButtonUpTempo->addItem(QString(me.key(i)), QVariant(me.value(i)));
            cbButtonDownTempo->addItem(QString(me.key(i)), QVariant(me.value(i)));
        }
    }
}

COptionsDialog::~COptionsDialog() {
}

void COptionsDialog::setPlayStopButton(int key) {
    for(int i=0;i<cbButtonPlayStop->count();i++) {
        if(cbButtonPlayStop->itemData(i).toInt() == key) {
            cbButtonPlayStop->setCurrentIndex(i);

            return;
        }
    }

    cbButtonPlayStop->setCurrentIndex(-1);
}

void COptionsDialog::setSpeechLanguage(QString language) {
    for(int i=0;i<cbSpeech->count();i++) {
        if(cbSpeech->itemText(i) == language) {
            cbSpeech->setCurrentIndex(i);

            return;
        }
    }

    cbSpeech->setCurrentIndex(-1);
}

void COptionsDialog::setUpTempoButton(int key) {
    for(int i=0;i<cbButtonUpTempo->count();i++) {
        if(cbButtonUpTempo->itemData(i).toInt() == key) {
            cbButtonUpTempo->setCurrentIndex(i);

            return;
        }
    }

    cbButtonUpTempo->setCurrentIndex(-1);
}

void COptionsDialog::setDownTempoButton(int key) {
    for(int i=0;i<cbButtonDownTempo->count();i++) {
        if(cbButtonDownTempo->itemData(i).toInt() == key) {
            cbButtonDownTempo->setCurrentIndex(i);

            return;
        }
    }

    cbButtonDownTempo->setCurrentIndex(-1);
}

int COptionsDialog::getPlayStopButton(void) {
    return cbButtonPlayStop->itemData(cbButtonPlayStop->currentIndex()).toInt();
}

QString COptionsDialog::getSpeechLanguage(void) {
    return cbSpeech->currentText();
}

int COptionsDialog::getUpTempoButton(void) {
    return cbButtonUpTempo->itemData(cbButtonUpTempo->currentIndex()).toInt();
}

int COptionsDialog::getDownTempoButton(void) {
    return cbButtonUpTempo->itemData(cbButtonUpTempo->currentIndex()).toInt();
}
