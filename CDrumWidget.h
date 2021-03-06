#ifndef CDRUMWIDGET_H
#define CDRUMWIDGET_H

#include <QWidget>
#include <QByteArray>

#define MIN_BEAT                1
#define MAX_BEAT                16
#define MIN_DIV                 1
#define MAX_DIV                 16

typedef struct _SPad {
    _SPad(QString nom, unsigned char note, QByteArray map) { this->nom = nom; this->note = note; this->map = map; this->mute = false; }
    QString nom;
    unsigned char note;
    QByteArray map;
    bool mute;
}SPad;

class CDrumWidget : public QWidget {
    Q_OBJECT
public:
    CDrumWidget(QWidget *);
    void addPad(SPad *pad, bool doRepaint = true);
    void addPads(QList<SPad *> *pads);
    QList<QByteArray> getMatrices();
    int getNbTemps(void);
    void clear(void);
    void setMatriceRow(int note, const QByteArray& map);
    void setNbBeat(int nbBeat);
    void setNbDivPerBeat(int nbBeat);
    void setCurTemps(int curTemps);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);
private:
    QList<SPad*> pads;
    QList<QByteArray> matrice;
    int nbBeat, nbDivPerBeat, nbTemps;
    int curTemps;
    int curLineOver;

    void resizeMatrice(void);
    void resizeMatriceRow(int row);
signals:
    void edit(const SPad& pad, const QByteArray& map, int temps);
    void mute(SPad *pad, const QPoint& pos);
};

#endif // CDRUMWIDGET_H

