#include "camtest.h"
#include "networkhelper.h"
#include "settings.h"
#include "sqlhelper.h"
#include "common/helper/ProcessHelper/processhelper.h"
#include "common/helper/string/stringhelper.h"
#include <QBuffer>
#include <QDirIterator>
#include <QNetworkInterface>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include "common/logger/log.h"
#include <QUrl>
#include <QtEndian>

extern Settings _settings;


//QString Camtest::CamIp = QStringLiteral("http://172.16.3.103:1997");
QUrl Camtest::CamUrl;// = QUrl(QStringLiteral("http://172.16.3.103:1997"));
com::helper::Downloader* Camtest::_d = nullptr;
Camtest::CamSettings Camtest::_camSettings;
// ping cél ip
// ha ok, akkor arp -a cél ip -> mac addr
// ha ismeretlen mac

//QString Camtest::GetDriverName(){
//    auto driverdir = QStringLiteral("/opt/microsoft/msodbcsql17/lib64");
//    auto driverpattern = QStringLiteral("^.*libmsodbcsql-?[1-9.so]*$");
//    auto driverfi = GetMostRecent(driverdir, driverpattern);
//    if(!driverfi.isFile()) return QString();
//    return driverfi.absoluteFilePath();
//}

//QFileInfo Camtest::GetMostRecent(const QString& path, const QString& pattern)
//{
//    QFileInfo most_recent;
//    auto tstamp = QDateTime(QDate(1980,1,1));// ::currentDateTimeUtc().addYears(-1);//f1.lastModified();
//    QRegularExpression re(pattern);

//    QDirIterator it(path);
//    while (it.hasNext()) {
//        auto fn = it.next();
//        QFileInfo fi(fn);
//        if(!fi.isFile()) continue;
//        auto m = re.match(fn);
//        if(!m.hasMatch()) continue;

//        auto ts = fi.lastModified();
//        if(ts>tstamp){ tstamp=ts; most_recent = fi;}
//    }
//    return most_recent;
//}

auto Camtest::setCamSettings(const QString& s, int i) -> int
{
    if(!_d) return -1;
    Camtest::_d->download(QStringLiteral("set_cam_settings"), s+'='+QString::number(i));
    return i;
}

auto Camtest::brightnest_p() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("brightness"),++_camSettings.brightness);
}

auto Camtest::brightnest_m() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("brightness"),--_camSettings.brightness);
}

auto Camtest::contrast_p() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("contrast"),++_camSettings.contrast);
}

auto Camtest::contrast_m() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("contrast"),--_camSettings.contrast);
}

auto Camtest::saturation_p() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("saturation"),++_camSettings.saturation);
}

auto Camtest::saturation_m() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("saturation"),--_camSettings.saturation);
}

auto Camtest::iso_p() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("iso"),_camSettings.iso+=50);
}

auto Camtest::iso_m() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("iso"),_camSettings.iso-=50);
}
/**/

auto Camtest::wb_p() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("awb"),++_camSettings.wb);
}

auto Camtest::wb_m() -> int
{
    GetCamSettings();
    return setCamSettings(QStringLiteral("awb"),--_camSettings.wb);
}
/**/

auto Camtest::GetIp(uchar i1, uchar i2, quint16 p) -> QStringList
{
    if(i1>0 && i2<255 && i1>=i2) return {};
    QTcpSocket socket;
    QStringList l;
    for(uchar i = i1;i<=i2;i++)
    {
        auto ip = "10.10.10."+QString::number(i);
        socket.connectToHost(ip, p);
        if(socket.waitForConnected(3))
        {
            socket.disconnectFromHost();
            l.append(ip);
        }
    }
    return l;
}

QT_DEPRECATED
auto Camtest::GetIp_old(int i1, int i2, int p) -> QStringList
{
    if(i1<1||i1>255) return {};
    if(i2<1||i2>255) return {};
    if(i1>i2) return {};
    if(p<1||p>UINT16_MAX) return {};

    auto cmd = QStringLiteral(R"(nmap -Pn -p%3 --open 10.10.10.%1-%2)").arg(i1).arg(i2).arg(p);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode) return {};

    static const QRegularExpression r(QStringLiteral(R"(Nmap scan report for\s+(?:\S+\s+)?\(?([0-9\.]+)\)?)"));
    auto mi = r.globalMatch(out.stdOut);

    QStringList e;
    while (mi.hasNext())
    {
        auto a = mi.next().captured(1);
        e.append(a);
    }

    return e;
}

void Camtest::FilterLocalIp(QStringList *l)
{
    auto lip = QNetworkInterface::allAddresses();

    for(auto&i:lip) l->removeAll(i.toString());
}


auto Camtest::Start() -> Camtest::StartR{
    qint16 cam_p = 1997;
    auto iplist=GetIp(100,130,cam_p);
    iplist.append(GetIp(150,155,cam_p));
    FilterLocalIp(&iplist);

    QString cam_ip;

    if(iplist.count()>0) { cam_ip= iplist[0];
    } else cam_ip=QStringLiteral("10.10.10.150");//CamUrl.host();//QStringLiteral("172.16.3.103"); //beallitasok(ip)

    CamUrl = QUrl(QStringLiteral("http://%1:%2").arg(cam_ip).arg(cam_p));

    delete _d;
    _d = new com::helper::Downloader(CamUrl.toString());

    QString driver = QStringLiteral("QODBC");//"QODBC";
    QString dbname = QStringLiteral("BuildInfoFlex");
    QString dbhost = QStringLiteral("172.16.1.5");//:1433";
    QString user = QStringLiteral("sa");
    QString password= QStringLiteral("Gtr7jv8fh2");

    if(!NetworkHelper::Ping(cam_ip)) return {QStringLiteral("cannot ping camera at ")+cam_ip, "", false, "",{}};
    auto isActive = DeviceActive();
    auto version = DeviceVersion();
    GetCamSettings();

    auto cmd = QStringLiteral(R"(arp -a -n %1)").arg(cam_ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return {QStringLiteral("arp error"), "", false, version,_camSettings};
    // ? (172.16.3.235) at dc:a6:32:74:92:dd [ether] on enp4s0
    if(out.stdOut.isEmpty()) return {QStringLiteral("no arp output"), "", false, version, {}};

    QString& x = out.stdOut;
    QRegularExpression re(QStringLiteral(R"(at\s+((?:[0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2})\s+)"));
    QRegularExpressionMatch m = re.match(x);

    QString mac;
    if (m.hasMatch())
    {
        mac = m.captured(m.lastCapturedIndex());
    }    

    QString msg;
    QString serial;

    append_value(&msg, cam_ip);
    append_value(&msg, mac);
    append_value(&msg, isActive);

    SQLHelper sqlh;
    static const QString CONN = QStringLiteral("conn1");
    auto db = sqlh.Connect(_settings._sql_settings, CONN);

    //auto isDB = Ping(dbhost);
    if(db.isValid())
    {
        /*QSqlDatabase db = QSqlDatabase::addDatabase(driver, QStringLiteral("conn1"));
        db.setHostName(dbhost);

        int port = 1433;
        auto driverfn = GetDriverName();
        if(driverfn.isEmpty()) return {"no db driver error", "", 0,version, _camSettings};
        db.setDatabaseName(QStringLiteral("DRIVER=%1;Server=%2,%3;Database=%4").arg(driverfn).arg(dbhost).arg(port).arg(dbname));


        //db.setDatabaseName(QStringLiteral("DRIVER=/opt/microsoft/msodbcsql17/lib64/libmsodbcsql-17.7.so.1.1;Server=%1;Database=%2").arg(dbhost).arg(dbname));
        db.setUserName(user);
        db.setPassword(password);
        */
        bool db_ok = db.open();
        QString dberr(QLatin1String(""));
        QString project = QStringLiteral("4");//mestercipo
        QString board_rev = QStringLiteral("a");//a válzozat
        bool isNew = false;
        int rows=0;
        if(db_ok)
        {
            QSqlQuery query(db);
            db_ok = query.exec(QStringLiteral("SELECT id, serial FROM ManufacturingInfo WHERE mac = '%1'").arg(mac));
            if(db_ok)
            {
                //int fieldNo = query.record().indexOf("country");
                rows = 0;
                while(query.next()){
                    rows++;
                    serial = query.value(1).toString();
                    break;
                }
                if(!rows){
                    isNew = true;
                    serial = NewSerial(db);

                    QSqlQuery query2(db);
                    db_ok = query2.exec(QStringLiteral("INSERT into ManufacturingInfo (mac,serial,project,board_rev) VALUES('%1','%2', '%3', '%4')")
                                .arg(mac).arg(serial).arg(project).arg(board_rev));

                }
            }
        }

        if(!db_ok)
        {
            QSqlError a = db.lastError();
            dberr = a.text().trimmed();
        }
        db.close();


        append_value(&msg, db_ok);
        append_value(&msg, dberr);
        append_value(&msg, isNew);
        append_value(&msg, serial);
        append_value(&msg, rows);
        append_value(&msg, version);
    }
    QSqlDatabase::removeDatabase(QStringLiteral("conn1"));

    return {msg, serial, isActive, version, _camSettings};
}

//bool Camtest::Ping(const QString& ip, int port){
//    Q_UNUSED(port)
//    auto cmd = QStringLiteral(R"(ping -c1 -W1 %1)").arg(ip);
//    auto out = com::helper::ProcessHelper::Execute(cmd);
//    if(out.exitCode) return false;
//    //test ping port
//    return true;
//}

auto Camtest::GetCamSettings() -> bool
{
    auto a = _d->download(QStringLiteral("get_cam_settings"), QLatin1String(""));
    if(a.isEmpty()) return false;
    auto b = a.split(';');
    if(b.length()<5) return false;
    bool isok;
    _camSettings.brightness=b[0].toInt(&isok);
    _camSettings.contrast=b[1].toInt(&isok);
    _camSettings.saturation=b[2].toInt(&isok);
    _camSettings.iso=b[3].toInt(&isok);
    _camSettings.wb=b[4].toInt(&isok);

    return true;
}

auto Camtest::ClearCamSettings(int id) -> bool
{
    Q_UNUSED(id)

    auto a = Camtest::_d->download(QStringLiteral("set_td_d"), QStringLiteral("id=0&min=-1&max=-1"));
    a = Camtest::_d->download(QStringLiteral("set_td_field"), QStringLiteral("id=0&x1=-1&y1=-1&&x2=-1&y2=-1"));
    a = Camtest::_d->download(QStringLiteral("set_td_clearfc"), QStringLiteral("id=0"));

    //auto a = Camtest::_d.download("set_td_reset", "&id=0");

    return true;
}

//set_td_d//id//min//max
auto  Camtest::SetCalD(int id, qreal dmin, qreal dmax) -> bool
{    
    auto q = QStringLiteral("id=%1&min=%2&max=%2").arg(id).arg(dmin).arg(dmax);
    auto a = Camtest::_d->download(QStringLiteral("set_td_d"), q);
    return true;
}

//set_td_field//id//x1//y1//x2//y2

auto Camtest::SetCalF(int id, qreal x0, qreal y0, qreal x1, qreal y1) -> bool
{
    auto q = QStringLiteral("id=%1&x1=%2&y1=%2&x2=%3&y2=%4").arg(id).arg(x0).arg(y0).arg(x1).arg(y1);
    auto a = Camtest::_d->download(QStringLiteral("set_td_d"), q);
    return true;
}

//set_td_tracking
//set_td_ftracking
//set_td_clearfc
//set_td_tryaddfc
//set_td_replacefcs

//isOpened;isGrabOk;isOpenOk;isRec;isActive;count;interval[ms];total[MB];free[MB]
auto Camtest::GetCamStatus() -> Camtest::Status
{
    Camtest::Status s;
    auto a = Camtest::_d->download(QStringLiteral("get_cam_status"), QLatin1String(""));
    if(a.isEmpty()) return s;
    auto b = a.split(';');
    if(b.length()<9) return s;
    s.isValid = true;
    bool isok;
    s.isOpened=b[0].toInt(&isok);
    return s;
}

auto Camtest::NewSerial(const QSqlDatabase& db) -> QString
{
    QSqlQuery q(db);
    auto db_ok = q.exec(QStringLiteral("SELECT MAX(serial) AS maxserial FROM ManufacturingInfo"));
    if(!db_ok) return QString();
    int serial;
    q.first();
    auto a = q.value(0);
    if(a.isNull()) serial = 1001; else serial = a.toInt()+1;
    return QString::number(serial);
}

auto Camtest::Stop() -> Camtest::StopR {return {};}

auto Camtest::GetPicture(bool isMvis)-> QByteArray
{
    QString q(QStringLiteral("format=jpeg&mode=0"));
    if(isMvis) q+=QStringLiteral("&mvis");
    auto a =  Camtest::_d->download(QStringLiteral("get_pic"), q);
    return a;
}


auto Camtest::GetPixmap(bool isMvis) -> QPixmap
{
    QByteArray b = GetPicture(isMvis);
    QPixmap p;

    if(b.length()>100){
        p.loadFromData(b,"JPG");
    } else {
        auto btxt = QString(b);
        zInfo("getpic: "+btxt);
        static auto const error = QStringLiteral("error:");
        if(btxt.startsWith(error)){
            auto errortxt = btxt.mid(error.length());
            if(errortxt=="not_opened"){
                Camtest::OpenCamera();
            }
        }
    }
    return p;
}

auto Camtest::Upload(const QString& fn) -> Camtest::UploadR
{
    Q_UNUSED(fn)
    if(!DeviceActive()) return{"Camera not active"};
    QByteArray a(100, 0);
    for(int i=0;i<100;i++)a[i]=i;

    //fn = "kurutty.txt";
    //felküldjük, hogy mi fog átmenni
    int maxlen = a.length();
    int slen = 16;

    auto key = UploadMetaData(QStringLiteral("kurutty.txt"), maxlen);

    if(key.isEmpty()){ zInfo(QStringLiteral("key is empty")); return {QStringLiteral("key is empty")};}

    zInfo("key:"+key);
    //
    bool hasNext=true;
    int ix = 0;
    //while(hasNext)
    {
        // UploadNext visszaküldi a fogadott length-t
        ix = UploadNext(key); //ennyi ment át
        auto len = maxlen-ix;// ennyi a küldendő
        hasNext = len>0;
        if(hasNext) // átment
        {
            auto data = a.mid(ix, (len>slen)?slen:len); // egy szegmens hosszt átküldünk
            UploadData(key, data);
        }
    }
    return {"ok"};
}

// minden fájl küldéskor kap egy kulcsot
auto Camtest::UploadMetaData(const QString& fn, int len) -> QString
{
    QString key = _d->download(QStringLiteral("upload_meta"), "fn="+fn+"len="+QString::number(len));
    return key;
}

void Camtest::UploadData(const QString& key, const QByteArray& data){
    QString err;
    _d->post(QStringLiteral("upload"), "key="+key, &err, data);
}

// UploadNext visszaküldi a fogadott length-t
//0: kezdődik
//>0 folyamatben
//-2 hiba volt, újra
auto Camtest::UploadNext(const QString& key) -> int
{
    QString b = _d->download(QStringLiteral("upload_length"), QStringLiteral("key=")+key);
    //b nem lehet busy, timeout vagy egyéb balgaság
    bool ok;
    int ix = b.toInt(&ok);
    return ok?ix:-2;
}


auto Camtest::Update() -> Camtest::UpdateR
{
    QString msg;

    auto a = DeviceActive();
    if(!a)
    {
        com::helper::StringHelper::AppendLine(&msg, QStringLiteral("not active"));
        return {false, msg};
    }

    auto v_old = DeviceVersion();

    append_value(&msg, v_old);

    auto updstatus = DeviceUpdateStorageStatus();
    if(!updstatus) updstatus=DeviceMountStorage();
    if(!updstatus){
        com::helper::StringHelper::AppendLine(&msg, QStringLiteral("cannot mount"));
        return {false, msg};
    }

    DeviceUpdate();

    int i;
    QString v_new;
    bool isok;
    for(i=0;i<10;i++)
    {
        QThread::sleep(3);
        v_new = DeviceVersion();
        if(v_new.isEmpty()) continue;
        isok = v_new!=v_old;
        if(isok) break;
    }

    append_value(&msg, v_new);
    append_value(&msg, i);

    return {isok, msg};
}


auto Camtest::Restart() -> Camtest::RestartR
{
    QString msg;

    auto a = DeviceActive();
    if(!a)
    {
        com::helper::StringHelper::AppendLine(&msg, QStringLiteral("not active"));
        return {false, msg};
    }

    DeviceRestart();

    int i;
    bool ping;

    auto host = CamUrl.host();
    for(i=0;i<10;i++)
    {
        QThread::sleep(3);
        ping = NetworkHelper::Ping(host);
        if(!ping) break;
    }

    if(i>10) msg+=QStringLiteral("Timeout\n");

    return {!ping, msg};
}

auto Camtest::StartRecording() -> Camtest::StartRecR
{
    Camtest::StartRecR r;
    if(!_d) return {"no camera"};
    bool isactive = DeviceActive();
    if(!isactive) return {"device is not active"};
    auto storagestatus = DeviceGetStorageStatusImages();
    if(!storagestatus){
        auto mountstatus = DeviceMountStorageImages();
        if(!mountstatus) {
            return {"cannot mount storage 'images'"};
        }
    }

    auto status = DeviceGetCamStatus();

    if(!status.isOpened){
        auto isopen = OpenCamera();
        if(!isopen) return {"cannot open"};
        status = DeviceGetCamStatus();
    }
    if(status.isRec) return {"recording in progress"};
    if(status.isActive) return {"recording timer is active"};

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    QString fn = "manymany_"+timestamp;
    auto isrecok = DeviceStartRec(fn);
    if(!isrecok) return{"cannot start recording"};
    r.msg = "recording started: "+ fn;
    return r;
}

auto Camtest::StopRecording() -> StopRecR
{
    if(!_d) return {"no camera"};
    bool isactive = DeviceActive();
    if(!isactive) return {"device is not active"};
    auto a = DeviceStopRec();
    if(!a) return {"cannot stop recording"};
    return {"recording stopped"};
}

auto Camtest::StartRecSync() -> Camtest::StartRecSyncR
{
    StartRecSyncR r;
    if(!_d) return {"no camera"};
    bool isactive = DeviceActive();
    if(!isactive) return {"device is not active"};
    auto storagestatus = DeviceGetStorageStatusImages();
    if(!storagestatus){
        auto mountstatus = DeviceMountStorageImages();
        if(!mountstatus) {
            return {"cannot mount storage 'images'"};
        }
    }

    auto status = DeviceGetCamStatus();

    if(!status.isOpened){
        auto isopen = OpenCamera();
        if(!isopen) return {"cannot open"};
        status = DeviceGetCamStatus();
    }
    if(status.isRec) return {"recording in progress"};
    if(status.isActive) return {"recording timer is active"};

    auto most = QDateTime::currentDateTime();
    QString timestamp = most.toString("yyyyMMdd-hhmmss");
    auto timestampMs = most.toString(Qt::ISODateWithMs);
    QString fn = "sync_many_"+timestamp;
    auto isrecok = DeviceStartRec(fn, true);
    if(!isrecok) return{"cannot start recording"};

    qint64 t = 3000;
    if(QSysInfo::ByteOrder==QSysInfo::Endian::BigEndian) t = qToLittleEndian(t);
    QByteArray a;
    a.append(Commands::RecStartSync);
    a.append(reinterpret_cast<char*>(&t), sizeof(t));

    QHostAddress aa = NetworkHelper::BroadcastAddress(CamUrl);

    //QElapsedTimer et;
    //et.start();
    static const int PMAX = 2;
    static const int BMAX = 5;
    unsigned long PTIME = 50;
    unsigned long BTIME = 10;

    for(int i=0;i<PMAX;i++){
        for(int j=0;j<BMAX;j++){
            NetworkHelper::sendUDP(aa, 1998, a);
            QThread::msleep(1<BMAX-j?BTIME:PTIME);
        }
    }

    status = DeviceGetCamStatus();
    if(status.isRec) AppendLine(&r.msg,
                   QStringLiteral("recording in progress"));
    if(status.isActive) AppendLine(&r.msg,
                   QStringLiteral("recording timer is active"));
    AppendLine(&r.msg, fn);
    AppendLine(&r.msg, timestampMs);

    return r;
}

auto Camtest::StopRecSync() -> Camtest::StopRecSyncR
{
    StopRecSyncR r;
    if(!_d) return {"no camera"};
//    bool isactive = DeviceActive();
//    if(!isactive) return {"device is not active"};
//    auto storagestatus = DeviceGetStorageStatusImages();
//    if(!storagestatus){
//        auto mountstatus = DeviceMountStorageImages();
//        if(!mountstatus) {
//            return {"cannot mount storage 'images'"};
//        }
//    }

//    auto status = DeviceGetCamStatus();

//    if(!status.isOpened){
//        auto isopen = OpenCamera();
//        if(!isopen) return {"cannot open"};
//        status = DeviceGetCamStatus();
//    }
//    if(status.isRec) return {"recording in progress"};
//    if(status.isActive) return {"recording timer is active"};

//    auto most = QDateTime::currentDateTime();
//    QString timestamp = most.toString("yyyyMMdd-hhmmss");
//    auto timestampMs = most.toString(Qt::ISODateWithMs);
//    QString fn = "sync_many_"+timestamp;
//    auto isrecok = DeviceStartRec(fn, true);
//    if(!isrecok) return{"cannot start recording"};

    //qint64 t = 3000;
    //if(QSysInfo::ByteOrder==QSysInfo::Endian::BigEndian) t = qToLittleEndian(t);
    QByteArray a;
    a.append(Commands::StopRec);
    //a.append(reinterpret_cast<char*>(&t), sizeof(t));

    QHostAddress aa = NetworkHelper::BroadcastAddress(CamUrl);

    //QElapsedTimer et;
    //et.start();
    static const int PMAX = 2;
    static const int BMAX = 5;
    unsigned long PTIME = 50;
    unsigned long BTIME = 10;

    for(int i=0;i<PMAX;i++){
        for(int j=0;j<BMAX;j++){
            NetworkHelper::sendUDP(aa, 1998, a);
            QThread::msleep(1<BMAX-j?BTIME:PTIME);
        }
    }

//    status = DeviceGetCamStatus();
//    if(status.isRec) AppendLine(&r.msg,
//                   QStringLiteral("recording in progress"));
//    if(status.isActive) AppendLine(&r.msg,
//                   QStringLiteral("recording timer is active"));
//    AppendLine(&r.msg, fn);
//    AppendLine(&r.msg, timestampMs);

    return r;
}

void Camtest::AppendLine(QString *msg, const QString& a)
{
    if(!msg)return;
    if(!msg->isEmpty())*msg+='\n';
    *msg+=a;
}

auto Camtest::TestSync() -> TestSyncR
{
    if(!_d) return {"no camera"};

    QHostAddress b = NetworkHelper::BroadcastAddress(CamUrl);
    QString aa = QStringLiteral("10.10.10.255");
    QByteArray a;
    a.append(Commands::Test);
    NetworkHelper::sendUDP(b, 1998, a);
    return {"test sent"};
}
