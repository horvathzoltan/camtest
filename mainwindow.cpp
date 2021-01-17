#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camtest.h"

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
    Camtest::Start();
}

void MainWindow::on_radioButton_stop_clicked()
{
    Camtest::Stop();
}


void setUI(){}
