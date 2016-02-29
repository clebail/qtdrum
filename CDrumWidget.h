#ifndef CDRUMWIDGET_H
#define CDRUMWIDGET_H

#include <QWidget>

typedef struct _SPad {
    _SPad(QString nom, int channel) { this->nom = nom; this->channel = channel; }
    QString nom;
    int channel;
}SPad;

class CDrumWidget : public QWidget {
public:
    CDrumWidget(QWidget *);
    void addPad(SPad *pad);
    void addPads(QList<SPad> *pads);
    QList<char*> getMatrices();
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
private:
    QList<SPad> pads;
    QList<char*> matrice;
};

#endif // CDRUMWIDGET_H

