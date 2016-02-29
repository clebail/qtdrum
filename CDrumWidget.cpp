#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QtDebug>
#include "CDrumWidget.h"

#define NB_TEMPS                ((int)16)
#define TEMPS_WIDTH             ((int)24)
#define TEMPS_HEIGHT            ((int)24)
#define TEMPS_CASE_DIFF_WIDTH   ((int)4)
#define TEMPS_CASE_DIFF_HEIGHT  ((int)4)
#define CASE_WIDTH              (TEMPS_WIDTH-TEMPS_CASE_DIFF_WIDTH*2)
#define CASE_HEIGHT             (TEMPS_HEIGHT-TEMPS_CASE_DIFF_HEIGHT*2)
#define TEMPS_TIMER_DIFF_WIDTH  ((int)6)
#define TEMPS_TIMER_DIFF_HEIGHT ((int)6)
#define TIMER_WIDTH             (TEMPS_WIDTH-TEMPS_TIMER_DIFF_WIDTH*2)
#define TIMER_HEIGHT            (TEMPS_HEIGHT-TEMPS_TIMER_DIFF_HEIGHT*2)

CDrumWidget::CDrumWidget(QWidget *parent) : QWidget(parent) {

}

void CDrumWidget::addPad(SPad *pad) {
    char *row = new char[NB_TEMPS];

    memset(row, 0, NB_TEMPS);

    pads.append(*pad);;
    matrice.append(row);

    repaint();
}

void CDrumWidget::addPads(QList<SPad> *pads2Add){
    for(int i=0;i<pads2Add->length();i++) {
        SPad pad = pads2Add->at(i);
        addPad(&pad);
    }

    repaint();
}

QList<char*> CDrumWidget::getMatrices() {
    return matrice;
}

void CDrumWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QRect header;
    QColor border(0x6f, 0x7f, 0x87);
    QColor backgroundTemps(0x62, 0x76, 0x7c);
    QColor background(0x6f, 0x89, 0x92);
    int titleWidth = event->rect().width() - NB_TEMPS * TEMPS_WIDTH;
    int i, x ,y;

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawRect(event->rect());

    header=QRect(0, 0, titleWidth, TEMPS_HEIGHT);
    painter.setPen(border);
    painter.setBrush(background);
    painter.drawRect(header);
    painter.setPen(Qt::white);
    painter.drawText(header, "Pad", QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

    for(i=0;i<NB_TEMPS;i++) {
        x = i * TEMPS_WIDTH + titleWidth;
        bool onTemps = i % 4 == 0;

        header=QRect(x, 0, TEMPS_WIDTH, TEMPS_HEIGHT);
        painter.setPen(border);
        painter.setBrush(onTemps ? backgroundTemps : background);
        painter.drawRect(header);
        painter.setPen(Qt::white);
        if(onTemps) {
            painter.drawText(header, QString::number(i / 4 + 1), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
        }
    }

    for(i=0;i<pads.length();i++) {
        bool even = i % 2 == 0;
        char *cRow = matrice.at(i);

        y = i * TEMPS_HEIGHT + TEMPS_HEIGHT + 1;

        header=QRect(0, y, titleWidth, TEMPS_HEIGHT);
        painter.setPen(Qt::white);
        painter.setBrush(even ? QColor(0xEF, 0xEF, 0xEF) : Qt::white);
        painter.drawRect(QRect(0, y, event->rect().width(), TEMPS_HEIGHT));
        painter.setPen(Qt::black);
        painter.drawText(header, pads.at(i).nom, QTextOption(Qt::AlignVCenter));

        for(int j=0;j<NB_TEMPS;j++) {
            int x = j * TEMPS_WIDTH + titleWidth;

            header=QRect(x + TEMPS_CASE_DIFF_WIDTH, y + TEMPS_CASE_DIFF_HEIGHT, CASE_WIDTH, CASE_HEIGHT);
            painter.setPen(border);
            painter.setBrush(cRow[j] ? QColor(0x2c, 0x89, 0xc9) : Qt::white);
            painter.drawRect(header);
        }
    }

    QPen pen(border);
    pen.setWidth(2);
    y = pads.length() * TEMPS_HEIGHT + TEMPS_HEIGHT + 1 + TEMPS_TIMER_DIFF_HEIGHT;
    for(int j=0;j<NB_TEMPS;j++) {
        x = j * TEMPS_WIDTH + titleWidth;

        header=QRect(x + TEMPS_TIMER_DIFF_WIDTH, y, TIMER_WIDTH, TIMER_HEIGHT);

        painter.setPen(pen);
        painter.setBrush(Qt::white);
        painter.drawEllipse(header);
    }

}

void CDrumWidget::mouseReleaseEvent(QMouseEvent *event) {
    int x = event->x(), y = event->y();
    int titleWidth = rect().width() - NB_TEMPS * TEMPS_WIDTH;

    if(x > titleWidth && y > TEMPS_HEIGHT && y < pads.length() * TEMPS_HEIGHT + TEMPS_HEIGHT + 1 + TEMPS_TIMER_DIFF_HEIGHT) {
        int col, row;
        char *cRow;

        col = (x - titleWidth) / TEMPS_WIDTH;
        row = (y - TEMPS_HEIGHT) / TEMPS_HEIGHT;

        cRow = matrice.at(row);
        cRow[col] = 1 - cRow[col];

        repaint();
    }
}