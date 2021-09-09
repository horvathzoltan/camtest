#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camtest.h"
#include "common/macrofactory/macro.h"
#include "common/logger/log.h"
//#include "common/helper/downloader/downloader.h"
#include <QTimer>
#include <QPixmap>
#include <QtMath>
#include <Qt>
#include "raspicamtypes.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_timerTimeout);

    ui->label_pic->installEventFilter( this );
    _eventFilter = new LabelEventFilter();//, clicked);
    connect(_eventFilter, &LabelEventFilter::clicked, this, &MainWindow::on_clicked);
    ui->label_pic->installEventFilter(_eventFilter);
}

void MainWindow::on_clicked(QPoint p)
{
    if(ui->radioButton_fc->isChecked()) on_fc_clicked(p);
    else if(ui->radioButton_d->isChecked()) on_d_clicked(p);
    else if(ui->radioButton_f->isChecked()) on_f_clicked(p);
}

void MainWindow::on_fc_clicked(const QPoint &p)
{
        QPixmap pix = ui->label_pic->pixmap(Qt::ReturnByValueConstant::ReturnByValue);
    //if(!pix) return;
    auto img = pix.toImage();
    auto c = img.pixelColor(p);
    auto txt = c.name();
    ui->label_rgb->setText(txt);
}



void MainWindow::on_d_clicked(const QPoint& p)
{
    if(!_d_status) _d_status = new DStatus();

    switch(_d_status->round)
    {
    case 0:
    {
        auto pix = ui->label_pic->pixmap(Qt::ReturnByValueConstant::ReturnByValue);
        //auto pix = ui->label_pic->pixmap();

        _d_status->round=1;
        _d_status->p0=p;
        _d_status->w = pix.width();
        break;
    }
    case 1:
    {
        _d_status->round=2;
        _d_status->p1=p;

        if(SetCamD())
        {
            delete _d_status;
            _d_status=nullptr;
        }
        break;
    }
    default: delete _d_status;_d_status=nullptr; break;
    }
}

void MainWindow::on_f_clicked(const QPoint& p)
{
    if(!_d_status) _d_status = new DStatus();

    switch(_d_status->round)
    {
    case 0:
    {
        //auto pix = ui->label_pic->pixmap();
        auto pix = ui->label_pic->pixmap(Qt::ReturnByValueConstant::ReturnByValue);

        _d_status->round=1;
        _d_status->p0=p;
        _d_status->w = pix.width();
        break;
    }
    case 1:
    {
        _d_status->round=2;
        _d_status->p1=p;

        if(SetCamF())
        {
            delete _d_status;
            _d_status=nullptr;
        }
        break;
    }
    default: delete _d_status;_d_status=nullptr; break;
    }
}

bool MainWindow::SetCamD()
{
    if(!_d_status) return false;
    if(_d_status->round!=2) return false;

    auto a = _d_status->p1.x()-_d_status->p0.x();
    auto b = _d_status->p1.y()-_d_status->p0.y();

    auto d = hypot(a, b);
    auto dd = d*0.2;

    auto dmin = (d-dd)/_d_status->w;
    auto dmax = (d+dd)/_d_status->w;

    auto txt = QStringLiteral("d=%1±%2").arg(d,0,'f',4).arg(dd,0,'f',4);
    txt += QStringLiteral(" (%1-%2)").arg(dmin,0,'f',4).arg(dmax,0,'f',4);

    auto isok = Camtest::SetCalD(ball_id, dmin, dmax);
    txt+=isok?" ok":" error";
    ui->label_rgb->setText(txt);
    return isok;
}

bool MainWindow::SetCamF()
{
    if(!_d_status) return false;
    if(_d_status->round!=2) return false;
    if(_d_status->p0.x()==_d_status->p1.x()) return false;
    if(_d_status->p0.y()==_d_status->p1.y()) return false;

    qreal x0, y0, x1, y1;

    if(_d_status->p0.x()<_d_status->p1.x())
    {
        x0=_d_status->p0.x();
        x1=_d_status->p1.x();
    }
    else
    {
        x1=_d_status->p0.x();
        x0=_d_status->p1.x();
    }

    if(_d_status->p0.y()<_d_status->p1.y())
    {
        y0=_d_status->p0.y();
        y1=_d_status->p1.y();
    }
    else
    {
        y1=_d_status->p0.y();
        y0=_d_status->p1.y();
    }


    auto txt = QStringLiteral("f=%1,%2").arg(x0).arg(y0);
    txt += QStringLiteral("-%1,%2").arg(x1).arg(y1);

    auto isok = Camtest::SetCalF(ball_id,
                                 x0/_d_status->w, y0/_d_status->w,
                                 x1/_d_status->w, y1/_d_status->w);
    txt+=isok?" ok":" error";
    ui->label_rgb->setText(txt);
    return isok;
}

MainWindow::~MainWindow()
{
    delete _eventFilter;
    delete ui;
    delete timer;
    delete _d_status;
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

void MainWindow::on_timerTimeout()
{
    auto p = Camtest::GetPixmap(ui->checkBox_mvis->isChecked());
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
        auto status = Camtest::GetCamStatus();
        if(!status.isOpened) status.isOpened = Camtest::OpenCamera();
        if(status.isOpened)
        {
            Camtest::ClearCamSettings(ball_id);
            timer->start(10);
        }
        else ui->radioButton_stop->setChecked(true);

    }
    setLabelB(m._settings.brightness);
    setLabelC(m._settings.contrast);
    setLabelS(m._settings.saturation);
    setLabelISO(m._settings.iso);
    setLabelWB(m._settings.wb);
}

void MainWindow::setUi(const Camtest::StopR& m){
    Q_UNUSED(m)

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

void MainWindow::setLabelB(int i){ ui->label_b->setText(QString::number(i));}
void MainWindow::setLabelC(int i){ ui->label_c->setText(QString::number(i));}
void MainWindow::setLabelS(int i){ ui->label_s->setText(QString::number(i));}
void MainWindow::setLabelISO(int i){ ui->label_iso->setText(QString::number(i));}
void MainWindow::setLabelWB(int i){ui->label_wb->setText(WbToString(i));

}



void MainWindow::on_pushButton_bp_clicked()
{
    setLabelB(Camtest::brightnest_p());
}

void MainWindow::on_pushButton_bm_clicked()
{
    setLabelB(Camtest::brightnest_m());
}

void MainWindow::on_pushButton_cp_clicked()
{
    setLabelC(Camtest::contrast_p());
}

void MainWindow::on_pushButton_cm_clicked()
{
    setLabelC(Camtest::contrast_m());
}

void MainWindow::on_pushButton_sm_clicked()
{
    setLabelS(Camtest::saturation_p());
}

void MainWindow::on_pushButton_sp_clicked()
{
    setLabelS(Camtest::saturation_m());
}

void MainWindow::on_pushButton_isop_clicked()
{
    setLabelISO(Camtest::iso_p());
}

void MainWindow::on_pushButton_isom_clicked()
{
    setLabelISO(Camtest::iso_m());
}

void MainWindow::on_pushButton_wbp_clicked()
{
    setLabelWB(Camtest::wb_p());
}

void MainWindow::on_pushButton_wbm_clicked()
{
    setLabelWB(Camtest::wb_m());
}

void MainWindow::on_pushButton_update_clicked()
{
    setUi(Camtest::Update());
}

void MainWindow::setUi(const Camtest::UpdateR& m){

    ui->label_msg->setText((m.isOk?"ok\n":"error\n")+m.msg);
}

//pushButton_shutdown

void MainWindow::on_pushButton_restart_clicked()
{
    ui->label_msg->setText(QStringLiteral("Waiting for restart..."));
    setUi(Camtest::Restart());
}

void MainWindow::setUi(const Camtest::RestartR& m){

    ui->label_msg->setText(QStringLiteral("Restart ")+(m.isOk?"ok\n":"error\n")+m.msg);
    ui->radioButton_stop->setChecked(true);
}

void MainWindow::setUi_StartRec(const Camtest::StartRecR &m)
{
    ui->label_msg->setText(m.msg);
}

void MainWindow::setUi_StopRec(const Camtest::StopRecR &m)
{
    ui->label_msg->setText(m.msg);
}

void MainWindow::setUi_StartRecSyncR(const Camtest::StartRecSyncR &m)
{
    ui->label_msg->setText(m.msg);
}

void MainWindow::setUi_StopRecSyncR(const Camtest::StopRecSyncR &m)
{
    ui->label_msg->setText(m.msg);
}

void MainWindow::setUi_TestRallR(const Camtest::TestRallR &m)
{
    QString e = "response: "+m.msg+"\nerr: "+m.err;
    ui->label_msg->setText(e);
}

void MainWindow::setUi_TestSyncR(const Camtest::TestSyncR &m)
{
    ui->label_msg->setText(m.msg);
}


void MainWindow::on_pushButton_rec_clicked()
{
    auto m = Camtest::StartRecording();
    setUi_StartRec(m);
}


void MainWindow::on_pushButton_stop_clicked()
{
    auto m = Camtest::StopRecording();
    setUi_StopRec(m);
}


void MainWindow::on_pushButton_sync_rec_clicked()
{
    auto m = Camtest::StartRecSync();
    setUi_StartRecSyncR(m);
}


void MainWindow::on_pushButton_clicked()
{
    auto m = Camtest::TestSync();
    setUi_TestSyncR(m);
}


void MainWindow::on_pushButton_sync_stop_clicked()
{
    auto m = Camtest::StopRecSync();
    setUi_StopRecSyncR(m);
}


void MainWindow::on_pushButton_test1_clicked()
{
    QString path = R"(set_cam_settings)";
    QString q = R"(brightness=43&contrast=51&saturation=51&gain=49&awb=1)";
    auto m = Camtest::TestCall(path, q);
    setUi_TestRallR(m);
}

