#include "networkhelper.h"
#include "common/helper/ProcessHelper/processhelper.h"

#include <QUdpSocket>


auto NetworkHelper::Ping(const QString& ip) -> bool
{
    auto cmd = QStringLiteral(R"(ping -c1 -W1 %1)").arg(ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    return !out.exitCode;
}

bool NetworkHelper::sendUDP(QHostAddress addr,
                            int port,
                            const QByteArray &datagram)
{
    QUdpSocket udpSocketSend;
    udpSocketSend.connectToHost(addr,port);
    udpSocketSend.write(datagram);
    return true;
}

QHostAddress NetworkHelper::BroadcastAddress(const QUrl& url)
{
    auto a = url.host().split('.');
    if(a.length()!=4) return QHostAddress("255.255.255.255");
    a[3]= QStringLiteral("255");
    return QHostAddress(a.join('.'));
}
