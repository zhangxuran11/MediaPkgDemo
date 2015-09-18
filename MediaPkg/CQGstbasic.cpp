#include "CQGstbasic.h"
#include <gst/interfaces/xoverlay.h>
#include "MediaPkgCommon.h"
#include<QTimer>
CQGstBasic::CQGstBasic(int _winID,int _winID_null,QObject* parent):
    QObject(parent),winID(_winID),winID_null(_winID_null)
{
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
    if(this->winID<=0)
    {
        return ;
    }
    GstElement *videosink = gst_bin_get_by_name((GstBin*)pipeline,"videosink");
    if(GST_IS_X_OVERLAY(videosink))
    {
        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(videosink),this->winID);

    }
}
void CQGstBasic::_offOverlay()
{
    //判断是否有窗口用以播放
    if(this->winID_null<=0)
    {
        return ;
    }
    GstElement *videosink = gst_bin_get_by_name((GstBin*)pipeline,"videosink");
    if(GST_IS_X_OVERLAY(videosink))
    {
        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(videosink),this->winID_null);

    }
}
bool CQGstBasic::play()
{
    if(getPlayingState()==PLAYING)
        return true;

#ifdef ARM
    if(getPlayingState()==STOPPED)
    {
        _updateDecoder();
    }
#endif
    _offOverlay( );
    QTimer::singleShot(3000,this,SLOT(_onOverlay()));
    GstStateChangeReturn ret=gst_element_set_state(this->pipeline,GST_STATE_PLAYING);

    if(GST_STATE_CHANGE_FAILURE!=ret)
    {
        this->state=PLAYING;
        qDebug("Playing");
        return true;
    }
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
        return true;
    }
    else
        return false;
}


