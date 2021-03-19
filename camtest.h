#ifndef CAMTEST_H
#define CAMTEST_H

#include <QFileInfo>
#include <QPixmap>
#include <QSqlDatabase>
#include <QString>
#include "common/helper/downloader/downloader.h"

class Camtest
{
public:
    struct CamSettings{
        int brightnest;
        int contrast;
        int saturation;
        int gain;
        int wb;
    };

    static com::helper::Downloader _d;
    static QUrl CamUrl;

    struct StartR{
        QString msg;
        QString serial;
        bool isActive;
        CamSettings _settings;
    };


    static CamSettings _camSettings;
    static StartR Start();
    struct StopR{};
    static StopR Stop();
    static QString NewSerial(const QSqlDatabase &db);
    static bool Ping(const QString &ip, int port=-1);

    static bool OpenCamera(){return Camtest::_d.download("set_cam_open", "")=="ok";}
    static bool ActiveCamera(){return Camtest::_d.download("active", "")=="active";}
    static bool GetCamSettings();
    static bool CloseCamera(){return Camtest::_d.download("set_cam_close", "")=="ok";}
    static QPixmap GetPixmap();
    struct UploadR
    {
        QString err;
    };
    static UploadR Upload(const QString& fn);
    static QString GetDriverName();
    static QFileInfo GetMostRecent(const QString &path, const QString &pattern);

    static int setCamSettings(const QString& s, int i);
    static int brightnest_p();
    static int brightnest_m();
    static int contrast_p();
    static int contrast_m();
    static int saturation_p();
    static int saturation_m();
    static int gain_p();
    static int gain_m();
    static int wb_p();
    static int wb_m();
private:
    static QByteArray GetPicture(){return Camtest::_d.download("get_pic", "format=jpeg&mode=0");}

    static QString UploadMetaData(const QString& fn, int len);
    static void UploadData(const QString& key, const QByteArray& a);
    static int UploadNext(const QString& key);
};


#endif // CAMTEST_H
