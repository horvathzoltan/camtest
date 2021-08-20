#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H

#include <QHostAddress>
#include <QString>
#include <QUrl>

class NetworkHelper
{
public:
    NetworkHelper(){};
    static bool Ping(const QString &ip);

    static bool sendUDP(QHostAddress addr, int port, const QByteArray& datagram);

    static QHostAddress BroadcastAddress(const QUrl& url);
};

#endif // NETWORKHELPER_H
