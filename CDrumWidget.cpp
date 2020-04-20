#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QtDebug>
#include "CDrumWidget.h"

#define TEMPS_WIDTH             (static_cast<int>(24))
#define TEMPS_HEIGHT            (static_cast<int>(24))
#define TEMPS_CASE_DIFF_WIDTH   (static_cast<int>(4))
#define TEMPS_CASE_DIFF_HEIGHT  (static_cast<int>(4))
#define CASE_WIDTH              (TEMPS_WIDTH-TEMPS_CASE_DIFF_WIDTH*2)
#define CASE_HEIGHT             (TEMPS_HEIGHT-TEMPS_CASE_DIFF_HEIGHT*2)

CDrumWidget::CDrumWidget(QWidget *parent) : QWidget(parent) {
    nbBeat = 4;
    nbDivPerBeat = 4;
    nbTemps = nbBeat * nbDivPerBeat;
    curTemps = -1;
    curLineOver = 0;

    setMouseTracking(true);
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void CDrumWidget::addPad(SPad *pad, bool doRepaint) {
    pads.append(pad);
    matrice.append(QByteArray(pad->map));

    if(doRepaint) {
        repaint();
    }
}

void CDrumWidget::addPads(QList<SPad *> *pads2Add){
    for(int i=0;i<pads2Add->length();i++) {
        SPad *pad = pads2Add->at(i);
        addPad(pad, false);
    }

    repaint();
}

QList<QByteArray> CDrumWidget::getMatrices() {
    return matrice;
}

void CDrumWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QRect header;
    QColor border(0x6f, 0x7f, 0x87);
    QColor backgroundTemps(0x62, 0x76, 0x7c);
    QColor backgroundCurrent(0xfd, 0x72, 0x72);
    QColor background(0x6f, 0x89, 0x92);
    QPen pen(border);
    int titleWidth = event->rect().width() - nbTemps * TEMPS_WIDTH;
    int i, x ,y;
    QFont font = QFont(painter.font());
    QFont muteFont = QFont(painter.font());

    muteFont.setItalic(true);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawRect(event->rect());

    header=QRect(0, 0, titleWidth, TEMPS_HEIGHT);
    painter.setPen(border);
    painter.setBrush(background);
    painter.drawRect(header);
    painter.setPen(Qt::white);
    painter.drawText(header, tr("Pad"), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

    for(i=0;i<nbTemps;i++) {
        x = i * TEMPS_WIDTH + titleWidth;
        bool onTemps = i % nbDivPerBeat == 0;

        header=QRect(x, 0, TEMPS_WIDTH, TEMPS_HEIGHT);
        painter.setPen(border);
        painter.setBrush(curTemps != -1 && i == curTemps ? backgroundCurrent : onTemps ? backgroundTemps : background);
        painter.drawRect(header);
        painter.setPen(Qt::white);
        if(onTemps) {
            painter.drawText(header, QString::number(i / nbDivPerBeat + 1), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
        }
    }

    for(i=0;i<pads.length();i++) {
        bool even = i % 2 == 0;
        QByteArray ba = matrice.at(i);

        y = i * TEMPS_HEIGHT + TEMPS_HEIGHT + 1;

        header=QRect(0, y, titleWidth, TEMPS_HEIGHT);
        painter.setPen(Qt::white);
        painter.setBrush(i == curLineOver ? QColor(0xfa, 0xb1, 0xa0) : (even ? QColor(0xEF, 0xEF, 0xEF) : Qt::white));
        painter.drawRect(QRect(0, y, event->rect().width(), TEMPS_HEIGHT));
        painter.setPen(Qt::black);

        if(pads.at(i)->mute) {
             painter.setPen(Qt::gray);
             painter.setFont(muteFont);
        } else {
            painter.setPen(Qt::black);
            painter.setFont(font);
        }

        painter.drawText(header, pads.at(i)->nom, QTextOption(Qt::AlignVCenter));

        for(int j=0;j<nbTemps;j++) {
            int x = j * TEMPS_WIDTH + titleWidth;

            pen.setWidth(j % nbDivPerBeat == 0 ? 2 : 1);

            header=QRect(x + TEMPS_CASE_DIFF_WIDTH, y + TEMPS_CASE_DIFF_HEIGHT, CASE_WIDTH, CASE_HEIGHT);
            painter.setPen(pen);
            painter.setBrush(ba.at(j) == '1' ? (pads.at(i)->mute ? Qt::gray : QColor(0x2c, 0x89, 0xc9)) : Qt::white);
            painter.drawRect(header);
        }
    }
}

void CDrumWidget::mouseReleaseEvent(QMouseEvent *event) {
    int x = event->x(), y = event->y();
    int titleWidth = rect().width() - nbTemps * TEMPS_WIDTH;

    if(x > titleWidth && y > TEMPS_HEIGHT && y < pads.length() * TEMPS_HEIGHT + TEMPS_HEIGHT + 1) {
        int col, row;
        QByteArray ba;

        col = (x - titleWidth) / TEMPS_WIDTH;
        row = (y - TEMPS_HEIGHT) / TEMPS_HEIGHT;

        ba = matrice[row];
        ba[col] = '1' + '0' - ba[col];

        matrice[row] = ba;

        repaint();

        emit(edit(*pads[row], ba, col));
    }
}

void CDrumWidget::mouseMoveEvent(QMouseEvent *event) {
    int newLineOver = qMin(qMax((event->y() - TEMPS_HEIGHT) / TEMPS_HEIGHT, 0), pads.size() - 1);


    if(curLineOver != newLineOver) {
        curLineOver = newLineOver;

        repaint();
    }
}

void CDrumWidget::contextMenuEvent(QContextMenuEvent *event) {
    int y = event->y();

    if(y > TEMPS_HEIGHT && y < pads.length() * TEMPS_HEIGHT + TEMPS_HEIGHT + 1) {
        int row;

        row = (y - TEMPS_HEIGHT) / TEMPS_HEIGHT;

        emit(mute(pads[row], event->globalPos()));
    }
}

int CDrumWidget::getNbTemps(void) {
    return nbTemps;
}

void CDrumWidget::clear(void) {
    for(int i=0;i<matrice.size();i++) {
        matrice[i] = QByteArray(nbTemps, '0');
    }

    repaint();
}

void CDrumWidget::setMatriceRow(int note, const QByteArray& map) {
    for(int i=0;i<pads.size();i++) {
        if(pads[i]->note == note) {
            matrice[i] = QByteArray(map);
            resizeMatriceRow(i);

            return;
        }
    }
}

void CDrumWidget::setNbBeat(int nbBeat) {
    if(this->nbBeat != nbBeat && nbBeat >= MIN_BEAT && nbBeat <= MAX_BEAT) {
        this->nbBeat = nbBeat;
        nbTemps = nbBeat * nbDivPerBeat;

        resizeMatrice();

        repaint();
    }
}

void CDrumWidget::setNbDivPerBeat(int nbDivPerBeat) {
    if(this->nbDivPerBeat != nbDivPerBeat && nbDivPerBeat >= MIN_DIV && nbDivPerBeat <= MAX_DIV) {
        this->nbDivPerBeat = nbDivPerBeat;
        nbTemps = nbBeat * nbDivPerBeat;

        resizeMatrice();

        repaint();
    }
}

void CDrumWidget::setCurTemps(int curTemps) {
    this->curTemps = curTemps;

    repaint();
}

void CDrumWidget::resizeMatrice(void) {
    for(int i=0;i<matrice.size();i++) {
        resizeMatriceRow(i);
    }
}

void CDrumWidget::resizeMatriceRow(int row) {
    int curSize = matrice[row].size();
    if(curSize < nbTemps) {
        matrice[row].append(QByteArray(nbTemps - curSize, '0'));
    }else if(curSize > nbTemps) {
        matrice[row].truncate(nbTemps);
    }
}
