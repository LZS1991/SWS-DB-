#include "mainwindow.h"
#include "downloadfromspacetrack.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
//    DownloadFromSpaceTrack *download = new DownloadFromSpaceTrack();
    return a.exec();
}
