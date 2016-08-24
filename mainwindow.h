#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QFile>
#include <QNetworkRequest>
#include <QProgressDialog>
#include <QMessageBox>
#include "datareadtodb.h"
#if defined(_MSC_VER)&&(_MSC_VER>=1600)
#pragma execution_character_set("utf-8")
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startRequest(QUrl url);

private:
    Ui::MainWindow *ui;

    QUrl m_url;
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QFile *m_file;
    int m_httpGetId;
    bool m_httpRequestAborted;
    QStringList m_fileList;
    QString m_urlStr;
    QMessageBox *m_finishMessageBox;
    DataReadToDB *m_dataToDB;

    void initFileList();
private slots:
    void downloadFile();
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();

    void openDataToDBWidget();
    void initDataToDBWidget();

};

#endif // MAINWINDOW_H
