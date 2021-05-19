#include "sqlhelper.h"
#include "networkhelper.h"
#include "common/logger/log.h"
#include <QDirIterator>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>

SQLHelper::SQLHelper()
{

}

QString SQLHelper::GetDriverName(){
    auto driverdir = QStringLiteral("/opt/microsoft/msodbcsql17/lib64");
    auto driverpattern = QStringLiteral("^.*libmsodbcsql-?[1-9.so]*$");
    auto driverfi = GetMostRecent(driverdir, driverpattern);
    if(!driverfi.isFile()) return QString();
    return driverfi.absoluteFilePath();
}

//https://docs.microsoft.com/en-us/sql/linux/sql-server-linux-setup-tools?view=sql-server-ver15#ubuntu
QSqlDatabase SQLHelper::Connect(const SQLSettings& s, const QString& name)
{
    QSqlDatabase db;
    const HostPort* h=nullptr;
    for(auto&i:s.hosts)
    {
        if(NetworkHelper::Ping(i.host)) {h=&(i);break;}
    }

    if(h)
    {
        db = QSqlDatabase::addDatabase(s.driver, name);
        auto driverfn = GetDriverName();
        if(driverfn.isEmpty()) return db;
        auto dbname = QStringLiteral("DRIVER=%1;Server=%2,%3;Database=%4").arg(driverfn).arg(h->host).arg(h->port).arg(s.dbname);
        db.setDatabaseName(dbname);
        db.setUserName(s.user);
        db.setPassword(s.password);
        //volatile bool db_ok = db.open();
        //        if(db_ok)
        //        {
        //            zTrace();
        //        }
        //        else
        //        {
        //            auto err = db.lastError().text();
        //            zInfo(err);
        //        }
    }
    return db;
}

void Error(const QSqlError& err)
{
    if(err.isValid()) zInfo(QStringLiteral("QSqlError: %1 - %2").arg(err.type()).arg(err.text()));
}


QFileInfo SQLHelper::GetMostRecent(const QString& path, const QString& pattern)
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
