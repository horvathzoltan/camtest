#ifndef CAMTEST_H
#define CAMTEST_H

#include <QPixmap>
#include <QSqlDatabase>
#include <QString>
#include "common/helper/downloader/downloader.h"

class Camtest
{
public:
    static com::helper::Downloader _d;

    struct StartR{
        QString msg;
        QString serial;
        bool isActive;
    };
    static StartR Start();
    struct StopR{};
    static StopR Stop();
    static QString NewSerial(const QSqlDatabase &db);
    static bool Ping(const QString &ip, int port=-1);

    static QByteArray OpenCamera(){return Camtest::_d.download("open", "");}
    static bool ActiveCamera(){return Camtest::_d.download("active", "")=="active";}
    static QByteArray CloseCamera(){return Camtest::_d.download("close", "");}
    static QPixmap GetPixmap();
    static void Upload(const QString& fn);
private:
    static QByteArray GetPicture(){return Camtest::_d.download("get", "format=jpg20&mode=0");}

};


#endif // CAMTEST_H
