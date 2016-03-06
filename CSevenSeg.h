#ifndef CSEVENSEG_H
#define CSEVENSEG_H

#include <QWidget>
#include <QColor>

class CSevenSeg : public QWidget {
    Q_OBJECT
public:
    CSevenSeg(QWidget *);

    void setValue(int value);
    void setSegmentColor(QColor color);
    void setSegmentInactifColor(QColor color);
    void setBackgroundColor(QColor color);
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    int value;
    QColor segmentColor;
    QColor segmentInactifColor;
    QColor backgroundColor;
};

#endif // CSEVENSEG_H

