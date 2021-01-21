#include "camtest.h"
#include "common/helper/ProcessHelper/processhelper.h"
#include "common/helper/string/stringhelper.h"
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

// ping cél ip
// ha ok, akkor arp -a cél ip -> mac addr
// ha ismeretlen mac
Camtest::StartR Camtest::Start(){
    QString ip = QStringLiteral("172.16.3.235"); //beallitasok(ip)
    QString driver = "QODBC";//"QODBC";
    QString dbname = "BuildInfoFlex";
    QString host = "172.16.1.5";//:1433";
    QString user = "sa";
    QString password= "Gtr7jv8fh2";

    if(!Ping(ip)) return {"ping error"};
    if(!Ping(host)) return {"db ping error"};

    auto cmd = QStringLiteral(R"(arp -a -n %1)").arg(ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return {"arp error"};
    // ? (172.16.3.235) at dc:a6:32:74:92:dd [ether] on enp4s0
    if(out.stdOut.isEmpty()) return {"no arp output"};

    QString& x = out.stdOut;
    QRegularExpression re(QStringLiteral(R"(at\s+((?:[0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2})\s+)"));
    QRegularExpressionMatch m = re.match(x);

    QString mac;
    if (m.hasMatch())
    {
        mac = m.captured(m.lastCapturedIndex());
    }    

    QSqlDatabase db = QSqlDatabase::addDatabase(driver, "conn1");
    db.setHostName(host);
    db.setDatabaseName(QStringLiteral("DRIVER=/opt/microsoft/msodbcsql17/lib64/libmsodbcsql-17.6.so.1.1;Server=%1;Database=%2").arg(host).arg(dbname));
    db.setUserName(user);
    db.setPassword(password);
    bool db_ok = db.open();
    QString dberr="";
    QString serial = "";
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

    db.removeDatabase("conn1");
    QString msg;
    append_value(&msg, ip);
    append_value(&msg, mac);
    append_value(&msg, db_ok);
    append_value(&msg, dberr);
    append_value(&msg, isNew);
    append_value(&msg, serial);
    append_value(&msg, rows);
    return {msg};    
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
