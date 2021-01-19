#ifndef CAMTEST_H
#define CAMTEST_H

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
};

#endif // CAMTEST_H
