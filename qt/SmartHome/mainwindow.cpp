#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client_fd=NULL;
    pic_socket_fd=NULL;
    timer_env=NULL;
    timer_img=NULL;
    timer_fps=NULL;


}
void MainWindow::on_pushButton_conn_clicked()
{
    QString ip=ui->lineEdit_ip->text();
    int port=ui->lineEdit_port->text().toInt();
    if(client_fd==NULL)
    {
        client_fd=new QTcpSocket();
        client_fd->connectToHost(ip,port);

    }
    connect(client_fd,SIGNAL(connected()),this,SLOT(conn_succ_env()));
    connect(client_fd,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(conn_fail_env()));
    if(pic_socket_fd==NULL)
    {
        pic_socket_fd=new QTcpSocket();
        pic_socket_fd->connectToHost(ip,port);

    }
    connect(pic_socket_fd,SIGNAL(connected()),this,SLOT(conn_succ_pic()));
    connect(pic_socket_fd,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(conn_fail_pic()));
}
void MainWindow::conn_succ_env()
{
    qDebug()<<"connect success";
    ui->label_tips->setText("connect success");
    if(timer_env==NULL)
    {
        timer_env=new QTimer();
        timer_env->start(500);
        connect(timer_env,SIGNAL(timeout()),this,SLOT(on_pushButton_env_clicked()));
    }
}
void MainWindow::conn_succ_pic()
{
    qDebug()<<"connect pic socket success";
    ui->label_tips->setText("connect pic socket success");
    //计算fps
    if(timer_fps==NULL)
    {
        timer_fps=new QTimer();
        timer_fps->start(1000);
        connect(timer_fps,SIGNAL(timeout()),this,SLOT(show_fps()));
    }

    if(timer_img==NULL)
    {
        timer_img=new QTimer();
        timer_img->start(35);//25
        connect(timer_img,SIGNAL(timeout()),this,SLOT(on_pushButton_img_clicked()));
    }
}
void MainWindow::conn_fail_env()
{
    qDebug()<<"conn env fail";
    client_fd=NULL;
}
void MainWindow::conn_fail_pic()
{
     qDebug()<<"conn pic fail";
     pic_socket_fd=NULL;

}
void MainWindow::on_pushButton_openled_clicked()
{
    if(client_fd!=NULL)
    {
        char req_buffer[10]="LED_ON";
        ui->label_tips->setText("LED_ON");
        client_fd->write(req_buffer,sizeof(req_buffer));
        client_fd->flush();
    }
    else if(client_fd==NULL)
    {
        ui->label_tips->setText("please connected first");
    }
}

void MainWindow::on_pushButton_closeled_clicked()
{
    if(client_fd!=NULL)
    {
        char req_buffer[10]="LED_OFF";
        ui->label_tips->setText("LED_OFF");
        client_fd->write(req_buffer,sizeof(req_buffer));
        client_fd->flush();
    }
    else if(client_fd==NULL)
    {
        ui->label_tips->setText("please connected first");
    }
}


void MainWindow::on_pushButton_env_clicked()
{
    char req_buffer[10]="ENV_SEND";
    ui->label_tips->setText("ENV_SEND");
    client_fd->write(req_buffer,sizeof(req_buffer));
    client_fd->flush();
    connect(client_fd,SIGNAL(readyRead()),this,SLOT(recv_env_data()));
}
void MainWindow::recv_env_data()
{
    int ret=0;
    char recv_buffer[1024]={0};
    ret=client_fd->read(recv_buffer,sizeof(recv_buffer));
    if(ret<0)
    {
        qDebug()<<"read socket fail";
        ui->label_tips->setText("read socket fail");
        client_fd->close();
        timer_env->stop();
    }
    sscanf(recv_buffer,"%dt%dh%dl",&temp,&humi,&light);
    char temp_buffer[5]={0};
    sprintf(temp_buffer,"%d",temp);
    ui->label_tem->setText(temp_buffer);
    sprintf(temp_buffer,"%d",humi);
    ui->label_hum->setText(temp_buffer);
    sprintf(temp_buffer,"%d",light);
    ui->label_light->setText(temp_buffer);
}

void MainWindow::on_pushButton_img_clicked()
{
    if(pic_socket_fd!=NULL)
    {
        char req_buffer[10]="PIC_SEND";
        ui->label_tips->setText("PIC_SEND");
        pic_socket_fd->write(req_buffer,sizeof(req_buffer));
        pic_socket_fd->flush();
    }
    else if(pic_socket_fd==NULL)
    {
        ui->label_tips->setText("please connected first");
    }
    connect(pic_socket_fd,SIGNAL(readyRead()),this,SLOT(recv_pic_data()));
}
void MainWindow::recv_pic_data()
{
    int ret=0;
    char pic_size_buffer[40]={0};
    ret=pic_socket_fd->read(pic_size_buffer,sizeof(pic_size_buffer));
    if(ret==0)
    {
        return;
    }
    else if(ret<0)
    {
        qDebug()<<"read pic_size_buffer fail";
        ui->label_tips->setText("read pic_size_buffer fail");
        pic_socket_fd->close();
    }
    int pic_size=atoi(pic_size_buffer);
    qDebug()<<"pic size="<<pic_size;
    int count=0;
    memset(PicBuffer,0,sizeof(PicBuffer));
    while(count != pic_size)
    {
        ret=pic_socket_fd->read(PicBuffer+count,pic_size-count);
        if(ret<0)
        {
            qDebug()<<"read pic fail";
            ui->label_tips->setText("read pic fail");
            pic_socket_fd->close();
        }
        else if(ret==0)
        {
            pic_socket_fd->waitForReadyRead(30000);
        }
        else
        {
            count +=ret;
        }
    }
    qDebug()<<"recv success";
    QPixmap pixmap;
    pixmap.loadFromData((uchar *)PicBuffer,pic_size,"jpeg");
    //ui->label_img->setPixmap(pixmap);
    ui->label_img->setPixmap(pixmap.scaled(ui->label_img->size()));

    fps_count++;
}

void MainWindow::show_fps()
{
    qDebug()<<"fps="<<fps_count;
    ui->label_fps->setText(QString::number(fps_count));
    fps_count=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_openbeep_clicked()
{
    if(client_fd!=NULL)
    {
        char req_buffer[10]="BEEP_ON";
        ui->label_tips->setText("BEEP_ON");
        client_fd->write(req_buffer,sizeof(req_buffer));
        client_fd->flush();
    }
    else if(client_fd==NULL)
    {
        ui->label_tips->setText("please connected first");
    }
}

void MainWindow::on_pushButton_closebeep_clicked()
{
     if(client_fd!=NULL)
     {
         char req_buffer[10]="BEEP_OFF";
         ui->label_tips->setText("BEEP_OFF");
         client_fd->write(req_buffer,sizeof(req_buffer));
         client_fd->flush();
     }
     else if(client_fd==NULL)
     {
         ui->label_tips->setText("please connected first or restart");
     }
}

void MainWindow::on_pushButton_openfan_clicked()
{
    if(client_fd!=NULL)
    {
        char req_buffer[10]="FAN_ON";
        ui->label_tips->setText("FAN_ON");
        client_fd->write(req_buffer,sizeof(req_buffer));
        client_fd->flush();
    }
    else if(client_fd==NULL)
    {
        ui->label_tips->setText("please connected first");
    }
}

void MainWindow::on_pushButton_closefan_clicked()
{
     if(client_fd!=NULL)
     {
         char req_buffer[10]="FAN_OFF";
         ui->label_tips->setText("FAN_OFF");
         client_fd->write(req_buffer,sizeof(req_buffer));
         client_fd->flush();
     }
     else if(client_fd==NULL)
     {
         ui->label_tips->setText("please connected first or restart");
     }
}
