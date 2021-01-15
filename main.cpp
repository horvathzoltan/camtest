#include "mainwindow.h"

#include <QApplication>
/*
001001a

00 project/készülék, 1001,sorozatszám, a:verzió
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
