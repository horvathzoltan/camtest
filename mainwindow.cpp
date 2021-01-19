#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camtest.h"
#include "common/macrofactory/macro.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_radioButton_start_clicked()
{
    setUi(Camtest::Start());
}

void MainWindow::on_radioButton_stop_clicked()
{    
    setUi(Camtest::Stop());
}

void MainWindow::setUi(const Camtest::StartR& m){
    ui->label_txt->setText(zfn());
}

void MainWindow::setUi(const Camtest::StopR& m){
    ui->label_txt->setText(zfn());
}
