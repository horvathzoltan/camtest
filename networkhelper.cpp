#include "networkhelper.h"
#include "common/helper/ProcessHelper/processhelper.h"


auto NetworkHelper::Ping(const QString& ip) -> bool
{
    auto cmd = QStringLiteral(R"(ping -c1 -W1 %1)").arg(ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    return !out.exitCode;
}
