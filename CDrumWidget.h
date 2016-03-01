#ifndef CDRUMWIDGET_H
#define CDRUMWIDGET_H

#include <QWidget>
#include <QByteArray>

typedef struct _SPad {
    _SPad(QString nom, unsigned char note, QByteArray map) { this->nom = nom; this->note = note; this->map = map; }
    QString nom;
    unsigned char note;
    QByteArray map;
}SPad;

class CDrumWidget : public QWidget {
public:
    CDrumWidget(QWidget *);
    void addPad(SPad *pad);
    void addPads(QList<SPad> *pads);
    QList<QByteArray> getMatrices();
    int getNbTemps(void);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
private:
    QList<SPad> pads;
    QList<QByteArray> matrice;
};

#endif // CDRUMWIDGET_H

