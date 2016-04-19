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

        cbButton->clear();
        for(int i=0;i<me.keyCount();i++) {
            cbButton->addItem(QString(me.key(i)), QVariant(me.value(i)));
        }
    }
}

COptionsDialog::~COptionsDialog() {
}

void COptionsDialog::setPlayPauseButton(int key) {
    for(int i=0;i<cbButton->count();i++) {
        if(cbButton->itemData(i).toInt() == key) {
            cbButton->setCurrentIndex(i);

            return;
        }
    }

    cbButton->setCurrentIndex(-1);
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

int COptionsDialog::getPlayPauseButton(void) {
    return cbButton->itemData(cbButton->currentIndex()).toInt();
}

QString COptionsDialog::getSpeechLanguage(void) {
    return cbSpeech->currentText();
}
