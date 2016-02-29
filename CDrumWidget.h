#ifndef CDRUMWIDGET_H
#define CDRUMWIDGET_H

#include <QWidget>

typedef struct _SPad {
    _SPad(QString nom, unsigned char note) { this->nom = nom; this->note = note; }
    QString nom;
    unsigned char note;
}SPad;

class CDrumWidget : public QWidget {
public:
    CDrumWidget(QWidget *);
    void addPad(SPad *pad);
    void addPads(QList<SPad> *pads);
    QList<char*> getMatrices();
    int getNbTemps(void);
    void setCurTemps(int curTemps);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
private:
    QList<SPad> pads;
    QList<char*> matrice;
    int curTemps;
};

#endif // CDRUMWIDGET_H

