#include "camtest.h"
#include "common/helper/ProcessHelper/processhelper.h"
#include "common/helper/string/stringhelper.h"
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "common/logger/log.h"

com::helper::Downloader Camtest::_d = com::helper::Downloader(QStringLiteral("http://172.16.3.136:1997"));
// ping cél ip
// ha ok, akkor arp -a cél ip -> mac addr
// ha ismeretlen mac
Camtest::StartR Camtest::Start(){
    QString cam_ip = QStringLiteral("172.16.3.136"); //beallitasok(ip)
    QString driver = "QODBC";//"QODBC";
    QString dbname = "BuildInfoFlex";
    QString dbhost = "172.16.1.5";//:1433";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";

    if(!Ping(cam_ip)) return {"ping error", "", 0};
    auto isActive = ActiveCamera();

    auto cmd = QStringLiteral(R"(arp -a -n %1)").arg(cam_ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return {"arp error", "", 0};
    // ? (172.16.3.235) at dc:a6:32:74:92:dd [ether] on enp4s0
    if(out.stdOut.isEmpty()) return {"no arp output", "", 0};

    QString& x = out.stdOut;
    QRegularExpression re(QStringLiteral(R"(at\s+((?:[0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2})\s+)"));
    QRegularExpressionMatch m = re.match(x);

    QString mac;
    if (m.hasMatch())
    {
        mac = m.captured(m.lastCapturedIndex());
    }    

    QString msg;
    QString serial = "";

    append_value(&msg, cam_ip);
    append_value(&msg, mac);
    append_value(&msg, isActive);

    auto isDB = Ping(dbhost);
    if(isDB)
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver, "conn1");
        db.setHostName(dbhost);
        db.setDatabaseName(QStringLiteral("DRIVER=/opt/microsoft/msodbcsql17/lib64/libmsodbcsql-17.7.so.1.1;Server=%1;Database=%2").arg(dbhost).arg(dbname));
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

    return {msg, serial, isActive};
}

bool Camtest::Ping(const QString& ip, int port){
    Q_UNUSED(port)
    auto cmd = QStringLiteral(R"(ping -c1 -W1 %1)").arg(ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode) return false;
    //test ping port
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
