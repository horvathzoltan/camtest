#include <QElapsedTimer>
#include <QThread>
#include "cameraautoset.h"
#include "camtest.h"
#include "settings.h"
#include "common/logger/log.h"
#include "imagehelper.h"


auto CameraAutoSet::AutosetStart() -> AutosetStartR{
    AutosetStartR autosetStartR;
    QElapsedTimer t;
    t.start();
    Camtest::OpenCamera();
    //bool ok = false;


    int x0 = 100;
    int x1 = 800;
    //int y = 250;

    AAAdata aaadata={};

    auto r = CameraAutoSet::approx(x0,x1, &CameraAutoSet::setAAA, &aaadata);

    autosetStartR.msg = QStringLiteral("approx: %0[%2;%3] %4[ms]")
                            .arg(250).arg(x0).arg(x1).arg(t.elapsed());

    if(!r.isEmpty()){
        if(r.count()>=100) autosetStartR.msg += " !!!MAX!!!";
        QString msg2;

        for(int i=0;i<r.count();i++){
            auto&a=r[i];
            if(!msg2.isEmpty()) msg2+=", ";
            if(i>0&&!(i%4)) msg2+="\n";
            msg2+=QString::number(a.x3)+"_"+
                QString::number(a.flag);
        }
        autosetStartR.msg+="\n"+msg2;
    }

    autosetStartR.msg+=QStringLiteral("\nb: %1 c: %2 iso: %3 n: %4 r: %5")
                             .arg(aaadata.brightness)
                             .arg(aaadata.contrast)
                             .arg(aaadata.iso)
                             .arg(aaadata.n)
                             .arg(aaadata.range);

    autosetStartR.iso=aaadata.iso;
    autosetStartR.brightness = aaadata.brightness;
    autosetStartR.contrast = aaadata.contrast;

    return autosetStartR;
}



auto CameraAutoSet::approx(int x0, int x1, int(*set)(int, void*), void* data) -> QList<Iter>
{

    QList<Iter> r;
    int x3=x0;
    int i=0;
    int y3;
    do{
        if(x0!=x1) x3=(x0+x1)/2;

        y3 = set(x3, data);

        r.append({x3,y3});

        if(y3==0){
            break;
        } else if(y3==1){
            x0=x3;
        } else if(y3==-1){
            x1=x3;
        } else {
            x0=x1=x3;
        }

        i++;
    }while(i<100);
    return r;
}


auto CameraAutoSet::approx2(int x0, int x1, int x) -> QList<Iter>
{

    QList<Iter> r;
    int x3 = 0;
    int min_d = 1000000;
    int i=0;
    do{
        x3=(x0+x1)/2;

        int d = x-x3;
        if(abs(d)<abs(min_d)){
            min_d = d;
        } else if (abs(d-min_d)<1) {
            r.append({x3,-2});
            break;
        }

        if(x>x3){
            r.append({x3,1});
            x0=x3;
        } else if(x<x3){
            r.append({x3,-1});
            x1=x3;
        } else {
            r.append({x3,0});
            break;
        }
        i++;
    }while(i<100);
    return r;
}


auto CameraAutoSet::setAAA(int iso, void* data)->int
{
    static const QString b_key = nameof(Camtest::_camSettings.brightness);
    static const QString c_key = nameof(Camtest::_camSettings.contrast);
    auto b_isOk=false,c_isOk = false;
    float cut = 3;

    static int B_MIN = 0;
    static int B_MAX = 100;
    static int C_MIN = 0;
    static int C_MAX = 100;
    static int STEP = 25;
    static int ITER = 0;

    static int B_N = B_MAX/STEP;
    static int C_N = B_MAX/STEP;

    auto data2 = static_cast<AAAdata*>(data);

    if(data2->iso == iso){

        switch(ITER){
        case 0:
            STEP = 5;
            break;
        case 1:
            STEP = 1;
            break;
        default: return 0;
        }
        B_MIN = data2->brightness-STEP;
        B_MAX = data2->brightness+STEP;
        C_MIN = data2->contrast-STEP;
        C_MAX = data2->contrast+STEP;
        if(B_MIN<0) B_MIN=0;
        if(B_MAX>100) B_MAX=100;
        if(C_MIN<0) C_MIN=0;
        if(C_MAX>100) C_MAX=100;
        B_N = B_MAX/STEP;
        C_N = B_MAX/STEP;
        ITER++;
    }
    else{
        b_isOk = Camtest::setCamSettings(b_key,50);
        c_isOk = Camtest::setCamSettings(c_key,50);
        auto r = Camtest::setIso(iso);
        if(r.ranges.isEmpty()) return 1; //-1

        ITER = 0;
        B_MIN = 0;
        B_MAX = 100;
        C_MIN = 0;
        C_MAX = 100;
        STEP = 25;
    }

    QElapsedTimer e;

    QVarLengthArray<ImageHelper::BCdata> ds;
    ds.reserve((B_N+1)*(C_N+1));

    for(int b=B_MIN;b<=B_MAX;b+=STEP){
        b_isOk = Camtest::setCamSettings(b_key,b);
        QThread::msleep(3);
        if(!b_isOk) break;
        for(int c=C_MIN;c<=C_MAX;c+=STEP){
            e.restart();
            c_isOk = Camtest::setCamSettings(c_key,c);
            QThread::msleep(3);
            if(!c_isOk) break;

            auto data = Camtest::GetThumbnail();
            if(data.length()<100) break;
            QImage image2;
            image2.loadFromData(data, "JPG");
            //volatile auto x = image2.size();
            auto d = ImageHelper::BrightnessAndContrast(image2, cut);

            if(d.inputRange){
                d.brightness = b;
                d.contrast = c;
                d.iso = iso;
                d.ix = ds.count();

                ds.append(d);
            }

            auto t = 16-e.elapsed();
            if(t>0) QThread::msleep(t);
        }
    }       

    //ImageHelper::BCdata d_max={};
    double max_inputRange = -100000000;
    int maxix = -1;

    QList<ImageHelper::BCdata> valids;
    for(int i=0;i<ds.count();i++){
        auto a = ds[i];
        if(a.inputRange>250 && a.alpha<1.1 && a.beta<1){
            valids.append(a);
        }
        if(a.inputRange>max_inputRange){
            max_inputRange = a.inputRange;
            maxix=i;
        }
    }


    zInfo("max: "+QString::number(maxix));

    // ha max == -1 akkor sikertelen kör volt, kilépünk
    // maxix a nem túl sok fényes pontot tartalmazó kép
    if(maxix<0){
        zInfo("setiso failed");
        return 0;
    }


    auto d_max = ds[maxix];
    double prange = data2->range;

    // valids a fényes pontokat tartalmazó

    if(valids.isEmpty()){
        data2->range = d_max.inputRange;
        data2->brightness = d_max.brightness;
        data2->contrast = d_max.contrast;
        data2->iso = d_max.iso;
        data2->n = 0;
    }
    else{
        int c0=0, b0=0;
        for(auto a: valids){
            b0+=a.brightness;
            c0+=a.contrast;
        }
        data2->n = valids.count();
        data2->range = d_max.inputRange;
        data2->brightness = b0/data2->n;
        data2->contrast = c0/data2->n;
        data2->iso = d_max.iso;
    }

    if(std::abs(prange-d_max.inputRange)<1){
        return (ITER<2)?-2:0;
    }

    if(valids.count()<1) {
        // ITER=0;
        return 1;
    }
    if(valids.count()>10) {
        //   ITER=0;
        return -1;
    }
    return (ITER<2)?-2:0;
};
