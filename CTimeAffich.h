#ifndef CTIMEAFFICH_H
#define CTIMEAFFICH_H

#include <QWidget>
#include <QColor>
#include <QHBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include "CSevenSeg.h"

class CTimeAffich : public QWidget {
    Q_OBJECT
public:
    CTimeAffich(QWidget *);
    ~CTimeAffich(void);

    void setValues(int min, int sec);
    void setSegmentColor(QColor color);
    void setSegmentInactifColor(QColor color);
    void setBackgroundColor(QColor color);
private:
    class CTowPoint : public QWidget {
    public:
        CTowPoint(QWidget *parent) : QWidget(parent) {
            segmentColor = Qt::red;
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            setFixedWidth(6);
        }
        void setSegmentColor(QColor segmentColor) { this->segmentColor = segmentColor; }
    protected:
        virtual void paintEvent(QPaintEvent *event) {
            QPainter painter(this);
            QRect rect = event->rect();
            int x = rect.width() / 2;
            int y = rect.height() / 3;

            painter.setPen(segmentColor);
            painter.setBrush(segmentColor);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawEllipse(x - 1, y, x, x);
            painter.drawEllipse(x - 1, y * 2, x, x);
        }
    private:
        QColor segmentColor;
    };

    QHBoxLayout *hBoxLayout;
    CSevenSeg *ssDMinute, *ssUMinute, *ssDSeconde, *ssUSeconde;
    CTowPoint *towPoint;
};

#endif // CTIMEAFFICH_H

