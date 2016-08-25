#include "datareadtodb.h"
#include <QDebug>
#include <QTimer>
DataReadToDB::DataReadToDB(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::DataReadToDBClass;
	ui->setupUi(this);

    openInitDB("Data/SSIMDB.db");
    //将开启之后默认设置为Enable的控件设置为Enabled（false）
	ui->CombinationBtn->setEnabled(true);// LL防止误操作，暂时注释掉
	ui->SInsertSatTable->setAlternatingRowColors(true);// 隔行换色
	ui->dataSourceListEdit->setReadOnly(true);
	ui->SInsertSatTable->resizeRowsToContents();
	ui->SInsertSatTable->setSelectionBehavior(QAbstractItemView::SelectRows);//设置整行选中

    m_manager = new QNetworkAccessManager(this);
}

DataReadToDB::~DataReadToDB()
{
	delete ui;
}

void DataReadToDB::initOperation()
{
    closeFile();
    SD_TCE_DB();
    initFileList();
    updateSWData();
    initPostGet();
}

void DataReadToDB::on_loadDataBaseBtn_clicked()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	QString fileName = QFileDialog::getOpenFileName(this, tr("选择卫星数据文件"), "", tr("DB files (*.db);;All files(*.*)"));
	if (!fileName.isEmpty())
		loadFile(fileName);
	QFileInfo fileInfo(fileName);
	ui->DBnameLE->setText(fileInfo.absoluteFilePath());

	QApplication::restoreOverrideCursor();
}

void DataReadToDB::on_SDfileBtn_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("选择SD数据文件"), "", tr("SD files (*.sd)"));
	if (!fileName.isEmpty())
	{
		SDPathName = fileName;
		QFileInfo fileInfo(fileName);
		ui->SDnameLE->setText(fileInfo.absoluteFilePath());
	}
}

void DataReadToDB::on_TCEfileBtn_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("选择TCE数据文件"), "", tr("TCE files (*.tce)"));
	if (!fileName.isEmpty())
	{
		TCEPathName = fileName;
		QFileInfo fileInfo(fileName);
		ui->TCEnameLE->setText(fileInfo.absoluteFilePath());
	}
}

void DataReadToDB::on_CombinationBtn_clicked()
{
	if (!curDBFile.isNull() && !SDPathName.isNull() && !TCEPathName.isNull())
	{
		SD_TCE_DB();
	}
	else
	{
		QMessageBox::warning(this, tr("提示"), tr("友情提示：\n请将文件配置完成后再进行合成"));
	}

	openInitDB(curDBFile);
}

void DataReadToDB::SCheck(int state)
{
	if (ui->SerialNum->isChecked())//编号
		ui->SerialNumLE->setEnabled(true);
	else
		ui->SerialNumLE->setEnabled(false);

	if (ui->SCommonNameCB->isChecked())//通称
		ui->SCommonNameLE->setEnabled(true);
	else
		ui->SCommonNameLE->setEnabled(false);

	if (ui->SOfficialNameCB->isChecked())//官方名称
		ui->SOfficialNameLE->setEnabled(true);
	else
		ui->SOfficialNameLE->setEnabled(false);

	if (ui->SInterNumCB->isChecked())//国际号码
		ui->SInterNumLE->setEnabled(true);
	else
		ui->SInterNumLE->setEnabled(false);

	if (ui->SOwnerCB->isChecked())//所有者
		ui->SOwnerLE->setEnabled(true);
	else
		ui->SOwnerLE->setEnabled(false);

	if (ui->SMissionCB->isChecked())//任务
		ui->SMissionLE->setEnabled(true);
	else
		ui->SMissionLE->setEnabled(false);

	if (ui->SStateCB->isChecked())//状态
		ui->SStateComB->setEnabled(true);
	else
		ui->SStateComB->setEnabled(false);
}

void DataReadToDB::SD_TCE_DB()
{
	db.open();// 打开数据库

    //在有新数据时，清空原来的数据
    QString deleteData = "delete from SAT";
    //因为频繁增删改查的原因，导致数据库碎片化，需要该语句进行整理
    QString vacuumData = "vacuum";
    db.exec(deleteData);
    db.exec(vacuumData);
    db.commit();

    QFile SDData(QDir::currentPath() + "/Data/satcat.txt");
    QFile TCEData(QDir::currentPath() + "/Data/stkSatDbAll.tce");

	QProgressDialog progress(this);
    progress.setLabelText(tr("searching......"));
	progress.setRange(0, RECORD_COUNT-1);
	progress.setModal(true);
    progress.setMinimumWidth(600);
	int index = 0;
	db.transaction();
	if (SDData.open(QIODevice::ReadOnly) && TCEData.open(QIODevice::ReadOnly))
	{
		QTextStream stream1(&SDData);
		QTextStream stream2(&TCEData);

		int lastNumber = -1;// 上一条记录的标记
		QString lastLine1 = "";
		QString lastLine2 = "";

        QString line21 = stream2.readLine();
        QString line22 = stream2.readLine();

        int ssc = line21.mid(2, 5).trimmed().toInt();
        while (!stream1.atEnd())
        {
            progress.setValue(index);
            qApp->processEvents();
            if (progress.wasCanceled())
            {
                return;
            }
            index++;

            QString line1 = stream1.readLine();
            SAT_International_number = line1.mid(0, 11).trimmed();
            SAT_SSC_NUMBER = line1.mid(13, 5).trimmed().toInt();
            if(SAT_SSC_NUMBER == 41725)
                int a = 0;
            SAT_Payload_Flag = line1.mid(20, 1).trimmed();
            SAT_Operation_Status_Code = line1.mid(21, 1).trimmed();
            SAT_Name = line1.mid(23, 24).trimmed();
            if(SAT_Name.contains("'"))
                SAT_Name.replace(QString("'"), QString("''"));
            SAT_Owner = line1.mid(49, 5).trimmed();
            SAT_Launch_date = line1.mid(56, 10).trimmed();
            SAT_Launch_site = line1.mid(68, 5).trimmed();
            SAT_Decay_Time = line1.mid(75, 10).trimmed();
            SAT_Period = line1.mid(87, 7).trimmed().toFloat();
            SAT_Inclination = line1.mid(96, 5).trimmed().toFloat();
            SAT_Apogee = line1.mid(103, 6).trimmed().toInt();
            SAT_Perigee = line1.mid(111, 6).trimmed().toInt();
            SAT_Radar_Cross_Section = line1.mid(119, 8).trimmed();
            SAT_Orbit_Status_Code = line1.mid(129, 3).trimmed();

            if(SAT_SSC_NUMBER == ssc)
            {
                SAT_TLE_LINE1 = line21;
                SAT_TLE_LINE2 = line22;

                if(!stream2.atEnd())
                {
                    line21 = stream2.readLine();
                    line22 = stream2.readLine();
                    ssc = line21.mid(2, 5).trimmed().toInt();
                }
            }
            else
            {
                SAT_TLE_LINE1 = "";
                SAT_TLE_LINE2 = "";
            }

            QString str = QString("insert into SAT(international_number,ssc_number,payload_flag,operation_status_code,name,owner,launch_date,launch_site,decay_time,period,inclination,apogee,perigee,radar_cross_section,orbit_status_code,TLE_LINE1,TLE_LINE2)"
                                  " values('%1',%2,'%3','%4','%5','%6','%7','%8','%9',%10,%11,%12,%13,'%14','%15','%16','%17')")
                                  .arg(SAT_International_number).arg(SAT_SSC_NUMBER).arg(SAT_Payload_Flag).arg(SAT_Operation_Status_Code)
                                  .arg(SAT_Name).arg(SAT_Owner).arg(SAT_Launch_date).arg(SAT_Launch_site).arg(SAT_Decay_Time)
                                  .arg(SAT_Period).arg(SAT_Inclination).arg(SAT_Apogee).arg(SAT_Perigee).arg(SAT_Radar_Cross_Section)
                                  .arg(SAT_Orbit_Status_Code).arg(SAT_TLE_LINE1).arg(SAT_TLE_LINE2);
            db.exec(str);
        }

		db.commit();
        db.close();
//		QMessageBox::information(this, tr("提示"), tr("数据转换完成"), QMessageBox::Ok);
	}
}

void DataReadToDB::on_CloseBtn_clicked()
{
	this->close();
}

void DataReadToDB::loadFile(const QString &fileName)
{
	if (fileName.isNull())
		on_loadDataBaseBtn_clicked();
	closeFile();
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(fileName);
	if (!db.open())
	{
		QMessageBox::critical(this, tr("错误"), tr("数据库错误：\n%1").arg(db.lastError().text()));
		return;
	}

	QSqlQuery q("SELECT * FROM SAT WHERE 1=2", db);// 判断是不是sql数据库
	if (!q.exec()) {
		//Database Open Error
        QMessageBox::critical(this, tr("error"), tr("DataBase error.\n%1 it maybe not SQLite DataBase").arg(fileName));
		return;
	}
    q.finish();
	//Database Open
	curDBFile = fileName;

	//Get Tables' name
	QStringList tableNameList = db.tables(QSql::Tables);  
	for (int i = 0; i < tableNameList.count();i++)
	{
		tableName = tableNameList.at(i);
		break;
	}

//	model = new QSqlTableModel(this, db);
//	model->setTable(tableName);
//	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
//	model->select();// 检索的意思
//	ui->SInsertSatTable->setModel(model);
//	ui->SInsertSatTable->resizeRowsToContents();
    db.close();
}

void DataReadToDB::closeFile()
{
    if(db.open())
        db.close();
}

void DataReadToDB::openInitDB(const QString &fileName)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	if (!fileName.isEmpty())
		loadFile(fileName);

    QApplication::restoreOverrideCursor();
}

void DataReadToDB::initFileList()
{
    ui->dataSourceListEdit->clear();
    fileNames.clear();
    QString tlePath = QDir::currentPath() + "/TLEData/";
    QDir dir(tlePath);
    if(!dir.exists())
        QMessageBox::information(this, "info", "the dir is not exist");
    QFileInfoList fileInfoList = dir.entryInfoList();
    for(int i = 0; i < fileInfoList.count(); i++)
    {
        if(fileInfoList.at(i).fileName() != "." && fileInfoList.at(i).fileName()!= "..")
        {
            ui->dataSourceListEdit->append(fileInfoList.at(i).fileName());
            fileNames.append(fileInfoList.at(i).filePath());
        }
    }

    this->on_updateDataButton_clicked();
}

QString DataReadToDB::getSWSQLStr(QString line, QString table)
{
    SW_YYYY = line.mid(0, 4).trimmed().toInt();
    SW_MM = line.mid(5, 2).trimmed().toInt();
    SW_DD = line.mid(8, 2).trimmed().toInt();
    SW_BSRN = line.mid(11, 4).trimmed().toInt();
    SW_ND = line.mid(16, 2).trimmed().toInt();
    SW_KP1 = line.mid(19, 2).trimmed().toInt();
    SW_KP2 = line.mid(22, 2).trimmed().toInt();
    SW_KP3 = line.mid(25, 2).trimmed().toInt();
    SW_KP4 = line.mid(28, 2).trimmed().toInt();
    SW_KP5 = line.mid(31, 2).trimmed().toInt();
    SW_KP6 = line.mid(34, 2).trimmed().toInt();
    SW_KP7 = line.mid(37, 2).trimmed().toInt();
    SW_KP8 = line.mid(40, 2).trimmed().toInt();
    SW_SUM = line.mid(43, 3).trimmed().toInt();
    SW_AP1 = line.mid(47, 3).trimmed().toInt();
    SW_AP2 = line.mid(51, 3).trimmed().toInt();
    SW_AP3 = line.mid(55, 3).trimmed().toInt();
    SW_AP4 = line.mid(59, 3).trimmed().toInt();
    SW_AP5 = line.mid(63, 3).trimmed().toInt();
    SW_AP6 = line.mid(67, 3).trimmed().toInt();
    SW_AP7 = line.mid(71, 3).trimmed().toInt();
    SW_AP8 = line.mid(75, 3).trimmed().toInt();
    SW_AVG = line.mid(79, 3).trimmed().toInt();
    SW_CP = line.mid(83, 3).trimmed().toFloat();
    SW_C9 = line.mid(87, 1).trimmed().toInt();
    SW_ISN = line.mid(89, 3).trimmed().toInt();
    SW_ADJ_F10_7 = line.mid(93, 5).trimmed().toFloat();
    SW_Q = line.mid(99, 1).trimmed().toInt();
    SW_ADJ_CTR81 = line.mid(101, 5).trimmed().toFloat();
    SW_ADJ_LST81 = line.mid(107, 5).trimmed().toInt();
    SW_OBS_F10_7 = line.mid(113, 5).trimmed().toFloat();
    SW_OBS_CTR81 = line.mid(119, 5).trimmed().toFloat();
    SW_OBS_LST81 = line.mid(125, 5).trimmed().toFloat();
    QString str = QString("insert into %1"
                          "(yyyy,mm,dd,bsrn,nd,kp1,kp2,kp3,kp4,kp5,kp6,kp7,kp8,sum,ap1,ap2,ap3,ap4,ap5,ap6,ap7,ap8,avg,cp,c9,isn,adj_f10_7,q,adj_ctr81,adj_lst81,obs_f10_7,obs_ctr81,obs_lst81) "
                          "values(%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34)")
                          .arg(table).arg(SW_YYYY).arg(SW_MM).arg(SW_DD).arg(SW_BSRN).arg(SW_ND).arg(SW_KP1).arg(SW_KP2).arg(SW_KP3).arg(SW_KP4)
                          .arg(SW_KP5).arg(SW_KP6).arg(SW_KP7).arg(SW_KP8).arg(SW_SUM).arg(SW_AP1).arg(SW_AP2).arg(SW_AP3).arg(SW_AP4).arg(SW_AP5)
                          .arg(SW_AP6).arg(SW_AP7).arg(SW_AP8).arg(SW_AVG).arg(SW_CP).arg(SW_C9).arg(SW_ISN).arg(SW_ADJ_F10_7).arg(SW_Q).arg(SW_ADJ_CTR81)
                          .arg(SW_ADJ_LST81).arg(SW_OBS_F10_7).arg(SW_OBS_CTR81).arg(SW_OBS_LST81);
    return str;
}

void DataReadToDB::updateSWData()
{
    QString swPath = QDir::currentPath() + "/SpaceWeatherData/";
    QDir dir(swPath);
    if(!dir.exists())
        QMessageBox::information(this, "info", "the dir is not exist");
    QFileInfoList fileInfoList = dir.entryInfoList();

    QString swFile = fileInfoList.at(2).filePath();
    QFile swData(swFile);

    db.open();//打开数据库

    QString deleteData1 = "delete from SPACE_WEATHER";
    QString deleteData2 = "delete from SPACE_WEATHER_DAILY_PRE";
    QString deleteData3 = "delete from SPACE_WEATHER_MONTHLY_FIT";
    QString deleteData4 = "delete from SPACE_WEATHER_MONTHLY_PRE";

    db.exec(deleteData1);
    db.exec(deleteData2);
    db.exec(deleteData3);
    db.exec(deleteData4);
    db.commit();

    db.transaction();
    if(swData.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&swData);
        while(!stream.atEnd())
        {
            QString line = stream.readLine();
            if(line == "BEGIN OBSERVED")
            {
                while (!stream.atEnd()) {
                    line = stream.readLine();
                    if(line == "END OBSERVED")
                        break;
                    QString sqlStr = this->getSWSQLStr(line, "SPACE_WEATHER");
                    db.exec(sqlStr);
                }
            }
            else if(line == "BEGIN DAILY_PREDICTED")
            {
                while (!stream.atEnd()) {
                    line = stream.readLine();
                    if(line == "END DAILY_PREDICTED")
                        break;
                    QString sqlStr = this->getSWSQLStr(line, "SPACE_WEATHER_DAILY_PRE");
                    db.exec(sqlStr);
                }
            }
            else if(line == "BEGIN MONTHLY_PREDICTED")
            {
                while (!stream.atEnd()) {
                    line = stream.readLine();
                    if(line == "END MONTHLY_PREDICTED")
                        break;
                    QString sqlStr = this->getSWSQLStr(line, "SPACE_WEATHER_MONTHLY_FIT");
                    db.exec(sqlStr);
                }
            }
            else if(line == "BEGIN MONTHLY_FIT")
            {
                while (!stream.atEnd()) {
                    line = stream.readLine();
                    if(line == "END MONTHLY_FIT")
                        break;
                    QString sqlStr = this->getSWSQLStr(line, "SPACE_WEATHER_MONTHLY_PRE");
                    db.exec(sqlStr);
                }
            }
        }
        db.commit();
        db.close();
    }
}

void DataReadToDB::on_newDataSRCButton_clicked()
{
    fileNames.clear();
    fileNames = QFileDialog::getOpenFileNames(this, "select the data source","","Files(*.txt)");
	int fileNum = fileNames.count();
    ui->dataSourceListEdit->clear();
	for (int i = 0; i < fileNum; i++)
	{
		QString fileName = QFileInfo(fileNames.at(i)).fileName();
		ui->dataSourceListEdit->append(fileName);
	}
}

void DataReadToDB::on_updateDataButton_clicked()
{
	db.open();//打开数据库
	int fileNum = fileNames.count();
	QString SatName = "";//卫星名称，在文件中是第3n+1行
	QString line1 = "";//两行根数第一行，在文件中是第3n+2行
	QString line2 = "";//两行根数第二行，在文件中是第3n+3行
	int SSCNUMBER;//卫星编目号

	//设置进度条
    QProgressDialog progress(this);
    progress.setRange(0, fileNum-1);
    progress.setModal(true);
    progress.setMinimumWidth(600);

	for (int i = 0; i < fileNum; i++)
	{
        progress.setLabelText(QString("update file NO.%1 ").arg(i+1));
        progress.setValue(i);
//		qApp->processEvents();
		if (progress.wasCanceled())
		{
			return;
		}

		QFile SRCData(fileNames.at(i));
		db.transaction();
        if(SRCData.open(QIODevice::ReadOnly))
		{
			QTextStream stream(&SRCData);
			//逐行读取
            while(!stream.atEnd())
			{
				//文件中按照每三行进行一次读取，因为文件格式是这样的
				SatName = stream.readLine().trimmed();
                if(SatName.contains("("))//判断是否包含（）中的名字
				{
					SatName = SatName.section(QRegExp("[()]"), 1, 1);//提取括号中的名称
				}
				line1 = stream.readLine();
				line2 = stream.readLine();
				SSCNUMBER = line1.mid(2,5).trimmed().toInt();
                if(SSCNUMBER == 7530)
                    int a = 0;
                QSqlQuery query(QString("select * from SAT where ssc_number = %1").arg(SSCNUMBER),db);
                query.exec();
                if(!query.next())
                    db.exec(QString("insert into SAT(ssc_number,name,TLE_LINE1,TLE_LINE2) values(%1,'%2','%3','%4')").arg(SSCNUMBER).arg(SatName).arg(line1).arg(line2));
                else
                    db.exec(QString("update SAT set TLE_LINE1 = '%1',TLE_LINE2 = '%2' where ssc_number = %3").arg(line1).arg(line2).arg(SSCNUMBER));
                query.finish();
			}
		}
		db.commit();
	}	
    db.close();
}

void DataReadToDB::initPostGet()
{
    QObject::connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(doPost(QNetworkReply*)));

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

void DataReadToDB::doGet(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        db.open();
        db.transaction();
        int SSCNUMBER;//卫星编号
        while(!reply->atEnd())
        {
            QByteArray SSCName = reply->readLine().trimmed();
            qDebug() << "Update the TLE Data of Satellite: " + QString::fromUtf8(SSCName.remove(0,2));
            QByteArray SSCTLE1 = reply->readLine().trimmed();
            QByteArray SSCTLE2 = reply->readLine().trimmed();
            SSCNUMBER = (QString::fromUtf8(SSCTLE1)).mid(2, 5).trimmed().toInt();
            QSqlQuery query(QString("select * from SAT where ssc_number = %1").arg(SSCNUMBER),db);
            query.exec();
            if(!query.next())
                db.exec(QString("insert into SAT(ssc_number,name,TLE_LINE1,TLE_LINE2) values(%1,'%2','%3','%4')")
                        .arg(SSCNUMBER)
                        .arg(QString::fromUtf8(SSCName))
                        .arg(QString::fromUtf8(SSCTLE1))
                        .arg(QString::fromUtf8(SSCTLE2)));
            else
                db.exec(QString("update SAT set TLE_LINE1 = '%1',TLE_LINE2 = '%2' where ssc_number = %3")
                        .arg(QString::fromUtf8(SSCTLE1))
                        .arg(QString::fromUtf8(SSCTLE1))
                        .arg(SSCNUMBER));
            query.finish();
        }
        db.commit();
        db.close();
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

void DataReadToDB::doPost(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug()<<"handle errors here";
        QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        //statusCodeV是HTTP服务器的相应码，reply->error()是Qt定义的错误码，可以参考QT的文档
        qDebug( "found error ....code: %d %d\n", statusCodeV.toInt(), (int)reply->error());
        qDebug(qPrintable(reply->errorString()));
        return;
    }

    disconnect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(doPost(QNetworkReply*)));
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(doGet(QNetworkReply*)));
    QString baseURL = "https://www.space-track.org";
    QString query = "/basicspacedata/query/class/tle_latest/ORDINAL/1/EPOCH/%3Enow-30/orderby/NORAD_CAT_ID/format/3le";
    QNetworkRequest *request_2 = new QNetworkRequest();
    request_2->setUrl(QUrl(baseURL + query));
    m_manager->get(*request_2);
    reply->deleteLater();
}
