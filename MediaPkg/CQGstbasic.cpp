#include "CQGstbasic.h"
#include <gst/interfaces/xoverlay.h>
#include "MediaPkgCommon.h"
#include<QTimer>
#include<QWidget>
#include<QPoint>
#include<QDebug>
CQGstBasic::CQGstBasic(int _winID,QWidget* win ,QObject* parent):
    QObject(parent),winID(_winID)
{

    qos_cnt = 0;
    barrier = NULL;
    if(win != 0 )
    {
        QRect rect = win->geometry();
        QPoint point = win->mapToGlobal(QPoint(0,0));
        barrier = new QWidget;
        Qt::WindowFlags flags = barrier->windowFlags()|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint;
        flags &= (~Qt::WindowCloseButtonHint);
        barrier->setWindowFlags(flags);

        barrier->setGeometry(point.x(),point.y(),rect.width(),rect.height());
        //barrier->show();
    }
    //初始化CQMedia各个变量
//    this->winID=winID;
//    this->winID_null = winID_null;

//    this->winID=(winID>0)? winID:0;
    //初始化gstreamer
    if(gst_init_check(NULL,NULL,NULL)==false)
    {
        return;
    }
}
void CQGstBasic::printDot(const gchar* fileName)
{
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline),GST_DEBUG_GRAPH_SHOW_ALL,fileName);
}
CQGstBasic::~CQGstBasic()
{
    if(this->getPlayingState()!=STOPPED)
    {
        this->stop();
    }
}



bool CQGstBasic::paused()
{
    GstStateChangeReturn ret;
    //判断播放是否已开始
    if(getPlayingState()==PAUSED)
    {

        return true;
    }
    ret=gst_element_set_state(this->pipeline,GST_STATE_PAUSED);

    if(GST_STATE_CHANGE_FAILURE!=ret)
    {
        this->state=PAUSED;
        qDebug("PAUSED");
        return true;
    }
    return false;
}
void CQGstBasic::_onOverlay()
{
    //判断是否有窗口用以播放
//    if(this->winID<=0)
//    {
//        return ;
//    }
//    GstElement *videosink = gst_bin_get_by_name((GstBin*)pipeline,"videosink");
//    GstStateChangeReturn ret=gst_element_set_state(videosink,GST_STATE_READY);
//    if(ret == GST_STATE_CHANGE_SUCCESS)
//    {
//        qDebug("gst_element_set_state(videosink,GST_STATE_PAUSED )successed");
//    }
//    else
//    {
//        qDebug("gst_element_set_state(videosink,GST_STATE_PAUSED )failed");


//    }
//    gst_x_overlay_expose(GST_X_OVERLAY(videosink));
//    if(GST_IS_X_OVERLAY(videosink))
//    {
//        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(videosink),this->winID);
        //gst_x_overlay_set_render_rectangle(GST_X_OVERLAY(videosink),500,500,500,500);

//    }
    if(barrier!=NULL)
        barrier->hide();

}
void CQGstBasic::_offOverlay()
{
    //判断是否有窗口用以播放
    if(this->winID<=0)
    {
        return ;
    }
    GstElement *videosink = gst_bin_get_by_name((GstBin*)pipeline,"v_sink");
    if(GST_IS_X_OVERLAY(videosink))
    {
        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(videosink),this->winID);

    }
    if(barrier!=NULL)
        barrier->show();
}
bool CQGstBasic::play()
{
    if(getPlayingState()==PLAYING)
        return true;
#ifdef ARM
    if(getPlayingState()==STOPPED)
    {
        _updateDecoder();
        _updateDemux();
    }
#endif

    GstStateChangeReturn ret=gst_element_set_state(this->pipeline,GST_STATE_PLAYING);
    if(GST_STATE_CHANGE_FAILURE!=ret)
    {
        this->state=PLAYING;
        qDebug("Playing");
        _offOverlay( );
        QTimer::singleShot(3000,this,SLOT(_onOverlay()));
        return true;
    }
    qDebug("Playing error");
    return false;
}

bool CQGstBasic::play(const QString& _url)
{

    if(getPlayingState()==STOPPED)
    {
        if(!loadURL(_url))
            return false;
    }
    return play();
}
bool CQGstBasic::stop()
{
    GstStateChangeReturn ret;
    if (STOPPED == getPlayingState()) {
      return false;
    }
    ret = gst_element_set_state(this->pipeline, GST_STATE_NULL);
    if (GST_STATE_CHANGE_FAILURE != ret) {
        this->state = STOPPED;
        emitError(STOP);
        return true;
    }
    else
        return false;
}


