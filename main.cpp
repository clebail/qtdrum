#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include "CMainWindow.h"

int main(int argc, char *argv[]){
    QTranslator qtTranslator, qtDrumTranslator;
    QApplication a(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
    QString locale = QLocale::system().name().section('_', 0, 0);

    qtTranslator.load("qt_"+locale, a.applicationDirPath());
    a.installTranslator(&qtTranslator);

    qtDrumTranslator.load("qtdrum_"+locale, a.applicationDirPath());
    a.installTranslator(&qtDrumTranslator);

    CMainWindow w;

    w.show();

    return a.exec();
}
