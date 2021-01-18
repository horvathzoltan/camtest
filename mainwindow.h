#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camtest.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_radioButton_start_clicked();

    void on_radioButton_stop_clicked();

private:
    Ui::MainWindow *ui;

    void setUi(const Camtest::StartR&);
    void setUi(const Camtest::StopR&);
};
#endif // MAINWINDOW_H
