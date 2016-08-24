#ifndef DATAREADTODB_H
#define DATAREADTODB_H

#if defined(_MSC_VER)&&(_MSC_VER>=1600)
#pragma execution_character_set("utf-8")
#endif
#define RECORD_COUNT 40862// 记录的数量，需要修改

#include <QtWidgets/QWidget>
#include "ui_datareadtodb.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QFileDialog>
#include <QSqlQuery>
#include <QProgressDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QFileInfo>
#include <QRegExp>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class DataReadToDB : public QWidget
{
	Q_OBJECT

public:
	DataReadToDB(QWidget *parent = 0);
	~DataReadToDB();

    void initOperation();
private:
	Ui::DataReadToDBClass *ui;

	//数据库中的字段
    QString SAT_International_number;
    int     SAT_SSC_NUMBER;
    QString SAT_Payload_Flag;
    QString SAT_Operation_Status_Code;
    QString SAT_Name;
    QString SAT_Owner;
    QString SAT_Launch_site;
    QString SAT_Launch_date;
    QString SAT_Decay_Time;
    QString SAT_Radar_Cross_Section;
    QString SAT_Orbit_Status_Code;
    int     SAT_Apogee;
    int     SAT_Perigee;
    float   SAT_Period;
    float   SAT_Inclination;

	QString SAT_TLE_LINE1;
	QString SAT_TLE_LINE2;

    //空间环境数据库中的数据
    int SW_YYYY;
    int SW_MM;
    int SW_DD;
    int SW_BSRN;
    int SW_ND;
    int SW_KP1;
    int SW_KP2;
    int SW_KP3;
    int SW_KP4;
    int SW_KP5;
    int SW_KP6;
    int SW_KP7;
    int SW_KP8;
    int SW_SUM;
    int SW_AP1;
    int SW_AP2;
    int SW_AP3;
    int SW_AP4;
    int SW_AP5;
    int SW_AP6;
    int SW_AP7;
    int SW_AP8;
    int SW_AVG;
    float SW_CP;
    int SW_C9;
    int SW_ISN;
    float SW_ADJ_F10_7;
    int SW_Q;
    float SW_ADJ_CTR81;
    float SW_ADJ_LST81;
    float SW_OBS_F10_7;
    float SW_OBS_CTR81;
    float SW_OBS_LST81;

	int ResultRow;

	QString SDPathName;//SD文件路径
	QString TCEPathName;//TCE文件路径名
	QString FileDataName;//文件数据名

	QSqlDatabase db;//数据库
//	QSqlTableModel *model;//数据库模型
	QString curDBFile;// 当前DB数据库文件
	QString tableName;//当前DB数据库表名称
	QStringList fileNames;//读取的进行更新的数据源列表

	void loadFile(const QString &fileName);//加载数据库文件
	void closeFile();//关闭文件
	void openInitDB(const QString &fileName);// 打开默认的数据库
    void initFileList();//need to update automatic
    void initPostGet();//初始化Post和Get操作需要的设置
    QString getSWSQLStr(QString line, QString table);
    void updateSWData();//update spaceweather data

    QNetworkAccessManager *m_manager;
private slots:
    void on_loadDataBaseBtn_clicked();//打开卫星数据
	void on_SDfileBtn_clicked();// 打开sd文件
	void on_TCEfileBtn_clicked();// 打开TCE文件
	void on_CombinationBtn_clicked();// 触发文件合成
	void SCheck(int state);//控件的状态控制
	void SD_TCE_DB();//文件合成
	void on_CloseBtn_clicked();//关闭
	void on_newDataSRCButton_clicked();//选取数据源
	void on_updateDataButton_clicked();//进行数据更新

    void doGet(QNetworkReply*);//完成Get操作
    void doPost(QNetworkReply*);//完成Post操作
};

#endif // DATAREADTODB_H
