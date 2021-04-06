#include "camtest.h"
#include "common/helper/ProcessHelper/processhelper.h"
#include "common/helper/string/stringhelper.h"
#include <QDirIterator>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "common/logger/log.h"
#include <QUrl>

//QString Camtest::CamIp = QStringLiteral("http://172.16.3.103:1997");
QUrl Camtest::CamUrl = QUrl(QStringLiteral("http://10.10.10.150:1997"));
com::helper::Downloader Camtest::_d(CamUrl.toString());
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
    Camtest::_d.download("set_cam_settings", s+'='+QString::number(i));
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

Camtest::StartR Camtest::Start(){
    QString cam_ip = CamUrl.host();//QStringLiteral("172.16.3.103"); //beallitasok(ip)
    QString driver = "QODBC";//"QODBC";
    QString dbname = "BuildInfoFlex";
    QString dbhost = "172.16.1.5";//:1433";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";

    if(!Ping(cam_ip)) return {"cannot ping camera at "+cam_ip, "", false,{}};
    auto isActive = ActiveCamera();

    GetCamSettings();

    auto cmd = QStringLiteral(R"(arp -a -n %1)").arg(cam_ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return {"arp error", "", 0, _camSettings};
    // ? (172.16.3.235) at dc:a6:32:74:92:dd [ether] on enp4s0
    if(out.stdOut.isEmpty()) return {"no arp output", "", 0,{}};

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
        if(driverfn.isEmpty()) return {"no db driver error", "", 0, _camSettings};
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
    }
    QSqlDatabase::removeDatabase("conn1");

    return {msg, serial, isActive, _camSettings};
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
    auto a = Camtest::_d.download("get_cam_settings", "");
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

bool Camtest::ClearCamSettings(int id)
{
    auto a = Camtest::_d.download("set_td_d", "id=0&min=-1&max=-1");
    a = Camtest::_d.download("set_td_field", "id=0&x1=-1&y1=-1&&x2=-1&y2=-1");
    a = Camtest::_d.download("set_td_clearfc", "id=0");

    //auto a = Camtest::_d.download("set_td_reset", "&id=0");

    return true;
}

//set_td_d//id//min//max
bool Camtest::SetCalD(int id, qreal dmin, qreal dmax)
{    
    auto q = QStringLiteral("id=%1&min=%2&max=%2").arg(id).arg(dmin).arg(dmax);
    auto a = Camtest::_d.download("set_td_d", q);
    return true;
}

//set_td_field//id//x1//y1//x2//y2

bool Camtest::SetCalF(int id, qreal x0, qreal y0, qreal x1, qreal y1)
{
    auto q = QStringLiteral("id=%1&x1=%2&y1=%2&x2=%3&y2=%4").arg(id).arg(x0).arg(y0).arg(x1).arg(y1);
    auto a = Camtest::_d.download("set_td_d", q);
    return true;
}

//set_td_tracking
//set_td_ftracking
//set_td_clearfc
//set_td_tryaddfc
//set_td_replacefcs

//isOpened;isGrabOk;isOpenOk;isRec;isActive;count;interval[ms];total[MB];free[MB]
Camtest::Status Camtest::GetCamStatus()
{
    Camtest::Status s;
    auto a = Camtest::_d.download("get_cam_status", "");
    if(a.isEmpty()) return s;
    auto b = a.split(';');
    if(b.length()<9) return s;
    s.isValid = true;
    bool isok;
    s.isOpened=b[0].toInt(&isok);
    return s;
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

QByteArray Camtest::GetPicture(bool isMvis)
{
    QString q("format=jpeg&mode=0");
    if(isMvis) q+="&mvis";
    return Camtest::_d.download("get_pic", q);
}


QPixmap Camtest::GetPixmap(bool isMvis)
{
    QByteArray b = Camtest::GetPicture(isMvis);
    QPixmap p;
    if(b.length()>100) p.loadFromData(b,"JPG");
    return p;
}

Camtest::UploadR Camtest::Upload(const QString& fn)
{
    if(!ActiveCamera()) return{"Camera not active"};
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
    QString key = _d.download("upload_meta", "fn="+fn+"len="+QString::number(len));
    return key;
}

void Camtest::UploadData(const QString& key, const QByteArray& data){
    QString err;
    _d.post("upload", "key="+key, &err, data);
}

// UploadNext visszaküldi a fogadott length-t
//0: kezdődik
//>0 folyamatben
//-2 hiba volt, újra
int Camtest::UploadNext(const QString& key){
    QString b = _d.download("upload_length", "key="+key);
    //b nem lehet busy, timeout vagy egyéb balgaság
    bool ok;
    int ix = b.toInt(&ok);
    return ok?ix:-2;
}
