#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    my_db = QSqlDatabase::addDatabase("QMYSQL","temp_db");
    my_db.setHostName("fabsv.wisol.co.kr");
    my_db.setUserName("EIS");
    my_db.setPassword("wisolfab!");
    my_db.setDatabaseName("FAB");
    if(!my_db.open()){
        qDebug()<<"open false";
    }
    ms_mes_db = QSqlDatabase::addDatabase("QODBC3","temp_ms_mes_db");
    ms_mes_db.setDatabaseName("DRIVER={FreeTDS};Server=10.20.10.221;Database=MESDB;Uid=fabview;Port=1433;Pwd=fabview");
    if(!ms_mes_db.open()){
        qDebug()<<ms_mes_db.lastError().text();
        ui->textEdit->append(ms_mes_db.lastError().text());
    }else {
        ui->textEdit->append("open");
    }
    connect(&moniter_loop,SIGNAL(timeout()),this,SLOT(moniter_time_loop()));
    moniter_loop.setInterval(2000);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::moniter_time_loop()
{
    QSqlQuery query1(my_db);
    query1.exec("select * from OI_system_probe_moniter");
    QString append_txt;
    int i =0;
    while(query1.next()){
            if(i==0){
                append_txt = append_txt.append(QString(" EQUIPMENT_ID = '%1' ").arg(query1.value("machine_code").toString()));
            }else {
                append_txt = append_txt.append(QString(" OR EQUIPMENT_ID = '%1' ").arg(query1.value("machine_code").toString()));
            }
            i++;
    }
    QSqlQuery query2(ms_mes_db);
    query2.exec(QString("select EQUIPMENT_ID,EQUIPMENT_NAME,LAST_EVENT_ID from NM_EQUIPMENT where DELETE_FLAG = 'N' AND (%1)").arg(append_txt));
    while(query2.next()){

            query1.exec(QString("select * from OI_system_probe_moniter where machine_code = '%1' ").arg(query2.value("EQUIPMENT_ID").toString()));
            if(query1.next()){
                if(query1.value("before_statue").toString() != query2.value("LAST_EVENT_ID").toString()){
                        QString event_name = query2.value("LAST_EVENT_ID").toString();
                        if(event_name=="RUN"){
                            event_name = event_name.replace("RUN",tr("RUN"));
                        }else if(event_name=="ENGR1"){
                            event_name = event_name.replace("ENGR1",tr("ENGR1"));
                        }else if(event_name=="SCHDOWN1"){
                            event_name = event_name.replace("SCHDOWN1",tr("SCHDOWN1"));
                        }else if(event_name=="USCHDOWN3"){
                            event_name = event_name.replace("USCHDOWN3",tr("USCHDOWN3"));
                        }else if(event_name=="WAIT"){
                            event_name = event_name.replace("WAIT",tr("WAIT"));
                        }else if(event_name=="NONSCHED"){
                            event_name = event_name.replace("NONSCHED",tr("NONSCHED"));
                        }else if(event_name=="SCHDOWN2"){
                            event_name = event_name.replace("SCHDOWN2",tr("SCHDOWN2"));
                        }else if(event_name=="USCHDOWN1"){
                            event_name = event_name.replace("USCHDOWN1",tr("USCHDOWN1"));
                        }else if(event_name=="USCHDOWN2"){
                            event_name = event_name.replace("USCHDOWN2",tr("USCHDOWN2"));
                        }else if(event_name=="USCHDOWN4"){
                            event_name = event_name.replace("USCHDOWN4",tr("USCHDOWN4"));
                        }else {
                            event_name = "";
                        }
                        QDateTime current_datetime = QDateTime::currentDateTime();
                        QString currenttime_str =current_datetime.toString("yyyy-MM-dd hh:mm:ss");
                        QSqlQuery query(my_db);
                        QString query_txt = QString("INSERT INTO `OI_system_history` "
                                                    "(`event_datetime`, `name`, "
                                                    "`machine_name`, `event_type`,`process`) "
                                                    "VALUES ("
                                                    "'%1', '%2', '%3', '%4' , '%5');")
                                                    .arg(currenttime_str)
                                                    .arg("auto").arg(query1.value("machine_name").toString()).arg(event_name).arg(query1.value("Main_process").toString());
                        query.exec(query_txt);

                        if(event_name == tr("RUN")){
                            query_txt = QString("select * from OI_system_time where run_time is NULL "
                                                "AND machine_code = '%1' "
                                                "AND machine_name = '%2' order by stop_time desc LIMIT 1").arg(query1.value("machine_code").toString())
                                                                                                          .arg(query1.value("machine_name").toString());
                            query.exec(query_txt);
                            if(query.next()){
                                QDateTime stop_time = query.value("stop_time").toDateTime();
                                qint64 secs = stop_time.secsTo(current_datetime);
                                QString time_str = from_sec_to_timestr(secs);
                                query_txt = QString("Update `OI_system_time` SET "
                                                    "run_time = '%1',run_name='%2',stop_time_calc='%3' "
                                                    "Where stop_time = '%5' AND machine_code ='%6'")
                                                    .arg(currenttime_str).arg("auto").arg(time_str)
                                                    .arg(query.value("stop_time").toDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                                    .arg(query.value("machine_code").toString());
                                query.exec(query_txt);
                            }else {

                            }
                        }else {
                            query_txt = QString("select * from OI_system_time where run_time is NULL "
                                                "AND machine_code = '%1' "
                                                "AND machine_name = '%2' order by run_time desc LIMIT 1")
                                    .arg(query1.value("machine_code").toString())
                                    .arg(query1.value("machine_name").toString());
                            query.exec(query_txt);
                            if(!query.next()){
                                query_txt = QString("INSERT INTO `OI_system_time` "
                                                    "(`process`,`machine_name`, `machine_code`, `stop_time`, `stop_name` , `stop_data`) "
                                                    "VALUES ('%1', '%2', '%3', '%4','%5','%6');")
                                                    .arg(query1.value("Main_process").toString())
                                                    .arg(query1.value("machine_name").toString())
                                                    .arg(query1.value("machine_code").toString())
                                                    .arg(currenttime_str).arg("auto").arg(event_name);
                                query.exec(query_txt);
                            }else {

                            }
                        }
                        query1.exec(QString("update OI_system_probe_moniter SET `before_statue`='%1' where machine_code = '%2'")
                                    .arg(query2.value("LAST_EVENT_ID").toString()).arg(query2.value("EQUIPMENT_ID").toString()));
                }
            }
    }

}
QString MainWindow::from_sec_to_timestr(int secs)
{
    int hour = secs/3600;
    int minute = (secs%3600)/60;
    int sec = (secs%3600)%60;
    QString reslut = QString("%1:%2:%3").arg(hour).arg(minute).arg(sec);
    return reslut;
}


void MainWindow::on_server_start_clicked()
{
    moniter_loop.start();
}
