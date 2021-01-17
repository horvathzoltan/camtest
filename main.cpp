#include "mainwindow.h"

#include <QApplication>
/*
001001a

0-1
00 project/készülék, program nevéből, verziójából jön - > api lekérdezhető(megismerhető) -> verziószámozási rendszer kell
    build számot központilag osztja ki, de verzió számot nem itt innen kap, hanem a központiban lehet egy build szám -tól tartományára (-tól-ig) kötni


 1001,sorozatszám, a:verzió
*/

/*
 * Adott host IP cím alapján MAC: arp -a 172.16.3.235
 *  - mac alapján sorozatszám
 * API név,verzió
 * név és verzió
 * API hívást lehet inzézni /gomb, /interval
*/

auto main(int argc, char *argv[]) -> int
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();
}
