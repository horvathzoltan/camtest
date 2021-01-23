#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camtest.h"
#include "common/macrofactory/macro.h"
//#include "common/helper/downloader/downloader.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_timer_timeout);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}


void MainWindow::on_radioButton_start_clicked()
{
    setUi(Camtest::Start());    
}

void MainWindow::on_radioButton_stop_clicked()
{        
    auto m = Camtest::Stop();
    setUi(m);
}

void MainWindow::on_timer_timeout()
{
    auto p = Camtest::GetPixmap();
    if(!p.isNull())
    {        
        ui->label_pic->setPixmap(p);
    }
    else
    {
        ui->label_pic->setText("upsz");
    }
}

void MainWindow::setUi(const Camtest::StartR& m){
    _camera_active = m.isActive;
    ui->label_txt->setText(m.msg);
    ui->label_serial->setText(m.serial);
    if(_camera_active)
    {
        if(Camtest::OpenCamera()) timer->start(16);
    }
}

void MainWindow::setUi(const Camtest::StopR& m){        
    timer->stop();
    //ui->label_txt->setText("");
    //ui->label_serial->setText("");
    if(_camera_active)
    {
        Camtest::CloseCamera();
    }
}



void MainWindow::on_pushButton_upload_clicked()
{
    if(!_camera_active)
    {
        ui->label_pic->setText("No active camera to upload.");
    }
    if(_camera_active && !timer->isActive())
    {
        setUi(Camtest::Upload("kurutty.txt"));
    }
}

void MainWindow::setUi(const Camtest::UploadR& m){
    ui->label_pic->setText(m.err);
}
