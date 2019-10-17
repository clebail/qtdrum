#include <QPainter>
#include <QPaintEvent>
#include <QtDebug>
#include "CSevenSeg.h"

#define A       0
#define B       1
#define C       2
#define D       3
#define E       4
#define F       5
#define G       6

static char maskValue[10][7] = {
    { 1, 1, 1, 1, 1, 1, 0 },
    { 0, 1, 1, 0, 0, 0, 0 },
    { 1, 1, 0, 1, 1, 0, 1 },
    { 1, 1, 1, 1, 0, 0, 1 },
    { 0, 1, 1, 0, 0, 1, 1 },
    { 1, 0, 1, 1, 0, 1, 1 },
    { 1, 0, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 0, 1, 1 }
};

CSevenSeg::CSevenSeg(QWidget *parent) : QWidget(parent) {
    value = 0;

    segmentColor = Qt::red;
    segmentInactifColor = QColor(0xfe, 0xfe, 0xfe);
    backgroundColor = QColor(0xef, 0xef, 0xef);
}

void CSevenSeg::setValue(int value) {
    if(value < 0) value = 0;
    if(value > 9) value = 9;

    this->value = value;

    repaint();
}

void CSevenSeg::setSegmentColor(QColor color) {
    segmentColor = color;
    repaint();
}

void CSevenSeg::setSegmentInactifColor(QColor color){
    segmentInactifColor = color;
    repaint();
}

void CSevenSeg::setBackgroundColor(QColor color) {
    backgroundColor = color;
    repaint();
}

void CSevenSeg::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPolygonF a, b, c, d, e, f, g;
    double mulX = event->rect().width() / 10.0;
    double mulY = event->rect().height() / 18.0;
    double mul, margeX=0, margeY=0;

    if(mulX < mulY) {
        mul = mulX;
        margeY = (event->rect().height() - 18.0 * mul) / 2.0;
    } else {
        mul = mulY;
        margeX = (event->rect().width() - 10.0 * mul) / 2.0;
    }

    painter.setPen(backgroundColor);
    painter.setBrush(backgroundColor);
    painter.drawRect(event->rect());

    a << QPointF(margeX + mul, margeY + mul) << QPointF(margeX + 2 * mul, margeY) << QPointF(margeX + 8 * mul, margeY) << QPointF(margeX + 9 * mul, margeY + mul) << QPointF(margeX + 8 * mul, margeY + 2 * mul) << QPointF(margeX + 2 * mul, margeY + 2 * mul);
    b << QPointF(margeX + 9 * mul, margeY + 1 * mul) << QPointF(margeX + 10 * mul, margeY + 2 * mul) << QPointF(margeX + 10 * mul, margeY + 8 * mul) << QPointF(margeX + 9 * mul, margeY + 9 * mul) << QPointF(margeX + 8 * mul, margeY + 8 * mul) << QPointF(margeX + 8 * mul, margeY + 2 * mul);
    c << QPointF(margeX + 9 * mul, margeY + 9 * mul) << QPointF(margeX + 10 * mul, margeY + 10 * mul) << QPointF(margeX + 10 * mul, margeY + 16 * mul) << QPointF(margeX + 9 * mul, margeY + 17 * mul) << QPointF(margeX + 8 * mul, margeY + 16 * mul) << QPointF(margeX + 8 * mul, margeY + 10 * mul);
    d << QPointF(margeX + 9 * mul, margeY + 17 * mul) << QPointF(margeX + 8 * mul, margeY + 18 * mul) << QPointF(margeX + 2 * mul, margeY + 18 * mul) << QPointF(margeX + 1 * mul, margeY + 17 * mul) << QPointF(margeX + 2 * mul, margeY + 16 * mul) << QPointF(margeX + 8 * mul, margeY + 16 * mul);
    e << QPointF(margeX + mul, margeY + 17 * mul) << QPointF(margeX, margeY + 16 * mul) << QPointF(margeX, margeY + 10 * mul) << QPointF(margeX + 1 * mul, margeY + 9 * mul) << QPointF(margeX + 2 * mul, margeY + 10 * mul) << QPointF(margeX + 2 * mul, margeY + 16 * mul);
    f << QPointF(margeX + mul, margeY + 9 * mul) << QPointF(margeX, margeY + 8 * mul) << QPointF(margeX, margeY + 2 * mul) << QPointF(margeX + 1 * mul, margeY + 1 * mul) << QPointF(margeX + 2 * mul, margeY + 2 * mul) << QPointF(margeX + 2 * mul, margeY + 8 * mul);
    g << QPointF(margeX + mul, margeY + 9 * mul) << QPointF(margeX + 2 * mul, margeY + 8 * mul) << QPointF(margeX + 8 * mul, margeY + 8 * mul) << QPointF(margeX + 9 * mul, margeY + 9 * mul) << QPointF(margeX + 8 * mul, margeY + 10 * mul) << QPointF(margeX + 2 * mul, margeY + 10 * mul);

    painter.setBrush(maskValue[value][A] == 1 ? segmentColor : segmentInactifColor);
    painter.drawPolygon(a);
    painter.setBrush(maskValue[value][B] == 1 ? segmentColor : segmentInactifColor);
    painter.drawPolygon(b);
    painter.setBrush(maskValue[value][C] == 1 ? segmentColor : segmentInactifColor);
    painter.drawPolygon(c);
    painter.setBrush(maskValue[value][D] == 1 ? segmentColor : segmentInactifColor);
    painter.drawPolygon(d);
    painter.setBrush(maskValue[value][E] == 1 ? segmentColor : segmentInactifColor);
    painter.drawPolygon(e);
    painter.setBrush(maskValue[value][F] == 1 ? segmentColor : segmentInactifColor);
    painter.drawPolygon(f);
    painter.setBrush(maskValue[value][G] == 1 ? segmentColor : segmentInactifColor);
    painter.drawPolygon(g);
}

