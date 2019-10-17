#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include "CMainWindow.h"

int main(int argc, char *argv[]){
    QTranslator qtTranslator, qtDrumTranslator;
    QApplication a(argc, argv);
    CMainWindow *w;
    int ret;

    qtTranslator.load("qt_fr", a.applicationDirPath());
    a.installTranslator(&qtTranslator);

    qtDrumTranslator.load("qtdrum_fr", a.applicationDirPath());
    a.installTranslator(&qtDrumTranslator);


    w = new CMainWindow();
    w->show();

    ret = a.exec();

    delete w;

    return ret;
}
