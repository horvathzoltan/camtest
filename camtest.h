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

    static bool OpenCamera(){return Camtest::_d.download("open", "")=="ok";}
    static bool ActiveCamera(){return Camtest::_d.download("active", "")=="active";}
    static bool CloseCamera(){return Camtest::_d.download("close", "")=="ok";}
    static QPixmap GetPixmap();
    struct UploadR
    {
        QString err;
    };
    static UploadR Upload(const QString& fn);
private:
    static QByteArray GetPicture(){return Camtest::_d.download("get", "format=jpg20&mode=0");}

    static QString UploadMetaData(const QString& fn, int len);
    static void UploadData(const QString& key, const QByteArray& a);
    static int UploadNext(const QString& key);
};


#endif // CAMTEST_H
