#ifndef DOWNLOADFROMSPACETRACK_H
#define DOWNLOADFROMSPACETRACK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
class DownloadFromSpaceTrack : public QObject
{
    Q_OBJECT
public:
    explicit DownloadFromSpaceTrack(QObject *parent = 0);

    QNetworkAccessManager *m_manager;


signals:

public slots:
    void finishedSlot(QNetworkReply*);
    void doGet(QNetworkReply*);
};

#endif // DOWNLOADFROMSPACETRACK_H
