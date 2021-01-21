#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camtest.h"
#include "common/macrofactory/macro.h"
#include "common/helper/downloader/downloader.h"
#include <QTimer>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_timer_timeout);
    _d = new com::helper::Downloader(QStringLiteral("http://172.16.3.235:1997"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}


void MainWindow::on_radioButton_start_clicked()
{
    setUi(Camtest::Start());
    QByteArray b = _d->download("open", "");
    timer->start(16);
}

void MainWindow::on_radioButton_stop_clicked()
{        
    setUi(Camtest::Stop());
    QByteArray b = _d->download("close", "");

}

void MainWindow::on_timer_timeout()
{
    QByteArray b = _d->download("get", "format=jpg20&mode=0");
    if(b.length()>100)
    {
        QPixmap p;
        p.loadFromData(b,"JPG");
        ui->label_pic->setPixmap(p);
    }
    else
    {
        ui->label_pic->setText("upsz");
    }
}

void MainWindow::setUi(const Camtest::StartR& m){
    ui->label_txt->setText(m.msg);
    timer->start(16);
}

void MainWindow::setUi(const Camtest::StopR& m){
    timer->stop();
    ui->label_txt->setText(nameof_fn_full());
}


