#ifndef SQLHELPER_H
#define SQLHELPER_H

#include <QVector>
#include <QString>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QVariant>

class SQLHelper
{
public:
    struct HostPort
    {
        QString host;
        int port;
    };

    struct SQLSettings
    {
        QString driver;
        QString dbname;
        QVector<HostPort> hosts;
        QString user;
        QString password;
    };

    SQLHelper();
    QSqlDatabase Connect(const SQLSettings& s, const QString &name);
    static QFileInfo GetMostRecent(const QString &path, const QString &pattern);
    static QString GetDriverName();
};

#endif // SQLHELPER_H
