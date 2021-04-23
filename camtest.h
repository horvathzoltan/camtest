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

    static com::helper::Downloader* _d;
    static QUrl CamUrl;

    struct StartR{
        QString msg;
        QString serial;
        bool isActive;
        QString version;
        CamSettings _settings;
    };

    static bool OpenCamera()
    {
        if(!_d) return false;
        return _d->download("set_cam_open", "")=="ok";
    }

    static bool CloseCamera()
    {
        if(!_d) return false;
        return _d->download("set_cam_close", "")=="ok";
    }


    static CamSettings _camSettings;
    static StartR Start();
    struct StopR{};
    static StopR Stop();
    static QString NewSerial(const QSqlDatabase &db);
    static bool Ping(const QString &ip, int port=-1);
    static QStringList GetIp(int i1, int i2, int p);
    static void FilterLocalIp(QStringList *l);
    static bool GetCamSettings();
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

    struct UpdateR
    {
      bool isOk;
      QString msg;
    };

    static Camtest::UpdateR Update();

private:
    static bool DeviceUpdateStorageStatus()
    {
        if(!_d) return false;
        auto a = _d->download("get_storage_status", "");
        return a.startsWith("ok");
    }

    static bool DeviceMountStorage()
    {
        if(!_d) return false;
        auto a = _d->download("set_storage_mount", "");
        return a.startsWith("ok");
    }

    static bool DeviceActive()
    {
        if(!_d) return false;
        auto a = _d->download("active", "");
        auto b = a=="active";
        return b;
    }

    static QByteArray GetPicture()
    {
        if(!_d) return nullptr;
        return _d->download("get_pic", "format=jpeg&mode=0");
    }

    static QString DeviceVersion()
    {
        if(!_d) return nullptr;
        return _d->download("version", "");
    }

    static QString DeviceUpdate()
    {
        if(!_d) return nullptr;
        return _d->download("update", "");
    }

    static QString UploadMetaData(const QString& fn, int len);
    static void UploadData(const QString& key, const QByteArray& a);
    static int UploadNext(const QString& key);
};


#endif // CAMTEST_H
