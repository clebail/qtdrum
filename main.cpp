#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include "CMainWindow.h"

int main(int argc, char *argv[]){
    QTranslator translator;
    QApplication a(argc, argv);
    CMainWindow w;

    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

    translator.load("qt_fr", a.applicationDirPath());
    a.installTranslator(&translator);

    w.show();

    return a.exec();
}
