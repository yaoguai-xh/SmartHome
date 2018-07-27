#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QPixmap>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_conn_clicked();
    void conn_succ_env();
    void conn_succ_pic();
    void conn_fail_env();
    void conn_fail_pic();

    void on_pushButton_openled_clicked();
    void on_pushButton_closeled_clicked();

    void on_pushButton_img_clicked();
    void recv_pic_data();

    void on_pushButton_env_clicked();
    void recv_env_data();

    void show_fps();
    void on_pushButton_openbeep_clicked();

    void on_pushButton_closebeep_clicked();

    void on_pushButton_openfan_clicked();

    void on_pushButton_closefan_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *client_fd;
    QTcpSocket *pic_socket_fd;
    QTimer *timer_env;
    QTimer *timer_img;
    QTimer *timer_fps;
    char PicBuffer[1024*1024];
    int fps_count=0;
    int temp;
    int humi;
    int light;
};

#endif // MAINWINDOW_H
