#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator qtTranslator;
    qtTranslator.load(":/lang/lang_oi_ko.qm");
    a.installTranslator(&qtTranslator);
    MainWindow w;
    w.show();

    return a.exec();
}
