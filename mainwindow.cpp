#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileInfo>
#include <QDebug>
#include <QIODevice>
#include <QDir>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->updateDBBtn->setEnabled(true);
    ui->progressBar->setEnabled(false);
    this->initFileList();
    m_manager = new QNetworkAccessManager(this);
    m_finishMessageBox = new QMessageBox(this);
    m_dataToDB = new DataReadToDB();
    connect(ui->beginDownloadBtn, SIGNAL(clicked()), this, SLOT(downloadFile()));
    connect(ui->updateDBBtn, SIGNAL(clicked()), this, SLOT(openDataToDBWidget()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_manager;
    if(m_finishMessageBox != NULL)
        delete m_finishMessageBox;
}

void MainWindow::startRequest(QUrl url)
{
    m_reply = m_manager->get(QNetworkRequest(url));
    connect(m_reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(m_reply, SIGNAL(readyRead()),this, SLOT(httpReadyRead()));
}

void MainWindow::initFileList()
{
    m_fileList.clear();
    m_urlStr = ui->urlLineEdit->text();
    QStringList fileList = ui->fileListTextEdit->toPlainText().split("\n");
    for(int i = 0; i < fileList.size(); i++)
        m_fileList.append(m_urlStr + fileList.at(i));
    m_fileList.append(ui->swUrlLineEdit->text());
    m_fileList.append(ui->satcatUrlLineEdit->text());
    ui->progressBar->setRange(0, m_fileList.size());
}

void MainWindow::downloadFile()
{
    if(m_fileList.size() <= 0)
    {
        m_finishMessageBox->setWindowTitle(tr("Information"));
        m_finishMessageBox->setText(tr("All File have been downloaded!"));
        m_finishMessageBox->show();
        QTimer::singleShot(1000, m_finishMessageBox, SLOT(close()));
        ui->label_3->setText("Finished!");
        ui->updateDBBtn->setEnabled(true);
        m_reply->deleteLater();
        this->initDataToDBWidget();
        this->initFileList();
        QTimer::singleShot(5000, this, SLOT(downloadFile()));
        return;
    }

    ui->progressBar->setEnabled(true);
    ui->updateDBBtn->setEnabled(false);
    m_url = m_fileList.at(0);
    m_fileList.removeFirst();
    QFileInfo fileInfo(m_url.path());
    QString fileName = fileInfo.fileName();
    if(fileName.isEmpty())
    {
        qDebug() << "the file name is empty";
        this->downloadFile();
    }

    if(QFile::exists(fileName))
    {
        qDebug() << "the file name is existed!";
        QFile::remove(fileName);
    }

    if(fileName == "sw19571001.txt")
        m_file = new QFile(QDir::currentPath() + "/SpaceWeatherData/" + fileName);
    else if(fileName == "satcat.txt")
        m_file = new QFile(QDir::currentPath() + "/Data/" + fileName);
    else
        m_file = new QFile(QDir::currentPath() + "/TLEData/" + fileName);

    if(!m_file->open(QIODevice::WriteOnly))
    {
        qDebug() << "can not save the file, because "+ m_file->errorString();
        delete m_file;
        m_file = nullptr;
        return;
    }

    m_httpRequestAborted = false;
    ui->label_3->setText(fileName);
    this->startRequest(m_url);
}

void MainWindow::cancelDownload()
{
    m_httpRequestAborted = true;
    m_reply->abort();
}

void MainWindow::httpFinished()
{
    if(m_httpRequestAborted)
    {
        if(m_file)
        {
            m_file->close();
            m_file->remove();
            delete m_file;
            m_file = 0;
        }
        m_reply->deleteLater();
        return;
    }

    m_file->flush();
    m_file->close();


    QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(m_reply->error())
    {
        m_file->remove();
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(m_reply->errorString()));
    }
    else if(!redirectionTarget.isNull()) {
        QUrl newUrl = m_url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_url = newUrl;
            m_reply->deleteLater();
            m_file->open(QIODevice::WriteOnly);
            m_file->resize(0);
            this->startRequest(m_url);
            return;
        }
    }
    m_reply->deleteLater();
    m_reply = 0;
    delete m_file;
    m_file = 0;

    ui->progressBar->setValue(ui->progressBar->maximum() - m_fileList.size());
    this->downloadFile();
}

void MainWindow::httpReadyRead()
{
    if(m_file)
        m_file->write(m_reply->readAll());
}

void MainWindow::openDataToDBWidget()
{
    DataReadToDB *dataToDB = new DataReadToDB();
    dataToDB->show();
}

void MainWindow::initDataToDBWidget()
{
    m_dataToDB->initOperation();
}

