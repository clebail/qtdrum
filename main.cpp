#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <QtDebug>
#include "CMainWindow.h"

int main(int argc, char *argv[]){
    QTranslator qtTranslator, qtDrumTranslator;
    QApplication a(argc, argv);
    CMainWindow *w;
	QString lang = QLocale().name().split('_')[0];
    int ret;

    qtTranslator.load("qt_"+lang, a.applicationDirPath());
    a.installTranslator(&qtTranslator);

    qtDrumTranslator.load("qtdrum_"+lang, a.applicationDirPath());
    a.installTranslator(&qtDrumTranslator);


    w = new CMainWindow();
    w->show();

    ret = a.exec();

    delete w;

    return ret;
}
