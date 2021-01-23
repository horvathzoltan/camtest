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
    void on_pushButton_upload_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    bool _camera_active;
    void setUi(const Camtest::StartR&);
    void setUi(const Camtest::StopR&);
    void setUi(const Camtest::UploadR &m);
};
#endif // MAINWINDOW_H
