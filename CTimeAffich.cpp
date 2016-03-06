#include <QtDebug>
#include "CTimeAffich.h"


CTimeAffich::CTimeAffich(QWidget *parent) : QWidget(parent) {
    hBoxLayout = new QHBoxLayout(this);

    ssDMinute = new CSevenSeg(this);
    ssUMinute = new CSevenSeg(this);
    ssDSeconde = new CSevenSeg(this);
    ssUSeconde = new CSevenSeg(this);

    towPoint = new CTowPoint(this);

    hBoxLayout->setContentsMargins(QMargins(0, 0, 0, 0));

    hBoxLayout->addWidget(ssDMinute);
    hBoxLayout->addWidget(ssUMinute);
    hBoxLayout->addWidget(towPoint);
    hBoxLayout->addWidget(ssDSeconde);
    hBoxLayout->addWidget(ssUSeconde);

    setLayout(hBoxLayout);
}

CTimeAffich::~CTimeAffich(void) {
    delete ssDMinute;
    delete ssUMinute;

    delete towPoint;

    delete ssDSeconde;
    delete ssUSeconde;

    delete hBoxLayout;
}

void CTimeAffich::setValues(int min, int sec) {
    ssDMinute->setValue(min /10);
    ssUMinute->setValue(min % 10);
    ssDSeconde->setValue(sec /10);
    ssUSeconde->setValue(sec % 10);
}

void CTimeAffich::setSegmentColor(QColor color) {
    ssDMinute->setSegmentColor(color);
    ssUMinute->setSegmentColor(color);
    ssDSeconde->setSegmentColor(color);
    ssUSeconde->setSegmentColor(color);

    towPoint->setSegmentColor(color);
}

void CTimeAffich::setSegmentInactifColor(QColor color){
    ssDMinute->setSegmentInactifColor(color);
    ssUMinute->setSegmentInactifColor(color);
    ssDSeconde->setSegmentInactifColor(color);
    ssUSeconde->setSegmentInactifColor(color);
}

void CTimeAffich::setBackgroundColor(QColor color) {
    ssDMinute->setBackgroundColor(color);
    ssUMinute->setBackgroundColor(color);
    ssDSeconde->setBackgroundColor(color);
    ssUSeconde->setBackgroundColor(color);
}


