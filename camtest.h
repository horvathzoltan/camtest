#ifndef CAMTEST_H
#define CAMTEST_H

#include <QFileInfo>
#include <QPixmap>
#include <QSqlDatabase>
#include <QString>
#include "downloadhelper.h"

class Camtest
{
public:
    struct CamSettings{
        int brightness;
        int contrast;
        int saturation;
        //int gain;
        int iso;
        int awb;
        int exposure;
    };


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
       auto r = _downloadHelper.download("set_cam_open", "");
       return r.content=="ok";
    }

    static bool CloseCamera()
    {
        auto r = _downloadHelper.download("set_cam_close", "");
        return r.content =="ok";
    }

    static DownloadHelper _downloadHelper;
    static CamSettings _camSettings;
    static StartR Start();
    struct StopR{};
    static StopR Stop();
    static QString NewSerial(const QSqlDatabase &db);
    //static bool Ping(const QString &ip, int port=-1);
    static QStringList GetIp_old(int i1, int i2, int p);
    static QStringList GetIp(uchar i1, uchar i2, quint16 p);
    static void FilterLocalIp(QStringList *l);
    static bool GetCamSettings();
    static bool ClearCamSettings(int id);
    static bool SetCalD(int id, qreal dmin, qreal dmax);
    struct Status{
        bool isValid = false;
        bool isOpened;
    };
    static bool SetCalF(int id, qreal x0, qreal y0, qreal x1, qreal y1);

    static Status GetCamStatus();
    static QPixmap GetPixmap(bool isMvis);
    struct UploadR
    {
        QString err;
    };
    static UploadR Upload(const QString& fn);
    //static QString GetDriverName();
//    static QFileInfo GetMostRecent(const QString &path, const QString &pattern);

    static bool setCamSettings(const QString& s, int i);
    static int brightnest_p();
    static int brightnest_m();
    static int contrast_p();
    static int contrast_m();
    static int saturation_p();
    static int saturation_m();
    static int iso_p();
    static int iso_m();
    static int wb_p();
    static int wb_m();
    static int exposure_p();
    static int exposure_m();

    struct UpdateR
    {
      bool isOk;
      QString msg;
    };

    static Camtest::UpdateR Update();

    struct RestartR
    {
        bool isOk;
        QString msg;
    };
    static Camtest::RestartR Restart();

    struct StartRecR{
        QString msg;
    };

    static Camtest::StartRecR StartRecording();

    struct StopRecR{
        QString msg;
    };

    static StopRecR StopRecording();

    struct StartRecSyncR{
        QString msg;
    };

    static Camtest::StartRecSyncR StartRecSync();

    struct StopRecSyncR{
        QString msg;
    };

    static Camtest::StopRecSyncR StopRecSync();


    struct TestSyncR{
        QString msg;
    };

    static TestSyncR TestSync();

    static void AppendLine(QString *msg, const QString &a);

    struct TestRallR{
        QString request;
        QString msg;
        QString err;
    };

    static TestRallR TestCall(const QString &p, const QString &u);

    static QByteArray GetPicture(bool isMvis);
    static QByteArray GetThumbnail();


private:
    static bool DeviceUpdateStorageStatus()
    {
        auto a = _downloadHelper.download("get_storage_status", "");
        return a.content.startsWith("ok");
    }

    static bool DeviceMountStorage()
    {        
        auto a = _downloadHelper.download("set_storage_mount", "");
        return a.content.startsWith("ok");
    }

    static bool DeviceActive()
    {
        auto a = _downloadHelper.download("active", "");
        auto b = a.content=="active";
        return b;
    }

    static QString DeviceRestart()
    {        
        auto a = _downloadHelper.download("restart", "");
        return a.content;
    }
    /*
     * http://10.10.10.150:1997/get_storage_status?images
status:_not_mounted
*/
    static bool DeviceGetStorageStatusImages(){
        auto a = _downloadHelper.download("get_storage_status", "images");
        return a.content.startsWith("ok");
    }
    /*
     * http://10.10.10.150:1997/set_storage_mount?images
ok
/mnt/images*/
    static bool DeviceMountStorageImages(){        
        auto a = _downloadHelper.download("set_storage_mount", "images");
        return a.content.startsWith("ok");
    }

/*
 * http://10.10.10.150:1997/get_cam_status
1;0;1;0;0;0;0;3854;3488
isOpened;isGrabOk;isOpenOk;isRec;isActive;count;interval;total;free
*/
    struct CamStatus{
        bool isOpened;
        bool isGrabOk;
        bool isOpenOk;
        bool isRec;
        bool isActive;//count;interval;total;free
    };

    static CamStatus DeviceGetCamStatus(){
        auto a = _downloadHelper.download("get_cam_status", "");
        auto l = a.content.split(';');
        if(l.length()<5) return {};
        CamStatus r;
        r.isOpened = l[0]=="1";
        r.isGrabOk = l[1]=="1";
        r.isOpenOk = l[2]=="1";
        r.isRec = l[3]=="1";
        r.isActive = l[4]=="1";
        return r;
    }

    static bool DeviceStartRec(const QString& fn, bool isSync = false){
        //if(!_d) return false;
        QString q =
            QStringLiteral("videofolder=%1/front_cam&data_mode=0&file_mode=0&sec=10").arg(fn);
        if(isSync) q+="&sync";
        auto a = _downloadHelper.download("set_rec_start", q);
        return a.content.startsWith("ok");
    }



    static bool DeviceStopRec(){
        //if(!_d) return false;
        auto a = _downloadHelper.download("set_rec_stop", "");
        return a.content.startsWith("ok");
    }
//    static QByteArray GetPicture()
//    {
//        if(!_d) return nullptr;
//        return _d->download("get_pic", "format=jpeg&mode=0");
//    }





    static QString DeviceVersion()
    {
        auto r = _downloadHelper.download("version", "");
        return r.content;
    }

    static QString DeviceUpdate()
    {
        auto r = _downloadHelper.download("update", "");
        return r.content;
    }

    static QString DeviceUpdate4()
    {
        auto r = _downloadHelper.download("update4", "");
        return r.content;
    }

    static QString UploadMetaData(const QString& fn, int len);
    static void UploadData(const QString& key, const QByteArray& a);
    static int UploadNext(const QString& key);

    enum Commands:uchar{
        GetTestR = '@',
        Test = 'A',
        RecStartSync,
        StopRec,
        SetClock
    };

public:

    struct SetIsoR{
        QList<int> ranges;
        QString err;
    };
    static SetIsoR setIso(int i);
    //static void setBrightness(int i);
    //static void setContrast(int i);

    /*iteration*/

    //static int isOkAAA(int i);
    struct SetCamAutoR{
        QString msg;
    };

    static SetCamAutoR SetCamAuto();
    static Camtest::UpdateR Update4();
};


#endif // CAMTEST_H
