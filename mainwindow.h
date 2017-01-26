#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QSqlDatabase my_db;
    QSqlDatabase ms_mes_db;
    QTimer moniter_loop;
    QString from_sec_to_timestr(int secs);

    ~MainWindow();

private slots:
    void moniter_time_loop();
    void on_server_start_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
