#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QPixmap>
#include "camtest.h"
#include "common/helper/downloader/downloader.h"
#include "labeleventfilter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setLabelB(int i);
    void setLabelC(int i);
    void setLabelS(int i);
    void setLabelISO(int i);
    void setLabelWB(int i);

    void on_fc_clicked(const QPoint &p);

    struct DStatus
    {
        int round=0;
        QPoint p0;
        QPoint p1;
        qreal w;
    };
    int ball_id = 1;

    void on_d_clicked(const QPoint &p);
    bool SetCamD();
    void on_f_clicked(const QPoint &p);
    bool SetCamF();

    void setUi_TestSyncR(const Camtest::TestSyncR &m);
    void setUi_StopRecSyncR(const Camtest::StopRecSyncR &m);
private slots:
    void on_clicked(QPoint p);

    void on_radioButton_start_clicked();

    void on_radioButton_stop_clicked();
    void on_timerTimeout();
    void on_pushButton_upload_clicked();

    void on_pushButton_bp_clicked();

    void on_pushButton_bm_clicked();

    void on_pushButton_cp_clicked();

    void on_pushButton_cm_clicked();

    void on_pushButton_sm_clicked();

    void on_pushButton_sp_clicked();

    void on_pushButton_isop_clicked();

    void on_pushButton_isom_clicked();

    void on_pushButton_wbp_clicked();

    void on_pushButton_wbm_clicked();

    void on_pushButton_update_clicked();

    void on_pushButton_restart_clicked();

    void on_pushButton_rec_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_sync_rec_clicked();

    void on_pushButton_clicked();

    void on_pushButton_sync_stop_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    LabelEventFilter* _eventFilter;
    DStatus* _d_status=nullptr;
    bool _camera_active;
    void setUi(const Camtest::StartR&);
    void setUi(const Camtest::StopR&);
    void setUi(const Camtest::UploadR &m);
    void setUi(const Camtest::UpdateR &m);
    void setUi(const Camtest::RestartR &m);

    void setUi_StartRec(const Camtest::StartRecR &m);
    void setUi_StopRec(const Camtest::StopRecR &m);
    void setUi_StartRecSyncR(const Camtest::StartRecSyncR &m);

};
#endif // MAINWINDOW_H
