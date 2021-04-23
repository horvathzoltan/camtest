#include "camtest.h"
#include "common/helper/ProcessHelper/processhelper.h"
#include "common/helper/string/stringhelper.h"
#include <QDirIterator>
#include <QNetworkInterface>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include "common/logger/log.h"
#include <QUrl>

//QString Camtest::CamIp = QStringLiteral("http://172.16.3.103:1997");
QUrl Camtest::CamUrl;// = QUrl(QStringLiteral("http://172.16.3.103:1997"));
com::helper::Downloader* Camtest::_d = nullptr;
Camtest::CamSettings Camtest::_camSettings;
// ping cél ip
// ha ok, akkor arp -a cél ip -> mac addr
// ha ismeretlen mac

QString Camtest::GetDriverName(){
    auto driverdir = QStringLiteral("/opt/microsoft/msodbcsql17/lib64");
    auto driverpattern = QStringLiteral("^.*libmsodbcsql-?[1-9.so]*$");
    auto driverfi = GetMostRecent(driverdir, driverpattern);
    if(!driverfi.isFile()) return QString();
    return driverfi.absoluteFilePath();
}

QFileInfo Camtest::GetMostRecent(const QString& path, const QString& pattern)
{
    QFileInfo most_recent;
    auto tstamp = QDateTime(QDate(1980,1,1));// ::currentDateTimeUtc().addYears(-1);//f1.lastModified();
    QRegularExpression re(pattern);

    QDirIterator it(path);
    while (it.hasNext()) {
        auto fn = it.next();
        QFileInfo fi(fn);
        if(!fi.isFile()) continue;
        auto m = re.match(fn);
        if(!m.hasMatch()) continue;

        auto ts = fi.lastModified();
        if(ts>tstamp){ tstamp=ts; most_recent = fi;}
    }
    return most_recent;
}

int Camtest::setCamSettings(const QString& s, int i)
{
    if(!_d) return -1;
    Camtest::_d->download("set_cam_settings", s+'='+QString::number(i));
    return i;
}

int Camtest::brightnest_p()
{
    GetCamSettings();
    return setCamSettings("brightness",++_camSettings.brightnest);
}

int Camtest::brightnest_m()
{
    GetCamSettings();
    return setCamSettings("brightness",--_camSettings.brightnest);
}

int Camtest::contrast_p()
{
    GetCamSettings();
    return setCamSettings("contrast",++_camSettings.contrast);
}

int Camtest::contrast_m()
{
    GetCamSettings();
    return setCamSettings("contrast",--_camSettings.contrast);
}

int Camtest::saturation_p()
{
    GetCamSettings();
    return setCamSettings("saturation",++_camSettings.saturation);
}

int Camtest::saturation_m()
{
    GetCamSettings();
    return setCamSettings("saturation",--_camSettings.saturation);
}

int Camtest::gain_p()
{
    GetCamSettings();
    return setCamSettings("gain",++_camSettings.gain);
}

int Camtest::gain_m()
{
    GetCamSettings();
    return setCamSettings("gain",--_camSettings.gain);
}
/**/

int Camtest::wb_p()
{
    GetCamSettings();
    return setCamSettings("awb",++_camSettings.wb);
}

int Camtest::wb_m()
{
    GetCamSettings();
    return setCamSettings("awb",--_camSettings.wb);
}
/**/

QStringList Camtest::GetIp(int i1, int i2, int p)
{
    if(i1<1||i1>255) return {};
    if(i2<1||i2>255) return {};
    if(i1>i2) return {};
    if(p<1||p>UINT16_MAX) return {};

    auto cmd = QStringLiteral(R"(nmap -Pn -p%3 --open 10.10.10.%1-%2)").arg(i1).arg(i2).arg(p);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode) return {};

    static const QRegularExpression r(R"(Nmap scan report for\s+(?:\S+\s+)?\(?([0-9\.]+)\)?)");
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


Camtest::StartR Camtest::Start(){
    int cam_p = 1997;
    auto iplist=GetIp(100,155,cam_p);
    FilterLocalIp(&iplist);

    QString cam_ip;

    if(iplist.count()>0) cam_ip= iplist[0];
    else cam_ip=QStringLiteral("10.10.10.150");//CamUrl.host();//QStringLiteral("172.16.3.103"); //beallitasok(ip)

    CamUrl = QUrl(QStringLiteral("http://%1:%2").arg(cam_ip).arg(cam_p));

    if(_d) delete _d;
    _d = new com::helper::Downloader(CamUrl.toString());

    QString driver = "QODBC";//"QODBC";
    QString dbname = "BuildInfoFlex";
    QString dbhost = "172.16.1.5";//:1433";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";

    if(!Ping(cam_ip)) return {"cannot ping camera at "+cam_ip, "", 0, "",{}};
    auto isActive = DeviceActive();
    auto version = DeviceVersion();
    GetCamSettings();

    auto cmd = QStringLiteral(R"(arp -a -n %1)").arg(cam_ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return {"arp error", "", 0, version,_camSettings};
    // ? (172.16.3.235) at dc:a6:32:74:92:dd [ether] on enp4s0
    if(out.stdOut.isEmpty()) return {"no arp output", "", 0, version, {}};

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

    auto isDB = Ping(dbhost);
    if(isDB)
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver, QStringLiteral("conn1"));
        db.setHostName(dbhost);

        int port = 1433;
        auto driverfn = GetDriverName();
        if(driverfn.isEmpty()) return {"no db driver error", "", 0,version, _camSettings};
        db.setDatabaseName(QStringLiteral("DRIVER=%1;Server=%2,%3;Database=%4").arg(driverfn).arg(dbhost).arg(port).arg(dbname));


        //db.setDatabaseName(QStringLiteral("DRIVER=/opt/microsoft/msodbcsql17/lib64/libmsodbcsql-17.7.so.1.1;Server=%1;Database=%2").arg(dbhost).arg(dbname));
        db.setUserName(user);
        db.setPassword(password);
        bool db_ok = db.open();
        QString dberr="";
        QString project = "00";//mestercipo
        QString board_rev = "a";//a válzozat
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
    QSqlDatabase::removeDatabase("conn1");

    return {msg, serial, isActive, version, _camSettings};
}

bool Camtest::Ping(const QString& ip, int port){
    Q_UNUSED(port)
    auto cmd = QStringLiteral(R"(ping -c1 -W1 %1)").arg(ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode) return false;
    //test ping port
    return true;
}

bool Camtest::GetCamSettings()
{
    auto a = _d->download("get_cam_settings", "");
    if(a.isEmpty()) return false;
    auto b = a.split(';');
    if(b.length()<5) return false;
    bool isok;
    _camSettings.brightnest=b[0].toInt(&isok);
    _camSettings.contrast=b[1].toInt(&isok);
    _camSettings.saturation=b[2].toInt(&isok);
    _camSettings.gain=b[3].toInt(&isok);
    _camSettings.wb=b[4].toInt(&isok);

    return true;
}

QString Camtest::NewSerial(const QSqlDatabase& db){
    QSqlQuery q(db);
    auto db_ok = q.exec(QStringLiteral("SELECT MAX(serial) AS maxserial FROM ManufacturingInfo"));
    if(!db_ok) return QString();
    int serial;
    q.first();
    auto a = q.value(0);
    if(a.isNull()) serial = 1001; else serial = a.toInt()+1;
    return QString::number(serial);
}

Camtest::StopR Camtest::Stop(){return {};}

QPixmap Camtest::GetPixmap()
{
    QByteArray b = Camtest::GetPicture();
    QPixmap p;
    if(b.length()>100) p.loadFromData(b,"JPG");
    return p;
}

Camtest::UploadR Camtest::Upload(const QString& fn)
{
    if(!DeviceActive()) return{"Camera not active"};
    QByteArray a(100, 0);
    for(int i=0;i<100;i++)a[i]=i;

    //fn = "kurutty.txt";
    //felküldjük, hogy mi fog átmenni
    int maxlen = a.length();
    int slen = 16;

    auto key = UploadMetaData("kurutty.txt", maxlen);

    if(key.isEmpty()){ zInfo("key is empty"); return {"key is empty"};}

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
QString Camtest::UploadMetaData(const QString& fn, int len){
    QString key = _d->download("upload_meta", "fn="+fn+"len="+QString::number(len));
    return key;
}

void Camtest::UploadData(const QString& key, const QByteArray& data){
    QString err;
    _d->post("upload", "key="+key, &err, data);
}

// UploadNext visszaküldi a fogadott length-t
//0: kezdődik
//>0 folyamatben
//-2 hiba volt, újra
int Camtest::UploadNext(const QString& key){
    QString b = _d->download("upload_length", "key="+key);
    //b nem lehet busy, timeout vagy egyéb balgaság
    bool ok;
    int ix = b.toInt(&ok);
    return ok?ix:-2;
}


Camtest::UpdateR Camtest::Update()
{
    QString msg;

    auto a = DeviceActive();
    if(!a)
    {
        com::helper::StringHelper::AppendLine(&msg, "not active");
        return {false, msg};
    }

    auto v_old = DeviceVersion();

    append_value(&msg, v_old);

    auto updstatus = DeviceUpdateStorageStatus();
    if(!updstatus) updstatus=DeviceMountStorage();
    if(!updstatus){
        com::helper::StringHelper::AppendLine(&msg, "cannot mount");
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
