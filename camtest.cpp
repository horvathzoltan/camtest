#include "camtest.h"
#include "common/helper/ProcessHelper/processhelper.h"
#include "common/helper/string/stringhelper.h"
#include <QRegularExpression>

// ping cél ip
// ha ok, akkor arp -a cél ip -> mac addr
// ha ismeretlen mac
Camtest::StartR Camtest::Start(){


    QString ip = QStringLiteral("172.16.3.235"); //beallitasok(ip)
    auto cmd = QStringLiteral(R"(ping -c1 -W1 %1)").arg(ip);

    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return {"ping error"};

    cmd = QStringLiteral(R"(arp -a -n %1)").arg(ip);
    out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode!=0) return {"arp error"};
    // ? (172.16.3.235) at dc:a6:32:74:92:dd [ether] on enp4s0
    if(out.stdOut.isEmpty()) return {"no arp output"};

    QString& x = out.stdOut;
    QRegularExpression re(QStringLiteral(R"(at\s+((?:[0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2})\s+)"));
    QRegularExpressionMatch m = re.match(x);

    QString arp;
    if (m.hasMatch())
    {
        arp = m.captured(m.lastCapturedIndex());
    }

    QString msg;
    append_value(&msg, ip);
    append_value(&msg, arp);
    return {msg};
}

Camtest::StopR Camtest::Stop(){return {};}
