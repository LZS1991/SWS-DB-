#include "downloadfromspacetrack.h"

DownloadFromSpaceTrack::DownloadFromSpaceTrack(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    QObject::connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(doGet(QNetworkReply*)));

    QString baseURL = "https://www.space-track.org";
    QString authPath = "/auth/login";
    QString userName = "285149763@qq.com";
    QString password = "luwanjie9330300384";
    QString query = "/basicspacedata/query/class/tle_latest/ORDINAL/1/EPOCH/%3Enow-30/orderby/NORAD_CAT_ID/format/3le";
    QNetworkRequest *request = new QNetworkRequest();
    request->setUrl(QUrl(baseURL + authPath));
    request->setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    request->setRawHeader("Referer","http://xxxxxxx");
    request->setRawHeader("Cookie","xxxxxxxxx");
    QByteArray postData;
    postData.append( "identity="+ userName + "&" + "password=" + password);
    m_manager->post(*request,postData);

}

void DownloadFromSpaceTrack::doGet(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
        return;

    disconnect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(doGet(QNetworkReply*)));
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
    QString baseURL = "https://www.space-track.org";
    QString query = "/basicspacedata/query/class/tle_latest/ORDINAL/1/EPOCH/%3Enow-30/orderby/NORAD_CAT_ID/format/3le";
    QNetworkRequest *request_2 = new QNetworkRequest();
    request_2->setUrl(QUrl(baseURL + query));
    m_manager->get(*request_2);
}

void DownloadFromSpaceTrack::finishedSlot(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        while(!reply->atEnd())
        {
            QByteArray bytes1 = reply->readLine().trimmed();
            QString string = QString::fromUtf8(bytes1);
            qDebug() << string;
        }
    }
    else
    {
        qDebug()<<"handle errors here";
        QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        //statusCodeV是HTTP服务器的相应码，reply->error()是Qt定义的错误码，可以参考QT的文档
        qDebug( "found error ....code: %d %d\n", statusCodeV.toInt(), (int)reply->error());
        qDebug(qPrintable(reply->errorString()));
    }
    reply->deleteLater();
}
