#include "downloadhelper.h"

void DownloadHelper::init(com::helper::Downloader *d) {
    _isInited = false;
    _d=d;
    _isInited = true;
}

DownloadHelper::Response DownloadHelper::download( const QString& path,
                                                  const QString& query)
{
    if(!_isInited) return {{},"not_inited"};
    if(!_d) return {{},"no_downloader"};
    Response r;
    r.content = _d->download(path, query, &r.errmsg);
    return r;
}

DownloadHelper::Response DownloadHelper::post( const QString& path,
                                              const QString& query,
                                              const QByteArray& a)
{
    if(!_isInited) return {{},"not_inited"};
    if(!_d) return {{},"no_downloader"};
    Response r;
    _d->post(path, query, &r.errmsg, a);
    return r;
}
