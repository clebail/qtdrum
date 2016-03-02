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
    Q_OBJECT
public:
    CDrumWidget(QWidget *);
    void addPad(SPad *pad, bool doRepaint = true);
    void addPads(QList<SPad> *pads);
    QList<QByteArray> getMatrices();
    int getNbTemps(void);
    void clear(void);
    void setMatriceRow(int note, const QByteArray& map);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
private:
    QList<SPad> pads;
    QList<QByteArray> matrice;
signals:
    void edit(const SPad& pad, const QByteArray& map, int temps);
};

#endif // CDRUMWIDGET_H

