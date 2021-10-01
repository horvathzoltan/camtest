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
    void setLabelExp(int i);

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
    void on_f_moved(const QPoint &p);
    bool SetCamF();

    void setUi_TestSyncR(const Camtest::TestSyncR &m);
    void setUi_StopRecSyncR(const Camtest::StopRecSyncR &m);
    void setUi_TestRallR(const Camtest::TestRallR &m);
    void setUi_SetCamAuto(const Camtest::SetCamAutoR &m);

    struct DrawData{
        QRectF f;
    };

    DrawData drawdata;

    void Draw();
private slots:
    void on_clicked(QPoint p);
    void on_moved(QPoint p);

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

    void on_pushButton_test1_clicked();

    void on_pushButton_test2_clicked();

    void on_pushButton_test3_clicked();

    void on_pushButton_p_exp_clicked();

    void on_pushButton_m_exp_clicked();

    void on_pushButton_pic1_clicked();

    void on_pushButton_setIso400_clicked();

    void on_pushButton_autoset_clicked();

    void on_pushButton_setcamauto_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    LabelEventFilter* _eventFilter;
    DStatus* _d_status=nullptr;
    QPixmap pixmap;
    QPixmap* _pixmap_f=nullptr;
    QPixmap* _pixmap2=nullptr;

    bool _camera_active;
    void setUi_StartR(const Camtest::StartR&);
    void setUi_StopR(const Camtest::StopR&);
    void setUi_UploadR(const Camtest::UploadR &m);
    void setUi_UpdateR(const Camtest::UpdateR &m);
    void setUi_RestartR(const Camtest::RestartR &m);

    void setUi_StartRec(const Camtest::StartRecR &m);
    void setUi_StopRec(const Camtest::StopRecR &m);
    void setUi_StartRecSyncR(const Camtest::StartRecSyncR &m);

};
#endif // MAINWINDOW_H
