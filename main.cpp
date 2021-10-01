#include "mainwindow.h"
//#include "mousemovesignaler.h"
#include "settings.h"
#include "common/logger/log.h"
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

Settings _settings(
    {
        "QODBC",
        "BuildInfoFlex",
        {{"office.logcontrol.hu", 9876}, {"172.16.1.5", 1433}},
        "sa",
        "Gtr7jv8fh2"
    } );

//MouseMoveSignaler mouse_move_signaler;


auto main(int argc, char *argv[]) -> int
{    
    QApplication a(argc, argv);
    Log::init(Errlevels::ERROR_,
              Dbglevels::DEBUG, nullptr, false, nullptr, false);

    MainWindow w;
    w.show();
    auto b =  a.exec();
    return b;
}
