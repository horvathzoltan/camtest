#ifndef CAMERAAUTOSET_H
#define CAMERAAUTOSET_H

#include <QList>
#include <QString>



class CameraAutoSet
{
public:
    struct AutosetStartR{
        QString msg;
        int iso;
        int brightness;
        int contrast;
    };

    static AutosetStartR AutosetStart();

    struct Iter{
        int x3;
        int flag;
    };

    struct AAAdata{
        int brightness;
        int contrast;
        int iso;
        int range;
        int n;
    };



    static int setAAA(int i, void* data);

    static QList<Iter> approx(int x0, int x1, int(*set)(int,void* data), void* data);
    static QList<Iter> approx2(int x0, int x1, int x);
};

#endif // CAMERAAUTOSET_H
