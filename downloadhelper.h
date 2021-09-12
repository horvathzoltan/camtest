#ifndef DOWNLOADHELPER_H
#define DOWNLOADHELPER_H

#include <common/helper/downloader/downloader.h>

#include <QByteArray>
#include <QString>

class DownloadHelper
{
private:
    com::helper::Downloader* _d = nullptr;
    bool _isInited = false;
public:
    void init(com::helper::Downloader* d);
    void uninit(){_isInited = false;delete _d;_d=nullptr;};
    struct Response{
        QByteArray content;
        QString errmsg;
    };
    Response download(const QString& path, const QString& query);
    QString url(){
        if(!_isInited) return "";
        if(!_d) return "";
        return _d->url();
    }
    Response post(const QString& path,
                  const QString& query,
                  const QByteArray& a);
};



#endif // DOWNLOADHELPER_H
