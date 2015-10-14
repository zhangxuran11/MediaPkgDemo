#include "CQplayerGUI.h"
#include "ui_CQplayerGUI.h"
#include<QTimer>

CQplayerGUI::CQplayerGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CQplayerGUI)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|(windowFlags() & (~Qt::WindowCloseButtonHint)));
    cqm = new CQMedia(ui->frameMovie->winId(),ui->frameMovie);
    cqi = new CQipc(ui->frameMovie->winId());
    QTimer::singleShot(2000,this,SLOT(on_playMediaButton_clicked()));
}


void CQplayerGUI::on_playMediaButton_clicked()
{
    cqm->play("http://192.168.1.123:8090/test2.sdp");
}

void CQplayerGUI::on_playIPCButton_clicked()
{
    cqi->play("rtsp://admin:wyx123456@192.168.1.64:554/h264/ch1/main/av_stream");
}

void CQplayerGUI::on_stopButton_clicked()
{
    cqm->stop();
    cqi->stop();
}

void CQplayerGUI::on_setOverButton_clicked()
{
    //cqm->setOver(ui->frameMovie->winId());
}
