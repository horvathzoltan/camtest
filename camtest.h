#ifndef CAMTEST_H
#define CAMTEST_H

#include <QSqlDatabase>
#include <QString>

class Camtest
{
public:
    struct StartR{
        QString msg;
    };
    static StartR Start();
    struct StopR{};
    static StopR Stop();
    static QString NewSerial(const QSqlDatabase &db);
    static bool Ping(const QString &ip, int port=-1);
};

#endif // CAMTEST_H
