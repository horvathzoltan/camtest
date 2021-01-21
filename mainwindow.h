#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "camtest.h"
#include "common/helper/downloader/downloader.h"

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
    void on_timer_timeout();
private:
    Ui::MainWindow *ui;
    QTimer *timer;
    com::helper::Downloader* _d;
    void setUi(const Camtest::StartR&);
    void setUi(const Camtest::StopR&);
};
#endif // MAINWINDOW_H
